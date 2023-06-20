imodelnewperipheral -name networkInterface \
                    -constructor constructor \
                    -destructor  destructor \
                    -vendor gaph \
                    -library peripheral \
                    -version 1.0 

iadddocumentation -name Description \
                  -text "A OVP DMA for a router"
                  
#########################################
# Master read and write ports
#########################################
imodeladdbusmasterport -name "MREAD"   -addresswidth 32
imodeladdbusmasterport -name "MWRITE"  -addresswidth 32

#########################################
## A slave port on the processor bus
#########################################
imodeladdbusslaveport -name DMAC -size 16 -mustbeconnected

# Address block for 8 bit control registers
imodeladdaddressblock -name ab8 -port DMAC -offset 0x0 -width 32 -size 16

# 8 bit control registers
imodeladdmmregister -addressblock DMAC/ab8 -name address -readfunction addressRead -writefunction addressWrite -offset 0
imodeladdmmregister -addressblock DMAC/ab8 -name statusTX -readfunction statusTXRead -writefunction statusTXWrite -offset 4
imodeladdmmregister -addressblock DMAC/ab8 -name statusRX -readfunction statusRXRead -writefunction statusRXWrite -offset 8
imodeladdmmregister -addressblock DMAC/ab8 -name timer -readfunction timerRead -writefunction timerWrite -offset 12


#########################################
## Data ports between routers
#########################################
imodeladdpacketnetport \
    -name dataPort \
    -maxbytes 4 \
    -updatefunction dataPortUpd \
    -updatefunctionargument 0x00

imodeladdpacketnetport \
    -name controlPort \
    -maxbytes 8 \
    -updatefunction controlPortUpd \
    -updatefunctionargument 0x00

#########################################
## Processor interrupt line
#########################################
imodeladdnetport -name INT_NI_RX -type output
iadddocumentation -name Description -text "NI RX Interrupt Request"
imodeladdnetport -name INT_NI_TX -type output
iadddocumentation -name Description -text "NI TX Interrupt Request"
imodeladdnetport -name INT_NI_TMR -type output
iadddocumentation -name Description -text "NI TMR Interrupt Request"