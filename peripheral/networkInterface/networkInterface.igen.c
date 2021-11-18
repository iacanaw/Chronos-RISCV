
// ////////////////////////////////////////////////////////////////////////////////
// //
// //                W R I T T E N   B Y   I M P E R A S   I G E N
// //
// //                             Version 20210408.0
// //
// ////////////////////////////////////////////////////////////////////////////////


// #include "networkInterface.igen.h"
// /////////////////////////////// Port Declarations //////////////////////////////

// DMAC_ab8_dataT DMAC_ab8_data;

// handlesT handles;

// /////////////////////////////// Diagnostic level ///////////////////////////////

// // Test this variable to determine what diagnostics to output.
// // eg. if (diagnosticLevel >= 1) bhmMessage("I", "networkInterface", "Example");
// //     Predefined macros PSE_DIAG_LOW, PSE_DIAG_MEDIUM and PSE_DIAG_HIGH may be used
// Uns32 diagnosticLevel;

// /////////////////////////// Diagnostic level callback //////////////////////////

// static void setDiagLevel(Uns32 new) {
//     diagnosticLevel = new;
// }

// ///////////////////////////// MMR Generic callbacks ////////////////////////////

// static PPM_VIEW_CB(view32) {  *(Uns32*)data = *(Uns32*)user; }

// //////////////////////////////// Bus Slave Ports ///////////////////////////////

// static void installSlavePorts(void) {
//     handles.DMAC = ppmCreateSlaveBusPort("DMAC", 16);
//     if (!handles.DMAC) {
//         bhmMessage("E", "PPM_SPNC", "Could not connect port 'DMAC'");
//     }

// }

// //////////////////////////// Memory mapped registers ///////////////////////////

// static void installRegisters(void) {

//     {
//         ppmCreateRegister(
//             "ab8_address",
//             0,
//             handles.DMAC,
//             0x0,
//             4,
//             addressRead,
//             addressWrite,
//             view32,
//             &(DMAC_ab8_data.address.value),
//             True
//         );
//     }
//     {
//         ppmCreateRegister(
//             "ab8_statusTX",
//             0,
//             handles.DMAC,
//             0x4,
//             4,
//             statusTXRead,
//             statusTXWrite,
//             view32,
//             &(DMAC_ab8_data.statusTX.value),
//             True
//         );
//     }
//     {
//         ppmCreateRegister(
//             "ab8_statusRX",
//             0,
//             handles.DMAC,
//             0x8,
//             4,
//             statusRXRead,
//             statusRXWrite,
//             view32,
//             &(DMAC_ab8_data.statusRX.value),
//             True
//         );
//     }
//     {
//         ppmCreateRegister(
//             "ab8_timer",
//             0,
//             handles.DMAC,
//             0xc,
//             4,
//             timerRead,
//             timerWrite,
//             view32,
//             &(DMAC_ab8_data.timer.value),
//             True
//         );
//     }

// }

// /////////////////////////////// Bus Master Ports ///////////////////////////////

// static void installMasterPorts(void) {
//     handles.MREAD = ppmOpenAddressSpace("MREAD");
//     handles.MWRITE = ppmOpenAddressSpace("MWRITE");
// }

// PPM_DOC_FN(installDocs){

//     ppmDocNodeP Root1_node = ppmDocAddSection(0, "Root");
//     {
//         ppmDocNodeP doc2_node = ppmDocAddSection(Root1_node, "Description");
//         ppmDocAddText(doc2_node, "A OVP DMA for a router");
//     }
// }
// ////////////////////////////////// Constructor /////////////////////////////////

// PPM_CONSTRUCTOR_CB(periphConstructor) {
//     installSlavePorts();
//     installRegisters();
//     installMasterPorts();
// }

// ///////////////////////////////////// Main /////////////////////////////////////

// int main(int argc, char *argv[]) {

//     diagnosticLevel = 0;
//     bhmInstallDiagCB(setDiagLevel);
//     constructor();

//     bhmWaitEvent(bhmGetSystemEvent(BHM_SE_END_OF_SIMULATION));
//     destructor();
//     return 0;
// }

