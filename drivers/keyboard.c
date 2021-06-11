#include "keyboard.h"
#include "../cpu/isr.h"
#include "../cpu/ports.h"
#include "../kernel/shell.h"
#include "../libc/function.h"
#include "screen.h"
#include <string.h>

static int caps_lock = 0;
static int azerty = 0;
char key_buffer[256];
uint8_t last_scancode;
char history[256];
uint8_t last_tab;
uint8_t scancode;
int enter = 0;
int i;

// Uppercase characters
const char sc_ascii[] = {
  '?', '?', '?', '?',  '?', '?', '?', '?',  '?', '?', '?', '?', '?', '?', '?',
  '?', '`', '?', '?',  '?', '?', 'Q', '1',  '?', '?', '?', 'Z', 'S', 'A', 'W',
  '2', '?', '?', 'C',  'X', 'D', 'E', '4',  '3', '?', '?', ' ', 'V', 'F', 'T',
  'R', '5', '?', '?',  'N', 'B', 'H', 'G',  'Y', '6', '?', '?', '?', 'M', 'J',
  'U', '7', '8', '?',  '?', ',', 'K', 'I',  'O', '0', '9', '?', '?', '.', '/',
  'L', ';', 'P', '-',  '?', '?', '?', '\'', '?', '[', '=', '?', '?', '?', '?',
  '?', ']', '?', '\\', '?', '?', '?', '?',  '?', '?', '?', '?', '?', '?', '?',
  '1', '?', '4', '7',  '?', '?', '?', '0',  '.', '2', '5', '6', '8', '?', '?',
  '?', '+', '3', '-',  '*', '9', '?', '?',  '?', '?', '?', '?'};

// Lowercase characters
const char Lsc_ascii[] = {
  '?', '?', '?', '?',  '?', '?', '?', '?',  '?', '?', '?', '?', '?', '?', '?',
  '?', '`', '?', '?',  '?', '?', 'q', '1',  '?', '?', '?', 'z', 's', 'a', 'w',
  '2', '?', '?', 'c',  'x', 'd', 'e', '4',  '3', '?', '?', ' ', 'v', 'f', 't',
  'r', '5', '?', '?',  'n', 'b', 'h', 'g',  'y', '6', '?', '?', '?', 'm', 'j',
  'u', '7', '8', '?',  '?', ',', 'k', 'i',  'o', '0', '9', '?', '?', '.', '/',
  'l', ';', 'p', '-',  '?', '?', '?', '\'', '?', '[', '=', '?', '?', '?', '?',
  '?', ']', '?', '\\', '?', '?', '?', '?',  '?', '?', '?', '?', '?', '?', '?',
  '1', '?', '4', '7',  '?', '?', '?', '0',  '.', '2', '5', '6', '8', '?', '?',
  '?', '+', '3', '-',  '*', '9', '?', '?',  '?', '?', '?', '?'};

// Azerty variants
const char sc_azerty[] = {
  '?', '?', '?', '?',  '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?',
  '?', '`', '?', '?',  '?', '?', 'A', '1', '?', '?', '?', 'W', 'S', 'Q', 'Z',
  '2', '?', '?', 'C',  'X', 'D', 'E', '4', '3', '?', '?', ' ', 'V', 'F', 'T',
  'R', '5', '?', '?',  'N', 'B', 'H', 'G', 'Y', '6', '?', '?', '?', ',', 'J',
  'U', '7', '8', '?',  '?', ';', 'K', 'I', 'O', '0', '9', '?', '?', ':', '~',
  'L', 'M', 'P', ')',  '?', '?', '?', '?', '?', '[', '=', '?', '?', '?', '?',
  '?', ']', '?', '\\', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?',
  '1', '?', '4', '7',  '?', '?', '?', '0', '.', '2', '5', '6', '8', '?', '?',
  '?', '+', '3', '-',  '*', '9', '?', '?', '?', '?', '?', '?'};

const char Lsc_azerty[] = {
  '?', '?', '?', '?',  '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?',
  '?', '`', '?', '?',  '?', '?', 'a', '1', '?', '?', '?', 'w', 's', 'q', 'z',
  '2', '?', '?', 'c',  'x', 'd', 'e', '4', '3', '?', '?', ' ', 'v', 'f', 't',
  'r', '5', '?', '?',  'n', 'b', 'h', 'g', 'y', '6', '?', '?', '?', ',', 'j',
  'u', '7', '8', '?',  '?', ';', 'k', 'i', 'o', '0', '9', '?', '?', ':', '~',
  'l', 'm', 'p', ')',  '?', '?', '?', '?', '?', '[', '=', '?', '?', '?', '?',
  '?', ']', '?', '\\', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', '?',
  '1', '?', '4', '7',  '?', '?', '?', '0', '.', '2', '5', '6', '8', '?', '?',
  '?', '+', '3', '-',  '*', '9', '?', '?', '?', '?', '?', '?'};

static void keyboard_callback(registers_t *regs) {
    last_scancode = scancode;
    // The PIC leaves us the scancode in port 0x60
    scancode = port_byte_in(0x60);

    if (scancode > SC_MAX) {
        return;
    }

    if (last_scancode == 0xF0) {
        // An key was released
        return;
    } else if (last_scancode == 0xE0) {
        if (scancode == 0x75) {
            if (key_buffer[0] == '\0') {
                kprint(history);
                i = 0;
                while (history[i] != 0) {
                    append(key_buffer, history[i]);
                    ++i;
                }
            }
            return;
        }

        if (scancode == 0x72) {
            i = strlen(key_buffer);
            while (i > 0) {
                kprint_backspace();
                i--;
            }
            key_buffer[0] = '\0';
            return;
        }

        return;
    }

    switch (scancode) {
        case BACKSPACE:
            i = 0;
            if (last_tab == 1) {
                for (i = 0; i < TAB_SIZE; ++i) {
                    kprint_backspace();
                    backspace(key_buffer);
                }
                last_tab = 0;
                enter = 0;
                break;
            }

            if (strlen(key_buffer) != 0) {
                kprint_backspace();
            }
            backspace(key_buffer);
            enter = 0;
            break;

        case ENTER:
            kprint("\n");
            shell(key_buffer);
            strcpy(history, key_buffer);
            key_buffer[0] = '\0';
            last_tab = 0;
            enter = 1;
            break;

        case CAPSLOCK:
            // Toggle caps lock current status
            if (caps_lock) {
                caps_lock = 0;
            } else {
                caps_lock = 1;
            }
            enter = 0;
            break;

        case TAB:
            putchar('\t');
            i = 0;
            for (i = 0; i < TAB_SIZE; ++i) {
                append(key_buffer, ' ');
            }
            last_tab = 1;
            enter = 0;
            break;

        case LSHIFT:
            if (azerty == 0) {
                azerty = 1;
            } else if (azerty == 1) {
                azerty = 0;
            }
            enter = 0;
            break;

        default: {
            char letter;
            if (azerty) {
                letter = sc_azerty[(int)scancode];
            } else {
                letter = sc_ascii[(int)scancode];
            }

            if (!caps_lock) {
                if (azerty) {
                    letter = Lsc_azerty[(int)scancode];
                } else {
                    letter = Lsc_ascii[(int)scancode];
                }
            }

            append(key_buffer, letter);
            putchar(letter);
            last_tab = 0;
            enter = 0;
            break;
        }
    }

    UNUSED(regs);
}

void init_keyboard() {
    // Disable keyboard
    keybcon_write(0xAD);
    keybcon_write(0xA7);

    // Flush output buffer
    port_byte_in(0x60);

    keybcon_write(0x20);
    // Store command byte
    uint8_t byte = keyb_read();

    keybcon_write(0x60);
    // Clear "First PS/2 port interrupt" bit
    keyb_write((byte & ~(1 << 0)));

    keybcon_write(0x60);
    // Clear "Second PS/2 port interrupt" bit
    keyb_write((byte & ~(1 << 1)));

    keybcon_write(0x60);
    // Clear "First PS/2 port translation" bit
    keyb_write((byte & ~(1 << 6)));

    keybcon_write(0x20);
    // Store command byte
    uint8_t byte1 = keyb_read();

    // Send test command
    keybcon_write(0xAA);

    // Read result
    int testres = keyb_read();
    int contestres;

    // Restore command byte
    keybcon_write(0x60);
    keyb_write((byte1));

    // Check self test result
    if (testres == 0x55) {
        contestres = 1;
    } else {
        contestres = 0;
    }

    // Check if is dual channel
    int dualchannel = 0;

    if (byte & ~(1 << 6)) {
        dualchannel = 1;
    }

    // Test PS/2 ports
    keybcon_write(0xAB);
    int firstret = keyb_read();
    int secondret = 1;

    if (dualchannel) {
        keybcon_write(0xA9);
        secondret = keyb_read();
    }

    // Enable devices
    keybcon_write(0xAE);

    keybcon_write(0x20);
    uint8_t byte2 = keyb_read();

    keybcon_write(0x60);
    keyb_write((byte2 & ~(2 << 0)));

    if (dualchannel) {
        keybcon_write(0x60);
        keyb_write((byte2 & ~(2 << 1)));

        keybcon_write(0xA8);
    }

    int devret1 = 0;
    int devret2 = 0;

    // Reset devices
    keyb_write(0xFF);

    // Read until device ports are clean
    while (devret1 != 101) {
        devret1 = keyb_read();
    }

    if (dualchannel) {
        keybdual_write(0xFF);
        while (devret2 != 101) {
            devret2 = keyb_read();
        }
    }

    if ((firstret == 0x00 && secondret == 1 && contestres == 1) ||
        (firstret == 0x00 && secondret == 0x00 && contestres == 1)) {
        kprint_gok();
        kprint("Enabling keyboard.\n");
    } else {
        kprint_rfail();
        kprint("Error enabling keyboard.\n");
    }

    register_interrupt_handler(IRQ1, keyboard_callback);
}

uint8_t keybcon_read() {
    return port_byte_in(0x64);
}

void keybcon_write(uint8_t cmd) {
    while (1) {
        if ((keybcon_read() & 2) == 0) {
            break;
        }
    }

    port_byte_out(0x64, cmd);
}

uint8_t keyb_read() {
    if ((keybcon_read() & 1) == 0) {
        // A generic error code
        return 101;
    }

    return port_byte_in(0x60);
}

void keyb_write(uint8_t cmd) {
    while (1) {
        if ((keybcon_read() & 2) == 0) {
            break;
        }
    }

    port_byte_out(0x60, cmd);
}

void keybdual_write(uint8_t cmd) {
    keybcon_write(0xD4);

    while (1) {
        if ((keybcon_read() & 2) == 0) {
            break;
        }
    }

    port_byte_out(0x60, cmd);
}
