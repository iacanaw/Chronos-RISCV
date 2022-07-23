from copy import deepcopy
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import sys 

time_step = float(sys.argv[1])
systemX = int(sys.argv[2])
systemY = int(sys.argv[2])

system_size = systemX*systemY
time = 0

with open("power.pwr", "w") as prw_file:

    print("time", file=prw_file, end='')
    for j in range(system_size):
        print("\tp"+str(j), file=prw_file, end='')
    print("\n", file=prw_file, end='')

    with open("matex.txt", "r") as log_file:
        while True:
            line = ""
            try:
                line = log_file.readline()
            except:
                print("catched an error in file")
                exit()
            if "Power:" in line:
                splited = line.split(' ')
                
                time+=time_step
                print(str("{:.6f}".format(time)), file=prw_file, end='')
                for j in range(system_size):
                    print("\t"+splited[j+1], file=prw_file, end='')
                print("\n",  file=prw_file, end='')
            if line == "":
                exit()