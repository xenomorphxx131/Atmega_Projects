from PyICe import lab_interfaces
import matplotlib.pyplot as plt
import numpy as np

record = input("Restart Recording? [Y/N]")
int_factory = lab_interfaces.interface_factory()
sensor = int_factory.get_visa_serial_interface("COM" + input("Enter COM Port Number: COM"), timeout=1)
if record in ["Y", "y"]:
    sensor.write('SYSTem:RECORD')
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