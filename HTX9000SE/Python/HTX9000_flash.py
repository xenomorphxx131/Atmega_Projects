import time, os, subprocess, serial, sys
from PyICe import lab_core, visa_wrappers

##################################################################################################
#   Print Banner                                                                                 #
##################################################################################################
def print_banner(title):
    print(u"\u250c" + u"\u2500" * 70 + u"\u2510")
    print(u"\u2502" + " " * 20 + title + " " * (50 - len(title)) + u"\u2502")
    print(u"\u2514" + u"\u2500" * 70 + u"\u2518")
    
MCU     = 'atmega32u4'
TARGET  = '../src/USB_Load_Box'

print_banner("Flashing HTX9000 Load Box with Latest Firmware....")
comport_number = input("Enter COM port number from the device manager *** Hit [ENTER] for a fresh unit *** COM")
freshunit = comport_number == ""
serialize = True

if not freshunit:
    master = lab_core.master()
    loadbox = master.get_visa_serial_interface("COM" + comport_number, timeout=10)
    ident = loadbox.ask("*IDN?")
    serialnum = ident.split(",")[2].strip()
    serialize = len(serialnum) != 10 # ten digit serial number plan developed May 23, 2020
    if not serialize:
        print_banner("Keeping current 10 digit serial number")
        print(f"{serialnum}\n\n")
    print("Starting Bootloader, Expect Timeout...")
    loadbox.write("SYSTem:RST:BTLOader")
    print("Begin Reprogramming Procedure...")
    time.sleep(5)

COMMAND1 = ["batchisp", "-hardware", "usb", "-device", MCU, "-operation", "erase", "f"]
COMMAND2 = ["batchisp", "-hardware", "usb", "-device", MCU, "-operation", "loadbuffer",  f"{TARGET}.hex", "program"]
COMMAND3 = ["batchisp", "-hardware", "usb", "-device", MCU, "-operation", "start", "reset", "0"]

try:
    print_banner("Erasing Unit....")
    erase = subprocess.run(COMMAND1, capture_output=True)
    if erase.returncode:
        print("Failed to erase unit, no more info available :(")
        exit()
    elif erase.stderr.decode(sys.stderr.encoding).strip() != "":
        print("Failed to erase unit, no more info available :(")
        exit()
    else:
        print(erase.stdout.decode(sys.stdout.encoding).strip())
    print_banner("Flashing Unit....")
    flash = subprocess.run(COMMAND2, capture_output=True)
    if flash.returncode:
        print("Failed to flash unit, it erased ok, no more info available :(")
        exit()
    elif flash.stderr.decode(sys.stderr.encoding).strip() != "":
        print("Failed to flash unit, it erased ok, no more info available :(")
        exit()
    else:
        print(flash.stdout.decode(sys.stdout.encoding).strip())
    print_banner("Restarting Unit....")
    reset = subprocess.run(COMMAND3, capture_output=True)
    if reset.returncode:
        print("Failed to reset unit, it erased and flashed ok, no more info available :(")
        exit()
    elif reset.stderr.decode(sys.stderr.encoding).strip() != "":
        print("Failed to reset unit, it erased and flashed ok, no more info available :(")
        exit()
    else:
        print(reset.stdout.decode(sys.stdout.encoding).strip())
except:
    print("\n\nConfirm that this computer has Flip Installed\n\n")


time.sleep(10)                                      # Wait for unit to boot up
os.system('cls')                                    # Windows only
print_banner("HTX9000SE Programming Complete!")     # Fresh banner
master = lab_core.master()                          # Get a fresh master
serialnum = f"{time.time():10.0f}"                  # May be discarded

if freshunit:
    comport_number = input("(Enter new COM port number from the device manager: COM")
    loadbox = master.get_visa_serial_interface("COM" + comport_number, timeout=10)
    print(f"The serial number of this unit will be: {serialnum}")
    loadbox.write(str(f"CALibrate:BOArdrev {serialnum}"))
    print(f'\n\n Response to *idn?: {loadbox.ask("*idn?")}')
else:
    loadbox = master.get_visa_serial_interface("COM" + comport_number, timeout=10)
    if serialize:
        loadbox = master.get_visa_serial_interface("COM" + comport_number, timeout=10)
        print(f"The serial number of this unit remains as is at: {serialnum}")
        loadbox.write(str(f"CALibrate:BOArdrev {serialnum}"))
        print(f'\n\n Response to *idn?: {loadbox.ask("*idn?")}')

print_banner("Response to SYSTem:VERSion?")
result = loadbox.ask("SYSTem:VERSion?")
print("'{}\'".format(result))

print_banner("Unit Serial Number from *IDN?")
ident = loadbox.ask("*IDN?")
serialnum = ident.split(",")[2].strip()
print(f"Serial#: {serialnum}")   

input("\nHit Enter to Quit")






