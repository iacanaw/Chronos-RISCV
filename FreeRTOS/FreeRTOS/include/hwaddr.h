#ifndef __HWADDR_H__
#define __HWADDR_H__


/******************** Processor ID ********************/
#define MY_ID                   0x8FFFFFFCU //(provided by harness)

/************ Router and NI mapped register ***********/
#define ROUTER_BASE             0x50000000U
#define NI_ADDR                 0x50000004U
#define NI_TX                   0x50000008U
#define NI_RX                   0x5000000CU
#define NI_TIMER                0x50000010U

/**************** Router and NI commands **************/
#define TX                      0x00002222U
#define RX                      0x00003333U
#define TX_RX                   0x00007777U
#define DONE                    0x80005555U
#define HOLD                    0xFFFFFFF6U
#define INTERRUPTION_ACK        0x00001234U
#define RESET                   0x00008888U
#define NI_TX_IRQn              External_3_IRQn
#define NI_RX_IRQn              External_2_IRQn
#define Timer_IRQn              External_4_IRQn

/*************** Printer mapped register **************/
#define PRINTER_CHAR            0x50000020U
#define PRINTER_INT             0x50000024U

/**************** Timer mapped register ***************/
#define TIMER_BASE              0x5000001CU
#define TIMER                   0x75315946U

#endif