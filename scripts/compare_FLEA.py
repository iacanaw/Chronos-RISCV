import matplotlib
import matplotlib.pyplot as plt
import numpy as np


def autolabel(rects):
    """Attach a text label above each bar in *rects*, displaying its height."""
    for rect in rects:
        height = rect.get_height()
        ax.annotate('{}'.format(height),
                    xy=(rect.get_x() + rect.get_width() / 2, height),
                    xytext=(0, 3),  # 3 points vertical offset
                    textcoords="offset points",
                    ha='center', va='bottom')

plt.rcParams['font.family'] = 'serif'
plt.rcParams['font.serif'] = ['Times New Roman'] + plt.rcParams['font.serif']

labels = ['MIXED 50%', 'MIXED 70%', 'MIXED 90%', 'COMP 50%', 'COMP 70%', 'COMP 90%']
men_means = [7.88, 5.24, 3.94, 5.04, 4.30, 3.53] # 8x8 FLEA
women_means = [7.92, 5.19, 3.93, 5.26, 4.36, 3.40] # 8x8 FLEA+

# men_means = [4.35, 3.20, 2.87, 2.58, 1.67, 1.04] # 14x14 FLEA
# women_means = [4.33, 3.18, 2.83, 2.59, 1.7, 1.07] # 14x14 FLEA+ 

x = np.arange(len(labels))  # the label locationss
width = 0.35  # the width of the bars

fig, ax = plt.subplots(figsize =(8,3))
rects1 = ax.bar(x - width/2, men_means, width, label='FLEA')
rects2 = ax.bar(x + width/2, women_means, width, label='FLEA+')

# Add some text for labels, title and custom x-axis tick labels, etc.
ax.set_ylabel('MTTF (years)')
ax.set_title('Comparison 8x8 scenarios')
# ax.set_title('Comparison 14x14 scenarios')
ax.set_xticks(x)
ax.set_xticklabels(labels)
ax.legend()
ax.set_ylim(0,10)

autolabel(rects1)
autolabel(rects2)

fig.tight_layout()
plt.xticks(rotation=0)


fig.savefig("MTTF_8_COMPARE.png", format='png', dpi=300, bbox_inches='tight')
fig.savefig("MTTF_8_COMPARE.pdf", format='pdf', bbox_inches='tight')




