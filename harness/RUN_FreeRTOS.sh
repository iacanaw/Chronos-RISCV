#!/bin/bash

################################################################################
# Help                                                                         #
################################################################################
Help(){
    # Display Help
    echo "Starts the RISC-V Chronos platform."
    echo
    echo "Syntax: RUN_FreeRTOS.sh [-n|x|y|t|s|m]"
    echo "options:"
    echo "-n     Simulation name (default uses the date and time to generate a name)."
    echo "-x     Defines the amount of PEs in the X coordinate (default: 5)."
    echo "-y     Defines the amount of PEs in the Y coordinate (default: 5)."
    echo "-t     Defines the amount of time that the simulation will last."
    echo "       If not defined the simulation will last until every application finishes."
    echo "-s     Defines the scenario source file."
    echo "       If not defined or if the file does not exist the system will run empty."
    echo "-m     Choose the management method [spiral|pidtm|pattern|chronos|characterize]"
    echo "-g     Choose if the migration algorithm is working or not [yes][no]"
}

if [ "$1" == "-h" ]; then
    Help
    exit
fi

while getopts ":h:n:x:y:t:s:m:g:" option; do
  case $option in
    h) # display Help
        Help
        exit;;
    n) # Enter a name
        SimName=$OPTARG
        echo "Simulation name defined by the user";;
    x) # defines the dimension size (axis x)
        XX=$OPTARG
        echo "The X axis is defined by the user: "$XX;;
    y) # defines the dimension size (axis y)
        YY=$OPTARG
        echo "The Y axis is defined by the user: "$YY;;
    m) #defines the type of simulation (pattern, pidtm, nossa tecnica)
        SimType=$OPTARG
        echo "Using the thermal management technique: "$SimType;;
    s) #defines the scenario name (without the extension (.yaml))
        ScenarioName=$OPTARG
        echo "Using the scenario named: "$ScenarioName;;
    t) #defines the simulation max time
        SimulationMaxTime=$OPTARG
        echo "Simulation will finish at "$SimulationMaxTime;;
    g) #defines the migration algorithm
        Migration=$OPTARG;;
    \?) # Invalid option
        echo "Error: Invalid option"
        exit;;
   esac
done
shift $((OPTIND-1))

# if [ -z ${SimType} ];
# then
#     SimType="PIDTM"
# fi

if [ -z ${SimulationMaxTime} ];
then
    SimulationMaxTime=0
    echo "Default Simulation time: "$(($SimulationMaxTime))
fi

if [ -z ${ScenarioName} ];
then
    ScenarioName='none'
    echo "Default Scenario name: "$(($ScenarioName))
fi

if [ -z ${XX} ];
then
    XX=5
    echo "Default X axis dimention: "$(($XX))
fi

if [ -z ${YY} ];
then
    YY=5
    echo "Default Y axis dimention: "$(($YY))
fi

if [ -z ${Migration} ];
then
    Migration='yes'
    echo "Default migration is ACTIVE"
fi

if [ -z ${SimName} ];
then
    SimName=$(date +"%d%m%Y%H%M%S")"_"$SimType"_"$(($XX))"x"$(($YY))
    echo "Default simulation name: "$SimName
else
    SimName=$SimName"_"$ScenarioName"_"$SimulationMaxTime"ticks_"$SimType"_"$(($XX))"x"$(($YY))
fi

N=$(($XX*$YY))
echo "Number of PEs in this simulation: "$(($N))

cd ../simulation
if [ -d "$SimName" ]; then rm -Rf $SimName; fi
mkdir $SimName
cd $SimName
mkdir simulation
cp -r ../../applications .
cp -r ../../FreeRTOS .
cp -r ../../harness .
cp -r ../../module .
cp -r ../../peripheral .
cp -r ../../scripts .


# create the platform.cfg file
cd simulation
mkdir debug
cd debug
echo "router_addressing XY" >> platform.cfg
echo "channel_number 1" >> platform.cfg
echo "mpsoc_x "$XX"" >> platform.cfg
echo "mpsoc_y "$YY"" >> platform.cfg
echo "flit_size 32" >> platform.cfg
echo "clock_period_ns 10" >> platform.cfg
echo "cluster_x "$XX"" >> platform.cfg
echo "cluster_y "$YY"" >> platform.cfg
echo "manager_position_x 0" >> platform.cfg
echo "manager_position_y 0" >> platform.cfg

echo "TASK_ALLOCATION 40" >> services.cfg
echo "TASK_TERMINATED 70" >> services.cfg
echo "" >> services.cfg
echo "\$TASK_ALLOCATION_SERVICE 40 221" >> services.cfg
echo "\$TASK_TERMINATED_SERVICE 70 221" >> services.cfg
cd ..
cd ..

if [[ $SimType == "spiral" ]]
then
    sed -i 's/#define THERMAL_MANAGEMENT.*/#define THERMAL_MANAGEMENT 0/' FreeRTOS/main.c
elif [[ $SimType == "pattern" ]]
then
    sed -i 's/#define THERMAL_MANAGEMENT.*/#define THERMAL_MANAGEMENT 1/' FreeRTOS/main.c
elif [[ $SimType == "pidtm" ]]
then
    sed -i 's/#define THERMAL_MANAGEMENT.*/#define THERMAL_MANAGEMENT 2/' FreeRTOS/main.c
elif [[ $SimType == "chronos" ]]
then
    sed -i 's/#define THERMAL_MANAGEMENT.*/#define THERMAL_MANAGEMENT 3/' FreeRTOS/main.c
elif [[ $SimType == "characterize" ]]
then
    sed -i 's/#define THERMAL_MANAGEMENT.*/#define THERMAL_MANAGEMENT 4/' FreeRTOS/main.c
elif [[ $SimType == "chronos2" ]]
then
    sed -i 's/#define THERMAL_MANAGEMENT.*/#define THERMAL_MANAGEMENT 5/' FreeRTOS/main.c
elif [[ $SimType == "chronos3" ]]
then
    sed -i 's/#define THERMAL_MANAGEMENT.*/#define THERMAL_MANAGEMENT 6/' FreeRTOS/main.c
else
    echo "Error: the -m option must be defined as \"pattern\", \"pidtm\" or \"chronos\"."
    exit
fi

if [[ $Migration == "yes" ]]
then
    sed -i 's/#define MIGRATION.*/#define MIGRATION 1/' FreeRTOS/main.c
elif [[ $Migration == "no" ]]
then
    sed -i 's/#define MIGRATION.*/#define MIGRATION 0/' FreeRTOS/main.c
else
    echo "Error: Migration not defined!"
    exit
fi

sed -i 's/#define SIMULATION_MAX_TICKS.*/#define SIMULATION_MAX_TICKS '$SimulationMaxTime'/' FreeRTOS/main.c

cd harness

SECONDS=0;
echo "====================="
echo "COMPILING APLICATIONS"
cd ../applications
for d in */ ; do
  cd $d
  make all
  cd ..
done
echo "======================="
echo "CREATING THE REPOSITORY"
python3 repositoryMaker.py $ScenarioName
# for d in */ ; do
#   cd $d
#   make clean
#   cd ..
# done
cd ../harness
echo "=================="
echo "COMPILING FREERTOS"
cd ../FreeRTOS/FreeRTOS/include
sed -i 's/#define DIM_X.*/#define DIM_X '$XX'/' chronos.h
sed -i 's/#define DIM_Y.*/#define DIM_Y '$YY'/' chronos.h
cd ../..
./BUILD.sh clean
./BUILD.sh
cd ../harness

# Check Installation supports this demo
checkinstall.exe -p install.pkg --nobanner || exit
echo "===================="
echo "COMPILING THE ROUTER"
cd ../peripheral/whnoc_dma
sed -i 's/#define DIM_X.*/#define DIM_X '$XX'/' noc.h
sed -i 's/#define DIM_Y.*/#define DIM_Y '$YY'/' noc.h
rm -rf obj
rm -f pse.pse
make NOVLNV=1
echo "======================"
echo "COMPILING THE ITERATOR"
cd ../iterator
rm -rf obj
rm -f pse.pse
./iteratorGenerator.sh "$XX" "$YY"
make NOVLNV=1
echo "==================="
echo "COMPILING THE TIMER"
cd ../timer
rm -rf obj
rm -f pse.pse
make NOVLNV=1
echo "=================="
echo "COMPILING THE PLIC"
cd ../plic
rm -rf obj
rm -f pse.pse
make NOVLNV=1
echo "==================="
echo "COMPILING THE CLINT"
cd ../CLINT
rm -rf obj
rm -f pse.pse
make NOVLNV=1
echo "==============================="
echo "COMPILING THE NETWORK INTERFACE"
cd ../networkInterface
rm -rf obj
rm -f pse.pse
make NOVLNV=1
echo "================="
echo "COMPILING THE TEA"
cd ../tea
cd matex
python3 flp_gen.py "$XX" "$YY"
./MatEx -c input/chronos.cfg -f input/floorplan.flp -p input/power.pwr
cd ..
rm -rf obj
rm -f pse.pse
make NOVLNV=1
echo "========================"
echo "COMPILING THE REPOSITORY"
cd ../repository
rm -rf obj
rm -f pse.pse
make NOVLNV=1
echo "====================="
echo "COMPILING THE PRINTER"
cd ../printer
rm -rf obj
rm -f pse.pse
make NOVLNV=1
echo "===================="
echo "COMPILING THE MODULE"
cd ../../module
sh moduleGenerator.sh "$XX" "$YY"
make clean
make NOVLNV=1
cd ..
echo "====================="
echo "COMPILING THE HARNESS"
cd harness
sh harnessGenerator.sh "$XX" "$YY"
cd ..
rm -rf harness/obj
rm harness/harness.Linux64.exe
make -C harness
rm callHarness.sh
echo "FREERTOS_ELF=FreeRTOS/Debug/miv-rv32im-freertos-port-test.elf" >> callHarness.sh
echo "echo \"===============================\"" >> callHarness.sh
echo "echo \"CLEANING THE SIMULATION FOLDER\"" >> callHarness.sh
echo "cd simulation" >> callHarness.sh
echo "rm -f *.txt" >> callHarness.sh
echo "rm -f *.log" >> callHarness.sh
echo "cd .." >> callHarness.sh
echo "MODULE=Chronos_RiscV_FreeRTOS" >> callHarness.sh
echo "cd harness " >> callHarness.sh
echo "# Check Installation supports this example" >> callHarness.sh
echo "checkinstall.exe -p install.pkg --nobanner || exit" >> callHarness.sh
echo "cd .." >> callHarness.sh
echo "" >> callHarness.sh
echo "harness/harness.${IMPERAS_ARCH}.exe             \\" >> callHarness.sh
for i in $(seq 0 $(($N-2)));
do
echo "  \\" >> callHarness.sh
echo "  --program  cpu"$i"=\${FREERTOS_ELF}               \\" >> callHarness.sh
echo "  --override dictsize=64                          \\" >> callHarness.sh
echo "  --override uart"$i"/console=F                    \\" >> callHarness.sh
echo "  --override uart"$i"/finishOnDisconnect=T         \\" >> callHarness.sh
echo "  --override uart"$i"/outfile=simulation/uart"$i".log \\" >> callHarness.sh
done
echo "  \\" >> callHarness.sh
echo "  --program  cpu"$(($N-1))"=\${FREERTOS_ELF}               \\" >> callHarness.sh
echo "  --override dictsize=64                                 \\" >> callHarness.sh
echo "  --override uart"$(($N-1))"/console=F                    \\" >> callHarness.sh
echo "  --override uart"$(($N-1))"/finishOnDisconnect=T         \\" >> callHarness.sh
echo "  --override uart"$(($N-1))"/outfile=simulation/uart"$(($N-1))".log \$* --verbose --parallelmax --parallelperipherals --output simulation/imperas.log" >> callHarness.sh
#--imperasintercepts            --parallelmax --parallelperipherals                         \

chmod +x callHarness.sh
./callHarness.sh

echo "Simulation total time elapsed: "$SECONDS" seconds..."
echo "Simulation took: $(($SECONDS / 3600))hrs $((($SECONDS / 60) % 60))min $(($SECONDS % 60))sec" >> simulationTime.txt

cd simulation
sed -i '2,10d' FITlog.tsv # removes the first 8 samples from the FIT file
sed -i '1,24d' matex.txt # removes the first 8 samples from the matex file
sed -i '2,10d' SystemTemperature.tsv # removes the first 8 samples from the matex file
sed -i '2,10d' SystemPower.tsv # removes the first 8 samples from the matex file
cd ..

./scripts/RunScripts.sh "$XX" "$YY" "$SimType" "$ScenarioName"
