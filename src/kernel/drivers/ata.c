#include "ata.h"
#include "../arch/x86_64/io.h"
#include "vga.h"

// Wait for the Drive to be ready (Not Busy)
void ata_wait_busy(void) {
    // Read Status Register (0x1F7) until BSY (bit 7) is clear
    while (inb(ATA_COMMAND) & ATA_SR_BSY);
}

// Wait for the Drive to be ready to transfer data (DRQ)
void ata_wait_drq(void) {
    // Read Status Register until DRQ (bit 3) is set
    while (!(inb(ATA_COMMAND) & ATA_SR_DRQ));
}

void ata_identify_drive(void) {
    ata_wait_busy();
    outb(ATA_DRIVE_HEAD, 0xA0); // Select Master
    outb(ATA_SECTOR_CNT, 0);
    outb(ATA_LBA_LOW, 0);
    outb(ATA_LBA_MID, 0);
    outb(ATA_LBA_HIGH, 0);
    outb(ATA_COMMAND, ATA_CMD_IDENTIFY);

    uint8_t status = inb(ATA_COMMAND);
    if (status == 0) {
        terminal_writestring("ATA: No Drive Found.\n");
        return;
    }

    ata_wait_busy();
    
    // Check for non-ATA devices (like ATAPI)
    uint8_t mid = inb(ATA_LBA_MID);
    uint8_t hi = inb(ATA_LBA_HIGH);
    if (mid || hi) {
        terminal_writestring("ATA: Drive is not ATA (likely ATAPI).\n");
        return;
    }

    // Wait for data to be ready
    ata_wait_drq();
    
    // Read 256 words (512 bytes) of info
    // We discard it for now, but this clears the buffer so the drive is happy.
    for (int i = 0; i < 256; i++) {
        inw(ATA_DATA);
    }
    
    terminal_writestring("ATA: Primary Master Drive Identified and Ready.\n");
}

void ata_read_sector(uint32_t lba, uint8_t* buffer) {
    ata_wait_busy();

    // Select Drive + Top 4 bits of LBA
    // 0xE0 = 11100000 (Mode LBA, Master Drive)
    outb(ATA_DRIVE_HEAD, 0xE0 | ((lba >> 24) & 0x0F));
    
    outb(ATA_SECTOR_CNT, 1);      // Read 1 sector
    outb(ATA_LBA_LOW,  (uint8_t)lba);
    outb(ATA_LBA_MID,  (uint8_t)(lba >> 8));
    outb(ATA_LBA_HIGH, (uint8_t)(lba >> 16));
    outb(ATA_COMMAND, ATA_CMD_READ);

    ata_wait_busy();
    ata_wait_drq();

    // Read the data into the buffer
    uint16_t* target = (uint16_t*)buffer;
    for (int i = 0; i < 256; i++) {
        target[i] = inw(ATA_DATA);
    }
}

void ata_write_sector(uint32_t lba, uint8_t* buffer) {
    ata_wait_busy();

    outb(ATA_DRIVE_HEAD, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_SECTOR_CNT, 1);
    outb(ATA_LBA_LOW,  (uint8_t)lba);
    outb(ATA_LBA_MID,  (uint8_t)(lba >> 8));
    outb(ATA_LBA_HIGH, (uint8_t)(lba >> 16));
    outb(ATA_COMMAND, ATA_CMD_WRITE);

    ata_wait_busy();
    ata_wait_drq();

    // Write the data from the buffer
    uint16_t* target = (uint16_t*)buffer;
    for (int i = 0; i < 256; i++) {
        outw(ATA_DATA, target[i]);
    }
    
    // Cache Flush (Optional but good practice)
    outb(ATA_COMMAND, 0xE7); 
    ata_wait_busy();
}