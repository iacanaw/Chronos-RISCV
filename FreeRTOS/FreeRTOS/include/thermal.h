#ifndef __THERMAL_H__
#define __THERMAL_H__

//#include "dvfs.h"
#include "chronos.h"

/*--------------------------------------------------------------------------------
 * ROUTER - Multipling power and period results in energy (pJ)
 * the energy is in pJ MULTIPLIED BY 100 (xxxx => xx.xx)
 * -------------------------------------------------------------------------------*/
// mW*100 or uW/10
#define DYN_BUFFER_ACTIVE_0 118
#define DYN_BUFFER_ACTIVE_1 151
#define DYN_BUFFER_ACTIVE_2 188

#define DYN_CTRL_ACTIVE_0 41
#define DYN_CTRL_ACTIVE_1 52
#define DYN_CTRL_ACTIVE_2 65

#define DYN_BUFFER_IDLE_0 49
#define DYN_BUFFER_IDLE_1 62
#define DYN_BUFFER_IDLE_2 77

#define DYN_CTRL_IDLE_0 14
#define DYN_CTRL_IDLE_1 18
#define DYN_CTRL_IDLE_2 22

const int powerAvgBufferIdle[3] = {DYN_BUFFER_IDLE_0, DYN_BUFFER_IDLE_1, DYN_BUFFER_IDLE_2};
const int powerSwitchControlIdle[3] = {DYN_CTRL_IDLE_0, DYN_CTRL_IDLE_1, DYN_CTRL_IDLE_2};

const int powerAvgBufferActive[3] = {DYN_BUFFER_ACTIVE_0, DYN_BUFFER_ACTIVE_1, DYN_BUFFER_ACTIVE_2};
const int powerSwitchControlActive[3] = {DYN_CTRL_ACTIVE_0, DYN_CTRL_ACTIVE_1, DYN_CTRL_ACTIVE_2};

/*--------------------------------------------------------------------------------
 * MEMORY - Total dynamic read/write energy per access (pJ)
 * THE INTEGER IS A FLOAT CONSTANT MULTIPLIED BY 100 (xxxx => xx.xx)
 * -------------------------------------------------------------------------------*/
#define DYN_READ_MEM_0 6351
#define DYN_READ_MEM_1 7796
//#define DYN_READ_MEM_2 9396
#define DYN_READ_MEM_2 832 // pJ = 260fJ/bit * 32 bit = 8,32pJ  
const int readEnergyMemory[3] = {DYN_READ_MEM_0, DYN_READ_MEM_1, DYN_READ_MEM_2};

#define DYN_WRITE_MEM_0 8843
#define DYN_WRITE_MEM_1 11043
//#define DYN_WRITE_MEM_2 13497
#define DYN_WRITE_MEM_2 576 // pJ = 180fJ/bit * 32 bit = 5,76pJ  
const int writeEnergyMemory[3] = {DYN_WRITE_MEM_0, DYN_WRITE_MEM_1, DYN_WRITE_MEM_2};

/*--------------------------------------------------------------------------------
 * PROCESSOR - Total dynamic energy per instruction class (pJ)
 * THE INTEGER IS A FLOAT CONSTANT MULTIPLIED BY 100 (xxxx => xx.xx)
 * -------------------------------------------------------------------------------*/
#define DYN_ARITH_0 859
#define DYN_ARITH_1 1097
//#define DYN_ARITH_2 1370 
//#define DYN_ARITH_2 2158*11 // paper benini
#define DYN_ARITH_2 1695 
const int arithDyn[3] = {DYN_ARITH_0, DYN_ARITH_1, DYN_ARITH_2};

#define DYN_LOAD_0 1570
#define DYN_LOAD_1 2001
//#define DYN_LOAD_2 2493
//#define DYN_LOAD_2 2521*11 // paper benini
#define DYN_LOAD_2 2245
const int loadStoreDyn[3] = {DYN_LOAD_0, DYN_LOAD_1, DYN_LOAD_2};

#define PE_LEAKAGE 108 // mW

#define LOADS_COUNT     0x8FFFFFF4u
#define STORES_COUNT    0x8FFFFFF0u
#define OTHERS_COUNT    0x8FFFFFECu
// #define MOVE_INST ((unsigned int *)0x0FFFFFE8)
// #define LOAD_INST ((unsigned int *)0x0FFFFFE4)
// #define STORE_INST ((unsigned int *)0x0FFFFFE0)
// #define SHIFT_INST ((unsigned int *)0x0FFFFFDC)
// #define NOP_INST ((unsigned int *)0x0FFFFFD8)
// #define LOGICAL_INST ((unsigned int *)0x0FFFFFD4)
// #define MULT_DIV_INST ((unsigned int *)0x0FFFFFD0)
// #define WEIRD_INST ((unsigned int *)0x0FFFFFCC)

#define EAST_FLITS      0x8FFFFFC8
#define EAST_PACKETS    0x8FFFFFC4
#define WEST_FLITS      0x8FFFFFC0
#define WEST_PACKETS    0x8FFFFFBC
#define NORTH_FLITS     0x8FFFFFB8
#define NORTH_PACKETS   0x8FFFFFB4
#define SOUTH_FLITS     0x8FFFFFB0
#define SOUTH_PACKETS   0x8FFFFFAC
#define LOCAL_FLITS     0x8FFFFFA8
#define LOCAL_PACKETS   0x8FFFFFA4

extern unsigned int ProcessorAddr;
unsigned int thermalPacket_pending = FALSE;

unsigned int memFlits;
unsigned int eastFlits_last = 0;
unsigned int eastPackets_last = 0;
//
unsigned int westFlits_last = 0;
unsigned int westPackets_last = 0;
//
unsigned int northFlits_last = 0;
unsigned int northPackets_last = 0;
//
unsigned int southFlits_last = 0;
unsigned int southPackets_last = 0;
//
unsigned int localFlits_last = 0;
unsigned int localPackets_last = 0;


void printExecutedInstructions();

void resetExecutedInstructions();

void powerEstimation();

unsigned int estimateNoCActivity();

unsigned int getNumberOfPorts(unsigned int address);

#endif
