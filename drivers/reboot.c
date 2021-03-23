#include "../cpu/ports.h"
#include "reboot.h"
#include <stdint.h>

void reboot() {
    uint8_t r_int = 0x02;

    while (r_int & 0x02) {
        r_int = port_byte_in(0x64);
    }

    port_byte_out(0x64, 0xFE); // pulse CPU reset line

loop:
    halt(); // if that did not work, halt the CPU
    goto loop; // if a NMI is received, halt again
}
