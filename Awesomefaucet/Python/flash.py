import serial, time, sys, os, subprocess

os.system('cls')        #WINDOWS
#os.system('clear')     #LINUX
##################################################################################################
#   Print Banner                                                                                 #
##################################################################################################
def print_banner(title):
    print(u"\u250c" + u"\u2500" * 70 + u"\u2510")
    print(u"\u2502" + " " * 3 + title + " " * (67 - len(title)) + u"\u2502")
    print(u"\u2514" + u"\u2500" * 70 + u"\u2518")
    
MCU = 'atmega32u4'
TARGET = '../src/Awesomefaucet'
print_banner("Flashing Board with Latest Firmware....")
comport_number = input("Enter COM Port Number, hit [ENTER] for fresh unit: COM")
if comport_number != "":
    virginboard = False
    port = serial.Serial("COM{}".format(comport_number), timeout = 5)
    print_banner("Starting Bootloader, Expect Timeout...")
    port.write(b"SYSTem:RST:BTLOader;")
    port.close()
    time.sleep(7)
else:
    virginboard = True

print_banner("Begin Reprogramming Procedure...")
COMMAND1 = "batchisp -hardware usb -device {} -operation erase f".format(MCU)
COMMAND2 = "batchisp -hardware usb -device {} -operation loadbuffer {}.hex program".format(MCU, TARGET)
COMMAND3 = "batchisp -hardware usb -device {} -operation start reset 0".format(MCU)

erase = subprocess.Popen(COMMAND1)
while erase.poll() is None:
    time.sleep(0.01)

flash = subprocess.Popen(COMMAND2)
while flash.poll() is None:
    time.sleep(0.01)
 
reset = subprocess.Popen(COMMAND3)
while reset.poll() is None:
    time.sleep(0.01)
    
print_banner("Restarting Device....")

time.sleep(10)
os.system('cls')        #Windows only

if virginboard:
    comport_number = input("Enter COM Port Number that Appeared: COM")
port = serial.Serial("COM{}".format(comport_number), timeout = 4)
port.write(b"SYSTem:VERSion?")
response = port.read(100)
port.close()
print_banner("Firmware Upgrade Complete!")
print_banner("Response was: {}".format(response))
input("\nHit Enter to Quit")