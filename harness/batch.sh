#!/bin/bash

TOTAL=0
# SECONDS=0
# ./batchXY.sh 5 5 SIMULATION_5 c_testing4 &
# pid_5=$!
# wait $pid_5
# ELAPSED0="Simulation 5x5 took: $(($SECONDS / 3600))hrs $((($SECONDS / 60) % 60))min $(($SECONDS % 60))sec"
# echo "$ELAPSED0" >> ../simulation/SIMULATION_5.csv 
# TOTAL=$(($TOTAL + $SECONDS))

# SECONDS=0
# ./batchXY.sh 6 6 SIMULATION_5 c_testing4 &
# pid_6=$!
# wait $pid_6
# ELAPSED1="Simulation 6x6 took: $(($SECONDS / 3600))hrs $((($SECONDS / 60) % 60))min $(($SECONDS % 60))sec"
# echo "$ELAPSED1" >> ../simulation/SIMULATION_5.csv 
# TOTAL=$(($TOTAL + $SECONDS))

SECONDS=0
./batchXY.sh 7 7 SIMULATION_5 c_testing4 &
pid_7=$!
wait $pid_7
ELAPSED2="Simulation 7x7 took: $(($SECONDS / 3600))hrs $((($SECONDS / 60) % 60))min $(($SECONDS % 60))sec"
echo "$ELAPSED2" >> ../simulation/SIMULATION_5.csv 
TOTAL=$(($TOTAL + $SECONDS))

SECONDS=0
./batchXY.sh 8 8  SIMULATION_5 c_testing4 &
pid_8=$!
wait $pid_8
ELAPSED3="Simulation 8x8 took: $(($SECONDS / 3600))hrs $((($SECONDS / 60) % 60))min $(($SECONDS % 60))sec"
echo "$ELAPSED3" >> ../simulation/SIMULATION_5.csv 
TOTAL=$(($TOTAL + $SECONDS))

SECONDS=0
./batchXY.sh 10 10 SIMULATION_5 c_testing4 &
pid_10=$!
wait $pid_10
ELAPSED4="Simulation 10x10 took: $(($SECONDS / 3600))hrs $((($SECONDS / 60) % 60))min $(($SECONDS % 60))sec"
echo "$ELAPSED4" >> ../simulation/SIMULATION_5.csv 
TOTAL=$(($TOTAL + $SECONDS))

SECONDS=0
./batchXY.sh 11 11 SIMULATION_5 c_testing4 &
pid_11=$!
wait $pid_11
ELAPSED5="Simulation 11x11 took: $(($SECONDS / 3600))hrs $((($SECONDS / 60) % 60))min $(($SECONDS % 60))sec"
echo "$ELAPSED5" >> ../simulation/SIMULATION_5.csv 
TOTAL=$(($TOTAL + $SECONDS))

SECONDS=0
./batchXY.sh 12 12 SIMULATION_5 c_testing4 &
pid_12=$!
wait $pid_12
ELAPSED6="Simulation 12x12 took: $(($SECONDS / 3600))hrs $((($SECONDS / 60) % 60))min $(($SECONDS % 60))sec"
echo "$ELAPSED6" >> ../simulation/SIMULATION_5.csv 
TOTAL=$(($TOTAL + $SECONDS))

ELAPSED="This simulation took: $(($TOTAL / 3600))hrs $((($TOTAL / 60) % 60))min $(($TOTAL % 60))sec"
echo "$ELAPSED" >> ../simulation/SIMULATION_5.csv 

cat ../simulation/SIMULATION_5*/simulation/0_log.csv >> ../simulation/SIMULATION_5.csv
