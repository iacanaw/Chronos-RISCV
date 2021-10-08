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

#ifndef NETWORKINTERFACE_IGEN_H
#define NETWORKINTERFACE_IGEN_H         

#ifdef _PSE_
#    include "peripheral/impTypes.h"
#    include "peripheral/bhm.h"
#    include "peripheral/bhmHttp.h"
#    include "peripheral/ppm.h"
#else
#    include "hostapi/impTypes.h"
#endif

#ifdef _PSE_
//////////////////////////////////// Externs ///////////////////////////////////

extern Uns32 diagnosticLevel;


/////////////////////////// Dynamic Diagnostic Macros //////////////////////////

// Bottom two bits of word used for PSE diagnostics
#define PSE_DIAG_LOW                    (BHM_DIAG_MASK_LOW(diagnosticLevel))
#define PSE_DIAG_MEDIUM                 (BHM_DIAG_MASK_MEDIUM(diagnosticLevel))

#define PSE_DIAG_HIGH                   (BHM_DIAG_MASK_HIGH(diagnosticLevel))
// Next two bits of word used for PSE semihost/intercept library diagnostics
#define PSE_DIAG_SEMIHOST               (BHM_DIAG_MASK_SEMIHOST(diagnosticLevel))

#endif
/////////////////////////// Register data declaration //////////////////////////

typedef struct DMAC_ab8_dataS { 
    union { 
        Uns32 value;
    } address;
    union { 
        Uns32 value;
    } statusTX;
    union { 
        Uns32 value;
    } statusRX;
} DMAC_ab8_dataT, *DMAC_ab8_dataTP;

#ifdef _PSE_
/////////////////////////////// Port Declarations //////////////////////////////

extern DMAC_ab8_dataT DMAC_ab8_data;

///////////////////////////////// Port handles /////////////////////////////////

typedef struct handlesS {
    ppmAddressSpaceHandle MREAD;
    ppmAddressSpaceHandle MWRITE;
    void                 *DMAC;
    ppmNetHandle          INT_NI_TX;
    ppmNetHandle          INT_NI_RX;
    ppmPacketnetHandle    dataPort;
    ppmPacketnetHandle    controlPort;
    ppmPacketnetHandle    txInterruption;
    ppmPacketnetHandle    rxInterruption;
} handlesT, *handlesTP;

extern handlesT handles;

////////////////////////////// Callback prototypes /////////////////////////////

PPM_REG_READ_CB(addressRead);
PPM_REG_WRITE_CB(addressWrite);
PPM_PACKETNET_CB(controlPortUpd);
PPM_PACKETNET_CB(dataPortUpd);
PPM_PACKETNET_CB(rxInterruptionPort);
PPM_REG_READ_CB(statusRXRead);
PPM_REG_WRITE_CB(statusRXWrite);
PPM_REG_READ_CB(statusTXRead);
PPM_REG_WRITE_CB(statusTXWrite);
PPM_PACKETNET_CB(txInterruptionPort);
PPM_CONSTRUCTOR_CB(periphConstructor);
PPM_DESTRUCTOR_CB(periphDestructor);
PPM_DOC_FN(installDocs);
PPM_CONSTRUCTOR_CB(constructor);
PPM_DESTRUCTOR_CB(destructor);
PPM_SAVE_STATE_FN(peripheralSaveState);
PPM_RESTORE_STATE_FN(peripheralRestoreState);


////////////////////////////////// Mask macros /////////////////////////////////

#define DMAC_AB8_ADDRESS_WIDTH          32
#define DMAC_AB8_STATUSTX_WIDTH         32
#define DMAC_AB8_STATUSRX_WIDTH         32

#endif

#endif
