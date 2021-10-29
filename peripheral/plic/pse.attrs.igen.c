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

#include "pse.igen.h"

static ppmBusPort busPorts[] = {
    {
        .name            = "port0",
        .type            = PPM_SLAVE_PORT,
        .addrHi          = 0x3ffffff,
        .mustBeConnected = 0,
        .remappable      = 0,
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

static ppmNetPort netPorts[] = {
    {
        .name            = "irqS1",
        .type            = PPM_INPUT_PORT,
        .mustBeConnected = 0,
        .description     = 0,
        .handlePtr       = &handles.irqS1,
        .netCB           = irqUpdate,
        .userData        = (void*)1,
    },
    {
        .name            = "irqT0",
        .type            = PPM_OUTPUT_PORT,
        .mustBeConnected = 0,
        .description     = 0,
        .handlePtr       = &handles.irqT0,
    },
    { 0 }
};


#define NUM_NET_PORT_ENTRIES            (sizeof(netPorts) / sizeof(*netPorts))
#define NET_PORT_TERMINATOR             (netPorts + (NUM_NET_PORT_ENTRIES - 1))

static PPM_NET_PORT_FN(nextNetPort) {
    if(!netPort) {
         netPort = netPorts;
    } else {
        netPort++;
        if (netPort == NET_PORT_TERMINATOR) {
            netPort = nextNetPortUser(NULL);
        }
    }
    return (netPort && netPort->name) ? netPort : 0;
}

static ppmParameter parameters[] = {
    {
        .name        = "num_sources",
        .type        = ppm_PT_UNS32,
        .description = "Number of Input Interrupt Sources",
        .u.uns32Param.min          = 1,
        .u.uns32Param.max          = 1023,
        .u.uns32Param.defaultValue = 1,
    },
    {
        .name        = "num_targets",
        .type        = ppm_PT_UNS32,
        .description = "Number of Output Interrupt Targets, Hart/Context",
        .u.uns32Param.min          = 1,
        .u.uns32Param.max          = 15871,
        .u.uns32Param.defaultValue = 1,
    },
    {
        .name        = "num_priorities",
        .type        = ppm_PT_UNS32,
        .description = "Number of Priority levels",
        .u.uns32Param.min          = 1,
        .u.uns32Param.max          = 1024,
        .u.uns32Param.defaultValue = 7,
    },
    {
        .name        = "priority_base",
        .type        = ppm_PT_UNS32,
        .description = "Base Address offset for Priority Registers",
        .u.uns32Param.defaultValue = 0x0,
    },
    {
        .name        = "pending_base",
        .type        = ppm_PT_UNS32,
        .description = "Base Address offset for Pending Registers",
        .u.uns32Param.defaultValue = 0x1000,
    },
    {
        .name        = "enable_base",
        .type        = ppm_PT_UNS32,
        .description = "Base Address offset for Enable Registers",
        .u.uns32Param.defaultValue = 0x2000,
    },
    {
        .name        = "enable_stride",
        .type        = ppm_PT_UNS32,
        .description = "Stride size for Enable Register Block",
        .u.uns32Param.defaultValue = 0x80,
    },
    {
        .name        = "context_base",
        .type        = ppm_PT_UNS32,
        .description = "Base Address offset for Context Registers, Threshold/Claim",
        .u.uns32Param.defaultValue = 0x200000,
    },
    {
        .name        = "context_stride",
        .type        = ppm_PT_UNS32,
        .description = "Stride size for Context Register Block",
        .u.uns32Param.defaultValue = 0x1000,
    },
    { 0 }
};

static PPM_PARAMETER_FN(nextParameter) {
    if(!parameter) {
        parameter = parameters;
    } else {
        parameter++;
    }
    return parameter->name ? parameter : 0;
}

ppmModelAttr modelAttrs = {

    .versionString    = PPM_VERSION_STRING,
    .type             = PPM_MT_PERIPHERAL,

    .busPortsCB       = nextBusPort,  
    .netPortsCB       = nextNetPort,  
    .paramSpecCB      = nextParameter,

    .saveCB        = peripheralSaveState,
    .restoreCB     = peripheralRestoreState,

    .docCB         = installDocs,

    .vlnv          = {
        .vendor  = "riscv.ovpworld.org",
        .library = "peripheral",
        .name    = "PLIC",
        .version = "1.0"
    },

    .family               = "riscv.ovpworld.org",
    .noRecursiveCallbacks = 1,

    .releaseStatus = PPM_OVP,
    .visibility     = PPM_VISIBLE,
    .saveRestore    = 0,

};
