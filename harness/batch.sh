#!/bin/bash

./RUN_FreeRTOS.sh -x 5 -y 5 -m pattern -s c_testing -n SIMULATION_1 -t 999999999 & 
pid5=$!
sleep 1m
./RUN_FreeRTOS.sh -x 6 -y 6 -m pattern -s c_testing -n SIMULATION_1 -t 999999999 &
pid6=$!
sleep 1m
./RUN_FreeRTOS.sh -x 7 -y 7 -m pattern -s c_testing -n SIMULATION_1 -t 999999999 &
pid7=$!
sleep 1m
./RUN_FreeRTOS.sh -x 8 -y 8 -m pattern -s c_testing -n SIMULATION_1 -t 999999999 &
pid8=$!
sleep 1m
./RUN_FreeRTOS.sh -x 10 -y 10 -m pattern -s c_testing -n SIMULATION_1 -t 999999999 &
pid10=$!
sleep 1m
./RUN_FreeRTOS.sh -x 11 -y 11 -m pattern -s c_testing -n SIMULATION_1 -t 999999999 &
pid11=$!
sleep 1m
./RUN_FreeRTOS.sh -x 12 -y 12 -m pattern -s c_testing -n SIMULATION_1 -t 999999999 &
pid12=$!
sleep 1m

wait $pid5 && echo "pid1 exited normally" || echo "pid1 exited abnormally with status $?"

./RUN_FreeRTOS.sh -x 5 -y 5 -m pidtm -s c_testing -n SIMULATION_1 -t 999999999 &
pid5=$!
sleep 1m

wait $pid6 && echo "pid1 exited normally" || echo "pid1 exited abnormally with status $?"

./RUN_FreeRTOS.sh -x 6 -y 6 -m pidtm -s c_testing -n SIMULATION_1 -t 999999999 &
pid6=$!
sleep 1m

wait $pid7 && echo "pid1 exited normally" || echo "pid1 exited abnormally with status $?"

./RUN_FreeRTOS.sh -x 7 -y 7 -m pidtm -s c_testing -n SIMULATION_1 -t 999999999 &
pid7=$!
sleep 1m

wait $pid8 && echo "pid1 exited normally" || echo "pid1 exited abnormally with status $?"

./RUN_FreeRTOS.sh -x 8 -y 8 -m pidtm -s c_testing -n SIMULATION_1 -t 999999999 &
pid8=$!
sleep 1m

wait $pid10 && echo "pid1 exited normally" || echo "pid1 exited abnormally with status $?"

./RUN_FreeRTOS.sh -x 10 -y 10 -m pidtm -s c_testing -n SIMULATION_1 -t 999999999 &
pid10=$!
sleep 1m

wait $pid11 && echo "pid1 exited normally" || echo "pid1 exited abnormally with status $?"

./RUN_FreeRTOS.sh -x 11 -y 11 -m pidtm -s c_testing -n SIMULATION_1 -t 999999999 &
pid11=$!
sleep 1m

wait $pid12 && echo "pid1 exited normally" || echo "pid1 exited abnormally with status $?"

./RUN_FreeRTOS.sh -x 12 -y 12 -m pidtm -s c_testing -n SIMULATION_1 -t 999999999 &
pid5=$!
sleep 1m

wait $pid5 && echo "pid1 exited normally" || echo "pid1 exited abnormally with status $?"
wait $pid6 && echo "pid1 exited normally" || echo "pid1 exited abnormally with status $?"
wait $pid7 && echo "pid1 exited normally" || echo "pid1 exited abnormally with status $?"
wait $pid8 && echo "pid1 exited normally" || echo "pid1 exited abnormally with status $?"
wait $pid10 && echo "pid1 exited normally" || echo "pid1 exited abnormally with status $?"
wait $pid11 && echo "pid1 exited normally" || echo "pid1 exited abnormally with status $?"
wait $pid12 && echo "pid1 exited normally" || echo "pid1 exited abnormally with status $?"
