/*
 * Copyright (c) 2005-2021 Imperas Software Ltd., www.imperas.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied.
 *
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */


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

#include "router.igen.h"

static ppmBusPort busPorts[] = {
    {
        .name            = "localPort",
        .type            = PPM_SLAVE_PORT,
        .addrHi          = 0x3,
        .mustBeConnected = 1,
        .remappable      = 0,
        .description     = 0,
    },
    {
        .name            = "RREAD",
        .type            = PPM_MASTER_PORT,
        .addrBits        = 32,
        .addrBitsMin     = 32,
        .addrBitsMax     = 0,
        .mustBeConnected = 0,
        .description     = 0,
    },
    {
        .name            = "RWRITE",
        .type            = PPM_MASTER_PORT,
        .addrBits        = 32,
        .addrBitsMin     = 32,
        .addrBitsMax     = 0,
        .mustBeConnected = 0,
        .description     = 0,
    },
    { 0 }
};

static PPM_BUS_PORT_FN(nextBusPort) {
    if(!busPort) {
        busPort = busPorts;
    } else {
        busPort++;
    }
    return busPort->name ? busPort : 0;
}


//                     Shared data for packetnet interface

Uns8 portDataEast_pnsd[4];
Uns8 portDataWest_pnsd[4];
Uns8 portDataNorth_pnsd[4];
Uns8 portDataSouth_pnsd[4];
Uns8 portDataLocal_pnsd[4];
Uns8 iterationsPort_pnsd[8];
Uns8 portControlEast_pnsd[8];
Uns8 portControlWest_pnsd[8];
Uns8 portControlNorth_pnsd[8];
Uns8 portControlSouth_pnsd[8];
Uns8 portControlLocal_pnsd[8];

static ppmPacketnetPort packetnetPorts[] = {
    {
        .name            = "portDataEast",
        .mustBeConnected = 0,
        .description     = 0,
        .sharedData      = portDataEast_pnsd,
        .sharedDataBytes = 4,
        .handlePtr       = &handles.portDataEast,
        .packetnetCB     = dataEast,
        .userData        = (void*)0
    },
    {
        .name            = "portDataWest",
        .mustBeConnected = 0,
        .description     = 0,
        .sharedData      = portDataWest_pnsd,
        .sharedDataBytes = 4,
        .handlePtr       = &handles.portDataWest,
        .packetnetCB     = dataWest,
        .userData        = (void*)0
    },
    {
        .name            = "portDataNorth",
        .mustBeConnected = 0,
        .description     = 0,
        .sharedData      = portDataNorth_pnsd,
        .sharedDataBytes = 4,
        .handlePtr       = &handles.portDataNorth,
        .packetnetCB     = dataNorth,
        .userData        = (void*)0
    },
    {
        .name            = "portDataSouth",
        .mustBeConnected = 0,
        .description     = 0,
        .sharedData      = portDataSouth_pnsd,
        .sharedDataBytes = 4,
        .handlePtr       = &handles.portDataSouth,
        .packetnetCB     = dataSouth,
        .userData        = (void*)0
    },
    {
        .name            = "portDataLocal",
        .mustBeConnected = 0,
        .description     = 0,
        .sharedData      = portDataLocal_pnsd,
        .sharedDataBytes = 4,
        .handlePtr       = &handles.portDataLocal,
        .packetnetCB     = dataLocal,
        .userData        = (void*)0
    },
    {
        .name            = "iterationsPort",
        .mustBeConnected = 0,
        .description     = 0,
        .sharedData      = iterationsPort_pnsd,
        .sharedDataBytes = 8,
        .handlePtr       = &handles.iterationsPort,
        .packetnetCB     = iterationPort,
        .userData        = (void*)0
    },
    {
        .name            = "portControlEast",
        .mustBeConnected = 0,
        .description     = 0,
        .sharedData      = portControlEast_pnsd,
        .sharedDataBytes = 8,
        .handlePtr       = &handles.portControlEast,
        .packetnetCB     = controlEast,
        .userData        = (void*)0
    },
    {
        .name            = "portControlWest",
        .mustBeConnected = 0,
        .description     = 0,
        .sharedData      = portControlWest_pnsd,
        .sharedDataBytes = 8,
        .handlePtr       = &handles.portControlWest,
        .packetnetCB     = controlWest,
        .userData        = (void*)0
    },
    {
        .name            = "portControlNorth",
        .mustBeConnected = 0,
        .description     = 0,
        .sharedData      = portControlNorth_pnsd,
        .sharedDataBytes = 8,
        .handlePtr       = &handles.portControlNorth,
        .packetnetCB     = controlNorth,
        .userData        = (void*)0
    },
    {
        .name            = "portControlSouth",
        .mustBeConnected = 0,
        .description     = 0,
        .sharedData      = portControlSouth_pnsd,
        .sharedDataBytes = 8,
        .handlePtr       = &handles.portControlSouth,
        .packetnetCB     = controlSouth,
        .userData        = (void*)0
    },
    {
        .name            = "portControlLocal",
        .mustBeConnected = 0,
        .description     = "Interrupt Request",
        .sharedData      = portControlLocal_pnsd,
        .sharedDataBytes = 8,
        .handlePtr       = &handles.portControlLocal,
        .packetnetCB     = controlLocal,
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

    .busPortsCB       = nextBusPort,  
    .packetnetPortsCB = nextPacketnetPort,

    .saveCB        = peripheralSaveState,
    .restoreCB     = peripheralRestoreState,

    .docCB         = installDocs,

    .vlnv          = {
        .vendor  = "gaph",
        .library = "peripheral",
        .name    = "router",
        .version = "1.0"
    },

    .family               = "gaph",
    .noRecursiveCallbacks = 1,

    .releaseStatus = PPM_UNSET,
    .visibility     = PPM_VISIBLE,
    .saveRestore    = 0,

};
