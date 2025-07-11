from PyICe import lab_interfaces
from datetime import datetime

int_factory = lab_interfaces.interface_factory()
sensor = int_factory.get_visa_serial_interface("COM" + input("Enter COM Port Number: COM"), timeout=1)

if input("Program Device?") == "Y":
    sensor.write('STORe:IIR_ALPHA -1.8')
    sensor.write('STORe:IIR_BETA 0.81')
    sensor.write('STORe:LASERPOWER 3')
    sensor.write('STORe:DETECTION_THRESHOLD_MM 3')
    sensor.write('STORe:MAX_DISTANCE_LEAKAGE 0.001666')
    sensor.write(f'STORe:WATER_DEBOUNCE_TIMEOUT {round(2/0.001024)}')




print(f"RANGE: {sensor.ask('GET:RANGE?')}mm")
print(f"IIR_ALPHA: {sensor.ask('GET:IIR_ALPHA?')}")
print(f"IIR_BETA: {sensor.ask('GET:IIR_BETA?')}")
print(f"IIR_GAIN: {sensor.ask('GET:IIR_GAIN?')}")
print(f"LASERPOWER: {sensor.ask('GET:LASERPOWER?')}")
print(f"DETECTION_THRESHOLD_MM: {sensor.ask('GET:DETECTION_THRESHOLD_MM?')}mm")
print(f"MAX_DISTANCE_LEAKAGE: {sensor.ask('GET:MAX_DISTANCE_LEAKAGE?')}")
print(f"WATER_DEBOUNCE_TIMEOUT: {sensor.ask('GET:WATER_DEBOUNCE_TIMEOUT?')}")