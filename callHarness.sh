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
  --override dictsize=64                          \
  --override uart0/console=F                    \
  --override uart0/finishOnDisconnect=T         \
  --override uart0/outfile=simulation/uart0.log \
  \
  --program  cpu1=${FREERTOS_ELF}               \
  --override dictsize=64                          \
  --override uart1/console=F                    \
  --override uart1/finishOnDisconnect=T         \
  --override uart1/outfile=simulation/uart1.log \
  \
  --program  cpu2=${FREERTOS_ELF}               \
  --override dictsize=64                          \
  --override uart2/console=F                    \
  --override uart2/finishOnDisconnect=T         \
  --override uart2/outfile=simulation/uart2.log \
  \
  --program  cpu3=${FREERTOS_ELF}               \
  --override dictsize=64                          \
  --override uart3/console=F                    \
  --override uart3/finishOnDisconnect=T         \
  --override uart3/outfile=simulation/uart3.log \
  \
  --program  cpu4=${FREERTOS_ELF}               \
  --override dictsize=64                          \
  --override uart4/console=F                    \
  --override uart4/finishOnDisconnect=T         \
  --override uart4/outfile=simulation/uart4.log \
  \
  --program  cpu5=${FREERTOS_ELF}               \
  --override dictsize=64                          \
  --override uart5/console=F                    \
  --override uart5/finishOnDisconnect=T         \
  --override uart5/outfile=simulation/uart5.log \
  \
  --program  cpu6=${FREERTOS_ELF}               \
  --override dictsize=64                          \
  --override uart6/console=F                    \
  --override uart6/finishOnDisconnect=T         \
  --override uart6/outfile=simulation/uart6.log \
  \
  --program  cpu7=${FREERTOS_ELF}               \
  --override dictsize=64                          \
  --override uart7/console=F                    \
  --override uart7/finishOnDisconnect=T         \
  --override uart7/outfile=simulation/uart7.log \
  \
  --program  cpu8=${FREERTOS_ELF}               \
  --override dictsize=64                          \
  --override uart8/console=F                    \
  --override uart8/finishOnDisconnect=T         \
  --override uart8/outfile=simulation/uart8.log \
  \
  --program  cpu9=${FREERTOS_ELF}               \
  --override dictsize=64                          \
  --override uart9/console=F                    \
  --override uart9/finishOnDisconnect=T         \
  --override uart9/outfile=simulation/uart9.log \
  \
  --program  cpu10=${FREERTOS_ELF}               \
  --override dictsize=64                          \
  --override uart10/console=F                    \
  --override uart10/finishOnDisconnect=T         \
  --override uart10/outfile=simulation/uart10.log \
  \
  --program  cpu11=${FREERTOS_ELF}               \
  --override dictsize=64                          \
  --override uart11/console=F                    \
  --override uart11/finishOnDisconnect=T         \
  --override uart11/outfile=simulation/uart11.log \
  \
  --program  cpu12=${FREERTOS_ELF}               \
  --override dictsize=64                          \
  --override uart12/console=F                    \
  --override uart12/finishOnDisconnect=T         \
  --override uart12/outfile=simulation/uart12.log \
  \
  --program  cpu13=${FREERTOS_ELF}               \
  --override dictsize=64                          \
  --override uart13/console=F                    \
  --override uart13/finishOnDisconnect=T         \
  --override uart13/outfile=simulation/uart13.log \
  \
  --program  cpu14=${FREERTOS_ELF}               \
  --override dictsize=64                          \
  --override uart14/console=F                    \
  --override uart14/finishOnDisconnect=T         \
  --override uart14/outfile=simulation/uart14.log \
  \
  --program  cpu15=${FREERTOS_ELF}               \
  --override dictsize=64                                 \
  --override uart15/console=F                    \
  --override uart15/finishOnDisconnect=T         \
  --override uart15/outfile=simulation/uart15.log $* --verbose --parallelmax --parallelperipherals --output simulation/imperas.log
