#!/bin/bash

./RUN_FreeRTOS.sh -x 7 -y 7 -m chronos3 -s c_testing -n SIMULATION_2 -t 999999999 & 
pid77_c3=$!
sleep 5m
./RUN_FreeRTOS.sh -x 7 -y 7 -m pattern -s c_testing -n SIMULATION_2 -t 999999999 &
pid77_p=$!
sleep 5m
./RUN_FreeRTOS.sh -x 11 -y 11 -m chronos3 -s c_testing -n SIMULATION_2 -t 999999999 &
pid11_c3=$!
sleep 5m

wait $pid77_c3 
wait $pid77_p 
wait $pid11_c3

./RUN_FreeRTOS.sh -x 8 -y 8 -m pidtm -s c_testing -n SIMULATION_2 -t 999999999 &
pid88_pidtm=$!
sleep 5m
./RUN_FreeRTOS.sh -x 8 -y 8 -m pattern -s c_testing -n SIMULATION_2 -t 999999999 &
pid88_pa=$!
sleep 5m
./RUN_FreeRTOS.sh -x 8 -y 8 -m chronos2 -s c_testing -n SIMULATION_2 -t 999999999 & 
pid8_c2=$!

wait $pid88_pidtm

./RUN_FreeRTOS.sh -x 8 -y 8 -m chronos3 -s c_testing -n SIMULATION_2 -t 999999999 & 
pid88_c3=$!

wait $pid88_pa
wait $pid8_c2
wait $pid88_c3


./RUN_FreeRTOS.sh -x 12 -y 12 -m chronos3 -s c_testing -n SIMULATION_2 -t 999999999 & 
pid12_c3=$!
sleep 5m
./RUN_FreeRTOS.sh -x 12 -y 12 -m pattern -s c_testing -n SIMULATION_2 -t 999999999 &
pid12_pa=$!

wait $pid12_c3
wait $pid12_pa

./RUN_FreeRTOS.sh -x 12 -y 12 -m chronos2 -s c_testing -n SIMULATION_2 -t 999999999 & 
pid12_c2=$!
sleep 5m
./RUN_FreeRTOS.sh -x 12 -y 12 -m pidtm -s c_testing -n SIMULATION_2 -t 999999999 &
pid12_pidtm=$!

wait $pid12_c2
wait $pid12_pidtm

