
imodelnewperipheral -name router \
                    -constructor constructor \
                    -destructor  destructor \
                    -vendor gaph \
                    -library peripheral \
                    -version 1.0 

iadddocumentation -name Description \
                  -text "A OVP Wormhole Router"

#########################################
# Master read and write ports
#########################################
imodeladdbusmasterport -name "RREAD"   -addresswidth 32
imodeladdbusmasterport -name "RWRITE"  -addresswidth 32

#########################################
## A slave port on the processor bus
#########################################
imodeladdbusslaveport -name localPort \
                      -size 4 \
                      -mustbeconnected

#########################################
## The whole address space of the slave port (four integers)
## is taken up by four registers
#########################################
imodeladdaddressblock -name regs   \
                      -port localPort \
                      -offset 0x0  \
                      -width 32    \
                      -size 4

imodeladdmmregister -name myAddress -readfunction addressRead -writefunction addressWrite -offset 0 

#########################################
## Data ports between routers
#########################################
imodeladdpacketnetport \
    -name portDataEast \
    -maxbytes 4 \
    -updatefunction dataEast \
    -updatefunctionargument 0x00

imodeladdpacketnetport \
    -name portDataWest \
    -maxbytes 4 \
    -updatefunction dataWest \
    -updatefunctionargument 0x00

imodeladdpacketnetport \
    -name portDataNorth \
    -maxbytes 4 \
    -updatefunction dataNorth \
    -updatefunctionargument 0x00

imodeladdpacketnetport \
    -name portDataSouth \
    -maxbytes 4 \
    -updatefunction dataSouth \
    -updatefunctionargument 0x00

imodeladdpacketnetport \
    -name portDataLocal \
    -maxbytes 4 \
    -updatefunction dataLocal \
    -updatefunctionargument 0x00

#########################################
## To Tick
#########################################

imodeladdpacketnetport \
    -name iterationsPort \
    -maxbytes 8 \
    -updatefunction iterationPort \
    -updatefunctionargument 0x00

#########################################
## Control ports between routers
#########################################
imodeladdpacketnetport \
    -name portControlEast \
    -maxbytes 8 \
    -updatefunction controlEast \
    -updatefunctionargument 0x00

imodeladdpacketnetport \
    -name portControlWest \
    -maxbytes 8 \
    -updatefunction controlWest \
    -updatefunctionargument 0x00

imodeladdpacketnetport \
    -name portControlNorth \
    -maxbytes 8 \
    -updatefunction controlNorth \
    -updatefunctionargument 0x00

imodeladdpacketnetport \
    -name portControlSouth \
    -maxbytes 8 \
    -updatefunction controlSouth \
    -updatefunctionargument 0x00

imodeladdpacketnetport \
    -name portControlLocal \
    -maxbytes 8 \
    -updatefunction controlLocal \
    -updatefunctionargument 0x00

iadddocumentation -name Description -text "Interrupt Request"
