import numpy as np
import logging
import threading
import time
import subprocess
from timeit import default_timer as timer

maxthreads = 8
sema = threading.Semaphore(value=maxthreads)

def main():

    sim_time_s = [2.0]
    scenarios_to_sim = [ "_mixed1_70", "_computation_70", "_mixed2_70", "_mixed1_90", "_computation_90", "_mixed2_90", "_mixed1_50", "_computation_50", "_mixed2_50" ]
    sizes_to_sim = [8]
    managements_mig = [["worst", ["no"]], ["pattern", ["no"]], ["pidtm", ["no", "yes"]], ["chronos", ["no", "yes"]], ["chronos2", ["no", "yes"]]]
    name = "SIMULATIONS_"
    i = 0

    # Creating Threads to run MATEX and Graphs
    for simtime in sim_time_s:
        for size in sizes_to_sim:
            for scenario in scenarios_to_sim:
                scenario = str(size)+scenario
                for mm in managements_mig:
                    for mig in mm[1]:
                        str_name = name+str(i)
                        method = mm[0]
                        # if(mm[0] == "chronos" or mm[0] == "chronos2" or i >= 26 ):
                        x = threading.Thread(target=run_ovp_sim, args=(str_name, simtime, size, scenario, method, mig,))
                        x.start()
                        # sleeptime = int(math.sqrt(size)*1.5*60)
                        # print("waiting for "+str(sleeptime)+" seconds...")
                        sema.acquire()
                        time.sleep(60)
                        sema.release()
                        
                        i+=1
                            

def run_ovp_sim(name, simtime, size, scenario, mm, mig):
    sema.acquire()
    print("Starting simulation "+name)
    i = 0
    while True:
        start = timer()
        log = open(name+"_"+str(i)+'.log', 'w')
        i+=1
        bashCommand = "./RUN_FreeRTOS.sh -x "+str(size)+" -y "+str(size)+" -t "+str(simtime*10000)+" -m "+mm+" -s "+scenario+" -n "+name+" -g "+mig
        print(bashCommand)
        process = subprocess.Popen(bashCommand, stdout=log, stderr=log, shell=True)
        process.wait()
        output, error = process.communicate()
        end = timer()
        print(str(end-start))
        if (end - start) > size*1.5*60: 
            break
    sema.release()

#-----------------------------------------------------
if __name__ == '__main__': # chamada da funcao principal
    main()