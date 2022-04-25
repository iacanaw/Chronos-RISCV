#!/bin/bash

echo "Starting simulations..."

for i in {2..10}
    do
        echo "$i - 2x2:"
        sed -i '/executions/c\   executions: '${i}'' ../applications/scenario_4.yaml
        ./RUN_FreeRTOS.sh -x 2 -y 2 -t pattern -s scenario_4 -n timeSim_$i >> sim_2x2_$i.txt
        
        echo "3x3:"
        sed -i '/executions/c\   executions: '${i}'' ../applications/scenario_9.yaml
        ./RUN_FreeRTOS.sh -x 3 -y 3 -t pattern -s scenario_9 -n timeSim_$i >> sim_3x3_$i.txt
        
        echo "4x4:"
        sed -i '/executions/c\   executions: '${i}'' ../applications/scenario_16.yaml
        ./RUN_FreeRTOS.sh -x 4 -y 4 -t pattern -s scenario_16 -n timeSim_$i >> sim_4x4_$i.txt

        echo "5x5:"
        sed -i '/executions/c\   executions: '${i}'' ../applications/scenario_25.yaml
        ./RUN_FreeRTOS.sh -x 5 -y 5 -t pattern -s scenario_25 -n timeSim_$i >> sim_5x5_$i.txt
        
        echo "6x6:"
        sed -i '/executions/c\   executions: '${i}'' ../applications/scenario_36.yaml
        ./RUN_FreeRTOS.sh -x 6 -y 6 -t pattern -s scenario_36 -n timeSim_$i >> sim_6x6_$i.txt
        
        echo "7x7:"
        sed -i '/executions/c\   executions: '${i}'' ../applications/scenario_49.yaml
        ./RUN_FreeRTOS.sh -x 7 -y 7 -t pattern -s scenario_49 -n timeSim_$i >> sim_7x7_$i.txt
        
        echo "8x8:"
        sed -i '/executions/c\   executions: '${i}'' ../applications/scenario_64.yaml
        ./RUN_FreeRTOS.sh -x 8 -y 8 -t pattern -s scenario_64 -n timeSim_$i >> sim_8x8_$i.txt
        
        echo "9x9:"
        sed -i '/executions/c\   executions: '${i}'' ../applications/scenario_81.yaml
        ./RUN_FreeRTOS.sh -x 9 -y 9 -t pattern -s scenario_81 -n timeSim_$i >> sim_9x9_$i.txt
        
        echo "10x10:"
        sed -i '/executions/c\   executions: '${i}'' ../applications/scenario_100.yaml
        ./RUN_FreeRTOS.sh -x 10 -y 10 -t pattern -s scenario_100 -n timeSim_$i >> sim_10x10_$i.txt
    done


echo "Simulations finished."