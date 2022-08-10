#!/bin/bash

X=$1
Y=$2
NAME=$3

./RUN_FreeRTOS.sh -x $X -y $Y -m pidtm -s c_testing3 -n $NAME -t 999999999 & 
pid_pidtm=$!
sleep 5m
./RUN_FreeRTOS.sh -x $X -y $Y -m chronos2 -s c_testing3 -n $NAME -t 999999999 & 
pid_chronos2=$!
sleep 5m
./RUN_FreeRTOS.sh -x $X -y $Y -m chronos3 -s c_testing3 -n $NAME -t 999999999 & 
pid_chronos3=$!
sleep 5m
./RUN_FreeRTOS.sh -x $X -y $Y -m pattern -s c_testing3 -n $NAME -t 999999999 & 
pid_pattern=$!

wait $pid_pidtm
wait $pid_chronos2
wait $pid_chronos3
wait $pid_pattern