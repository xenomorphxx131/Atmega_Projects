This project supports two sensor variants (somewhat).

The old sensor is the VL6180X which is obsolete.
The replacement is the VL534CD.

The code can support either variant (maybe) by setting the switch inside the make file
The switch is set by seeding the CFLAGS variable at likes 72/73 to the desired sensor.
Obviously only one of these should be set at a time.