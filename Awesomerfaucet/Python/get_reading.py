from PyICe import lab_interfaces

int_factory = lab_interfaces.interface_factory()
sensor = int_factory.get_visa_serial_interface("COM" + input("Enter COM Port Number: COM"), timeout=1)

while True:
    print(f"{float(sensor.ask('get:range?')[:-2]):0.1f}mm", end="\r")