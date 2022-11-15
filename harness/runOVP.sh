#!/bin/bash

X=$1
TIME=$2
MANAGER=$3
SCENARIO=$4
NAME=$5
MIGRATION=$6

#for (( ; ; )) do
    pwd
    ./RUN_FreeRTOS.sh -x $X -y $X -m $MANAGER -s $SCENARIO -n $NAME -t $TIME -g $MIGRATION #& 
    # pid_val=$!
    # sleep 6m
    # if ps -p $pid_val > /dev/null
    # then
    #     break
    # fi
#done

wait $pid_val