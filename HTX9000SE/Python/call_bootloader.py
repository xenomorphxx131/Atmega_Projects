import serial, time

foo = serial.Serial(f"COM{input('Comport: COM')}")
foo.write(b"SYSTem:RST:BTLOader;")
foo.close()
time.sleep(7)