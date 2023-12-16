import matplotlib.pyplot as plt
import matplotlib.lines as lines
import numpy as np
import pandas as pd
import math 
from statistics import mean 
from BATCH_FIT_effects import get_label

def stacked_batch(folders):
    plt.rcParams['font.family'] = 'serif'
    plt.rcParams['font.serif'] = ['Times New Roman'] + plt.rcParams['font.serif']

    folders = [x for x in folders if "_chronos_" not in x]

    fig, axes = plt.subplots(nrows=len(folders), ncols=1, sharex=True, sharey=True,  figsize=((2*len(folders)),8), constrained_layout=True)
    
    average = 80
    last_max = 0.0
    last_min = 99999999.0
    k = 0
    for folder in folders:
        tsv_data = pd.read_csv("simulation/"+folder+"/scripts_data/SystemTemperature.tsv", sep='\t')
        raw_data = tsv_data.to_numpy()

        generalMax = 0
        sysSize = len(raw_data[0])-1
        n_samples = len(raw_data)

        time = []

        max_temp = np.zeros(n_samples)
        avg_temp = np.zeros(n_samples)
        sample = np.zeros(sysSize)

        for i in range(len(raw_data)):
            time.append(raw_data[i][0])
            sample = np.zeros(sysSize)
            for j in range(len(raw_data[i])-2):
                sample[j] = raw_data[i][j+2]
                if generalMax < sample[j]:
                    generalMax = sample[j]
            
            sample.sort()
            max_temp[i] = sample[sysSize-1]
            avg_temp[i] = mean(sample)
            #print(avg_temp[i])
        
        sliding_temp_peak = []
        sliding_temp_mean = []
        sliding_time = []
        window_size = 1
        for i in range(int(len(max_temp)/window_size)):
            max_temp_sample = 0
            avg_temp_sample = 0
            for j in range(window_size):
                max_temp_sample += max_temp[(i*window_size)+j]
                avg_temp_sample += avg_temp[(i*window_size)+j]
            max_temp_sample = max_temp_sample/window_size
            avg_temp_sample = avg_temp_sample/window_size
            sliding_temp_peak.append(max_temp_sample)
            sliding_temp_mean.append(avg_temp_sample)
            sliding_time.append(time[(i*window_size)])
        
        heur,eff = get_label(folder, 0)

        if (mean(max_temp)+(mean(max_temp)*0.15)) > last_max:
            last_max = (mean(max_temp)+(mean(max_temp)*0.25))
        

        if (mean(avg_temp)-(mean(avg_temp)*0.25)) < last_min:
            last_min = (mean(avg_temp)-(mean(avg_temp)*0.25))

        axes[k].set_ylim(last_min, last_max)

        axes[k].set_xlim(0.1, sliding_time[-1])
        axes[k].yaxis.grid(True)
        axes[k].xaxis.grid(True)
        axes[k].plot([0,3],[mean(max_temp),mean(max_temp)], linestyle='dotted', color='red', linewidth=2)
        axes[k].annotate(("%.1f°C" % (mean(max_temp))), xy=(sliding_time[int(len(sliding_time)/15)], (mean(max_temp)+(last_max-mean(max_temp))/3)))
        axes[k].plot(sliding_time,sliding_temp_peak, linestyle='-', alpha=0.75, color='red', linewidth=1)


        axes[k].plot([0,3],[mean(avg_temp),mean(avg_temp)], linestyle='dotted', color='purple', linewidth=2) 
        axes[k].annotate(("%.1f°C" % (mean(avg_temp))), xy=(sliding_time[int(len(sliding_time)/15)], (mean(avg_temp)-(mean(avg_temp)-last_min)/1.5)))
        axes[k].plot(sliding_time,sliding_temp_mean, linestyle='-', alpha=0.75, color='purple', linewidth=1)
        
        axes[k].set_title(heur)
        k+=1

    lines_labels = [ax.get_legend_handles_labels() for ax in fig.axes]
    lines, labels = [sum(lol, []) for lol in zip(*lines_labels)]
    #fig.legend(lines, labels, ncol=len(folders)-2, loc="lower center",  bbox_to_anchor=(0.705, -0.12))

    fig.text(0.5, -0.02, 'Time (s)', ha='center',  fontsize=18)
    fig.text(-0.02, 0.5, 'Temperature (°C)', va='center', rotation='vertical',  fontsize=18)

    parts = folders[0].split('_')
    ninit = parts[1]
    parts = folders[-1].split('_')
    nfinal = parts[1]
    name = "SIM_"+ninit+"_TO_"+nfinal

    fig.savefig("simulation/"+name+"_STACKED_BATCH.png", format='png', dpi=300, bbox_inches='tight')
    fig.savefig("simulation/"+name+"_STACKED_BATCH.pdf", format='pdf', bbox_inches='tight')



if __name__ == '__main__': # chamada da funcao principal
    folders = ['SIMULATIONS_8_8_mixed1_90_20000.0ticks_migno_worst_8x8', 'SIMULATIONS_9_8_mixed1_90_20000.0ticks_migno_pattern_8x8', 'SIMULATIONS_10_8_mixed1_90_20000.0ticks_migno_pidtm_8x8', 'SIMULATIONS_11_8_mixed1_90_20000.0ticks_migyes_pidtm_8x8', 'SIMULATIONS_12_8_mixed1_90_20000.0ticks_migno_chronos_8x8', 'SIMULATIONS_13_8_mixed1_90_20000.0ticks_migyes_chronos_8x8', 'SIMULATIONS_14_8_mixed1_90_20000.0ticks_migno_chronos2_8x8', 'SIMULATIONS_15_8_mixed1_90_20000.0ticks_migyes_chronos2_8x8']
    stacked_batch(folders)