imodelnewperipheral -name iterator \
                    -constructor constructor \
                    -destructor  destructor \
                    -vendor gaph \
                    -library peripheral \
                    -version 1.0

iadddocumentation -name Description \
                  -text "The NoC iterator"

#########################################
## A slave port on the processor bus
#########################################


#############################################
## Data ports between iterator and routers ##
#############################################
imodeladdpacketnetport  \
      -name iterationPort0 \
      -maxbytes 8 \
      -updatefunction iteration0 \
      -updatefunctionargument 0x00

imodeladdpacketnetport  \
      -name iterationPort1 \
      -maxbytes 8 \
      -updatefunction iteration1 \
      -updatefunctionargument 0x00

imodeladdpacketnetport  \
      -name iterationPort2 \
      -maxbytes 8 \
      -updatefunction iteration2 \
      -updatefunctionargument 0x00

imodeladdpacketnetport  \
      -name iterationPort3 \
      -maxbytes 8 \
      -updatefunction iteration3 \
      -updatefunctionargument 0x00

imodeladdpacketnetport  \
      -name iterationPort4 \
      -maxbytes 8 \
      -updatefunction iteration4 \
      -updatefunctionargument 0x00

imodeladdpacketnetport  \
      -name iterationPort5 \
      -maxbytes 8 \
      -updatefunction iteration5 \
      -updatefunctionargument 0x00

imodeladdpacketnetport  \
      -name iterationPort6 \
      -maxbytes 8 \
      -updatefunction iteration6 \
      -updatefunctionargument 0x00

imodeladdpacketnetport  \
      -name iterationPort7 \
      -maxbytes 8 \
      -updatefunction iteration7 \
      -updatefunctionargument 0x00

imodeladdpacketnetport  \
      -name iterationPort8 \
      -maxbytes 8 \
      -updatefunction iteration8 \
      -updatefunctionargument 0x00

imodeladdpacketnetport  \
      -name iterationPort9 \
      -maxbytes 8 \
      -updatefunction iteration9 \
      -updatefunctionargument 0x00

imodeladdpacketnetport  \
      -name iterationPort10 \
      -maxbytes 8 \
      -updatefunction iteration10 \
      -updatefunctionargument 0x00

imodeladdpacketnetport  \
      -name iterationPort11 \
      -maxbytes 8 \
      -updatefunction iteration11 \
      -updatefunctionargument 0x00

imodeladdpacketnetport  \
      -name iterationPort12 \
      -maxbytes 8 \
      -updatefunction iteration12 \
      -updatefunctionargument 0x00

imodeladdpacketnetport  \
      -name iterationPort13 \
      -maxbytes 8 \
      -updatefunction iteration13 \
      -updatefunctionargument 0x00

imodeladdpacketnetport  \
      -name iterationPort14 \
      -maxbytes 8 \
      -updatefunction iteration14 \
      -updatefunctionargument 0x00

imodeladdpacketnetport  \
      -name iterationPort15 \
      -maxbytes 8 \
      -updatefunction iteration15 \
      -updatefunctionargument 0x00

imodeladdpacketnetport  \
      -name iterationPort16 \
      -maxbytes 8 \
      -updatefunction iteration16 \
      -updatefunctionargument 0x00

imodeladdpacketnetport  \
      -name iterationPort17 \
      -maxbytes 8 \
      -updatefunction iteration17 \
      -updatefunctionargument 0x00

imodeladdpacketnetport  \
      -name iterationPort18 \
      -maxbytes 8 \
      -updatefunction iteration18 \
      -updatefunctionargument 0x00

imodeladdpacketnetport  \
      -name iterationPort19 \
      -maxbytes 8 \
      -updatefunction iteration19 \
      -updatefunctionargument 0x00

imodeladdpacketnetport  \
      -name iterationPort20 \
      -maxbytes 8 \
      -updatefunction iteration20 \
      -updatefunctionargument 0x00

imodeladdpacketnetport  \
      -name iterationPort21 \
      -maxbytes 8 \
      -updatefunction iteration21 \
      -updatefunctionargument 0x00

imodeladdpacketnetport  \
      -name iterationPort22 \
      -maxbytes 8 \
      -updatefunction iteration22 \
      -updatefunctionargument 0x00

imodeladdpacketnetport  \
      -name iterationPort23 \
      -maxbytes 8 \
      -updatefunction iteration23 \
      -updatefunctionargument 0x00

imodeladdpacketnetport  \
      -name iterationPort24 \
      -maxbytes 8 \
      -updatefunction iteration24 \
      -updatefunctionargument 0x00

