import serial
import sys
sys.path.append("../../../../PyICe")
import visa_wrappers
import time

comport_number = raw_input("Enter COM Port Number: COM")
port = serial.Serial("COM" + comport_number, timeout = 5)
loadbox = visa_wrappers.visa_wrapper_serial(port)

print "Sending: SYSTem:LOCK:RELease"
loadbox.write("SYSTem:LOCK:RELease")