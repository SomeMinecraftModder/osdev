#include "acpi.h"
#include "../cpu/ports.h"
#include "../cpu/timer.h"
#include "../drivers/screen.h"
#include "../kernel/panic.h"
#include <stddef.h>
#include <string.h>

uint32_t *SMI_CMD;
uint8_t ACPI_ENABLE;
uint8_t ACPI_DISABLE;
uint32_t *PM1a_CNT;
uint32_t *PM1b_CNT;
uint16_t SLP_TYPa;
uint16_t SLP_TYPb;
uint16_t SLP_EN;
uint16_t SCI_EN;

struct RSDPtr {
    char Signature[8];
    uint8_t Checksum;
    char OEMID[6];
    uint8_t Revision;
    uint32_t RsdtAddress;
} __attribute__((packed));

struct ACPISDTHeader {
    char Signature[4];
    uint32_t Length;
    uint8_t Revision;
    uint8_t Checksum;
    char OEMID[6];
    char OEMTableID[8];
    uint32_t OEMRevision;
    uint32_t CreatorID;
    uint32_t CreatorRevision;
};

struct GenericAddressStructure {
    uint8_t AddressSpace;
    uint8_t BitWidth;
    uint8_t BitOffset;
    uint8_t AccessSize;
    uint64_t Address;
};

// The FADT is a complex structure and contains a lot of data
// Source (for the above comment and this table): OSDev wiki
struct FACP {
    struct ACPISDTHeader h;
    uint32_t FirmwareCtrl;
    uint32_t Dsdt;

    // Field used in ACPI 1.0; no longer in use, for compatibility only
    uint8_t Reserved;

    uint8_t PreferredPowerManagementProfile;
    uint16_t SCI_Interrupt;
    uint32_t SMI_CommandPort;
    uint8_t AcpiEnable;
    uint8_t AcpiDisable;
    uint8_t S4BIOS_REQ;
    uint8_t PSTATE_Control;
    uint32_t PM1aEventBlock;
    uint32_t PM1bEventBlock;
    uint32_t PM1aControlBlock;
    uint32_t PM1bControlBlock;
    uint32_t PM2ControlBlock;
    uint32_t PMTimerBlock;
    uint32_t GPE0Block;
    uint32_t GPE1Block;
    uint8_t PM1EventLength;
    uint8_t PM1ControlLength;
    uint8_t PM2ControlLength;
    uint8_t PMTimerLength;
    uint8_t GPE0Length;
    uint8_t GPE1Length;
    uint8_t GPE1Base;
    uint8_t CStateControl;
    uint16_t WorstC2Latency;
    uint16_t WorstC3Latency;
    uint16_t FlushSize;
    uint16_t FlushStride;
    uint8_t DutyOffset;
    uint8_t DutyWidth;
    uint8_t DayAlarm;
    uint8_t MonthAlarm;
    uint8_t Century;

    // Reserved in ACPI 1.0; used since ACPI 2.0+
    uint16_t BootArchitectureFlags;

    uint8_t Reserved2;
    uint32_t Flags;

    struct GenericAddressStructure ResetReg;

    uint8_t ResetValue;
    uint8_t Reserved3[3];

    // 64bit pointers - Available on ACPI 2.0+
    uint64_t X_FirmwareControl;
    uint64_t X_Dsdt;

    struct GenericAddressStructure X_PM1aEventBlock;
    struct GenericAddressStructure X_PM1bEventBlock;
    struct GenericAddressStructure X_PM1aControlBlock;
    struct GenericAddressStructure X_PM1bControlBlock;
    struct GenericAddressStructure X_PM2ControlBlock;
    struct GenericAddressStructure X_PMTimerBlock;
    struct GenericAddressStructure X_GPE0Block;
    struct GenericAddressStructure X_GPE1Block;
};

// Check if the given address has a valid header
uint32_t *acpi_check_rsdp_tr(uint32_t *ptr) {
    struct RSDPtr *rsdp = (struct RSDPtr *)ptr;

    if (memcmp("RSD PTR ", rsdp, 8) == 0) {
        uint8_t *bptr;
        uint8_t check = 0;
        uint32_t i;
        // Check checksum RSDPD
        bptr = (uint8_t *)ptr;
        for (i = 0; i < sizeof(struct RSDPtr); i++) {
            check += *bptr;
            bptr++;
        }

        // Found valid RSDPD
        if (check == 0) {
            return (uint32_t *)rsdp->RsdtAddress;
        }
    }

    return NULL;
}

// Finds the ACPI header and returns the address of the RSDT
uint32_t *acpi_get_rsdp_tr() {
    uint32_t *addr;
    uint32_t *rsdp;

    // Search below the 1mb mark for RSDP signature
    for (addr = (uint32_t *)0x000E0000; (int)addr < 0x00100000;
         addr += 0x10 / sizeof(addr)) {
        rsdp = acpi_check_rsdp_tr(addr);
        if (rsdp != NULL) {
            return rsdp;
        }
    }

    // At address 0x40:0x0E is the RM segment of the EBDA
    int ebda = *((short *)0x40E);    // Get pointer
    ebda = ebda * 0x10 & 0x000FFFFF; // Transform segment into linear address

    // Search Extended BIOS Data Area for the Root System Description Pointer
    // signature
    for (addr = (uint32_t *)ebda; (int)addr < ebda + 1024;
         addr += 0x10 / sizeof(addr)) {
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
        char *checkPtr = (char *)ptr;
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
    if ((port_word_in((uint32_t)PM1a_CNT) & SCI_EN) == 0) {
        // Check if ACPI can be enabled
        if (SMI_CMD != 0 && ACPI_ENABLE != 0) {
            port_byte_out((uint32_t)SMI_CMD,
                          ACPI_ENABLE); // Send ACPI enable command
            // Give 3 seconds time to enable ACPI
            int i;
            for (i = 0; i < 300; i++) {
                if ((port_word_in((uint32_t)PM1a_CNT) & SCI_EN) == 1) {
                    break;
                }

                sleep(10);
            }

            if (PM1b_CNT != 0) {
                for (; i < 300; i++) {
                    if ((port_word_in((uint32_t)PM1b_CNT) & SCI_EN) == 1) {
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
                kprint("Couldn't enable ACPI.\n");
                return -1;
            }
        } else {
            kprint_rfail();
            kprint("No known way to enable ACPI.\n");
            return -1;
        }
    } else {
        kprint_gok();
        kprint("ACPI is already enabled.\n");
        return 0;
    }
}

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
            if (acpi_check_header((uint32_t *)*ptr, "FACP") == 0) {
                entrys = -2;
                struct FACP *facp = (struct FACP *)*ptr;
                if (acpi_check_header((uint32_t *)facp->Dsdt, "DSDT") == 0) {
                    // Search the \_S5 package in the DSDT
                    char *S5Addr = (char *)facp->Dsdt + 36; // Skip header
                    int dsdtLength = (facp->Dsdt + 1) - 36;
                    while (0 < dsdtLength--) {
                        if (memcmp(S5Addr, "_S5_", 4) == 0) {
                            break;
                        }
                        S5Addr++;
                    }

                    // Check if \_S5 was found
                    if (dsdtLength > 0) {
                        // Check for valid AML structure
                        if ((*(S5Addr - 1) == 0x08 ||
                             (*(S5Addr - 2) == 0x08 &&
                              *(S5Addr - 1) == '\\')) &&
                            *(S5Addr + 4) == 0x12) {
                            S5Addr += 5;
                            S5Addr += ((*S5Addr & 0xC0) >> 6) +
                                      2; // Calculate PkgLength size

                            if (*S5Addr == 0x0A) {
                                S5Addr++; // Skip byteprefix
                            }

                            SLP_TYPa = *(S5Addr) << 10;
                            S5Addr++;

                            if (*S5Addr == 0x0A) {
                                S5Addr++; // Skip byteprefix
                            }

                            SLP_TYPb = *(S5Addr) << 10;

                            SMI_CMD = (uint32_t *)facp->SMI_CommandPort;

                            ACPI_ENABLE = facp->AcpiEnable;
                            ACPI_DISABLE = facp->AcpiDisable;

                            PM1a_CNT = (uint32_t *)facp->PM1aControlBlock;
                            PM1b_CNT = (uint32_t *)facp->PM1bControlBlock;

                            SLP_EN = 1 << 13;
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
    port_word_out((uint32_t)PM1a_CNT, SLP_TYPa | SLP_EN);
    if (PM1b_CNT != 0) {
        port_word_out((uint32_t)PM1b_CNT, SLP_TYPb | SLP_EN);
    }

    PANIC(__FILE__, __LINE__, "ACPI shutdown failed.");
}

void acpi_install() {
    acpi_init();
    acpi_enable();
}
