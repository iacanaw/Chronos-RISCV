from copy import deepcopy
import matplotlib.pyplot as plt
import numpy as np
import re
import sys 

XX = int(sys.argv[1])
YY = int(sys.argv[2])

samples = []
samples_time = []

occupancy = []
occupancy_time = []

index = 0
files = 0
for xx in range(XX):
    for yy in range(YY):
        files+=1
        print("\roccupancy.py - Reading File "+str(files)+"/"+str(XX*YY)+" - "+str((files*100)/(XX*YY))+"%", end='')
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
                    try:
                        if value[1].isnumeric() and value[3].isnumeric():
                            empty.append(int(value[1]))
                            time.append(int(value[3]))
                    except:
                        pass
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
        occupancy.append(deepcopy(mean))
        occupancy_time.append(deepcopy(up))
        low = up
        up += 50

max = 1000000
for i in range(len(occupancy)):
    
    if occupancy[i] > max:
        max = occupancy[i]


for i in range(len(occupancy)):
    occupancy[i] = occupancy[i]/max
    sum += occupancy[i]
avg = sum / len(occupancy)
print("\n"+ str(avg))
print("\tUso (%)\t\t||    Ocorrências")
print("========================||========================")
for j in range(10):
    value=0
    for i in range(len(occupancy)):
        if occupancy[i] < (j+1)/10 and occupancy[i] >= j/10:
            value+=1
    print("\t   "+str((j+1)*10)+"   \t||\t"+format(value/len(occupancy)*100, ".3f"))

for i in range(len(occupancy)):
    occupancy[i] = occupancy[i]*100

slotOcc = []
timeSlotOcc = []
PEOcc = []
timePEOcc = []
migStart = []
migEnd = []
migRef = []

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
        elif "it to" in line: #look for "Migrating it to"
            migStart.append(timePEOcc[-1:])
        elif "25NI_RX_DONE" in line:
            migEnd.append(timePEOcc[-1:])
        elif "29NI_RX_DONE" in line:
            migRef.append(timePEOcc[-1:])
        elif line == '':
            break       



fig, ax = plt.subplots()
ax.plot(occupancy_time, occupancy, 'k-', linewidth=1.0, label="Instructions Usage")
#ax.plot(timeSlotOcc, slotOcc, 'r-', linewidth=1.1, label="Slots Occupancy")
ax.plot(timePEOcc, PEOcc, 'b-', linewidth=1.0, label = "PE Occupancy")
ax.vlines(migStart, 0, 100, colors='green', linestyle='dashdot', label="Start Mig")
ax.vlines(migEnd, 0, 100, colors='orange', linestyle="dashed", label="End Mig")
ax.vlines(migRef, 0, 100, colors='red', linestyle="dotted", label="Refused Mig")
ax.legend()
ax.set_ylim([0, 100])
ax.set_title('Occupancy')

fig = plt.gcf()
fig.set_size_inches(18.5, 10.5)
#plt.show()
fig.savefig('simulation/Occupancy.png', format='png', dpi=600, bbox_inches='tight')
