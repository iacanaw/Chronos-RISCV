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

#define BRANCH  1
#define ARITH   2
#define JUMP    3
#define MOVE    4
#define LOAD    5
#define STORE   6
#define SHIFT   7
#define NOP     8
#define LOGICAL 9
#define MULTDIV 10
#define WEIRD   11

char branchInstructions[][12]   = {"beqz","bgez","bnez","beq","bne","blt","bge","bltu","bgeu","bltz","bgtz","blez","bgt","ble","bgtu","bleu","EndList@"};
char arithInstructions[][12]    = {"add","addi","sub","lui","auipc","slt","slti","sltu","sltiu","EndList@"};
char jumpInstructions[][12]     = {"mret","ret","jal","jalr","j","jr","call","EndList@"};
char moveInstructions[][12]     = {"mv","EndList@"};
char loadInstructions[][12]     = {"lb","lh","lw","lbu","lhu","EndList@"};
char storeInstructions[][12]    = {"sb","sh","sw","EndList@"};
char shiftInstructions[][12]    = {"sll","slli","srl","srli","sra","srai","EndList@"};
char nopInstructions[][12]      = {"nop","EndList@"};
char logicalInstructions[][12]  = {"and","not","andi","or","ori","xor","xori","csrr","csrwi","csrci","csrw","csrs","csrsi","csrrw","csrrs","csrrc","csrrwi","csrrsi","csrrci","EndList@"};
char multDivInstructions[][12]  = {"mul","rem","remu","div","divu","EndList@"};
char weirdInstructions[][12]    = {"ecall","ebreak","eret","mrts","mrth","hrts","wfi","sfence.vm","fence","fence.i","scall","sbreak","rdcycle","rdcycleh","rdtime","rdtimeh","rdinstret","rdinstreth","EndList@"};

unsigned int getInstructionType(char *instruction){
    int i = 0;
    while(strcmp(branchInstructions[i],"EndList@") != 0) { // Branch type
        if(strcmp(branchInstructions[i],instruction) == 0){
            return BRANCH;
        }
        i++;
    }
    i = 0;
    while(strcmp(arithInstructions[i],"EndList@") != 0) { // Arith type
        if(strcmp(arithInstructions[i],instruction) == 0){
            return ARITH;
        }
        i++;
    }
    i = 0;
    while(strcmp(jumpInstructions[i],"EndList@") != 0) { // Jump type
        if(strcmp(jumpInstructions[i],instruction) == 0){
            return JUMP;
        }
        i++;
    }
    i = 0;
    while(strcmp(moveInstructions[i],"EndList@") != 0) { // Move type
        if(strcmp(moveInstructions[i],instruction) == 0){
            return MOVE;
        }
        i++;
    }
    i = 0;
    while(strcmp(loadInstructions[i],"EndList@") != 0) { // Load type
        if(strcmp(loadInstructions[i],instruction) == 0){
            return LOAD;
        }
        i++;
    }
    i = 0;
    while(strcmp(storeInstructions[i],"EndList@") != 0) { // Store type
        if(strcmp(storeInstructions[i],instruction) == 0){
            return STORE;
        }
        i++;
    }
    i = 0;
    while(strcmp(shiftInstructions[i],"EndList@") != 0) { // Shift type
        if(strcmp(shiftInstructions[i],instruction) == 0){
            return SHIFT;
        }
        i++;
    }
    i = 0;
    while(strcmp(nopInstructions[i],"EndList@") != 0) { // NOP type
        if(strcmp(nopInstructions[i],instruction) == 0){
            return NOP;
        }
        i++;
    }
    i = 0;
    while(strcmp(logicalInstructions[i],"EndList@") != 0) { // Logical type
        if(strcmp(logicalInstructions[i],instruction) == 0){
            return LOGICAL;
        }
        i++;
    }
    i = 0;
    while(strcmp(multDivInstructions[i],"EndList@") != 0) { // Multiplication and Division type
        if(strcmp(multDivInstructions[i],instruction) == 0){
            return MULTDIV;
        }
        i++;
    }
    i = 0;
    while(strcmp(weirdInstructions[i],"EndList@") != 0) { // Weird stuff type
        if(strcmp(weirdInstructions[i],instruction) == 0){
            return WEIRD;
        }
        i++;
    }
    return 66;
}

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

unsigned int load[N_PES];
unsigned int store[N_PES];
unsigned int others[N_PES];
unsigned int fetch[N_PES];

static OP_MONITOR_FN(FetchCallback) { 
    // get the processor ID
    int processorID = getProcessorID(processor);
    fetch[processorID]++;

    //char instruction[4];
    //unsigned int instruction32 = vec2usi(instruction);
    // opProcessorRead(processor, addr, &instruction, 4, 1, True, OP_HOSTENDIAN_TARGET);
    
    // if((instruction32 & 0x0000007F) == 0x00000003){
    //     load[processorID]++;
    // }
    // else if((instruction32 & 0x0000007F) == 0x00000023){
    //     store[processorID]++;
    // }
    // else{
    //     others[processorID]++;
    // }

    /*if(fetch[processorID] > 10000){
        opMessage("I", "HARNESS", "Nas ultimas 10k instrucoes o PE%d executou %d loads, %d stores e %d outras instruções", processorID, load[processorID], store[processorID], others[processorID]);

        fetch[processorID] = 0;
        load[processorID] = 0;
        store[processorID] = 0;
        others[processorID] = 0;
    }*/

    char instruction[60];
    strcpy(instruction,opProcessorDisassemble(processor, addr, OP_DSA_UNCOOKED));
    sscanf(instruction,"%s %*s\n",instruction);
    
    unsigned int tipo = getInstructionType(instruction);
    
    if(tipo == LOAD){
        load[processorID]++;
    }
    else if(tipo == STORE){
        store[processorID]++;
    }
    else{
        others[processorID]++;
    }

    // if(66 == tipo){
    //     opMessage("I", "HARNESS", "ERRO: INSTRUÇÃO NÃO ENCONTRADA: %s", instruction);
    //     while(1){}
    // }
    // else{
    //     //                         BASE ADDRESS -  (INSTRUCTION TYPE OFFSET)
    //     //unsigned int counterAddress = 0x8FFFFFF8 - (tipo*4); // *4 porque é endereçado por palavra (32bits)
    //     char counterValue_8bit[4];
    //     //Reads the current count value
    //     //opProcessorRead(processor, counterAddress, &counterValue_8bit, 4, 1, True, OP_HOSTENDIAN_TARGET);
    //     unsigned int read_executedInstructions = vec2usi(counterValue_8bit);
    //     read_executedInstructions = read_executedInstructions + 1;

    //     //Stores the new count value
    //     counterValue_8bit[3] = ((read_executedInstructions) >> 24) & 0x000000FF;
    //     counterValue_8bit[2] = ((read_executedInstructions) >> 16) & 0x000000FF;
    //     counterValue_8bit[1] = ((read_executedInstructions) >> 8) & 0x000000FF;
    //     counterValue_8bit[0] = (read_executedInstructions) & 0x000000FF;
    //     //opProcessorWrite(processor, counterAddress, counterValue_8bit, 4, 1, True, OP_HOSTENDIAN_TARGET);
    // }

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

