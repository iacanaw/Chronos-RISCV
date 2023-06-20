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

fig, axes = plt.subplots(nrows=1, ncols=4, sharex=True, sharey=True,  figsize=(12,3), constrained_layout=True)

# generate some random test data
folder_w = "ISCAS23_Q3_30_iscas_70_10000ticks_migno_worst_8x8"
folder_p = "ISCAS23_Q3_31_iscas_70_10000ticks_migno_pattern_8x8"
folder_pid = "ISCAS23_Q3_32_iscas_70_10000ticks_migyes_pidtm_8x8"
folder_c = "ISCAS23_Q3_34_iscas_70_10000ticks_migyes_chronos_8x8"

folders = [folder_w, folder_p, folder_pid, folder_c] 
labels = ["Worst", "Pattern", "PIDTM Mig.", "FLEA Mig."]
#colors = ["#f9c80e", "#f86624", "#ea3546", "#43bccd" ]
colors = ["#ef476f", "#FFD166", "#06D6A0", "#118AB2" ]

boxplot = []
picplot = []

k = 0
for folder in folders:
        tsv_data = pd.read_csv("../simulation/"+folder+"/simulation/SystemTemperature.tsv", sep='\t')
        raw_data = tsv_data.to_numpy()

        generalMax = 0
        sysSize = len(raw_data[0])-1
        side = int(math.sqrt(sysSize))
        picMax = np.zeros((side,side))

        time = []
        samples = np.zeros((len(raw_data[0])-1, len(raw_data)))

        n_pes = len(raw_data[0])-1
        n_samples = len(raw_data)

        max_temp = np.zeros(n_samples)
        sample = np.zeros(n_pes)

        for i in range(len(raw_data)):
                time.append(raw_data[i][0])
                for j in range(len(raw_data[i])-2):
                        sample[j] = raw_data[i][j+2]
                        if generalMax < sample[j]:
                                generalMax = sample[j]
                                uj = 1
                                for ux in range(side):
                                        for uy in range(side):
                                                picMax[ux][uy] = raw_data[i][uj]
                                                uj+=1

                sample.sort()
                max_temp[i] = sample[n_pes-2]
        
        boxplot.append(axes[k].imshow(picMax,  interpolation='nearest', aspect='auto', vmin=50, vmax=80))
        # divider = make_axes_locatable(axes[k])
        # cax = divider.append_axes("right", size="20%", pad=0.05)
        # plt.colorbar(boxplot[len(boxplot)-1], cax=cax, ticks=MultipleLocator(10), format="%.0f")
        k+=1

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
