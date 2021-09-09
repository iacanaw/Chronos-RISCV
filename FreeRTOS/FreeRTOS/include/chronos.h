#ifndef __CHRONOS_H__
#define __CHRONOS_H__

#include "riscv_hal/riscv_plic.h"
#include "hw_reg_access.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "hwaddr.h"
#include "message.h"
#include "packet.h"
#include "services.h"
#include "applications.h"

////////////////////////////////////////////////////////////
// DEFINES
#define TRUE                1
#define FALSE               0
#define ERRO                -1
////////////////////////////////////////////////////////////
// SYSTEM INFO
#define DIM_X               3
#define DIM_Y               3
#define GLOBAL_MASTER_ADDR  0x0000
#define MAX_LOCAL_TASKS     10
////////////////////////////////////////////////////////////
// PIPE 
#define PIPE_SIZE           4 // Defines the PIPE size
#define PIPE_OCCUPIED       1
#define PIPE_FREE           -1
#define PIPE_TRANSMITTING   -2
#define PIPE_WAIT           0xFFFFFFFF
////////////////////////////////////////////////////////////
// SERVICE PACKET SLOTS
#define SPS_EMPTY           1
#define SPS_OCCUPIED        0
//////////////////////////////
//////////////////////////////

// Auxiliar Packet used by the NI to store incomming packets
volatile unsigned int incommingPacket[ PACKET_MAX_SIZE ];

// Initiate chronos stuff
void Chronos_init();
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
// Configure the NI to transmitt a given packet
void SendRaw(unsigned int addr);
// Enables interruptions incomming from NI
void NI_enable_irq(int which);
// Disables interruptions incomming from NI
void NI_disable_irq(int which);
// Interruptions handler for TX
uint8_t External_1_IRQHandler(void);
// Interruptions handler for RX
uint8_t External_2_IRQHandler(void);
// An alternative implemenation of ITOA and its support functions
char* myItoa(int value, char* buffer, int base);
void mySwap(char *x, char *y);
char* reverse(char *buffer, int i, int j);
// Receives a message and alocates it in the application structure
void ReceiveMessage(Message *theMessage, unsigned int from);

#endif
