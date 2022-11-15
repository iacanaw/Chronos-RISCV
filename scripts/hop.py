from pickle import FALSE
import numpy as np
import sys 

mpsoc_x = 0
mpsoc_y = 0

dijkstra_graph = [[5], [5], [5], [5], [5], [], [0, 1, 2, 3, 4]]
dtw_graph = [[1,2,3,4], [5], [5], [5], [5], [1,2,3,4]]
prod_graph = [[], [0]]
alpha_graph = [[], [0], [1], [2]]
beta_graph = [[], [0], [1], [2]]
gama_graph = [[], [0], [1], [2], [3], [4], [5], [6]]
double_graph = [[], [0], [1], [2], [0], [4], [5]]

def main():
    with open("simulation/debug/platform.cfg", "r") as platform_file:
        log_task_name = False
        log_app_name = False
        tasks = {}
        apps = {}
        while True:
            line = ""
            try:
                line = platform_file.readline()
            except:
                print("erro ao ler a linha do platform.cfg")
                exit
            if "mpsoc_x" in line:
                mpsoc_x = int(line.split(" ")[1])
            elif "mpsoc_y" in line:
                mpsoc_y = int(line.split(" ")[1])
            elif line == "":
                break

            if "BEGIN_task_name_relation" in line:
                log_task_name = True
            elif "END_task_name_relation" in line:
                log_task_name = False
            elif log_task_name:
                task_name = line.split(" ")[0]
                task_id = int(line.split(" ")[1])
                tasks[task_id] = task_name
            
            if "BEGIN_app_name_relation" in line:
                log_app_name = True
            elif "END_app_name_relation" in line:
                log_app_name = FALSE
            elif log_app_name:
                app_name = line.split(" ")[0]
                app_id = int(line.split(" ")[1])
                apps[app_id] = app_name

    print("Sistema de dimensoes: "+str(mpsoc_x)+"x"+str(mpsoc_y))
    print(tasks)
    print(apps)

    with open("simulation/debug/traffic_router.txt", "r") as traffic_file:
        allocations = {}
        hop_count = {}
        hop_acc = {}
        hops = 0
        hop_nedges = 0
        while True:
            line = ""
            try:
                line = traffic_file.readline()
            except:
                print("erro ao ler a linha do trafic_router.txt")
                exit
            if line == "":
                print("Fim do arquivo!")
                break
            line = line.split("\t")

            # register when a task is allocated
            if(int(line[2]) == 40):
                time = int(line[0])/10000 # in secs
                addr_x = getX(int(line[1])) # get the X addr
                addr_y = getY(int(line[1])) # get the X addr
                apptask = int(line[len(line)-1])

                allocations[apptask] = [addr_x, addr_y] 

            # when a task is dealocated - checks the hop count with communicating tasks
            elif(int(line[2]) == 70):
                time = int(line[0])/10000 # in secs
                addr_x = getX(int(line[1])) # get the X addr
                addr_y = getY(int(line[1])) # get the X addr
                apptask = int(line[len(line)-1])
                app_id = getAppID(apptask)
                task_id = getTaskID(apptask)
                if "prodcons" in apps[app_id]:
                    print("Achei uma prodcons! "+str(getAppID(apptask)))
                    for task_connected in prod_graph[task_id]:
                        hops += distance(allocations[getApptask(app_id, task_connected)], [addr_x, addr_y])
                        hop_nedges+=1

                elif "dijkstra" in apps[getAppID(apptask)]:
                    print("Achei uma dijkstra! "+str(getAppID(apptask)))
                    for task_connected in dijkstra_graph[task_id]:
                        hops += distance(allocations[getApptask(app_id, task_connected)], [addr_x, addr_y])
                        hop_nedges+=1

                elif "dtw" in apps[getAppID(apptask)]:
                    print("Achei uma dtw! "+str(getAppID(apptask)))
                    for task_connected in dtw_graph[task_id]:
                        hops += distance(allocations[getApptask(app_id, task_connected)], [addr_x, addr_y])
                        hop_nedges+=1
                
                elif "alpha" in apps[getAppID(apptask)]:
                    print("Achei um pipeline_alpha! "+str(getAppID(apptask)))
                    for task_connected in alpha_graph[task_id]:
                        hops += distance(allocations[getApptask(app_id, task_connected)], [addr_x, addr_y])
                        hop_nedges+=1
                
                elif "beta" in apps[getAppID(apptask)]:
                    print("Achei um pipeline_beta! "+str(getAppID(apptask)))
                    for task_connected in beta_graph[task_id]:
                        hops += distance(allocations[getApptask(app_id, task_connected)], [addr_x, addr_y])
                        hop_nedges+=1

                elif "gama" in apps[getAppID(apptask)]:
                    print("Achei um pipeline_gama! "+str(getAppID(apptask)))
                    for task_connected in gama_graph[task_id]:
                        hops += distance(allocations[getApptask(app_id, task_connected)], [addr_x, addr_y])
                        hop_nedges+=1
                
                elif "double" in apps[getAppID(apptask)]:
                    print("Achei um double_pipeline! "+str(getAppID(apptask)))
                    for task_connected in double_graph[task_id]:
                        hops += distance(allocations[getApptask(app_id, task_connected)], [addr_x, addr_y])
                        hop_nedges+=1

    print("-----------------")
    print(hops)
    print(hop_nedges)
    hops = hops / hop_nedges
    print("AVG HOP: "+str(hops))

    
    with open("simulation/hop.txt", "w") as out_file:
        print(hops, file=out_file)
    
def distance(xy1, xy2):
    # print(xy1)
    # print(xy2)
    delta_x = xy1[0] - xy2[0]
    delta_y = xy1[1] - xy2[1]
    if delta_x < 0:
        delta_x*=-1
    if delta_y < 0:
        delta_y*=-1
    return (delta_x + delta_y)


def getApptask(app_id, task_id):
    return int((app_id*256)+task_id)

def getAppID(apptask):
    return int(apptask/256)

def getTaskID(apptask):
    return int(apptask%256)

def getX(addr):
    return ((addr & 65280) >> 8)

def getY(addr):
    return (addr & 255)

#-----------------------------------------------------
if __name__ == '__main__': # chamada da funcao principal
    main()
