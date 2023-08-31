import serial, time
port = raw_input("Comport: COM")
foo = serial.Serial("COM{}".format(port))
foo.write("System:rst:btlo;")
foo.close()
time.sleep(7)
# raw_input("Hit enter once Atmel device appears in device manager...")

