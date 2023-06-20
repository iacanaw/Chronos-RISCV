from copy import deepcopy
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import sys 

N_TASKTYPES = int(sys.argv[1])
N_ACTIONS = int(sys.argv[2])

evolution = []
amount = 0
biggest = 0
with open("log_0x0.txt", "r") as log_file:
    while True:
        line = ""
        try:
            line = log_file.readline()
        except:
            print("catched an error in file")
        if "policyTable" in line:
            splited = line.split(';')
            idx = 1
            samples = np.zeros((N_TASKTYPES, N_ACTIONS))
            for type in range(N_TASKTYPES):
                for act in range(N_ACTIONS):
                    samples[type][act] = float(int(splited[idx])/1000)
                    if float(int(splited[idx])/1000) > biggest:
                        biggest = int(samples[type][act])
                    idx+=1
            #print(samples)
            evolution.append(deepcopy(samples))
            amount+=1
        elif line == '':
            break
        
fig, axs = plt.subplots(N_ACTIONS, N_TASKTYPES)
#index = 0
for act in range(N_ACTIONS):
    for type in range(N_TASKTYPES):
        x = []
        y = []
        for i in range(amount):
            x.append(deepcopy(i))
            y.append(evolution[i][type][act])

        axs[act, type].plot(x, y)
        axs[act, type].set_title('Act '+str(act)+', Type '+str(type))
        axs[act, type].text(0, biggest+15,  "final: "+str(y[len(y)-1]))
        axs[act, type].set_ylim([0, biggest+20])

for ax in axs.flat:
    ax.set(xlabel='x-label', ylabel='y-label')

# Hide x labels and tick labels for top plots and y ticks for right plots.
for ax in axs.flat:
    ax.label_outer()

#plt.show()
fig = plt.gcf()
fig.set_size_inches(18.5, 10.5)
fig.savefig('qlearningtable.png', format='png', dpi=600)