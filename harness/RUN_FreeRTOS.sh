#!/bin/bash

while getopts ":h:n:x:y:t:" option; do
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
    t) #defines the type of simulation (pattern, pidtm, nossa tecnica)
        SimType=$OPTARG
        echo "Using the thermal management technique: "$SimType;;
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

if [ -z ${SimName} ];
then
    SimName=$(date +"%d%m%Y%H%M%S")"_"$SimType"_"$(($XX))"x"$(($YY))
    echo "Default simulation name: "$SimName
fi

N=$(($XX*$YY))
echo "Number of PEs in this simulation: "$(($N))

cd ../simulation
mkdir $SimName
cd $SimName
mkdir simulation
cp -r ../../applications .
cp -r ../../FreeRTOS .
cp -r ../../harness .
cp -r ../../module .
cp -r ../../peripheral .
cp -r ../../scripts .

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
else
    echo "Error: the -t option must be defined as \"pattern\", \"pidtm\" or \"chronos\"."
    exit
fi

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
python3 repositoryMaker.py myscenario
for d in */ ; do
  cd $d
  make clean
  cd ..
done
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
echo "  --override uart"$i"/console=T                    \\" >> callHarness.sh
echo "  --override uart"$i"/finishOnDisconnect=T         \\" >> callHarness.sh
echo "  --override uart"$i"/outfile=simulation/uart"$i".log \\" >> callHarness.sh
done
echo "  \\" >> callHarness.sh
echo "  --program  cpu"$(($N-1))"=\${FREERTOS_ELF}               \\" >> callHarness.sh
echo "  --override dictsize=64                                 \\" >> callHarness.sh
echo "  --override uart"$(($N-1))"/console=T                    \\" >> callHarness.sh
echo "  --override uart"$(($N-1))"/finishOnDisconnect=T         \\" >> callHarness.sh
echo "  --override uart"$(($N-1))"/outfile=simulation/uart"$(($N-1))".log \$* --verbose --parallelmax --parallelperipherals --output simulation/imperas.log" >> callHarness.sh
#--imperasintercepts                                     \

chmod +x callHarness.sh
./callHarness.sh

echo "Simulation total time elapsed: "$SECONDS" seconds..."

python3 scripts/graphTemperature.py 
python3 scripts/graphInstructions.py "$XX" "$YY"

shopt -s extglob
rm -rfv !('simulation');
cp simulation/* .
rm -rf simulation
