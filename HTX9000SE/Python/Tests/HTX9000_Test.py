#| -*- coding: utf-8 -*-
from PyICe import lab_core
import time, datetime, serial

# comport_number = raw_input("Enter COM Port Number: COM")
# port = serial.Serial("COM" + comport_number, timeout = 5)
# loadbox = visa_wrappers.visa_wrapper_serial(port)

master  = lab_core.master()
loadbox = master.get_visa_serial_interface("COM" + input("Enter HTX9000 COM Port Number: COM"), timeout=10) 
start_time = time.time()

def print_banner(title):
    print(u"\u250c" + u"\u2500" * 70 + u"\u2510")
    print(u"\u2502" + " " * 20 + title + " " * (50 - len(title)) + u"\u2502")
    print(u"\u2514" + u"\u2500" * 70 + u"\u2518")
#------------------------------------------------------------------
#|                 *OPC?                                          |
#------------------------------------------------------------------
print_banner("*OPC?")
test = "*OPC?"
result = loadbox.ask(test)
print(f"{test: <50}{result: <50}")
#------------------------------------------------------------------
#|                 [SYSTem:]VERSion?                              |
#------------------------------------------------------------------
print_banner("[SYSTem:]VERSion?")
test = ":SYSTem:VERSion?"
result = loadbox.ask(test)
print(f"{test: <50}{result: <50}")
test = "SYSTem:VERSion?"
result = loadbox.ask(test)
print(f"{test: <50}{result: <50}")
test = "VERSion?"
result = loadbox.ask(test)
print(f"{test: <50}{result: <50}")
test = ":SYST:VERS?"
result = loadbox.ask(test)
print(f"{test: <50}{result: <50}")
test = "SYST:VERS?"
result = loadbox.ask(test)
print(f"{test: <50}{result: <50}")
test = "VERS?"
result = loadbox.ask(test)
print(f"{test: <50}{result: <50}")
#------------------------------------------------------------------
#|                 [SYSTem:]ERRor?                                |
#------------------------------------------------------------------
print_banner("[SYSTem:]ERRor?")
test = ":SYSTem:ERRor?"
result = loadbox.ask(":SYSTem:ERRor?")
print(f"{test: <50}{result: <50}")
test = "SYSTem:ERRor?"
result = loadbox.ask("SYSTem:ERRor?")
print(f"{test: <50}{result: <50}")
test = "ERRor?"
result = loadbox.ask("ERRor?")
print(f"{test: <50}{result: <50}")
test = "TUNA? Melt"
loadbox.write(test)
result = loadbox.ask("ERRor?")
print(f"{test: <50}{result: <50}")
#------------------------------------------------------------------
#|                 [SOURce:]CURRent <val>                         |
#------------------------------------------------------------------
print_banner("[SOURce:]CURRent <val>")
test = "SOURce:CURRent 350nA"
loadbox.write(test)
result = loadbox.ask("SOURce:CURRent?")
print(f"{test: <50}{result: <50}")
test = "SOURce:CURRent 1uA"
loadbox.write(test)
result = loadbox.ask("SOURce:CURRent?")
print(f"{test: <50}{result: <50}")
test = "SOURce:CURRent 12uA"
loadbox.write(test)
result = loadbox.ask("SOURce:CURRent?")
print(f"{test: <50}{result: <50}")
test = "SOURce:CURRent 123uA"
loadbox.write(test)
result = loadbox.ask("SOURce:CURRent?")
print(f"{test: <50}{result: <50}")
test = "SOURce:CURRent 1.234mA"
loadbox.write(test)
result = loadbox.ask("SOURce:CURRent?")
print(f"{test: <50}{result: <50}")
test = "CURRent 12.345mA"
loadbox.write(test)
result = loadbox.ask("SOURce:CURRent?")
print(f"{test: <50}{result: <50}")
test = "CURRent 123.456mA"
loadbox.write(test)
result = loadbox.ask("SOURce:CURRent?")
print(f"{test: <50}{result: <50}")
test = "CURR 1234.56mA"
loadbox.write(test)
result = loadbox.ask("SOURce:CURRent?")
print(f"{test: <50}{result: <50}")
test = "CURR 1.23456A"
loadbox.write(test)
result = loadbox.ask("SOURce:CURRent?")
print(f"{test: <50}{result: <50}")
test = ":CURR 1e9nA"
loadbox.write(test)
result = loadbox.ask("SOURce:CURRent?")
print(f"{test: <50}{result: <50}")
#------------------------------------------------------------------
#|                 [SOURce:]CURRent:RANGe:LOw <val>               |
#------------------------------------------------------------------
print_banner("[SOURce:]CURRent:RANGe:LOw <val>")
test = "CURRent 1A"
loadbox.write(test)
result = loadbox.ask("SOURce:CURRent?")
print(f"{test: <50}{result: <50}")
test = "SOURce:CURRent:RANGe?"
result = loadbox.ask(test)
print(f"{test: <50}{result: <50}")
test = "SOURce:CURRent:RANGe:LOw 1.12A"
loadbox.write(test)
result = loadbox.ask("SOURce:CURRent?")
print(f"{test: <50}{result: <50}")
test = "SOURce:CURRent:RANGe?"
result = loadbox.ask(test)
print(f"{test: <50}{result: <50}")
test = "CURRent 1A"
loadbox.write(test)
result = loadbox.ask("SOURce:CURRent?")
print(f"{test: <50}{result: <50}")
test = "SOURce:CURRent:RANGe?"
result = loadbox.ask(test)
print(f"{test: <50}{result: <50}")
test = "SOURce:CURRent:RANGe:LOw 100uA"
loadbox.write(test)
result = loadbox.ask("SOURce:CURRent?")
print(f"{test: <50}{result: <50}")
test = "SOURce:CURRent:RANGe?"
result = loadbox.ask(test)
print(f"{test: <50}{result: <50}")
#------------------------------------------------------------------
#|                 [SOURce:]CURRent:RANGe:HIgh <val>              |
#------------------------------------------------------------------
print_banner("[SOURce:]CURRent:RANGe:HIgh <val>")
test = "CURRent 300nA"
loadbox.write(test)
result = loadbox.ask("SOURce:CURRent?")
print(f"{test: <50}{result: <50}")
test = "SOURce:CURRent:RANGe?"
result = loadbox.ask(test)
print(f"{test: <50}{result: <50}")
test = "SOURce:CURRent:RANGe:HIgh 50uA"
loadbox.write(test)
result = loadbox.ask("SOURce:CURRent?")
print(f"{test: <50}{result: <50}")
test = "SOURce:CURRent:RANGe?"
result = loadbox.ask(test)
print(f"{test: <50}{result: <50}")
test = "CURRent 100uA"
loadbox.write(test)
result = loadbox.ask("SOURce:CURRent?")
print(f"{test: <50}{result: <50}")
test = "SOURce:CURRent:RANGe?"
result = loadbox.ask(test)
print(f"{test: <50}{result: <50}")
test = "SOURce:CURRent:RANGe:HIgh 100uA"
loadbox.write(test)
result = loadbox.ask("SOURce:CURRent?")
print(f"{test: <50}{result: <50}")
test = "SOURce:CURRent:RANGe?"
result = loadbox.ask(test)
print(f"{test: <50}{result: <50}")
#------------------------------------------------------------------
#|                 TEMP:HEATsink?                                 |
#------------------------------------------------------------------
print_banner("Temperatures")
test = "TEMP:HEATsink?"
result = loadbox.ask(test)
print(f"{test: <50}{result: <50}")

test = "TEMP:BOARD?"
result = loadbox.ask(test)
print(f"{test: <50}{result: <50}")
#------------------------------------------------------------------
#|                 DROPout?                                       |
#------------------------------------------------------------------
print_banner("DROPout?")
test = "CURRent 300mA"
loadbox.write(test)
result = loadbox.ask("SOURce:CURRent?")
print(f"{test: <50}{result: <50}")
test = "DROPout?"
result = loadbox.ask(test)
print(f"{test: <50}{result: <50}")
test = "CURRent 0"
loadbox.write(test)
result = loadbox.ask("SOURce:CURRent?")
print(f"{test: <50}{result: <50}")
test = "DROPout?"
result = loadbox.ask(test)
print(f"{test: <50}{result: <50}")
#------------------------------------------------------------------
#|                 CALibrate:BOArdrev                             |
#------------------------------------------------------------------
print_banner("CALibrate:BOArdrev")
test = "Get old BOArdrev"
result = loadbox.ask("*IDN?")
oldvalue = result.split(",")[2].strip()
print(f"{test: <50}{oldvalue: <50}")
test = "CALibrate:BOArdrev "
value = "Steve's Board"
loadbox.write(test + value)
# result = loadbox.ask(test)
print(f"{test: <50}{value: <50}")
test = "*IDN?"
result = loadbox.ask(test)
print(f"{test: <50}{result: <50}")
test = "Restore old BOArdrev"
result = oldvalue
print(f"{test: <50}{result: <50}")
test = "CALibrate:BOArdrev "
loadbox.write(test + oldvalue)
result = oldvalue
print(f"{test: <50}{result: <50}")
test = "*IDN?"
result = loadbox.ask(test)
print(f"{test: <50}{result: <50}")
#------------------------------------------------------------------
print_banner("Get Old cal values")
#------------------------------------------------------------------
test = "CALibrate:DATE?"
cal_date = loadbox.ask(test)
result = cal_date
print(f"{test: <50}{result: <50}")

test = "CALibrate:OUTPut:CURRent:DAC:RANGe:HIgh?"
dac_high = loadbox.ask(test)
result = dac_high
print(f"{test: <50}{result: <50}")

test = "CALibrate:OUTPut:CURRent:VAL:RANGe:HIgh?"
val_high = loadbox.ask(test)
result = val_high
print(f"{test: <50}{result: <50}")

test = "CALibrate:OUTPut:CURRent:DAC:RANGe:LOw?"
dac_low = loadbox.ask(test)
result = dac_low
print(f"{test: <50}{result: <50}")

test = "CALibrate:OUTPut:CURRent:VAL:RANGe:LOw?"
val_low = loadbox.ask(test)
result = val_low
print(f"{test: <50}{result: <50}")

test = "CALibrate:CHECKsum:HIgh?"
checksum_high = loadbox.ask(test)
result = checksum_high
print(f"{test: <50}{result: <50}")

test = "CALibrate:CHECKsum:LOw?"
checksum_low = loadbox.ask(test)
result = checksum_low
print(f"{test: <50}{result: <50}")

#------------------------------------------------------------------
print_banner("Set new cal values")
#------------------------------------------------------------------
now = datetime.datetime.now()
test = f"CALibrate:DATE {datetime.datetime.now().strftime('%Y-%m-%d')}"
result = loadbox.write(test)
print(f"{test: <50}")

test = "CALibrate:OUTPut:CURRent:DAC:RANGe:HIgh 123456"
result = loadbox.write(test)
print(f"{test: <50}")

test = "CALibrate:OUTPut:CURRent:VAL:RANGe:HIgh 1.2345A"
result = loadbox.write(test)
print(f"{test: <50}")

test = "CALibrate:OUTPut:CURRent:DAC:RANGe:LOw 654321"
result = loadbox.write(test)
print(f"{test: <50}")

test = "CALibrate:OUTPut:CURRent:VAL:RANGe:LOw 5.4321mA"
result = loadbox.write(test)
print(f"{test: <50}")

#------------------------------------------------------------------
print_banner("new cal values")
#------------------------------------------------------------------
test = "CALibrate:DATE?"
result = loadbox.ask(test)
print(f"{test: <50}{result: <50}")

test = "CALibrate:OUTPut:CURRent:DAC:RANGe:HIgh?"
result = loadbox.ask(test)
print(f"{test: <50}{result: <50}")

test = "CALibrate:OUTPut:CURRent:VAL:RANGe:HIgh?"
result = loadbox.ask(test)
print(f"{test: <50}{result: <50}")

test = "CALibrate:OUTPut:CURRent:DAC:RANGe:LOw?"
result = loadbox.ask(test)
print(f"{test: <50}{result: <50}")

test = "CALibrate:OUTPut:CURRent:VAL:RANGe:LOw?"
result = loadbox.ask(test)
print(f"{test: <50}{result: <50}")

test = "CALibrate:CHECKsum:HIgh?"
result = loadbox.ask(test)
print(f"{test: <50}{result: <50}")

test = "CALibrate:CHECKsum:LOw?"
result = loadbox.ask(test)
print(f"{test: <50}{result: <50}")

#------------------------------------------------------------------
print_banner("Restore old cal values")
#------------------------------------------------------------------
test = "CALibrate:DATE " + cal_date
result = loadbox.write(test)
print(f"{test: <50}")

test = "CALibrate:OUTPut:CURRent:DAC:RANGe:HIgh " + str(int(dac_high, 16))
result = loadbox.write(test)
print(f"{test: <50}")

test = "CALibrate:OUTPut:CURRent:VAL:RANGe:HIgh " + val_high
result = loadbox.write(test)
print(f"{test: <50}")

test = "CALibrate:OUTPut:CURRent:DAC:RANGe:LOw " + str(int(dac_low, 16))
result = loadbox.write(test)
print(f"{test: <50}")

test = "CALibrate:OUTPut:CURRent:VAL:RANGe:LOw " + val_low
result = loadbox.write(test)
print(f"{test: <50}")

#------------------------------------------------------------------
print_banner("Restored cal values")
#------------------------------------------------------------------
test = "CALibrate:DATE?"
result = loadbox.ask(test)
print(f"{test: <50}{result: <50}")

test = "CALibrate:OUTPut:CURRent:DAC:RANGe:HIgh?"
result = loadbox.ask(test)
print(f"{test: <50}{result: <50}")

test = "CALibrate:OUTPut:CURRent:VAL:RANGe:HIgh?"
result = loadbox.ask(test)
print(f"{test: <50}{result: <50}")

test = "CALibrate:OUTPut:CURRent:DAC:RANGe:LOw?"
result = loadbox.ask(test)
print(f"{test: <50}{result: <50}")

test = "CALibrate:OUTPut:CURRent:VAL:RANGe:LOw?"
result = loadbox.ask(test)
print(f"{test: <50}{result: <50}")

test = "CALibrate:CHECKsum:HIgh?"
result = loadbox.ask(test)
print(f"{test: <50}{result: <50}")

test = "CALibrate:CHECKsum:LOw?"
result = loadbox.ask(test)
print(f"{test: <50}{result: <50}")

#------------------------------------------------------------------
print_banner("Locking Screen for 5 Seconds.....")
#------------------------------------------------------------------
test = "SYSTem:LOCK"
result = loadbox.write(test)
print(f"{test: <50}")
lock_time = 5
time.sleep(lock_time)
test = "SYSTem:LOCK:RELease"
result = loadbox.write(test)
print(f"{test: <50}")
#------------------------------------------------------------------
print_banner("Setting Current")
#------------------------------------------------------------------
test = "SOURce:CURRent 1.234567A"
loadbox.write(test)
result = loadbox.ask("SOURce:CURRent?")
print(f"{test: <50}{result: <50}")
#------------------------------------------------------------------
print_banner("Enabling Fan")
#------------------------------------------------------------------
test = "FAN:ON"
loadbox.write(test)
result = loadbox.ask("FAN?")
print(f"{test: <50}{result: <50}")
time.sleep(5)
#------------------------------------------------------------------
print_banner("Disabling Fan")
#------------------------------------------------------------------
test = "FAN:OFF"
loadbox.write(test)
result = loadbox.ask("FAN?")
print(f"{test: <50}{result: <50}")
#------------------------------------------------------------------
#|                 Test Time                                       |
#------------------------------------------------------------------
print_banner("Test Time")
test = "Test Time"
result = time.time() - start_time - lock_time
print(f"{test: <50}{result: <50}")

loadbox.write("SOURce:CURRent 0A")
input("\n\nHit Enter to Quit")











