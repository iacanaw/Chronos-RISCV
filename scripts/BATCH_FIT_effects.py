import matplotlib.pyplot as plt
import matplotlib.lines as lines
from mpl_toolkits.axes_grid1 import make_axes_locatable
from matplotlib.colors import LogNorm
from matplotlib.ticker import MultipleLocator
from BATCH_generate_TSV_file import get_x_y
import numpy as np
import pandas as pd
import math 
import re

def fit_effects(folders):
    EM = 0
    SM = 1
    TDDB = 2
    TC = 3
    NBTI = 4
    TOTAL = 5

    folders = [x for x in folders if "_chronos_" not in x]

    
    x,y = get_x_y(folders[0])

    highest = np.zeros(6)
    smallest = np.zeros(6)
    for i in range(6): smallest[i] = 9999999999999

    fit_list = []

    for folder in folders:
        fit = np.zeros((6,x,y))
        #print(fit)

        with open("simulation/"+folder+"/scripts_data/montecarlofile", "r") as mttffile:
            for i in range(x):
                for j in range(y):
                    for k in range(5): #number of effects
                        fit[k][i][j] = float(mttffile.readline())
                        fit[TOTAL][i][j] += fit[k][i][j]
                        if fit[k][i][j] > highest[k]:
                            highest[k] = fit[k][i][j]
                        if fit[TOTAL][i][j] > highest[TOTAL]:
                            highest[TOTAL] = fit[TOTAL][i][j]

                        if fit[k][i][j] < smallest[k]:
                            smallest[k] = fit[k][i][j]
                        if fit[TOTAL][i][j] < smallest[TOTAL]:
                            smallest[TOTAL] = fit[TOTAL][i][j]
        fit_list.append(fit)
                

    for i in range(6):
        print(highest[i])
        print(smallest[i])
    
    print("MAX: "+str(max(highest[0:4])))
    print("MIN: "+str(min(smallest[0:4])))

    for i in range(5):
        highest[i] = max(highest[0:4])
        smallest[i] = min(smallest[0:4])
    
    plt.rcParams['font.family'] = 'serif'
    plt.rcParams['font.serif'] = ['Times New Roman'] + plt.rcParams['font.serif']
    fig, axes = plt.subplots(nrows=6, ncols=len(folders), sharex=True, sharey=True,  figsize=(2.7*len(folders),2.7*6), constrained_layout=True)


    # for i, ax in enumerate(axes.flatten()):
        # ax.set_title("Window "+"{:.2f}".format(i*window_time)+" to "+"{:.2f}".format((i+1)*window_time), fontsize=16)
    
    colors = ['Blues', 'Greens', 'Oranges', 'Reds', 'Purples', 'Greys']
    colors = ['YlOrRd', 'YlOrRd', 'YlOrRd', 'YlOrRd', 'YlOrRd', 'Reds']
    for effect in range(6):
        for method in range(len(folders)):
            # https://matplotlib.org/stable/gallery/images_contours_and_fields/interpolation_methods.html -> more interpolation methods
            # https://matplotlib.org/stable/tutorials/colors/colormaps.html                   -> more colors
            pcm = axes[effect][method].imshow(fit_list[method][effect], interpolation='hermite', aspect='auto', vmin=smallest[effect], vmax=highest[effect], cmap=colors[effect%6]) #colors[effect%6]) # YlOrRd, jet, turbo

            # Major ticks
            axes[effect][method].set_xticks(np.arange(0, x, 1))
            axes[effect][method].set_yticks(np.arange(0, y, 1))

            # Labels for major ticks
            axes[effect][method].set_xticklabels(np.arange(0, x, 1))
            axes[effect][method].set_yticklabels(np.arange(0, y, 1))

            # Minor ticks
            axes[effect][method].set_xticks(np.arange(-.5, x-1, 1), minor=True)
            axes[effect][method].set_yticks(np.arange(-.5, y-1, 1), minor=True)

            #Title
            #heur,eff = get_label(folders[method], effect)
            #axes[effect][method].set_title(heur+" - "+eff)
                       

            # #Display for the user the progress
            # for x in range(75):
            #     print('*' * (75 - x), x, end='\x1b[1K\r')
            #print("=======>"+labels[method]+": Generating graph "+str(1)+" of 6 EFFECTS", end='\r')

            # Gridlines based on minor ticks
            axes[effect][method].grid(which='minor', color='black', linestyle='-', linewidth=1)
            axes[effect][method].tick_params(axis='both', which='major', labelsize=8)
    

    cols = []
    for i in range(len(folders)):
        heur,eff = get_label(folders[i], effect)
        cols.append(heur)
    
    rows = []
    for i in range(6):
        heur,eff = get_label(folders[method], i)
        rows.append(eff)

    for ax, col in zip(axes[0], cols):
        ax.set_title(col,  fontsize=20)

    for ax, row in zip(axes[:,0], rows):
        ax.set_ylabel(row, rotation=90, size='large', fontsize=24)

    #fig.colorbar(pcm, ax=axes[:], location='right', shrink=0.75, label="Temperature (°C)")
    #fig.suptitle('Average PE Temperature - 70% System Occupancy', fontsize=16)
    parts = folders[0].split('_')
    ninit = parts[1]
    parts = folders[-1].split('_')
    nfinal = parts[1]
    name = "SIM_"+ninit+"_TO_"+nfinal

    fig.savefig("simulation/"+name+"_FIT_BATCH.png", format='png', dpi=300, bbox_inches='tight')
    fig.savefig("simulation/"+name+"_FIT_BATCH.pdf", format='pdf', bbox_inches='tight')

def get_label(folder, effect):
    # Extract PEs and calculate total count
    pe_match = re.search(r'(\d+)x(\d+)', folder)
     # Get Heuristic name
    parts = folder.split('_')
    heuristic = parts[7]
    if heuristic == 'worst':
        heuristic = "Grouped"
    elif heuristic =='pattern':
        heuristic = "Pattern"
    elif heuristic =='pidtm':
        heuristic = "PIDTM"
    elif heuristic =='chronos':
        heuristic = "FLEA"
    elif heuristic =='chronos2':
        heuristic = "FLEA+"

    if "migyes" in folder:
        heuristic = heuristic + " (MIG)"
    
    if effect == 0:
        eff = "EM"
    elif effect == 1:
        eff = "SM"
    elif effect == 2:
        eff = "TDDB"
    elif effect == 3:
        eff = "TC"
    elif effect == 4:
        eff = "NBTI"
    elif effect == 5:
        eff = "TOTAL"
    
    return heuristic,eff


#-----------------------------------------------------
if __name__ == '__main__': # chamada da funcao principal
    folders = ['SIMULATIONS_8_8_mixed1_90_20000.0ticks_migno_worst_8x8', 'SIMULATIONS_9_8_mixed1_90_20000.0ticks_migno_pattern_8x8', 'SIMULATIONS_10_8_mixed1_90_20000.0ticks_migno_pidtm_8x8', 'SIMULATIONS_11_8_mixed1_90_20000.0ticks_migyes_pidtm_8x8', 'SIMULATIONS_12_8_mixed1_90_20000.0ticks_migno_chronos_8x8', 'SIMULATIONS_13_8_mixed1_90_20000.0ticks_migyes_chronos_8x8', 'SIMULATIONS_14_8_mixed1_90_20000.0ticks_migno_chronos2_8x8', 'SIMULATIONS_15_8_mixed1_90_20000.0ticks_migyes_chronos2_8x8']
    labels = ['WORST', 'PATTERN', 'PIDTM', 'PIDTM+MIG', 'FLEA', 'FLEA+MIG', 'FLEA2', 'FLEA2+MIG']
    fit_effects(folders)
