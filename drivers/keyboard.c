#include "../libc/function.h"
#include "../kernel/kernel.h"
#include "../libc/string.h"
#include "../cpu/ports.h"
#include "../cpu/isr.h"
#include "keyboard.h"
#include "screen.h"
#include <stdint.h>

static int CapsLockStatus = 0;
static char key_buffer[256];
static int AzertyStatus = 0;
static char *history = " ";

// UpperCase Chars
const char *sc_name[] = { "ERROR", "Esc", "1", "2", "3", "4", "5", "6",
    "7", "8", "9", "0", "-", "=", "Backspace", "Tab", "Q", "W", "E",
        "R", "T", "Y", "U", "I", "O", "P", "[", "]", "Enter", "Lctrl",
        "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "'", "`",
        "LShift", "\\", "Z", "X", "C", "V", "B", "N", "M", ",", ".",
        "/", "RShift", "Keypad *", "LAlt", "Spacebar", "CapsLock"};
const char sc_ascii[] = { '?', '?', '1', '2', '3', '4', '5', '6',
    '7', '8', '9', '0', '-', '=', '?', '?', 'Q', 'W', 'E', 'R', 'T', 'Y',
        'U', 'I', 'O', 'P', '[', ']', '?', '?', 'A', 'S', 'D', 'F', 'G',
        'H', 'J', 'K', 'L', ';', '\'', '`', '?', '\\', 'Z', 'X', 'C', 'V',
        'B', 'N', 'M', ',', '.', '/', '?', '?', '?', ' ', '?'};

// LowerCase Chars
const char *Lsc_name[] = { "ERROR", "Esc", "1", "2", "3", "4", "5", "6",
    "7", "8", "9", "0", "-", "=", "Backspace", "Tab", "q", "w", "e",
        "r", "t", "y", "u", "i", "o", "p", "[", "]", "Enter", "Lctrl",
        "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "'", "`",
        "LShift", "\\", "z", "x", "c", "v", "b", "n", "m", ",", ".",
        "/", "RShift", "Keypad *", "LAlt", "Spacebar", "CapsLock"};
const char Lsc_ascii[] = { '?', '?', '1', '2', '3', '4', '5', '6',
    '7', '8', '9', '0', '-', '=', '?', '?', 'q', 'w', 'e', 'r', 't', 'y',
        'u', 'i', 'o', 'p', '[', ']', '?', '?', 'a', 's', 'd', 'f', 'g',
        'h', 'j', 'k', 'l', ';', '\'', '`', '?', '\\', 'z', 'x', 'c', 'v',
        'b', 'n', 'm', ',', '.', '/', '?', '?', '?', ' ', '?'};

// Azerty variants
const char sc_azerty[] = { '?', '?', '1', '2', '3', '4', '5', '6',
    '7', '8', '9', '0', ')', '-', '?', '?', 'A', 'Z', 'E', 'R', 'T', 'Y',
        'U', 'I', 'O', 'P', '[', ']', '?', '?', 'Q', 'S', 'D', 'F', 'G',
        'H', 'J', 'K', 'L', 'M', '?', '`', '?', '\\', 'W', 'X', 'C', 'V',
        'B', 'N', ',', ';', ':', '~', '?', '?', '?', ' ', '?'};

const char Lsc_azerty[] = { '?', '?', '1', '2', '3', '4', '5', '6',
    '7', '8', '9', '0', ')', '=', '?', '?', 'a', 'z', 'e', 'r', 't', 'y',
        'u', 'i', 'o', 'p', '[', ']', '?', '?', 'q', 's', 'd', 'f', 'g',
        'h', 'j', 'k', 'l', 'm', '?', '`', '?', '\\', 'w', 'x', 'c', 'v',
        'b', 'n', ',', ';', ':', '~', '?', '?', '?', ' ', '?'};


static void keyboard_callback(registers_t *regs) {
    // The PIC leaves us the scancode in port 0x60
    uint8_t scancode = port_byte_in(0x60);

    if (scancode > SC_MAX) return;
    if (scancode == BACKSPACE) {
        kprint_backspace(key_buffer);
        backspace(key_buffer);
    } else if (scancode == ENTER) {
        kprint("\n");
        user_input(key_buffer); // kernel-controlled function
        strcpy(key_buffer, history);
        key_buffer[0] = '\0';
    } else if (scancode == CAPSLOCK) {
        // Toggle caps lock current status
        if (CapsLockStatus) {
            CapsLockStatus = 0;
        } else {
            CapsLockStatus = 1;
        }
    } else if (scancode == TAB) {
        if (AzertyStatus == 0) {
            AzertyStatus = 1;
        } else {
            AzertyStatus = 0;
        }
    } else if (scancode == LSHIFT) {
        if (key_buffer[0] == '\0') {
            kprint(history);
            int i = 0;
            while (history[i] != 0) {
                append(key_buffer, history[i]);
                i++;
            }
        }
    } else {
        char letter;
        if (AzertyStatus == 1) {
            letter = sc_azerty[(int)scancode];
        } else {
            letter = sc_ascii[(int)scancode];
        }

        if (CapsLockStatus == 0) {
            if (AzertyStatus == 1) {
                letter = Lsc_azerty[(int)scancode];
            } else {
                letter = Lsc_ascii[(int)scancode];
            }
        }
        // Remember that kprint only accepts char[]
        char str[2] = {letter, '\0'};
        append(key_buffer, letter);
        kprint(str);
    }
    UNUSED(regs);
}

void init_keyboard() {
   register_interrupt_handler(IRQ1, keyboard_callback);
}
