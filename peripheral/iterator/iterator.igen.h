
////////////////////////////////////////////////////////////////////////////////
//
//                W R I T T E N   B Y   I M P E R A S   I G E N
//
//                             Version 20210408.0
//
////////////////////////////////////////////////////////////////////////////////

#ifndef ITERATOR_IGEN_H
#define ITERATOR_IGEN_H                 

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

#ifdef _PSE_
/////////////////////////////// Port Declarations //////////////////////////////

///////////////////////////////// Port handles /////////////////////////////////

typedef struct handlesS {
    ppmPacketnetHandle    iterationPort0;
    ppmPacketnetHandle    iterationPort1;
    ppmPacketnetHandle    iterationPort2;
    ppmPacketnetHandle    iterationPort3;
    ppmPacketnetHandle    iterationPort4;
    ppmPacketnetHandle    iterationPort5;
    ppmPacketnetHandle    iterationPort6;
    ppmPacketnetHandle    iterationPort7;
    ppmPacketnetHandle    iterationPort8;
    ppmPacketnetHandle    iterationPort9;
    ppmPacketnetHandle    iterationPort10;
    ppmPacketnetHandle    iterationPort11;
    ppmPacketnetHandle    iterationPort12;
    ppmPacketnetHandle    iterationPort13;
    ppmPacketnetHandle    iterationPort14;
    ppmPacketnetHandle    iterationPort15;
} handlesT, *handlesTP;

extern handlesT handles;

////////////////////////////// Callback prototypes /////////////////////////////

PPM_PACKETNET_CB(iteration0);
PPM_PACKETNET_CB(iteration1);
PPM_PACKETNET_CB(iteration10);
PPM_PACKETNET_CB(iteration11);
PPM_PACKETNET_CB(iteration12);
PPM_PACKETNET_CB(iteration13);
PPM_PACKETNET_CB(iteration14);
PPM_PACKETNET_CB(iteration15);
PPM_PACKETNET_CB(iteration2);
PPM_PACKETNET_CB(iteration3);
PPM_PACKETNET_CB(iteration4);
PPM_PACKETNET_CB(iteration5);
PPM_PACKETNET_CB(iteration6);
PPM_PACKETNET_CB(iteration7);
PPM_PACKETNET_CB(iteration8);
PPM_PACKETNET_CB(iteration9);
PPM_CONSTRUCTOR_CB(periphConstructor);
PPM_DESTRUCTOR_CB(periphDestructor);
PPM_DOC_FN(installDocs);
PPM_CONSTRUCTOR_CB(constructor);
PPM_DESTRUCTOR_CB(destructor);
PPM_SAVE_STATE_FN(peripheralSaveState);
PPM_RESTORE_STATE_FN(peripheralRestoreState);


////////////////////////////////// Mask macros /////////////////////////////////


#endif

#endif
