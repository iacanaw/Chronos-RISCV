from copy import deepcopy
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import sys 

XX = int(sys.argv[1])
YY = int(sys.argv[2])

samples = []
samples_time = []

index = 0
for xx in range(XX):
    for yy in range(YY):
        empty = []
        time = []
        with open("simulation/log_"+str(xx)+"x"+str(yy)+".txt", "r") as log_file:
            while True:
                line = log_file.readline()
                if "inst~~~>" in line:
                    value = line.split(' ')
                    empty.append(int(value[1]))
                    time.append(int(value[3])/10000)
                elif line == '':
                    break
            #print(empty)
            samples.append(deepcopy(empty))
            samples_time.append(deepcopy(time))
            #print(samples) 
    index += 1


fig, axs = plt.subplots(XX, YY)
index = 0
for xx in range(XX):
    for yy in range(YY):
        axs[xx, yy].plot(samples_time[index], samples[index])
        axs[xx, yy].set_title('Axis ['+str(xx)+', '+str(yy)+']')
        axs[xx, yy].set_ylim([0, 1100000])

        index += 1

for ax in axs.flat:
    ax.set(xlabel='x-label', ylabel='y-label')

# Hide x labels and tick labels for top plots and y ticks for right plots.
for ax in axs.flat:
    ax.label_outer()

#plt.show()

#FIT stuff

tsv_data = pd.read_csv("simulation/FITlog.tsv", sep='\t')
raw_data = tsv_data.to_numpy()

FIT_time = []
FIT_samples = np.zeros((len(raw_data[0])-1, len(raw_data)))

n_pes = len(raw_data[0])-1
n_samples = len(raw_data)

for i in range(len(raw_data)):
    FIT_time.append(raw_data[i][0])
    for j in range(len(raw_data[i])-1):
        FIT_samples[j][i] = raw_data[i][j+1]

index = 0
for xx in range(XX):
    for yy in range(YY):
        ax2 = axs[yy, xx].twinx()
        ax2.plot(FIT_time, FIT_samples[index], color="red")
        ax2.set_title('Axis ['+str(yy)+', '+str(xx)+']')
        ax2.set_ylim([1000, 2000])
        index += 1

for ax in axs.flat:
    ax.set(xlabel='x-label', ylabel='y-label')

fig = plt.gcf()
fig.set_size_inches(18.5, 10.5)
fig.savefig('simulation/Instructions2.png', format='png', dpi=600)

