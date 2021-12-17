FREERTOS_ELF=FreeRTOS/Debug/miv-rv32im-freertos-port-test.elf
echo "==============================="
echo "CLEANING THE SIMULATION FOLDER"
cd simulation
rm -f *.txt
rm -f *.log
cd ..
MODULE=Chronos_RiscV_FreeRTOS
cd harness 
# Check Installation supports this example
checkinstall.exe -p install.pkg --nobanner || exit
cd ..

harness/harness.Linux64.exe             \
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
  --override uart8/outfile=simulation/uart8.log  --verbose --parallelmax --parallelperipherals --output simulation/imperas.log
