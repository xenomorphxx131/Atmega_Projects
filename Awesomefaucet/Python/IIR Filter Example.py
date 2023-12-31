

iir = 235

data = 2
data_in = 113

SCALEFACTOR = 655536

for loop in range(250):
    print(data, data/SCALEFACTOR)
    data = int((data*iir + data_in*SCALEFACTOR*(256-iir)) / 256)
