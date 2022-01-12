import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

plt.close("all")
plt.style.use('_mpl-gallery')


tsv_data = pd.read_csv("simulation/SystemTemperature.tsv", sep='\t')
raw_data = tsv_data.to_numpy()

time = []
samples = np.zeros((len(raw_data[0])-1, len(raw_data)))

n_pes = len(raw_data[0])-1
n_samples = len(raw_data)

peak_temp = np.zeros(n_samples)
avg_temp = np.zeros(n_samples)

for i in range(len(raw_data)):
    time.append(raw_data[i][0])
    for j in range(len(raw_data[i])-1):
        samples[j][i] = raw_data[i][j+1]
        avg_temp[i] += samples[j][i]
        if(samples[j][i] > peak_temp[i]):
            peak_temp[i] = samples[j][i]
    avg_temp[i] = avg_temp[i] / n_pes

fig, ax = plt.subplots()

for i in range(n_pes):
    ax.plot(time, samples[i], '--', linewidth=0.2)
    
ax.plot(time, peak_temp, 'k-', linewidth=1.0)
ax.plot(time, avg_temp, 'r-', linewidth=1.0)

ax.set_title('Temperature')

fig = plt.gcf()
fig.set_size_inches(18.5, 10.5)
fig.savefig('simulation/PeakTemperature.png', format='png', dpi=600, bbox_inches='tight')
fig.savefig('simulation/PeakTemperature.eps', format='eps', bbox_inches='tight')
