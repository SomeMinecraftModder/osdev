#include "../cpu/ports.h"
#include "reboot.h"
#include <stdint.h>

void reboot() {
    uint8_t r_int = 0x02;

    while (r_int & 0x02) {
        r_int = port_byte_in(0x64);
    }

    port_byte_out(0x64, 0xFE); // Pulse CPU reset line

loop:
    halt(); // If that did not work, halt the CPU
    goto loop; // If a NMI is received, halt again
}

void shutdown_vm() {
    port_word_out(0x604, 0x2000); // QEMU specific instuction for shutdown
    port_word_out(0xB004, 0x2000); // QEMU (<2.0), Bochs
    port_word_out(0x4004, 0x3400); // VirtualBox
}
