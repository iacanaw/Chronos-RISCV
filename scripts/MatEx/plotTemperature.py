import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import sys


original_src = sys.argv[1]
chronos_src = sys.argv[2]
tea_src = sys.argv[3]

original_time = []
original_value = []

chronos_time = []
chronos_value = []

tea_time = []
tea_value = []

with open(original_src, "r") as original_file:
    line = ""
    line = original_file.readline()
    while True:
        line = ""
        line = original_file.readline()
        splited = line.split("\t")
        if len(splited) > 2:
            original_time.append(float(splited[0]))
            sum = 0
            for i in range(len(splited)-1):
                sum += float(splited[i+1])
            avg = sum/(len(splited)-1)
            original_value.append(avg)
            #print(avg)
        else:
            break      
        if original_time[-1] > 1.1:
            break

with open(chronos_src, "r") as chronos_file:
    line = ""
    line = chronos_file.readline()
    while True:
        line = ""
        line = chronos_file.readline()
        splited = line.split("\t")
        if len(splited) > 2:
            chronos_time.append(float(splited[0]))
            sum = 0
            for i in range(len(splited)-1):
                sum += float(splited[i+1])
            avg = sum/(len(splited)-1)
            chronos_value.append(avg)
            #print(avg)
        else:
            break
        if chronos_time[-1] > 1.1:
            break

with open(tea_src, "r") as tea_file:
    line = ""
    line = tea_file.readline()
    while True:
        line = ""
        line = tea_file.readline()
        splited = line.split("\t")
        if len(splited) > 2:
            tea_time.append(float(splited[0]))
            sum = 0
            for i in range(len(splited)-1):
                sum += float(splited[i+1])
            avg = sum/(len(splited)-1)
            tea_value.append(avg)
            #print(avg)
        else:
            break 
        if tea_time[-1] > 1.1:
            break

fig, ax = plt.subplots()

ax.plot(chronos_time, chronos_value, 'k-', linewidth=1.0, label="Matex-Chronos")
ax.plot(original_time, original_value, 'r-', linewidth=1.0, label="Matex-Original")
ax.plot(tea_time, tea_value, 'b-', linewidth=1.0, label="TEA")
ax.legend()

fig = plt.gcf()
fig.set_size_inches(18.5, 10.5)
fig.savefig('Temperatures.png', format='png', dpi=600, bbox_inches='tight')