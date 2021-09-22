import sys
import math
import os
import copy
import re
from os import path
import yaml

SCENARIO = str(sys.argv[1]) + '.yaml'

appsName = []
appsPeriod = []
appsExecutions = []
appsTasks = []
appsTaskSize = []
appsTaskBss = []
appsTaskStart = []
appsTaskCode = []
bigCode = 0

with open(SCENARIO) as file:
    scenario = yaml.load(file, Loader=yaml.SafeLoader)
    for app in range(len(scenario['apps'])):
        appsName.append(scenario['apps'][app]['name'])
        appsPeriod.append(int(scenario['apps'][app]['period']))
        appsExecutions.append(int(scenario['apps'][app]['executions']))
        tasks=[]
        for task in scenario['apps'][app]['tasks'].keys():
            tasks.append(task)
        appsTasks.append(copy.deepcopy(tasks))

# Finds information about each application
for i in range(len(appsName)):
    print("Application " + str(i) + " - " + appsName[i] + " has period of "+ str(appsPeriod[i]) + " and will execute " + str(appsExecutions[i]) + " times.")
    taskSize = []
    taskBss = []
    taskCode = []
    taskStart = []
    for j in range(len(appsTasks[i])):
        taskSize.append(0)
        taskBss.append(0)
        taskStart.append(0)
        code = []
        bss = False
        print("       Task " + str(j) + " - " + appsTasks[i][j])
        taskfilename = appsName[i]+'/'+appsTasks[i][j]+'.lst'
        with open (taskfilename, 'r') as taskFile:
            while True:
                line = taskFile.readline()
                if "	" in line: 
                    if bss == True:
                        taskBss[j] += 1
                    else: 
                        taskSize[j] += 1
                        value = re.split(r'\t+', line)
                        value = re.split(r' ', value[1])
                        if value[0] == '':
                            value[0] = "00000000"
                        code.append(value[0])
                        #print(value[0])

                if "<main>:" in line:
                    taskStart[j] = copy.deepcopy(taskSize[j]);
                        
                if ".bss" in line: #identify when the bss has begun
                    bss = True
                elif line == '':
                    break
            taskCode.append(copy.deepcopy(code))
            print("         TaskSize: " + str(taskSize[j]))
            print("         TaskStart: " + str(taskStart[j]))
            if taskSize[j] > bigCode:
                bigCode = taskSize[j]
            print("         BSS Size: " + str(taskBss[j]))
    appsTaskBss.append(copy.deepcopy(taskBss))
    appsTaskStart.append(copy.deepcopy(taskStart))
    appsTaskSize.append(copy.deepcopy(taskSize))
    appsTaskCode.append(copy.deepcopy(taskCode))

# Creates the "repository.h"
with open('repository.h', 'w') as file:
    file.write('#ifndef __REPOSITORY_H__\n')
    file.write('#define __REPOSITORY_H__\n')
    file.write('\n')
    file.write('#define NUM_APPS ' + str(len(appsName)) + '\n' )
    file.write('#define INFO_SIZE ' + str(10) + '\n' )
    file.write('#define MAX_TASKS ' + str(15) + '\n' )
    file.write('#define MASTER_ADDR 0x0000\n')
    file.write('#define BIG_CODE ' + str(bigCode) + '\n')
    file.write('// Application IDs\n')
    for i in range(len(appsName)):
        file.write('#define ' + appsName[i] + " " + str(i) + "\n" )
    file.write('// Application INFO\n')
    file.write('static unsigned int appInfo[NUM_APPS][INFO_SIZE] = {\n')
    for i in range(len(appsName)):
        if i > 0:
            file.write(',\n')
        file.write('\t{ ' + str('0x%08X'%i) + ',  // app id '+ appsName[i] + '\n')
        file.write('\t  ' + str('0x%08X'%appsPeriod[i]) + ',  // app period \n')
        file.write('\t  ' + str('0x%08X'%appsExecutions[i]) + ',  // app executions \n')
        file.write('\t  ' + str('0x%08X'%len(appsTasks[i])) + ',  // number of tasks \n')
        file.write('\t  ' + str('0x%08X'%4294967295) + ',  // nothing \n')
        file.write('\t  ' + str('0x%08X'%4294967295) + ',  // nothing \n')
        file.write('\t  ' + str('0x%08X'%4294967295) + ',  // nothing \n')
        file.write('\t  ' + str('0x%08X'%4294967295) + ',  // nothing \n')
        file.write('\t  ' + str('0x%08X'%4294967295) + ',  // nothing \n')
        file.write('\t  ' + str('0x%08X'%4294967295) + ' } // nothing \n')
    file.write('};\n\n')

    file.write('static unsigned int tasksInfo[NUM_APPS][MAX_TASKS][INFO_SIZE] = {\n')
    for i in range(len(appsName)):
        if i > 0:
            file.write(',\n')
        file.write('\t{ \t\t // '+ appsName[i] + '\n')
        for j in range(len(appsTasks[i])):
            if j > 0:
                file.write(',\n')
            file.write('\t\t{ ' + str('0x%08X'%j) + ',  // task id '+ appsTasks[i][j] + '\n')
            file.write('\t\t  ' + str('0x%08X'%appsTaskSize[i][j]) + ',  // task size \n')
            file.write('\t\t  ' + str('0x%08X'%appsTaskBss[i][j]) + ',  // bss size \n')
            file.write('\t\t  ' + str('0x%08X'%appsTaskStart[i][j]) + ',  // start point \n')
            file.write('\t\t  ' + str('0x%08X'%i) + ',  // task appID \n')
            file.write('\t\t  ' + str('0x%08X'%4294967295) + ',  // nothing \n')
            file.write('\t\t  ' + str('0x%08X'%4294967295) + ',  // nothing \n')
            file.write('\t\t  ' + str('0x%08X'%4294967295) + ',  // nothing \n')
            file.write('\t\t  ' + str('0x%08X'%4294967295) + ',  // nothing \n')
            file.write('\t\t  ' + str('0x%08X'%4294967295) + ' } // nothing \n')
        file.write('\t}')
    file.write('\n};\n\n')

    file.write('static unsigned int tasksCode[NUM_APPS][MAX_TASKS][BIG_CODE] = {\n')
    for i in range(len(appsName)):
        if i > 0:
            file.write(',\n')
        file.write('\t{ \t\t // '+ appsName[i] + '\n')
        for j in range(len(appsTasks[i])):
            if j > 0:
                file.write(',\n')
            for l in range(len(appsTaskCode[i][j])):
                if l == 0:
                    file.write('\t\t{ 0x' + appsTaskCode[i][j][l] + ',  // line ' + str(l) + ' - task '+ appsTasks[i][j] + '\n')
                elif l == len(appsTaskCode[i][j])-1:
                    file.write('\t\t  0x' + appsTaskCode[i][j][l] + ' } // line ' + str(l) + '\n')
                else:
                    file.write('\t\t  0x' + appsTaskCode[i][j][l] + ',  // line ' + str(l) + '\n')
        file.write('\t}')
    file.write('\n};\n\n')

    file.write("#endif /*__REPOSITORY_H__*/\n")
    