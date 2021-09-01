/*
 *
 * Copyright (c) 2005-2021 Imperas Software Ltd., www.imperas.com
 *
 * The contents of this file are provided under the Software License
 * Agreement that you accepted before downloading this file.
 *
 * This source forms part of the Software and can be used for educational,
 * training, and demonstration purposes but cannot be used for derivative
 * works except in cases where the derivative works require OVP technology
 * to run.
 *
 * For open source models released under licenses that you can use for
 * derivative works, please visit www.OVPworld.org or www.imperas.com
 * for the location of the open source models.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "op/op.h"

#define MODULE_NAME "top"
#define MODULE_DIR "module"
#define MODULE_INSTANCE "u2"
#define N_PES 9

// Quantum defines
#define INSTRUCTIONS_PER_SECOND 1000000000.0                                            // 1GHz (assuming 1 instruction per cycle)
#define INSTRUCTIONS_PER_TIME_SLICE 25000.0                                               //(INSTRUCTIONS_PER_SECOND*QUANTUM_TIME_SLICE)
#define QUANTUM_TIME_SLICE (INSTRUCTIONS_PER_TIME_SLICE / INSTRUCTIONS_PER_SECOND)      // 0.0000010 //

struct optionsS {
	Bool configurecpuinstance;
} options = {
    .configurecpuinstance = False,
};

/*static OP_CONSTRUCT_FN(moduleConstruct) {
    const char *u1_path = "module";
    opModuleNew(
        mi,       // parent module
        u1_path,       // modelfile
        "u1",   // name
        0,
        0
    );
}*/

/*static void cmdParser(optCmdParserP parser) {
}*/

/*typedef struct optModuleObjectS {
    // insert module persistent data here
} optModuleObject;*/

/*static OP_PRE_SIMULATE_FN(modulePreSimulate) {
// insert modulePreSimulate code here
}*/

/*static OP_SIMULATE_STARTING_FN(moduleSimulate) {
// insert moduleSimulate code here
}*/

static OP_POST_SIMULATE_FN(modulePostSimulate) {
// insert modulePostSimulate code here
}

static OP_DESTRUCT_FN(moduleDestruct) {
// insert moduleDestruct code here
}

/*Attributes Set in Module Construction */
optModuleAttr modelAttrs = {
    .versionString = OP_VERSION,
    .type = OP_MODULE,
    .name = MODULE_NAME,
    .releaseStatus = OP_UNSET,
    .purpose = OP_PP_BAREMETAL,
    .visibility = OP_VISIBLE,
    //.constructCB          = moduleConstruct,
    .postSimulateCB = modulePostSimulate,
    .destructCB = moduleDestruct,
};

/*optModuleAttr modelAttrs = {
    .versionString       = OP_VERSION,
    .type                = OP_MODULE,
    .name                = MODULE_NAME,
    .objectSize          = sizeof(optModuleObject),
    .releaseStatus       = OP_UNSET,
    .purpose             = OP_PP_BAREMETAL,
    .visibility          = OP_VISIBLE,
    .constructCB          = moduleConstruct,
    .preSimulateCB        = modulePreSimulate,
    .simulateCB           = moduleSimulate,
    .postSimulateCB       = modulePostSimulate,
    .destructCB           = moduleDestruct,
};*/

int main(int argc, const char *argv[]) {
    int runningPE;
    int countQuantum = 0;
    int finishedProcessors = 0;
    optTime myTime = QUANTUM_TIME_SLICE;
    optStopReason stopReason = OP_SR_SCHED;
	optProcessorP proc;
    
    opSessionInit(OP_VERSION);
    /* create the root module with reduced Quantum (in line with Custom Scheduler) */
	optParamP params = OP_PARAMS(OP_PARAM_DOUBLE_SET(OP_FP_QUANTUM, QUANTUM_TIME_SLICE));
    optCmdParserP parser = opCmdParserNew(MODULE_NAME, OP_AC_ALL);
    opCmdParserAdd(parser, "configurecpuinstance", 0, "bool", "user", OP_FT_BOOLVAL, &options.configurecpuinstance, "Add configuration to enable Imperas Intercepts to CPU instance", OP_AC_ALL, 0, 0);  // enable interception
    if (!opCmdParseArgs(parser, argc, argv)) {
		opMessage("E", MODULE_NAME, "Command line parse incomplete");
	}
    //cmdParser(parser);
    opCmdParseArgs(parser, argc, argv);
    optModuleP mi = opRootModuleNew(&modelAttrs, MODULE_NAME, params);
    optModuleP modNew = opModuleNew(mi, MODULE_DIR, MODULE_INSTANCE, 0, 0);

    // must advance to next phase for the API calls that follow
	opRootModulePreSimulate(mi);

    // flag to add the callbacks during the first quantum
	int firstRun = N_PES;
    
    do{
        // move time forward by time slice on root module
		// NOTE: This matches the standard scheduler which moves time forward in
		//       the system and then executes instructions on all processors
        opRootModuleTimeAdvance(mi, myTime);

        // reset the PE index
        runningPE = 0;

        while ((proc = opProcessorNext(modNew, proc))) {
            if(firstRun){
                char id[4];
                id[3] = ((9-firstRun) >> 24) & 0x000000FF;
                id[2] = ((9-firstRun) >> 16) & 0x000000FF;
                id[1] = ((9-firstRun) >> 8) & 0x000000FF;
                id[0] = (9-firstRun) & 0x000000FF;
                opProcessorWrite(proc, 0x8FFFFFFC, id, 4, 1, True, OP_HOSTENDIAN_TARGET);
                firstRun--;
                // Go to the next processor
			    opMessage("I", "HARNESS INFO", ">>>>>> INICIALIZANDO PE %d", runningPE);  // alzemiro modification
            }

  			// simulate processor for INSTRUCTIONS PER_TIME_SLICE instructions
            stopReason = opProcessorSimulate(proc, INSTRUCTIONS_PER_TIME_SLICE);
            if (stopReason == OP_SR_EXIT) {
				finishedProcessors++;
			}
            runningPE++;
        }

        // increments the number of simulated quanta
        countQuantum++;
        if (countQuantum % 10000 == 0) {
            opMessage("I", "HARNESS INFO", "Iniciando Quantum %d - elapsed time: %lfs / %.2lfms", countQuantum, (countQuantum * QUANTUM_TIME_SLICE),
                     (countQuantum * QUANTUM_TIME_SLICE * 1000));  // alzemiro modification
        }

        break; 
        // checks if all processors has exited
        if (finishedProcessors == N_PES) {
			opMessage("I", "HARNESS", "Simulation Complete (%s) e %d quantums", opStopReasonString(stopReason), countQuantum);
			break;  // finish simulation loop
		}

        myTime += QUANTUM_TIME_SLICE;

    }while(1);


    opRootModuleSimulate(mi);
    opSessionTerminate();

    return (opErrors() ? 1 : 0);    // set exit based upon any errors
}

