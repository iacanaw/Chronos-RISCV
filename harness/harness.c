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
#define QUANTUM_TIME_SLICE (double)(INSTRUCTIONS_PER_TIME_SLICE / INSTRUCTIONS_PER_SECOND)

// Address to save the instruction counters
#define LOAD_ADDR       0x8FFFFFF4u
#define STORE_ADDR      0x8FFFFFF0u
#define OTHERS_ADDR     0x8FFFFFECu

unsigned int load[N_PES];
unsigned int store[N_PES];
unsigned int others[N_PES];
unsigned int fetch[N_PES];

struct optionsS {
	Bool configurecpuinstance;
} options = {
    .configurecpuinstance = False,
};

unsigned int peCount[N_PES];
double pe_DeRate[N_PES];
double myTime = 0;

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

static OP_MONITOR_FN(FetchCallback) { 
    // get the processor ID
    int processorID = getProcessorID(processor);
    fetch[processorID]++;

    char aux_8bits[4];
    opProcessorRead(processor, addr, &aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int instruction32 = vec2usi(aux_8bits);
    
    // https://www2.eecs.berkeley.edu/Pubs/TechRpts/2014/EECS-2014-54.pdf - pag 60
    if((instruction32 & 0x0000007F) == 0x00000003){         // checks if the opcode is equal to b0000011 (LOAD) 
        load[processorID]++;
    }
    else if((instruction32 & 0x0000007F) == 0x00000023){    // checks if the opcode is equal to b0100011 (STORE)
        store[processorID]++;
    }
    else{
        others[processorID]++;
    }

    if(fetch[processorID] > 100){
        //opMessage("I", "HARNESS", "Nas ultimas 10k instrucoes o PE%d executou %d loads, %d stores e %d outras instruções", processorID, load[processorID], store[processorID], others[processorID]);
        fetch[processorID] = 0;

        opProcessorRead(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load[processorID] = load[processorID] + vec2usi(aux_8bits);
        aux_8bits[3] = (load[processorID] >> 24) & 0x000000FF;
        aux_8bits[2] = (load[processorID] >> 16) & 0x000000FF;
        aux_8bits[1] = (load[processorID] >> 8) & 0x000000FF;
        aux_8bits[0] =  load[processorID] & 0x000000FF;
        opProcessorWrite(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load[processorID] = 0;

        opProcessorRead(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store[processorID] = store[processorID] + vec2usi(aux_8bits);
        aux_8bits[3] = (store[processorID] >> 24) & 0x000000FF;
        aux_8bits[2] = (store[processorID] >> 16) & 0x000000FF;
        aux_8bits[1] = (store[processorID] >> 8) & 0x000000FF;
        aux_8bits[0] =  store[processorID] & 0x000000FF;
        opProcessorWrite(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store[processorID] = 0;

        opProcessorRead(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others[processorID] = others[processorID] + vec2usi(aux_8bits);
        aux_8bits[3] = (others[processorID] >> 24) & 0x000000FF;
        aux_8bits[2] = (others[processorID] >> 16) & 0x000000FF;
        aux_8bits[1] = (others[processorID] >> 8) & 0x000000FF;
        aux_8bits[0] =  others[processorID] & 0x000000FF;
        opProcessorWrite(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others[processorID] = 0;
    }

    return;
}

static OP_MONITOR_FN(FinishCallback){
    opMessage("I", "HARNESS", " >>> SIMULAÇÃO FINALIZADA COM SUCESSO!");
    opProcessorFinish(processor, 0);
    return;
}

static OP_MONITOR_FN(FreqCallback){
    // gets the processor ID
    int processorID = getProcessorID(processor);

    // reads the frequency scale, provided by the system
    char readFreq_8bit[4];
    opProcessorRead(processor, 0x8FFFFFF8, &readFreq_8bit, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int readFreq_32bit = vec2usi(readFreq_8bit);

    // translates from fixed point to float
    float freqScale = 100 - (float)((float)readFreq_32bit / 10);
    if(freqScale == 100){ // this should never occour, but this is a security if to ensure that the processor does never stall by going to 0% of its frequency
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

    for(int i = 0; i < N_PES; i++){
        load[i] = 0;
        store[i] = 0;
        others[i] = 0;
        fetch[i] = 0;
    }

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

        
        opProcessorFetchMonitorAdd(proc, 0x80000000, 0x8fffffff, FetchCallback, "fetch");
        
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

    for(int processorID = 0; processorID < N_PES; processorID++){
        opMessage("I", "HARNESS", "PE%d executou %d loads, %d stores e %d outras instruções", processorID, load[processorID], store[processorID], others[processorID]);
    }

    opSessionTerminate();

    return (opErrors() ? 1 : 0);    // set exit based upon any errors
}

