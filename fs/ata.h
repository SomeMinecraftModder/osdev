#ifndef ATA_H
#define ATA_H

#include <stdint.h>

void ata_read(uint8_t *target_address, uint32_t LBA, uint8_t sector_count);
void ata_write(uint32_t LBA, uint8_t sector_count, uint8_t *bytes);
void ata_write_one(uint32_t LBA, uint8_t *bytes);

#endif
