#!/bin/bash

# Parameters
XX=$1
YY=$2

# copy data from the simulation folder
cp simulation/matex.txt scripts/RAMP/matex.log

# changes the system size
sed -i 's/#define DIM_X.*/#define DIM_X '$XX'/' scripts/RAMP/montecarlo.c
sed -i 's/#define DIM_Y.*/#define DIM_Y '$YY'/' scripts/RAMP/montecarlo.c  
sed -i 's/#define DIM_X.*/#define DIM_X '$XX'/' scripts/RAMP/reliability.h
sed -i 's/#define DIM_Y.*/#define DIM_Y '$YY'/' scripts/RAMP/reliability.h

cd scripts/RAMP

# compile tools
g++ ramp_main.c reliability.cc -o ramp_main
gcc montecarlo.c -o montecarlo -lm

# run the RAMP to generate the statistics
./ramp_main

# call the montecarlo to generate the system MTTF 
./montecarlo montecarlofile > ../../simulation/mttflog.txt

# return to the base folder
cd ../..