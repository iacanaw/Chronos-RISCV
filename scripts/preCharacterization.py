import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import sys 

numTasks = int(sys.argv[1])
appName = sys.argv[2]

plt.close("all")
plt.style.use('_mpl-gallery')

tsv_data = pd.read_csv("SystemPower.tsv", sep='\t')
raw_data = tsv_data.to_numpy()

n_pes = len(raw_data[0])-1
#numTasks = n_pes
n_samples = len(raw_data)

time = []
samples = np.zeros((numTasks, len(raw_data)))

#peak_temp = np.zeros(n_samples)
avg_power = np.zeros(numTasks)

for i in range(len(raw_data)):
    time.append(raw_data[i][0])
    for j in range(numTasks):
        samples[j][i] = raw_data[i][n_pes-j]
        avg_power[j] += samples[j][i]
        #if(samples[n_pes-j][i] > peak_temp[i]):
            #peak_temp[i] = samples[n_pes-j][i]

for j in range(numTasks):
    avg_power[j] = avg_power[j] / n_samples
    print(str(j)+" "+str(avg_power[j]))

fig, ax = plt.subplots()

for i in range(numTasks):
    ax.plot(time, samples[i], '-', linewidth=1.5, label=str(i))


# ax.plot(time, peak_temp, 'k-', linewidth=1.0)
# ax.plot(time, avg_temp, 'r-', linewidth=1.0)
# ax.set_ylim([50, 85])

ax.legend()
ax.set_title('Power')

fig = plt.gcf()
fig.set_size_inches(18.5, 10.5)
fig.savefig('Power.png', format='png', dpi=600, bbox_inches='tight')
#fig.savefig('PeakTemperature.eps', format='eps', bbox_inches='tight')

