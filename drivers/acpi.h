#ifndef ACPI_H
#define ACPI_H

#include <stdint.h>

typedef unsigned char byte;
typedef unsigned long dword;
typedef unsigned short word;

uint32_t *acpi_check_rsdp_tr(uint32_t *ptr);
uint32_t *acpi_get_rsdp_tr();
void acpi_power_off();
void acpi_install();
int acpi_enable();
int acpi_init();

#endif
