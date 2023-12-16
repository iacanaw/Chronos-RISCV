import matplotlib.pyplot as plt
import numpy as np
plt.rcParams['font.family'] = 'serif'
plt.rcParams['font.serif'] = ['Times New Roman'] + plt.rcParams['font.serif']

# fig, axes = plt.subplots(nrows=1, ncols=2, figsize=(7.85,5), constrained_layout=True)
fig, axes = plt.subplots(nrows=1, ncols=2, figsize=(9,2.866), constrained_layout=True)


# generate some random test data
# oc90 = [[81.3263,	76.0900,	71.8157,	68.1843,	62.0369],
#         [80.6344,	76.1717,	72.1543,	68.9599,	63.4716],
#         [77.3879,	69.7506,	66.0386,	64.0159,	60.2984],
#         [80.7487,	76.3105,	72.4385,	69.4775,	64.8749],
#         [76.9512,	71.6757,	68.2872,	65.7172,	61.5780],
#         [80.0852,	75.7210,	71.8676,	69.1836,	65.0951]]

# oc70 = [[91.64,	81.82,	74.48,	64.53,	58.27],
#         [86.24,	77.61,	73.22,	69.32,	63.48],
#         [88.14,	78.51,	73.85,  70.30,	63.89], 
#         [82.83,	73.60,	69.36,	66.16,	60.14], 
#         [86.02,	77.05,	72.89,	69.90,	64.22],
#         [84.10,	73.80,	69.43,	66.51,	60.17]] 
# oc50 = [[90.21,	79.75,	68.73,	58.84,	55.98],
#         [82.73,	73.69,	69.07,	65.35,	60.46],
#         [84.94,	73.36,	68.80,	65.68,	60.34],
#         [78.77,	68.00,	64.32,	61.71,	57.57],
#         [80.71,	71.40,	67.98,	65.76,	61.22],
#         [77.93,	69.07,  65.82,	63.47,	58.79]]

oc70 = [[91.34014376078207,82.49510638297872,76.30470960322023,64.34713053479011,56.76424381828637],
        [87.6462456946039,78.415579793341,73.92424799081516,69.64914466130884,62.791417910447755],
        [87.85722698684961,78.21683819325328,73.55336192109777,69.95777587192681,63.58488279016581],
        [83.10944855031268,73.4257305287095,69.2448891415577,66.33859010801592,60.933291642978965],
        [86.51986827033218,77.88769186712486,73.44860824742268,70.33267468499427,64.11509736540664],
        [83.53005714285715,73.46997142857143,69.31996,66.57306285714286,60.894154285714286]]
oc50 = [[89.2666418338109,78.74428653295129,67.9020687679083,57.945065902578804,55.06763323782235],
        [81.79558806655191,72.84827309236948,68.29329890992541,64.36608146873208,59.33329317269076],
        [83.79314857142857,72.59741142857143,67.94831428571429,64.64710857142857,59.34746285714286],
        [78.81894766780434,67.86455062571103,64.43008532423208,61.98501137656427,57.81032992036405],
        [80.93299370349169,71.78180881511162,68.05933600457928,65.5480251860332,60.32150543789353],
        [78.30182389937107,69.28947970268725,66.06899942824471,63.77475700400228,59.03817038307604]]



# oc30 = [[74.3946,	65.4698,	56.1304,	53.2970,	52.0686],
#         [68.9861,	61.7478,	58.5275,	56.1924,	54.0994],
#         [71.5149,	61.9688,	58.8835,	56.9736,	54.4333],
#         [70.2041,	61.6862,	58.7707,	57.0731,	54.4805],
#         [68.1220,	61.4530,	59.0513,	56.8672,	53.5452],
#         [68.3635,	61.3590,	58.9239,	56.7885,	54.1087]]

labels = ["Clumped", "Pattern", "PID", "PID+M", "FLEA", "FLEA+M"]
boxplot = []
# plot violin plot
# boxplot.append(axes[0][0].boxplot(oc90, vert=True, patch_artist=True, labels=labels))
# axes[0][0].set_title("(a) 90% of PE occupancy")

# # plot violin plot
# boxplot.append(axes[0][1].boxplot(oc70, vert=True, patch_artist=True, labels=labels))
# axes[0][1].set_title("(b) 70% of PE occupancy")

# # # plot violin plot
# boxplot.append(axes[1][0].boxplot(oc50, vert=True, patch_artist=True, labels=labels))
# axes[1][0].set_title("(c) 50% of PE occupancy")

# # # plot violin plot
# boxplot.append(axes[1][1].boxplot(oc30, vert=True, patch_artist=True, labels=labels))
# axes[1][1].set_title("(d) 30% of PE occupancy")

# https://matplotlib.org/3.1.1/api/_as_gen/matplotlib.pyplot.boxplot.html
#https://towardsdatascience.com/create-and-customize-boxplots-with-pythons-matplotlib-to-get-lots-of-insights-from-your-data-d561c9883643
# plot violin plot
boxplot.append(axes[1].boxplot(oc70, vert=True, patch_artist=True, labels=labels, showfliers=False, whis=2.0))
axes[1].set_title("(b) 70% of PE occupancy")

# # plot violin plot
boxplot.append(axes[0].boxplot(oc50, vert=True, patch_artist=True, labels=labels, whis=2.0)) 
axes[0].set_title("(a) 50% of PE occupancy")

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
for col in axes:
        col.yaxis.grid(True)
        col.xaxis.grid(True)
        # col.set_xticks([y+1 for y in range(len(oc90))])
        #col.set_xlabel('Management technique')
        col.set_ylabel('Temperature (°C)')
        col.set_ylim(50,95)


# add x-tick labels
# plt.setp(axes, xticks=[y+1 for y in range(len(oc90))],
#             xticklabels=['W', 'P', 'PID-M', 'PID', 'C-M', 'C' ])
fig.savefig('candle_temp.png', format='png', dpi=300, bbox_inches='tight')
fig.savefig('candle_temp.pdf', format='pdf', bbox_inches='tight')
#plt.show()

