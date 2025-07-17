from PyICe import lab_interfaces
from datetime import datetime

int_factory = lab_interfaces.interface_factory()
sensor = int_factory.get_visa_serial_interface("COM" + input("Enter COM Port Number: COM"), timeout=1)

if input("Program Device?") in ["Y", "y"]:
    sensor.write('SET:IIR_ALPHA -1.7')                              # Slightly faster dual pole IIR Filter
    sensor.write('SET:IIR_BETA 0.72')                               # but also slightly more high frequency noise
    # sensor.write('SET:IIR_ALPHA -1.8')                            # Slightly slower dual pole IIR Filter
    # sensor.write('SET:IIR_BETA 0.81')                             # but also slightly less high frequency noise
    sensor.write('SET:LASERPOWER 1')                                # Not using the ALS anymore. This is plenty bright
    sensor.write('SET:DETECTION_THRESHOLD_MM 2')                    # Trying to keep to 2mm. increase only if necessary for false trips.
    sensor.write('SET:MAX_DISTANCE_LEAKAGE 0.00333')                # Adjusts the MAX downward tracking leakage. Determines how long a foor can remain wihtout being calibrated away.
    sensor.write('SET:MAX_DISTANCE_MM_RESET_RATE 0.025')            # Adjusts the MAX upward tracking rate. Each cycle adjusts the max by this percentage times the current difference from the max.
    sensor.write(f'SET:WATER_DEBOUNCE_TIMEOUT {round(2/0.001024)}') # This is the water dwell time after a foot is removed in 1.024ms steps. There will also be a natural dwell from filtering as well.

print(f"RANGE: {sensor.ask('GET:RANGE?')}mm")
print(f"IIR_ALPHA: {sensor.ask('GET:IIR_ALPHA?')}")
print(f"IIR_BETA: {sensor.ask('GET:IIR_BETA?')}")
print(f"IIR_GAIN: {sensor.ask('GET:IIR_GAIN?')}")
print(f"LASERPOWER: {sensor.ask('GET:LASERPOWER?')}")
print(f"DETECTION_THRESHOLD_MM: {sensor.ask('GET:DETECTION_THRESHOLD_MM?')}mm")
print(f"MAX_DISTANCE_LEAKAGE: {sensor.ask('GET:MAX_DISTANCE_LEAKAGE?')}")
print(f"MAX_DISTANCE_MM_RESET_RATE: {sensor.ask('GET:MAX_DISTANCE_MM_RESET_RATE?')}")
print(f"WATER_DEBOUNCE_TIMEOUT: {sensor.ask('GET:WATER_DEBOUNCE_TIMEOUT?')}")