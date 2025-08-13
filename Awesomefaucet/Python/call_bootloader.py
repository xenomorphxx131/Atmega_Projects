import serial, time
port = input("Comport: COM")
foo = serial.Serial("COM{}".format(port))
foo.write(b"SYSTem:RST:BTLOader;")
foo.close()
time.sleep(7)