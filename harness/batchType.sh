#!/bin/bash

NAME=$1
TYPE=$2

./RUN_FreeRTOS.sh -x 5 -y 5 -m $TYPE -s c_testing -n $NAME -t 999999999 & 
pid5=$!
sleep 2m
./RUN_FreeRTOS.sh -x 12 -y 12 -m $TYPE -s c_testing -n $NAME -t 999999999 & 
pid12=$!
sleep 2m

wait $pid5

./RUN_FreeRTOS.sh -x 6 -y 6 -m $TYPE -s c_testing -n $NAME -t 999999999 & 
pid6=$!
sleep 2m

wait $pid6

./RUN_FreeRTOS.sh -x 7 -y 7 -m $TYPE -s c_testing -n $NAME -t 999999999 & 
pid7=$!
sleep 2m

wait $pid7

./RUN_FreeRTOS.sh -x 8 -y 8 -m $TYPE -s c_testing -n $NAME -t 999999999 & 
pid8=$!
sleep 2m

wait $pid8
wait $pid12

./RUN_FreeRTOS.sh -x 10 -y 10 -m $TYPE -s c_testing -n $NAME -t 999999999 & 
pid10=$!
sleep 5m

./RUN_FreeRTOS.sh -x 11 -y 11 -m $TYPE -s c_testing -n $NAME -t 999999999 & 
pid11=$!
sleep 2m

wait $pid10
wait $pid11