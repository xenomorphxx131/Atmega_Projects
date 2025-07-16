from PyICe import lab_interfaces
from datetime import datetime
import time

int_factory = lab_interfaces.interface_factory()
sensor = int_factory.get_visa_serial_interface("COM" + input("Enter COM Port Number: COM"), timeout=1)

print(f"DETECTION_THRESHOLD_MM: {sensor.ask('GET:DETECTION_THRESHOLD_MM?')}mm")
print("Starting Blackbox...")
sensor.write('RECORD')
time.sleep(5)

reset_time = datetime.now()
print(f"\nBegin Water Monitoring Program at {datetime.now()} .....\n")
while True:
    if sensor.ask('water:state?') == "1":
        current_time = datetime.now()
        print(f" Water ON at: {current_time}, contiguous time with no trips: {current_time - reset_time}")
        while sensor.ask('water:state?') == "1":
            reset_time = datetime.now()
        print(f" Water OFF {datetime.now()}\n")
        
        print(sensor.ask('GET:BLACKBOX?'))
        print()
        print()
        print()