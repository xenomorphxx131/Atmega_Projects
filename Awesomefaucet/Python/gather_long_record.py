from PyICe import lab_interfaces
import matplotlib.pyplot as plt
import numpy as np
import time

int_factory = lab_interfaces.interface_factory()
sensor = int_factory.get_visa_serial_interface("COM" + input("Enter COM Port Number: COM"), timeout=1)

readings = []
print("Collecting Readings, Please Wait...")
starttime = time.time()
for reading in range(5000):
    readings.append(eval(sensor.ask('get:RANGE_AND_MAXRANGE?')))
endtime = time.time()

def remove_adjacent_duplicates(lst):
    result = [lst[0]]
    for val in lst[1:]:
        if val["RANGE"] != result[-1]["RANGE"]:
            result.append(val)
    return result

results = remove_adjacent_duplicates(readings)
print(f"Total Samples: {len(results)}, Duration = {endtime-starttime}")
plt.plot([result["RANGE"] for result in results], label='"DISTANCE_mm"', linewidth=2)
plt.plot([result["MAXRANGE"] for result in results], label='"MAX DISTANCE_mm"', linewidth=2)
plt.legend()
plt.xlabel(f'{(endtime-starttime)/5:0.1f}s/DIV')
plt.ylabel('RANGE (mm)')
plt.tick_params(
    axis='both',
    which='both',
    labelbottom=False,       # Hide x tick labels
    labelleft=True)          # Hide y tick labels
plt.title('Raw Sensor Data')
xmin, xmax = plt.xlim()
plt.xticks(np.linspace(xmin, xmax, 6))
plt.autoscale(enable=True, axis='y')
ymin, ymax = plt.ylim()
ymin = int(np.floor(ymin))
ymax = int(np.ceil(ymax))
num_ticks = 5
ytick_values = np.linspace(ymin, ymax, 5)
ytick_values = np.round(ytick_values).astype(int)
plt.yticks(ytick_values)
plt.grid(True)
plt.show()