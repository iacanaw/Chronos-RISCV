#!/bin/bash

TOTAL=0

SECONDS=0
./batchXY.sh 8 8 CHRONOS3UPDT c_testing6 &
pid_8=$!
wait $pid_8
ELAPSED2="Simulation 8x8 took: $(($SECONDS / 3600))hrs $((($SECONDS / 60) % 60))min $(($SECONDS % 60))sec"
echo "$ELAPSED2" >> ../simulation/CHRONOS3UPDT.csv 
TOTAL=$(($TOTAL + $SECONDS))

SECONDS=0
./batchXY.sh 10 10 CHRONOS3UPDT c_testing6 &
pid_10=$!
wait $pid_10
ELAPSED4="Simulation 10x10 took: $(($SECONDS / 3600))hrs $((($SECONDS / 60) % 60))min $(($SECONDS % 60))sec"
echo "$ELAPSED4" >> ../simulation/CHRONOS3UPDT.csv 
TOTAL=$(($TOTAL + $SECONDS))

ELAPSED="This simulation took: $(($TOTAL / 3600))hrs $((($TOTAL / 60) % 60))min $(($TOTAL % 60))sec"
echo "$ELAPSED" >> ../simulation/CHRONOS3UPDT.csv 

cat ../simulation/CHRONOS3UPDT*/simulation/0_log.csv >> ../simulation/CHRONOS3UPDT.csv
