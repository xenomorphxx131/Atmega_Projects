#ifndef BOOTLOADER_H_
#define BOOTLOADER_H_
/**************************************************************************
*  Define Bootload Start Address                                          *
***************************************************************************/
//  #define BOOTLOADER_START_ADDRESS  (FLASH_SIZE_BYTES - BOOTLOADER_SEC_SIZE_BYTES)
	#define BOOTLOADER_START_ADDRESS 0x3800 // 0x3800 = (32768 - 4096) / 2 in hex, word address vs byte address
//	#define BOOTLOADER_START_ADDRESS 0x7000 // byte address - compiler specific issue
	#define MAGIC_BOOT_KEY	0xDC42ACCA

void Bootloader_Jump_Check(void) ATTR_INIT_SECTION(3);
void Jump_To_Bootloader(void);

#endif // SCPI_CONFIG_H_