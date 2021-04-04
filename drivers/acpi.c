#include "../drivers/screen.h"
#include "../kernel/panic.h"
#include "../cpu/ports.h"
#include "../cpu/timer.h"
#include "../libc/mem.h"
#include <stddef.h>
#include "acpi.h"

dword *SMI_CMD;
byte ACPI_ENABLE;
byte ACPI_DISABLE;
dword *PM1a_CNT;
dword *PM1b_CNT;
word SLP_TYPa;
word SLP_TYPb;
word SLP_EN;
word SCI_EN;
byte PM1_CNT_LEN;

struct RSDPtr {
   byte Signature[8];
   byte CheckSum;
   byte OemID[6];
   byte Revision;
   dword *RsdtAddress;
};

struct FACP {
   byte Signature[4];
   dword Length;
   byte unneded1[40 - 8];
   dword *DSDT;
   byte unneded2[48 - 44];
   dword *SMI_CMD;
   byte ACPI_ENABLE;
   byte ACPI_DISABLE;
   byte unneded3[64 - 54];
   dword *PM1a_CNT_BLK;
   dword *PM1b_CNT_BLK;
   byte unneded4[89 - 72];
   byte PM1_CNT_LEN;
};

// Check if the given address has a valid header
uint32_t *acpi_check_rsdp_tr(uint32_t *ptr) {
   char *sig = "RSD PTR ";
   struct RSDPtr *rsdp = (struct RSDPtr *) ptr;
   byte *bptr;
   byte check = 0;
   uint32_t i;

   if (memcmp(sig, rsdp, 8) == 0) {
      // Check checksum RSDPD
      bptr = (byte *) ptr;
      for (i = 0; i < sizeof(struct RSDPtr); i++) {
         check += *bptr;
         bptr++;
      }

      // Found valid RSDPD
      if (check == 0) {
         return (uint32_t *) rsdp->RsdtAddress;
      }
   }

   return NULL;
}

// Finds the ACPI header and returns the address of the RSDT
uint32_t *acpi_get_rsdp_tr() {
   uint32_t *addr;
   uint32_t *rsdp;

   // Search below the 1mb mark for RSDP signature
   for (addr = (uint32_t *) 0x000E0000; (int) addr < 0x00100000; addr += 0x10 / sizeof(addr)) {
      rsdp = acpi_check_rsdp_tr(addr);
      if (rsdp != NULL) {
         return rsdp;
      }
   }

   // At address 0x40:0x0E is the RM segment of the EBDA
   int ebda = *((short *) 0x40E); // Get pointer
   ebda = ebda * 0x10 &0x000FFFFF; // Transform segment into linear address

   // Search Extended BIOS Data Area for the Root System Description Pointer signature
   for (addr = (uint32_t *) ebda; (int) addr < ebda + 1024; addr += 0x10 / sizeof(addr)) {
      rsdp = acpi_check_rsdp_tr(addr);
      if (rsdp != NULL) {
         return rsdp;
      }
   }

   return NULL;
}

// Checks for a given header and validates checksum
int acpi_check_header(uint32_t *ptr, char *sig) {
   if (memcmp(ptr, sig, 4) == 0) {
      char *checkPtr = (char *) ptr;
      int len = *(ptr + 1);
      char check = 0;
      while (0 < len--) {
         check += *checkPtr;
         checkPtr++;
      }
      if (check == 0) {
         return 0;
      }
   }
   return -1;
}

int acpi_enable() {
   // Check if ACPI is enabled
   if ((port_word_in((uint32_t) PM1a_CNT) &SCI_EN) == 0) {
      // Check if ACPI can be enabled
      if (SMI_CMD != 0 && ACPI_ENABLE != 0) {
         port_byte_out((uint32_t) SMI_CMD, ACPI_ENABLE); // Send ACPI enable command
         // Give 3 seconds time to enable ACPI
         int i;
         for (i = 0; i < 300; i++) {
            if ((port_word_in((uint32_t) PM1a_CNT) &SCI_EN) == 1) {
               break;
            }
            sleep(10);
         }
         if (PM1b_CNT != 0) {
            for (; i < 300; i++) {
               if ((port_word_in((uint32_t) PM1b_CNT) &SCI_EN) == 1) {
                  break;
               }
               sleep(10);
            }
         }
         if (i < 300) {
            kprint_gok();
            kprint("Enabling ACPI.\n");
            return 0;
         } else {
            kprint_rfail();
            kprint("Couldn't Enable ACPI.\n");
            return -1;
         }
      } else {
         kprint_rfail();
         kprint("No Known Way to Enable ACPI.\n");
         return -1;
      }
   } else {
      kprint_gok();
      kprint("ACPI is Already Enabled.\n");
      return 0;
   }
}

//
// Bytecode of the \_S5 object
// -----------------------------------------
//        | (optional) |    |    |    |   
// NameOP | \          | _  | S  | 5  | _
// 08     | 5A         | 5F | 53 | 35 | 5F
//
// -----------------------------------------------------------------------------------------------------------
//           |           |              | ( SLP_TYPa   ) | ( SLP_TYPb   ) | ( Reserved   ) | (Reserved    )
// PackageOP | PkgLength | NumElements  | byteprefix Num | byteprefix Num | byteprefix Num | byteprefix Num
// 12        | 0A        | 04           | 0A         05  | 0A          05 | 0A         05  | 0A         05
//
//----this-structure-was-also-seen----------------------
// PackageOP | PkgLength | NumElements |
// 12        | 06        | 04          | 00 00 00 00
//
// (Pkglength bit 6-7 encode additional PkgLength bytes [shouldn't be the case here])
//
int acpi_init() {
   uint32_t *ptr = acpi_get_rsdp_tr();

   // Check if address is correct (if ACPI is available on this PC)
   if (ptr != NULL && acpi_check_header(ptr, "RSDT") == 0) {
      // The RSDT contains an unknown number of pointers to ACPI tables
      int entrys = *(ptr + 1);
      entrys = (entrys - 36) / 4;
      ptr += 36 / 4; // Skip header information

      while (0 < entrys--) {
         // Check if the desired table is reached
         if (acpi_check_header((uint32_t *) *ptr, "FACP") == 0) {
            entrys = -2;
            struct FACP *facp = (struct FACP *) *ptr;
            if (acpi_check_header((uint32_t *) facp->DSDT, "DSDT") == 0) {
               // Search the \_S5 package in the DSDT
               char *S5Addr = (char *) facp->DSDT + 36; // Skip header
               int dsdtLength = *(facp->DSDT+1) - 36;
               while (0 < dsdtLength--) {
                  if (memcmp(S5Addr, "_S5_", 4) == 0) {
                     break;
                  }
                  S5Addr++;
               }
               // Check if \_S5 was found
               if (dsdtLength > 0) {
                  // Check for valid AML structure
                  if ((*(S5Addr - 1) == 0x08 || (*(S5Addr - 2) == 0x08 && *(S5Addr - 1) == '\\')) && *(S5Addr + 4) == 0x12) {
                     S5Addr += 5;
                     S5Addr += ((*S5Addr &0xC0)>>6) + 2; // Calculate PkgLength size

                     if (*S5Addr == 0x0A) {
                        S5Addr++; // Skip byteprefix
                     }
                     SLP_TYPa = *(S5Addr)<<10;
                     S5Addr++;

                     if (*S5Addr == 0x0A) {
                        S5Addr++; // Skip byteprefix
                     }
                     SLP_TYPb = *(S5Addr)<<10;

                     SMI_CMD = facp->SMI_CMD;

                     ACPI_ENABLE = facp->ACPI_ENABLE;
                     ACPI_DISABLE = facp->ACPI_DISABLE;

                     PM1a_CNT = facp->PM1a_CNT_BLK;
                     PM1b_CNT = facp->PM1b_CNT_BLK;
                     
                     PM1_CNT_LEN = facp->PM1_CNT_LEN;

                     SLP_EN = 1<<13;
                     SCI_EN = 1;

                     return 0;
                  } else {
                     kprint_rfail();
                     kprint("\\_S5 parse error.\n");
                  }
               } else {
                  kprint_rfail();
                  kprint("\\_S5 not present.\n");
               }
            } else {
               kprint_rfail();
               kprint("DSDT invalid.\n");
            }
         }
         ptr++;
      }
      kprint_rfail();
      kprint("No valid FACP present.\n");
   } else {
      kprint_rfail();
      kprint("No ACPI.\n");
   }

   return -1;
}

void acpi_shutdown() {
   // SCI_EN is set to 1 if ACPI shutdown is possible
   if (SCI_EN == 0) {
      return;
   }

   acpi_enable();

   // Send the shutdown command
   port_word_out((uint32_t) PM1a_CNT, SLP_TYPa | SLP_EN);
   if (PM1b_CNT != 0) {
      port_word_out((uint32_t) PM1b_CNT, SLP_TYPb | SLP_EN);
   }

   PANIC("ACPI Shutdown Failed.\n", "acpi.c", 269);
}

void acpi_install() {
   acpi_init();
   acpi_enable();
}
