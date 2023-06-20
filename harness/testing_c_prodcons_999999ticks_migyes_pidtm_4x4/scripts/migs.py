from pickle import FALSE
import numpy as np
import sys 

def main():
    with open("simulation/log_0x0.txt", "r") as master_file:
        mig = 0
        stamp = []
        while True:
            line = ""
            try:
                line = master_file.readline()
            except:
                print("erro ao ler a linha")
                #exit
            if line == "":
                break
            elif "SUCCESSFULLYMIGRATED" in line:
                stamp.append(float(int(line.split(" ")[1])/10000))
                mig+=1
        print(str(mig)+" Registered Migrations: ")
        print(stamp)

    with open("simulation/migs.txt", "w") as out_file:
        print(mig, file=out_file)

#-----------------------------------------------------
if __name__ == '__main__': # chamada da funcao principal
    main()
