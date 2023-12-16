import numpy as np
import logging
import threading
import time
import subprocess
from timeit import default_timer as timer
from BATCH_thermal_shot import shot_batch
from BATCH_stacked_temp import stacked_batch
from BATCH_candle_fit import violin_batch
from BATCH_candle_fit_FLEA import violin_batch_FLEA
from BATCH_generate_TSV_file import generate_TSV_file
from BATCH_FIT_effects import fit_effects
from BATCH_candle_temp import violin_batch_temp
from BATCH_candle_peak import violin_batch_peak

def main():

    # sim_time_s = [2.0]
    # scenarios_to_sim = [ "_mixed1_70", "_mixed1_90", "_computation_70", "_computation_90", "_mixed2_70", "_mixed2_90", "_mixed1_50", "_computation_50", "_mixed2_50" ]
    # sizes_to_sim = [8]
    # managements_mig = [["worst", ["no"]], ["pattern", ["no"]], ["pidtm", ["no", "yes"]], ["chronos", ["no", "yes"]], ["chronos2", ["no", "yes"]]]
    # name = "SIMULATIONS_NOCL_"
    # i = 0

    # 14x14
    # sim_time_s = [1.5]
    # scenarios_to_sim = [ "_computation_70", "_computation_90", "_misto_70", "_misto_90", "_computation_50", "_misto_50" ]
    # sizes_to_sim = [14]
    # managements_mig = [["worst", ["no"]], ["pattern", ["no"]], ["pidtm", ["no", "yes"]], ["chronos", ["no", "yes"]], ["chronos2", ["no", "yes"]]]
    # name = "SIMULATIONS_"
    # i = 72

    # # 8x8
    # sim_time_s = [2.0]
    # scenarios_to_sim = [ "_mixed1_70", "_mixed1_90", "_computation_70", "_computation_90", "_mixed2_70", "_mixed2_90", "_mixed1_50", "_computation_50", "_mixed2_50" ]
    # sizes_to_sim = [8]
    # managements_mig = [["worst", ["no"]], ["pattern", ["no"]], ["pidtm", ["no", "yes"]], ["chronos", ["no", "yes"]], ["chronos2", ["no", "yes"]]]
    # name = "SIMULATIONS_"
    # i = 0

    # 20x20
    sim_time_s = [1.0]
    scenarios_to_sim = [ "_mist_70", "_mist_90" ]
    sizes_to_sim = [20]
    managements_mig = [["worst", ["no"]], ["pattern", ["no"]], ["pidtm", ["no", "yes"]], ["chronos", ["no", "yes"]], ["chronos2", ["no", "yes"]]]
    name = "SIMULATIONS_"
    i = 200

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
                #try:
                generate_TSV_file(scenarios)
                violin_batch_FLEA(scenarios)
                stacked_batch(scenarios)
                fit_effects(scenarios)
                shot_batch(scenarios, 4)
                violin_batch(scenarios)
                violin_batch_temp(scenarios)
                violin_batch_peak(scenarios)
                    # 
                #break
                # except:
                #     break
#-----------------------------------------------------
if __name__ == '__main__': # chamada da funcao principal
    main()