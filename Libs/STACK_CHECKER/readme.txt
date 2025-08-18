This is a cool stack checker the AI came up with.

The RAM reporting of the AVR-GCC compiler doesn't tell the whole story and it's possible to get crashes when the memory utilization is north of 90%.

It should be pssoible to include this code as a watchdog for stack problem and maybe jump to bootloader or a blinky program if the excess memory gets too low.