#ifndef __HWADDR_H__
#define __HWADDR_H__


/******************** Processor ID ********************/
#define MY_ID                   0x8FFFFFFCU //(provided by harness)

/************ Router and NI mapped register ***********/
#define ROUTER_BASE             0x50000000U
#define NI_ADDR                 0x50000004U
#define NI_TX                   0x50000008U
#define NI_RX                   0x5000000CU

/**************** Router and NI commands **************/
#define TX                      0x00002222U
#define RX                      0x00003333U
#define TX_RX                   0x00007777U
#define DONE                    0x00005555U
#define INTERRUPTION_ACK        0x00001234U
#define RESET                   0x00008888U
#define NI_TX_IRQn              External_1_IRQn
#define NI_RX_IRQn              External_2_IRQn

/*************** Printer mapped register **************/
#define PRINTER_CHAR            0x50000020U
#define PRINTER_INT             0x50000024U

#endif