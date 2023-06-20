imodelnewperipheral -name networkInterface \
                    -constructor constructor \
                    -destructor  destructor \
                    -vendor gaph \
                    -library peripheral \
                    -version 1.0 

iadddocumentation -name Description \
                  -text "A OVP printer"
                  
#########################################
## A slave port on the processor bus
#########################################
imodeladdbusslaveport -name PRINTREGS -size 8 -mustbeconnected

# Address block for 8 bit control registers
imodeladdaddressblock -name ab8 -port PRINTREGS -offset 0x0 -width 32 -size 8

# 8 bit control registers
imodeladdmmregister -addressblock PRINTREGS/ab8 -name printValue_char -readfunction readValue_char -writefunction writeValue_char -offset 0
imodeladdmmregister -addressblock PRINTREGS/ab8 -name printValue_int -readfunction readValue_int -writefunction writeValue_int -offset 4