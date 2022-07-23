
with open("simulation/debug/traffic_router.txt", "w") as tfile:
    with open("simulation/log_0x0.txt", "r") as log_file:
        while True:
            line = ""
            try:
                line = log_file.readline()
            except:
                print("catched an error in file")
                exit()
            if "Debugger" in line:
                splited = line.split(',')
                for j in range(len(splited)-1):
                    if int(splited[j+1]) == 4294967295:
                        print("-1", file=tfile, end='')
                    else:  
                        print(int(splited[j+1]), file=tfile, end='')
                    if (j != len(splited)-2):
                        print("\t", file=tfile, end='')
                    if ( j == len(splited)-2):
                        print("\n", file=tfile, end='')
            if line == "":
                exit()