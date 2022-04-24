from logging import critical
import sys
import math
import os
import copy
import re
from os import path
from pandas import cut
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
appsTaskDeadline = []
appsTaskMigration = []
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
    taskMigrateVar = []
    appDeadline = 0
    with open(appsName[i]+'/info.yaml') as info_file:
        taskInfo = yaml.load(info_file, Loader=yaml.SafeLoader)
        appDeadline = int(float(taskInfo['info'][0]['deadline'])*100)
    print(appDeadline)
    for j in range(len(appsTasks[i])):
        taskSize.append(0)
        taskBss.append(0)
        taskStart.append(0)
        taskMigrateVar.append(0)
        code = []
        bss = False
        print("       Task " + str(j) + " - " + appsTasks[i][j])
        taskfilename = appsName[i]+'/'+appsTasks[i][j]+'.lst'
        with open (taskfilename, 'r') as taskFile:
            lastAddr = int("7FFFFFFC", 16)
            code = []
            while True:
                line = taskFile.readline()
                if line == '':
                    break
                else:
                    # detects the main address
                    if "<main>:" in line:
                            cut_line = re.split(r' ', line)
                            currentAddr = cut_line[0]
                            currentAddr = int(currentAddr, 16)
                            taskStart[j] = int((currentAddr - 2147483648)/4)

                    # detects the migrate address
                    if "<MIGRATE>:" in line:
                            cut_line = re.split(r' ', line)
                            currentAddr = cut_line[0]
                            currentAddr = int(currentAddr, 16)
                            taskMigrateVar[j] = int((currentAddr - 2147483648)/4)

                    # if any line that has an instruction
                    if ("800" in line) and ("\t" in line):
                        cut_line = re.split(r'\t', line)
                        #print(cut_line)
                        currentAddr = cut_line[0][:-1]
                        currentAddr = int(currentAddr, 16)
                        cut_line.pop(0) 
                        #print(currentAddr)
                        cut_line = re.split(r' ', cut_line[0])
                        for n in range(len(cut_line)):
                            try:
                                int(cut_line[n], 16)
                            except:
                                for m in range(n, len(cut_line)):
                                    cut_line.pop(n)
                                break
                        #print(cut_line)
                        if len(cut_line) > 0:
                            if len(cut_line) == 1:
                                cut_line = cut_line[0]
                            elif len(cut_line) == 2:
                                cut_line = cut_line[0] + cut_line[1]
                            elif len(cut_line) == 3:
                                cut_line = cut_line[0] + cut_line[1] + cut_line[2]
                            elif len(cut_line) == 4:
                                cut_line = cut_line[0] + cut_line[1] + cut_line[2] + cut_line[3]
                            else:
                                critical("ERRO: "+ line)
                            #print(cut_line)
                            code.append([currentAddr, cut_line])
                        #print("====")
                        
                # if "<.rodata>" in line:
                #     value = re.split(r' ', line)
                #     rodataAddr = int(value[0], 16)
                #     txtEndAddr = int(lastAddr, 16)
                #     wordgap = (rodataAddr - txtEndAddr) / 4
                #     #print("wordgap: " + str(wordgap))
                #     for k in range(int(wordgap)):
                #         #print(txtEndAddr)
                #         taskSize[j] += 1
                #         code.append("00000000")

                # elif "	" in line: 
                #     if bss == True:
                #         taskBss[j] += 1
                #     else: 
                #         taskSize[j] += 1
                #         value = re.split(r'\t+', line)
                #         lastAddr = value[0][:-1] # [:-1] removes the last character 
                #         value = re.split(r' ', value[1])
                #         # case the line is empty
                #         if value[0] == '':
                #             value[0] = "00000000"

                #         # there is a space between each half-word
                #         # example:
                #         # 80000a64:	    6974 676e     ting
                #         if len(value[0]) == 4 and len(value[1]) == 4:
                #             fullWord = value[0] + value[1]
                #             #print("data fullWord: ", fullWord)
                #             code.append(fullWord)

                #         # the last word is incomplete
                #         # example:
                #         # 80000bb8:	    000a         
                #         elif len(value[0]) == 4 and len(value) == 11:
                #             fullWord = value[0] + "0000"
                #             code.append(fullWord)

                #         # case we have just half word in each line
                #         # example:
                #         # 80000a5c:	    0001      	nop
                #         # 80000a5e:	    0000      	unimp
                #         elif len(value[0]) == 4:
                #             fullWord = value[0];
                #             line = taskFile.readline()
                #             #when there is nothing after the half word
                #             if len(line) == 1:
                #                 fullWord = "0000"+value[0]
                #                 #print(fullWord)
                #             #when its another half word next...
                #             else: 
                #                 value = re.split(r'\t+', line)
                #                 value = re.split(r' ', value[1])
                #                 fullWord = fullWord + value[0]
                #                 #print("roda fullWord: ", fullWord)
                #             code.append(fullWord)

                #         # in case of a regular command
                #         # example:
                #         # 800004e8:	    59878793 	addi	a5,a5,1432 # 80000a7c <msg>
                #         else:   
                #             code.append(value[0])
                        
                # if "<main>:" in line:
                #     taskStart[j] = copy.deepcopy(taskSize[j])

                # if ".bss" in line: #identify when the bss has begun
                #     bss = True
                # elif line == '':
                #     break
            #print(code)
            toPop = []
            for n in range(len(code)-1):
                if not( n in toPop):
                    if (code[n+1][0] - code[n][0] == 2):
                        #print("Merging: ")
                        #print(code[n])
                        #print(code[n+1])
                        if len(code[n+1][1]) > 4:
                            code[n][1] = code[n][1] + code[n+1][1][0] + code[n+1][1][1] + code[n+1][1][2] + code[n+1][1][3]
                            code[n+1][1] = code[n+1][1][-4:]
                            code[n+1][0] = code[n+1][0]+2
                        else:
                            code[n][1] = code[n][1] + code[n+1][1]
                            toPop.append(n+1) #code.pop(i+1)
                        #print(code[n])
                    if (code[n+1][0] - code[n][0] > 4):
                        wordgap = (code[n+1][0] - code[n][0]) / 4
                        print("wordgap: " + str(math.floor(wordgap)))
                        print(code[n])
                        print(code[n+1])
                        print("news: ")
                        for k in reversed(range(int(math.floor(wordgap)-1))):
                            code.insert(n+1 , [code[n][0]+(4*int(k+1)),"00000000"])
                            print(code[n+1])      
            for n in range(len(toPop)):
                code.pop(toPop[n]-n)

            #print("checking...")
            for n in range(len(code)-1):
                #print(code[i])
                if(code[1+n][0] - code[n][0] != 4):
                    print("ERRO: " + str(code[1+n][0] - code[n][0]))
                    print(code[n])
                    print(code[n+1])
                    break
            taskSize[j] = len(code)
            
            #taskCode.append(copy.deepcopy(code))
            thecode = []
            for n in range(len(code)):
                thecode.append(copy.deepcopy(code[n][1]))
            
            taskCode.append(copy.deepcopy(thecode))
            
            print("         TaskSize: " + str(taskSize[j]))
            print("         TaskStart: " + str(taskStart[j]))
            print("         MigrateVar: " + str(taskMigrateVar[j]))
            if taskSize[j] > bigCode:
                bigCode = taskSize[j]
            print("         BSS Size: " + str(taskBss[j]))
    appsTaskBss.append(copy.deepcopy(taskBss))
    appsTaskStart.append(copy.deepcopy(taskStart))
    appsTaskSize.append(copy.deepcopy(taskSize))
    appsTaskCode.append(copy.deepcopy(taskCode))
    appsTaskDeadline.append(copy.deepcopy(appDeadline))
    appsTaskMigration.append(copy.deepcopy(taskMigrateVar))

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
        appsName[i] = appsName[i] + "_" + str(i)
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
        file.write('\t  ' + str('0x%08X'%appsTaskDeadline[i]) + ',  // deadline \n')
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
            file.write('\t\t  ' + str('0x%08X'%appsTaskMigration[i][j]) + ',  // migration variable \n')
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
    