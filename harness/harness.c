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
#define INSTRUCTIONS_PER_TIME_SLICE 25000.0                                             //(INSTRUCTIONS_PER_SECOND*QUANTUM_TIME_SLICE)
#define QUANTUM_TIME_SLICE (INSTRUCTIONS_PER_TIME_SLICE / INSTRUCTIONS_PER_SECOND)      // 0.0000010 //

struct optionsS {
	Bool configurecpuinstance;
} options = {
    .configurecpuinstance = False,
};

unsigned int peCount[N_PES];
double pe_DeRate[N_PES];
float myTime = 0;

unsigned int vec2usi(char *vec){
    unsigned int auxValue = 0x00000000;
    unsigned int aux;
    aux = 0x000000FF & vec[3]; 
    auxValue = ((aux << 24) & 0xFF000000);
    aux = 0x000000FF & vec[2];
    auxValue = auxValue | ((aux << 16) & 0x00FF0000);
    aux = 0x000000FF & vec[1];
    auxValue = auxValue | ((aux << 8) & 0x0000FF00);
    aux = 0x000000FF & vec[0];
    auxValue = auxValue | ((aux) & 0x000000FF);
    return auxValue;
}

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

#if PRINT_FETCH
#endif
int getProcessorID(optProcessorP processor){
    int processorID;
    char processorName[7] = "@@@@@@@";
    strcpy(processorName,opObjectName(processor)); 
    if(((int)processorName[5] - 48) >= 0 && ((int)processorName[5] - 48) <= 9){
        processorID = ((int)processorName[3] - 48)*100 + ((int)processorName[4] - 48)*10 + ((int)processorName[5] - 48);
    }
    else if(((int)processorName[4] - 48) >= 0 && ((int)processorName[4] - 48) <= 9){
        processorID = ((int)processorName[3] - 48)*10 + ((int)processorName[4] - 48);
    }
    else processorID = ((int)processorName[3] - 48);
    //ERROR CATCHER!
    if(processorID < 0 || processorID > N_PES){
        opMessage("I", "FETCH CALLBACK", "~~~~> Ocorreu um erro! %d",processorID);
        while(1){}     
    }
    return processorID;
}

// unsigned int saiu_int = 0;
// unsigned int entrou_int = 0;

// static OP_MONITOR_FN(FetchCallback) { 
//     /* get the processor id*/
//     int processorID = getProcessorID(processor);

//     /*get the waiting packet flag*/
//     //char value[4];
//     //opProcessorRead(processor, 0x0FFFFFFC, &value, 4, 1, True, OP_HOSTENDIAN_TARGET);
//     //unsigned int intValue = htonl(vec2usi(value));
    
//     /*if the processor is not waiting a packet then run the disassemble*/
//     //if(!intValue){
//         //char instruction[60];
//         //strcpy(instruction,opProcessorDisassemble(processor, addr, OP_DSA_UNCOOKED));
//         //sscanf(instruction,"%s %*s\n",instruction);
//     //if (processorID == 7)
      
      
//     peCount[processorID]++;

//     if(processorID == 2){
//         if( strcmp(opProcessorDisassemble(processor, addr, OP_DSA_UNCOOKED), "mret") == 0){
//             saiu_int++;
//             opMessage("I", "FETCH", "PE%d- SAIU do handler %d vezes & entrou %d vezes --------> dif: %d ", processorID, saiu_int, entrou_int, entrou_int-saiu_int);    
//         }
//         else if (addr == 0x80000030){
//             entrou_int++;
//             opMessage("I", "FETCH", "PE%d- saiu do handler %d vezes & ENTROU %d vezes --------> dif: %d ", processorID, saiu_int, entrou_int, entrou_int-saiu_int);    
//         }
//         if( entrou_int-saiu_int == -1){
//             opMessage("I", "FETCH", "PE deu RUIM!");
//             opProcessorTraceBufferDump(processor);
//             opProcessorHalt(processor);
//         }
//     }

      
      
      
//         //opMessage("I", "FETCH", "PE%d- %s @ %x", processorID, opProcessorDisassemble(processor, addr, OP_DSA_UNCOOKED), (unsigned int)addr);

//         //                         BASE ADDRESS -  (INSTRUCTION TYPE OFFSET)
//         //unsigned int countAddress = 0x0FFFFFF8 - (getInstructionType(instruction)*4);

//         /* Load the atual value and add one */
//         /*char read_EI[4];
//         opProcessorRead(processor, countAddress, &read_EI, 4, 1, True, OP_HOSTENDIAN_TARGET);
//         unsigned int read_executedInstructions = vec2usi(read_EI);
//         read_executedInstructions = htonl(read_executedInstructions) + 1;*/

//         /* Store the atual value */
//         /*char EI[4];
//         EI[3] = (htonl(read_executedInstructions) >> 24) & 0x000000FF;
//         EI[2] = (htonl(read_executedInstructions) >> 16) & 0x000000FF;
//         EI[1] = (htonl(read_executedInstructions) >> 8) & 0x000000FF;
//         EI[0] = htonl(read_executedInstructions) & 0x000000FF;
//         opProcessorWrite(processor, countAddress, EI, 4, 1, True, OP_HOSTENDIAN_TARGET);*/
//     //}
// }
// //#endif

static OP_MONITOR_FN(FinishCallback){
    opMessage("I", "HARNESS", " >>> SIMULAÇÃO FINALIZADA COM SUCESSO!");
    opProcessorFinish(processor, 0);
    return;
}

static OP_MONITOR_FN(FreqCallback){
    //opMessage("I", "HARNESS", " >>> DETECTADA MODIFICAÇÃO NA FREQUENCIA!");
    // gets the processor ID
    int processorID = getProcessorID(processor);

    // reads the frequency scale, provided by the system
    char readFreq_8bit[4];
    opProcessorRead(processor, 0x8FFFFFF8, &readFreq_8bit, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int readFreq_32bit = vec2usi(readFreq_8bit);

    // translates from fixed point to float
    float freqScale = 100 - (float)((float)readFreq_32bit / 10);
    if(freqScale == 100){
        freqScale = 50;
    }
    opMessage("I", "HARNESS", " >>> PE %d changing to %.1f%% of its nominal frequency -- (%.0fMHz)", processorID, 100-freqScale, 10*(100-freqScale));
    opProcessorDerate(processor, freqScale);
    return;
}

int main(int argc, const char *argv[]) {
    int             runningPE       = 0;
    optProcessorP   stopProcessor   = 0;
    Bool            finished        = False;
	optProcessorP proc;
    opSessionInit(OP_VERSION);
    /* create the root module with reduced Quantum (in line with Custom Scheduler) */
	optParamP params = OP_PARAMS(OP_PARAM_DOUBLE_SET(OP_FP_QUANTUM, QUANTUM_TIME_SLICE));
    optCmdParserP parser = opCmdParserNew(MODULE_NAME, OP_AC_ALL);
    opCmdParserAdd(parser, "configurecpuinstance", 0, "bool", "user", OP_FT_BOOLVAL, &options.configurecpuinstance, "Add configuration to enable Imperas Intercepts to CPU instance", OP_AC_ALL, 0, 0);  // enable interception
    if (!opCmdParseArgs(parser, argc, argv)) {
		opMessage("E", MODULE_NAME, "Command line parse incomplete");
	}
    opCmdParseArgs(parser, argc, argv);
    optModuleP mi = opRootModuleNew(&modelAttrs, MODULE_NAME, params);
    optModuleP modNew = opModuleNew(mi, MODULE_DIR, MODULE_INSTANCE, 0, 0);

    while ((proc = opProcessorNext(modNew, proc))) {
        char id[4];
        id[3] = (runningPE >> 24) & 0x000000FF;
        id[2] = (runningPE >> 16) & 0x000000FF;
        id[1] = (runningPE >> 8) & 0x000000FF;
        id[0] = runningPE & 0x000000FF;
        opProcessorWrite(proc, 0x8FFFFFFC, id, 4, 1, True, OP_HOSTENDIAN_TARGET);

        // Go to the next processor
        opMessage("I", "HARNESS INFO", "================== INICIALIZANDO PE %d ==================", runningPE);
    
        if (runningPE == 0){
            opMessage("I", "HARNESS INFO", "\t > MONITOR DE FINALIZAÇÃO ADICIONADO!");
            opProcessorFetchMonitorAdd(proc, 0x80000e04, 0x80000e07, FinishCallback, "finish");
        }
        
        opMessage("I", "HARNESS INFO", "\t > MONITOR DE FREQUENCIA ADICIONADO!");
        opProcessorWriteMonitorAdd (proc, 0x8FFFFFF8, 0x8FFFFFFB, FreqCallback, "frequency");

        
        //opProcessorFetchMonitorAdd(proc, 0x80000000, 0x8fffffff, FetchCallback, "fetch");
        

#if PRINT_FETCH
        if (runningPE == PRINT_FETCH_PE)
            opProcessorFetchMonitorAdd(proc, 0x80000000, 0x8fffffff, FetchCallback, "fetch");
#endif     
        runningPE++;
    }

    /* Simulation loop */
    while(!finished) {
        myTime += 0.1;
        opRootModuleSetSimulationStopTime(mi, myTime);
        stopProcessor = opRootModuleSimulate(mi);
        
        optStopReason sr = stopProcessor ? opProcessorStopReason(stopProcessor)
                                         : OP_SR_EXIT;
        switch(sr) {
            case OP_SR_EXIT:
                opMessage("I", "HARNESS", "Simulation time: %.1f seconds elapsed...", myTime);
                break; 
            
            case OP_SR_FINISH:
                finished = True;
                break;
            
            default:
                opMessage("I", "HARNESS", "Processor %s icount %u stopped with unexpected reason 0x%x (%s)\n", opObjectName (stopProcessor), (Uns32)opProcessorICount(stopProcessor), sr, opStopReasonString(sr) );
                break;
        }
        
    }

    opSessionTerminate();

    return (opErrors() ? 1 : 0);    // set exit based upon any errors
}

