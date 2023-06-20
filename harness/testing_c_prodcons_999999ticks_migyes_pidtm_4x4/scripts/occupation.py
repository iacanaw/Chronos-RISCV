from copy import deepcopy
import matplotlib.pyplot as plt
import numpy as np
import re
import sys 

XX = int(sys.argv[1])
YY = int(sys.argv[2])

samples = []
samples_time = []

occupation = []
occupation_time = []

index = 0
for xx in range(XX):
    for yy in range(YY):
        #print("reading file " + str(xx) + "x" + str(yy))
        empty = []
        time = []
        with open("simulation/log_"+str(xx)+"x"+str(yy)+".txt", "r") as log_file:
            while True:
                line = ""
                try:
                    line = log_file.readline()
                except:
                    pass#print("catched an error in file "+ str(xx) + "x" + str(yy))
                if "inst~~~>" in line:
                    value = line.split(' ')
                    if value[1].isnumeric() and value[3].isnumeric():
                        empty.append(int(value[1]))
                        time.append(int(value[3]))
                elif line == '':
                    break
            samples.append(deepcopy(empty))
            samples_time.append(deepcopy(time))
    index += 1


low = 0
up = 50
while True:
    match = 0
    sum = 0
    for i in range(len(samples_time)):
        for j in range(len(samples_time[i])-1):
            if(samples_time[i][j+1] > low and samples_time[i][j+1] <= up):
                sum += samples[i][j+1]
                match += 1
    
    if match == 0:
        break
    else:
        mean = sum / match
        occupation.append(deepcopy(mean))
        occupation_time.append(deepcopy(up))
        low = up
        up += 50

max = 1000000
for i in range(len(occupation)):
    
    if occupation[i] > max:
        max = occupation[i]


for i in range(len(occupation)):
    occupation[i] = occupation[i]/max
    sum += occupation[i]
avg = sum / len(occupation)
print(avg)
print("\tUso (%)\t\t||    Ocorrências")
print("========================||========================")
for j in range(10):
    value=0
    for i in range(len(occupation)):
        if occupation[i] < (j+1)/10 and occupation[i] >= j/10:
            value+=1
    print("\t   "+str((j+1)*10)+"   \t||\t"+format(value/len(occupation)*100, ".3f"))

for i in range(len(occupation)):
    occupation[i] = occupation[i]*100

slotOcc = []
timeSlotOcc = []
PEOcc = []
timePEOcc = []

with open("simulation/log_0x0.txt", "r") as log_file:
    while True:
        line = ""
        try:
            line = log_file.readline()
        except:
            pass#print("catched an error in file "+ str(xx) + "x" + str(yy))
        if "---->Slot_occ" in line:
            value = line.split(' ')
            if value[1].isnumeric() and value[3].isnumeric():
                slotOcc.append(int(value[1]))
                timeSlotOcc.append(int(value[3]))
        elif "---->PE_occ:" in line:
            value = line.split(' ')
            if value[1].isnumeric() and value[3].isnumeric():
                PEOcc.append(int(value[1]))
                timePEOcc.append(int(value[3]))
        elif line == '':
            break


fig, ax = plt.subplots()
ax.plot(occupation_time, occupation, 'k-', linewidth=1.0, label="Instructions Usage")
ax.plot(timeSlotOcc, slotOcc, 'r-', linewidth=1.1, label="Slots Occupation")
ax.plot(timePEOcc, PEOcc, 'b-', linewidth=1.0, label = "PE Occupation")
ax.legend()
ax.set_ylim([0, 100])
ax.set_title('Occupation')

fig = plt.gcf()
fig.set_size_inches(18.5, 10.5)
#plt.show()
fig.savefig('simulation/Occupation.png', format='png', dpi=600, bbox_inches='tight')
