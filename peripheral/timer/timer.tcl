imodelnewperipheral -name networkInterface \
                    -constructor constructor \
                    -destructor  destructor \
                    -vendor gaph \
                    -library peripheral \
                    -version 1.0 

iadddocumentation -name Description \
                  -text "A OVP DMA for a router"
                  
#########################################
## A slave port on the processor bus
#########################################
imodeladdbusslaveport -name TIMEREG -size 4 -mustbeconnected

# Address block for 8 bit control registers
imodeladdaddressblock -name ab8 -port TIMEREG -offset 0x0 -width 32 -size 4

# 8 bit control registers
imodeladdmmregister -addressblock TIMEREG/ab8 -name timerCfg -readfunction cfgRead -writefunction cfgWrite -offset 0

#########################################
## Processor interrupt line
#########################################
imodeladdnetport -name INT_TIMER -type output
iadddocumentation -name Description -text "Timer Interrupt Request"