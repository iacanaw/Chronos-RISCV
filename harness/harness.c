#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "op/op.h"

#define MODULE_NAME "top"
#define MODULE_DIR "module"
#define MODULE_INSTANCE "u2"
#define N_PES 25

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
unsigned int load9 = 0;
unsigned int store9 = 0;
unsigned int others9 = 0;
unsigned int fetch9 = 0;
unsigned int load10 = 0;
unsigned int store10 = 0;
unsigned int others10 = 0;
unsigned int fetch10 = 0;
unsigned int load11 = 0;
unsigned int store11 = 0;
unsigned int others11 = 0;
unsigned int fetch11 = 0;
unsigned int load12 = 0;
unsigned int store12 = 0;
unsigned int others12 = 0;
unsigned int fetch12 = 0;
unsigned int load13 = 0;
unsigned int store13 = 0;
unsigned int others13 = 0;
unsigned int fetch13 = 0;
unsigned int load14 = 0;
unsigned int store14 = 0;
unsigned int others14 = 0;
unsigned int fetch14 = 0;
unsigned int load15 = 0;
unsigned int store15 = 0;
unsigned int others15 = 0;
unsigned int fetch15 = 0;
unsigned int load16 = 0;
unsigned int store16 = 0;
unsigned int others16 = 0;
unsigned int fetch16 = 0;
unsigned int load17 = 0;
unsigned int store17 = 0;
unsigned int others17 = 0;
unsigned int fetch17 = 0;
unsigned int load18 = 0;
unsigned int store18 = 0;
unsigned int others18 = 0;
unsigned int fetch18 = 0;
unsigned int load19 = 0;
unsigned int store19 = 0;
unsigned int others19 = 0;
unsigned int fetch19 = 0;
unsigned int load20 = 0;
unsigned int store20 = 0;
unsigned int others20 = 0;
unsigned int fetch20 = 0;
unsigned int load21 = 0;
unsigned int store21 = 0;
unsigned int others21 = 0;
unsigned int fetch21 = 0;
unsigned int load22 = 0;
unsigned int store22 = 0;
unsigned int others22 = 0;
unsigned int fetch22 = 0;
unsigned int load23 = 0;
unsigned int store23 = 0;
unsigned int others23 = 0;
unsigned int fetch23 = 0;
unsigned int load24 = 0;
unsigned int store24 = 0;
unsigned int others24 = 0;
unsigned int fetch24 = 0;

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

// unsigned int saiu_int = 0;
// unsigned int entrou_int = 0;

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
static OP_MONITOR_FN(FetchCallback9) { 
    char aux_8bits[4];
    opProcessorRead(processor, addr, &aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int instruction32 = vec2usi(aux_8bits);
    fetch9++;

    //opMessage("I", "FETCH", "PE%d- %s @ %x", 9, opProcessorDisassemble(processor, addr, OP_DSA_UNCOOKED), (unsigned int)addr);
    
    // https://www2.eecs.berkeley.edu/Pubs/TechRpts/2014/EECS-2014-54.pdf - pag 60
    if((instruction32 & 0x0000007F) == 0x00000003){         // checks if the opcode is equal to b0000011 (LOAD) 
        load9++;
    }
    else if((instruction32 & 0x0000007F) == 0x00000023){    // checks if the opcode is equal to b0100011 (STORE)
        store9++;
    }
    else{
        others9++;
    }

    if(fetch9 > 100){
        fetch9 = 0;

        opProcessorRead(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load9 = load9 + vec2usi(aux_8bits);
        aux_8bits[3] = (load9 >> 24) & 0x000000FF;
        aux_8bits[2] = (load9 >> 16) & 0x000000FF;
        aux_8bits[1] = (load9 >> 8) & 0x000000FF;
        aux_8bits[0] =  load9 & 0x000000FF;
        opProcessorWrite(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load9 = 0;

        opProcessorRead(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store9 = store9 + vec2usi(aux_8bits);
        aux_8bits[3] = (store9 >> 24) & 0x000000FF;
        aux_8bits[2] = (store9 >> 16) & 0x000000FF;
        aux_8bits[1] = (store9 >> 8) & 0x000000FF;
        aux_8bits[0] =  store9 & 0x000000FF;
        opProcessorWrite(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store9 = 0;

        opProcessorRead(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others9 = others9 + vec2usi(aux_8bits);
        aux_8bits[3] = (others9 >> 24) & 0x000000FF;
        aux_8bits[2] = (others9 >> 16) & 0x000000FF;
        aux_8bits[1] = (others9 >> 8) & 0x000000FF;
        aux_8bits[0] =  others9 & 0x000000FF;
        opProcessorWrite(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others9 = 0;
    }

    return;
}
static OP_MONITOR_FN(FetchCallback10) { 
    char aux_8bits[4];
    opProcessorRead(processor, addr, &aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int instruction32 = vec2usi(aux_8bits);
    fetch10++;

    //opMessage("I", "FETCH", "PE%d- %s @ %x", 10, opProcessorDisassemble(processor, addr, OP_DSA_UNCOOKED), (unsigned int)addr);
    
    // https://www2.eecs.berkeley.edu/Pubs/TechRpts/2014/EECS-2014-54.pdf - pag 60
    if((instruction32 & 0x0000007F) == 0x00000003){         // checks if the opcode is equal to b0000011 (LOAD) 
        load10++;
    }
    else if((instruction32 & 0x0000007F) == 0x00000023){    // checks if the opcode is equal to b0100011 (STORE)
        store10++;
    }
    else{
        others10++;
    }

    if(fetch10 > 100){
        fetch10 = 0;

        opProcessorRead(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load10 = load10 + vec2usi(aux_8bits);
        aux_8bits[3] = (load10 >> 24) & 0x000000FF;
        aux_8bits[2] = (load10 >> 16) & 0x000000FF;
        aux_8bits[1] = (load10 >> 8) & 0x000000FF;
        aux_8bits[0] =  load10 & 0x000000FF;
        opProcessorWrite(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load10 = 0;

        opProcessorRead(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store10 = store10 + vec2usi(aux_8bits);
        aux_8bits[3] = (store10 >> 24) & 0x000000FF;
        aux_8bits[2] = (store10 >> 16) & 0x000000FF;
        aux_8bits[1] = (store10 >> 8) & 0x000000FF;
        aux_8bits[0] =  store10 & 0x000000FF;
        opProcessorWrite(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store10 = 0;

        opProcessorRead(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others10 = others10 + vec2usi(aux_8bits);
        aux_8bits[3] = (others10 >> 24) & 0x000000FF;
        aux_8bits[2] = (others10 >> 16) & 0x000000FF;
        aux_8bits[1] = (others10 >> 8) & 0x000000FF;
        aux_8bits[0] =  others10 & 0x000000FF;
        opProcessorWrite(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others10 = 0;
    }

    return;
}
static OP_MONITOR_FN(FetchCallback11) { 
    char aux_8bits[4];
    opProcessorRead(processor, addr, &aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int instruction32 = vec2usi(aux_8bits);
    fetch11++;

    //opMessage("I", "FETCH", "PE%d- %s @ %x", 11, opProcessorDisassemble(processor, addr, OP_DSA_UNCOOKED), (unsigned int)addr);
    
    // https://www2.eecs.berkeley.edu/Pubs/TechRpts/2014/EECS-2014-54.pdf - pag 60
    if((instruction32 & 0x0000007F) == 0x00000003){         // checks if the opcode is equal to b0000011 (LOAD) 
        load11++;
    }
    else if((instruction32 & 0x0000007F) == 0x00000023){    // checks if the opcode is equal to b0100011 (STORE)
        store11++;
    }
    else{
        others11++;
    }

    if(fetch11 > 100){
        fetch11 = 0;

        opProcessorRead(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load11 = load11 + vec2usi(aux_8bits);
        aux_8bits[3] = (load11 >> 24) & 0x000000FF;
        aux_8bits[2] = (load11 >> 16) & 0x000000FF;
        aux_8bits[1] = (load11 >> 8) & 0x000000FF;
        aux_8bits[0] =  load11 & 0x000000FF;
        opProcessorWrite(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load11 = 0;

        opProcessorRead(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store11 = store11 + vec2usi(aux_8bits);
        aux_8bits[3] = (store11 >> 24) & 0x000000FF;
        aux_8bits[2] = (store11 >> 16) & 0x000000FF;
        aux_8bits[1] = (store11 >> 8) & 0x000000FF;
        aux_8bits[0] =  store11 & 0x000000FF;
        opProcessorWrite(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store11 = 0;

        opProcessorRead(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others11 = others11 + vec2usi(aux_8bits);
        aux_8bits[3] = (others11 >> 24) & 0x000000FF;
        aux_8bits[2] = (others11 >> 16) & 0x000000FF;
        aux_8bits[1] = (others11 >> 8) & 0x000000FF;
        aux_8bits[0] =  others11 & 0x000000FF;
        opProcessorWrite(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others11 = 0;
    }

    return;
}
static OP_MONITOR_FN(FetchCallback12) { 
    char aux_8bits[4];
    opProcessorRead(processor, addr, &aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int instruction32 = vec2usi(aux_8bits);
    fetch12++;

    //opMessage("I", "FETCH", "PE%d- %s @ %x", 12, opProcessorDisassemble(processor, addr, OP_DSA_UNCOOKED), (unsigned int)addr);
    
    // https://www2.eecs.berkeley.edu/Pubs/TechRpts/2014/EECS-2014-54.pdf - pag 60
    if((instruction32 & 0x0000007F) == 0x00000003){         // checks if the opcode is equal to b0000011 (LOAD) 
        load12++;
    }
    else if((instruction32 & 0x0000007F) == 0x00000023){    // checks if the opcode is equal to b0100011 (STORE)
        store12++;
    }
    else{
        others12++;
    }

    if(fetch12 > 100){
        fetch12 = 0;

        opProcessorRead(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load12 = load12 + vec2usi(aux_8bits);
        aux_8bits[3] = (load12 >> 24) & 0x000000FF;
        aux_8bits[2] = (load12 >> 16) & 0x000000FF;
        aux_8bits[1] = (load12 >> 8) & 0x000000FF;
        aux_8bits[0] =  load12 & 0x000000FF;
        opProcessorWrite(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load12 = 0;

        opProcessorRead(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store12 = store12 + vec2usi(aux_8bits);
        aux_8bits[3] = (store12 >> 24) & 0x000000FF;
        aux_8bits[2] = (store12 >> 16) & 0x000000FF;
        aux_8bits[1] = (store12 >> 8) & 0x000000FF;
        aux_8bits[0] =  store12 & 0x000000FF;
        opProcessorWrite(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store12 = 0;

        opProcessorRead(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others12 = others12 + vec2usi(aux_8bits);
        aux_8bits[3] = (others12 >> 24) & 0x000000FF;
        aux_8bits[2] = (others12 >> 16) & 0x000000FF;
        aux_8bits[1] = (others12 >> 8) & 0x000000FF;
        aux_8bits[0] =  others12 & 0x000000FF;
        opProcessorWrite(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others12 = 0;
    }

    return;
}
static OP_MONITOR_FN(FetchCallback13) { 
    char aux_8bits[4];
    opProcessorRead(processor, addr, &aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int instruction32 = vec2usi(aux_8bits);
    fetch13++;

    //opMessage("I", "FETCH", "PE%d- %s @ %x", 13, opProcessorDisassemble(processor, addr, OP_DSA_UNCOOKED), (unsigned int)addr);
    
    // https://www2.eecs.berkeley.edu/Pubs/TechRpts/2014/EECS-2014-54.pdf - pag 60
    if((instruction32 & 0x0000007F) == 0x00000003){         // checks if the opcode is equal to b0000011 (LOAD) 
        load13++;
    }
    else if((instruction32 & 0x0000007F) == 0x00000023){    // checks if the opcode is equal to b0100011 (STORE)
        store13++;
    }
    else{
        others13++;
    }

    if(fetch13 > 100){
        fetch13 = 0;

        opProcessorRead(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load13 = load13 + vec2usi(aux_8bits);
        aux_8bits[3] = (load13 >> 24) & 0x000000FF;
        aux_8bits[2] = (load13 >> 16) & 0x000000FF;
        aux_8bits[1] = (load13 >> 8) & 0x000000FF;
        aux_8bits[0] =  load13 & 0x000000FF;
        opProcessorWrite(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load13 = 0;

        opProcessorRead(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store13 = store13 + vec2usi(aux_8bits);
        aux_8bits[3] = (store13 >> 24) & 0x000000FF;
        aux_8bits[2] = (store13 >> 16) & 0x000000FF;
        aux_8bits[1] = (store13 >> 8) & 0x000000FF;
        aux_8bits[0] =  store13 & 0x000000FF;
        opProcessorWrite(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store13 = 0;

        opProcessorRead(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others13 = others13 + vec2usi(aux_8bits);
        aux_8bits[3] = (others13 >> 24) & 0x000000FF;
        aux_8bits[2] = (others13 >> 16) & 0x000000FF;
        aux_8bits[1] = (others13 >> 8) & 0x000000FF;
        aux_8bits[0] =  others13 & 0x000000FF;
        opProcessorWrite(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others13 = 0;
    }

    return;
}
static OP_MONITOR_FN(FetchCallback14) { 
    char aux_8bits[4];
    opProcessorRead(processor, addr, &aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int instruction32 = vec2usi(aux_8bits);
    fetch14++;

    //opMessage("I", "FETCH", "PE%d- %s @ %x", 14, opProcessorDisassemble(processor, addr, OP_DSA_UNCOOKED), (unsigned int)addr);
    
    // https://www2.eecs.berkeley.edu/Pubs/TechRpts/2014/EECS-2014-54.pdf - pag 60
    if((instruction32 & 0x0000007F) == 0x00000003){         // checks if the opcode is equal to b0000011 (LOAD) 
        load14++;
    }
    else if((instruction32 & 0x0000007F) == 0x00000023){    // checks if the opcode is equal to b0100011 (STORE)
        store14++;
    }
    else{
        others14++;
    }

    if(fetch14 > 100){
        fetch14 = 0;

        opProcessorRead(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load14 = load14 + vec2usi(aux_8bits);
        aux_8bits[3] = (load14 >> 24) & 0x000000FF;
        aux_8bits[2] = (load14 >> 16) & 0x000000FF;
        aux_8bits[1] = (load14 >> 8) & 0x000000FF;
        aux_8bits[0] =  load14 & 0x000000FF;
        opProcessorWrite(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load14 = 0;

        opProcessorRead(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store14 = store14 + vec2usi(aux_8bits);
        aux_8bits[3] = (store14 >> 24) & 0x000000FF;
        aux_8bits[2] = (store14 >> 16) & 0x000000FF;
        aux_8bits[1] = (store14 >> 8) & 0x000000FF;
        aux_8bits[0] =  store14 & 0x000000FF;
        opProcessorWrite(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store14 = 0;

        opProcessorRead(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others14 = others14 + vec2usi(aux_8bits);
        aux_8bits[3] = (others14 >> 24) & 0x000000FF;
        aux_8bits[2] = (others14 >> 16) & 0x000000FF;
        aux_8bits[1] = (others14 >> 8) & 0x000000FF;
        aux_8bits[0] =  others14 & 0x000000FF;
        opProcessorWrite(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others14 = 0;
    }

    return;
}
static OP_MONITOR_FN(FetchCallback15) { 
    char aux_8bits[4];
    opProcessorRead(processor, addr, &aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int instruction32 = vec2usi(aux_8bits);
    fetch15++;

    //opMessage("I", "FETCH", "PE%d- %s @ %x", 15, opProcessorDisassemble(processor, addr, OP_DSA_UNCOOKED), (unsigned int)addr);
    
    // https://www2.eecs.berkeley.edu/Pubs/TechRpts/2014/EECS-2014-54.pdf - pag 60
    if((instruction32 & 0x0000007F) == 0x00000003){         // checks if the opcode is equal to b0000011 (LOAD) 
        load15++;
    }
    else if((instruction32 & 0x0000007F) == 0x00000023){    // checks if the opcode is equal to b0100011 (STORE)
        store15++;
    }
    else{
        others15++;
    }

    if(fetch15 > 100){
        fetch15 = 0;

        opProcessorRead(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load15 = load15 + vec2usi(aux_8bits);
        aux_8bits[3] = (load15 >> 24) & 0x000000FF;
        aux_8bits[2] = (load15 >> 16) & 0x000000FF;
        aux_8bits[1] = (load15 >> 8) & 0x000000FF;
        aux_8bits[0] =  load15 & 0x000000FF;
        opProcessorWrite(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load15 = 0;

        opProcessorRead(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store15 = store15 + vec2usi(aux_8bits);
        aux_8bits[3] = (store15 >> 24) & 0x000000FF;
        aux_8bits[2] = (store15 >> 16) & 0x000000FF;
        aux_8bits[1] = (store15 >> 8) & 0x000000FF;
        aux_8bits[0] =  store15 & 0x000000FF;
        opProcessorWrite(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store15 = 0;

        opProcessorRead(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others15 = others15 + vec2usi(aux_8bits);
        aux_8bits[3] = (others15 >> 24) & 0x000000FF;
        aux_8bits[2] = (others15 >> 16) & 0x000000FF;
        aux_8bits[1] = (others15 >> 8) & 0x000000FF;
        aux_8bits[0] =  others15 & 0x000000FF;
        opProcessorWrite(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others15 = 0;
    }

    return;
}
static OP_MONITOR_FN(FetchCallback16) { 
    char aux_8bits[4];
    opProcessorRead(processor, addr, &aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int instruction32 = vec2usi(aux_8bits);
    fetch16++;

    //opMessage("I", "FETCH", "PE%d- %s @ %x", 16, opProcessorDisassemble(processor, addr, OP_DSA_UNCOOKED), (unsigned int)addr);
    
    // https://www2.eecs.berkeley.edu/Pubs/TechRpts/2014/EECS-2014-54.pdf - pag 60
    if((instruction32 & 0x0000007F) == 0x00000003){         // checks if the opcode is equal to b0000011 (LOAD) 
        load16++;
    }
    else if((instruction32 & 0x0000007F) == 0x00000023){    // checks if the opcode is equal to b0100011 (STORE)
        store16++;
    }
    else{
        others16++;
    }

    if(fetch16 > 100){
        fetch16 = 0;

        opProcessorRead(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load16 = load16 + vec2usi(aux_8bits);
        aux_8bits[3] = (load16 >> 24) & 0x000000FF;
        aux_8bits[2] = (load16 >> 16) & 0x000000FF;
        aux_8bits[1] = (load16 >> 8) & 0x000000FF;
        aux_8bits[0] =  load16 & 0x000000FF;
        opProcessorWrite(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load16 = 0;

        opProcessorRead(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store16 = store16 + vec2usi(aux_8bits);
        aux_8bits[3] = (store16 >> 24) & 0x000000FF;
        aux_8bits[2] = (store16 >> 16) & 0x000000FF;
        aux_8bits[1] = (store16 >> 8) & 0x000000FF;
        aux_8bits[0] =  store16 & 0x000000FF;
        opProcessorWrite(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store16 = 0;

        opProcessorRead(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others16 = others16 + vec2usi(aux_8bits);
        aux_8bits[3] = (others16 >> 24) & 0x000000FF;
        aux_8bits[2] = (others16 >> 16) & 0x000000FF;
        aux_8bits[1] = (others16 >> 8) & 0x000000FF;
        aux_8bits[0] =  others16 & 0x000000FF;
        opProcessorWrite(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others16 = 0;
    }

    return;
}
static OP_MONITOR_FN(FetchCallback17) { 
    char aux_8bits[4];
    opProcessorRead(processor, addr, &aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int instruction32 = vec2usi(aux_8bits);
    fetch17++;

    //opMessage("I", "FETCH", "PE%d- %s @ %x", 17, opProcessorDisassemble(processor, addr, OP_DSA_UNCOOKED), (unsigned int)addr);
    
    // https://www2.eecs.berkeley.edu/Pubs/TechRpts/2014/EECS-2014-54.pdf - pag 60
    if((instruction32 & 0x0000007F) == 0x00000003){         // checks if the opcode is equal to b0000011 (LOAD) 
        load17++;
    }
    else if((instruction32 & 0x0000007F) == 0x00000023){    // checks if the opcode is equal to b0100011 (STORE)
        store17++;
    }
    else{
        others17++;
    }

    if(fetch17 > 100){
        fetch17 = 0;

        opProcessorRead(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load17 = load17 + vec2usi(aux_8bits);
        aux_8bits[3] = (load17 >> 24) & 0x000000FF;
        aux_8bits[2] = (load17 >> 16) & 0x000000FF;
        aux_8bits[1] = (load17 >> 8) & 0x000000FF;
        aux_8bits[0] =  load17 & 0x000000FF;
        opProcessorWrite(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load17 = 0;

        opProcessorRead(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store17 = store17 + vec2usi(aux_8bits);
        aux_8bits[3] = (store17 >> 24) & 0x000000FF;
        aux_8bits[2] = (store17 >> 16) & 0x000000FF;
        aux_8bits[1] = (store17 >> 8) & 0x000000FF;
        aux_8bits[0] =  store17 & 0x000000FF;
        opProcessorWrite(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store17 = 0;

        opProcessorRead(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others17 = others17 + vec2usi(aux_8bits);
        aux_8bits[3] = (others17 >> 24) & 0x000000FF;
        aux_8bits[2] = (others17 >> 16) & 0x000000FF;
        aux_8bits[1] = (others17 >> 8) & 0x000000FF;
        aux_8bits[0] =  others17 & 0x000000FF;
        opProcessorWrite(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others17 = 0;
    }

    return;
}
static OP_MONITOR_FN(FetchCallback18) { 
    char aux_8bits[4];
    opProcessorRead(processor, addr, &aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int instruction32 = vec2usi(aux_8bits);
    fetch18++;

    //opMessage("I", "FETCH", "PE%d- %s @ %x", 18, opProcessorDisassemble(processor, addr, OP_DSA_UNCOOKED), (unsigned int)addr);
    
    // https://www2.eecs.berkeley.edu/Pubs/TechRpts/2014/EECS-2014-54.pdf - pag 60
    if((instruction32 & 0x0000007F) == 0x00000003){         // checks if the opcode is equal to b0000011 (LOAD) 
        load18++;
    }
    else if((instruction32 & 0x0000007F) == 0x00000023){    // checks if the opcode is equal to b0100011 (STORE)
        store18++;
    }
    else{
        others18++;
    }

    if(fetch18 > 100){
        fetch18 = 0;

        opProcessorRead(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load18 = load18 + vec2usi(aux_8bits);
        aux_8bits[3] = (load18 >> 24) & 0x000000FF;
        aux_8bits[2] = (load18 >> 16) & 0x000000FF;
        aux_8bits[1] = (load18 >> 8) & 0x000000FF;
        aux_8bits[0] =  load18 & 0x000000FF;
        opProcessorWrite(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load18 = 0;

        opProcessorRead(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store18 = store18 + vec2usi(aux_8bits);
        aux_8bits[3] = (store18 >> 24) & 0x000000FF;
        aux_8bits[2] = (store18 >> 16) & 0x000000FF;
        aux_8bits[1] = (store18 >> 8) & 0x000000FF;
        aux_8bits[0] =  store18 & 0x000000FF;
        opProcessorWrite(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store18 = 0;

        opProcessorRead(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others18 = others18 + vec2usi(aux_8bits);
        aux_8bits[3] = (others18 >> 24) & 0x000000FF;
        aux_8bits[2] = (others18 >> 16) & 0x000000FF;
        aux_8bits[1] = (others18 >> 8) & 0x000000FF;
        aux_8bits[0] =  others18 & 0x000000FF;
        opProcessorWrite(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others18 = 0;
    }

    return;
}
static OP_MONITOR_FN(FetchCallback19) { 
    char aux_8bits[4];
    opProcessorRead(processor, addr, &aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int instruction32 = vec2usi(aux_8bits);
    fetch19++;

    //opMessage("I", "FETCH", "PE%d- %s @ %x", 19, opProcessorDisassemble(processor, addr, OP_DSA_UNCOOKED), (unsigned int)addr);
    
    // https://www2.eecs.berkeley.edu/Pubs/TechRpts/2014/EECS-2014-54.pdf - pag 60
    if((instruction32 & 0x0000007F) == 0x00000003){         // checks if the opcode is equal to b0000011 (LOAD) 
        load19++;
    }
    else if((instruction32 & 0x0000007F) == 0x00000023){    // checks if the opcode is equal to b0100011 (STORE)
        store19++;
    }
    else{
        others19++;
    }

    if(fetch19 > 100){
        fetch19 = 0;

        opProcessorRead(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load19 = load19 + vec2usi(aux_8bits);
        aux_8bits[3] = (load19 >> 24) & 0x000000FF;
        aux_8bits[2] = (load19 >> 16) & 0x000000FF;
        aux_8bits[1] = (load19 >> 8) & 0x000000FF;
        aux_8bits[0] =  load19 & 0x000000FF;
        opProcessorWrite(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load19 = 0;

        opProcessorRead(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store19 = store19 + vec2usi(aux_8bits);
        aux_8bits[3] = (store19 >> 24) & 0x000000FF;
        aux_8bits[2] = (store19 >> 16) & 0x000000FF;
        aux_8bits[1] = (store19 >> 8) & 0x000000FF;
        aux_8bits[0] =  store19 & 0x000000FF;
        opProcessorWrite(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store19 = 0;

        opProcessorRead(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others19 = others19 + vec2usi(aux_8bits);
        aux_8bits[3] = (others19 >> 24) & 0x000000FF;
        aux_8bits[2] = (others19 >> 16) & 0x000000FF;
        aux_8bits[1] = (others19 >> 8) & 0x000000FF;
        aux_8bits[0] =  others19 & 0x000000FF;
        opProcessorWrite(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others19 = 0;
    }

    return;
}
static OP_MONITOR_FN(FetchCallback20) { 
    char aux_8bits[4];
    opProcessorRead(processor, addr, &aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int instruction32 = vec2usi(aux_8bits);
    fetch20++;

    //opMessage("I", "FETCH", "PE%d- %s @ %x", 20, opProcessorDisassemble(processor, addr, OP_DSA_UNCOOKED), (unsigned int)addr);
    
    // https://www2.eecs.berkeley.edu/Pubs/TechRpts/2014/EECS-2014-54.pdf - pag 60
    if((instruction32 & 0x0000007F) == 0x00000003){         // checks if the opcode is equal to b0000011 (LOAD) 
        load20++;
    }
    else if((instruction32 & 0x0000007F) == 0x00000023){    // checks if the opcode is equal to b0100011 (STORE)
        store20++;
    }
    else{
        others20++;
    }

    if(fetch20 > 100){
        fetch20 = 0;

        opProcessorRead(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load20 = load20 + vec2usi(aux_8bits);
        aux_8bits[3] = (load20 >> 24) & 0x000000FF;
        aux_8bits[2] = (load20 >> 16) & 0x000000FF;
        aux_8bits[1] = (load20 >> 8) & 0x000000FF;
        aux_8bits[0] =  load20 & 0x000000FF;
        opProcessorWrite(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load20 = 0;

        opProcessorRead(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store20 = store20 + vec2usi(aux_8bits);
        aux_8bits[3] = (store20 >> 24) & 0x000000FF;
        aux_8bits[2] = (store20 >> 16) & 0x000000FF;
        aux_8bits[1] = (store20 >> 8) & 0x000000FF;
        aux_8bits[0] =  store20 & 0x000000FF;
        opProcessorWrite(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store20 = 0;

        opProcessorRead(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others20 = others20 + vec2usi(aux_8bits);
        aux_8bits[3] = (others20 >> 24) & 0x000000FF;
        aux_8bits[2] = (others20 >> 16) & 0x000000FF;
        aux_8bits[1] = (others20 >> 8) & 0x000000FF;
        aux_8bits[0] =  others20 & 0x000000FF;
        opProcessorWrite(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others20 = 0;
    }

    return;
}
static OP_MONITOR_FN(FetchCallback21) { 
    char aux_8bits[4];
    opProcessorRead(processor, addr, &aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int instruction32 = vec2usi(aux_8bits);
    fetch21++;

    //opMessage("I", "FETCH", "PE%d- %s @ %x", 21, opProcessorDisassemble(processor, addr, OP_DSA_UNCOOKED), (unsigned int)addr);
    
    // https://www2.eecs.berkeley.edu/Pubs/TechRpts/2014/EECS-2014-54.pdf - pag 60
    if((instruction32 & 0x0000007F) == 0x00000003){         // checks if the opcode is equal to b0000011 (LOAD) 
        load21++;
    }
    else if((instruction32 & 0x0000007F) == 0x00000023){    // checks if the opcode is equal to b0100011 (STORE)
        store21++;
    }
    else{
        others21++;
    }

    if(fetch21 > 100){
        fetch21 = 0;

        opProcessorRead(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load21 = load21 + vec2usi(aux_8bits);
        aux_8bits[3] = (load21 >> 24) & 0x000000FF;
        aux_8bits[2] = (load21 >> 16) & 0x000000FF;
        aux_8bits[1] = (load21 >> 8) & 0x000000FF;
        aux_8bits[0] =  load21 & 0x000000FF;
        opProcessorWrite(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load21 = 0;

        opProcessorRead(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store21 = store21 + vec2usi(aux_8bits);
        aux_8bits[3] = (store21 >> 24) & 0x000000FF;
        aux_8bits[2] = (store21 >> 16) & 0x000000FF;
        aux_8bits[1] = (store21 >> 8) & 0x000000FF;
        aux_8bits[0] =  store21 & 0x000000FF;
        opProcessorWrite(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store21 = 0;

        opProcessorRead(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others21 = others21 + vec2usi(aux_8bits);
        aux_8bits[3] = (others21 >> 24) & 0x000000FF;
        aux_8bits[2] = (others21 >> 16) & 0x000000FF;
        aux_8bits[1] = (others21 >> 8) & 0x000000FF;
        aux_8bits[0] =  others21 & 0x000000FF;
        opProcessorWrite(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others21 = 0;
    }

    return;
}
static OP_MONITOR_FN(FetchCallback22) { 
    char aux_8bits[4];
    opProcessorRead(processor, addr, &aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int instruction32 = vec2usi(aux_8bits);
    fetch22++;

    //opMessage("I", "FETCH", "PE%d- %s @ %x", 22, opProcessorDisassemble(processor, addr, OP_DSA_UNCOOKED), (unsigned int)addr);
    
    // https://www2.eecs.berkeley.edu/Pubs/TechRpts/2014/EECS-2014-54.pdf - pag 60
    if((instruction32 & 0x0000007F) == 0x00000003){         // checks if the opcode is equal to b0000011 (LOAD) 
        load22++;
    }
    else if((instruction32 & 0x0000007F) == 0x00000023){    // checks if the opcode is equal to b0100011 (STORE)
        store22++;
    }
    else{
        others22++;
    }

    if(fetch22 > 100){
        fetch22 = 0;

        opProcessorRead(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load22 = load22 + vec2usi(aux_8bits);
        aux_8bits[3] = (load22 >> 24) & 0x000000FF;
        aux_8bits[2] = (load22 >> 16) & 0x000000FF;
        aux_8bits[1] = (load22 >> 8) & 0x000000FF;
        aux_8bits[0] =  load22 & 0x000000FF;
        opProcessorWrite(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load22 = 0;

        opProcessorRead(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store22 = store22 + vec2usi(aux_8bits);
        aux_8bits[3] = (store22 >> 24) & 0x000000FF;
        aux_8bits[2] = (store22 >> 16) & 0x000000FF;
        aux_8bits[1] = (store22 >> 8) & 0x000000FF;
        aux_8bits[0] =  store22 & 0x000000FF;
        opProcessorWrite(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store22 = 0;

        opProcessorRead(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others22 = others22 + vec2usi(aux_8bits);
        aux_8bits[3] = (others22 >> 24) & 0x000000FF;
        aux_8bits[2] = (others22 >> 16) & 0x000000FF;
        aux_8bits[1] = (others22 >> 8) & 0x000000FF;
        aux_8bits[0] =  others22 & 0x000000FF;
        opProcessorWrite(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others22 = 0;
    }

    return;
}
static OP_MONITOR_FN(FetchCallback23) { 
    char aux_8bits[4];
    opProcessorRead(processor, addr, &aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int instruction32 = vec2usi(aux_8bits);
    fetch23++;

    //opMessage("I", "FETCH", "PE%d- %s @ %x", 23, opProcessorDisassemble(processor, addr, OP_DSA_UNCOOKED), (unsigned int)addr);
    
    // https://www2.eecs.berkeley.edu/Pubs/TechRpts/2014/EECS-2014-54.pdf - pag 60
    if((instruction32 & 0x0000007F) == 0x00000003){         // checks if the opcode is equal to b0000011 (LOAD) 
        load23++;
    }
    else if((instruction32 & 0x0000007F) == 0x00000023){    // checks if the opcode is equal to b0100011 (STORE)
        store23++;
    }
    else{
        others23++;
    }

    if(fetch23 > 100){
        fetch23 = 0;

        opProcessorRead(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load23 = load23 + vec2usi(aux_8bits);
        aux_8bits[3] = (load23 >> 24) & 0x000000FF;
        aux_8bits[2] = (load23 >> 16) & 0x000000FF;
        aux_8bits[1] = (load23 >> 8) & 0x000000FF;
        aux_8bits[0] =  load23 & 0x000000FF;
        opProcessorWrite(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load23 = 0;

        opProcessorRead(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store23 = store23 + vec2usi(aux_8bits);
        aux_8bits[3] = (store23 >> 24) & 0x000000FF;
        aux_8bits[2] = (store23 >> 16) & 0x000000FF;
        aux_8bits[1] = (store23 >> 8) & 0x000000FF;
        aux_8bits[0] =  store23 & 0x000000FF;
        opProcessorWrite(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store23 = 0;

        opProcessorRead(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others23 = others23 + vec2usi(aux_8bits);
        aux_8bits[3] = (others23 >> 24) & 0x000000FF;
        aux_8bits[2] = (others23 >> 16) & 0x000000FF;
        aux_8bits[1] = (others23 >> 8) & 0x000000FF;
        aux_8bits[0] =  others23 & 0x000000FF;
        opProcessorWrite(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others23 = 0;
    }

    return;
}
static OP_MONITOR_FN(FetchCallback24) { 
    char aux_8bits[4];
    opProcessorRead(processor, addr, &aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int instruction32 = vec2usi(aux_8bits);
    fetch24++;

    //opMessage("I", "FETCH", "PE%d- %s @ %x", 24, opProcessorDisassemble(processor, addr, OP_DSA_UNCOOKED), (unsigned int)addr);
    
    // https://www2.eecs.berkeley.edu/Pubs/TechRpts/2014/EECS-2014-54.pdf - pag 60
    if((instruction32 & 0x0000007F) == 0x00000003){         // checks if the opcode is equal to b0000011 (LOAD) 
        load24++;
    }
    else if((instruction32 & 0x0000007F) == 0x00000023){    // checks if the opcode is equal to b0100011 (STORE)
        store24++;
    }
    else{
        others24++;
    }

    if(fetch24 > 100){
        fetch24 = 0;

        opProcessorRead(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load24 = load24 + vec2usi(aux_8bits);
        aux_8bits[3] = (load24 >> 24) & 0x000000FF;
        aux_8bits[2] = (load24 >> 16) & 0x000000FF;
        aux_8bits[1] = (load24 >> 8) & 0x000000FF;
        aux_8bits[0] =  load24 & 0x000000FF;
        opProcessorWrite(processor, LOAD_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        load24 = 0;

        opProcessorRead(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store24 = store24 + vec2usi(aux_8bits);
        aux_8bits[3] = (store24 >> 24) & 0x000000FF;
        aux_8bits[2] = (store24 >> 16) & 0x000000FF;
        aux_8bits[1] = (store24 >> 8) & 0x000000FF;
        aux_8bits[0] =  store24 & 0x000000FF;
        opProcessorWrite(processor, STORE_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        store24 = 0;

        opProcessorRead(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others24 = others24 + vec2usi(aux_8bits);
        aux_8bits[3] = (others24 >> 24) & 0x000000FF;
        aux_8bits[2] = (others24 >> 16) & 0x000000FF;
        aux_8bits[1] = (others24 >> 8) & 0x000000FF;
        aux_8bits[0] =  others24 & 0x000000FF;
        opProcessorWrite(processor, OTHERS_ADDR, aux_8bits, 4, 1, True, OP_HOSTENDIAN_TARGET);
        others24 = 0;
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

static OP_MONITOR_FN(FreqCallback9){

    // reads the frequency scale, provided by the system
    char readFreq_8bit[4];
    opProcessorRead(processor, 0x8FFFFFF8, &readFreq_8bit, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int readFreq_32bit = vec2usi(readFreq_8bit);

    // translates from fixed point to float
    float freqScale = 100 - (float)((float)readFreq_32bit / 10);
    if(freqScale > 91.0){ // this should never occour, but this is a security if to ensure that the processor does never stall by going to 0% of its frequency
        freqScale = 90;
    }
    opMessage("I", "HARNESS", " >>> PE 9 changing to %.1f%% of its nominal frequency -- (%.0fMHz)", 100-freqScale, 10*(100-freqScale));
    opProcessorDerate(processor, freqScale);
    return;
}

static OP_MONITOR_FN(FreqCallback10){

    // reads the frequency scale, provided by the system
    char readFreq_8bit[4];
    opProcessorRead(processor, 0x8FFFFFF8, &readFreq_8bit, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int readFreq_32bit = vec2usi(readFreq_8bit);

    // translates from fixed point to float
    float freqScale = 100 - (float)((float)readFreq_32bit / 10);
    if(freqScale > 91.0){ // this should never occour, but this is a security if to ensure that the processor does never stall by going to 0% of its frequency
        freqScale = 90;
    }
    opMessage("I", "HARNESS", " >>> PE 10 changing to %.1f%% of its nominal frequency -- (%.0fMHz)", 100-freqScale, 10*(100-freqScale));
    opProcessorDerate(processor, freqScale);
    return;
}

static OP_MONITOR_FN(FreqCallback11){

    // reads the frequency scale, provided by the system
    char readFreq_8bit[4];
    opProcessorRead(processor, 0x8FFFFFF8, &readFreq_8bit, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int readFreq_32bit = vec2usi(readFreq_8bit);

    // translates from fixed point to float
    float freqScale = 100 - (float)((float)readFreq_32bit / 10);
    if(freqScale > 91.0){ // this should never occour, but this is a security if to ensure that the processor does never stall by going to 0% of its frequency
        freqScale = 90;
    }
    opMessage("I", "HARNESS", " >>> PE 11 changing to %.1f%% of its nominal frequency -- (%.0fMHz)", 100-freqScale, 10*(100-freqScale));
    opProcessorDerate(processor, freqScale);
    return;
}

static OP_MONITOR_FN(FreqCallback12){

    // reads the frequency scale, provided by the system
    char readFreq_8bit[4];
    opProcessorRead(processor, 0x8FFFFFF8, &readFreq_8bit, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int readFreq_32bit = vec2usi(readFreq_8bit);

    // translates from fixed point to float
    float freqScale = 100 - (float)((float)readFreq_32bit / 10);
    if(freqScale > 91.0){ // this should never occour, but this is a security if to ensure that the processor does never stall by going to 0% of its frequency
        freqScale = 90;
    }
    opMessage("I", "HARNESS", " >>> PE 12 changing to %.1f%% of its nominal frequency -- (%.0fMHz)", 100-freqScale, 10*(100-freqScale));
    opProcessorDerate(processor, freqScale);
    return;
}

static OP_MONITOR_FN(FreqCallback13){

    // reads the frequency scale, provided by the system
    char readFreq_8bit[4];
    opProcessorRead(processor, 0x8FFFFFF8, &readFreq_8bit, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int readFreq_32bit = vec2usi(readFreq_8bit);

    // translates from fixed point to float
    float freqScale = 100 - (float)((float)readFreq_32bit / 10);
    if(freqScale > 91.0){ // this should never occour, but this is a security if to ensure that the processor does never stall by going to 0% of its frequency
        freqScale = 90;
    }
    opMessage("I", "HARNESS", " >>> PE 13 changing to %.1f%% of its nominal frequency -- (%.0fMHz)", 100-freqScale, 10*(100-freqScale));
    opProcessorDerate(processor, freqScale);
    return;
}

static OP_MONITOR_FN(FreqCallback14){

    // reads the frequency scale, provided by the system
    char readFreq_8bit[4];
    opProcessorRead(processor, 0x8FFFFFF8, &readFreq_8bit, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int readFreq_32bit = vec2usi(readFreq_8bit);

    // translates from fixed point to float
    float freqScale = 100 - (float)((float)readFreq_32bit / 10);
    if(freqScale > 91.0){ // this should never occour, but this is a security if to ensure that the processor does never stall by going to 0% of its frequency
        freqScale = 90;
    }
    opMessage("I", "HARNESS", " >>> PE 14 changing to %.1f%% of its nominal frequency -- (%.0fMHz)", 100-freqScale, 10*(100-freqScale));
    opProcessorDerate(processor, freqScale);
    return;
}

static OP_MONITOR_FN(FreqCallback15){

    // reads the frequency scale, provided by the system
    char readFreq_8bit[4];
    opProcessorRead(processor, 0x8FFFFFF8, &readFreq_8bit, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int readFreq_32bit = vec2usi(readFreq_8bit);

    // translates from fixed point to float
    float freqScale = 100 - (float)((float)readFreq_32bit / 10);
    if(freqScale > 91.0){ // this should never occour, but this is a security if to ensure that the processor does never stall by going to 0% of its frequency
        freqScale = 90;
    }
    opMessage("I", "HARNESS", " >>> PE 15 changing to %.1f%% of its nominal frequency -- (%.0fMHz)", 100-freqScale, 10*(100-freqScale));
    opProcessorDerate(processor, freqScale);
    return;
}

static OP_MONITOR_FN(FreqCallback16){

    // reads the frequency scale, provided by the system
    char readFreq_8bit[4];
    opProcessorRead(processor, 0x8FFFFFF8, &readFreq_8bit, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int readFreq_32bit = vec2usi(readFreq_8bit);

    // translates from fixed point to float
    float freqScale = 100 - (float)((float)readFreq_32bit / 10);
    if(freqScale > 91.0){ // this should never occour, but this is a security if to ensure that the processor does never stall by going to 0% of its frequency
        freqScale = 90;
    }
    opMessage("I", "HARNESS", " >>> PE 16 changing to %.1f%% of its nominal frequency -- (%.0fMHz)", 100-freqScale, 10*(100-freqScale));
    opProcessorDerate(processor, freqScale);
    return;
}

static OP_MONITOR_FN(FreqCallback17){

    // reads the frequency scale, provided by the system
    char readFreq_8bit[4];
    opProcessorRead(processor, 0x8FFFFFF8, &readFreq_8bit, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int readFreq_32bit = vec2usi(readFreq_8bit);

    // translates from fixed point to float
    float freqScale = 100 - (float)((float)readFreq_32bit / 10);
    if(freqScale > 91.0){ // this should never occour, but this is a security if to ensure that the processor does never stall by going to 0% of its frequency
        freqScale = 90;
    }
    opMessage("I", "HARNESS", " >>> PE 17 changing to %.1f%% of its nominal frequency -- (%.0fMHz)", 100-freqScale, 10*(100-freqScale));
    opProcessorDerate(processor, freqScale);
    return;
}

static OP_MONITOR_FN(FreqCallback18){

    // reads the frequency scale, provided by the system
    char readFreq_8bit[4];
    opProcessorRead(processor, 0x8FFFFFF8, &readFreq_8bit, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int readFreq_32bit = vec2usi(readFreq_8bit);

    // translates from fixed point to float
    float freqScale = 100 - (float)((float)readFreq_32bit / 10);
    if(freqScale > 91.0){ // this should never occour, but this is a security if to ensure that the processor does never stall by going to 0% of its frequency
        freqScale = 90;
    }
    opMessage("I", "HARNESS", " >>> PE 18 changing to %.1f%% of its nominal frequency -- (%.0fMHz)", 100-freqScale, 10*(100-freqScale));
    opProcessorDerate(processor, freqScale);
    return;
}

static OP_MONITOR_FN(FreqCallback19){

    // reads the frequency scale, provided by the system
    char readFreq_8bit[4];
    opProcessorRead(processor, 0x8FFFFFF8, &readFreq_8bit, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int readFreq_32bit = vec2usi(readFreq_8bit);

    // translates from fixed point to float
    float freqScale = 100 - (float)((float)readFreq_32bit / 10);
    if(freqScale > 91.0){ // this should never occour, but this is a security if to ensure that the processor does never stall by going to 0% of its frequency
        freqScale = 90;
    }
    opMessage("I", "HARNESS", " >>> PE 19 changing to %.1f%% of its nominal frequency -- (%.0fMHz)", 100-freqScale, 10*(100-freqScale));
    opProcessorDerate(processor, freqScale);
    return;
}

static OP_MONITOR_FN(FreqCallback20){

    // reads the frequency scale, provided by the system
    char readFreq_8bit[4];
    opProcessorRead(processor, 0x8FFFFFF8, &readFreq_8bit, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int readFreq_32bit = vec2usi(readFreq_8bit);

    // translates from fixed point to float
    float freqScale = 100 - (float)((float)readFreq_32bit / 10);
    if(freqScale > 91.0){ // this should never occour, but this is a security if to ensure that the processor does never stall by going to 0% of its frequency
        freqScale = 90;
    }
    opMessage("I", "HARNESS", " >>> PE 20 changing to %.1f%% of its nominal frequency -- (%.0fMHz)", 100-freqScale, 10*(100-freqScale));
    opProcessorDerate(processor, freqScale);
    return;
}

static OP_MONITOR_FN(FreqCallback21){

    // reads the frequency scale, provided by the system
    char readFreq_8bit[4];
    opProcessorRead(processor, 0x8FFFFFF8, &readFreq_8bit, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int readFreq_32bit = vec2usi(readFreq_8bit);

    // translates from fixed point to float
    float freqScale = 100 - (float)((float)readFreq_32bit / 10);
    if(freqScale > 91.0){ // this should never occour, but this is a security if to ensure that the processor does never stall by going to 0% of its frequency
        freqScale = 90;
    }
    opMessage("I", "HARNESS", " >>> PE 21 changing to %.1f%% of its nominal frequency -- (%.0fMHz)", 100-freqScale, 10*(100-freqScale));
    opProcessorDerate(processor, freqScale);
    return;
}

static OP_MONITOR_FN(FreqCallback22){

    // reads the frequency scale, provided by the system
    char readFreq_8bit[4];
    opProcessorRead(processor, 0x8FFFFFF8, &readFreq_8bit, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int readFreq_32bit = vec2usi(readFreq_8bit);

    // translates from fixed point to float
    float freqScale = 100 - (float)((float)readFreq_32bit / 10);
    if(freqScale > 91.0){ // this should never occour, but this is a security if to ensure that the processor does never stall by going to 0% of its frequency
        freqScale = 90;
    }
    opMessage("I", "HARNESS", " >>> PE 22 changing to %.1f%% of its nominal frequency -- (%.0fMHz)", 100-freqScale, 10*(100-freqScale));
    opProcessorDerate(processor, freqScale);
    return;
}

static OP_MONITOR_FN(FreqCallback23){

    // reads the frequency scale, provided by the system
    char readFreq_8bit[4];
    opProcessorRead(processor, 0x8FFFFFF8, &readFreq_8bit, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int readFreq_32bit = vec2usi(readFreq_8bit);

    // translates from fixed point to float
    float freqScale = 100 - (float)((float)readFreq_32bit / 10);
    if(freqScale > 91.0){ // this should never occour, but this is a security if to ensure that the processor does never stall by going to 0% of its frequency
        freqScale = 90;
    }
    opMessage("I", "HARNESS", " >>> PE 23 changing to %.1f%% of its nominal frequency -- (%.0fMHz)", 100-freqScale, 10*(100-freqScale));
    opProcessorDerate(processor, freqScale);
    return;
}

static OP_MONITOR_FN(FreqCallback24){

    // reads the frequency scale, provided by the system
    char readFreq_8bit[4];
    opProcessorRead(processor, 0x8FFFFFF8, &readFreq_8bit, 4, 1, True, OP_HOSTENDIAN_TARGET);
    unsigned int readFreq_32bit = vec2usi(readFreq_8bit);

    // translates from fixed point to float
    float freqScale = 100 - (float)((float)readFreq_32bit / 10);
    if(freqScale > 91.0){ // this should never occour, but this is a security if to ensure that the processor does never stall by going to 0% of its frequency
        freqScale = 90;
    }
    opMessage("I", "HARNESS", " >>> PE 24 changing to %.1f%% of its nominal frequency -- (%.0fMHz)", 100-freqScale, 10*(100-freqScale));
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

            case 9:
                opProcessorWriteMonitorAdd(proc, 0x8FFFFFF8, 0x8FFFFFFB, FreqCallback9, "frequency9");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FREQUENCIA 9 ADICIONADO!");
                opProcessorFetchMonitorAdd(proc, 0x80000000, 0x8fffffff, FetchCallback9, "fetch9");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FETCH 9 ADICIONADO!");
                break;

            case 10:
                opProcessorWriteMonitorAdd(proc, 0x8FFFFFF8, 0x8FFFFFFB, FreqCallback10, "frequency10");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FREQUENCIA 10 ADICIONADO!");
                opProcessorFetchMonitorAdd(proc, 0x80000000, 0x8fffffff, FetchCallback10, "fetch10");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FETCH 10 ADICIONADO!");
                break;

            case 11:
                opProcessorWriteMonitorAdd(proc, 0x8FFFFFF8, 0x8FFFFFFB, FreqCallback11, "frequency11");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FREQUENCIA 11 ADICIONADO!");
                opProcessorFetchMonitorAdd(proc, 0x80000000, 0x8fffffff, FetchCallback11, "fetch11");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FETCH 11 ADICIONADO!");
                break;

            case 12:
                opProcessorWriteMonitorAdd(proc, 0x8FFFFFF8, 0x8FFFFFFB, FreqCallback12, "frequency12");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FREQUENCIA 12 ADICIONADO!");
                opProcessorFetchMonitorAdd(proc, 0x80000000, 0x8fffffff, FetchCallback12, "fetch12");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FETCH 12 ADICIONADO!");
                break;

            case 13:
                opProcessorWriteMonitorAdd(proc, 0x8FFFFFF8, 0x8FFFFFFB, FreqCallback13, "frequency13");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FREQUENCIA 13 ADICIONADO!");
                opProcessorFetchMonitorAdd(proc, 0x80000000, 0x8fffffff, FetchCallback13, "fetch13");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FETCH 13 ADICIONADO!");
                break;

            case 14:
                opProcessorWriteMonitorAdd(proc, 0x8FFFFFF8, 0x8FFFFFFB, FreqCallback14, "frequency14");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FREQUENCIA 14 ADICIONADO!");
                opProcessorFetchMonitorAdd(proc, 0x80000000, 0x8fffffff, FetchCallback14, "fetch14");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FETCH 14 ADICIONADO!");
                break;

            case 15:
                opProcessorWriteMonitorAdd(proc, 0x8FFFFFF8, 0x8FFFFFFB, FreqCallback15, "frequency15");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FREQUENCIA 15 ADICIONADO!");
                opProcessorFetchMonitorAdd(proc, 0x80000000, 0x8fffffff, FetchCallback15, "fetch15");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FETCH 15 ADICIONADO!");
                break;

            case 16:
                opProcessorWriteMonitorAdd(proc, 0x8FFFFFF8, 0x8FFFFFFB, FreqCallback16, "frequency16");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FREQUENCIA 16 ADICIONADO!");
                opProcessorFetchMonitorAdd(proc, 0x80000000, 0x8fffffff, FetchCallback16, "fetch16");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FETCH 16 ADICIONADO!");
                break;

            case 17:
                opProcessorWriteMonitorAdd(proc, 0x8FFFFFF8, 0x8FFFFFFB, FreqCallback17, "frequency17");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FREQUENCIA 17 ADICIONADO!");
                opProcessorFetchMonitorAdd(proc, 0x80000000, 0x8fffffff, FetchCallback17, "fetch17");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FETCH 17 ADICIONADO!");
                break;

            case 18:
                opProcessorWriteMonitorAdd(proc, 0x8FFFFFF8, 0x8FFFFFFB, FreqCallback18, "frequency18");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FREQUENCIA 18 ADICIONADO!");
                opProcessorFetchMonitorAdd(proc, 0x80000000, 0x8fffffff, FetchCallback18, "fetch18");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FETCH 18 ADICIONADO!");
                break;

            case 19:
                opProcessorWriteMonitorAdd(proc, 0x8FFFFFF8, 0x8FFFFFFB, FreqCallback19, "frequency19");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FREQUENCIA 19 ADICIONADO!");
                opProcessorFetchMonitorAdd(proc, 0x80000000, 0x8fffffff, FetchCallback19, "fetch19");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FETCH 19 ADICIONADO!");
                break;

            case 20:
                opProcessorWriteMonitorAdd(proc, 0x8FFFFFF8, 0x8FFFFFFB, FreqCallback20, "frequency20");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FREQUENCIA 20 ADICIONADO!");
                opProcessorFetchMonitorAdd(proc, 0x80000000, 0x8fffffff, FetchCallback20, "fetch20");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FETCH 20 ADICIONADO!");
                break;

            case 21:
                opProcessorWriteMonitorAdd(proc, 0x8FFFFFF8, 0x8FFFFFFB, FreqCallback21, "frequency21");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FREQUENCIA 21 ADICIONADO!");
                opProcessorFetchMonitorAdd(proc, 0x80000000, 0x8fffffff, FetchCallback21, "fetch21");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FETCH 21 ADICIONADO!");
                break;

            case 22:
                opProcessorWriteMonitorAdd(proc, 0x8FFFFFF8, 0x8FFFFFFB, FreqCallback22, "frequency22");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FREQUENCIA 22 ADICIONADO!");
                opProcessorFetchMonitorAdd(proc, 0x80000000, 0x8fffffff, FetchCallback22, "fetch22");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FETCH 22 ADICIONADO!");
                break;

            case 23:
                opProcessorWriteMonitorAdd(proc, 0x8FFFFFF8, 0x8FFFFFFB, FreqCallback23, "frequency23");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FREQUENCIA 23 ADICIONADO!");
                opProcessorFetchMonitorAdd(proc, 0x80000000, 0x8fffffff, FetchCallback23, "fetch23");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FETCH 23 ADICIONADO!");
                break;

            case 24:
                opProcessorWriteMonitorAdd(proc, 0x8FFFFFF8, 0x8FFFFFFB, FreqCallback24, "frequency24");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FREQUENCIA 24 ADICIONADO!");
                opProcessorFetchMonitorAdd(proc, 0x80000000, 0x8fffffff, FetchCallback24, "fetch24");
                opMessage("I", "HARNESS INFO", "	 > MONITOR DE FETCH 24 ADICIONADO!");
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

