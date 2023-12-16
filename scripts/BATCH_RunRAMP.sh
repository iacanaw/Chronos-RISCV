#!/bin/bash

# Parameters
XX=$1
YY=$2
FOLDER=$3

RETURNFOLDER=pwd

# copy data from the simulation folder
cp $FOLDER/scripts_data/matex.log $FOLDER/scripts/RAMP/matex.log

cd $FOLDER/scripts/RAMP/
# changes the system size
sed -i 's/#define DIM_X.*/#define DIM_X '$XX'/' montecarlo.c
sed -i 's/#define DIM_Y.*/#define DIM_Y '$YY'/' montecarlo.c  
sed -i 's/#define DIM_X.*/#define DIM_X '$XX'/' reliability.h
sed -i 's/#define DIM_Y.*/#define DIM_Y '$YY'/' reliability.h

# compile tools
g++ ramp_main.c reliability.cc -o ramp_main
gcc montecarlo.c -o montecarlo -lm

echo "Starting RAMP to generate statistics!"
# run the RAMP to generate the statistics

./ramp_main

echo "Starting MONTECARLO simulation to obtain the MTTF!"
# call the montecarlo to generate the system MTTF 
./montecarlo montecarlofile > ../../scripts_data/mttflog.txt
cp montecarlofile ../../scripts_data/montecarlofile
#cd $RETURNFOLDER
