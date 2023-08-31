  #include <avr/wdt.h>
  #include <avr/io.h>
  #include <util/delay.h>

  #include <LUFA/Common/Common.h>
  #include <LUFA/Drivers/USB/USB.h>
  
  #include "bootloader.h"

  uint32_t Boot_Key ATTR_NO_INIT;

/**************************************************************************
*    From Dean Camera, runs before main program - see the prototype       *
***************************************************************************/
void Bootloader_Jump_Check(void)
{
	// If the reset source was the bootloader and the key is correct,
	// clear it and jump to the bootloader
	if ((MCUSR & (1 << WDRF)) && (Boot_Key == MAGIC_BOOT_KEY))
	{
		Boot_Key = 0;
		((void (*)(void))BOOTLOADER_START_ADDRESS)();
	}
}
/**************************************************************************
*    From Dean Camera, runs before main program - see the prototype       *
***************************************************************************/
void Jump_To_Bootloader(void)
{
	USB_Disable();				// If USB is used, detach from the bus and reset it
	cli();						// Disable all interrupts
	Delay_MS(2000); 			// Wait two seconds for the USB detachment to register on the host
	Boot_Key = MAGIC_BOOT_KEY;	// Set the bootloader key to the magic value and force a reset
	wdt_enable(WDTO_250MS);		// By enabling the watchdog timer
	for (;;);					// and getting stuck in an endless loop
}