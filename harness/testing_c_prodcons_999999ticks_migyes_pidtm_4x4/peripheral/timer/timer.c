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

#include "timer.igen.h"
#include <string.h>


/////////////////////////////// Global Variable ////////////////////////////////

volatile double timer_us = (double)0.0; // interruption timer in us ~~ default is off (zero)!

//////////////////////////////// Callback stubs ////////////////////////////////

PPM_REG_READ_CB(cfgRead) {
    // YOUR CODE HERE (cfgRead)
    return *(Uns32*)user;
}

PPM_REG_WRITE_CB(cfgWrite) {
    unsigned int value = (double)data;
    if(value == 0xFFFFFFFF){
        //bhmMessage("I", "TIMER", "Timer turning interruption down!");
        ppmWriteNet(handles.INT_TIMER, 0);
    }
    else{
        timer_us = (double)value;
        bhmMessage("I", "TIMER", "Timer set to interrupt the processor once every %.2lf us!",timer_us);
    }
    *(Uns32*)user = data;
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
