
////////////////////////////////////////////////////////////////////////////////
//
//                W R I T T E N   B Y   I M P E R A S   I G E N
//
//                             Version 20210408.0
//
////////////////////////////////////////////////////////////////////////////////


#ifdef _PSE_
#    include "peripheral/impTypes.h"
#    include "peripheral/bhm.h"
#    include "peripheral/bhmHttp.h"
#    include "peripheral/ppm.h"
#else
#    include "hostapi/impTypes.h"
#endif

#include "iterator.igen.h"


//                     Shared data for packetnet interface

Uns8 iterationPort0_pnsd[8];
Uns8 iterationPort1_pnsd[8];
Uns8 iterationPort2_pnsd[8];
Uns8 iterationPort3_pnsd[8];
Uns8 iterationPort4_pnsd[8];
Uns8 iterationPort5_pnsd[8];
Uns8 iterationPort6_pnsd[8];
Uns8 iterationPort7_pnsd[8];
Uns8 iterationPort8_pnsd[8];
Uns8 iterationPort9_pnsd[8];
Uns8 iterationPort10_pnsd[8];
Uns8 iterationPort11_pnsd[8];
Uns8 iterationPort12_pnsd[8];
Uns8 iterationPort13_pnsd[8];
Uns8 iterationPort14_pnsd[8];
Uns8 iterationPort15_pnsd[8];
Uns8 iterationPort16_pnsd[8];
Uns8 iterationPort17_pnsd[8];
Uns8 iterationPort18_pnsd[8];
Uns8 iterationPort19_pnsd[8];
Uns8 iterationPort20_pnsd[8];
Uns8 iterationPort21_pnsd[8];
Uns8 iterationPort22_pnsd[8];
Uns8 iterationPort23_pnsd[8];
Uns8 iterationPort24_pnsd[8];

static ppmPacketnetPort packetnetPorts[] = {
    {
        .name            = "iterationPort0",
        .mustBeConnected = 0,
        .description     = 0,
        .sharedData      = iterationPort0_pnsd,
        .sharedDataBytes = 8,
        .handlePtr       = &handles.iterationPort0,
        .packetnetCB     = iteration0,
        .userData        = (void*)0
    },
    {
        .name            = "iterationPort1",
        .mustBeConnected = 0,
        .description     = 0,
        .sharedData      = iterationPort1_pnsd,
        .sharedDataBytes = 8,
        .handlePtr       = &handles.iterationPort1,
        .packetnetCB     = iteration1,
        .userData        = (void*)0
    },
    {
        .name            = "iterationPort2",
        .mustBeConnected = 0,
        .description     = 0,
        .sharedData      = iterationPort2_pnsd,
        .sharedDataBytes = 8,
        .handlePtr       = &handles.iterationPort2,
        .packetnetCB     = iteration2,
        .userData        = (void*)0
    },
    {
        .name            = "iterationPort3",
        .mustBeConnected = 0,
        .description     = 0,
        .sharedData      = iterationPort3_pnsd,
        .sharedDataBytes = 8,
        .handlePtr       = &handles.iterationPort3,
        .packetnetCB     = iteration3,
        .userData        = (void*)0
    },
    {
        .name            = "iterationPort4",
        .mustBeConnected = 0,
        .description     = 0,
        .sharedData      = iterationPort4_pnsd,
        .sharedDataBytes = 8,
        .handlePtr       = &handles.iterationPort4,
        .packetnetCB     = iteration4,
        .userData        = (void*)0
    },
    {
        .name            = "iterationPort5",
        .mustBeConnected = 0,
        .description     = 0,
        .sharedData      = iterationPort5_pnsd,
        .sharedDataBytes = 8,
        .handlePtr       = &handles.iterationPort5,
        .packetnetCB     = iteration5,
        .userData        = (void*)0
    },
    {
        .name            = "iterationPort6",
        .mustBeConnected = 0,
        .description     = 0,
        .sharedData      = iterationPort6_pnsd,
        .sharedDataBytes = 8,
        .handlePtr       = &handles.iterationPort6,
        .packetnetCB     = iteration6,
        .userData        = (void*)0
    },
    {
        .name            = "iterationPort7",
        .mustBeConnected = 0,
        .description     = 0,
        .sharedData      = iterationPort7_pnsd,
        .sharedDataBytes = 8,
        .handlePtr       = &handles.iterationPort7,
        .packetnetCB     = iteration7,
        .userData        = (void*)0
    },
    {
        .name            = "iterationPort8",
        .mustBeConnected = 0,
        .description     = 0,
        .sharedData      = iterationPort8_pnsd,
        .sharedDataBytes = 8,
        .handlePtr       = &handles.iterationPort8,
        .packetnetCB     = iteration8,
        .userData        = (void*)0
    },
    {
        .name            = "iterationPort9",
        .mustBeConnected = 0,
        .description     = 0,
        .sharedData      = iterationPort9_pnsd,
        .sharedDataBytes = 8,
        .handlePtr       = &handles.iterationPort9,
        .packetnetCB     = iteration9,
        .userData        = (void*)0
    },
    {
        .name            = "iterationPort10",
        .mustBeConnected = 0,
        .description     = 0,
        .sharedData      = iterationPort10_pnsd,
        .sharedDataBytes = 8,
        .handlePtr       = &handles.iterationPort10,
        .packetnetCB     = iteration10,
        .userData        = (void*)0
    },
    {
        .name            = "iterationPort11",
        .mustBeConnected = 0,
        .description     = 0,
        .sharedData      = iterationPort11_pnsd,
        .sharedDataBytes = 8,
        .handlePtr       = &handles.iterationPort11,
        .packetnetCB     = iteration11,
        .userData        = (void*)0
    },
    {
        .name            = "iterationPort12",
        .mustBeConnected = 0,
        .description     = 0,
        .sharedData      = iterationPort12_pnsd,
        .sharedDataBytes = 8,
        .handlePtr       = &handles.iterationPort12,
        .packetnetCB     = iteration12,
        .userData        = (void*)0
    },
    {
        .name            = "iterationPort13",
        .mustBeConnected = 0,
        .description     = 0,
        .sharedData      = iterationPort13_pnsd,
        .sharedDataBytes = 8,
        .handlePtr       = &handles.iterationPort13,
        .packetnetCB     = iteration13,
        .userData        = (void*)0
    },
    {
        .name            = "iterationPort14",
        .mustBeConnected = 0,
        .description     = 0,
        .sharedData      = iterationPort14_pnsd,
        .sharedDataBytes = 8,
        .handlePtr       = &handles.iterationPort14,
        .packetnetCB     = iteration14,
        .userData        = (void*)0
    },
    {
        .name            = "iterationPort15",
        .mustBeConnected = 0,
        .description     = 0,
        .sharedData      = iterationPort15_pnsd,
        .sharedDataBytes = 8,
        .handlePtr       = &handles.iterationPort15,
        .packetnetCB     = iteration15,
        .userData        = (void*)0
    },
    {
        .name            = "iterationPort16",
        .mustBeConnected = 0,
        .description     = 0,
        .sharedData      = iterationPort16_pnsd,
        .sharedDataBytes = 8,
        .handlePtr       = &handles.iterationPort16,
        .packetnetCB     = iteration16,
        .userData        = (void*)0
    },
    {
        .name            = "iterationPort17",
        .mustBeConnected = 0,
        .description     = 0,
        .sharedData      = iterationPort17_pnsd,
        .sharedDataBytes = 8,
        .handlePtr       = &handles.iterationPort17,
        .packetnetCB     = iteration17,
        .userData        = (void*)0
    },
    {
        .name            = "iterationPort18",
        .mustBeConnected = 0,
        .description     = 0,
        .sharedData      = iterationPort18_pnsd,
        .sharedDataBytes = 8,
        .handlePtr       = &handles.iterationPort18,
        .packetnetCB     = iteration18,
        .userData        = (void*)0
    },
    {
        .name            = "iterationPort19",
        .mustBeConnected = 0,
        .description     = 0,
        .sharedData      = iterationPort19_pnsd,
        .sharedDataBytes = 8,
        .handlePtr       = &handles.iterationPort19,
        .packetnetCB     = iteration19,
        .userData        = (void*)0
    },
    {
        .name            = "iterationPort20",
        .mustBeConnected = 0,
        .description     = 0,
        .sharedData      = iterationPort20_pnsd,
        .sharedDataBytes = 8,
        .handlePtr       = &handles.iterationPort20,
        .packetnetCB     = iteration20,
        .userData        = (void*)0
    },
    {
        .name            = "iterationPort21",
        .mustBeConnected = 0,
        .description     = 0,
        .sharedData      = iterationPort21_pnsd,
        .sharedDataBytes = 8,
        .handlePtr       = &handles.iterationPort21,
        .packetnetCB     = iteration21,
        .userData        = (void*)0
    },
    {
        .name            = "iterationPort22",
        .mustBeConnected = 0,
        .description     = 0,
        .sharedData      = iterationPort22_pnsd,
        .sharedDataBytes = 8,
        .handlePtr       = &handles.iterationPort22,
        .packetnetCB     = iteration22,
        .userData        = (void*)0
    },
    {
        .name            = "iterationPort23",
        .mustBeConnected = 0,
        .description     = 0,
        .sharedData      = iterationPort23_pnsd,
        .sharedDataBytes = 8,
        .handlePtr       = &handles.iterationPort23,
        .packetnetCB     = iteration23,
        .userData        = (void*)0
    },
    {
        .name            = "iterationPort24",
        .mustBeConnected = 0,
        .description     = 0,
        .sharedData      = iterationPort24_pnsd,
        .sharedDataBytes = 8,
        .handlePtr       = &handles.iterationPort24,
        .packetnetCB     = iteration24,
        .userData        = (void*)0
    },
    { 0 }
};

static PPM_PACKETNET_PORT_FN(nextPacketnetPort) {
    if(!port) {
         port = packetnetPorts;
    } else {
        port++;
    }
    return port->name ? port : 0;
}

ppmModelAttr modelAttrs = {

    .versionString    = PPM_VERSION_STRING,
    .type             = PPM_MT_PERIPHERAL,

    .packetnetPortsCB = nextPacketnetPort,

    .saveCB        = peripheralSaveState,
    .restoreCB     = peripheralRestoreState,

    .docCB         = installDocs,

    .vlnv          = {
        .vendor  = "gaph",
        .library = "peripheral",
        .name    = "iterator",
        .version = "1.0"
    },

    .family               = "gaph",
    .noRecursiveCallbacks = 1,

    .releaseStatus = PPM_UNSET,
    .visibility     = PPM_VISIBLE,
    .saveRestore    = 0,

};
