import time
from PyICe import lab_core, lab_utils

lab_utils.print_banner("Assigning a serial number to HTX9000 Load Box....")
comport_number = input("Enter COM port number from the device manager: COM")
five_amp_version = input("Is this a modified HTX9000SE-5A [y/n]: ")
master = lab_core.master()
loadbox = master.get_visa_serial_interface("COM" + comport_number, timeout=10)
ident = loadbox.ask("*IDN?")
lab_utils.print_banner("Response to *IDN?")
print(f"'{ident}'")
serialnum = ident.split(",")[2].split("_")[0].strip()
if len(serialnum) == 10:# ten digit serial number plan developed May 23, 2020
    lab_utils.print_banner("Keeping current 10 digit serial number")
    print(f"{serialnum}\n\n")
    input("\nHit Enter to Quit")
    exit()

serialnum = f"{time.time():10.0f}"
serialnum = serialnum + "_SE5A" if "y" in five_amp_version else serialnum
print(f"The serial number of this unit will be: {serialnum}")
loadbox.write(f"CALibrate:BOArdrev {serialnum}")
lab_utils.print_banner("Response to SYSTem:VERSion?")
print(f'{loadbox.ask("SYSTem:VERSion?")}')
lab_utils.print_banner("Unit Serial Number from *IDN?")
ident = loadbox.ask("*IDN?")
serialnum = ident.split(",")[2].strip()
print(f"Serial#: {serialnum}")
input("\nHit Enter to Quit")
exit()






