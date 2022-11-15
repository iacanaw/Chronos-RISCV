import matplotlib.pyplot as plt
import numpy as np
plt.rcParams['font.family'] = 'serif'
plt.rcParams['font.serif'] = ['Times New Roman'] + plt.rcParams['font.serif']

# fig, axes = plt.subplots(nrows=1, ncols=2, figsize=(7.85,5), constrained_layout=True)
fig, axes = plt.subplots(nrows=1, ncols=2, figsize=(7.85,2.5), constrained_layout=True)


# generate some random test data
# oc90 = [[81.3263,	76.0900,	71.8157,	68.1843,	62.0369],
#         [80.6344,	76.1717,	72.1543,	68.9599,	63.4716],
#         [77.3879,	69.7506,	66.0386,	64.0159,	60.2984],
#         [80.7487,	76.3105,	72.4385,	69.4775,	64.8749],
#         [76.9512,	71.6757,	68.2872,	65.7172,	61.5780],
#         [80.0852,	75.7210,	71.8676,	69.1836,	65.0951]]

oc70 = [[81.1929,	74.2483,	67.9038,	60.4341,	55.3078],
        [77.0359,	71.0943,	67.2858,	64.3558,	60.0132],
        [74.5976,	68.4704,	64.7058,	62.3384,	58.3274],
        [77.9872,	71.8650,	67.7194,	64.8227,	60.3273],
        [73.1152,	66.4677,	63.4331,	61.2463,	57.5836],
        [77.2606,	70.6804,	67.2492,	64.6851,	60.5481]]

oc50 = [[79,92814,	72.93129,	64.06901,	56.21261,	53.37390],
        [74,60048,	67.87870,	64.18581,	60.90977,	57.26104],
        [72,97194,	65.85454,	62.32466,	59.73616,	56.35562],
        [75,81441,	68.22860,	63.94348,	61.54547,	57.67834],
        [72.10110,	66.85192,	63.82657,	62.14709,	58.80774],
        [72,59939,	66.85192,	63.81737,	62.06588,	58.87028]]

# oc30 = [[74.3946,	65.4698,	56.1304,	53.2970,	52.0686],
#         [68.9861,	61.7478,	58.5275,	56.1924,	54.0994],
#         [71.5149,	61.9688,	58.8835,	56.9736,	54.4333],
#         [70.2041,	61.6862,	58.7707,	57.0731,	54.4805],
#         [68.1220,	61.4530,	59.0513,	56.8672,	53.5452],
#         [68.3635,	61.3590,	58.9239,	56.7885,	54.1087]]

labels = ["Worst", "Pattern", "PID-M", "PID", "FLEA-M", "FLEA"]
boxplot = []
# plot violin plot
# boxplot.append(axes[0][0].boxplot(oc90, vert=True, patch_artist=True, labels=labels))
# axes[0][0].set_title("(a) 90% of PE occupation")

# # plot violin plot
# boxplot.append(axes[0][1].boxplot(oc70, vert=True, patch_artist=True, labels=labels))
# axes[0][1].set_title("(b) 70% of PE occupation")

# # # plot violin plot
# boxplot.append(axes[1][0].boxplot(oc50, vert=True, patch_artist=True, labels=labels))
# axes[1][0].set_title("(c) 50% of PE occupation")

# # # plot violin plot
# boxplot.append(axes[1][1].boxplot(oc30, vert=True, patch_artist=True, labels=labels))
# axes[1][1].set_title("(d) 30% of PE occupation")



# plot violin plot
boxplot.append(axes[1].boxplot(oc70, vert=True, patch_artist=True, labels=labels))
axes[1].set_title("(b) 70% of PE occupation")

# # plot violin plot
boxplot.append(axes[0].boxplot(oc50, vert=True, patch_artist=True, labels=labels))
axes[0].set_title("(a) 50% of PE occupation")

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
        col.set_ylim(50,85)


# add x-tick labels
# plt.setp(axes, xticks=[y+1 for y in range(len(oc90))],
#             xticklabels=['W', 'P', 'PID-M', 'PID', 'C-M', 'C' ])
fig.savefig('candle_temp.png', format='png', dpi=300, bbox_inches='tight')
fig.savefig('candle_temp.pdf', format='pdf', bbox_inches='tight')
#plt.show()

