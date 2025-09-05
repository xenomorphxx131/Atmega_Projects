import serial
import sys
sys.path.append("../../../../PyICe")
import visa_wrappers
import time
import datetime

comport_number = raw_input("Enter COM Port Number: COM")
port = serial.Serial("COM" + comport_number, timeout = 5)
loadbox = visa_wrappers.visa_wrapper_serial(port)

loadbox.write("CALibrate:BOArdrev Steve's LoadBox")
loadbox.write("CALibrate:DATE 2039/1/17")
loadbox.write("CALibrate:OUTPut:CURRent:DAC:RANGe:HI 65535")
loadbox.write("CALibrate:OUTPut:CURRent:VALue:RANGe:HI 2.4998A")
loadbox.write("CALibrate:OUTPut:CURRent:DAC:RANGe:LO 65535")
loadbox.write("CALibrate:OUTPut:CURRent:VALue:RANGe:LO 24.99962mA")
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

raw_input("Hit enter to mess with chceksums or ^c to quit")

loadbox.write("CALibrate:BOArdrev Steve's LoadBox")
loadbox.write("CALibrate:DATE 2040/1/17")
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

