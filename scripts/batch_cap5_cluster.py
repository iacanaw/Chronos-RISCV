import numpy as np
import logging
import threading
import time
import subprocess
from timeit import default_timer as timer


maxthreads = 6
sema = threading.Semaphore(value=maxthreads)

def main():

    sim_time_s = [2.5]
    scenarios_to_sim = ["70occ", "50occ", "90occ"]
    sizes_to_sim = [3, 6, 8, 11] #, 14]
    managements_mig = [["worst", ["no"]], ["pattern", ["no"]], ["pidtm", ["no", "yes"]], ["chronos", ["no", "yes"]]]
    clusters = ["fit", "temp", "tasks", "clusterless"]
    name = "TESE_TAB3535_"
    i = 0

    # Creating Threads to run MATEX and Graphs
    for time in sim_time_s:
        for size in sizes_to_sim:
            for scenario in scenarios_to_sim:
                scenario = str(size)+"_"+scenario
                for mm in managements_mig:
                    for mig in mm[1]:
                        method = mm[0]
                        
                        if(mm[0] == "chronos"):
                            for cluster in clusters:
                                str_name = name+str(i)
                                if(mig == "yes"):
                                    x = threading.Thread(target=run_ovp_sim, args=(str_name, time, size, scenario, method, mig, cluster,))
                                    x.start()
                                i+=1
                        else:
                            str_name = name+str(i)
                            if(mig == "yes" ):
                                x = threading.Thread(target=run_ovp_sim, args=(str_name, time, size, scenario, method, mig, " ", ))
                                x.start()
                            i+=1
                            
    

def run_ovp_sim(name, time, size, scenario, mm, mig, cluster):
    sema.acquire()
    print("Starting simulation "+name)
    while True:
        start = timer()
        if cluster != " ":
            bashCommand = "./RUN_FreeRTOS.sh -x "+str(size)+" -y "+str(size)+" -t "+str(time*10000)+" -m "+mm+" -s "+scenario+" -n "+name+" -g "+mig+" -c "+cluster
        else:
            bashCommand = "./RUN_FreeRTOS.sh -x "+str(size)+" -y "+str(size)+" -t "+str(time*10000)+" -m "+mm+" -s "+scenario+" -n "+name+" -g "+mig
        #print(bashCommand)
        process = subprocess.Popen(bashCommand, shell=True)
        process.wait()
        output, error = process.communicate()
        end = timer()
        print(str(end-start))
        if (end - start) > size*1.5*60: # 10min in seconds
            break
    sema.release()

#-----------------------------------------------------
if __name__ == '__main__': # chamada da funcao principal
    main()