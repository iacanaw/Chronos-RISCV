import numpy as np
import logging
import threading
import time
import subprocess
import os
from timeit import default_timer as timer
import matplotlib.pyplot as plt

plt.rcParams['font.family'] = 'serif'
plt.rcParams['font.serif'] = ['Times New Roman'] + plt.rcParams['font.serif']

maxthreads = 12
sema = threading.Semaphore(value=maxthreads)

GRAPH_ORDER = ["worst", "pattern", "pidtm", "chronos"]
EFFECTS = ["EM", "SM", "TDDB", "TC", "NBTI"]
NUM_CAT = 5
bar_width = 1 / (NUM_CAT+1)

def main():

    sim_time_s = [1]
    scenarios_to_sim = ["iscas_90", "iscas_70", "iscas_50", "iscas_30"]
    sizes_to_sim = [8]
    managements_mig = [["worst", ["no"]], ["pattern", ["no"]], ["pidtm", ["yes", "no"]], ["chronos", ["yes", "no"]]]
    name = "ISCAS23_Q3_"
    i = 24

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
        bigPE_folder = []
        masterPE_folder = []
        for folder in graph:
            print("Starting at folder: "+folder)
            with open("../simulation/"+folder+"/simulation/montecarlofile", "r") as fit_file:
                sys_fit = fit_file.readlines()
                sys_size = int(len(sys_fit)/len(EFFECTS))
                print("The system has "+str(sys_size)+" PEs.")
                fits = []
                bigPE = 0
                masterPE = 0
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
                    
                    if masterPE == 0:
                        masterPE = pe_total_fit
                    elif bigPE < pe_total_fit:
                        bigPE = pe_total_fit
                    
                with open("mttf_calc.txt", "+a") as mttffile:
                    mttf = ((10**9)/masterPE) / (365*24)
                    print(folder+" M-MTTF: "+str(mttf).replace(".",",") +" "+str(masterPE).replace(".",","), file=mttffile)
                    mttf = ((10**9)/bigPE) / (365*24)
                    print(folder+" OTHER-MTTF: "+str(mttf).replace(".",",")+" "+str(bigPE).replace(".",","), file=mttffile)

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
        fig = plt.subplots(figsize =(7.85,3),  constrained_layout=True)
        
        X = np.arange(NUM_CAT)

        for k in GRAPH_ORDER:
            for i in range(len(graph)):
                if k in graph[i]:
                    the_label = graph[i].split("_")[7]
                    if "worst" in graph[i]:
                        plt.bar((X + i*bar_width), organized_fits[i], width = bar_width, edgecolor ='black', label="Worst", color="#ef476f", hatch="xx") #f9c80e
    
                    elif "pidtm" in graph[i] and "migyes" in graph[i]:
                        plt.bar((X + i*bar_width), organized_fits[i], width = bar_width, edgecolor ='black', label="PID Mig.", color="#06D6A0", hatch="\\\\\\") #ea3546

                    elif "pidtm" in graph[i] and "migno" in graph[i]: 
                        plt.bar((X + i*bar_width), organized_fits[i], width = bar_width, edgecolor ='black', label="PID", color="#9AFCE2", hatch="///") #F06E7B
                    elif "chronos" in graph[i] and "migyes" in graph[i]:
                        plt.bar((X + i*bar_width), organized_fits[i], width = bar_width, edgecolor ='black', label="FLEA Mig.", color="#118AB2", hatch="//") #43bccd
                    elif "chronos" in graph[i] and "migno" in graph[i]:
                        plt.bar((X + i*bar_width), organized_fits[i], width = bar_width, edgecolor ='black', label="FLEA", color="#98DEF5", hatch="\\\\") #78CFDB
                    else: # "pattern" in graph[i]:
                        plt.bar((X + i*bar_width), organized_fits[i], width = bar_width, edgecolor ='black', label="Pattern", color="#FFD166", hatch="---") #f86624
                    or_fit = []
                    organized_fits_p1 = []
                    for st in organized_fits[i]:
                        or_fit.append(str(st))
                        organized_fits_p1.append(st+1)
                    #print(or_fit)
                    for x, y, p in zip((X + i*bar_width), organized_fits_p1, or_fit):
                        plt.text(x,y,p,ha="center")

        
        #["#f9c80e", "#f86624", "#ea3546", "#F06E7B", "#43bccd", "#78CFDB" ]
        plt.xlabel('FIT categories (interval: '+str(int(interval))+')')#, fontweight ='bold', fontsize = 15)
        plt.ylabel('Number of PEs')#, fontweight ='bold', fontsize = 15)
    
        for i in range(len(categories_low_bound)):
            categories_low_bound[i] = str(int(categories_low_bound[i]))+" - "+str(int(categories_low_bound[i]+interval))
        plt.xticks([r + bar_width*(NUM_CAT/2) for r in range(NUM_CAT)],categories_low_bound)
        plt.title(graph[0].split("_")[4]+"% of PE occupation")
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
        plt.savefig("graphs"+graph[0]+".pdf",  format='pdf', bbox_inches='tight')  



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
