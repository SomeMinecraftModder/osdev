#include "detect.h"
#include "../debug/printf.h"
#include "../drivers/screen.h"
#include <stdint.h>

static inline void cpuid(uint32_t reg, uint32_t *eax, uint32_t *ebx,
                         uint32_t *ecx, uint32_t *edx) {
    asm volatile("cpuid"
                 : "=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx)
                 : "0"(reg));
}

void cpudetect() {
    // Register storage
    uint32_t eax, ebx, ecx, edx;

    uint32_t largestStandardFunc;
    char vendor[13];
    cpuid(0, &largestStandardFunc, (uint32_t *)(vendor + 0),
          (uint32_t *)(vendor + 8), (uint32_t *)(vendor + 4));
    vendor[12] = '\0';

    kprintf("CPU vendor: %s\n", vendor);

    if (largestStandardFunc >= 0x01) {
        cpuid(0x01, &eax, &ebx, &ecx, &edx);

        kprint("Features:");

        if (edx & EDX_PSE)
            kprint(" PSE");
        if (edx & EDX_PAE)
            kprint(" PAE");
        if (edx & EDX_APIC)
            kprint(" APIC");
        if (edx & EDX_MTRR)
            kprint(" MTRR");

        kprint("\n");

        kprint("Instructions:");

        if (edx & EDX_TSC)
            kprint(" TSC");
        if (edx & EDX_MSR)
            kprint(" MSR");
        if (ecx & ECX_AVX)
            kprint(" AVX");
        if (ecx & ECX_F16C)
            kprint(" F16C");
        if (ecx & ECX_RDRAND)
            kprint(" RDRAND");
        if (edx & EDX_MMX)
            kprint(" MMX");
        if (edx & EDX_SSE)
            kprint(" SSE");
        if (edx & EDX_SSE2)
            kprint(" SSE2");
        if (ecx & ECX_SSE3)
            kprint(" SSE3");
        if (ecx & ECX_SSSE3)
            kprint(" SSSE3");
        if (ecx & ECX_SSE41)
            kprint(" SSE4.1");
        if (ecx & ECX_SSE42)
            kprint(" SSE4.2");
    }

    uint32_t largestExtendedFunc;
    cpuid(0x80000000, &largestExtendedFunc, &ebx, &ecx, &edx);

    if (largestExtendedFunc >= 0x80000001) {
        cpuid(0x80000001, &eax, &ebx, &ecx, &edx);

        if (ecx & ECX_SSE4A)
            kprint(" SSE4A");
        if (edx & EDX_3DNOW)
            kprint(" 3DNow");
        if (edx & EDX_3DNOWEXT)
            kprint(" 3DNowext");

        kprint("\n");

        if (edx & EDX_64_BIT) {
            kprint("64-bit Architecture\n");
        }
    } else {
        kprint("\n");
    }

    if (largestExtendedFunc >= 0x80000004) {
        char name[48];
        cpuid(0x80000002, (uint32_t *)(name + 0), (uint32_t *)(name + 4),
              (uint32_t *)(name + 8), (uint32_t *)(name + 12));
        cpuid(0x80000003, (uint32_t *)(name + 16), (uint32_t *)(name + 20),
              (uint32_t *)(name + 24), (uint32_t *)(name + 28));
        cpuid(0x80000004, (uint32_t *)(name + 32), (uint32_t *)(name + 36),
              (uint32_t *)(name + 40), (uint32_t *)(name + 44));

        // Processor name is right justified with leading spaces
        char *p = name;
        while (*p == ' ') {
            ++p;
        }

        kprintf("CPU name: %s\n", p);
    }
}
