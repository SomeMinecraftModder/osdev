#include "detect.h"
#include "../debug/printf.h"
#include "../drivers/screen.h"
#include <cpuid.h>
#include <stdint.h>

void cpudetect() {
    // Register storage
    unsigned int eax, ebx, ecx, edx;

    unsigned int largestStandardFunc;
    char vendor[13];
    __get_cpuid(0, &largestStandardFunc, (unsigned int *)(vendor + 0),
                (unsigned int *)(vendor + 8), (unsigned int *)(vendor + 4));
    vendor[12] = '\0';

    printf("CPU vendor: %s\n", vendor);

    if (largestStandardFunc >= 0x01) {
        __get_cpuid(0x01, &eax, &ebx, &ecx, &edx);

        kprint("Features:");

        if (edx & EDX_PSE)
            kprint(" PSE");
        if (edx & EDX_PAE)
            kprint(" PAE");
        if (edx & EDX_FPU)
            kprint(" FPU");
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

    unsigned int largestExtendedFunc;
    __get_cpuid(0x80000000, &largestExtendedFunc, &ebx, &ecx, &edx);

    if (largestExtendedFunc >= 0x80000001) {
        __get_cpuid(0x80000001, &eax, &ebx, &ecx, &edx);

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
        __get_cpuid(0x80000002, (unsigned int *)(name + 0),
                    (unsigned int *)(name + 4), (unsigned int *)(name + 8),
                    (unsigned int *)(name + 12));
        __get_cpuid(0x80000003, (unsigned int *)(name + 16),
                    (unsigned int *)(name + 20), (unsigned int *)(name + 24),
                    (unsigned int *)(name + 28));
        __get_cpuid(0x80000004, (unsigned int *)(name + 32),
                    (unsigned int *)(name + 36), (unsigned int *)(name + 40),
                    (unsigned int *)(name + 44));

        // Processor name is right justified with leading spaces
        char *p = name;
        while (*p == ' ') {
            ++p;
        }

        printf("CPU name: %s\n", p);
    }
}
