#ifndef ACPI_H
#define ACPI_H

#include <stdint.h>

uint32_t *acpi_check_rsdp_tr(uint32_t *ptr);
uint32_t *acpi_get_rsdp_tr();
void acpi_shutdown();
void acpi_install();
int acpi_enable();
int acpi_init();

#endif
