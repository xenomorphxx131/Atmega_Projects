#ifndef PROCESS_USB_H
#define PROCESS_USB_H

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>
#include <LUFA/Version.h>
#include <LUFA/Drivers/USB/USB.h>

#include "Descriptors.h"

/* Function Prototypes: */

void process_USB(void);
void EVENT_USB_Device_Connect(void);
void EVENT_USB_Device_Disconnect(void);
void EVENT_USB_Device_ConfigurationChanged(void);
void EVENT_USB_Device_ControlRequest(void);
void CDC_init(void);

extern USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface;
#endif
