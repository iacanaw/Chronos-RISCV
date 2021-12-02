#ifndef __THERMAL_H__
#define __THERMAL_H__

//#include "dvfs.h"
#include "chronos.h"

/*--------------------------------------------------------------------------------
 * MEMORY - Total dynamic read/write energy per access (pJ)
 * THE INTEGER IS A FLOAT CONSTANT MULTIPLIED BY 100 (xxxx => xx.xx)
 * -------------------------------------------------------------------------------*/
#define DYN_READ_MEM_0 6351
#define DYN_READ_MEM_1 7796
#define DYN_READ_MEM_2 9396
const int readEnergyMemory[3] = {DYN_READ_MEM_0, DYN_READ_MEM_1, DYN_READ_MEM_2};

#define DYN_WRITE_MEM_0 8843
#define DYN_WRITE_MEM_1 11043
#define DYN_WRITE_MEM_2 13497
const int writeEnergyMemory[3] = {DYN_WRITE_MEM_0, DYN_WRITE_MEM_1, DYN_WRITE_MEM_2};

/*--------------------------------------------------------------------------------
 * PROCESSOR - Total dynamic energy per instruction class (pJ)
 * THE INTEGER IS A FLOAT CONSTANT MULTIPLIED BY 100 (xxxx => xx.xx)
 * -------------------------------------------------------------------------------*/
#define DYN_ARITH_0 859
#define DYN_ARITH_1 1097
//#define DYN_ARITH_2 1370 
#define DYN_ARITH_2 2158 // paper benini
const int arithDyn[3] = {DYN_ARITH_0, DYN_ARITH_1, DYN_ARITH_2};

#define DYN_LOAD_0 1570
#define DYN_LOAD_1 2001
//#define DYN_LOAD_2 2493
#define DYN_LOAD_2 2521 // paper benini
const int loadStoreDyn[3] = {DYN_LOAD_0, DYN_LOAD_1, DYN_LOAD_2};

#define PE_LEAKAGE 108 // mW

#define LOADS_COUNT     0x8FFFFFF4u
#define STORES_COUNT    0x8FFFFFF0u
#define OTHERS_COUNT    0x8FFFFFECu

extern unsigned int ProcessorAddr;
unsigned int thermalPacket_pending = FALSE;

void printExecutedInstructions();

void resetExecutedInstructions();

void powerEstimation();

#endif
