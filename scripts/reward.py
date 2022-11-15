from symbol import power
from matplotlib import pyplot as plt
import numpy as np
#plt.style.use('science')
#plt.style.use(['science','no-latex'])

plt.rcParams['font.family'] = 'serif'
plt.rcParams['font.serif'] = ['Times New Roman'] + plt.rcParams['font.serif']
fig = plt.figure()
fig.set_size_inches(6, 2)
ax = fig.add_axes([0.1,0.1,0.8,0.8])
x = range(-1000, 1000)
y = []
for i in range(len(x)):
    y.append(( 1 / pow( (10000+pow(x[i],2)), 1/2) )*x[i]*-100 +100)


plt.plot(x,y,color="#D07000")

ax.grid(axis='both', alpha=0.3)
ax.set_xlabel(r'$\Delta$'+"FIT")
ax.set_ylabel("r("+r'$\Delta$'+"FIT)")
ax.set_xlim(-500,500)
ax.set_ylim(0,201)

# fig = plt.figure()
# fig.set_size_inches(6, 2)
# ax = fig.add_axes([0.1,0.1,0.8,0.8])
# ax.scatter(-500, 500, color='#95E1D3', marker="^", label=r'$p_{0}$')
# ax.scatter(-500, 500, color='#FCE38A', marker="s", label=r'$p_{1}$')
# ax.scatter(-500, 500, color='#F38181', marker="o", label=r'$p_{2}$')
# for i in range(len(power_trace)):
#     if power_trace[i] < 0.2:
#         ax.scatter(i, power_trace[i], color='#95E1D3', marker="^")
#     elif power_trace[i] < 0.4 : # and power_trace[i] > 2.0 :
#         ax.scatter(i, power_trace[i], color='#FCE38A', marker="s")
#     else:
#         ax.scatter(i, power_trace[i], color='#F38181', marker="o")
# #ax.scatter(x, power_trace, s=lowpower, marker='0')
# ax.set_ylabel("Average Power (W)")

# ax.set_xticks(x)
# # params = {'mathtext.default': 'regular' }          
# # plt.rcParams.update(params)

# ax.set_xticklabels([r'$\tau_{1,1}$',r'$\tau_{1,2}$',r'$\tau_{1,3}$',r'$\tau_{1,4}$',r'$\tau_{1,5}$',r'$\tau_{1,6}$',r'$\tau_{1,7}$',r'$\tau_{1,8}$',\
#                     r'$\tau_{2,1}$',r'$\tau_{2,2}$',r'$\tau_{2,3}$',r'$\tau_{2,4}$',\
#                     r'$\tau_{3,1}$',r'$\tau_{3,2}$',r'$\tau_{3,3}$',r'$\tau_{3,4}$',\
#                     r'$\tau_{4,1}$',r'$\tau_{4,2}$',\
#                     r'$\tau_{5,1}$',r'$\tau_{5,2}$',r'$\tau_{5,3}$',r'$\tau_{5,4}$',r'$\tau_{5,5}$',r'$\tau_{5,6}$',\
#                     r'$\tau_{6,1}$',r'$\tau_{6,2}$',r'$\tau_{6,3}$',r'$\tau_{6,4}$',r'$\tau_{6,5}$',r'$\tau_{6,6}$',r'$\tau_{6,7}$',\
#                     r'$\tau_{7,1}$',r'$\tau_{7,2}$',r'$\tau_{7,3}$',r'$\tau_{7,4}$',r'$\tau_{7,5}$',r'$\tau_{7,6}$',r'$\tau_{7,7}$'])
# for tick in ax.xaxis.get_major_ticks():
#     tick.label.set_fontsize(8) 
#     # specify integer or one of preset strings, e.g.
#     #tick.label.set_fontsize('x-small') 
#     tick.label.set_rotation(65)
# ax.grid(axis='both', alpha=0.3)
# ax.set_xlabel("Task")
# ax.set_xlim(-1,38)
# ax.set_ylim(0.1,0.5)

# ax.fill_between(range(-500, 500), 0, 0.2, alpha=0.1, color="#95E1D3")
# ax.fill_between(range(-500, 500), 0.2, 0.4, alpha=0.1, color="#FCE38A")
# ax.fill_between(range(-500, 500), 0.4, 0.7, alpha=0.1, color="#F38181")
ax.set_title("Reward function")
#ax.legend(loc='center right', bbox_to_anchor=(1.16, 0.5), ncol=1, fancybox=False, shadow=False)
fig.savefig('reward.png', format='png', dpi=300, bbox_inches='tight')
fig.savefig('reward.pdf', format='pdf', bbox_inches='tight')
#plt.show()

