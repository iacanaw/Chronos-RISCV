#ifndef __CHRONOS_H__
#define __CHRONOS_H__

#include "riscv_hal/riscv_plic.h"
#include "hw_reg_access.h"
#include "hwaddr.h"
#include "message.h"
#include "packet.h"
#include "services.h"
#include "hwaddr.h"

////////////////////////////////////////////////////////////
// PIPE 
#define PIPE_SIZE           4 // Defines the PIPE size
#define PIPE_OCCUPIED       1
#define PIPE_FREE           -1
#define PIPE_TRANSMITTING   -2
#define PIPE_WAIT           0xFFFFFFFF
//////////////////////////////
//////////////////////////////

////////////////////////////////////////////////////////////
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
	if (which == TX || which == TX_RX){
        PLIC_EnableIRQ(NI_TX_IRQn);
        PLIC_SetPriority(NI_TX_IRQn, 1);
    }
    if (which == RX || which == TX_RX){
        PLIC_EnableIRQ(NI_RX_IRQn);
        PLIC_SetPriority(NI_RX_IRQn, 1);
    }
    return;
}

////////////////////////////////////////////////////////////
// Disables interruptions incomming from NI
void NI_disable_irq(int which){
	if (which == TX || which == TX_RX) PLIC_DisableIRQ(NI_TX_IRQn);
    if (which == RX || which == TX_RX) PLIC_DisableIRQ(NI_RX_IRQn);
    return;
}

////////////////////////////////////////////////////////////
// Interruptions handler for TX
uint8_t External_1_IRQHandler(void){ 
    prints("INTERRUPTION TX\n");
    HW_set_32bit_reg(NI_TX, DONE);
    return 0;
}

////////////////////////////////////////////////////////////
// Interruptions handler for RX
uint8_t External_2_IRQHandler(void){ 
    prints("INTERRUPTION RX\n");
    HW_set_32bit_reg(NI_RX, DONE);
    return 0;
}

////////////////////////////////////////////////////////////
// https://www.techiedelight.com/implement-itoa-function-in-c/
// Function to swap two numbers
void mySwap(char *x, char *y) {
    char t = *x; *x = *y; *y = t;
}
// Function to reverse `buffer[i…j]`
char* reverse(char *buffer, int i, int j){
    while (i < j) {
        mySwap(&buffer[i++], &buffer[j--]);
    }
 
    return buffer;
}
// Iterative function to implement `itoa()` function in C
char* myItoa(int value, char* buffer, int base){
    // invalid input
    if (base < 2 || base > 32) {
        return buffer;
    }
 
    // consider the absolute value of the number
    int n;
    if (value < 0)
        n = value * -1;
    else 
        n = value;

    int i = 0;
    while (n){
        int r = n % base;
 
        if (r >= 10) {
            buffer[i++] = 65 + (r - 10);
        }
        else {
            buffer[i++] = 48 + r;
        }
 
        n = n / base;
    }
 
    // if the number is 0
    if (i == 0) {
        buffer[i++] = '0';
    }
 
    // If the base is 10 and the value is negative, the resulting string
    // is preceded with a minus sign (-)
    // With any other base, value is always considered unsigned
    if (value < 0 && base == 10) {
        buffer[i++] = '-';
    }
 
    buffer[i] = '\0'; // null terminate string
 
    // reverse the string and return it
    return reverse(buffer, 0, i - 1);
}


#endif