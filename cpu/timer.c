#include "timer.h"
#include "../libc/function.h"
#include "../libc/string.h"
#include "isr.h"
#include "ports.h"

uint32_t secboot = 0;
uint32_t minboot = 0;
uint32_t hourboot = 0;

volatile uint32_t tick = 0;
uint32_t hz = 0;

static void timer_update() {
    if ((tick % hz) == 0) {
        secboot += 1;
    }

    if (secboot == 60) {
        minboot += 1;
        secboot = 0;
    }

    if (minboot == 60) {
        hourboot += 1;
        minboot = 0;
    }
}

static void timer_callback(registers_t *regs) {
    tick++;
    timer_update();
    UNUSED(regs);
}

void init_timer(uint32_t freq) {
    hz = freq;
    // Install the function we just wrote
    register_interrupt_handler(IRQ0, timer_callback);

    // Get the PIT value: hardware clock at 1193180 Hz
    uint32_t divisor = 1193180 / freq;
    uint8_t low = (uint8_t)(divisor & 0xFF);
    uint8_t high = (uint8_t)((divisor >> 8) & 0xFF);
    // Send the command
    port_byte_out(0x43, 0x36); // Command port
    port_byte_out(0x40, low);
    port_byte_out(0x40, high);
}

void sleep(uint32_t ms) {
    char *s = "\0";
    uint32_t end;

    if (ms < 10) {
        end = tick + ms;
    } else {
        end = tick + atoi(strtruncate(int_to_ascii(ms, s), 1));
    }

    while (tick < end) {
        asm volatile("sti//hlt//cli");
    }
}

void sleepsec(uint32_t sec) {
    uint32_t end = tick + (sec * hz);
    while (tick < end) {
        asm volatile("sti//hlt//cli");
    }
}
