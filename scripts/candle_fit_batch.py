import matplotlib.pyplot as plt
import numpy as np
import copy

def adjacent_values(vals, q1, q3):
    upper_adjacent_value = q3 + (q3 - q1) * 1.5
    upper_adjacent_value = np.clip(upper_adjacent_value, q3, vals[-1])

    lower_adjacent_value = q1 - (q3 - q1) * 1.5
    lower_adjacent_value = np.clip(lower_adjacent_value, vals[0], q1)
    return lower_adjacent_value, upper_adjacent_value

# def set_axis_style(ax, labelss):
#     ax.set_xticks(np.arange(1, len(labelss) + 1), labels=labelss)
#     ax.set_xlim(0.25, len(labelss) + 0.75)
#     ax.set_xlabel('Sample name')

def violin_batch(folders, labels):
    plt.rcParams['font.family'] = 'serif'
    plt.rcParams['font.serif'] = ['Times New Roman'] + plt.rcParams['font.serif']

    fig, ax = plt.subplots(figsize =(1.1*len(folders), 2.866))

    colors = ["#390099", "#9e0059", "#ff5400", "#ffbd00", "#996900", "#f5ab00", "#004af5", "#0033AB" ]

    EFFECTS = ["EM", "SM", "TDDB", "TC", "NBTI"]

    big_fit = 0
    small_fit = 9999999999
    fit_values = []
    for folder in folders:
        with open("simulation/"+folder+"/simulation/montecarlofile", "r") as fit_file:
                sys_fit = fit_file.readlines()
                sys_size = int(len(sys_fit)/len(EFFECTS))
                print("===============")
                print("The system has "+str(sys_size)+" PEs.")
                fits = []
                fit_sum = []
                bigPE = 0
                masterPE = 0
                for pe in range(sys_size):
                    # saves the fits
                    if(pe > -1):
                        summed =  float(sys_fit[pe*5])+float(sys_fit[(pe*5)+1])+float(sys_fit[(pe*5)+2])+float(sys_fit[(pe*5)+3])+float(sys_fit[(pe*5)+4])
                        pe = [ float(sys_fit[pe*5]), float(sys_fit[(pe*5)+1]), float(sys_fit[(pe*5)+2]), float(sys_fit[(pe*5)+3]), float(sys_fit[(pe*5)+4]) ]
                        fit_sum.append(summed)
                        fits.append(pe)

                        # update the graph limits
                        pe_total_fit = pe[0] + pe[1] + pe[2] + pe[3] + pe[4]
                        if(pe_total_fit > big_fit):
                            big_fit = pe_total_fit
                        if(pe_total_fit < small_fit):
                            small_fit = pe_total_fit
                        
                        if masterPE == 0:
                            masterPE = pe_total_fit
                        elif bigPE < pe_total_fit:
                            bigPE = pe_total_fit
                
                fit_values.append(fit_sum)

                with open("simulation/"+folder+"/simulation/mttf_calc.txt", "w") as mttffile:
                    mttf = ((10**9)/masterPE) / (365*24)
                    print(folder+" M-MTTF: "+str(mttf).replace(".",",") +" "+str(masterPE).replace(".",","), file=mttffile)
                    mttf = ((10**9)/bigPE) / (365*24)
                    print(folder+" OTHER-MTTF: "+str(mttf).replace(".",",")+" "+str(bigPE).replace(".",","), file=mttffile)
        
        print("Big: "+str(big_fit))
        print("Small: "+str(small_fit))
        interval = (big_fit - small_fit)
        print("Interval: "+str(interval))
        data = copy.deepcopy(fit_values)

    boxplot = []

    # https://matplotlib.org/3.1.1/api/_as_gen/matplotlib.pyplot.boxplot.html
    #https://towardsdatascience.com/create-and-customize-boxplots-with-pythons-matplotlib-to-get-lots-of-insights-from-your-data-d561c9883643
    # plot violin plot
    plt.rcParams.update({'font.size': 10})
    plt.rcParams.update({'axes.labelsize': 10})
    plt.rc('axes', labelsize=12)

    ax.set_title('Customized violin plot')
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

    quartile1, medians, quartile3 = np.percentile(data, [25, 50, 75], axis=1)
    whiskers = np.array([
        adjacent_values(sorted_array, q1, q3)
        for sorted_array, q1, q3 in zip(data, quartile1, quartile3)])
    whiskers_min, whiskers_max = whiskers[:, 0], whiskers[:, 1]

    inds = np.arange(1, len(medians) + 1)
    ax.scatter(inds, medians, marker='.', color='white', s=30, zorder=3)
    ax.vlines(inds, quartile1, quartile3, color='k', linestyle='-', lw=5)
    ax.vlines(inds, whiskers_min, whiskers_max, color='k', linestyle='-', lw=1)

    ax.yaxis.grid(True)
    ax.xaxis.grid(True)

    ax.set_ylabel(r'Failures in $10^9$ hours')
    ax.set_ylim(small_fit-200,big_fit+200)


    # add x-tick labels
    plt.setp(ax, xticks=[y+1 for y in range(len(data))], xticklabels=labels)

    fig.savefig("simulation/"+folders[0]+"_VIOLIN_BATCH.png", format='png', dpi=300, bbox_inches='tight')
    fig.savefig("simulation/"+folders[0]+"_VIOLIN_BATCH.pdf", format='pdf', bbox_inches='tight')
    #plt.show()

if __name__ == '__main__': # chamada da funcao principal
    folder_w = "SIMULATIONS_24_14_computation_90_25000.0ticks_migno_worst_14x14"
    folder_p = "SIMULATIONS_25_14_computation_90_25000.0ticks_migno_pattern_14x14"
    folder_pid = "SIMULATIONS_26_14_computation_90_25000.0ticks_migno_pidtm_14x14"
    folder_c = "SIMULATIONS_27_14_computation_90_25000.0ticks_migyes_pidtm_14x14"
    folder_flea = "SIMULATIONS_28_14_computation_90_25000.0ticks_migno_chronos_14x14"
    folder_d = "SIMULATIONS_29_14_computation_90_25000.0ticks_migyes_chronos_14x14"

    folders = [folder_w, folder_p, folder_pid, folder_c, folder_flea] 
    labels = ["Worst", "Pattern", "PID", "PID+Mig", "FLEA", "FLEA+Mig"]
    violin_batch(folders, labels)