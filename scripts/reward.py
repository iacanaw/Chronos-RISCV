# if(reward >= 1.0){
#     reward = 1000 / reward;
#     printsvsv("addr ", addr, "woned reward1: ", (int)reward);
# } else if( reward < 1.0 && reward > 0.0 ){
#     reward = 1000 + (reward * 10);
#     printsvsv("addr ", addr, "woned reward2: ", (int)reward);
# } else {
#     reward = 1000 + 100 * (reward * -1);
#     printsvsv("addr ", addr, "woned reward3: ", (int)reward);
# }

# Implementation of matplotlib function 
    
from cmath import log
import matplotlib.pyplot as plt
import numpy as np
import math 


# 100 linearly spaced numbers
x = np.linspace(-250,250,10000)

y = np.zeros(10000)
y2 = np.zeros(10000)
y3 = np.zeros(10000)

# the function, which is y = x^2 here
for i in range(len(x)):
    if( x[i] >= 1 ):
        y[i] = 1000 / x[i]
    elif ( x[i] < 1 and x[i] > 0):
        y[i] = 1000 + (x[i] * 10)
    else:
        y[i] = 1000 + 100 * (x[i] * -1)


    if( x[i] >= 1 ):
        y2[i] = 1000 / x[i]
    else:
        y2[i] = 1100 + (-1 * x[i] * 100)

    #y3[i] = (1000 / (1 + 2.71828**(x[i])))
    
    y2[i] =  ((1 / math.sqrt(7000+(x[i])*(x[i]))) * x[i]*-500) + 500          # (x[i]*-500 / math.sqrt(10000+(-1*x[i])**2) + 500)
    
    #y2[i] = (1000 / (1 + 2.71828**(0.02*x[i])))

# setting the axes at the centre
fig = plt.figure()
ax = fig.add_subplot(1, 1, 1)
ax.spines['left'].set_position('center')
ax.spines['bottom'].set_position('zero')
ax.spines['right'].set_color('none')
ax.spines['top'].set_color('none')
ax.xaxis.set_ticks_position('bottom')
ax.yaxis.set_ticks_position('left')

# plot the function
plt.plot(x,y, 'r')
plt.plot(x,y2, 'b')
#plt.plot(x,y3, 'g')

# show the plot
plt.show()
#fig.savefig('reward.png', format='png', dpi=600, bbox_inches='tight')
