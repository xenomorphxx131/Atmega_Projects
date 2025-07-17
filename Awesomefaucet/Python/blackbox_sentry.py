from PyICe import lab_interfaces
from datetime import datetime
import matplotlib.pyplot as plt
import numpy as np
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
        blackbox = eval(sensor.ask('GET:BLACKBOX?'))
        plt.plot(blackbox["DISTANCE_mm"], label='"DISTANCE_mm"', linewidth=2)
        plt.plot(blackbox["MAX_DISTANCE_mm"], label='"MAX_DISTANCE_mm"', linewidth=2)
        plt.legend()
        plt.xlabel('Time [0.5s/div]')
        plt.autoscale(enable=True, axis='y')
        ymin, ymax = plt.ylim()
        ymin = int(np.floor(ymin))
        ymax = int(np.ceil(ymax))
        ytick_values = np.linspace(ymin, ymax, 5)
        ytick_values = np.round(ytick_values).astype(int)
        plt.yticks(ytick_values)
        plt.grid(True)
        plt.show()
        sensor.write('RECORD')
        print(f"Restarting Water Monitoring Program at {datetime.now()} .....\n")