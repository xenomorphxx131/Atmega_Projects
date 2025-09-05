import datetime, time
from PyICe import lab_core, lab_instruments

##################################################################################################
#   Print Banner                                                                                 #
##################################################################################################
def print_banner(title):
    print(u"\u250c" + u"\u2500" * 70 + u"\u2510")
    print(u"\u2502" + " " * 20 + title + " " * (50 - len(title)) + u"\u2502")
    print(u"\u2514" + u"\u2500" * 70 + u"\u2518")
    
print_banner("Connecting Equipment...")
print("Connect load box to low voltage lab_instruments supply. ")
HTX9000_comport_number = input("Enter HTX9000 COM Port Number: COM")
A34401_comport_number = input("Enter A34401 COM Port Number: COM")
now = datetime.datetime.now()                   
master = lab_core.master()
print("Connecting to ammeter (Agilent 34401A) on COM{}...".format(A34401_comport_number))
a34401a = lab_instruments.agilent_34401a(master.get_visa_serial_interface("COM"+A34401_comport_number, baudrate = 9600, dsrdtr = True, timeout = 1))
a34401a.config_dc_current()
a34401a.add_channel("iin_meas")
print("Successfully connected to ammeter on COM{}".format(A34401_comport_number))
loadbox=master.get_visa_serial_interface("COM" + HTX9000_comport_number, timeout=1)

print_banner("Calibrating...")
print_banner("  Old Values")
print("*IDN?:", end=' ')
print(loadbox.ask("*IDN?"))
print("CAL DATE:               ", end=' ')
print(loadbox.ask("CALibrate:DATE?"))
print("CAL DAC HIGH SETTING:   ", end=' ')
print(loadbox.ask("CALibrate:OUTPut:CURRent:DAC:RANGe:HIgh?"))
print("CAL VALUE HIGH SETTING: ", end=' ')
print(loadbox.ask("CALibrate:OUTPut:CURRent:VALue:RANGe:HIgh?"))
print("CAL DAC LOW SETTING:    ", end=' ')
print(loadbox.ask("CALibrate:OUTPut:CURRent:DAC:RANGe:LOw?"))
print("CAL VALUE LOW SETTING:  ", end=' ')
print(loadbox.ask("CALibrate:OUTPut:CURRent:VALue:RANGe:LOw?"))
print("CAL LOW CHECKSUM:       ", end=' ')
print(loadbox.ask("CALibrate:CHECKsum:LOw?"))
print("CAL HIGH CHECKSUM:      ", end=' ')
print(loadbox.ask("CALibrate:CHECKsum:HIgh?"))

a34401a.config_dc_current(NPLC=10, range=3)
loadbox.write(f"CALibrate:DATE {now.strftime('%Y-%m-%d')}")
loadbox.write("CALibrate:OUTPut:CURRent:DAC:RANGe:HI 65535")
time.sleep(1);                                      #wait for things to settle
measured_current = a34401a.read_channel("iin_meas")
loadbox.write(f"CALibrate:OUTPut:CURRent:VALue:RANGe:HI {measured_current:2.4f}A")
a34401a.config_dc_current(NPLC=10, range=0.1)
loadbox.write("CALibrate:OUTPut:CURRent:DAC:RANGe:LO 65535")
time.sleep(1);
measured_current = a34401a.read_channel("iin_meas") * 1000
loadbox.write(f"CALibrate:OUTPut:CURRent:VALue:RANGe:LO {measured_current:2.4f}mA")

print_banner("  New Values")

print("*IDN?:", end=' ')
print(loadbox.ask("*IDN?"))
print("CAL DATE:               ", end=' ')
print(loadbox.ask("CALibrate:DATE?"))
print("CAL DAC HIGH SETTING:   ", end=' ')
print(loadbox.ask("CALibrate:OUTPut:CURRent:DAC:RANGe:HIgh?"))
print("CAL VALUE HIGH SETTING: ", end=' ')
value_hi=loadbox.ask("CALibrate:OUTPut:CURRent:VALue:RANGe:HIgh?")
print(value_hi)
print("CAL DAC LOW SETTING:    ", end=' ')
print(loadbox.ask("CALibrate:OUTPut:CURRent:DAC:RANGe:LOw?"))
print("CAL VALUE LOW SETTING:  ", end=' ')
value_low=loadbox.ask("CALibrate:OUTPut:CURRent:VALue:RANGe:LOw?")
print(value_low)
print("CAL LOW CHECKSUM:       ", end=' ')
print(loadbox.ask("CALibrate:CHECKsum:LOw?"))
print("CAL HIGH CHECKSUM:      ", end=' ')
print(loadbox.ask("CALibrate:CHECKsum:HIgh?"))

print_banner("Checking Calibration...")
a34401a.config_dc_current(NPLC=10, range=0.1)
loadbox.write("CURRent 0") # get into low range
loadbox.write("CURRent 24.5mA")
time.sleep(0.2)
measured_current_lo = float(a34401a.read_channel("iin_meas"))
print(f"\nError in low mode: {(measured_current_lo/0.0245-1)*100:0.2f}%")

a34401a.config_dc_current(NPLC=10, range=3)
loadbox.write("CURRent 2.45A")
time.sleep(0.2)
measured_current_hi = float(a34401a.read_channel("iin_meas"))
print(f"\nError in high mode: {(measured_current_hi/2.45-1)*100:0.2f}%")

loadbox.write("CURRent 0A")

