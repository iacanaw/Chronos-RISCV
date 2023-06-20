import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import sys 
# python3 scripts/temperatureComparison.py pattern_test66_c_training_999999999ticks_pattern_7x7 pidtm_test66_c_training_999999999ticks_pidtm_7x7 chronos2_test66_c_training_9999999999ticks_chronos2_7x7
pattern_src = sys.argv[1]
pidtm_src = sys.argv[2]
spiral_src = sys.argv[3]

plt.close("all")
plt.style.use('_mpl-gallery')

pattern_tsv_data = pd.read_csv("simulation/"+pattern_src+"/SystemTemperature.tsv", sep='\t')
pattern_raw_data = pattern_tsv_data.to_numpy()

pidtm_tsv_data = pd.read_csv("simulation/"+pidtm_src+"/SystemTemperature.tsv", sep='\t')
pidtm_raw_data = pidtm_tsv_data.to_numpy()

spiral_tsv_data = pd.read_csv("simulation/"+spiral_src+"/SystemTemperature.tsv", sep='\t')
spiral_raw_data = spiral_tsv_data.to_numpy()

pattern_time = []
pattern_samples = np.zeros((len(pattern_raw_data[0])-1, len(pattern_raw_data)))

pidtm_time = []
pidtm_samples = np.zeros((len(pidtm_raw_data[0])-1, len(pidtm_raw_data)))

spiral_time = []
spiral_samples = np.zeros((len(spiral_raw_data[0])-1, len(spiral_raw_data)))

n_pes = len(pattern_raw_data[0])-1
pattern_n_samples = len(pattern_raw_data)
pidtm_n_samples = len(pidtm_raw_data)
spiral_n_samples = len(spiral_raw_data)

pattern_peak_temp = np.zeros(pattern_n_samples)
pattern_avg_temp = np.zeros(pattern_n_samples)

pidtm_peak_temp = np.zeros(pidtm_n_samples)
pidtm_avg_temp = np.zeros(pidtm_n_samples)

spiral_peak_temp = np.zeros(spiral_n_samples)
spiral_avg_temp = np.zeros(spiral_n_samples)

for i in range(len(pattern_raw_data)):
    pattern_time.append(pattern_raw_data[i][0])
    for j in range(len(pattern_raw_data[i])-1):
        pattern_samples[j][i] = pattern_raw_data[i][j+1]
        pattern_avg_temp[i] += pattern_samples[j][i]
        if(pattern_samples[j][i] > pattern_peak_temp[i]):
            pattern_peak_temp[i] = pattern_samples[j][i]
    pattern_avg_temp[i] = (pattern_avg_temp[i] / n_pes)

for i in range(len(pidtm_raw_data)):
    pidtm_time.append(pidtm_raw_data[i][0])
    for j in range(len(pidtm_raw_data[i])-1):
        pidtm_samples[j][i] = pidtm_raw_data[i][j+1]
        pidtm_avg_temp[i] += pidtm_samples[j][i]
        if(pidtm_samples[j][i] > pidtm_peak_temp[i]):
            pidtm_peak_temp[i] = pidtm_samples[j][i]
    pidtm_avg_temp[i] = (pidtm_avg_temp[i] / n_pes)

for i in range(len(spiral_raw_data)):
    spiral_time.append(spiral_raw_data[i][0])
    for j in range(len(spiral_raw_data[i])-1):
        spiral_samples[j][i] = spiral_raw_data[i][j+1]
        spiral_avg_temp[i] += spiral_samples[j][i]
        if(spiral_samples[j][i] > spiral_peak_temp[i]):
            spiral_peak_temp[i] = spiral_samples[j][i]
    spiral_avg_temp[i] = (spiral_avg_temp[i] / n_pes)

fig, ax = plt.subplots()

ax.plot(pattern_time, pattern_peak_temp, 'g-', linewidth=1.0, label="PEAK Pattern")
ax.plot(pattern_time, pattern_avg_temp, 'g-', linewidth=0.5, label="AVG Pattern")

ax.plot(pidtm_time, pidtm_peak_temp, 'r-', linewidth=1.0, label="PEAK PIDTM")
ax.plot(pidtm_time, pidtm_avg_temp, 'r-', linewidth=0.5, label="AVG PIDTM")

ax.plot(spiral_time, spiral_peak_temp, 'b-', linewidth=1.0, label="PEAK Chronos")
ax.plot(spiral_time, spiral_avg_temp, 'b-', linewidth=0.5, label="AVG Chronos")

ax.set_ylim([30, 110])

ax.legend()

ax.set_title('Temperature (°C) / Time (s)')

fig = plt.gcf()
fig.set_size_inches(18.5, 10.5)
fig.savefig('simulation/TemperatureComparison.png', format='png', dpi=600, bbox_inches='tight')
#fig.savefig('simulation/PeakTemperature.eps', format='eps', bbox_inches='tight')


print("Statistics: ")
avg = 0
pavg = 0
for i in range(len(spiral_avg_temp)):
    avg += spiral_avg_temp[i]
    pavg += spiral_peak_temp[i]
avg = avg / len(spiral_avg_temp)
pavg = pavg / len(spiral_avg_temp)

print("Chronos:\n\tAVG: "+str(avg))
print("\tPEAK AVG: "+str(pavg))
print("=======================")

avg = 0
pavg = 0
for i in range(len(pidtm_avg_temp)):
    avg += pidtm_avg_temp[i]
    pavg += pidtm_peak_temp[i]
avg = avg / len(pidtm_avg_temp)
pavg = pavg / len(pidtm_avg_temp)

print("PIDTM:\n\tAVG: "+str(avg))
print("\tPEAK AVG: "+str(pavg))
print("=======================")

avg = 0
pavg = 0
for i in range(len(pattern_avg_temp)):
    avg += pattern_avg_temp[i]
    pavg += pattern_peak_temp[i]
avg = avg / len(pattern_avg_temp)
pavg = pavg / len(pattern_avg_temp)

print("Pattern:\n\tAVG: "+str(avg))
print("\tPEAK AVG: "+str(pavg))
print("=======================")
