import matplotlib.pyplot as plt
import numpy as np
import copy
plt.rcParams['font.family'] = 'serif'
plt.rcParams['font.serif'] = ['Times New Roman'] + plt.rcParams['font.serif']

# fig, axes = plt.subplots(nrows=1, ncols=2, figsize=(7.85,5), constrained_layout=True)
# fig, axes = plt.subplots(nrows=1, ncols=2, figsize=(9, 2.866), constrained_layout=True)
fig, ax = plt.subplots(figsize =(5, 2.866))


EFFECTS = ["EM", "SM", "TDDB", "TC", "NBTI"]

folder_w = "SIMULATIONS_12_14_misto_70_25000.0ticks_migno_worst_14x14"
folder_p = "SIMULATIONS_13_14_misto_70_25000.0ticks_migno_pattern_14x14"
folder_pid = "SIMULATIONS_14_14_misto_70_25000.0ticks_migno_pidtm_14x14"
folder_c = "SIMULATIONS_15_14_misto_70_25000.0ticks_migyes_pidtm_14x14"
folder_flea = "SIMULATIONS_16_14_misto_70_25000.0ticks_migno_chronos_14x14"
folder_d = "SIMULATIONS_17_14_misto_70_25000.0ticks_migyes_chronos_14x14"

folders = [folder_w, folder_p, folder_pid, folder_c, folder_flea, folder_d] 

big_fit = 0
small_fit = 9999999999
fit_values = []
for folder in folders:
        # if i == 0 or i == 6:
        #         fit_values = []

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
        oc70 = copy.deepcopy(fit_values)

labels = ["Worst", "Pattern", "PID", "PID+Mig", "FLEA", "FLEA+Mig"]
boxplot = []

# https://matplotlib.org/3.1.1/api/_as_gen/matplotlib.pyplot.boxplot.html
#https://towardsdatascience.com/create-and-customize-boxplots-with-pythons-matplotlib-to-get-lots-of-insights-from-your-data-d561c9883643
# plot violin plot
plt.rcParams.update({'font.size': 10})
plt.rcParams.update({'axes.labelsize': 10})
plt.rc('axes', labelsize=12)

boxplot.append(plt.boxplot(oc70, vert=True, patch_artist=True, labels=labels, whis=10.0))
# boxplot.append(plt.violinplot(oc70, vert=True))
plt.title("70% of PE occupation - Mixed - 14x14")

# # plot violin plot
# boxplot.append(axes[0].boxplot(oc50, vert=True, patch_artist=True, labels=labels, whis=10.0)) 
# axes[0].set_title("(a) 50% of PE occupation")

# print(oc70)

colors = ["#ef476f", "#FFD166", "#06D6A0", "#9AFCE2", "#118AB2", "#98DEF5" ]
#hatches = ["x", "-", "\\", "/", "o", "."]

for bplot in boxplot:
    for patch, color in zip(bplot['boxes'], colors):
        patch.set_facecolor(color)
#     for patch, hatch in zip(bplot['boxes'], hatches):
#         patch.set(hatch = hatch)
    for median in bplot['medians']:
        median.set(color='black', linewidth=2)

# adding horizontal grid lines
# for row in axes:
#     for col in row:
# for col in axes:
#         col.yaxis.grid(True)
#         col.xaxis.grid(True)
#         # col.set_xticks([y+1 for y in range(len(oc90))])
#         #col.set_xlabel('Management technique')
#         col.set_ylabel(r'Failures in $10^9$ hours')
#         col.set_ylim(small_fit-200,big_fit+200)

ax.yaxis.grid(True)
ax.xaxis.grid(True)
# col.set_xticks([y+1 for y in range(len(oc90))])
#col.set_xlabel('Management technique')
ax.set_ylabel(r'Failures in $10^9$ hours')
ax.set_ylim(small_fit-200,big_fit+200)


# add x-tick labels
# plt.setp(axes, xticks=[y+1 for y in range(len(oc90))],
#             xticklabels=['W', 'P', 'PID-M', 'PID', 'C-M', 'C' ])
fig.savefig('candle_fit.png', format='png', dpi=300, bbox_inches='tight')
fig.savefig('candle_fit.pdf', format='pdf', bbox_inches='tight')
#plt.show()

