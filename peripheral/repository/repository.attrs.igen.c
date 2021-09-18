
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

#include "repository.igen.h"


//                     Shared data for packetnet interface

Uns8 portData_pnsd[4];
Uns8 portControl_pnsd[8];

static ppmPacketnetPort packetnetPorts[] = {
    {
        .name            = "portData",
        .mustBeConnected = 0,
        .description     = 0,
        .sharedData      = portData_pnsd,
        .sharedDataBytes = 4,
        .handlePtr       = &handles.portData,
        .packetnetCB     = dataUpdate,
        .userData        = (void*)0
    },
    {
        .name            = "portControl",
        .mustBeConnected = 0,
        .description     = 0,
        .sharedData      = portControl_pnsd,
        .sharedDataBytes = 8,
        .handlePtr       = &handles.portControl,
        .packetnetCB     = controlUpdate,
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
        .name    = "repository",
        .version = "1.0"
    },

    .family               = "gaph",
    .noRecursiveCallbacks = 1,

    .releaseStatus = PPM_UNSET,
    .visibility     = PPM_VISIBLE,
    .saveRestore    = 0,

};
