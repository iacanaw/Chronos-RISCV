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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pse.igen.h"

#define PREFIX "PLIC"

#define WORDBYTES  4
#define WORDBITS  32

// Maximum sizes supported in RiscV PLIC architecture specification
#define ARCH_S_MAX 1023
#define ARCH_T_MAX 15872

////////////////////////////////////////////////////////////////////////////////
// CONFIGURATION DEFINES
////////////////////////////////////////////////////////////////////////////////

// Maximum number of targets supported by implementation (valid target numbers are 0..T_MAX-1)
#ifndef T_MAX
#define T_MAX  ARCH_T_MAX
#endif
#if T_MAX > ARCH_T_MAX
#error T_MAX is larger than the architectural maximum
#endif

// Maximum number of interrupt sources supported by implementation (valid source numbers are 1..S_MAX)
#ifndef S_MAX
#define S_MAX  ARCH_S_MAX
#endif
#if S_MAX > ARCH_S_MAX
#error S_MAX is larger than the architectural maximum
#endif

// Number of registers needed for S_MAX+1 bits in packed arrays for each source:
#define S_PACK (((S_MAX | (WORDBITS-1)) + 1) / WORDBITS)

// Maximum number of priorities supported by implementation (valid priority numbers are 1..S_MAX)
#ifndef P_MAX
// No architectural maximum on number of priorities so default to any value
#define P_MAX  -1
#endif

// Are the Pending registers writable?
#ifndef PENDING_WRITABLE
#define PENDING_WRITABLE False
#endif

// Should accesses to Reserved regions be ignored?
#ifndef IGNORE_RESERVED_ACCESS
#define IGNORE_RESERVED_ACCESS True
#endif

// Are interrupt inputs level sensitive (True) or edge triggered (False)
// Note: This setting is not used when ANDES_EXTENSIONS is True
#ifndef LEVEL_SENSITIVE
#define LEVEL_SENSITIVE True
#endif

// Are interrupt inputs active high/positive edge (True) or active low/negative edge (False)
#ifndef ACTIVE_VALUE
#define ACTIVE_VALUE True
#endif

// Are Andes PLIC extensions present?
#ifndef ANDES_EXTENSIONS
#define ANDES_EXTENSIONS False
#endif

////////////////////////////////////////////////////////////////////////////////
// STRUCTURES AND STATIC VARIABLES
////////////////////////////////////////////////////////////////////////////////

// Net handle array (Note irqS[0] corresponds to net irqS1
ppmNetHandle *irqS, *irqT;

/*
 * Architectural Memory Map
 * ------------------------
 * 0x0000_0000 - 0x0000_0003    - *** Reserved/Andes Feature Register ***
 * 0x0000_0004 - 0x0000_0FFF    - Priority (num)
 * 0x0000_1000 - 0x0000_107F    - Pending (num)
 * 0x0000_1080 - 0x0000_10FF    - *** Reserved/Andes Trigger Types ***
 * 0x0000_1100 - 0x0000_1FFF    - *** Reserved ***
 * 0x0000_2000 - 0x0000_207F    - Enables for context 0 (num)
 * 0x0000_2080 - 0x0000_21FF    - Enables for context 1 (num)
 * ...
 * 0x001F_1F80 - 0x001F_1FFF    - Enables for context 15871 (num)
 * 0x001F_2000 - 0x001F_1FFF    - *** Reserved ***
 * 0x0020_0000 - 0x0020_0003    - Priority Threshold for context 0 (num)
 * 0x0020_0004 - 0x0020_0007    - Claim/Complete for context 0 (num)
 * 0x0020_0008 - 0x0020_0FFF    - *** Reserved ***
 * 0x0020_1000 - 0x0020_1003    - Priority Threshold for context 1 (num)
 * 0x0020_1004 - 0x0020_1007    - Claim/Complete for context 1 (num)
 * 0x0020_1008 - 0x0020_1FFF    - *** Reserved ***
 * ....
 * 0x03FF_E000 - 0x03FF_E003    - Priority Threshold for context 15871 (num)
 * 0x03FF_E004 - 0x03FF_E007    - Claim/Complete for context 15871 (num)
 * 0x03FF_E008 - 0x03FF_FFFF    - *** Reserved ***
 *
 * Shown for default 4K alignment of Enables and Priority Threshold registers
 * - This is configurable with the enable_stride and context_stride parameters
 */

struct {
    Uns32 sources;
    Uns32 targets;
    Uns32 priorities;
    Uns32 PriorityBase;
    Uns32 PendingBase;
    Uns32 EnablesBase;
    Uns32 EnablesStride;
    Uns32 ContextBase;
    Uns32 ContextStride;
} config;

// T=TARGETS (aka contexts), S=SOURCES

// Input  IRQS[1..S_MAX], 0 is reserved
Bool  IRQS[S_MAX+1]         = { 0 };

// Output IRQT[0..T_MAX-1]
Bool  IRQT[T_MAX]           = { 0 };

// Priority of input IRQS (0=disable, 1-config.priorities, 1 lowest)
Uns32 PRIORITY[S_MAX+1]     = { 0 };

// Pending array - 1 bit per source
Uns32 PENDING[S_PACK]       = { 0 };

// Enable bits - each target has array with 1 bit per source
Uns32 ENABLE[T_MAX][S_PACK] = { { 0 } };

// Priority Threshold - 1 per target
Uns32 THRESHOLD[T_MAX]      = { 0 };

// Claim/Complete - 1 per target
Uns32 CLAIM[T_MAX]          = { 0 };

// Internal flag: source claimed not completed - 1 byte per source
Uns32  GATEWAY[S_PACK]      = { 0 };

// Andes Preempt bits - each target has array with 1 bit per priority level
#define ANDES_PREEMPT_OFFSET 0x400
#define ANDES_PREEMPT_NUM_PRIORITIES 255
#define ANDES_PREEMPT_NUM_REGS (((ANDES_PREEMPT_NUM_PRIORITIES | (WORDBITS-1)) + 1) / WORDBITS)
Uns32 ANDES_PREEMPT[T_MAX][ANDES_PREEMPT_NUM_REGS] = { { 0 } };

// Andes Feature Register
Uns32 andesFeature = 0;
#define ANDES_FEATURE_MASK 0x3
#define FEATURE_PREMPT_MASK 0
#define FEATURE_VECTOR_MASK 1
#define FEATURE_ENABLED(_R, _F) (((_R) & FEATURE_##_F##_MASK) != 0)

// Andes Trigger type array - edge (1) or level (0) triggered
#define ANDES_TRIGGER_BASE 0x1080
Uns32 ANDES_TRIGGER[S_PACK]  = { 0 };      // Andes Trigger type array [1..S_MAX], 0 is reserved

// Andes number of interrupt and target configuration register
union {
    Uns32 value;
    struct {
        Uns32 num_interrupt : 16;
        Uns32 num_target    : 16;
    } bits;
} num_interrupts_targets;
#define ANDES_NUM_INTERRUPTS_TARGETS_ADDR 0x1100

// Andes version and max priority configuration register
union {
    Uns32 value;
    struct {
        Uns32 version      : 16;
        Uns32 max_priority : 16;
    } bits;
} version_max_priority;
#define ANDES_VERSION_MAX_PRIORITY_ADDR 0x1104


////////////////////////////////////////////////////////////////////////////////
// UTILITY FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

#define IS_READ  1
#define IS_WRITE 0

static ppmNetPortP getNetPortsUser() {

    static ppmNetPort *netPortsUser = NULL;

    if (!netPortsUser) {

        Uns32 numSources = 0;
        Uns32 numTargets = 0;

        bhmUns32ParamValue("num_sources", &numSources);
        bhmUns32ParamValue("num_targets", &numTargets);

        // Note: netports for first source and first target are added by iGen
        Int32 numNetPorts = numSources + numTargets - 2;

        if (numNetPorts > 0) {

            // Allocate array with one entry for each netport + a terminating empty entry
            netPortsUser = (ppmNetPortP) calloc(numNetPorts+1, sizeof(ppmNetPort));

            Uns32 id;
            char name[32];
            ppmNetPortP ptr = netPortsUser;

            // Sources numbered from 1 to numSources (1 created by iGen)
            for(id=2; id<=numSources; id++, ptr++) {
                snprintf(name, sizeof(name), "irqS%d", id);
                ptr->name = strdup(name);
                ptr->type = PPM_INPUT_PORT;
                ptr->description = "Input Interrupt Port";
            }

            // Targets numbered from 0 to numTargets-1  (0 created by iGen)
            for(id=1; id<numTargets; id++, ptr++) {
                snprintf(name, sizeof(name), "irqT%d", id);
                ptr->name = strdup(name);
                ptr->type = PPM_OUTPUT_PORT;
                ptr->description = "Output Interrupt Port, to Hart/Context";
            }

            // Terminating entry
            ptr->name = NULL;
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

static Bool active(Bool old, Bool new, Bool levelSensitive) {
    if (levelSensitive) {
        return new == ACTIVE_VALUE;
    } else {
        return (old != ACTIVE_VALUE) && (new == ACTIVE_VALUE);
    }
}

static Bool irqAsserted(Bool old, Bool new, Uns32 id) {
    Bool levelSensitive = LEVEL_SENSITIVE;

    if (ANDES_EXTENSIONS) {
        // On Andes PLIC trigger type for each source comes from registers
        Uns32 trigger_posn = id & 0x1F;
        Uns32 trigger_word = id >> 5;
        Uns32 trigger      = ANDES_TRIGGER[trigger_word];
        levelSensitive = (trigger >> trigger_posn) & 1;
    }

    return active(old, new, levelSensitive);
}

static Bool getPackedBit(Uns32 id, Uns32 *array) {
    Uns32 posn = id & 0x1F;
    Uns32 word = id >> 5;
    return (array[word] & (1 << posn)) != 0;
}

static void setPackedBit(Uns32 id, Uns32 *array, Bool value) {
    Uns32 posn = id & 0x1F;
    Uns32 word = id >> 5;
    if (value) {
        array[word] |= (1 << posn);
    } else {
        array[word] &= ~(1 << posn);
    }
}

static Uns32 highestPackedBit(Uns32 *array, Uns32 maxBit) {
    Uns32 i;
    for (i = maxBit; i >= 0; i--) {
        if (getPackedBit(i, array)) {
            return i;
        }
    }
    return 0;
}

//
// Sanity check the current CLAIM value for target:
// - the pending bit for a non-zero CLAIM source must be set
// - the gateway bit for a non-zero CLAIM source must not be set
//
static void assertClaimIsPending(Uns32 target) {
    Uns32 source = CLAIM[target];
    if (source) {
        if (!getPackedBit(source, PENDING)) {
            bhmMessage("F", PREFIX,
                    "CLAIM[%d] = %d, but PENDING bit for irqS%d not set",
                    target, source, source);
        }
        if (getPackedBit(source, GATEWAY)) {
            bhmMessage("F", PREFIX,
                    "CLAIM[%d] = %d, but GATEWAY bit for irqS%d is set",
                    target, source, source);
        }
    }
}

static void throwBusError(Bool accessType) {
    if (accessType == IS_READ) {
        ppmReadAbort();
    } else {
        ppmWriteAbort();
    }
}

//
// Check for valid size and alignment
// - if error then signal a bus error and return False
// - if no error return True
//
static Bool validAccess(Uns32 offset, Uns32 bytes, Bool accessType, Bool artifactAccess) {

    if (bytes != 4) {
        if (!artifactAccess) {
            bhmMessage("W", PREFIX,
                       "Invalid register access size %d bytes at offset 0x%08x. "
                       "Only 4 byte accesses supported",
                       bytes, offset);
            throwBusError(accessType);
        }
        return False;
    }

    if (offset & 0x3) {
        if (!artifactAccess) {
            bhmMessage("W", PREFIX,
                       "Unaligned register access at offset 0x%08x. "
                       "Only 4 byte accesses supported",
                       offset);
            throwBusError(accessType);
        }
        return False;
    }

    return True;
}

static Uns32 indexFromUserData(void *userData, Uns32 arraySize) {
    Uns32 index = PTR_TO_UNS32(userData);
    if (index >= arraySize) {
        bhmMessage("F", PREFIX,
                "Invalid userData %d (>= %d)", index, arraySize);
    }
    return index;
}

static Uns32 bitIndexFromWordOffset(Uns32 offset, Uns32 maxIndex) {
    Uns32 index = (offset & 0x7f) >> 2;
    if (index >= maxIndex) {
        bhmMessage("F", PREFIX,
                "Invalid index %d (> %d) encoded in offset",
                index, maxIndex);
    }
    return index;
}

////////////////////////////////////////////////////////////////////////////////
// INTERRUPTS
////////////////////////////////////////////////////////////////////////////////

static void irqTSet(Uns32 target) {
    if (PSE_DIAG_MEDIUM) bhmMessage("I", PREFIX, "irqT%d 0->1\n", target);
    ppmWriteNet(irqT[target], 1);
}

static void irqTClr(Uns32 target) {
    if (PSE_DIAG_LOW) bhmMessage("I", PREFIX, "irqT%d 1->0\n", target);
    ppmWriteNet(irqT[target], 0);
}

static void evaluate() {
    Uns32 source, target;
    Bool  prevIRQT[config.targets];
    Uns32 prevCLAIM[config.targets];

    // Record previous IRQT values and clear them
    memcpy(prevIRQT, IRQT, config.targets * sizeof(*IRQT));
    memset(IRQT, 0, config.targets * sizeof(*IRQT));

    // Record previous CLAIM values and clear them
    memcpy(prevCLAIM, CLAIM, config.targets * sizeof(*CLAIM));
    memset(CLAIM, 0, config.targets * sizeof(*CLAIM));

    // Iterate over all interrupt sources looking for active pending bits
    for(source = 0; source <= config.sources; source++) {

        // Get Pending bit for this source
        Uns32 pending_word = source >> 5;
        Uns32 pending      = PENDING[pending_word];

        if (pending == 0) {
            // Optimization when all bits in pending word are inactive
            source |= WORDBITS-1;
            continue;
        }

        // Iterate each bit in pending
        Uns32 i;
        for (i = 0; i < WORDBITS && source <= config.sources; i++, source++) {

            Uns32 irqSPRIO     = PRIORITY[source];
            Bool  irqSPEND     = getPackedBit(source, PENDING);
            Bool  irqSACTV     = getPackedBit(source, GATEWAY);

            // only check targets when irq is pending and not active
            if (!irqSPEND || irqSACTV) continue;

            for (target=0; target<config.targets; target++) {

                Bool  irqTENAB = getPackedBit(source, ENABLE[target]);
                Uns32 irqTTHR  = THRESHOLD[target];

                // evaluate if pending is propagated to this target:
                // - Priority must be > targets current threshold setting
                // - Must be enabled for this target
                if ((irqSPRIO > irqTTHR) && irqTENAB) {
                    Uns32 claim         = CLAIM[target];
                    Uns32 claimPriority = claim ? PRIORITY[claim] : 0;

                    // Is this the first claim or a higher priority claim?
                    if (!claim || claimPriority < irqSPRIO) {
                        CLAIM[target] = source;
                        IRQT[target]  = True;
                    }
                }
            }
        }
    }

    for (target=0; target<config.targets; target++) {
        Bool oldIRQ   = prevIRQT[target];
        Bool newIRQ   = IRQT[target];

        if (PSE_DIAG_MEDIUM) {
            Uns32 oldCLAIM = prevCLAIM[target];
            Uns32 newCLAIM = CLAIM[target];

            if (newCLAIM && (newCLAIM != oldCLAIM)) {
                bhmMessage("I", PREFIX,
                        "new CLAIM[Target=%d]=irqS%d\n", target, newCLAIM);
            }
        }

        // Update interrupt signal for target
        if (newIRQ && !oldIRQ) {
            //bhmMessage("I", "PLIC", "iterate setting! %d", (int)target);
            irqTSet(target);
        }

        if (oldIRQ && !newIRQ) {
            irqTClr(target);
        }

        assertClaimIsPending(target);
    }
}


////////////////////////////////////////////////////////////////////////////////
// CALLBACKS
////////////////////////////////////////////////////////////////////////////////

//
// IRQ asserted sets the bit in the PENDING Register, only if this interrupt
// is not currently claimed and in-progress
// This is cleared by the claimRD or, if the PENDING registers are configured to
// be writable, a write directly to the PENDING register
//
PPM_NET_CB(irqUpdate) {
    Uns32 id  = indexFromUserData(userData, S_MAX+1);;
    Bool  old = IRQS[id];
    Bool  new = (value != 0);
    //bhmMessage("I", "PLIC", "irqUpdate %d", (int)id);
    if (PSE_DIAG_LOW) {
        if (old != new) {
            bhmMessage("I", PREFIX, "irqS%d %d->%d\n", id, old, new);
        }
    }

    // Track most recent value seen on net
    IRQS[id] = new;

    // Set the pending gor this irq if not already set and irq is asserted
    if (!getPackedBit(id, PENDING) && irqAsserted(old, new, id)) {

        setPackedBit(id, PENDING, 1);

        if (PSE_DIAG_LOW) {
            bhmMessage("I", PREFIX, "irqS%d Pending bit set by active irq input\n", id);
        }
    }

    evaluate();
}

//
// Trap Memory RD/Write
//
PPM_NBYTE_READ_CB(reservedRD) {

    if (!artifactAccess) {
        if (!IGNORE_RESERVED_ACCESS || PSE_DIAG_LOW) {
            bhmMessage("W", PREFIX,
                       "Read from reserved location at offset 0x%08x ignored\n",
                       offset);
        }
        if (!IGNORE_RESERVED_ACCESS) {
            throwBusError(IS_READ);
        }
    }

    memset(data, 0, bytes);
    return;
}

PPM_NBYTE_WRITE_CB(reservedWR) {
    if (!artifactAccess) {
        if (!IGNORE_RESERVED_ACCESS || PSE_DIAG_LOW) {
            bhmMessage("W", PREFIX,
                       "Write to reserved location at offset 0x%08x ignored\n",
                       offset);
        }
        if (!IGNORE_RESERVED_ACCESS && !artifactAccess) {
            throwBusError(IS_WRITE);
        }
    }
}

PPM_NBYTE_READ_CB(claimRD) {
    Uns32 source = 0;

    if (validAccess(offset, bytes, IS_READ, artifactAccess)) {

        // Get highest priority pending source for target
        Uns32 target = indexFromUserData(userData, T_MAX);
        source = CLAIM[target];

        // Sanity check that CLAIM is valid
        assertClaimIsPending(target);

        // Silently ignore artifact accesses and inactive claims
        if (!artifactAccess && source) {

            // Atomically clear pending bit for claimed source
            setPackedBit(source, PENDING, 0);

            // Set the claimed but not completed flag for the source
            setPackedBit(source, GATEWAY, 1);

            // Is Andes Preemptive Priority Interrupt feature active?
            if (ANDES_EXTENSIONS && FEATURE_ENABLED(andesFeature, PREMPT)) {
                Uns32 priority   = PRIORITY[source];
                Uns32 threshhold = THRESHOLD[target];

                // Limit thresholds to maximum supported
                if (threshhold > ANDES_PREEMPT_NUM_PRIORITIES) {
                    threshhold = ANDES_PREEMPT_NUM_PRIORITIES;
                }

                // Update threshold to claimed sources value and save previous
                THRESHOLD[target] = priority;
                setPackedBit(threshhold, ANDES_PREEMPT[target], 1);
            }

            evaluate();
        }
    }

    *(Uns32 *)data = source;
}

PPM_NBYTE_WRITE_CB(claimWR) {

    if (validAccess(offset, bytes, IS_WRITE, artifactAccess)) {
        Uns32 target = PTR_TO_UNS32(userData);
        Uns32 source = *(Uns32 *)data;

        if (source > config.sources) {

            bhmMessage("W", PREFIX,
                "Ignoring write of %d to claim[%d]: Exceeds number of sources=%d",
                source, target, config.sources);

        } else if (!artifactAccess && source && getPackedBit(source, GATEWAY)) {

            // Clear the claimed but not completed flag for the source
            setPackedBit(source, GATEWAY, 0);

            // Is Andes Preemptive Priority Interrupt feature active?
            if (ANDES_EXTENSIONS && FEATURE_ENABLED(andesFeature, PREMPT)) {
                Uns32 priority = highestPackedBit(ANDES_PREEMPT[target], ANDES_PREEMPT_NUM_REGS);

                // Restore threshold to highest saved and clear that bit
                THRESHOLD[target] = priority;
                setPackedBit(priority, ANDES_PREEMPT[target], 0);
            }

            evaluate();
        }
    }
}

PPM_NBYTE_READ_CB(priorityRD) {
    Uns32 priority = 0;

    if (validAccess(offset, bytes, IS_READ, artifactAccess)) {
        Uns32 id = indexFromUserData(userData, S_MAX+1);

        priority = PRIORITY[id];
    }

    *(Uns32 *)data = priority;
}

PPM_NBYTE_WRITE_CB(priorityWR) {

    if (validAccess(offset, bytes, IS_WRITE, artifactAccess)) {
        Uns32 id       = indexFromUserData(userData, S_MAX+1);
        Uns32 priority = *(Uns32 *)data;

        if (!artifactAccess) {

            if (priority > config.priorities) {
                // Mask priority value to highest supported value
                priority = config.priorities;
            }

            if (PRIORITY[id] != priority) {
                PRIORITY[id] = priority;
                evaluate();
            }
        }
    }
}

PPM_NBYTE_READ_CB(pendingRD) {
    Uns32 pending = 0;

    if (validAccess(offset, bytes, IS_READ, artifactAccess)) {
        Uns32 id = indexFromUserData(userData, S_PACK);
        pending = PENDING[id];
    }

    *(Uns32 *)data =  pending;
}

PPM_NBYTE_WRITE_CB(pendingWR) {

    if (!PENDING_WRITABLE) {

        // Pending is RO: treat writes same as writes to reserved
        reservedWR(offset, data, bytes, userData, artifactAccess);

    } else if (validAccess(offset, bytes, IS_WRITE, artifactAccess)) {

        if (!artifactAccess) {
            Uns32 id          = indexFromUserData(userData, S_PACK);
            Uns32 pendingMask = *(Uns32 *)data;
            Uns32 oldPending  = PENDING[id];

            if (id == 0) {
                // bit 0 of first register is hardwired to 0
                pendingMask &= ~1;
            }

            PENDING[id] |= pendingMask;

            if (PENDING[id] != oldPending) {

                if (PSE_DIAG_LOW) {
                    Uns32 i;
                    Uns32 mask = 1;
                    for (i = 0; i < 32; i++, mask <<= 1) {
                        if (!(oldPending & mask) && (PENDING[id] & mask)) {
                            bhmMessage(
                                "I", PREFIX,
                                "irqS%d Pending bit set by write to pending[%d]\n",
                                (id << 5) | i, id);
                        }
                    }
                }

                evaluate();
            }
        }
    }
}

PPM_NBYTE_READ_CB(enableRD) {
    Uns32 enable = 0;

    if (validAccess(offset, bytes, IS_READ, artifactAccess)) {
        Uns32 target = indexFromUserData(userData, T_MAX);
        Uns32 id     = bitIndexFromWordOffset(offset, S_PACK);

        enable = ENABLE[target][id];
    }

    *(Uns32 *)data = enable;
}

PPM_NBYTE_WRITE_CB(enableWR) {

    if (validAccess(offset, bytes, IS_WRITE, artifactAccess)) {
        Uns32 target = indexFromUserData(userData, T_MAX);
        Uns32 id     = bitIndexFromWordOffset(offset, S_PACK);
        Uns32 enable = *(Uns32 *)data;

        if (!artifactAccess) {

            if (id == 0) {
                // bit 0 of first register is hardwired to 0
                enable &= ~1;
            }

            if (ENABLE[target][id] != enable) {
                ENABLE[target][id] = enable;
                evaluate();
            }
        }
    }
}

PPM_NBYTE_READ_CB(thresholdRD) {
    Uns32 thresh = 0;

    if (validAccess(offset, bytes, IS_READ, artifactAccess)) {
        Uns32 target  = indexFromUserData(userData, T_MAX);
        thresh = THRESHOLD[target];
    }

    *(Uns32 *)data = thresh;
}

PPM_NBYTE_WRITE_CB(thresholdWR) {

    if (validAccess(offset, bytes, IS_WRITE, artifactAccess)) {
        Uns32 target = indexFromUserData(userData, T_MAX);
        Uns32 thresh = *(Uns32 *)data;

        if (!artifactAccess) {

            if (thresh > config.priorities) {
                // Mask threshold value to highest supported value
                thresh = config.priorities;
            }

            if (THRESHOLD[target] != thresh) {
                THRESHOLD[target] = thresh;
                evaluate();
            }
        }
    }
}

PPM_NBYTE_READ_CB(andesFeatureRD) {
    Uns32 feature = 0;

    if (validAccess(offset, bytes, IS_READ, artifactAccess)) {
        feature = andesFeature;
    }

    *(Uns32 *)data = feature;
}

PPM_NBYTE_WRITE_CB(andesFeatureWR) {

    if (validAccess(offset, bytes, IS_WRITE, artifactAccess)) {
        Uns32 feature = *(Uns32 *)data;

        if (!artifactAccess) {

            // Check for unimplemented feature usage
            if (FEATURE_ENABLED(feature, VECTOR)) {
                bhmMessage("W", PREFIX,
                        "Andes Vector mode feature not implemented");
                feature &= ~FEATURE_VECTOR_MASK;
            }

            andesFeature = feature & ANDES_FEATURE_MASK;
        }
    }
}

PPM_NBYTE_READ_CB(andesPreemptRD) {
    Uns32 preempt = 0;

    if (validAccess(offset, bytes, IS_READ, artifactAccess)) {
        Uns32 target = indexFromUserData(userData, T_MAX);
        Uns32 id     = bitIndexFromWordOffset(offset, ANDES_PREEMPT_NUM_REGS);
        preempt = ANDES_PREEMPT[target][id];
    }

    *(Uns32 *)data = preempt;
}

PPM_NBYTE_WRITE_CB(andesPreemptWR) {

    if (validAccess(offset, bytes, IS_WRITE, artifactAccess)) {
        Uns32 preempt = *(Uns32 *)data;

        if (!artifactAccess) {
            Uns32 target = indexFromUserData(userData, T_MAX);
            Uns32 id     = bitIndexFromWordOffset(offset, ANDES_PREEMPT_NUM_REGS);

            if (ANDES_PREEMPT[target][id] != preempt) {
                ANDES_PREEMPT[target][id] = preempt;
                evaluate();
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// MODEL CONSTRUCTION
////////////////////////////////////////////////////////////////////////////////

static void netCallbacks() {
    //
    // Allocate the ports
    //
    irqS = (ppmNetHandle *) malloc(sizeof(ppmNetHandle) * config.sources);
    irqT = (ppmNetHandle *) malloc(sizeof(ppmNetHandle) * config.targets);

    Uns32 iter;
    char name[32];
    for (iter=0; iter<config.sources; iter++) {
        Uns32 irq = iter+1;
        sprintf(name, "irqS%d", irq);

        irqS[iter] = ppmOpenNetPort(name);
        ppmInstallNetCallback(irqS[iter], irqUpdate, UNS32_TO_PTR(irq));
    }

    for (iter=0; iter<config.targets; iter++) {
        sprintf(name, "irqT%d", iter);
        irqT[iter] = ppmOpenNetPort(name);
    }
}

static void modelInit () {

    char name[32];
    char desc[128];

    bhmUns32ParamValue("num_sources",    &config.sources);
    bhmUns32ParamValue("num_targets",    &config.targets);
    bhmUns32ParamValue("num_priorities", &config.priorities);
    bhmUns32ParamValue("priority_base",  &config.PriorityBase);
    bhmUns32ParamValue("pending_base",   &config.PendingBase);
    bhmUns32ParamValue("enable_base",    &config.EnablesBase);
    bhmUns32ParamValue("enable_stride",  &config.EnablesStride);
    bhmUns32ParamValue("context_base",   &config.ContextBase);
    bhmUns32ParamValue("context_stride", &config.ContextStride);

    netCallbacks();

    // Check that parameters are legal (and arrays cannot be overflowed)
    if (config.sources > S_MAX) {
        bhmMessage("F", PREFIX, "num_sources=%d, must be <= %d", config.sources, S_MAX);
    }
    if (config.targets > T_MAX) {
        bhmMessage("F", PREFIX, "num_targets=%d, must be <= %d", config.targets, T_MAX);
    }
    if (config.priorities > P_MAX) {
        bhmMessage("F", PREFIX, "num_priorities=%d, must be <= %d", config.priorities, P_MAX);
    }

    //
    // Priority registers irqS[0..sources]
    //
    Uns32 id;
    for(id = 0; id <= config.sources; id++) {
        Uns32 offset = config.PriorityBase + (id * WORDBYTES);

        if (id == 0) {
            if (ANDES_EXTENSIONS) {
                // Andes PLIC extension adds Feature Enable register at priority[0] reserved location
                sprintf(name, "FeatureEnable");
                sprintf(desc, "AndesCore PLIC Feature Enable Register");
                ppmCreateNByteRegister(name, desc, handles.port0, offset, WORDBYTES,
                        andesFeatureRD, andesFeatureWR, 0, &andesFeature, 0, True, 1, 1, BHM_ENDIAN_LITTLE);
            } else {
                // priority[0] is a reserved location in PLIC spec, so no register added
                // - Note: accesses will be ignored when IGNORE_RESERVED_ACCESS is enabled
            }
        } else {
            sprintf(name, "Priority%d", id);
            sprintf(desc, "Priority of Input Interrupt Source %d", id);
            ppmCreateNByteRegister(name, desc, handles.port0, offset, WORDBYTES,
                    priorityRD, priorityWR, 0, &PRIORITY[id], UNS32_TO_PTR(id), True, 1, 1, BHM_ENDIAN_LITTLE);
        }
    }

    //
    // One Pending register for every 32 irqS
    // - irqS numbered from 1..sources, with id 0, bit 0 not used
    //
    for (id = 0; id * WORDBITS <= config.sources; id++) {
        Uns32 offset = config.PendingBase + (id * WORDBYTES);

        sprintf(name, "Pending%d", id);
        sprintf(desc, "Pending Interrupt Register for Interrupts %d downto %d",
                (id*WORDBITS)+WORDBITS-1, (id*WORDBITS));
        ppmCreateNByteRegister(name, desc, handles.port0, offset, WORDBYTES,
                pendingRD, pendingWR, 0, &PENDING[id], UNS32_TO_PTR(id), True, 1, 1, BHM_ENDIAN_LITTLE);
    }

    //
    // Andes PLIC Extension trigger type and info registers
    //
    if (ANDES_EXTENSIONS) {

        // Andes trigger type array (read only)
        for (id = 0; id * WORDBITS <= config.sources; id++) {
            Uns32 offset = ANDES_TRIGGER_BASE + (id * WORDBYTES);

            sprintf(name, "TriggerType%d", id);
            sprintf(desc, "Andes Trigger Type Register for Interrupts %d downto %d",
                    (id*WORDBITS)+WORDBITS-1, (id*WORDBITS));
            ppmCreateNByteRegister(name, desc, handles.port0, offset, WORDBYTES,
                    0, 0, 0, &ANDES_TRIGGER[id], 0, True, 1, 0, BHM_ENDIAN_LITTLE);

            // Initialise register value from parameters
            bhmUns32ParamValue(name, &ANDES_TRIGGER[id]);
        }

        // Andes number of interrupts and targets register (read only)
        num_interrupts_targets.bits.num_interrupt = config.sources;
        num_interrupts_targets.bits.num_target    = config.targets;
        ppmCreateNByteRegister(
                "NumIntsAndTgts",
                "Andes number of interrupts and targets register",
                handles.port0, ANDES_NUM_INTERRUPTS_TARGETS_ADDR, WORDBYTES,
                0, 0, 0, &num_interrupts_targets, 0, True, 1, 0, BHM_ENDIAN_LITTLE);

        // Andes version and max priority register register (read only)
        Uns32 version;
        bhmUns32ParamValue("andes_version", &version);
        version_max_priority.bits.version      = version;
        version_max_priority.bits.max_priority = config.priorities;
        ppmCreateNByteRegister(
                "VersionMaxPriority",
                "Andes version and max priority register",
                handles.port0, ANDES_VERSION_MAX_PRIORITY_ADDR, WORDBYTES,
                0, 0, 0, &version_max_priority, 0, True, 1, 0, BHM_ENDIAN_LITTLE);
    }

    //
    // For each target add enable bit register for each source
    //
    Uns32 target;
    for (target=0; target < config.targets; target++) {
        Uns32 enableBase = config.EnablesBase + (target * config.EnablesStride);

        for (id = 0; id * WORDBITS <= config.sources; id++) {
            Uns32 offset = enableBase + (id * WORDBYTES);

            sprintf(name, "Target%d_Enable%d", target, id);
            sprintf(desc, "Target %d: Enable Register for Interrupts %d downto %d",
                    target, (id*WORDBITS)+WORDBITS-1, (id*WORDBITS));
            ppmCreateNByteRegister(name, desc, handles.port0, offset, WORDBYTES,
                    enableRD, enableWR, 0, &ENABLE[target][id], UNS32_TO_PTR(target), True, 1, 1, BHM_ENDIAN_LITTLE);
        }
    }

    //
    // For each target add priority threshold and claim/complete registers
    //
    for (target=0; target < config.targets; target++) {
        Uns32 contextBase = config.ContextBase + (target * config.ContextStride);

        sprintf(name, "Target%d_Threshold", target);
        sprintf(desc, "Target %d Priority Threshold", target);
        ppmCreateNByteRegister(name, desc, handles.port0, contextBase, WORDBYTES,
                thresholdRD, thresholdWR, 0, &THRESHOLD[target], UNS32_TO_PTR(target), True, 1, 1, BHM_ENDIAN_LITTLE);

        sprintf(name, "Target%d_Claim", target);
        sprintf(desc, "Target %d Claim for Source", target);
        ppmCreateNByteRegister(name, desc, handles.port0, contextBase+WORDBYTES, WORDBYTES,
                claimRD, claimWR, 0, &CLAIM[target], UNS32_TO_PTR(target), True, 1, 1, BHM_ENDIAN_LITTLE);

        //
        // Andes PLIC Extension preempted priority stack registers
        //
        if (ANDES_EXTENSIONS) {
            for (id = 0; id <= ANDES_PREEMPT_NUM_REGS; id++) {
                Uns32 offset = contextBase + ANDES_PREEMPT_OFFSET + (id * WORDBYTES);

                sprintf(name, "PreemptedPriorityStack%d", id);
                sprintf(desc, "Andes preempted priority stack Register for priorities %d downto %d",
                        (id*WORDBITS)+WORDBITS-1, (id*WORDBITS));
                ppmCreateNByteRegister(name, desc, handles.port0, offset, WORDBYTES,
                        andesPreemptRD, andesPreemptWR, 0, &ANDES_PREEMPT[target][id],
                        UNS32_TO_PTR(target), True, 1, 1, BHM_ENDIAN_LITTLE);
            }
        }
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
