import matplotlib.pyplot as plt
import numpy as np

def forceAspect(ax,aspect=1):
    im = ax.get_images()
    extent =  im[0].get_extent()
    ax.set_aspect(abs((extent[1]-extent[0])/(extent[3]-extent[2]))/aspect)


ovp_minpersec = [2.36, 4.92, 8.98, 14.18, 22.20, 30.99, 47.37, 67.65, 92.46]
sc_minpersec = [147.90, 269.57, 434.21, 653.58, 1026.82, 1549.55, 2172.54, 2846.34, 3731.39]
num_pes = [4, 9, 16, 25, 36, 49, 64, 81, 100]
#scenario = [ [1,0,0], [2,0,0], [1,0,1], [0,1,1], [6,0,1], [2,2,1], [3,2,2], [1,3,3], [2,4,3] ]

# Create figure
fig, ((ax1)) = plt.subplots(1, 1)

# log y axis
ax1.semilogy(num_pes, ovp_minpersec, label='Chronos-V', dashes=[6, 2], color='#de0000', marker='D')#dashes=[2, 2, 10, 2],
ax1.semilogy(num_pes, sc_minpersec, label="Memphis", dashes=[6, 2], color='#0029de', marker='o')

ax1.set_ylabel('Simulation Effort (min/s)')
ax1.set_xlabel('System Size (PEs)')
########################################################

ax1.legend(loc=4, fancybox=True, shadow=True)
ax1.tick_params(which="minor", length=2)
ax1.grid(True, which="both", ls="--", alpha=0.75)

#Anotations
plt.annotate( '', xy=(4, 2.36), xycoords='data', xytext=(4, 147.90), textcoords='data',arrowprops={'arrowstyle': '<->'})
plt.annotate( '62,56x', xy=(4, 14), xycoords='data', xytext=(3, 3), textcoords='offset points')

plt.annotate( '', xy=(100, 92.46), xycoords='data', xytext=(100, 3731.39), textcoords='data',arrowprops={'arrowstyle': '<->'})
plt.annotate( '40,35x', xy=(90, 1000), xycoords='data', xytext=(3, 3), textcoords='offset points')

# for i in range(2,9):
    # ax1.plot(num_pes[i], sc_minpersec[i], 'o', ms=12, color='green', mfc='none', mew=2, alpha=0.5)
#ax1.set_title("Simulation Effort")

fig.tight_layout()
ax1.set_aspect(12)
plt.savefig('graphs1.pdf')

plt.show()