from curses import termattrs
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import sys

simType = sys.argv[1]

plt.close("all")
plt.style.use('seaborn-dark-palette')

tsv_data = pd.read_csv("simulation/SystemTemperature.tsv", sep='\t')
raw_data = tsv_data.to_numpy()

time = []
samples = np.zeros((len(raw_data[0])-1, len(raw_data)))

n_pes = len(raw_data[0])-1
n_samples = len(raw_data)


sample = np.zeros(n_pes)
min_temp = np.zeros(n_samples)
pri_quartil = np.zeros(n_samples)
mediana = np.zeros(n_samples)
ter_quartil = np.zeros(n_samples)
max_temp = np.zeros(n_samples)

avg_temp = np.zeros(n_samples)

for i in range(len(raw_data)):
    time.append(raw_data[i][0])
    for j in range(len(raw_data[i])-1):
        sample[j] = raw_data[i][j+1]
        avg_temp[i] += sample[j]

    sample.sort()
    min_temp[i] = sample[0]
    max_temp[i] = sample[n_pes-1]
    pri_quartil[i] = sample[int(n_pes/4)]
    mediana[i] = sample[int(n_pes/2)]
    ter_quartil[i] = sample[int((n_pes/2) + (n_pes/4))]

    avg_temp[i] = avg_temp[i] / n_pes

fig, ax = plt.subplots()

ax.plot(time, avg_temp, 'y-', linewidth=0.85, label = "Average Temperature")    
ax.plot(time, max_temp, 'r-', linewidth=1.0, label = "Maximum Temperature")
ax.plot(time, min_temp, 'b-', linewidth=1.0, label = "Minimum Temperature")
ax.plot(time, mediana, 'g-', linewidth=1.0, label = "Median Temperature")
ax.plot(time, pri_quartil, 'c-', linewidth=1.0, label = "First Quartile")
ax.plot(time, ter_quartil, 'm-', linewidth=1.0, label = "Third Quartile")

ax.set_ylim([50, 100])

ax.set_title('Temperature - '+simType)
ax.legend()
fig = plt.gcf()
fig.set_size_inches(18.5, 10.5)
fig.savefig('simulation/PeakTemperature.png', format='png', dpi=300, bbox_inches='tight')
fig.savefig('simulation/PeakTemperature.eps', format='eps', bbox_inches='tight')

with open("simulation/ThermalStatistics.csv", "w") as TSfile:
    print(str(max_temp.mean()).replace(".", ",")+";"+str(max_temp.std()).replace(".", ","), file=TSfile)
    print(str(ter_quartil.mean()).replace(".", ",")+";"+str(ter_quartil.std()).replace(".", ","), file=TSfile)
    print(str(mediana.mean()).replace(".", ",")+";"+str(mediana.std()).replace(".", ","), file=TSfile)
    print(str(pri_quartil.mean()).replace(".", ",")+";"+str(pri_quartil.std()).replace(".", ","), file=TSfile)
    print(str(min_temp.mean()).replace(".", ",")+";"+str(min_temp.std()).replace(".", ","), file=TSfile)
