# libraries
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
plt.rcParams['font.family'] = 'serif'
plt.rcParams['font.serif'] = ['Times New Roman'] + plt.rcParams['font.serif']

 # width of the bars
barWidth = 0.25
 
barsIdle = [1, 1, 1, 1, 1]
barsBorda = [3.90, 2.74, 1.61, 4.77, 1.69]
barsDiag = [2.43, 1.84, 1.33, 2.85, 1.37]
 
# The x position of bars
rIdle = np.arange(len(barsIdle))
rBorda = [x + barWidth for x in rIdle]
rDiag = [x + barWidth for x in rBorda]


# Create blue bars
plt.bar(rIdle, barsIdle, width = barWidth, color = '#002855', edgecolor = 'black', capsize=7, label='Without near Load')
 
# Create cyan bars
plt.bar(rBorda, barsBorda, width = barWidth, color = '#7E1717', edgecolor = 'black', capsize=7, label='Border Load')
 
# Create cyan bars
plt.bar(rDiag, barsDiag, width = barWidth, color = "#E55807", edgecolor = 'black', capsize=7, label='Diagonal Load')

# general layout
plt.xticks([r + barWidth for r in range(len(barsIdle))], ['EM', 'SM', 'TDDB', 'TC', 'NBTI'])
plt.ylabel('Normalized Failures in Time (FIT)')
plt.legend()

plt.grid(alpha=0.25, which='both')
# Show graphic
#plt.show()
plt.tight_layout()
plt.savefig('graph_motivation.pdf')
plt.savefig('graph_motivation.png')