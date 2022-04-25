#!/bin/bash

echo "Starting simulations..."

echo "2x2:"
./RUN_FreeRTOS.sh -x 2 -y 2 -t pattern -s scenario_4 -n timeSim >> sim_2x2.txt
echo "3x3:"
./RUN_FreeRTOS.sh -x 3 -y 3 -t pattern -s scenario_9 -n timeSim >> sim_3x3.txt
echo "4x4:"
./RUN_FreeRTOS.sh -x 4 -y 4 -t pattern -s scenario_16 -n timeSim >> sim_4x4.txt
echo "5x5:"
./RUN_FreeRTOS.sh -x 5 -y 5 -t pattern -s scenario_25 -n timeSim >> sim_5x5.txt
echo "6x6:"
./RUN_FreeRTOS.sh -x 6 -y 6 -t pattern -s scenario_36 -n timeSim >> sim_6x6.txt
echo "7x7:"
./RUN_FreeRTOS.sh -x 7 -y 7 -t pattern -s scenario_49 -n timeSim >> sim_7x7.txt
echo "8x8:"
./RUN_FreeRTOS.sh -x 8 -y 8 -t pattern -s scenario_64 -n timeSim >> sim_8x8.txt
echo "9x9:"
./RUN_FreeRTOS.sh -x 9 -y 9 -t pattern -s scenario_81 -n timeSim >> sim_9x9.txt
echo "10x10:"
./RUN_FreeRTOS.sh -x 10 -y 10 -t pattern -s scenario_100 -n timeSim >> sim_10x10.txt

echo "Simulations finished."