import numpy as np
import logging
import threading
import time
import subprocess

maxthreads = 7
sema = threading.Semaphore(value=maxthreads)

def main():

    scenarios_to_sim = ["test1_79tasks_deadline", "test2_61tasks_deadline"]
    sizes_to_sim = [9, 10, 11]
    managements_mig = [["pidtm", ["yes", "no"]], ["chronos", ["yes", "no"]], ["pattern", ["no"]]]
    sim_time_s = [1, 10]
    name = "batch_script_"
    i = 0

    # Creating Threads to run MATEX and Graphs
    for time in sim_time_s:
        for size in sizes_to_sim:
            for scenario in scenarios_to_sim:
                for mm in managements_mig:
                    for mig in mm[1]:
                        str_name = name+str(i)
                        i+=1
                        method = mm[0]
                        x = threading.Thread(target=run_ovp_sim, args=(str_name, time, size, scenario, method, mig,))
                        x.start()
    

def run_ovp_sim(name, time, size, scenario, mm, mig):
    sema.acquire()
    print("Starting simulation "+name)
    bashCommand = "./RUN_FreeRTOS.sh -x "+str(size)+" -y "+str(size)+" -t "+str(time*10000)+" -m "+mm+" -s "+scenario+" -n "+name+" -g "+mig+" &"
    print(bashCommand)
    #process = subprocess.Popen(bashCommand.split(), stdout=subprocess.PIPE)
    #output, error = process.communicate()
    sema.release()


#-----------------------------------------------------
if __name__ == '__main__': # chamada da funcao principal
    main()