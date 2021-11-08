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
//                           Imperas/OVP Generated
//
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pse.igen.h"

#define PORT0 handles.port0
#include "pse.macros.igen.h"

#define PREFIX "CLINT"

struct {
    Uns32  harts;              // Number of harts supported
    double clockMHz;           // Counter frequency from parameter
    double mtime;              // Most recently read value for mtime register
} config;

typedef struct timerDataS {
    bhmThreadHandle threadHandle;             // This timer's thread handle
    Uns64           startCount;               // Starting counter value
    Uns64           stopCount;                // Ending counter value
    double          threadDelay;              // Duration of timer in micro-seconds
    double          threadStarted;            // bhm time when timer was started.
} timerData, *timerDataP;

// Pointers to dynamically allocated arrays - 1 entry per hart
timerDataP    timer;
ppmNetHandle *MTimerInterrupt, *MSWInterrupt;
Uns8         *MSIP;
Uns64        *mtimecmp;

static void initPort(ppmNetPortP port, char *name) {
    port->name            = strdup(name);
    port->type            = PPM_OUTPUT_PORT;
    port->mustBeConnected = 0;
    port->description     = 0;
}

//
// Get dynamic net port table - create on first call
//
static ppmNetPortP getNetPortsUser() {

    static ppmNetPort *netPortsUser = NULL;

    if (!netPortsUser) {

        Uns32 numHarts;

        if (bhmUns32ParamValue("num_harts", &numHarts) && (numHarts > 1)) {

            ppmNetPort *p;
            char        name[32];
            Uns32       hart;
            Uns32       numPortsUser = (2 * (numHarts-1));

            // Allocate ppmNetPort array with 2 entries for each of
            // harts 1..n plus a terminating entry.
            // Note: hart 0's ports are created by iGen code
            netPortsUser = calloc(numPortsUser+1, sizeof(ppmNetPort));

            for (p=netPortsUser, hart = 1; hart < numHarts; hart++) {
                snprintf(name, sizeof(name), "MTimerInterrupt%u", hart);
                initPort(p++, name);
                snprintf(name, sizeof(name), "MSWInterrupt%u", hart);
                initPort(p++, name);
            }

            // Set name to NULL on last entry to indicate end of list
            p->name = NULL;

        }
    }

    return netPortsUser;

}

PPM_NET_PORT_FN(nextNetPortUser) {
    if(!netPort) {
        netPort = getNetPortsUser();
    } else {
        netPort++;
    }
    return (netPort && netPort->name) ? netPort : 0;
}

#define READ_ACCESS True
#define WRITE_ACCESS False

static Bool checkAlignedAccess(
    Uns32 bytes,
    Uns32 offset,
    char *name,
    Bool  isRead,
    Bool  okay
) {
    if (!bytes || ((bytes&(bytes-1))!=0) || ((offset&(bytes-1)))!=0) {
        bhmMessage(
            "W", PREFIX,
            "Unaligned %u byte access at offset %u on %s %s",
            bytes, offset, name, isRead ? "read" : "write"
        );
        okay = False;
    }
    if (!okay) {
        if (isRead) {
            ppmReadAbort();
        } else {
            ppmWriteAbort();
        }
    }
    return okay;
}

static Bool checkAlignedDWordAccess(Uns32 bytes, Uns32 offset, char *name, Bool isRead) {
    Bool okay = True;

    if (bytes != 4 && bytes != 8) {
        bhmMessage(
            "W", PREFIX,
            "Unsupported size %u on %s %s",
            bytes, name, isRead ? "read" : "write"
        );
        okay = False;
    }

    okay = checkAlignedAccess(bytes, offset, name, isRead, okay);

    return okay;
}

static Bool checkAlignedWordAccess(Uns32 bytes, Uns32 offset, char *name, Bool isRead) {
    Bool okay = True;

    if (bytes != 4) {
        bhmMessage(
            "W", PREFIX,
            "Unsupported size %u on %s %s",
            bytes, name, isRead ? "read" : "write"
        );
        okay = False;
    }

    okay = checkAlignedAccess(bytes, offset, name, isRead, okay);

    return okay;
}

//
// timerThread - wait for time indicated by timer and set timerExpired
//
static void timerThread(void *user) {
    Uns32 hartid = PTR_TO_UNS32(user);

    timerDataP timerP     = &timer[hartid];

    timerP->threadStarted = bhmGetLocalTime();
    timerP->startCount    = timerP->threadStarted * config.clockMHz;
    timerP->stopCount     = mtimecmp[hartid];

    // Check to see if a delay is required or we should generate interrupt immediately
    // Timer Registers: A timer interrupt is pending whenever mtime >= mtimecmp
    if (timerP->stopCount > timerP->startCount) {
        timerP->threadDelay = (double)((timerP->stopCount - timerP->startCount)) / config.clockMHz;
        double nextQ   = bhmGetCurrentTime();           // time of next quantum start (minimum delay possible)
        bhmMessage("I", "CLINT", "bhmGetCurrentTime() = %f", nextQ);
        double toNextQ = nextQ - timerP->threadStarted; // time to next quantum start
        if (toNextQ > timerP->threadDelay) {            // if cannot generate required delay, warn and generate minimal
            if (PSE_DIAG_LOW) {
                bhmMessage("W", PREFIX, "timer[%u] delay %0.2f before time to next Quantum %0.2f", hartid, timerP->threadDelay, toNextQ);
            }
            timerP->threadDelay = 1;
        }
    } else {
        timerP->threadDelay = 0;
    }

    if (PSE_DIAG_LOW) {
        bhmMessage("I", PREFIX, "timer[%u]", hartid);
        bhmMessage("I", PREFIX, "  startCount = %lld", timerP->startCount);
        bhmMessage("I", PREFIX, "  stopCount  = %lld", timerP->stopCount);
        bhmMessage("I", PREFIX, "  threadStart = %0.2f", timerP->threadStarted);
        bhmMessage("I", PREFIX, "  threadDelay = %0.2f", timerP->threadDelay);
    }

    // Wait for timer to expire
    if(timerP->threadDelay) {
        //bhmMessage("I", "CLINT", "threadDelay = %f", timerP->threadDelay);
        bhmWaitDelay(timerP->threadDelay);
    }

    if (PSE_DIAG_LOW) {
        double now   = bhmGetCurrentTime();
        bhmMessage(
            "I", PREFIX,
            "timer[%u] Expired now=%0.2f", hartid, now
        );
    }
    bhmMessage("I", "CLINT", "Interrupt = 1");
    ppmWriteNet(MTimerInterrupt[hartid], 1);
}

//
// Stop the thread if active.
//
static void stopThread(Uns32 hartid) {
    if (timer[hartid].threadHandle) {

        if (PSE_DIAG_LOW)
            bhmMessage("I", PREFIX, "Delete timer[%u]", hartid);

        bhmDeleteThread(timer[hartid].threadHandle);
        timer[hartid].threadHandle = 0;
    }
}

//
// start a new one.
//
static void startThread(Uns32 hartid) {
    // Make sure any existing thread is stopped
    stopThread(hartid);

    if (PSE_DIAG_LOW)
        bhmMessage("I", PREFIX, "Start timer[%u]", hartid);

    char name[32];
    sprintf(name, "Timer%u", hartid);
    timer[hartid].threadHandle = bhmCreateThread(
        timerThread,
        UNS32_TO_PTR(hartid),
        name,
        0
    );
}

static Uns32 checkHartid(Uns32 hartid) {
    if (hartid >= config.harts) {
        bhmMessage("F", PREFIX, "Internal error: Invalid hartid %u", hartid);
        // Not reached
    }
    return hartid;
}

//////////////////////////////// Callbacks ////////////////////////////////

PPM_REG_READ_CB(msipRead) {
    Uns32 hartid = checkHartid(PTR_TO_UNS32(user));
    Uns32 offset = addr - handles.port0;

    if (!checkAlignedWordAccess(bytes, offset, "msip", READ_ACCESS)) return 0;

    if (PSE_DIAG_LOW) {
        bhmMessage("I", PREFIX, "msipRead[%u]=0x%x", hartid, MSIP[hartid]);
    }

    return MSIP[hartid];
}

PPM_REG_WRITE_CB(msipWrite) {
    Uns32 hartid = checkHartid(PTR_TO_UNS32(user));
    Uns8  value  = (data & 0x1);
    Uns32 offset = addr - handles.port0;

    if (!checkAlignedWordAccess(bytes, offset, "msip", WRITE_ACCESS)) return;

    MSIP[hartid] = value;

    if (PSE_DIAG_LOW) {
        bhmMessage("I", PREFIX, "msipWrite[%u]=0x%x", hartid, MSIP[hartid]);
    }

    ppmWriteNet(MSWInterrupt[hartid], MSIP[hartid] & 1);
}

PPM_REG_VIEW_CB(msipView) {
    Uns32 hartid = checkHartid(PTR_TO_UNS32(user));
    *(Uns32 *)data = MSIP[hartid];
}


PPM_REG_READ_CB(mtimecmpRead) {
    Uns32 hartid = checkHartid(PTR_TO_UNS32(user));
    Uns32 ret    = 0;
    Uns32 offset = addr - handles.port0;
    Bool  loWord = ((offset & 0x4) == 0);

    if (!checkAlignedDWordAccess(bytes, offset, "mtimecmp", READ_ACCESS)) return 0;

    if (loWord) {
        // return low order 32 bits of mtimecmp value
        ret = (Uns32) mtimecmp[hartid];
        if (PSE_DIAG_LOW) {
            bhmMessage("I", PREFIX, "mtimecmpRead[%u] LO/32 (0x%08x)", hartid, ret);
        }
    } else {
        // return high order 32 bits of mtimecmp value
        ret = (Uns32) (mtimecmp[hartid] >> 32);
        if (PSE_DIAG_LOW) {
            bhmMessage("I", PREFIX, "mtimecmpRead[%u] HI/32 (0x%08x)", hartid, ret);
        }
    }

    return ret;
}

PPM_REG_WRITE_CB(mtimecmpWrite) {
    Uns32 hartid   = checkHartid(PTR_TO_UNS32(user));
    Uns32 offset   = addr - handles.port0;
    Bool  loWord   = ((offset & 0x4) == 0);
    Uns64 value    = mtimecmp[hartid];
    Uns64 oldValue = value;

    if (!checkAlignedDWordAccess(bytes, offset, "mtimecmp", WRITE_ACCESS)) return;

    if (loWord) {
        value &= 0xFFFFFFFF00000000ULL;
        value |= data;
        if (PSE_DIAG_LOW) {
            bhmMessage("I", PREFIX, "mtimecmpWrite[%u] LO/32 mtimecmp=0x%08x_%08x",
                    hartid,
                    (Uns32)(value >> 32),
                    (Uns32)(value & 0xffffffff));

        }
    } else {
        value &= 0x00000000FFFFFFFFULL;
        value |= ((Uns64)data << 32);
        if (PSE_DIAG_LOW) {
            bhmMessage("I", PREFIX, "mtimecmpWrite[%u] HI/32 mtimecmp=0x%08x_%08x",
                    hartid,
                    (Uns32)(value >> 32),
                    (Uns32)(value & 0xffffffff));
        }
    }

    mtimecmp[hartid] = value;

    // clear interrupt
    bhmMessage("I", "CLINT", "Interrupt = 0");
    ppmWriteNet(MTimerInterrupt[hartid], 0);

    if (value != oldValue) {
        // start timer thread when value changes
        startThread(hartid);
    }
}

PPM_REG_VIEW_CB(mtimecmpViewLo) {
    Uns32 hartid = checkHartid(PTR_TO_UNS32(user));
    *(Uns32 *)data = (Uns32) mtimecmp[hartid];
}

PPM_REG_VIEW_CB(mtimecmpViewHi) {
    Uns32 hartid = checkHartid(PTR_TO_UNS32(user));
    *(Uns32 *)data = (Uns32) (mtimecmp[hartid] >> 32);
}

PPM_REG_READ_CB(mtimeRead) {
    Uns32 ret    = 0;
    Uns32 offset = addr - handles.port0;
    Bool  loWord = ((offset & 0x4) == 0);
    Uns64 tval64;

    if (!checkAlignedDWordAccess(bytes, offset, "mtime", READ_ACCESS)) return 0;

    if (loWord) {
        // Only update on loWord read to use same time on
        // consecutive Hi and Lo word reads
        config.mtime = bhmGetLocalTime() * config.clockMHz;
    }
    tval64 = (Uns64) config.mtime;

    if (loWord) {
        ret = (Uns32) tval64;
        if (PSE_DIAG_LOW)
            bhmMessage("I", PREFIX, "mtimeRead Low  = %u", ret);
    } else {
        ret = (Uns32) (tval64 >> 32);
        if (PSE_DIAG_LOW)
            bhmMessage("I", PREFIX, "mtimeRead High = %u", ret);
    }

    return ret;
}

PPM_REG_VIEW_CB(mtimeViewLo) {
    // Only update on loWord read to use same time on
    // consecutive Hi and Lo word reads
    config.mtime = bhmGetLocalTime() * config.clockMHz;
    *(Uns32 *)data = (Uns32)((Uns64)config.mtime);
}

PPM_REG_VIEW_CB(mtimeViewHi) {
    *(Uns32 *)data = (Uns32)(((Uns64)config.mtime) >> 32);
}

//
// Trap Reserved Memory RD/Write
//
PPM_READ_CB(reservedRD) {
    Uns32 addressBlockOffset = PTR_TO_UNS32(user);
    Uns32 offset = (addr - handles.port0) + addressBlockOffset;

    (void) checkAlignedWordAccess(bytes, offset, "reserved", READ_ACCESS);

    if (PSE_DIAG_HIGH) {
        bhmMessage(
            "W", PREFIX,
            "Read from unallocated region at offset 0x%08x",
            offset
        );
    }
    return 0;
}

PPM_WRITE_CB(reservedWR) {
    Uns32 addressBlockOffset = PTR_TO_UNS32(user);
    Uns32 offset = (addr - handles.port0) + addressBlockOffset;

    (void) checkAlignedWordAccess(bytes, offset, "reserved", WRITE_ACCESS);

    if (PSE_DIAG_HIGH) {
        bhmMessage(
            "W", PREFIX,
        "Write to unallocated region at offset 0x%08x",
            offset
        );
    }
}

static void openNetPorts() {
    MTimerInterrupt = (ppmNetHandle *) calloc(config.harts, sizeof(ppmNetHandle));
    MSWInterrupt    = (ppmNetHandle *) calloc(config.harts, sizeof(ppmNetHandle));

    Uns32 iter;
    char name[32];
    for (iter=0; iter<config.harts; iter++) {
        snprintf(name, sizeof(name), "MTimerInterrupt%u", iter);
        MTimerInterrupt[iter] = ppmOpenNetPort(name);
        snprintf(name, sizeof(name), "MSWInterrupt%u", iter);
        MSWInterrupt[iter] = ppmOpenNetPort(name);
    }
}

static void modelInit () {

    char name[32];
    char desc[128];

    bhmUns32ParamValue ("num_harts", &config.harts);
    bhmDoubleParamValue("clockMHz",  &config.clockMHz);

    MSIP     = (Uns8 *)     (Uns8  *) calloc(config.harts, sizeof(Uns8));
    mtimecmp = (Uns64 *)    (Uns64 *) calloc(config.harts, sizeof(Uns64));
    timer    = (timerDataP) calloc(config.harts, sizeof(timerData));

    openNetPorts();

    // Create registers for harts 1..num_harts
    // Note: registers for hart0 were created by iGen
    int id;
    for (id=1; id<config.harts; id++) {
        Uns32 offset;

        offset = 4 * id;    // 4-byte aligned
        snprintf(name, sizeof(name), "msip_msip%u", id);
        snprintf(desc, sizeof(desc), "Hart %u Machine-Mode Software Interrupt", id);
        ppmCreateRegister(name, desc, PORT0_MSIP_MSIP0, offset, 4, msipRead, msipWrite, msipView, UNS32_TO_PTR(id), True);

        offset = (8 * id); // 8-byte aligned
        snprintf(desc, sizeof(desc), "Hart %u Machine-Mode Timer Compare Low", id);
        snprintf(name, sizeof(name), "mtime_mtimecmpLo%u", id);
        ppmCreateRegister(name, desc, PORT0_MTIME_MTIMECMPLO0, offset + 0, 4, mtimecmpRead, mtimecmpWrite, mtimecmpViewLo, UNS32_TO_PTR(id), True);
        snprintf(desc, sizeof(desc), "Hart %u Machine-Mode Timer Compare High", id);
        snprintf(name, sizeof(name), "mtime_mtimecmpHi%u", id);
        ppmCreateRegister(name, desc, PORT0_MTIME_MTIMECMPLO0, offset + 4, 4, mtimecmpRead, mtimecmpWrite, mtimecmpViewHi, UNS32_TO_PTR(id), True);
    }
}

PPM_CONSTRUCTOR_CB(constructor) {
    periphConstructor();
    modelInit();
}


PPM_SAVE_STATE_FN(peripheralSaveState) {
    bhmMessage("E", "PPM_RSNI", "Model does not implement save/restore");
}

PPM_RESTORE_STATE_FN(peripheralRestoreState) {
    bhmMessage("E", "PPM_RSNI", "Model does not implement save/restore");
}
