import numpy as np
import matplotlib.pyplot as plt

# Signal parameters
mean = 100
std_dev = 1
samples = 250

# Generate noisy, quantized signal
np.random.seed(4)  # For reproducibility
quantized_signal = np.concatenate([(mean/2)*np.ones(50), np.round(np.random.normal(loc=mean, scale=std_dev, size=samples))])

# Simplified 2-pole feedback-only IIR filter

#          α(1) β(2)   
a = [1.0, -1.8, 0.81]   # Slower, slightly less noise
b = [1.0, -1.7, 0.72]   # Faster, slightly more noise
ya = [mean/2, mean/2, mean/2]    # Output history
yb = [mean/2, mean/2, mean/2]    # Output history

def update_output_a(sample):
    ya[2], ya[1] = ya[1], ya[0]
    ya[0] = sample / 100 - a[1]*ya[1] - a[2]*ya[2]
    return ya[0]
    
def update_output_b(sample):
    yb[2], yb[1] = yb[1], yb[0]
    yb[0] = sample / 50 - b[1]*yb[1] - b[2]*yb[2]
    return yb[0]

def compute_single_pole():
    running_value = mean/2
    one_pole_values = [mean/2]
    for point in quantized_signal:
        running_value = 2000/65535 * point + (65535-2000) / 65535 * running_value
        one_pole_values.append(running_value)
    return one_pole_values

one_pole_values = compute_single_pole()
output_signal_a = [update_output_a(s) for s in quantized_signal]
output_signal_b = [update_output_b(s) for s in quantized_signal]

plt.plot(quantized_signal, label='Input')
plt.plot(output_signal_a, label='Filtered A', linewidth=2)
plt.plot(output_signal_b, label='Filtered B', linewidth=2)
plt.plot(one_pole_values, label='Single Pole', linewidth=2)
plt.legend()
plt.grid(True)
plt.xlabel('Time [s]')
plt.ylabel('Amplitude')
plt.title('2-Pole Butterworth Low-Pass Filter (50 Hz cutoff)')
plt.ylim(50, 104)
plt.show()