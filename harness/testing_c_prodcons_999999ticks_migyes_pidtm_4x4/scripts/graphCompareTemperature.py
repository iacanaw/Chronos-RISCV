from copy import deepcopy
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import sys 

#pidtm_src   = sys.argv[1]
spiral_src  = sys.argv[1]
pattern_src = sys.argv[2]

# Create figure
fig, ((ax1, ax2)) = plt.subplots(2, 1)

#pidtm_data = pd.read_csv("simulation/"+pidtm_src+"/SystemTemperature.tsv", sep='\t')
#pidtm_raw_data = pidtm_data.to_numpy()

spiral_data = pd.read_csv("simulation/"+spiral_src+"/SystemTemperature.tsv", sep='\t')
spiral_raw_data = spiral_data.to_numpy()

pattern_data = pd.read_csv("simulation/"+pattern_src+"/SystemTemperature.tsv", sep='\t')
pattern_raw_data = pattern_data.to_numpy()

#pidtm_time = []
#pidtm_samples = np.zeros((len(pidtm_raw_data[0])-1, len(pidtm_raw_data)))

spiral_time = []
spiral_samples = np.zeros((len(spiral_raw_data[0])-1, len(spiral_raw_data)))

pattern_time = []
pattern_samples = np.zeros((len(pattern_raw_data[0])-1, len(pattern_raw_data)))

n_pes = len(spiral_raw_data[0])-1
# pidtm_n_samples = len(pidtm_raw_data)
spiral_n_samples = len(spiral_raw_data)
pattern_n_samples = len(pattern_raw_data)

#pidtm_peak_temp = np.zeros(pidtm_n_samples)
#pidtm_avg_temp = np.zeros(pidtm_n_samples)

spiral_peak_temp = np.zeros(spiral_n_samples)
spiral_avg_temp = np.zeros(spiral_n_samples)

pattern_peak_temp = np.zeros(pattern_n_samples)
pattern_avg_temp = np.zeros(pattern_n_samples)

# for i in range(len(pidtm_raw_data)):
#     pidtm_time.append(pidtm_raw_data[i][0])
#     for j in range(len(pidtm_raw_data[i])-1):
#         pidtm_samples[j][i] = pidtm_raw_data[i][j+1]
#         pidtm_avg_temp[i] += pidtm_samples[j][i]
#         if(pidtm_samples[j][i] > pidtm_peak_temp[i]):
#             pidtm_peak_temp[i] = pidtm_samples[j][i]
#     pidtm_avg_temp[i] = pidtm_avg_temp[i] / n_pes


for i in range(len(pattern_raw_data)):
    pattern_time.append(pattern_raw_data[i][0])
    for j in range(len(pattern_raw_data[i])-1):
        pattern_samples[j][i] = pattern_raw_data[i][j+1]
        pattern_avg_temp[i] += pattern_samples[j][i]
        if(pattern_samples[j][i] > pattern_peak_temp[i]):
            pattern_peak_temp[i] = pattern_samples[j][i]
    pattern_avg_temp[i] = pattern_avg_temp[i] / n_pes


for i in range(len(spiral_raw_data)):
    spiral_time.append(spiral_raw_data[i][0])
    for j in range(len(spiral_raw_data[i])-1):
        spiral_samples[j][i] = spiral_raw_data[i][j+1]
        spiral_avg_temp[i] += spiral_samples[j][i]
        if(spiral_samples[j][i] > spiral_peak_temp[i]):
            spiral_peak_temp[i] = spiral_samples[j][i]
    spiral_avg_temp[i] = spiral_avg_temp[i] / n_pes


#fig, ax = plt.subplots()

# ax.plot(pidtm_time, pidtm_peak_temp, 'r-', linewidth=1.0, label="PIDTM Peak")
# ax.plot(pidtm_time, pidtm_avg_temp, 'r--', linewidth=0.5, label="PIDTM Avg")

ax1.plot(spiral_time, spiral_peak_temp, 'k-', linewidth=1.0, label="Spiral")
ax2.plot(spiral_time, spiral_avg_temp, 'k-', linewidth=1.0, label="Spiral")

ax1.plot(pattern_time, pattern_peak_temp, 'g--', linewidth=1.0, label="Pattern")
ax2.plot(pattern_time, pattern_avg_temp, 'g--', linewidth=1.0, label="Pattern")

ax1.set_title('Peak Temperature')
ax1.set_ylabel("Temperature (°C)")
ax1.set_ylim([60, 80])
ax1.set_xlabel("Time (s)")
ax1.set_xlim([0.1, 0.9])
ax1.legend()
ax1.grid()

ax2.set_title('Average Temperature')
ax2.set_ylabel("Temperature (°C)")
ax2.set_ylim([55, 70])
ax2.set_xlabel("Time (s)")
ax2.set_xlim([0.1, 0.9])
ax2.legend()
ax2.grid()

plt.subplots_adjust(left=None, bottom=0.2, right=None, top=0.1, wspace=None, hspace=None)

fig = plt.gcf()
#fig.set_size_inches(18.5, 10.5)
#fig.savefig('TemperatureComparison.png', format='png', dpi=600, bbox_inches='tight')
#fig.savefig('TemperatureComparison.eps', format='eps', bbox_inches='tight')
plt.savefig('graphs2.pdf')  
plt.show()


