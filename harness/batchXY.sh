#!/bin/bash

X=$1
Y=$2
NAME=$3
SCENARIO=$4

for (( ; ; ))
do
    ./RUN_FreeRTOS.sh -x $X -y $Y -m pidtm -s $SCENARIO -n $NAME -t 999999999 & 
    pid_pidtm=$!
    sleep 5m
    if ps -p $pid_pidtm > /dev/null
    then
        break
    fi
done

# for (( ; ; ))
# do
#     ./RUN_FreeRTOS.sh -x $X -y $Y -m chronos2 -s $SCENARIO -n $NAME -t 999999999 & 
#     pid_chronos2=$!
#     sleep 5m
#     if ps -p $pid_chronos2 > /dev/null
#     then
#         break
#     fi
# done

for (( ; ; ))
do
    ./RUN_FreeRTOS.sh -x $X -y $Y -m chronos3 -s $SCENARIO -n $NAME -t 999999999 & 
    pid_chronos3=$!
    sleep 5m
    if ps -p $pid_chronos3 > /dev/null
    then
        break
    fi
done

for (( ; ; ))
do
    ./RUN_FreeRTOS.sh -x $X -y $Y -m pattern -s $SCENARIO -n $NAME -t 999999999 & 
    pid_pattern=$!
    sleep 5m
    if ps -p $pid_pattern > /dev/null
    then
        break
    fi
done


wait $pid_pidtm
wait $pid_chronos2
wait $pid_chronos3
wait $pid_pattern