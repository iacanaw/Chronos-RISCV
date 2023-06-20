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


#include "pse.igen.h"
/////////////////////////////// Port Declarations //////////////////////////////

port0_msip_dataT port0_msip_data;

port0_mtime_dataT port0_mtime_data;

handlesT handles;

/////////////////////////////// Diagnostic level ///////////////////////////////

// Test this variable to determine what diagnostics to output.
// eg. if (diagnosticLevel >= 1) bhmMessage("I", "CLINT", "Example");
//     Predefined macros PSE_DIAG_LOW, PSE_DIAG_MEDIUM and PSE_DIAG_HIGH may be used
Uns32 diagnosticLevel;

/////////////////////////// Diagnostic level callback //////////////////////////

static void setDiagLevel(Uns32 new) {
    diagnosticLevel = new;
}

///////////////////////////// MMR Generic callbacks ////////////////////////////

//////////////////////////////// Bus Slave Ports ///////////////////////////////

static void installSlavePorts(void) {
    handles.port0 = ppmCreateSlaveBusPort("port0", 49152);

    ppmInstallReadCallback(reservedRD, (void*)0x0 , handles.port0 + 0x0, 0x4000);
    ppmInstallWriteCallback(reservedWR, (void*)0x0 , handles.port0 + 0x0, 0x4000);
    ppmInstallReadCallback(reservedRD, (void*)0x4000 , handles.port0 + 0x4000, 0x8000);
    ppmInstallWriteCallback(reservedWR, (void*)0x4000 , handles.port0 + 0x4000, 0x8000);
}

//////////////////////////// Memory mapped registers ///////////////////////////

static void installRegisters(void) {

    {
        ppmCreateRegister(
            "msip_msip0",
            "Hart 0 Machine-Mode Software Interrupt",
            handles.port0,
            0x0,
            4,
            msipRead,
            msipWrite,
            msipView,
            (void*)0,
            False
        );
    }
    {
        ppmCreateRegister(
            "mtime_mtimecmpLo0",
            "Hart 0 Machine-Mode Timer Compare Low",
            handles.port0,
            0x4000,
            4,
            mtimecmpRead,
            mtimecmpWrite,
            mtimecmpViewLo,
            (void*)0,
            True
        );
    }
    {
        ppmCreateRegister(
            "mtime_mtimecmpHi0",
            "Hart 0 Machine-Mode Timer Compare High",
            handles.port0,
            0x4004,
            4,
            mtimecmpRead,
            mtimecmpWrite,
            mtimecmpViewHi,
            (void*)0,
            True
        );
    }
    {
        ppmCreateRegister(
            "mtime_mtimeLo",
            "Machine-Mode mtime Low",
            handles.port0,
            0xbff8,
            4,
            mtimeRead,
            0,
            mtimeViewLo,
            &(port0_mtime_data.mtimeLo.value),
            True
        );
    }
    {
        ppmCreateRegister(
            "mtime_mtimeHi",
            "Machine-Mode mtime High",
            handles.port0,
            0xbffc,
            4,
            mtimeRead,
            0,
            mtimeViewHi,
            &(port0_mtime_data.mtimeHi.value),
            True
        );
    }

}

PPM_DOC_FN(installDocs){

    ppmDocNodeP Root1_node = ppmDocAddSection(0, "Root");
    {
        ppmDocNodeP doc2_node = ppmDocAddSection(Root1_node, "Licensing");
        ppmDocAddText(doc2_node, "Open Source Apache 2.0");
        ppmDocNodeP doc_12_node = ppmDocAddSection(Root1_node, "Description");
        ppmDocAddText(doc_12_node, "\nRisc-V Core Local Interruptor (CLINT).\nUse the num_harts parameter to specify the number of harts suported (default 1).\nFor each supported hart there will be an MTimerInterruptN and MSWInterruptN net port, plus msipN and mtimecmpN registers implemented, where N is a value from 0..num_harts-1\nThere is also a single mtime register.\n");
        ppmDocNodeP doc_22_node = ppmDocAddSection(Root1_node, "Limitations");
        ppmDocAddText(doc_22_node, "Writes to mtime register are not supported");
        ppmDocNodeP doc_32_node = ppmDocAddSection(Root1_node, "Reference");
        ppmDocAddText(doc_32_node, "SiFive Freedom U540-C000 Manual FU540-C000-v1.0.pdf (https://www.sifive.com/documentation/chips/freedom-u540-c000-manual)");
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

    bhmWaitEvent(bhmGetSystemEvent(BHM_SE_END_OF_SIMULATION));
    return 0;
}

