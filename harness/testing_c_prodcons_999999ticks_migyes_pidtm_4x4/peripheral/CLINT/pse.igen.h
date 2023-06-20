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

#ifndef PSE_IGEN_H
#define PSE_IGEN_H                      

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

typedef struct port0_msip_dataS { 
    union { 
        Uns32 value;
    } msip0;
} port0_msip_dataT, *port0_msip_dataTP;

typedef struct port0_mtime_dataS { 
    union { 
        Uns32 value;
    } mtimecmpLo0;
    union { 
        Uns32 value;
    } mtimecmpHi0;
    union { 
        Uns32 value;
    } mtimeLo;
    union { 
        Uns32 value;
    } mtimeHi;
} port0_mtime_dataT, *port0_mtime_dataTP;

#ifdef _PSE_
/////////////////////////////// Port Declarations //////////////////////////////

extern port0_msip_dataT port0_msip_data;

extern port0_mtime_dataT port0_mtime_data;

///////////////////////////////// Port handles /////////////////////////////////

typedef struct handlesS {
    void                 *port0;
    ppmNetHandle          MTimerInterrupt0;
    ppmNetHandle          MSWInterrupt0;
} handlesT, *handlesTP;

extern handlesT handles;

////////////////////////////// Callback prototypes /////////////////////////////

PPM_REG_READ_CB(msipRead);
PPM_REG_VIEW_CB(msipView);
PPM_REG_WRITE_CB(msipWrite);
PPM_REG_READ_CB(mtimeRead);
PPM_REG_VIEW_CB(mtimeViewHi);
PPM_REG_VIEW_CB(mtimeViewLo);
PPM_REG_READ_CB(mtimecmpRead);
PPM_REG_VIEW_CB(mtimecmpViewHi);
PPM_REG_VIEW_CB(mtimecmpViewLo);
PPM_REG_WRITE_CB(mtimecmpWrite);
PPM_READ_CB(reservedRD);
PPM_WRITE_CB(reservedWR);
PPM_CONSTRUCTOR_CB(periphConstructor);
PPM_DOC_FN(installDocs);
PPM_CONSTRUCTOR_CB(constructor);
PPM_SAVE_STATE_FN(peripheralSaveState);
PPM_RESTORE_STATE_FN(peripheralRestoreState);
PPM_NET_PORT_FN(nextNetPortUser);


///////////////////////////////// Local memory /////////////////////////////////

// To access local memory port0/msip/msipMem : (for example)
// char value = ((char*)handles.port0)[PORT0_MSIP_MSIPMEM_OFFSET + myAddress];

#define PORT0_MSIP_MSIPMEM_OFFSET       0x0
#define PORT0_MSIP_MSIPMEM_SIZE         0x4000

// To access local memory port0/mtime/mtimeMem : (for example)
// char value = ((char*)handles.port0)[PORT0_MTIME_MTIMEMEM_OFFSET + myAddress];

#define PORT0_MTIME_MTIMEMEM_OFFSET     0x4000
#define PORT0_MTIME_MTIMEMEM_SIZE       0x8000

////////////////////////////////// Mask macros /////////////////////////////////

#define PORT0_MSIP_MSIP0_WRMASK         0x1
#define PORT0_MSIP_MSIP0_WRNMASK        (0xfffffffe)
#define PORT0_MSIP_MSIP0_WIDTH          32
#define PORT0_MTIME_MTIMECMPLO0_WIDTH   32
#define PORT0_MTIME_MTIMECMPHI0_WIDTH   32
#define PORT0_MTIME_MTIMELO_WIDTH       32
#define PORT0_MTIME_MTIMEHI_WIDTH       32

#endif

#endif
