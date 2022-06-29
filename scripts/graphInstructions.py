from copy import deepcopy
import matplotlib.pyplot as plt
import numpy as np
import re
import sys 

XX = int(sys.argv[1])
YY = int(sys.argv[2])

samples = []
samples_time = []

index = 0
for xx in range(XX):
    for yy in range(YY):
        print("reading file " + str(xx) + "x" + str(yy))
        empty = []
        time = []
        with open("simulation/log_"+str(xx)+"x"+str(yy)+".txt", "r") as log_file:
            while True:
                line = ""
                try:
                    line = log_file.readline()
                except:
                    print("catched an error in file "+ str(xx) + "x" + str(yy))
                if "inst~~~>" in line:
                    value = line.split(' ')
                    if value[1].isnumeric() and value[3].isnumeric():
                        empty.append(int(value[1]))
                        time.append(int(value[3]))
                elif line == '':
                    break
            samples.append(deepcopy(empty))
            samples_time.append(deepcopy(time))
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

fig = plt.gcf()
fig.set_size_inches(18.5, 10.5)
fig.savefig('simulation/Instructions.png', format='png', dpi=600)
