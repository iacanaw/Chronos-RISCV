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
// PERIPHERAL PORTS
#define PERIPH_EAST         0x00010000
#define PERIPH_WEST         0x00020000
#define PERIPH_NORTH        0x00030000
#define PERIPH_SOUTH        0x00040000
////////////////////////////////////////////////////////////
// SENDING QUEUE
#define SERVICE             0x10100000
#define MESSAGE             0x20200000
#define EMPTY               0x30300000
////////////////////////////////////////////////////////////
// NI STATUS
#define NI_STATUS_ON        0x0011
#define NI_STATUS_OFF       0x00FF
#define NI_STATUS_INTER     0x0F0F
#define NI_STATUS_HANDLING  0x0EEE


// Auxiliar Packet used by the NI to store incomming packets
volatile unsigned int incommingPacket[ PACKET_MAX_SIZE ];

// Sending Queue
volatile unsigned int SendingQueue[PIPE_SIZE*2];
volatile unsigned int SendingQueue_front;
volatile unsigned int SendingQueue_tail;
volatile unsigned int SendingSlot;

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
// Try to send some packet! 
void API_Try2Send();
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
// Returns the PE address for a giver pair of coords
unsigned int makeAddress(unsigned int x, unsigned int y);
// Pushes one slot to the sending queue
void API_PushSendQueue(unsigned int type, unsigned int slot);
// Pops the first slot from the sending queue
unsigned int API_PopSendQueue();

#endif
