import numpy as np
import logging
import threading
import time
import subprocess
import os
from timeit import default_timer as timer
import matplotlib.pyplot as plt
import matplotlib.lines as lines
from mpl_toolkits.axes_grid1 import make_axes_locatable
from matplotlib.colors import LogNorm
from matplotlib.ticker import MultipleLocator
import pandas as pd
import math 


def main():

    sim_time_s = [2.5]
    scenarios_to_sim = ["70occ", "50occ", "90occ"]
    sizes_to_sim = [3, 6, 8, 11]#, 14]
    managements_mig = [["worst", ["no"]], ["pattern", ["no"]], ["pidtm", ["no", "yes"]], ["chronos", ["no", "yes"]]]
    clusters = ["fit", "temp", "tasks", "clusterless"]
    name = "TESE_TAB3535_"
    i = 0

    # Creating Threads to run MATEX and Graphs
    for time in sim_time_s:
        for size in sizes_to_sim:
            for scenario in scenarios_to_sim:
                # print("-----\nIniciando geração dos dados\nTempo da simulação "+str(time)+" segundos\nSistema "+str(size*size)+" PEs\nScenario: "+str(size)+"_"+scenario)
                directory = "data/"+name+"_"+str(time)+"_"+str(size*size)+"_"+str(size)+"_"+scenario
                
                try:
                    os.mkdir(directory)
                except:
                    print(directory+" Diretório já existente!")
                
                sim_folders = []
                                
                scenario_name = str(size)+"_"+scenario
                for mm in managements_mig:
                    for mig in mm[1]:
                        method = mm[0]
                        if(mm[0] == "chronos"):
                            for cluster in clusters:
                                str_name = name+str(i)
                                sim_folders.append(name+str(i)+"_"+str(size)+"_"+scenario+"_"+str(time*10000)+"ticks_mig"+mig+"_"+method+"_"+str(size)+"x"+str(size))
                                i+=1
                        else:
                            str_name = name+str(i)
                            sim_folders.append(name+str(i)+"_"+str(size)+"_"+scenario+"_"+str(time*10000)+"ticks_mig"+mig+"_"+method+"_"+str(size)+"x"+str(size))
                            i+=1
                
                for folder in sim_folders:
                    if(not os.path.isdir("../simulation/"+folder)):
                        print(folder+"\nNÃO EXISTE - Finalizando o programa.")
                        exit()

                thermal_shot(directory, sim_folders)

def thermal_shot(directory, folders):
    clusters = ["FIT", "Temp", "Tasks", "Clusterless"]
    clust = 0
    # Inicialização dos gráficos
    plt.rcParams['font.family'] = 'serif'
    plt.rcParams['font.serif'] = ['Times New Roman'] + plt.rcParams['font.serif']

    # definir tamanho do gráfico
    print("Numero de folders: "+str(len(folders)))
    n_colunas = 5
    n_linhas = int(math.ceil(len(folders)/n_colunas))
    fig, axes = plt.subplots(nrows=n_linhas, ncols=n_colunas, sharex=True, sharey=True,  figsize=(12*n_linhas,3*n_colunas), constrained_layout=True)

    #definir as labels
    labels = []
    colors = []
    for folder in folders:
        if "worst" in folder:
            labels.append("Grouped")
            colors.append("#FB5607")
        elif "pattern" in folder:
            labels.append("Pattern")
            colors.append("#FFBE0B")
        elif "pidtm" in folder:
            if "migyes" in folder:
                labels.append("PID+Mig")
                colors.append("#8338EC")
            elif "migno" in folder:
                labels.append("PID")
                colors.append("#E139ED")
            else:
                print("Erro - 1")
                exit()
        elif "chronos" in folder:
            if "migyes" in folder:
                labels.append("FLEA+Mig+"+clusters[clust%4])
                colors.append("#3A86FF")
            elif "migno" in folder:
                labels.append("FLEA+"+clusters[clust%4])
                colors.append("#3BC9FF")
            else:
                print("Erro - 2")
                exit()
            clust+=1
        else:
            print("Erro - 3")
            exit()
    
    k = 0
    l = 0
    for folder in folders:
        tsv_data = pd.read_csv("../simulation/"+folder+"/simulation/SystemTemperature.tsv", sep='\t')
        raw_data = tsv_data.to_numpy()

        generalMax = 0
        sysSize = len(raw_data[0])-1
        side = int(math.sqrt(sysSize))
        picMax = np.zeros((side,side))

        picAvg = np.zeros((side,side))
        nsamples = 0

        time = []
        samples = np.zeros((len(raw_data[0])-1, len(raw_data)))

        n_pes = len(raw_data[0])-1
        n_samples = len(raw_data)

        max_temp = np.zeros(n_samples)
        sample = np.zeros(n_pes)

        for i in range(len(raw_data)):
            time.append(raw_data[i][0])
            nsamples += 1
            for j in range(len(raw_data[i])-1):
                sample[j] = raw_data[i][j+1]
                picAvg[int(j%side)][int(j/side)] += sample[j]
                if generalMax < sample[j]:
                    generalMax = sample[j]
                    uj = 1
                    for ux in range(side):
                        for uy in range(side):
                            picMax[ux][uy] = raw_data[i][uj]
                            uj+=1

            sample.sort()
            max_temp[i] = sample[n_pes-1]
        
        for x in range(side):
            for y in range(side):
                picAvg[x][y] = picAvg[x][y] / nsamples


        # https://matplotlib.org/stable/gallery/images_contours_and_fields/interpolation_methods.html -> more interpolation methods
        # https://matplotlib.org/stable/tutorials/colors/colormaps.html                   -> more colors
        pcm = axes[l][k].imshow(picAvg, interpolation='gaussian', aspect='auto', vmin=50, vmax=generalMax, cmap='jet') # YlOrRd, jet, turbo

        side = int(math.sqrt(n_pes))
        # Major ticks
        axes[l][k].set_xticks(np.arange(0, side, 1))
        axes[l][k].set_yticks(np.arange(0, side, 1))

        # Labels for major ticks
        axes[l][k].set_xticklabels(np.arange(0, side, 1))
        axes[l][k].set_yticklabels(np.arange(0, side, 1))

        # Minor ticks
        axes[l][k].set_xticks(np.arange(-.5, side-1, 1), minor=True)
        axes[l][k].set_yticks(np.arange(-.5, side-1, 1), minor=True)

        #Title
        axes[l][k].set_title(labels[(l*n_colunas)+k])

        # Gridlines based on minor ticks
        axes[l][k].grid(which='minor', color='black', linestyle='-', linewidth=1)
        axes[l][k].tick_params(axis='both', which='major', labelsize=8)

        k+=1
        if(k==n_colunas):
            k = 0
            l+=1

    fig.colorbar(pcm, ax=axes[:], location='right', shrink=1.0, label="Temperature (°C)")
    #fig.suptitle('Average PE Temperature - 70% System Occupation', fontsize=16)

    fig.savefig(directory+"/thermal_shot.png", format='png', dpi=300, bbox_inches='tight')
    fig.savefig(directory+"/thermal_shot.pdf", format='pdf', bbox_inches='tight')    
    return

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