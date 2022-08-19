import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import sys 


DIM_X = int(sys.argv[1])
DIM_Y = int(sys.argv[2])
ALGORITHM = sys.argv[3]

temp_data = pd.read_csv("simulation/SystemTemperature.tsv", sep='\t')
temp_data = temp_data.to_numpy()

power_data = pd.read_csv("simulation/SystemPower.tsv", sep='\t')
power_data = power_data.to_numpy()

fit_data = pd.read_csv("simulation/FITlog.tsv", sep='\t')
fit_data = fit_data.to_numpy()

temp_avg = 0
temp_samples = 0

peak_avg = 0
peak_samples = 0

fit_avg = 0
fit_samples = 0

power_avg = 0
power_samples = 0

for i in range (len(temp_data)):
    peak = 0
    for j in range(len(temp_data[i])-1):
        temp_avg += temp_data[i][j+1]
        temp_samples += 1
        if temp_data[i][j+1] > peak:
            peak = temp_data[i][j+1]
    peak_avg += peak
    peak_samples += 1

peak_avg = peak_avg/peak_samples

temp_avg = temp_avg/temp_samples

for i in range (len(power_data)):
    for j in range(len(power_data[i])-1):
        power_avg += power_data[i][j+1]
        power_samples += 1

power_avg = power_avg/power_samples

for i in range (len(fit_data)):
    for j in range(len(fit_data[i])-1):
        fit_avg += fit_data[i][j+1]
        fit_samples += 1

fit_avg = fit_avg/fit_samples

with open("simulation/mttflog.txt", "r") as mttffile:
    line = mttffile.readline()
    line = mttffile.readline()
    splited = line.split(' ')
    mttf = splited[len(splited)-1]

with open("simulation/occupation.txt", "r") as occfile:
    line = occfile.readline()
    occ = float(line)

with open("simulation/0_log.csv", "w") as logfile:
    print(str(DIM_X*DIM_Y), file=logfile, end=';')
    print(ALGORITHM, file=logfile, end=';')
    print(str(occ).replace(".",","), file=logfile, end=';')
    print(str(power_avg).replace(".",","), file=logfile, end=';')
    print(str(temp_avg).replace(".",","), file=logfile, end=';')
    print(str(peak_avg).replace(".",","), file=logfile, end=';')
    print(str(fit_avg).replace(".",","), file=logfile, end=';')
    print(str(mttf).replace(".",","), file=logfile, end='')