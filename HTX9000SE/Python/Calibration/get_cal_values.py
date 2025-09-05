import serial
import sys
sys.path.append("../../../../../PyICe")
import visa_wrappers
import time

comport_number = raw_input("Enter COM Port Number: COM")
port = serial.Serial("COM" + comport_number, timeout = 5)
loadbox = visa_wrappers.visa_wrapper_serial(port)

print "\n\n" + loadbox.ask("*IDN?")
print "CAL DATE:               ",
print loadbox.ask("CALibrate:DATE?")
print "CAL DAC HIGH SETTING:   ",
print loadbox.ask("CALibrate:OUTPut:CURRent:DAC:RANGe:HIgh?")
print "CAL VALUE HIGH SETTING: ",
print loadbox.ask("CALibrate:OUTPut:CURRent:VALue:RANGe:HIgh?")
print "CAL DAC LOW SETTING:    ",
print loadbox.ask("CALibrate:OUTPut:CURRent:DAC:RANGe:LOw?")
print "CAL VALUE LOW SETTING:  ",
print loadbox.ask("CALibrate:OUTPut:CURRent:VALue:RANGe:LOw?")
print "CAL LOW CHECKSUM:       ",
print loadbox.ask("CALibrate:CHECKsum:LOw?")
print "CAL HIGH CHECKSUM:      ",
print loadbox.ask("CALibrate:CHECKsum:HIgh?")

raw_input("\n\nHit Enter to Quit")