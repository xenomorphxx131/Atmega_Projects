from PyICe import lab_interfaces
from datetime import datetime

int_factory = lab_interfaces.interface_factory()
sensor = int_factory.get_visa_serial_interface("COM" + input("Enter COM Port Number: COM"), timeout=1)

print(f"\nBegin Water Monitoring Program at {datetime.now()} .....\n")

reset_time = datetime.now()

while True:
    if sensor.ask('water:state?') == "1":
        current_time = datetime.now()
        print(f" Water ON  {current_time}, contiguous time with no trips: {current_time - reset_time}")
        while sensor.ask('water:state?') == "1":
            reset_time = datetime.now()
        print(f" Water OFF {datetime.now()}")
