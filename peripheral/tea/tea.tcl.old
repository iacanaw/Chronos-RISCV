
imodelnewperipheral -name tea \
                    -constructor constructor \
                    -destructor  destructor \
                    -vendor gaph \
                    -library peripheral \
                    -version 1.0 

iadddocumentation -name Description \
                  -text "Termal Estimator Accelerator"

#########################################
## Data ports between routers
#########################################
imodeladdpacketnetport \
    -name portData \
    -maxbytes 4 \
    -updatefunction dataUpdate \
    -updatefunctionargument 0x00

#########################################
## Control ports between routers
#########################################
imodeladdpacketnetport \
    -name portControl \
    -maxbytes 8 \
    -updatefunction controlUpdate \
    -updatefunctionargument 0x00
