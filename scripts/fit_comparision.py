import numpy as np
import logging
import threading
import time
import subprocess
import os
from timeit import default_timer as timer
import matplotlib.pyplot as plt


maxthreads = 12
sema = threading.Semaphore(value=maxthreads)

GRAPH_ORDER = ["worst", "pattern", "pidtm", "chronos"]
EFFECTS = ["EM", "SM", "TDDB", "TC", "NBTI"]
NUM_CAT = 5
bar_width = 1 / (NUM_CAT+1)

def main():

    sim_time_s = [1]
    scenarios_to_sim = ["test3_70ocup", "test3_50ocup", "test3_90ocup", "test3_30ocup"]
    sizes_to_sim = [8]
    managements_mig = [["worst", ["no"]], ["pidtm", ["yes", "no"]], ["chronos", ["yes", "no"]], ["pattern", ["no"]]]
    name = "batch_script_"
    i = 20

    graphs = []
    threads = []
    # Creating Threads to run MATEX and Graphs
    for time in sim_time_s:
        for size in sizes_to_sim:
            for scenario in scenarios_to_sim:
                graph = [] 
                for mm in managements_mig:
                    for mig in mm[1]:
                        str_name = name+str(i)
                        method = mm[0]
                        foldername = str_name+"_"+scenario+"_"+str(time*10000)+"ticks_mig"+mig+"_"+method+"_"+str(size)+"x"+str(size)                        
                        #x = threading.Thread(target=run_ramp, args=(foldername, size, size,))
                        #x.start()
                        #threads.append(x)
                        graph.append(foldername)
                        i+=1
                graphs.append(graph)
        
    for graph in graphs:
        # for each graph...
        small_fit = 9999999999
        big_fit = 0
        fit_values = []
        bar_width = 1 / (len(graph)+1)
        for folder in graph:
            print("Starting at folder: "+folder)
            with open("../simulation/"+folder+"/simulation/montecarlofile", "r") as fit_file:
                sys_fit = fit_file.readlines()
                sys_size = int(len(sys_fit)/len(EFFECTS))
                print("The system has "+str(sys_size)+" PEs.")
                fits = []
                for pe in range(sys_size):
                    # saves the fits
                    pe = [ float(sys_fit[pe*5]), float(sys_fit[(pe*5)+1]), float(sys_fit[(pe*5)+2]), float(sys_fit[(pe*5)+3]), float(sys_fit[(pe*5)+4]) ]
                    fits.append(pe)
                    
                    # update the graph limits
                    pe_total_fit = pe[0] + pe[1] + pe[2] + pe[3] + pe[4]
                    if(pe_total_fit > big_fit):
                        big_fit = pe_total_fit
                    if(pe_total_fit < small_fit):
                        small_fit = pe_total_fit
            # saves the folder fits
            fit_values.append(fits)
        
        print("Big: "+str(big_fit))
        print("Small: "+str(small_fit))
        interval = (big_fit - small_fit) / NUM_CAT
        print("Interval: "+str(interval))

        categories_low_bound = []
        for i in range(NUM_CAT):
            categories_low_bound.append(small_fit + interval*i)
        
        print(categories_low_bound)
        
        organized_fits = []
        for fits in fit_values:
            organized = []
            for i in range(NUM_CAT):
                organized.append(0)
            for pe_fit in fits:
                pe_total_fit =  pe_fit[0] + pe_fit[1] + pe_fit[2] + pe_fit[3] + pe_fit[4]
                for i in range(NUM_CAT):
                    if (pe_total_fit > categories_low_bound[i] and pe_total_fit <= (categories_low_bound[i]+interval)):
                        organized[i]+=1
            print(organized)
            organized_fits.append(organized)
        fig = plt.subplots(figsize =(12, 8))
        
        X = np.arange(NUM_CAT)

        for k in GRAPH_ORDER:
            for i in range(len(graph)):
                if k in graph[i]:
                    the_label = graph[i].split("_")[7]
                    if "worst" in graph[i]:
                        plt.bar((X + i*bar_width), organized_fits[i], width = bar_width, edgecolor ='black', label=the_label, color='darkred')
                    elif "pidtm" in graph[i] and "migyes" in graph[i]:
                        plt.bar((X + i*bar_width), organized_fits[i], width = bar_width, edgecolor ='black', label=the_label+" mig", color='darkorange')
                    elif "pidtm" in graph[i] and "migno" in graph[i]:
                        plt.bar((X + i*bar_width), organized_fits[i], width = bar_width, edgecolor ='black', label=the_label, color='bisque')
                    elif "chronos" in graph[i] and "migyes" in graph[i]:
                        plt.bar((X + i*bar_width), organized_fits[i], width = bar_width, edgecolor ='black', label=the_label+" mig", color='darkblue')
                    elif "chronos" in graph[i] and "migno" in graph[i]:
                        plt.bar((X + i*bar_width), organized_fits[i], width = bar_width, edgecolor ='black', label=the_label, color='cornflowerblue')
                    else: # "pattern" in graph[i]:
                        plt.bar((X + i*bar_width), organized_fits[i], width = bar_width, edgecolor ='black', label=the_label, color='forestgreen')
                        
        
        plt.xlabel('FIT CATEGORIES', fontweight ='bold', fontsize = 15)
        plt.ylabel('PE Count', fontweight ='bold', fontsize = 15)
    
        for i in range(len(categories_low_bound)):
            categories_low_bound[i] = str(int(categories_low_bound[i]))+" - "+str(int(categories_low_bound[i]+interval))
        plt.xticks([r + bar_width*(NUM_CAT/2) for r in range(NUM_CAT)],categories_low_bound)
        plt.title(graph[0].split("_")[4])
        plt.legend()
        ax = plt.gca()

        x = int((graph[0].split("_"))[len(graph[0].split("_"))-1].split("x")[0])
        y = int((graph[0].split("_"))[len(graph[0].split("_"))-1].split("x")[1])
        ax.set_ylim([0, x*y])
        ax.spines['top'].set_visible(False)
        ax.spines['right'].set_visible(False)
        ax.spines['left'].set_visible(False)
        ax.spines['bottom'].set_color('#DDDDDD')
        ax.tick_params(bottom=False, left=True)
        ax.set_axisbelow(True)
        ax.yaxis.grid(True, color='#EEEEEE')
        ax.xaxis.grid(False)
        

        fig = plt.gcf()

        plt.savefig("graphs"+graph[0]+".png")  



def run_ramp(folder, x, y):
    sema.acquire()
    print("Running RAMP at folder: simulation/"+folder)
    bashCommand = "cd ../simulation/"+folder+"; ./scripts/RunRAMP.sh "+str(x)+" "+str(y)
    process = subprocess.Popen(bashCommand, shell=True)
    process.wait()
    sema.release()

#-----------------------------------------------------
if __name__ == '__main__': # chamada da funcao principal
    main()
