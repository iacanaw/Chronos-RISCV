#!/bin/bash

SECONDS=0;
echo "==============================="
echo "CLEANING THE SIMULATION FOLDER"
cd ../simulation
rm -f *.txt
cd ../harness
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
  #make clean
  cd ..
done
cd ../harness
echo "=================="
echo "COMPILING FREERTOS"
cd ../FreeRTOS
./BUILD.sh clean
./BUILD.sh
cd ../harness

# Check Installation supports this demo
checkinstall.exe -p install.pkg --nobanner || exit
echo "===================="
echo "COMPILING THE ROUTER"
cd ../peripheral/whnoc_dma
rm -rf obj
rm -f pse.pse
make NOVLNV=1
echo "======================"
echo "COMPILING THE ITERATOR"
cd ../iterator
rm -rf obj
rm -f pse.pse
./iteratorGenerator.sh 3 3
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
sh moduleGenerator.sh 3 3
make clean
make NOVLNV=1
cd ..
echo "====================="
echo "COMPILING THE HARNESS"
rm harness/harness.Linux64.exe
make -C harness

FREERTOS_ELF=FreeRTOS/Debug/miv-rv32im-freertos-port-test.elf
MODULE=Chronos_RiscV_FreeRTOS
cd harness
# Check Installation supports this example
checkinstall.exe -p install.pkg --nobanner || exit
cd ..
#cd harness
#harness.exe \
#  --modulefile ../module/model.${IMPERAS_ARCH}.${IMPERAS_SHRSUF} \
# trocar "../" no TEA, moduleGenerator e nesse script
#colocar: ${MODULE}/


harness/harness.${IMPERAS_ARCH}.exe             \
  \
  --program  cpu0=${FREERTOS_ELF}               \
  --override uart0/console=T                    \
  --override uart0/finishOnDisconnect=T         \
  --override uart0/outfile=simulation/uart0.log \
  \
  --program  cpu1=${FREERTOS_ELF}               \
  --override uart1/console=T                    \
  --override uart1/finishOnDisconnect=T         \
  --override uart1/outfile=simulation/uart1.log \
  \
  --program  cpu2=${FREERTOS_ELF}               \
  --override uart2/console=T                    \
  --override uart2/finishOnDisconnect=T         \
  --override uart2/outfile=simulation/uart2.log \
  \
  --program  cpu3=${FREERTOS_ELF}               \
  --override uart3/console=T                    \
  --override uart3/finishOnDisconnect=T         \
  --override uart3/outfile=simulation/uart3.log \
  \
  --program  cpu4=${FREERTOS_ELF}               \
  --override uart4/console=T                    \
  --override uart4/finishOnDisconnect=T         \
  --override uart4/outfile=simulation/uart4.log \
  \
  --program  cpu5=${FREERTOS_ELF}               \
  --override uart5/console=T                    \
  --override uart5/finishOnDisconnect=T         \
  --override uart5/outfile=simulation/uart5.log \
  \
  --program  cpu6=${FREERTOS_ELF}               \
  --override uart6/console=T                    \
  --override uart6/finishOnDisconnect=T         \
  --override uart6/outfile=simulation/uart6.log \
  \
  --program  cpu7=${FREERTOS_ELF}               \
  --override uart7/console=T                    \
  --override uart7/finishOnDisconnect=T         \
  --override uart7/outfile=simulation/uart7.log \
  \
  --program  cpu8=${FREERTOS_ELF}               \
  --override uart8/console=T                    \
  --override uart8/finishOnDisconnect=T         \
  --override uart8/outfile=simulation/uart8.log $* --verbose  --output simulation/imperas.log
#--imperasintercepts                                     \
# --parallelperipherals

echo "Simulation total time elapsed: "$SECONDS" seconds..."