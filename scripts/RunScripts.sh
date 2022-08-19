#!/bin/bash


# Parameters
XX=$1
YY=$2
SimType=$3

# generates the temperature graph
python3 scripts/graphTemperature.py 

# generates the instruction graph
python3 scripts/graphInstructions.py "$XX" "$YY"

# generates the instruction graph with the FIT
python3 scripts/graphInstructionsFIT.py "$XX" "$YY"

# generates files to run the debugger (ferramenta do ruaro)
python3 scripts/filter_debug.py 

# generates the occupation graph
python3 scripts/occupation.py "$XX" "$YY" >> simulation/occupation.txt

# calculates the MTTF using the montecarlo algorithm 
sed -i 's/#define TOTAL_STRUCTURES.*/#define TOTAL_STRUCTURES '$XX'*'$YY'/' scripts/montecarlo.c
gcc scripts/montecarlo.c -o simulation/montecarlo -lm
cd simulation
./montecarlo montecarlofile >> mttflog.txt
cd ..

# create a CSV file with data about the simulation
python3 scripts/csvGen.py "$XX" "$YY" "$SimType"