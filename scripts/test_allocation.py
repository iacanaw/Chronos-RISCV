from cmath import sqrt
import numpy as np
import matplotlib.pyplot as plt


DIM_X = 6 # sqrt(N_TASKS*2)
DIM_Y = 6 # sqrt(N_TASKS*2)

policyTable = [ [106560 , 102982 , 90179 , 0 , 0 , 113432 , 104099 , 75465 , 2772 , 109278 , 95909 , 78506 , 101419 , 89714 , 68987 , 82937 , 73381 , 26229 , 0 , 96344 , 76139 , 48091 , 85704 , 86114 , 81530 , 12541 , 40693 , 0 , 37662 , 34594 , 41015 , 0 , 0 , 0 , 0 ] ,
 [122675 , 123069 , 113517 , 46312 , 0 , 127479 , 118781 , 83948 , 73519 , 113787 , 89447 , 103114 , 99993 , 81174 , 69701 , 109971 , 104216 , 95492 , 6781 , 113872 , 95713 , 87982 , 101787 , 83749 , 60238 , 86607 , 95637 , 42929 , 79368 , 90561 , 76970 , 36094 , 7723 , 34941 , 0] ,
 [126556 , 105553 , 49953 , 0 , 0 , 131112 , 93881 , 76465 , 0 , 110298 , 102253 , 57799 , 63681 , 89180 , 70210 , 103656 , 74781 , 33431 , 0 , 101817 , 100631 , 33287 , 95171 , 94479 , 78870 , 65636 , 365 , 0 , 74685 , 14330 , 48701 , 0 , 0 , 9758 , 0 ] ]

tasks = [1, 1 ,1 ,1 ,1  ,0 ,2, 0 ,2 ,1 ,0 , 1, 1, 1 ,1 ,1 ,1  ,0 ,2, 0 ,2 ,1 ]

cluster = np.zeros((DIM_X, DIM_Y), dtype=int)

def main():

    # policy table normalization
    for state in range(len(policyTable)):
        max = 0
        for score in range(len(policyTable[state])):
            if policyTable[state][score] > max:
                max = policyTable[state][score]
        for score in range(len(policyTable[state])):
            policyTable[state][score] = policyTable[state][score] / max

    # generate random application caracterization
    for x in range(len(cluster)):
        for y in range(len(cluster[x])):
            cluster[x][y] = -1
            # if x == 1 and y == 1:
            #     cluster[x][y] = 1

    allocation_score = []
    allocation_score.append(allocate())
    for i in range(100):
        allocation_score.append(reallocate())

    print(cluster)
    max = 0
    for i in range(len(allocation_score)):
        if max < allocation_score[i]:
            max = allocation_score[i]
    for i in range(len(allocation_score)):
        allocation_score[i] = allocation_score[i] / max

    plt.plot(allocation_score)
    plt.ylim(0, 1.1)
    plt.show()

def allocate():
    for task in range(len(tasks)):
        #creating the allocation array
        array = np.zeros((DIM_X*DIM_Y, 3))
        for x in range(len(cluster)):
            for y in range(len(cluster[x])):
                array[(y*DIM_X)+x] = [int(x), int(y), policyTable[tasks[task]][getState(x,y)]]
                #print(str(int(x))+"x"+str(int(y))+" = "+str(policyTable[tasks[task]][getState(x,y)]))
        array.view('i8,i8,i8').sort(order=['f2'], axis=0)
        print(array)
        for i in reversed(range(len(array))):
            x = int(array[i][0])
            y = int(array[i][1])
            if cluster[x][y] == -1:
                print("task "+str(task)+" got PE "+str(array[i][0])+"x"+str(array[i][1]))
                cluster[x][y] = task
                break
    
    score = 0
    for x in range(len(cluster)):
        for y in range(len(cluster[x])):
            if cluster[x][y] != -1:
                task_type = tasks[cluster[x][y]]
                state = getState(x, y)
                score += policyTable[task_type][state]
    return score

def reallocate():
    for task in range(len(tasks)):
        # remove the task from the cluster
        for x in range(len(cluster)):
            for y in range(len(cluster[x])):
                if task == cluster[x][y]:
                    cluster[x][y] = -1

        # creating the allocation array
        array = np.zeros((DIM_X*DIM_Y, 3))
        for x in range(len(cluster)):
            for y in range(len(cluster[x])):
                array[(y*DIM_X)+x] = [int(x), int(y), policyTable[tasks[task]][getState(x,y)]]
                #print(str(int(x))+"x"+str(int(y))+" = "+str(policyTable[tasks[task]][getState(x,y)]))
        array.view('i8,i8,i8').sort(order=['f2'], axis=0)
        print(array)
        for i in reversed(range(len(array))):
            x = int(array[i][0])
            y = int(array[i][1])
            if cluster[x][y] == -1:
                print("task "+str(task)+" got PE "+str(array[i][0])+"x"+str(array[i][1]))
                cluster[x][y] = task
                break
    
    score = 0
    for x in range(len(cluster)):
        for y in range(len(cluster[x])):
            if cluster[x][y] != -1:
                task_type = tasks[cluster[x][y]]
                state = getState(x, y)
                score += policyTable[task_type][state]
    return score

            
def getState(x, y):
    immediate = np.zeros((3), dtype=int)
    if getSouth(x, y) != -1:
        immediate[tasks[getSouth(x, y)]]+=1
    if getNorth(x, y) != -1:
        immediate[tasks[getNorth(x, y)]]+=1
    if getEast(x, y) != -1:
        immediate[tasks[getEast(x, y)]]+=1
    if getWest(x, y) != -1:
        immediate[tasks[getWest(x, y)]]+=1
    
    a = immediate[0]
    b = immediate[1]
    c = immediate[2]

    state_a = int((a*a*a - 18*a*a + 107*a) / 6)
    state_b = int((11*b - b*b - 2*a*b) / 2)
    state_abc = state_a + state_b + c

    return state_abc

def getSouth(x, y):
    if(y > 0):
        return cluster[x][y-1]
    else:
        return -1

def getNorth(x, y):
    if(y < DIM_Y-1):
        return cluster[x][y+1]
    else:
        return -1


def getEast(x, y):
    if(x < DIM_X-1):
        return cluster[x+1][y]
    else:
        return -1

def getWest(x, y):
    if(x > 0):
        return cluster[x-1][y]
    else:
        return -1

#-----------------------------------------------------
if __name__ == '__main__': # chamada da funcao principal
    main()