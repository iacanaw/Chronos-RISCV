import matplotlib.pyplot as plt
import matplotlib.lines as lines
import numpy as np
import pandas as pd
import math 

def stacked_batch(folders, labels):
        plt.rcParams['font.family'] = 'serif'
        plt.rcParams['font.serif'] = ['Times New Roman'] + plt.rcParams['font.serif']

        fig, axes = plt.subplots(nrows=len(folders), ncols=1, sharex=True, sharey=True,  figsize=(((7.85/4)*len(folders)),4), constrained_layout=True)
        
        fig2, ax2 = plt.subplots(figsize=(10,4))

        # 
        colors = ["#390099", "#9e0059", "#ff5400", "#ffbd00", "#996900", "#f5ab00", "#004af5", "#0033AB" ]
        average = 80
        styles = ['-', '-', '-', '-','-', '-']

        boxplot = []
        picplot = []

        k = 0
        for folder in folders:
                tsv_data = pd.read_csv("simulation/"+folder+"/simulation/SystemTemperature.tsv", sep='\t')
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
                
                sliding_temp = []
                sliding_time = []
                window_size = 10
                for i in range(int(len(max_temp)/window_size)):
                        max_temp_sample = 0
                        for j in range(window_size):
                                max_temp_sample += max_temp[(i*window_size)+j]
                        max_temp_sample = max_temp_sample/window_size
                        sliding_temp.append(max_temp_sample)
                        sliding_time.append(time[(i*window_size)])
                
                        
                
                ax2.plot(sliding_time, sliding_temp, linewidth=1.0, label=labels[k], alpha=0.8)
                boxplot.append(axes[k].plot(time, max_temp, styles, color=colors[k%len(colors)], linewidth=1.0, label=labels[k]))
                k+=1
        ax2.set(xlabel='Time (s)', ylabel='Temperature (°C)')
        ax2.grid()
        ax2.set_ylim(65,100)
        ax2.set_xlim(0.1,2)
        ax2.legend(ncol=4)

        fig2.savefig("simulation/"+folders[0]+"_MAXTEMP_BATCH.png", format='png', dpi=300, bbox_inches='tight')
        
        for i in range(len(axes)):
                axes[i].set_ylim(65,100)
                axes[i].set_xlim(0.75, 1.75)
                axes[i].yaxis.grid(True)
                axes[i].xaxis.grid(True)
                axes[i].plot([0,2],[average,average], linestyle='dotted', color='black', linewidth=2)
                if i == 0:
                        axes[i].set_title('70% of PE occupation - Mixed - 14x14')

        lines_labels = [ax.get_legend_handles_labels() for ax in fig.axes]
        lines, labels = [sum(lol, []) for lol in zip(*lines_labels)]
        fig.legend(lines, labels, ncol=len(folders), loc="lower center",  bbox_to_anchor=(0.705, -0.12))


        fig.text(0.5, -0.02, 'Time (s)', ha='center')
        fig.text(-0.02, 0.5, 'Temperature (°C)', va='center', rotation='vertical')

        fig.savefig("simulation/"+folders[0]+"_STACKED_BATCH.png", format='png', dpi=300, bbox_inches='tight')
        fig.savefig("simulation/"+folders[0]+"_STACKED_BATCH.pdf", format='pdf', bbox_inches='tight')



if __name__ == '__main__': # chamada da funcao principal
        folder_w = "SIMULATIONS_24_14_computation_90_25000.0ticks_migno_worst_14x14"
        folder_p = "SIMULATIONS_25_14_computation_90_25000.0ticks_migno_pattern_14x14"
        folder_pid = "SIMULATIONS_26_14_computation_90_25000.0ticks_migno_pidtm_14x14"
        folder_c = "SIMULATIONS_27_14_computation_90_25000.0ticks_migyes_pidtm_14x14"
        folder_flea = "SIMULATIONS_28_14_computation_90_25000.0ticks_migno_chronos_14x14"
        folder_d = "SIMULATIONS_29_14_computation_90_25000.0ticks_migyes_chronos_14x14"

        folders = [folder_w, folder_p, folder_pid, folder_c, folder_flea] 
        labels = ["Worst", "Pattern", "PID", "PID+Mig", "FLEA", "FLEA+Mig"]
        stacked_batch(folders, labels)