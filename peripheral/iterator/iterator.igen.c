////////////////////////////////////////////////////////////////////////////////
//
//                W R I T T E N   B Y   I M P E R A S   I G E N
//
//                             Version 20191106.0
//
////////////////////////////////////////////////////////////////////////////////


#include "iterator.igen.h"
/////////////////////////////// Port Declarations //////////////////////////////

handlesT handles;

// Iteration counter
unsigned long long int iterationN = 0;
unsigned long long int iteration;
unsigned int activity;

////////////////////////////// Functions ///////////////////////////////////////
/* Make a callback to each router */
void runIterations(){  
    unsigned int i=0;
    unsigned int tryAgain = 0;
    if(iterationN > 0x07ffffff){
        iterationN = 0;
    }
    do{
        activity = 0;
        iterationN++;
        //bhmMessage("I", "Iterator", "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
        //bhmMessage("I", "Iterator", "Starting %llu round",iterationN);
        iteration = iterationN;
        ppmPacketnetWrite(handles.iterationPort0, &iteration, sizeof(iteration));
        iteration = iterationN;
        ppmPacketnetWrite(handles.iterationPort1, &iteration, sizeof(iteration));
        iteration = iterationN;
        ppmPacketnetWrite(handles.iterationPort2, &iteration, sizeof(iteration));
        iteration = iterationN;
        ppmPacketnetWrite(handles.iterationPort3, &iteration, sizeof(iteration));
        iteration = iterationN;
        ppmPacketnetWrite(handles.iterationPort4, &iteration, sizeof(iteration));
        iteration = iterationN;
        ppmPacketnetWrite(handles.iterationPort5, &iteration, sizeof(iteration));
        iteration = iterationN;
        ppmPacketnetWrite(handles.iterationPort6, &iteration, sizeof(iteration));
        iteration = iterationN;
        ppmPacketnetWrite(handles.iterationPort7, &iteration, sizeof(iteration));
        iteration = iterationN;
        ppmPacketnetWrite(handles.iterationPort8, &iteration, sizeof(iteration));
        iteration = iterationN;
        ppmPacketnetWrite(handles.iterationPort9, &iteration, sizeof(iteration));
        iteration = iterationN;
        ppmPacketnetWrite(handles.iterationPort10, &iteration, sizeof(iteration));
        iteration = iterationN;
        ppmPacketnetWrite(handles.iterationPort11, &iteration, sizeof(iteration));
        iteration = iterationN;
        ppmPacketnetWrite(handles.iterationPort12, &iteration, sizeof(iteration));
        iteration = iterationN;
        ppmPacketnetWrite(handles.iterationPort13, &iteration, sizeof(iteration));
        iteration = iterationN;
        ppmPacketnetWrite(handles.iterationPort14, &iteration, sizeof(iteration));
        iteration = iterationN;
        ppmPacketnetWrite(handles.iterationPort15, &iteration, sizeof(iteration));
        i++;
        if(tryAgain == 0 && activity == 0){
            activity++; 
            tryAgain++;// segunda chance
        }
        else{
            tryAgain = 0;
        }
    }while(activity != 0);
}

/////////////////////////////// Diagnostic level ///////////////////////////////

// Test this variable to determine what diagnostics to output.
// eg. if (diagnosticLevel >= 1) bhmMessage(I, iterator, Example);
//     Predefined macros PSE_DIAG_LOW, PSE_DIAG_MEDIUM and PSE_DIAG_HIGH may be used
Uns32 diagnosticLevel;

/////////////////////////// Diagnostic level callback //////////////////////////

static void setDiagLevel(Uns32 new) {
    diagnosticLevel = new;
}
////////////////////////////////// Constructor /////////////////////////////////

PPM_CONSTRUCTOR_CB(periphConstructor) {
}


//////////////////////////////// Callback stubs ////////////////////////////////

/* Read callback from the extra PE */
PPM_REG_READ_CB(iterateRead) { 
    iterationN++;
    runIterations();
    return *(Uns32*)user;
}

PPM_REG_WRITE_CB(iterateWrite) {
    // YOUR CODE HERE (iterateWrite)
    *(Uns32*)user = data;
}

PPM_PACKETNET_CB(iteration0) {
    unsigned int act = *(unsigned int *)data;
    if(act > 0){
        activity++;
    }
 }

PPM_PACKETNET_CB(iteration1) {
    unsigned int act = *(unsigned int *)data;
    if(act > 0){
        activity++;
    }
 }

PPM_PACKETNET_CB(iteration2) {
    unsigned int act = *(unsigned int *)data;
    if(act > 0){
        activity++;
    }
 }

PPM_PACKETNET_CB(iteration3) {
    unsigned int act = *(unsigned int *)data;
    if(act > 0){
        activity++;
    }
 }

PPM_PACKETNET_CB(iteration4) {
    unsigned int act = *(unsigned int *)data;
    if(act > 0){
        activity++;
    }
 }

PPM_PACKETNET_CB(iteration5) {
    unsigned int act = *(unsigned int *)data;
    if(act > 0){
        activity++;
    }
 }

PPM_PACKETNET_CB(iteration6) {
    unsigned int act = *(unsigned int *)data;
    if(act > 0){
        activity++;
    }
 }

PPM_PACKETNET_CB(iteration7) {
    unsigned int act = *(unsigned int *)data;
    if(act > 0){
        activity++;
    }
 }

PPM_PACKETNET_CB(iteration8) {
    unsigned int act = *(unsigned int *)data;
    if(act > 0){
        activity++;
    }
 }

PPM_PACKETNET_CB(iteration9) {
    unsigned int act = *(unsigned int *)data;
    if(act > 0){
        activity++;
    }
 }

PPM_PACKETNET_CB(iteration10) {
    unsigned int act = *(unsigned int *)data;
    if(act > 0){
        activity++;
    }
 }

PPM_PACKETNET_CB(iteration11) {
    unsigned int act = *(unsigned int *)data;
    if(act > 0){
        activity++;
    }
 }

PPM_PACKETNET_CB(iteration12) {
    unsigned int act = *(unsigned int *)data;
    if(act > 0){
        activity++;
    }
 }

PPM_PACKETNET_CB(iteration13) {
    unsigned int act = *(unsigned int *)data;
    if(act > 0){
        activity++;
    }
 }

PPM_PACKETNET_CB(iteration14) {
    unsigned int act = *(unsigned int *)data;
    if(act > 0){
        activity++;
    }
 }

PPM_PACKETNET_CB(iteration15) {
    unsigned int act = *(unsigned int *)data;
    if(act > 0){
        activity++;
    }
 }

PPM_CONSTRUCTOR_CB(constructor) {
    // YOUR CODE HERE (pre constructor)
    periphConstructor();
    // YOUR CODE HERE (post constructor)
}

PPM_DESTRUCTOR_CB(destructor) {
    // YOUR CODE HERE (destructor)
}


PPM_SAVE_STATE_FN(peripheralSaveState) {
    bhmMessage("E", "PPM_RSNI", "Model does not implement save/restore");
}

PPM_RESTORE_STATE_FN(peripheralRestoreState) {
    bhmMessage("E", "PPM_RSNI", "Model does not implement save/restore");
}

PPM_DOC_FN(installDocs){
    ppmDocNodeP Root1_node = ppmDocAddSection(0, "Root");
    {
        ppmDocNodeP doc2_node = ppmDocAddSection(Root1_node, "Description");
        ppmDocAddText(doc2_node, "The NoC iterator");
    }
}
///////////////////////////////////// Main /////////////////////////////////////

int main(int argc, char *argv[]) {

    diagnosticLevel = 0;
    bhmInstallDiagCB(setDiagLevel);
    constructor();

    while(1){
        bhmWaitDelay(QUANTUM_DELAY);
        runIterations();
    }


    bhmWaitEvent(bhmGetSystemEvent(BHM_SE_END_OF_SIMULATION));
    destructor();
    return 0;
}
