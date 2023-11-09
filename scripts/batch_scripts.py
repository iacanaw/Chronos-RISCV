import numpy as np
import logging
import threading
import time
import subprocess
from timeit import default_timer as timer
from thermal_shot_batch import shot_batch
from stacked_temp_batch import stacked_batch
from candle_fit_batch import violin_batch

def main():

    sim_time_s = [2.0]
    scenarios_to_sim = [ "_mixed1_70", "_mixed1_90", "_computation_70", "_computation_90", "_mixed2_70", "_mixed2_90", "_mixed1_50", "_computation_50", "_mixed2_50" ]
    sizes_to_sim = [8]
    managements_mig = [["worst", ["no"]], ["pattern", ["no"]], ["pidtm", ["no", "yes"]], ["chronos", ["no", "yes"]], ["chronos2", ["no", "yes"]]]
    name = "SIMULATIONS_"
    i = 0

    # Creating Threads to run MATEX and Graphs
    for time in sim_time_s:
        for size in sizes_to_sim:
            for scenario in scenarios_to_sim:
                scenario = str(size)+scenario
                scenarios = []
                labels = []
                for mm in managements_mig:
                    for mig in mm[1]:
                        str_name = name+str(i)
                        method = mm[0]
                        scenarios.append(str_name+"_"+scenario+"_"+str(time*10000)+"ticks_mig"+mig+"_"+method+"_"+str(size)+"x"+str(size))
                        method = "FLEA" if method == "chronos" else "FLEA2" if method == "chronos2" else method.upper()
                        label = method+"+MIG" if mig == "yes" else method.upper()
                        labels.append(label)
                        i+=1
                print(scenarios)
                try:
                    shot_batch(scenarios, labels)
                    stacked_batch(scenarios, labels)
                    violin_batch(scenarios, labels)
                #break
                except:
                    break
#-----------------------------------------------------
if __name__ == '__main__': # chamada da funcao principal
    main()