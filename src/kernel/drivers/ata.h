#ifndef ATA_H
#define ATA_H

#include <stdint.h>

// --- ATA I/O Ports (Primary Bus) ---
#define ATA_DATA        0x1F0   // Read/Write Data
#define ATA_ERROR       0x1F1   // Error Register
#define ATA_SECTOR_CNT  0x1F2   // Number of sectors to read/write
#define ATA_LBA_LOW     0x1F3   // LBA (Address) Low Byte
#define ATA_LBA_MID     0x1F4   // LBA Mid Byte
#define ATA_LBA_HIGH    0x1F5   // LBA High Byte
#define ATA_DRIVE_HEAD  0x1F6   // Drive/Head Select
#define ATA_COMMAND     0x1F7   // Command Port (Write) / Status Port (Read)

// --- ATA Commands ---
#define ATA_CMD_IDENTIFY 0xEC   // "Who are you?"
#define ATA_CMD_READ     0x20   // Read Sectors
#define ATA_CMD_WRITE    0x30   // Write Sectors

// --- Status Bitmasks ---
#define ATA_SR_BSY      0x80    // Busy
#define ATA_SR_DRQ      0x08    // Data Request (Ready to transfer)
#define ATA_SR_ERR      0x01    // Error

void ata_init(void);
void ata_read_sector(uint32_t lba, uint8_t* buffer);
void ata_write_sector(uint32_t lba, uint8_t* buffer);
void ata_identify_drive(void);

#endif