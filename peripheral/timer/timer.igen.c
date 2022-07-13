
////////////////////////////////////////////////////////////////////////////////
//
//                W R I T T E N   B Y   I M P E R A S   I G E N
//
//                             Version 20210408.0
//
////////////////////////////////////////////////////////////////////////////////


#include "timer.igen.h"

extern volatile double timer_us;

/////////////////////////////// Port Declarations //////////////////////////////

TIMEREG_ab8_dataT TIMEREG_ab8_data;

handlesT handles;

/////////////////////////////// Diagnostic level ///////////////////////////////

// Test this variable to determine what diagnostics to output.
// eg. if (diagnosticLevel >= 1) bhmMessage("I", "networkInterface", "Example");
//     Predefined macros PSE_DIAG_LOW, PSE_DIAG_MEDIUM and PSE_DIAG_HIGH may be used
Uns32 diagnosticLevel;

/////////////////////////// Diagnostic level callback //////////////////////////

static void setDiagLevel(Uns32 new) {
    diagnosticLevel = new;
}

///////////////////////////// MMR Generic callbacks ////////////////////////////

static PPM_VIEW_CB(view32) {  *(Uns32*)data = *(Uns32*)user; }

//////////////////////////////// Bus Slave Ports ///////////////////////////////

static void installSlavePorts(void) {
    handles.TIMEREG = ppmCreateSlaveBusPort("TIMEREG", 4);
    if (!handles.TIMEREG) {
        bhmMessage("E", "PPM_SPNC", "Could not connect port 'TIMEREG'");
    }

}

//////////////////////////// Memory mapped registers ///////////////////////////

static void installRegisters(void) {

    {
        ppmCreateRegister(
            "ab8_timerCfg",
            0,
            handles.TIMEREG,
            0x0,
            4,
            cfgRead,
            cfgWrite,
            view32,
            &(TIMEREG_ab8_data.timerCfg.value),
            True
        );
    }

}

PPM_DOC_FN(installDocs){

    ppmDocNodeP Root1_node = ppmDocAddSection(0, "Root");
    {
        ppmDocNodeP doc2_node = ppmDocAddSection(Root1_node, "Description");
        ppmDocAddText(doc2_node, "A OVP DMA for a router");
    }
}
////////////////////////////////// Constructor /////////////////////////////////

PPM_CONSTRUCTOR_CB(periphConstructor) {
    installSlavePorts();
    installRegisters();
}


///////////////////////////////////// Main /////////////////////////////////////

int main(int argc, char *argv[]) {

    diagnosticLevel = 0;
    bhmInstallDiagCB(setDiagLevel);
    constructor();

    while(1){
        if(timer_us == (double)0.0){
            bhmWaitDelay(1000); // if the timer is unset then waits for 10 us to check if the timer was reprogrammed
        }
        else{
            //bhmMessage("I", "TIMER", "Timer set to turn interruption on in %lf us - current time: %lf",timer_us,bhmGetCurrentTime());
            bhmWaitDelay(timer_us); // Every time_us 
            //bhmMessage("I", "TIMER", "Timer turning interruption up! at %lf",bhmGetCurrentTime());
            ppmWriteNet(handles.INT_TIMER, 1);
        }
    }

    bhmWaitEvent(bhmGetSystemEvent(BHM_SE_END_OF_SIMULATION));
    destructor();
    return 0;
}

