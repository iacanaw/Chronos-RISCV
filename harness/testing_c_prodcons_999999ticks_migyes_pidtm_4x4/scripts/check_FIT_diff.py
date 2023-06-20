from curses import termattrs
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import sys

plt.close("all")
plt.style.use('_mpl-gallery')

tsv_data = pd.read_csv("simulation/FITlog.tsv", sep='\t')
raw_data = tsv_data.to_numpy()

time = []
samples = np.zeros((len(raw_data[0])-1, len(raw_data)))

n_pes = len(raw_data[0])-1
n_samples = len(raw_data)

delta = samples = np.zeros((n_pes, n_samples-5))

for i in range(5, n_samples):
    for j in range(len(raw_data[i])-1):
        delta[j][i-5] = raw_data[i][j+1] - raw_data[i-5][j+1]

for j in range(n_pes):
    print(str(j)+" min: "+str(delta[j].min())+" max: "+str(delta[j].max()))


# fig, ax = plt.subplots()

# ax.plot(time, avg_temp, 'y-', linewidth=0.85, label = "Average Temperature")    
# ax.plot(time, max_temp, 'r-', linewidth=1.0, label = "Maximum Temperature")
# ax.plot(time, min_temp, 'b-', linewidth=1.0, label = "Minimum Temperature")
# ax.plot(time, mediana, 'g-', linewidth=1.0, label = "Median Temperature")
# ax.plot(time, pri_quartil, 'c-', linewidth=1.0, label = "First Quartile")
# ax.plot(time, ter_quartil, 'm-', linewidth=1.0, label = "Third Quartile")

# ax.set_ylim([50, 90])

# ax.set_title('Temperature - '+simType)
# ax.legend()
# fig = plt.gcf()
# fig.set_size_inches(18.5, 10.5)
# fig.savefig('simulation/PeakTemperature.png', format='png', dpi=300, bbox_inches='tight')
# fig.savefig('simulation/PeakTemperature.eps', format='eps', bbox_inches='tight')

# with open("simulation/ThermalStatistics.csv", "w") as TSfile:
#     print(";Average;STD", file=TSfile)
#     print("MAX;"+str(max_temp.mean()).replace(".", ",")+";"+str(max_temp.std()).replace(".", ","), file=TSfile)
#     print("MIN;"+str(min_temp.mean()).replace(".", ",")+";"+str(min_temp.std()).replace(".", ","), file=TSfile)
#     print("MEDIAN;"+str(mediana.mean()).replace(".", ",")+";"+str(mediana.std()).replace(".", ","), file=TSfile)
#     print("AVERAGE;"+str(avg_temp.mean()).replace(".", ",")+";"+str(avg_temp.std()).replace(".", ","), file=TSfile)
#     print("1 QUARTILE;"+str(pri_quartil.mean()).replace(".", ",")+";"+str(pri_quartil.std()).replace(".", ","), file=TSfile)
#     print("3 QUARTILE;"+str(ter_quartil.mean()).replace(".", ",")+";"+str(ter_quartil.std()).replace(".", ","), file=TSfile)
