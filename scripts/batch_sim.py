import numpy as np
import logging
import threading
import time
import subprocess
from timeit import default_timer as timer


maxthreads = 1
sema = threading.Semaphore(value=maxthreads)

def main():

    #scenarios_to_sim = ["c_prodcons"]
    #sizes_to_sim = [2]
    #managements_mig = [["pidtm", ["no"]]]
    #sim_time_s = [1]

    sim_time_s = [1]
    scenarios_to_sim = ["test3_70ocup", "test3_50ocup", "test3_90ocup", "test3_30ocup"]
    sizes_to_sim = [8]
    managements_mig = [["worst", ["no"]], ["pattern", ["no"]], ["pidtm", ["yes", "no"]], ["chronos", ["yes", "no"]]]
    name = "batch_script_"
    i = 200

    # Creating Threads to run MATEX and Graphs
    for time in sim_time_s:
        for size in sizes_to_sim:
            for scenario in scenarios_to_sim:
                for mm in managements_mig:
                    for mig in mm[1]:
                        str_name = name+str(i)
                        method = mm[0]
                        if( method == "chronos" ):
                            x = threading.Thread(target=run_ovp_sim, args=(str_name, time, size, scenario, method, mig,))
                            x.start()
                        i+=1
                            
    

def run_ovp_sim(name, time, size, scenario, mm, mig):
    sema.acquire()
    print("Starting simulation "+name)
    while True:
        start = timer()
        bashCommand = "./RUN_FreeRTOS.sh -x "+str(size)+" -y "+str(size)+" -t "+str(time*10000)+" -m "+mm+" -s "+scenario+" -n "+name+" -g "+mig
        #print(bashCommand)
        process = subprocess.Popen(bashCommand, shell=True)
        process.wait()
        output, error = process.communicate()
        end = timer()
        print(str(end-start))
        if (end - start) > 10*60: # 10min in seconds
            break
    sema.release()

#-----------------------------------------------------
if __name__ == '__main__': # chamada da funcao principal
    main()