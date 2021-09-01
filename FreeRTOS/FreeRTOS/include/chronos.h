#include "hw_reg_access.h"
// OVP stuff


////////////////////////////////////////////////////////////
// DVFS Info
//#define FREQUENCY_SCALE ((unsigned int *)0x8FFFFFA0)
//#define CLK_GATING ((unsigned int *)0x8FFFFFFC)
//#define EXECUTED_INST ((unsigned int *)0x8FFFFFF8)

////////////////////////////////////////////////////////////
// Processor ID info
#define MY_ID 0x8FFFFFFCUL

////////////////////////////////////////////////////////////
// Router and NI mapped register - informs the router its address
#define ROUTER_BASE 0x50000000UL
#define NI_ADDR 0x50000004UL
#define NI_TX 0x50000008UL
#define NI_RX 0x5000000CUL
// NI commands
#define TX 0x2222
#define RX 0x3333
#define TX_RX 0x4444 
#define NI_TX_IRQn External_1_IRQn
#define NI_RX_IRQn External_2_IRQn

////////////////////////////////////////////////////////////
// Router mapped register - informs the printer registers
#define PRINTER_CHAR 0x50000020UL
#define PRINTER_INT 0x50000024UL
// Prints a string
void prints(char *text); 

// Prints an integer
void printi(int value); 

// Prints a string followed by a integer
void printsv(char *text1, int value1); 

// Prints two strings and two integers interspersed
void printsvsv(char *text1, int value1, char *text2, int value2);

// Gets the X coordinate from the address
unsigned int getXpos(unsigned int addr);

// Gets the Y coordinate from the address
unsigned int getYpos(unsigned int addr);

// Enables interruptions incomming from NI
void NI_enable_irq(int which);

// Disables interruptions incomming from NI
void NI_disable_irq(int which);

////////////////////////////////////////////////////////////
// Initialize Chronos stuff
void Chronos_init(){
    //Informs the Router this tile ID, that is provided by Harness
    HW_set_32bit_reg(ROUTER_BASE, HW_get_32bit_reg(MY_ID)); 
    
    // Reads this processor address
    unsigned int myaddr = HW_get_32bit_reg(ROUTER_BASE);

    // Informs the PRINTER this tile address
    HW_set_32bit_reg(PRINTER_CHAR, getXpos(myaddr));
    HW_set_32bit_reg(PRINTER_CHAR, getYpos(myaddr));

    // Enables interruption from NI
    NI_enable_irq(TX_RX);

    return;
}

////////////////////////////////////////////////////////////
// Prints a string
void prints(char *text) {
    int i = 0;
    do {
        HW_set_32bit_reg(PRINTER_CHAR, text[i]);
        i++;
    } while (text[i - 1] != '\0');
    return;
}

////////////////////////////////////////////////////////////
// Prints an integer
void printi(int value) {
    HW_set_32bit_reg(PRINTER_INT, value);
    return;
}

////////////////////////////////////////////////////////////
// Prints a string followed by a integer
void printsv(char *text1, int value1) {
    prints(text1);
    printi(value1);
    prints("\n");
    return;
}

////////////////////////////////////////////////////////////
// Prints two strings and two integers interspersed
void printsvsv(char *text1, int value1, char *text2, int value2) {
    prints(text1);
    printi(value1);
    prints(text2);
    printi(value2);
    prints("\n");
    return;
}

////////////////////////////////////////////////////////////
// Gets the X coordinate from the address
unsigned int getXpos(unsigned int addr) { return ((addr & 0x0000FF00) >> 8); }

////////////////////////////////////////////////////////////
// Gets the Y coordinate from the address
unsigned int getYpos(unsigned int addr) { return (addr & 0x000000FF); }

///////////////////////////////////////////////////////////////////
// Configure the NI to transmitt a given packet
void SendRaw(unsigned int addr) {
    HW_set_32bit_reg(NI_ADDR, addr);
    HW_set_32bit_reg(NI_TX, TX);
    prints("Mensagem enviada!");
    return;
}

////////////////////////////////////////////////////////////
// Enables interruptions incomming from NI
void NI_enable_irq(int which){
	if (which == TX || which == TX_RX) PLIC_EnableIRQ(NI_TX_IRQn);
    if (which == RX || which == TX_RX) PLIC_EnableIRQ(NI_RX_IRQn);
    return;
}

////////////////////////////////////////////////////////////
// Disables interruptions incomming from NI
void NI_disable_irq(int which){
	if (which == TX || which == TX_RX) PLIC_DisableIRQ(NI_TX_IRQn);
    if (which == RX || which == TX_RX) PLIC_DisableIRQ(NI_RX_IRQn);
    return;
}