import matplotlib.pyplot as plt
import matplotlib.lines as lines
import numpy as np
import pandas as pd
import os
import time
import re
import subprocess
import csv


def generate_TSV_file(folders):
    time_max = 9999999999

    # for i in range(len(folders)):
    #     folders[i] = "simulation/"+folders[i]

    for folder in folders:
        #tsv_data = pd.read_csv("simulation/"+folder+"/simulation/SystemTemperature.tsv", sep='\t')
        tsv_data = pd.read_csv("simulation/"+folder+"/simulation/SystemTemperature.tsv", sep='\t')
        raw_data = tsv_data.to_numpy()
        if time_max > raw_data[len(raw_data)-1][0]:
            time_max = raw_data[len(raw_data)-1][0]
    print("Generating TSV file...")
    print("Max time found: " + str(time_max))

    for folder in folders:
        if not os.path.exists("simulation/"+folder+"/scripts_data"):
            os.mkdir("simulation/"+folder+"/scripts_data")

    p = []
    for folder in folders:
        x,y = get_x_y(folder)
        
        cut_lines(time_max, "simulation/"+folder+"/simulation/SystemTemperature.tsv", "simulation/"+folder+"/scripts_data/SystemTemperature.tsv")
        cut_lines(time_max, "simulation/"+folder+"/simulation/SystemPower.tsv", "simulation/"+folder+"/scripts_data/SystemPower.tsv")
        new_matex_file("simulation/"+folder+"/scripts_data/SystemPower.tsv", "simulation/"+folder+"/scripts_data/SystemTemperature.tsv", "simulation/"+folder+"/scripts_data/matex.log")
        
        first_char = ''
        try:
            with open("simulation/"+folder+"/scripts_data/mttflog.txt", 'r') as read_obj:
                # read first character
                first_char = read_obj.read(1)
        except:
            pass
        if not os.path.exists("simulation/"+folder+"/scripts_data/mttflog.txt") or not first_char: 
            print("Running RAMP! hold on...")
            p.append(subprocess.Popen(["simulation/"+folder+"/scripts/BATCH_RunRAMP.sh", str(x), str(y), "simulation/"+folder]))

    wait_RAMP(p)

    for folder in folders:
        make_mttf_file("simulation/"+folder)
        make_temp_file("simulation/"+folder)
        make_power_file("simulation/"+folder)
        generate_tsv("simulation/"+folder)
    
    
    with open("batch.tsv", "a") as file:
        for folder in folders:
            with open("simulation/"+folder+"/scripts_data/data.tsv", "r") as in_f:
                #print("simulation/"+folder)
                line = in_f.readline()
                file.write(line+"\n")
        file.write("SIM ID\tPEs	Heuristic\tScenario Name\tAvgOccupancy\tAvgHop\tMigrations\tAvg(Power)\tAvg(Temp)\tAvg(PeakTemp)\tMTTF monte\tMTTF MPE\tMTTF SPE\tMTTF\n")


def generate_tsv(folder):
    # Extract SIM_ID
    sim_id = re.findall(r'SIMULATIONS_(\d+)', folder)[0]
    
    # Extract PEs and calculate total count
    pe_match = re.search(r'(\d+)x(\d+)', folder)
    if pe_match:
        pe1 = int(pe_match.group(1))
        pe2 = int(pe_match.group(2))
        pes = pe1 * pe2
    else:
        pes = None
    
     # Get Heuristic and Scenario
    parts = folder.split('_')
    heuristic = parts[7]
    scenario = '_'.join(parts[3:5])

    try:
        with open(folder+"/simulation/hop.txt", "r") as file:
            hop = float(file.readline())
    except:
        hop = -1
    
    try:
        with open(folder+"/simulation/occupation.txt", "r") as file:
            occupancy = 0
            for line in file:
                try:
                    occupancy = float(line.strip())
                except ValueError:
                    pass
            if occupancy == 0:
                print(folder)
                exit(0)
    except:
        occupancy = -1
    
    try:    
        with open(folder+"/simulation/migs.txt", "r") as file:
            migrations = float(file.readline())
    except:
        migrations = -1

    with open(folder+"/scripts_data/power_data.txt", "r") as file:
        avg_power = float(file.readline().split(' ')[1])
    
    with open(folder+"/scripts_data/temp_data.txt", "r") as file:
        avg_temp = float(file.readline().split(' ')[1])
        avg_peak_temp = float(file.readline().split(' ')[1])
    
    with open(folder+"/scripts_data/mttf_data.txt", "r") as file:
        mttf_monte = float(file.readline().split(' ')[1])
        mttf_mpe = float(file.readline().split(' ')[1])
        mttf_spe = float(file.readline().split(' ')[1])
        
    with open(folder+"/scripts_data/data.tsv", "w") as tsv_f:
        tsv_f.write(sim_id+"\t")
        tsv_f.write(str(pes).replace(".", ",")+"\t")
        tsv_f.write(heuristic+"\t")
        tsv_f.write(scenario+"\t")
        tsv_f.write(str(occupancy).replace(".", ",")+"\t")
        tsv_f.write(str(hop).replace(".", ",")+"\t")
        tsv_f.write(str(migrations).replace(".", ",")+"\t")
        tsv_f.write(str(avg_power).replace(".", ",")+"\t")
        tsv_f.write(str(avg_temp).replace(".", ",")+"\t")
        tsv_f.write(str(avg_peak_temp).replace(".", ",")+"\t")
        tsv_f.write(str(mttf_monte).replace(".", ",")+"\t")
        tsv_f.write(str(mttf_mpe).replace(".", ",")+"\t")
        tsv_f.write(str(mttf_spe).replace(".", ",")+"\t")
        if mttf_mpe < mttf_spe:
            tsv_f.write(str(mttf_mpe).replace(".", ",")+"\t")
        else:
            tsv_f.write(str(mttf_spe).replace(".", ",")+"\t")


def make_power_file(folder):
    data = []
    with open(folder+"/scripts_data/SystemPower.tsv") as power_file:
        # Create a csv reader
        reader = csv.reader(power_file, delimiter='\t')        
        next(reader)  # Skip the header line
        for row in reader:
            data.append(list(map(float, row)))

    # Get the number of elements and samples
    num_elements = len(data[0]) - 1
    num_samples = len(data)

    average_power = sum([sum(row[1:]) for row in data]) / (num_elements * num_samples)
    # Print the results
    with open(folder+"/scripts_data/power_data.txt", "w") as power_f:
        power_f.write("AVG_PWER: {}".format(average_power)+"\n")

def make_temp_file(folder):
    data=[]
    with open(folder+"/scripts_data/SystemTemperature.tsv") as temp_file:
        # Create a csv reader
        reader = csv.reader(temp_file, delimiter='\t')        
        next(reader)  # Skip the header line
        for row in reader:
            data.append(list(map(float, row)))

    # Get the number of elements and samples
    num_elements = len(data[0]) - 1
    num_samples = len(data)

    average_temperature = sum([sum(row[1:]) for row in data]) / (num_elements * num_samples)
    max_temperatures = [max(row[1:]) for row in data]
    average_peak_temperature = sum(max_temperatures) / num_samples

    # Print the results
    with open(folder+"/scripts_data/temp_data.txt", "w") as temp_f:
        temp_f.write("AVG_TEMP: {}".format(average_temperature)+"\n")
        temp_f.write("AVG_P_TEMP: {}".format(average_peak_temperature))


def make_mttf_file(folder):
    with open(folder+"/scripts_data/mttflog.txt", "r") as mttffile:
        line = mttffile.readline()
        splited = line.split(' ')
        montecarlo_mttf = float(splited[len(splited)-1])
        print("MONTE: "+str(montecarlo_mttf))

    # calculate MTTF for MPE
    with open(folder+"/scripts_data/montecarlofile", "r") as mttffile:
        EM_fit   = float(mttffile.readline())
        SM_fit   = float(mttffile.readline())
        TDDB_fit = float(mttffile.readline())
        TC_fit   = float(mttffile.readline())
        NBTI_fit = float(mttffile.readline())
        mpe_mttf = (1000000000/(EM_fit+SM_fit+TDDB_fit+TC_fit+NBTI_fit))
        mpe_mttf = mpe_mttf/(365*24)
        print("MPE: "+ str(mpe_mttf))
        
        x,y = get_x_y(folder)
        pes_mttf = []
        for i in range(x):
            for j in range(y):
                if(i != 0 and j != 0):
                    EM_fit   = float(mttffile.readline())
                    SM_fit   = float(mttffile.readline())
                    TDDB_fit = float(mttffile.readline())
                    TC_fit   = float(mttffile.readline())
                    NBTI_fit = float(mttffile.readline())
                    pe_mttf = (1000000000/(EM_fit+SM_fit+TDDB_fit+TC_fit+NBTI_fit))
                    pe_mttf = pe_mttf/(365*24)
                    #print(pe_mttf)
                    pes_mttf.append(pe_mttf)
        #print(pes_mttf)
        pes_mttf.sort()
        print("SPE: "+ str(pes_mttf[0]))

        with open(folder+"/scripts_data/mttf_data.txt", "w") as mttf_f:
            mttf_f.write("MONTE: "+str(montecarlo_mttf)+"\n")
            mttf_f.write("MPE: "+ str(mpe_mttf)+"\n")
            mttf_f.write("SPE: "+ str(pes_mttf[0]))
        
        return



def get_x_y(folder):
    # Extracting the two last numbers using regular expression
    result = re.findall(r"(\d+)x(\d+)$", folder)
    # Converting the extracted numbers from string to integers
    x = int(result[0][0])
    y = int(result[0][1])
    return x,y

def wait_RAMP(threads):
    n = 0
    print("TOTAL RAMPS running: "+str(len(threads)), end='\n')
    while n < len(threads):
        n = 0
        for ps in threads:
            if ps.poll() is None:
                time.sleep(1)
                print("RAMPS running: "+str(n)+"/"+str(len(threads)), end='\r')
            else:
                n += 1
    return

def new_matex_file(pwr_file, tmp_file, matex_file):
    with open(pwr_file, 'r') as pwr_f, open(tmp_file, 'r') as tmp_f, open(matex_file, 'w') as matex_f:
        tmp_f.readline()
        pwr_f.readline()
        while True:
            pwr_line = pwr_f.readline()
            if pwr_line == '':
                break
            powers = pwr_line.split('\t')
            matex_f.write("Power: ")
            for p in powers[1:]:
                p = float(p)
                matex_f.write(str("{:.4f}".format(p))+" ")
            matex_f.write("\n")

            matex_f.write("Tsteady: ")
            for p in powers[1:]:
                matex_f.write("0.1 ")
            matex_f.write("\n")

            temp_line = tmp_f.readline()
            if temp_line == '':
                break
            matex_f.write("Temperatures: ")
            temps = temp_line.split('\t')
            for t in temps[1:]:
                t = float(t)+273.15
                matex_f.write(str("{:.4f}".format(t))+" ")
            matex_f.write("\n")
            
def cut_lines(max_time, input_file, output_file):
    with open(input_file, 'r') as input_f, open(output_file, 'w') as output_f:
        header = input_f.readline()
        output_f.write(header)
        
        for line in input_f:
            time_value = float(line.split('\t')[0])
            if time_value <= max_time:
                output_f.write(line)

if __name__ == '__main__': # chamada da funcao principal
    folder_w = "limnos/simulation/SIMULATIONS_0_8_mixed1_70_20000.0ticks_migno_worst_8x8"
    folder_p = "limnos/simulation/SIMULATIONS_1_8_mixed1_70_20000.0ticks_migno_pattern_8x8"
    folder_pid = "limnos/simulation/SIMULATIONS_2_8_mixed1_70_20000.0ticks_migno_pidtm_8x8"
    folder_c = "limnos/simulation/SIMULATIONS_3_8_mixed1_70_20000.0ticks_migyes_pidtm_8x8"
    folder_flea = "limnos/simulation/SIMULATIONS_4_8_mixed1_70_20000.0ticks_migno_chronos_8x8"
    folder_d = "limnos/simulation/SIMULATIONS_5_8_mixed1_70_20000.0ticks_migyes_chronos_8x8"

    folders = [folder_w, folder_p, folder_pid, folder_c, folder_flea, folder_d] 
    generate_TSV_file(folders)