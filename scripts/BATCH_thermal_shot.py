import matplotlib.pyplot as plt
import matplotlib.lines as lines
from mpl_toolkits.axes_grid1 import make_axes_locatable
from matplotlib.colors import LogNorm
from matplotlib.ticker import MultipleLocator
from BATCH_generate_TSV_file import get_x_y
from BATCH_FIT_effects import get_label
import numpy as np
import pandas as pd
import math 

def shot_batch(folders, windows):
    plt.rcParams['font.family'] = 'serif'
    plt.rcParams['font.serif'] = ['Times New Roman'] + plt.rcParams['font.serif']

    folders = [x for x in folders if "_chronos_" not in x]


    fig, axes = plt.subplots(nrows=windows, ncols=len(folders), sharex=True, sharey=True,  figsize=(3*len(folders),3*windows), constrained_layout=True)
    time_max = 9999999999
    
    for folder in folders:
        tsv_data = pd.read_csv("simulation/"+folder+"/simulation/SystemTemperature.tsv", sep='\t')
        raw_data = tsv_data.to_numpy()
        if time_max > raw_data[len(raw_data)-1][0]:
            time_max = raw_data[len(raw_data)-1][0]
    
    print(time_max)
    window_time = time_max/windows
    print("%.2f"%window_time)

    temp_max = 0

    for l in range(windows):
        k = 0
        for folder in folders:
            tsv_data = pd.read_csv("simulation/"+folder+"/scripts_data/SystemTemperature.tsv", sep='\t')
            raw_data = tsv_data.to_numpy()

            sysSize = len(raw_data[0])-1
            side = int(math.sqrt(sysSize))

            picAvg = np.zeros((side,side))
            nsamples = 0

            time = []
            n_pes = len(raw_data[0])-1
            sample = np.zeros(n_pes)
            
            for i in range(len(raw_data)): 
                if raw_data[i][0] > window_time*l and raw_data[i][0] <= window_time*(l+1):
                    time.append(raw_data[i][0])
                    nsamples += 1
                    for j in range(len(raw_data[i])-1):
                        sample[j] = raw_data[i][j+1]
                        picAvg[int(j/side)][int(j%side)] += sample[j]


            for x in range(side):
                for y in range(side):
                    picAvg[x][y] = picAvg[x][y] / nsamples
                    if picAvg[x][y] > temp_max:
                        temp_max = picAvg[x][y]


    for l in range(windows):
        k = 0
        for folder in folders:
            tsv_data = pd.read_csv("simulation/"+folder+"/scripts_data/SystemTemperature.tsv", sep='\t')
            raw_data = tsv_data.to_numpy()

            sysSize = len(raw_data[0])-1
            side = int(math.sqrt(sysSize))

            picAvg = np.zeros((side,side))
            nsamples = 0

            time = []
            n_pes = len(raw_data[0])-1
            sample = np.zeros(n_pes)
            
            for i in range(len(raw_data)): 
                if raw_data[i][0] > window_time*l and raw_data[i][0] <= window_time*(l+1):
                    time.append(raw_data[i][0])
                    nsamples += 1
                    for j in range(len(raw_data[i])-1):
                        sample[j] = raw_data[i][j+1]
                        picAvg[int(j/side)][int(j%side)] += sample[j]


            for x in range(side):
                for y in range(side):
                    picAvg[x][y] = picAvg[x][y] / nsamples


            # https://matplotlib.org/stable/gallery/images_contours_and_fields/interpolation_methods.html -> more interpolation methods
            # https://matplotlib.org/stable/tutorials/colors/colormaps.html                   -> more colors
            pcm = axes[l][k].imshow(picAvg, interpolation='gaussian', aspect='auto', vmin=50, vmax=max(temp_max, 100), cmap='jet') # YlOrRd, jet, turbo

            # Major ticks
            axes[l][k].set_xticks(np.arange(0, side, 1))
            axes[l][k].set_yticks(np.arange(0, side, 1))

            # Labels for major ticks
            axes[l][k].set_xticklabels(np.arange(0, side, 1))
            axes[l][k].set_yticklabels(np.arange(0, side, 1))

            # Minor ticks
            axes[l][k].set_xticks(np.arange(-.5, side-1, 1), minor=True)
            axes[l][k].set_yticks(np.arange(-.5, side-1, 1), minor=True)

            #Title
            #axes[l][k].set_title(labels[k])
            
            # #Display for the user the progress
            # for x in range(75):
            #     print('*' * (75 - x), x, end='\x1b[1K\r')
            #print(("=======>"+labels[k]+": Generating graph "+str(l+1)+" of 6 encompassing from "+str(time[0])+"s to "+str(time[-1])+"s"))#, end='\r')

            # Gridlines based on minor ticks
            axes[l][k].grid(which='minor', color='black', linestyle='-', linewidth=1)
            axes[l][k].tick_params(axis='both', which='major', labelsize=8)

            k+=1
  

    cols = []
    for i in range(len(folders)):
        heur,eff = get_label(folders[i], 0)
        cols.append(heur)
    
    rows = []
    for i in range(windows):
        rows.append("{:.2f}".format((i*window_time))+" to "+"{:.2f}".format(((i+1)*window_time)-0.01))

    for ax, col in zip(axes[0], cols):
        ax.set_title(col,  fontsize=20)

    for ax, row in zip(axes[:,0], rows):
        ax.set_ylabel(row, rotation=90, size='large', fontsize=24)

    fig.colorbar(pcm, ax=axes[:], location='right', shrink=0.75, label="Temperature (°C)")
    #fig.suptitle('Average PE Temperature - 70% System Occupancy', fontsize=16)
    parts = folders[0].split('_')
    ninit = parts[1]
    parts = folders[-1].split('_')
    nfinal = parts[1]
    name = "SIM_"+ninit+"_TO_"+nfinal
    if temp_max > 100.0:
        name = name + "_maxScale_" + "%.0f"%temp_max
    fig.savefig("simulation/"+name+"_THERMAL_BATCH.png", format='png', dpi=300, bbox_inches='tight')
    fig.savefig("simulation/"+name+"_THERMAL_BATCH.pdf", format='pdf', bbox_inches='tight')

#-----------------------------------------------------
if __name__ == '__main__': # chamada da funcao principal
    folders = ['SIMULATIONS_8_8_mixed1_90_20000.0ticks_migno_worst_8x8', 'SIMULATIONS_9_8_mixed1_90_20000.0ticks_migno_pattern_8x8', 'SIMULATIONS_10_8_mixed1_90_20000.0ticks_migno_pidtm_8x8', 'SIMULATIONS_11_8_mixed1_90_20000.0ticks_migyes_pidtm_8x8', 'SIMULATIONS_12_8_mixed1_90_20000.0ticks_migno_chronos_8x8', 'SIMULATIONS_13_8_mixed1_90_20000.0ticks_migyes_chronos_8x8', 'SIMULATIONS_14_8_mixed1_90_20000.0ticks_migno_chronos2_8x8', 'SIMULATIONS_15_8_mixed1_90_20000.0ticks_migyes_chronos2_8x8']
    
    shot_batch(folders)
