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
#define INSTRUCTIONS_PER_TIME_SLICE 10000.0                                             //(INSTRUCTIONS_PER_SECOND*QUANTUM_TIME_SLICE)
#define QUANTUM_TIME_SLICE (double)(INSTRUCTIONS_PER_TIME_SLICE / INSTRUCTIONS_PER_SECOND)

// Address to save the instruction counters
#define LOAD_ADDR       0x8FFFFFF4u
#define STORE_ADDR      0x8FFFFFF0u
#define OTHERS_ADDR     0x8FFFFFECu

unsigned int load0 = 0;
unsigned int store0 = 0;
unsigned int others0 = 0;
unsigned int fetch0 = 0;
unsigned int load1 = 0;
unsigned int store1 = 0;
unsigned int others1 = 0;
unsigned int fetch1 = 0;
unsigned int load2 = 0;
unsigned int store2 = 0;
unsigned int others2 = 0;
unsigned int fetch2 = 0;
unsigned int load3 = 0;
unsigned int store3 = 0;
unsigned int others3 = 0;
unsigned int fetch3 = 0;
unsigned int load4 = 0;
unsigned int store4 = 0;
unsigned int others4 = 0;
unsigned int fetch4 = 0;
unsigned int load5 = 0;
unsigned int store5 = 0;
unsigned int others5 = 0;
unsigned int fetch5 = 0;
unsigned int load6 = 0;
unsigned int store6 = 0;
unsigned int others6 = 0;
unsigned int fetch6 = 0;
unsigned int load7 = 0;
unsigned int store7 = 0;
unsigned int others7 = 0;
unsigned int fetch7 = 0;
unsigned int load8 = 0;
unsigned int store8 = 0;
unsigned int others8 = 0;
unsigned int fetch8 = 0;

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

/*int getProcessorID(optProcessorP processor){
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
}*/

//unsigned int saiu_int = 0;
//unsigned int entrou_int = 0;

static OP_MONITOR_FN(FetchCallback0) { 
    char aux_8bits[4];
    opProcessorRead(processor, addr, &aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int instruction32 = vec2usi(aux_8bits);
    fetch0++;

    //opMessage("I", "FETCH", "PE%d- %s @ %x", 0, opProcessorDisassemble(processor, addr, OP_DSA_UNCOOKED), (unsigned int)addr);
    
    // https://www2.eecs.berkeley.edu/Pubs/TechRpts/2014/EECS-2014-54.pdf - pag 60
    if((instruction32 & 0x0000007F) == 0x00000003){         // checks if the opcode is equal to b0000011 (LOAD) 
        load0++;
    }
    else if((instruction32 & 0x0000007F) == 0x00000023){    // checks if the opcode is equal to b0100011 (STORE)
        store0++;
    }
    else{
        others0++;
    }

    if(fetch0 > 100){
        fetch0 = 0;

        opProcessorRead(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load0 = load0 + vec2usi(aux_8bits);
        aux_8bits[3] = (load0 >> 24) & 0x000000FF;
        aux_8bits[2] = (load0 >> 16) & 0x000000FF;
        aux_8bits[1] = (load0 >> 8) & 0x000000FF;
        aux_8bits[0] =  load0 & 0x000000FF;
        opProcessorWrite(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load0 = 0;

        opProcessorRead(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store0 = store0 + vec2usi(aux_8bits);
        aux_8bits[3] = (store0 >> 24) & 0x000000FF;
        aux_8bits[2] = (store0 >> 16) & 0x000000FF;
        aux_8bits[1] = (store0 >> 8) & 0x000000FF;
        aux_8bits[0] =  store0 & 0x000000FF;
        opProcessorWrite(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store0 = 0;

        opProcessorRead(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others0 = others0 + vec2usi(aux_8bits);
        aux_8bits[3] = (others0 >> 24) & 0x000000FF;
        aux_8bits[2] = (others0 >> 16) & 0x000000FF;
        aux_8bits[1] = (others0 >> 8) & 0x000000FF;
        aux_8bits[0] =  others0 & 0x000000FF;
        opProcessorWrite(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others0 = 0;
    }

    return;
}
static OP_MONITOR_FN(FetchCallback1) { 
    char aux_8bits[4];
    opProcessorRead(processor, addr, &aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int instruction32 = vec2usi(aux_8bits);
    fetch1++;

    //opMessage("I", "FETCH", "PE%d- %s @ %x", 1, opProcessorDisassemble(processor, addr, OP_DSA_UNCOOKED), (unsigned int)addr);
    
    // https://www2.eecs.berkeley.edu/Pubs/TechRpts/2014/EECS-2014-54.pdf - pag 60
    if((instruction32 & 0x0000007F) == 0x00000003){         // checks if the opcode is equal to b0000011 (LOAD) 
        load1++;
    }
    else if((instruction32 & 0x0000007F) == 0x00000023){    // checks if the opcode is equal to b0100011 (STORE)
        store1++;
    }
    else{
        others1++;
    }

    if(fetch1 > 100){
        fetch1 = 0;

        opProcessorRead(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load1 = load1 + vec2usi(aux_8bits);
        aux_8bits[3] = (load1 >> 24) & 0x000000FF;
        aux_8bits[2] = (load1 >> 16) & 0x000000FF;
        aux_8bits[1] = (load1 >> 8) & 0x000000FF;
        aux_8bits[0] =  load1 & 0x000000FF;
        opProcessorWrite(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load1 = 0;

        opProcessorRead(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store1 = store1 + vec2usi(aux_8bits);
        aux_8bits[3] = (store1 >> 24) & 0x000000FF;
        aux_8bits[2] = (store1 >> 16) & 0x000000FF;
        aux_8bits[1] = (store1 >> 8) & 0x000000FF;
        aux_8bits[0] =  store1 & 0x000000FF;
        opProcessorWrite(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store1 = 0;

        opProcessorRead(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others1 = others1 + vec2usi(aux_8bits);
        aux_8bits[3] = (others1 >> 24) & 0x000000FF;
        aux_8bits[2] = (others1 >> 16) & 0x000000FF;
        aux_8bits[1] = (others1 >> 8) & 0x000000FF;
        aux_8bits[0] =  others1 & 0x000000FF;
        opProcessorWrite(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others1 = 0;
    }

    return;
}
static OP_MONITOR_FN(FetchCallback2) { 
    char aux_8bits[4];
    opProcessorRead(processor, addr, &aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int instruction32 = vec2usi(aux_8bits);
    fetch2++;

    //opMessage("I", "FETCH", "PE%d- %s @ %x", 2, opProcessorDisassemble(processor, addr, OP_DSA_UNCOOKED), (unsigned int)addr);
    
    // https://www2.eecs.berkeley.edu/Pubs/TechRpts/2014/EECS-2014-54.pdf - pag 60
    if((instruction32 & 0x0000007F) == 0x00000003){         // checks if the opcode is equal to b0000011 (LOAD) 
        load2++;
    }
    else if((instruction32 & 0x0000007F) == 0x00000023){    // checks if the opcode is equal to b0100011 (STORE)
        store2++;
    }
    else{
        others2++;
    }

    if(fetch2 > 100){
        fetch2 = 0;

        opProcessorRead(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load2 = load2 + vec2usi(aux_8bits);
        aux_8bits[3] = (load2 >> 24) & 0x000000FF;
        aux_8bits[2] = (load2 >> 16) & 0x000000FF;
        aux_8bits[1] = (load2 >> 8) & 0x000000FF;
        aux_8bits[0] =  load2 & 0x000000FF;
        opProcessorWrite(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load2 = 0;

        opProcessorRead(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store2 = store2 + vec2usi(aux_8bits);
        aux_8bits[3] = (store2 >> 24) & 0x000000FF;
        aux_8bits[2] = (store2 >> 16) & 0x000000FF;
        aux_8bits[1] = (store2 >> 8) & 0x000000FF;
        aux_8bits[0] =  store2 & 0x000000FF;
        opProcessorWrite(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store2 = 0;

        opProcessorRead(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others2 = others2 + vec2usi(aux_8bits);
        aux_8bits[3] = (others2 >> 24) & 0x000000FF;
        aux_8bits[2] = (others2 >> 16) & 0x000000FF;
        aux_8bits[1] = (others2 >> 8) & 0x000000FF;
        aux_8bits[0] =  others2 & 0x000000FF;
        opProcessorWrite(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others2 = 0;
    }

    return;
}
static OP_MONITOR_FN(FetchCallback3) { 
    char aux_8bits[4];
    opProcessorRead(processor, addr, &aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int instruction32 = vec2usi(aux_8bits);
    fetch3++;

    //opMessage("I", "FETCH", "PE%d- %s @ %x", 3, opProcessorDisassemble(processor, addr, OP_DSA_UNCOOKED), (unsigned int)addr);
    
    // https://www2.eecs.berkeley.edu/Pubs/TechRpts/2014/EECS-2014-54.pdf - pag 60
    if((instruction32 & 0x0000007F) == 0x00000003){         // checks if the opcode is equal to b0000011 (LOAD) 
        load3++;
    }
    else if((instruction32 & 0x0000007F) == 0x00000023){    // checks if the opcode is equal to b0100011 (STORE)
        store3++;
    }
    else{
        others3++;
    }

    if(fetch3 > 100){
        fetch3 = 0;

        opProcessorRead(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load3 = load3 + vec2usi(aux_8bits);
        aux_8bits[3] = (load3 >> 24) & 0x000000FF;
        aux_8bits[2] = (load3 >> 16) & 0x000000FF;
        aux_8bits[1] = (load3 >> 8) & 0x000000FF;
        aux_8bits[0] =  load3 & 0x000000FF;
        opProcessorWrite(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load3 = 0;

        opProcessorRead(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store3 = store3 + vec2usi(aux_8bits);
        aux_8bits[3] = (store3 >> 24) & 0x000000FF;
        aux_8bits[2] = (store3 >> 16) & 0x000000FF;
        aux_8bits[1] = (store3 >> 8) & 0x000000FF;
        aux_8bits[0] =  store3 & 0x000000FF;
        opProcessorWrite(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store3 = 0;

        opProcessorRead(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others3 = others3 + vec2usi(aux_8bits);
        aux_8bits[3] = (others3 >> 24) & 0x000000FF;
        aux_8bits[2] = (others3 >> 16) & 0x000000FF;
        aux_8bits[1] = (others3 >> 8) & 0x000000FF;
        aux_8bits[0] =  others3 & 0x000000FF;
        opProcessorWrite(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others3 = 0;
    }

    return;
}
static OP_MONITOR_FN(FetchCallback4) { 
    char aux_8bits[4];
    opProcessorRead(processor, addr, &aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int instruction32 = vec2usi(aux_8bits);
    fetch4++;

    //opMessage("I", "FETCH", "PE%d- %s @ %x", 4, opProcessorDisassemble(processor, addr, OP_DSA_UNCOOKED), (unsigned int)addr);
    
    // https://www2.eecs.berkeley.edu/Pubs/TechRpts/2014/EECS-2014-54.pdf - pag 60
    if((instruction32 & 0x0000007F) == 0x00000003){         // checks if the opcode is equal to b0000011 (LOAD) 
        load4++;
    }
    else if((instruction32 & 0x0000007F) == 0x00000023){    // checks if the opcode is equal to b0100011 (STORE)
        store4++;
    }
    else{
        others4++;
    }

    if(fetch4 > 100){
        fetch4 = 0;

        opProcessorRead(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load4 = load4 + vec2usi(aux_8bits);
        aux_8bits[3] = (load4 >> 24) & 0x000000FF;
        aux_8bits[2] = (load4 >> 16) & 0x000000FF;
        aux_8bits[1] = (load4 >> 8) & 0x000000FF;
        aux_8bits[0] =  load4 & 0x000000FF;
        opProcessorWrite(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load4 = 0;

        opProcessorRead(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store4 = store4 + vec2usi(aux_8bits);
        aux_8bits[3] = (store4 >> 24) & 0x000000FF;
        aux_8bits[2] = (store4 >> 16) & 0x000000FF;
        aux_8bits[1] = (store4 >> 8) & 0x000000FF;
        aux_8bits[0] =  store4 & 0x000000FF;
        opProcessorWrite(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store4 = 0;

        opProcessorRead(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others4 = others4 + vec2usi(aux_8bits);
        aux_8bits[3] = (others4 >> 24) & 0x000000FF;
        aux_8bits[2] = (others4 >> 16) & 0x000000FF;
        aux_8bits[1] = (others4 >> 8) & 0x000000FF;
        aux_8bits[0] =  others4 & 0x000000FF;
        opProcessorWrite(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others4 = 0;
    }

    return;
}
static OP_MONITOR_FN(FetchCallback5) { 
    char aux_8bits[4];
    opProcessorRead(processor, addr, &aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int instruction32 = vec2usi(aux_8bits);
    fetch5++;

    //opMessage("I", "FETCH", "PE%d- %s @ %x", 5, opProcessorDisassemble(processor, addr, OP_DSA_UNCOOKED), (unsigned int)addr);
    
    // https://www2.eecs.berkeley.edu/Pubs/TechRpts/2014/EECS-2014-54.pdf - pag 60
    if((instruction32 & 0x0000007F) == 0x00000003){         // checks if the opcode is equal to b0000011 (LOAD) 
        load5++;
    }
    else if((instruction32 & 0x0000007F) == 0x00000023){    // checks if the opcode is equal to b0100011 (STORE)
        store5++;
    }
    else{
        others5++;
    }

    if(fetch5 > 100){
        fetch5 = 0;

        opProcessorRead(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load5 = load5 + vec2usi(aux_8bits);
        aux_8bits[3] = (load5 >> 24) & 0x000000FF;
        aux_8bits[2] = (load5 >> 16) & 0x000000FF;
        aux_8bits[1] = (load5 >> 8) & 0x000000FF;
        aux_8bits[0] =  load5 & 0x000000FF;
        opProcessorWrite(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load5 = 0;

        opProcessorRead(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store5 = store5 + vec2usi(aux_8bits);
        aux_8bits[3] = (store5 >> 24) & 0x000000FF;
        aux_8bits[2] = (store5 >> 16) & 0x000000FF;
        aux_8bits[1] = (store5 >> 8) & 0x000000FF;
        aux_8bits[0] =  store5 & 0x000000FF;
        opProcessorWrite(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store5 = 0;

        opProcessorRead(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others5 = others5 + vec2usi(aux_8bits);
        aux_8bits[3] = (others5 >> 24) & 0x000000FF;
        aux_8bits[2] = (others5 >> 16) & 0x000000FF;
        aux_8bits[1] = (others5 >> 8) & 0x000000FF;
        aux_8bits[0] =  others5 & 0x000000FF;
        opProcessorWrite(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others5 = 0;
    }

    return;
}
static OP_MONITOR_FN(FetchCallback6) { 
    char aux_8bits[4];
    opProcessorRead(processor, addr, &aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int instruction32 = vec2usi(aux_8bits);
    fetch6++;

    //opMessage("I", "FETCH", "PE%d- %s @ %x", 6, opProcessorDisassemble(processor, addr, OP_DSA_UNCOOKED), (unsigned int)addr);
    
    // https://www2.eecs.berkeley.edu/Pubs/TechRpts/2014/EECS-2014-54.pdf - pag 60
    if((instruction32 & 0x0000007F) == 0x00000003){         // checks if the opcode is equal to b0000011 (LOAD) 
        load6++;
    }
    else if((instruction32 & 0x0000007F) == 0x00000023){    // checks if the opcode is equal to b0100011 (STORE)
        store6++;
    }
    else{
        others6++;
    }

    if(fetch6 > 100){
        fetch6 = 0;

        opProcessorRead(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load6 = load6 + vec2usi(aux_8bits);
        aux_8bits[3] = (load6 >> 24) & 0x000000FF;
        aux_8bits[2] = (load6 >> 16) & 0x000000FF;
        aux_8bits[1] = (load6 >> 8) & 0x000000FF;
        aux_8bits[0] =  load6 & 0x000000FF;
        opProcessorWrite(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load6 = 0;

        opProcessorRead(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store6 = store6 + vec2usi(aux_8bits);
        aux_8bits[3] = (store6 >> 24) & 0x000000FF;
        aux_8bits[2] = (store6 >> 16) & 0x000000FF;
        aux_8bits[1] = (store6 >> 8) & 0x000000FF;
        aux_8bits[0] =  store6 & 0x000000FF;
        opProcessorWrite(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store6 = 0;

        opProcessorRead(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others6 = others6 + vec2usi(aux_8bits);
        aux_8bits[3] = (others6 >> 24) & 0x000000FF;
        aux_8bits[2] = (others6 >> 16) & 0x000000FF;
        aux_8bits[1] = (others6 >> 8) & 0x000000FF;
        aux_8bits[0] =  others6 & 0x000000FF;
        opProcessorWrite(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others6 = 0;
    }

    return;
}
static OP_MONITOR_FN(FetchCallback7) { 
    char aux_8bits[4];
    opProcessorRead(processor, addr, &aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int instruction32 = vec2usi(aux_8bits);
    fetch7++;

    //opMessage("I", "FETCH", "PE%d- %s @ %x", 7, opProcessorDisassemble(processor, addr, OP_DSA_UNCOOKED), (unsigned int)addr);
    
    // https://www2.eecs.berkeley.edu/Pubs/TechRpts/2014/EECS-2014-54.pdf - pag 60
    if((instruction32 & 0x0000007F) == 0x00000003){         // checks if the opcode is equal to b0000011 (LOAD) 
        load7++;
    }
    else if((instruction32 & 0x0000007F) == 0x00000023){    // checks if the opcode is equal to b0100011 (STORE)
        store7++;
    }
    else{
        others7++;
    }

    if(fetch7 > 100){
        fetch7 = 0;

        opProcessorRead(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load7 = load7 + vec2usi(aux_8bits);
        aux_8bits[3] = (load7 >> 24) & 0x000000FF;
        aux_8bits[2] = (load7 >> 16) & 0x000000FF;
        aux_8bits[1] = (load7 >> 8) & 0x000000FF;
        aux_8bits[0] =  load7 & 0x000000FF;
        opProcessorWrite(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load7 = 0;

        opProcessorRead(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store7 = store7 + vec2usi(aux_8bits);
        aux_8bits[3] = (store7 >> 24) & 0x000000FF;
        aux_8bits[2] = (store7 >> 16) & 0x000000FF;
        aux_8bits[1] = (store7 >> 8) & 0x000000FF;
        aux_8bits[0] =  store7 & 0x000000FF;
        opProcessorWrite(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store7 = 0;

        opProcessorRead(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others7 = others7 + vec2usi(aux_8bits);
        aux_8bits[3] = (others7 >> 24) & 0x000000FF;
        aux_8bits[2] = (others7 >> 16) & 0x000000FF;
        aux_8bits[1] = (others7 >> 8) & 0x000000FF;
        aux_8bits[0] =  others7 & 0x000000FF;
        opProcessorWrite(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others7 = 0;
    }

    return;
}
static OP_MONITOR_FN(FetchCallback8) { 
    char aux_8bits[4];
    opProcessorRead(processor, addr, &aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int instruction32 = vec2usi(aux_8bits);
    fetch8++;

    //opMessage("I", "FETCH", "PE%d- %s @ %x", 8, opProcessorDisassemble(processor, addr, OP_DSA_UNCOOKED), (unsigned int)addr);
    
    // https://www2.eecs.berkeley.edu/Pubs/TechRpts/2014/EECS-2014-54.pdf - pag 60
    if((instruction32 & 0x0000007F) == 0x00000003){         // checks if the opcode is equal to b0000011 (LOAD) 
        load8++;
    }
    else if((instruction32 & 0x0000007F) == 0x00000023){    // checks if the opcode is equal to b0100011 (STORE)
        store8++;
    }
    else{
        others8++;
    }

    if(fetch8 > 100){
        fetch8 = 0;

        opProcessorRead(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load8 = load8 + vec2usi(aux_8bits);
        aux_8bits[3] = (load8 >> 24) & 0x000000FF;
        aux_8bits[2] = (load8 >> 16) & 0x000000FF;
        aux_8bits[1] = (load8 >> 8) & 0x000000FF;
        aux_8bits[0] =  load8 & 0x000000FF;
        opProcessorWrite(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load8 = 0;

        opProcessorRead(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store8 = store8 + vec2usi(aux_8bits);
        aux_8bits[3] = (store8 >> 24) & 0x000000FF;
        aux_8bits[2] = (store8 >> 16) & 0x000000FF;
        aux_8bits[1] = (store8 >> 8) & 0x000000FF;
        aux_8bits[0] =  store8 & 0x000000FF;
        opProcessorWrite(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store8 = 0;

        opProcessorRead(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others8 = others8 + vec2usi(aux_8bits);
        aux_8bits[3] = (others8 >> 24) & 0x000000FF;
        aux_8bits[2] = (others8 >> 16) & 0x000000FF;
        aux_8bits[1] = (others8 >> 8) & 0x000000FF;
        aux_8bits[0] =  others8 & 0x000000FF;
        opProcessorWrite(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others8 = 0;
    }

    return;
}

static OP_MONITOR_FN(FinishCallback){
    opMessage("I", "HARNESS", " >>> SIMULAÇÃO FINALIZADA COM SUCESSO!");
    opProcessorFinish(processor, 0);
    return;
}

static OP_MONITOR_FN(FreqCallback0){

    // reads the frequency scale, provided by the system
    char readFreq_8bit[4];
    opProcessorRead(processor, 0x8FFFFFF8, &readFreq_8bit, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int readFreq_32bit = vec2usi(readFreq_8bit);

    // translates from fixed point to float
    float freqScale = 100 - (float)((float)readFreq_32bit / 10);
    if(freqScale > 91.0){ // this should never occour, but this is a security if to ensure that the processor does never stall by going to 0% of its frequency
        freqScale = 90;
    }
    opMessage("I", "HARNESS", " >>> PE 0 changing to %.1f%% of its nominal frequency -- (%.0fMHz)", 100-freqScale, 10*(100-freqScale));
    opProcessorDerate(processor, freqScale);
    return;
}

static OP_MONITOR_FN(FreqCallback1){

    // reads the frequency scale, provided by the system
    char readFreq_8bit[4];
    opProcessorRead(processor, 0x8FFFFFF8, &readFreq_8bit, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int readFreq_32bit = vec2usi(readFreq_8bit);

    // translates from fixed point to float
    float freqScale = 100 - (float)((float)readFreq_32bit / 10);
    if(freqScale > 91.0){ // this should never occour, but this is a security if to ensure that the processor does never stall by going to 0% of its frequency
        freqScale = 90;
    }
    opMessage("I", "HARNESS", " >>> PE 1 changing to %.1f%% of its nominal frequency -- (%.0fMHz)", 100-freqScale, 10*(100-freqScale));
    opProcessorDerate(processor, freqScale);
    return;
}

static OP_MONITOR_FN(FreqCallback2){

    // reads the frequency scale, provided by the system
    char readFreq_8bit[4];
    opProcessorRead(processor, 0x8FFFFFF8, &readFreq_8bit, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int readFreq_32bit = vec2usi(readFreq_8bit);

    // translates from fixed point to float
    float freqScale = 100 - (float)((float)readFreq_32bit / 10);
    if(freqScale > 91.0){ // this should never occour, but this is a security if to ensure that the processor does never stall by going to 0% of its frequency
        freqScale = 90;
    }
    opMessage("I", "HARNESS", " >>> PE 2 changing to %.1f%% of its nominal frequency -- (%.0fMHz)", 100-freqScale, 10*(100-freqScale));
    opProcessorDerate(processor, freqScale);
    return;
}

static OP_MONITOR_FN(FreqCallback3){

    // reads the frequency scale, provided by the system
    char readFreq_8bit[4];
    opProcessorRead(processor, 0x8FFFFFF8, &readFreq_8bit, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int readFreq_32bit = vec2usi(readFreq_8bit);

    // translates from fixed point to float
    float freqScale = 100 - (float)((float)readFreq_32bit / 10);
    if(freqScale > 91.0){ // this should never occour, but this is a security if to ensure that the processor does never stall by going to 0% of its frequency
        freqScale = 90;
    }
    opMessage("I", "HARNESS", " >>> PE 3 changing to %.1f%% of its nominal frequency -- (%.0fMHz)", 100-freqScale, 10*(100-freqScale));
    opProcessorDerate(processor, freqScale);
    return;
}

static OP_MONITOR_FN(FreqCallback4){

    // reads the frequency scale, provided by the system
    char readFreq_8bit[4];
    opProcessorRead(processor, 0x8FFFFFF8, &readFreq_8bit, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int readFreq_32bit = vec2usi(readFreq_8bit);

    // translates from fixed point to float
    float freqScale = 100 - (float)((float)readFreq_32bit / 10);
    if(freqScale > 91.0){ // this should never occour, but this is a security if to ensure that the processor does never stall by going to 0% of its frequency
        freqScale = 90;
    }
    opMessage("I", "HARNESS", " >>> PE 4 changing to %.1f%% of its nominal frequency -- (%.0fMHz)", 100-freqScale, 10*(100-freqScale));
    opProcessorDerate(processor, freqScale);
    return;
}

static OP_MONITOR_FN(FreqCallback5){

    // reads the frequency scale, provided by the system
    char readFreq_8bit[4];
    opProcessorRead(processor, 0x8FFFFFF8, &readFreq_8bit, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int readFreq_32bit = vec2usi(readFreq_8bit);

    // translates from fixed point to float
    float freqScale = 100 - (float)((float)readFreq_32bit / 10);
    if(freqScale > 91.0){ // this should never occour, but this is a security if to ensure that the processor does never stall by going to 0% of its frequency
        freqScale = 90;
    }
    opMessage("I", "HARNESS", " >>> PE 5 changing to %.1f%% of its nominal frequency -- (%.0fMHz)", 100-freqScale, 10*(100-freqScale));
    opProcessorDerate(processor, freqScale);
    return;
}

static OP_MONITOR_FN(FreqCallback6){

    // reads the frequency scale, provided by the system
    char readFreq_8bit[4];
    opProcessorRead(processor, 0x8FFFFFF8, &readFreq_8bit, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int readFreq_32bit = vec2usi(readFreq_8bit);

    // translates from fixed point to float
    float freqScale = 100 - (float)((float)readFreq_32bit / 10);
    if(freqScale > 91.0){ // this should never occour, but this is a security if to ensure that the processor does never stall by going to 0% of its frequency
        freqScale = 90;
    }
    opMessage("I", "HARNESS", " >>> PE 6 changing to %.1f%% of its nominal frequency -- (%.0fMHz)", 100-freqScale, 10*(100-freqScale));
    opProcessorDerate(processor, freqScale);
    return;
}

static OP_MONITOR_FN(FreqCallback7){

    // reads the frequency scale, provided by the system
    char readFreq_8bit[4];
    opProcessorRead(processor, 0x8FFFFFF8, &readFreq_8bit, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int readFreq_32bit = vec2usi(readFreq_8bit);

    // translates from fixed point to float
    float freqScale = 100 - (float)((float)readFreq_32bit / 10);
    if(freqScale > 91.0){ // this should never occour, but this is a security if to ensure that the processor does never stall by going to 0% of its frequency
        freqScale = 90;
    }
    opMessage("I", "HARNESS", " >>> PE 7 changing to %.1f%% of its nominal frequency -- (%.0fMHz)", 100-freqScale, 10*(100-freqScale));
    opProcessorDerate(processor, freqScale);
    return;
}

static OP_MONITOR_FN(FreqCallback8){

    // reads the frequency scale, provided by the system
    char readFreq_8bit[4];
    opProcessorRead(processor, 0x8FFFFFF8, &readFreq_8bit, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int readFreq_32bit = vec2usi(readFreq_8bit);

    // translates from fixed point to float
    float freqScale = 100 - (float)((float)readFreq_32bit / 10);
    if(freqScale > 91.0){ // this should never occour, but this is a security if to ensure that the processor does never stall by going to 0% of its frequency
        freqScale = 90;
    }
    opMessage("I", "HARNESS", " >>> PE 8 changing to %.1f%% of its nominal frequency -- (%.0fMHz)", 100-freqScale, 10*(100-freqScale));
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
            opMessage("I", "HARNESS INFO", "	 > MONITOR DE FINALIZAÇÃO ADICIONADO!");
            opProcessorFetchMonitorAdd(proc, 0x80000e04, 0x80000e07, FinishCallback, "finish");
        }
        
        switch(runningPE){
            case 0:
                opProcessorWriteMonitorAdd(proc, 0x8FFFFFF8, 0x8FFFFFFB, FreqCallback0, "frequency0");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FREQUENCIA 0 ADICIONADO!");
                opProcessorFetchMonitorAdd(proc, 0x80000000, 0x8fffffff, FetchCallback0, "fetch0");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FETCH 0 ADICIONADO!");
                break;

            case 1:
                opProcessorWriteMonitorAdd(proc, 0x8FFFFFF8, 0x8FFFFFFB, FreqCallback1, "frequency1");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FREQUENCIA 1 ADICIONADO!");
                opProcessorFetchMonitorAdd(proc, 0x80000000, 0x8fffffff, FetchCallback1, "fetch1");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FETCH 1 ADICIONADO!");
                break;

            case 2:
                opProcessorWriteMonitorAdd(proc, 0x8FFFFFF8, 0x8FFFFFFB, FreqCallback2, "frequency2");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FREQUENCIA 2 ADICIONADO!");
                opProcessorFetchMonitorAdd(proc, 0x80000000, 0x8fffffff, FetchCallback2, "fetch2");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FETCH 2 ADICIONADO!");
                break;

            case 3:
                opProcessorWriteMonitorAdd(proc, 0x8FFFFFF8, 0x8FFFFFFB, FreqCallback3, "frequency3");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FREQUENCIA 3 ADICIONADO!");
                opProcessorFetchMonitorAdd(proc, 0x80000000, 0x8fffffff, FetchCallback3, "fetch3");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FETCH 3 ADICIONADO!");
                break;

            case 4:
                opProcessorWriteMonitorAdd(proc, 0x8FFFFFF8, 0x8FFFFFFB, FreqCallback4, "frequency4");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FREQUENCIA 4 ADICIONADO!");
                opProcessorFetchMonitorAdd(proc, 0x80000000, 0x8fffffff, FetchCallback4, "fetch4");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FETCH 4 ADICIONADO!");
                break;

            case 5:
                opProcessorWriteMonitorAdd(proc, 0x8FFFFFF8, 0x8FFFFFFB, FreqCallback5, "frequency5");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FREQUENCIA 5 ADICIONADO!");
                opProcessorFetchMonitorAdd(proc, 0x80000000, 0x8fffffff, FetchCallback5, "fetch5");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FETCH 5 ADICIONADO!");
                break;

            case 6:
                opProcessorWriteMonitorAdd(proc, 0x8FFFFFF8, 0x8FFFFFFB, FreqCallback6, "frequency6");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FREQUENCIA 6 ADICIONADO!");
                opProcessorFetchMonitorAdd(proc, 0x80000000, 0x8fffffff, FetchCallback6, "fetch6");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FETCH 6 ADICIONADO!");
                break;

            case 7:
                opProcessorWriteMonitorAdd(proc, 0x8FFFFFF8, 0x8FFFFFFB, FreqCallback7, "frequency7");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FREQUENCIA 7 ADICIONADO!");
                opProcessorFetchMonitorAdd(proc, 0x80000000, 0x8fffffff, FetchCallback7, "fetch7");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FETCH 7 ADICIONADO!");
                break;

            case 8:
                opProcessorWriteMonitorAdd(proc, 0x8FFFFFF8, 0x8FFFFFFB, FreqCallback8, "frequency8");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FREQUENCIA 8 ADICIONADO!");
                opProcessorFetchMonitorAdd(proc, 0x80000000, 0x8fffffff, FetchCallback8, "fetch8");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FETCH 8 ADICIONADO!");
                break;

            default: 
                break;
        }
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
                opMessage("I", "HARNESS", "Processor %s icount %u stopped with unexpected reason 0x%x (%s)", opObjectName (stopProcessor), (Uns32)opProcessorICount(stopProcessor), sr, opStopReasonString(sr) );
                break;
        }
        
    }

    opSessionTerminate();

    return (opErrors() ? 1 : 0);    // set exit based upon any errors
}

