import matplotlib.pyplot as plt
import matplotlib.lines as lines
from mpl_toolkits.axes_grid1 import make_axes_locatable
from matplotlib.colors import LogNorm
from matplotlib.ticker import MultipleLocator
import numpy as np
import pandas as pd
import math

SHOTS = 1
LEGEND = 1


plt.rcParams['font.family'] = 'serif'
plt.rcParams['font.serif'] = ['Times New Roman'] + plt.rcParams['font.serif']

nrows = math.ceil(SHOTS/4)
if(SHOTS > 1):
        fig, axes = plt.subplots(nrows=nrows, ncols=SHOTS, sharex=True, sharey=True,  figsize=(12,nrows-1), constrained_layout=True)
else:
        #fig, axes = plt.subplots(sharex=True, sharey=True,  figsize=(7.037,6), constrained_layout=True)
        fig, axes = plt.subplots(sharex=True, sharey=True,  figsize=(3.5185*1.5,3*1.5), constrained_layout=True)

labels = ["Worst", "Pattern", "PID", "PID+Mig"]
#colors = ["#f9c80e", "#f86624", "#ea3546", "#43bccd" ]
colors = ["#390099", "#9e0059", "#ff5400", "#ffbd00" ]

#folder = "test_power2_c_pipeline_beta_9999999999ticks_migno_characterize_11x11"
#folder = "test_power1_c_dtw_9999999999ticks_migno_characterize_11x11"
#folder = "test_single_test_singlex4_9999999999ticks_migno_characterize_11x11"
folder = "TESE_4TASK_DIAGONAL"
labels = ["single"]

boxplot = []
picplot = []

for k in range(0, SHOTS):
        tsv_data = pd.read_csv("../simulation/"+folder+"/simulation/SystemTemperature.tsv", sep='\t')
        raw_data = tsv_data.to_numpy()

        timeSamples = len(raw_data)-1
        
        deltaShot =  1000#timeSamples/SHOTS

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

        for i in range(int(k*deltaShot), int(((k+1)*deltaShot))):
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
        #pcm = axes[k].imshow(picAvg, interpolation='catrom', aspect='auto', vmin=50, vmax=85, cmap='jet') # YlOrRd, jet, turbo

        #pcm = axes[k].imshow(picAvg, interpolation='catrom', aspect='auto', vmin=50, vmax=generalMax, cmap='jet') # YlOrRd, jet, turbo

        # Major ticks
        if(SHOTS > 1):
                pcm = axes[k].imshow(picAvg, interpolation='gaussian', aspect='auto', vmin=50, vmax=65, cmap='jet') # YlOrRd, jet, turbo
                axes[k].set_xticks(np.arange(0, side, 1))
                axes[k].set_yticks(np.arange(0, side, 1))

                # Labels for major ticks
                axes[k].set_xticklabels(np.arange(0, side, 1))
                axes[k].set_yticklabels(np.arange(0, side, 1))

                # Minor ticks
                axes[k].set_xticks(np.arange(-.5, side-1, 1), minor=True)
                axes[k].set_yticks(np.arange(-.5, side-1, 1), minor=True)

                #Title
                #axes[k].set_title("{:.2f}".format((0.011+(k*deltaShot)*0.001))+"s ~ "+"{:.2f}".format(0.011+0.001*(((k+1)*deltaShot)-1))+"s")

                # Gridlines based on minor ticks
                axes[k].grid(which='minor', color='black', linestyle='-', linewidth=1)
                axes[k].tick_params(axis='both', which='major', labelsize=8)
        else: 
                pcm = axes.imshow(picAvg, origin='lower', interpolation='gaussian', aspect='auto', vmin=50, vmax=65, cmap='jet') # YlOrRd, jet, turbo
                axes.set_xticks(np.arange(0, side, 1))
                axes.set_yticks(np.arange(0, side, 1))

                # Labels for major ticks
                axes.set_xticklabels(np.arange(0, side, 1))
                axes.set_yticklabels(np.arange(0, side, 1))

                # Minor ticks
                axes.set_xticks(np.arange(-.5, side-1, 1), minor=True)
                axes.set_yticks(np.arange(-.5, side-1, 1), minor=True)

                #Title
                #axes.set_title("{:.2f}".format((0.011+(k*deltaShot)*0.001))+"s ~ "+"{:.2f}".format(0.011+0.001*(((k+1)*deltaShot)-1))+"s")

                # Gridlines based on minor ticks
                axes.grid(which='minor', color='black', linestyle='-', linewidth=1)
                axes.tick_params(axis='both', which='major', labelsize=8)
        k+=1

        x_end = side-0.75
        x_start = -0.6
        y_end = x_end
        y_start = x_start
        size = 11
        jump_x = (x_end - x_start) / (1.5 * size)
        jump_y = (y_end - y_start) / (1.5 * size)
        x_positions = np.linspace(start=x_start, stop=x_end, num=size, endpoint=False)
        y_positions = np.linspace(start=y_start, stop=y_end, num=size, endpoint=False)

        for y_index, y in enumerate(y_positions):
                for x_index, x in enumerate(x_positions):
                        label = picAvg[x_index][y_index]
                        text_x = x + jump_x
                        text_y = y + jump_y
                        if(x_index == 10 and y_index== 10):
                                axes.text(text_x+0.055, text_y, "{:.0f}".format(label), fontsize=12, color='black', ha='center', va='center')
                        if(y_index >= 3 and y_index <= 7):
                                if(x_index >= 3 and x_index <= 7):
                                        axes.text(text_x, text_y, "{:.0f}".format(label), fontsize=12, color='black', ha='center', va='center')
        
if(LEGEND == 1):
        if(SHOTS > 1):
                fig.colorbar(pcm, ax=axes[:], location='right', shrink=1.0, label="Temperature (°C)")
        else:
                fig.colorbar(pcm, ax=axes, location='right', shrink=1.0, label="Temperature (°C)")
##fig.suptitle('System Temperature Shot', fontsize=16)

# for i in range(len(axes)):
#         pass
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

fig.savefig('thermal_evo.png', format='png', dpi=300, bbox_inches='tight')
fig.savefig('thermal_evo.pdf', format='pdf', bbox_inches='tight')

