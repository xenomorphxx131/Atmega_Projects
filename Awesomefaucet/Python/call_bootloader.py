import serial, time
port = input("Comport: COM")
foo = serial.Serial("COM{}".format(port))
foo.write(b"System:rst:btlo;")
foo.close()
time.sleep(7)