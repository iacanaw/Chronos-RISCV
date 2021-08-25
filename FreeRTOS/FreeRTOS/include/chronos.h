// OVP stuff

////////////////////////////////////////////////////////////
// DVFS Info
//#define FREQUENCY_SCALE ((unsigned int *)0x8FFFFFA0)
//#define CLK_GATING ((unsigned int *)0x8FFFFFFC)
//#define EXECUTED_INST ((unsigned int *)0x8FFFFFF8)

////////////////////////////////////////////////////////////
// Processor ID info
#define MY_ID ((unsigned int *)0x8FFFFFFC)
volatile unsigned int *myID = MY_ID;

////////////////////////////////////////////////////////////
// Router mapped register - informs the router its address
#define ROUTER_BASE ((unsigned int *)0x50000000)
volatile unsigned int *myAddress = ROUTER_BASE;

void Chronos_init(){
    *myAddress = 5;//*myID; // This information is provided by HARNESS
    
    return;
}
