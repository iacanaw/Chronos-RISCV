import matplotlib.pyplot as plt
import matplotlib.lines as lines
from mpl_toolkits.axes_grid1 import make_axes_locatable
from matplotlib.colors import LogNorm
from matplotlib.ticker import MultipleLocator
import numpy as np
import pandas as pd
import math 

plt.rcParams['font.family'] = 'serif'
plt.rcParams['font.serif'] = ['Times New Roman'] + plt.rcParams['font.serif']

fig, axes = plt.subplots(nrows=1, ncols=6, sharex=True, sharey=True,  figsize=(18,3.5), constrained_layout=True)

folder_w = "SIMULATIONS_18_14_communication_90_25000.0ticks_migno_worst_14x14"
folder_p = "SIMULATIONS_19_14_communication_90_25000.0ticks_migno_pattern_14x14"
folder_pid = "SIMULATIONS_20_14_communication_90_25000.0ticks_migno_pidtm_14x14"
folder_c = "SIMULATIONS_21_14_communication_90_25000.0ticks_migyes_pidtm_14x14"
folder_flea = "SIMULATIONS_22_14_communication_90_25000.0ticks_migno_chronos_14x14"
folder_d = "SIMULATIONS_23_14_communication_90_25000.0ticks_migyes_chronos_14x14"

folders = [folder_w, folder_p, folder_pid, folder_c, folder_flea, folder_d] 
labels = ["Worst", "Pattern", "PID", "PID+Mig", "FLEA", "FLEA+Mig"]
#colors = ["#f9c80e", "#f86624", "#ea3546", "#43bccd" ]
colors = ["#390099", "#9e0059", "#ff5400", "#ffbd00", "#996900", "#f5ab00" ]

boxplot = []
picplot = []

time_max = 99999999999

for folder in folders:
    tsv_data = pd.read_csv("simulation/"+folder+"/simulation/SystemTemperature.tsv", sep='\t')
    raw_data = tsv_data.to_numpy()

    if time_max > raw_data[len(raw_data)-1][0]:
        time_max = raw_data[len(raw_data)-1][0]
print(time_max)
k = 0
for folder in folders:
        tsv_data = pd.read_csv("simulation/"+folder+"/simulation/SystemTemperature.tsv", sep='\t')
        raw_data = tsv_data.to_numpy()

        generalMax = 0
        sysSize = len(raw_data[0])-1
        side = int(math.sqrt(sysSize))
        picMax = np.zeros((side,side))

        picAvg = np.zeros((side,side))
        nsamples = 0

        time = []
        samples = np.zeros((len(raw_data[0])-1, len(raw_data)))

        n_pes = len(raw_data[0])-1
        n_samples = len(raw_data)

        max_temp = np.zeros(n_samples)
        sample = np.zeros(n_pes)

        for i in range(len(raw_data)):
                if raw_data[i][0] < time_max:
                        time.append(raw_data[i][0])
                        nsamples += 1
                        for j in range(len(raw_data[i])-1):
                                sample[j] = raw_data[i][j+1]
                                picAvg[int(j%side)][int(j/side)] += sample[j]
                                if generalMax < sample[j]:
                                        generalMax = sample[j]
                                        uj = 1
                                        for ux in range(side):
                                                for uy in range(side):
                                                        picMax[ux][uy] = raw_data[i][uj]
                                                        uj+=1

                sample.sort()
                max_temp[i] = sample[n_pes-1]
        
        # print(picAvg)
        # print(nsamples)
        for x in range(side):
                for y in range(side):
                        
                        picAvg[x][y] = picAvg[x][y] / nsamples


        # https://matplotlib.org/stable/gallery/images_contours_and_fields/interpolation_methods.html -> more interpolation methods
        # https://matplotlib.org/stable/tutorials/colors/colormaps.html                               -> more colors
        pcm = axes[k].imshow(picAvg, interpolation='gaussian', aspect='auto', vmin=50, vmax=90, cmap='jet') # YlOrRd, jet, turbo

        # Major ticks
        axes[k].set_xticks(np.arange(0, 14, 1))
        axes[k].set_yticks(np.arange(0, 14, 1))

        # Labels for major ticks
        axes[k].set_xticklabels(np.arange(0, 14, 1))
        axes[k].set_yticklabels(np.arange(0, 14, 1))

        # Minor ticks
        axes[k].set_xticks(np.arange(-.5, 13, 1), minor=True)
        axes[k].set_yticks(np.arange(-.5, 13, 1), minor=True)

        #Title
        axes[k].set_title(labels[k])

        # Gridlines based on minor ticks
        axes[k].grid(which='minor', color='black', linestyle='-', linewidth=1)
        axes[k].tick_params(axis='both', which='major', labelsize=8)

        k+=1

fig.colorbar(pcm, ax=axes[:], location='right', shrink=0.75, label="Temperature (°C)")
fig.suptitle('Average PE Temperature - 70% System Occupation', fontsize=16)

for i in range(len(axes)):
        pass
        # axes[i].yaxis.grid(True)
        # axes[i].xaxis.grid(True)
        #axes[i].legend()
        # axes[i].plot([0,1],[80,80], linestyle='dotted', color="black")
        # if i == 0:
        #         axes[i].set_title('Peak Temperature Comparison')

# lines_labels = [ax.get_legend_handles_labels() for ax in fig.axes]
# lines, labels = [sum(lol, []) for lol in zip(*lines_labels)]
#fig.legend(lines, labels, ncol=4, loc="lower center",  bbox_to_anchor=(0.673, -0.12))


# fig.text(0.5, -0.02, 'Time (s)', ha='center')
# fig.text(-0.02, 0.5, 'Temperature (°C)', va='center', rotation='vertical')

fig.savefig('thermal_shot.png', format='png', dpi=300, bbox_inches='tight')
fig.savefig('thermal_shot.pdf', format='pdf', bbox_inches='tight')

