from PyICe import lab_interfaces
import statistics

int_factory = lab_interfaces.interface_factory()
sensor = int_factory.get_visa_serial_interface("COM" + input("Enter COM Port Number: COM"), timeout=1)

readings = []
for reading in range(10000):
    readings.append(float(sensor.ask('get:range?')[:-2]))

def remove_adjacent_duplicates(lst):
    if not lst:
        return []

    result = [lst[0]]
    for val in lst[1:]:
        if val != result[-1]:
            result.append(val)
    return result

results = remove_adjacent_duplicates(readings)

statistics.stdev(results)
print(f"σ = {statistics.stdev(results)}\n")
print(results)

# while True:
    # print(f"{float(sensor.ask('get:range?')[:-2]):0.1f}mm", end="\r")