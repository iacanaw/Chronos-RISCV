import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import copy
from BATCH_FIT_effects import get_label
from BATCH_generate_TSV_file import get_x_y

def adjacent_values(vals, q1, q3):
    upper_adjacent_value = q3 + (q3 - q1) * 1.5
    upper_adjacent_value = np.clip(upper_adjacent_value, q3, vals[-1])

    lower_adjacent_value = q1 - (q3 - q1) * 1.5
    lower_adjacent_value = np.clip(lower_adjacent_value, vals[0], q1)
    return lower_adjacent_value, upper_adjacent_value

def violin_batch_temp(folders):
    plt.rcParams['font.family'] = 'serif'
    plt.rcParams['font.serif'] = ['Times New Roman'] + plt.rcParams['font.serif']

    folders = [x for x in folders if "_chronos_" not in x]

    fig, ax = plt.subplots(figsize =(1.1*len(folders), 2.866))

    samples = []
    for folder in folders:
        x,y = get_x_y(folder)
        tsv_data = pd.read_csv("simulation/"+folder+"/scripts_data/SystemTemperature.tsv", sep='\t')
        raw_data = tsv_data.to_numpy()
        
        nsamples = 0
        npes = x*y

        pe_temp = np.zeros(npes)

        for i in range(len(raw_data)): 
            nsamples += 1
            for j in range(npes):
                pe_temp[j] += raw_data[i][j+1]
                        
        for j in range(npes):
            pe_temp[j] = pe_temp[j] / nsamples
        
        samples.append(pe_temp)

    data = copy.deepcopy(samples)

    # https://matplotlib.org/3.1.1/api/_as_gen/matplotlib.pyplot.boxplot.html
    #https://towardsdatascience.com/create-and-customize-boxplots-with-pythons-matplotlib-to-get-lots-of-insights-from-your-data-d561c9883643
    # plot violin plot
    plt.rcParams.update({'font.size': 10})
    plt.rcParams.update({'axes.labelsize': 10})
    plt.rc('axes', labelsize=12)

    #ax.set_title('Customized violin plot')
    parts = ax.violinplot(
            data, showmeans=False, showmedians=False,
            showextrema=False)

    colors = plt.cm.turbo(np.linspace(0, 1, len(folders)))

    k = 0
    for pc in parts['bodies']:
        pc.set_facecolor(colors[k%len(folders)])
        pc.set_edgecolor('black')
        pc.set_alpha(1)
        k+=1

    print(data)
    quartile1, medians, quartile3, maximum = np.percentile(data, [25, 50, 75, 100], axis=1)
    whiskers = np.array([
        adjacent_values(sorted_array, q1, q3)
        for sorted_array, q1, q3 in zip(data, quartile1, quartile3)])
    whiskers_min, whiskers_max = whiskers[:, 0], whiskers[:, 1]

    inds = np.arange(1, len(medians) + 1)
    ax.scatter(inds, medians, marker='_', color='k', s=20, zorder=3)
    ax.vlines(inds, quartile1, quartile3, color='white', linestyle='-', lw=5)
    ax.hlines(quartile3[-1], inds[-1]+1, 0, color="#0026FF", linestyle=(0, (5, 1)), lw=1 )
    ax.hlines(maximum[-1], inds[-1]+1, 0, color='r', linestyle=(0, (5, 1)), lw=1 )
    #ax.vlines(inds, whiskers_min, whiskers_max, color='k', linestyle='-', lw=1)

    ax.yaxis.grid(True)
    ax.xaxis.grid(True)

    ax.set_ylabel(r'Average Temperature in °C')
    ax.set_ylim(50,max([(max(maximum)+5),100]))
    ax.set_xlim(0.5, inds[-1]+0.5)

    labels = []
    for i in range(len(folders)):
        heur,eff = get_label(folders[i], 0)
        labels.append(heur)

    # add x-tick labels
    plt.setp(ax, xticks=[y+1 for y in range(len(data))], xticklabels=labels)
    parts = folders[0].split('_')
    ninit = parts[1]
    parts = folders[-1].split('_')
    nfinal = parts[1]
    name = "SIM_"+ninit+"_TO_"+nfinal
    fig.savefig("simulation/"+name+"_VIOLIN_AVGTEMP.png", format='png', dpi=300, bbox_inches='tight')
    fig.savefig("simulation/"+name+"_VIOLIN_AVGTEMP.pdf", format='pdf', bbox_inches='tight')
    #plt.show()

if __name__ == '__main__': # chamada da funcao principal
    folder_w = "SIMULATIONS_24_14_computation_90_25000.0ticks_migno_worst_14x14"
    folder_p = "SIMULATIONS_25_14_computation_90_25000.0ticks_migno_pattern_14x14"
    folder_pid = "SIMULATIONS_26_14_computation_90_25000.0ticks_migno_pidtm_14x14"
    folder_c = "SIMULATIONS_27_14_computation_90_25000.0ticks_migyes_pidtm_14x14"
    folder_flea = "SIMULATIONS_28_14_computation_90_25000.0ticks_migno_chronos_14x14"
    folder_d = "SIMULATIONS_29_14_computation_90_25000.0ticks_migyes_chronos_14x14"

    folders = [folder_w, folder_p, folder_pid, folder_c, folder_flea] 
    violin_batch(folders)