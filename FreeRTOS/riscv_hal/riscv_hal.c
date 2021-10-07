/*******************************************************************************
 * (c) Copyright 2016-2017 Microsemi SoC Products Group. All rights reserved.
 *
 * @file riscv_hal.c
 * @author Microsemi SoC Products Group
 * @brief Implementation of Hardware Abstraction Layer for Mi-V soft processors
 *   
 * SVN $Revision: 9575 $
 * SVN $Date: 2017-11-14 14:23:11 +0530 (Tue, 14 Nov 2017) $
 */
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include "core_uart_apb.h"
#include "chronos.h"

#include "../FreeRTOS/include/system_call.h"
#include "../FreeRTOS/include/chronos.h"
#include "riscv_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RTC_PRESCALER 100

#define SUCCESS 0
#define ERROR   1

extern UART_instance_t g_uart;

char str[20];

/*------------------------------------------------------------------------------
 * 
 */
uint8_t Invalid_IRQHandler(void);
uint8_t External_1_IRQHandler(void);
uint8_t External_2_IRQHandler(void);
uint8_t External_3_IRQHandler(void);
uint8_t External_4_IRQHandler(void);
uint8_t External_5_IRQHandler(void);
uint8_t External_6_IRQHandler(void);
uint8_t External_7_IRQHandler(void);
uint8_t External_8_IRQHandler(void);
uint8_t External_9_IRQHandler(void);
uint8_t External_10_IRQHandler(void);
uint8_t External_11_IRQHandler(void);
uint8_t External_12_IRQHandler(void);
uint8_t External_13_IRQHandler(void);
uint8_t External_14_IRQHandler(void);
uint8_t External_15_IRQHandler(void);
uint8_t External_16_IRQHandler(void);
uint8_t External_17_IRQHandler(void);
uint8_t External_18_IRQHandler(void);
uint8_t External_19_IRQHandler(void);
uint8_t External_20_IRQHandler(void);
uint8_t External_21_IRQHandler(void);
uint8_t External_22_IRQHandler(void);
uint8_t External_23_IRQHandler(void);
uint8_t External_24_IRQHandler(void);
uint8_t External_25_IRQHandler(void);
uint8_t External_26_IRQHandler(void);
uint8_t External_27_IRQHandler(void);
uint8_t External_28_IRQHandler(void);
uint8_t External_29_IRQHandler(void);
uint8_t External_30_IRQHandler(void);
uint8_t External_31_IRQHandler(void);

/*------------------------------------------------------------------------------
 * 
 */
extern void handle_m_ext_interrupt();
extern void handle_m_timer_interrupt();
extern void Software_IRQHandler();

/*------------------------------------------------------------------------------
 * Increment value for the mtimecmp register in order to achieve a system tick
 * interrupt as specified through the SysTick_Config() function.
 */
static uint64_t g_systick_increment = 0;

/*------------------------------------------------------------------------------
 * Disable all interrupts.
 */
void __disable_irq(void)
{
    clear_csr(mstatus, MSTATUS_MPIE);
    clear_csr(mstatus, MSTATUS_MIE);
}

/*------------------------------------------------------------------------------
 * Enabler all interrupts.
 */
void __enable_irq(void)
{
    set_csr(mstatus, MSTATUS_MIE);
}

/*------------------------------------------------------------------------------
 * Configure the machine timer to generate an interrupt.
 */
uint32_t SysTick_Config(uint32_t ticks)
{
    uint32_t ret_val = ERROR;
    
    g_systick_increment = ticks / RTC_PRESCALER;

    if (g_systick_increment > 0)
    {
        uint32_t mhart_id = read_csr(mhartid);
        PRCI->MTIMECMP[mhart_id] = PRCI->MTIME + g_systick_increment;

        set_csr(mie, MIP_MTIP);

        __enable_irq();

        ret_val = SUCCESS;
    }
    
    return ret_val;
}

/*------------------------------------------------------------------------------
 * RISC-V interrupt handler for machine timer interrupts.
 */
void handle_m_timer_interrupt()
{
    clear_csr(mie, MIP_MTIP);

    SysTick_Handler();

    PRCI->MTIMECMP[read_csr(mhartid)] = PRCI->MTIME + g_systick_increment;

    set_csr(mie, MIP_MTIP);
}

/*------------------------------------------------------------------------------
 * RISC-V interrupt handler for external interrupts.
 */
uint8_t (*ext_irq_handler_table[32])(void) =
{
    Invalid_IRQHandler,
    External_1_IRQHandler,
    External_2_IRQHandler,
    External_3_IRQHandler,
    External_4_IRQHandler,
    External_5_IRQHandler,
    External_6_IRQHandler,
    External_7_IRQHandler,
    External_8_IRQHandler,
    External_9_IRQHandler,
    External_10_IRQHandler,
    External_11_IRQHandler,
    External_12_IRQHandler,
    External_13_IRQHandler,
    External_14_IRQHandler,
    External_15_IRQHandler,
    External_16_IRQHandler,
    External_17_IRQHandler,
    External_18_IRQHandler,
    External_19_IRQHandler,
    External_20_IRQHandler,
    External_21_IRQHandler,
    External_22_IRQHandler,
    External_23_IRQHandler,
    External_24_IRQHandler,
    External_25_IRQHandler,
    External_26_IRQHandler,
    External_27_IRQHandler,
    External_28_IRQHandler,
    External_29_IRQHandler,
    External_30_IRQHandler,
    External_31_IRQHandler
};

/*------------------------------------------------------------------------------
 * 
 */
void handle_m_ext_interrupt()
{
    uint32_t int_num  = PLIC_ClaimIRQ();
    uint8_t disable = EXT_IRQ_KEEP_ENABLED;

    disable = ext_irq_handler_table[int_num]();

    PLIC_CompleteIRQ(int_num);

    if(EXT_IRQ_DISABLE == disable)
    {
        PLIC_DisableIRQ((IRQn_Type)int_num);
    }
}

void handle_m_soft_interrupt()
{
    Software_IRQHandler();

    /*Clear software interrupt*/
    PRCI->MSIP[0] = 0x00;
}

/* syscall funcion defined at system_call.c */
unsigned int handle_syscall(){
	unsigned int arg0, arg1, arg2, arg3, arg4, arg5, type;
	unsigned int *pointer;
	register long temp asm("s2") = 0;
	asm("addi	s2, sp, 0");
	
	pointer = (unsigned int *)(temp + (29*4));
	arg0 =  *pointer;

	pointer = (unsigned int *)(temp + (30*4));
	arg1 = *pointer;
	
	pointer = (unsigned int *)(temp + (31*4));
	arg2 = *pointer;

	pointer = (unsigned int *)(temp + (32*4));
	arg3 = *pointer;

	pointer = (unsigned int *)(temp + (33*4));
	arg4 = *pointer;

	pointer = (unsigned int *)(temp + (34*4));
	arg4 = *pointer;

	pointer = (unsigned int *)(temp + (36*4));
	type = *pointer;
	
	switch (type){

		case SYS_TESTING:
			prints("Detectei uma chamada TESTE de sistema!\n");
			printsv("arg0 ", arg0);
			printsv("arg1 ", arg1);
			printsv("arg2 ", arg2);
			printsv("arg3 ", arg3);
			printsv("arg4 ", arg4);
			printsv("type ", type);
			break;

        case SYS_PRINTS:
            prints(arg0);
            UART_polled_tx_string( &g_uart, arg0 );
            break;
        
        case SYS_PRINTI:
            printi(arg0);
            myItoa(arg0, str, 10);
            UART_polled_tx_string( &g_uart, (const uint8_t *)str );
            break;
        
        case SYS_SEND_MSG:
            //              addr, task
            API_SendMessage(arg0, arg1);
            break;
        
        case SYS_RECV_MSG:
            //                 addr, task
            API_SendMessageReq(arg0, arg1);
            break;
        
        case SYS_END_TASK:
            API_FinishRunningTask();
            break;

		default:
			prints("Systemcall não identificada!\n");
			printsv("arg0 ", arg0);
			printsv("arg1 ", arg1);
			printsv("arg2 ", arg2);
			printsv("arg3 ", arg3);
			printsv("arg4 ", arg4);
			printsv("type ", type);
			break;
	}
	return 42;
	////////////////////////////////////////////////////
	// Utilize para encontrar os argumentos na pilha!!!
	// int i;
	// unsigned int *p;
	// unsigned int value;
	// register long temp asm("t4") = 0;
	// asm("addi	t4, sp, 0");
	// for (i = 0; i < 64; i++){
	// 	p = (temp + (i*4));
	// 	value = *p;
	// 	printsvsv("i: ", i, " value: ", value);
	// }
    // prints("Cheguei aqui!!!!!!! sucessooo :D\n");
	////////////////////////////////////////////////////
}

/*------------------------------------------------------------------------------
 * Trap/Interrupt handler
 */
uintptr_t handle_trap(uintptr_t mcause, uintptr_t epc)
{
    if (mcause == ENV_CALL_M || mcause == ENV_CALL_H || mcause == ENV_CALL_S || mcause == ENV_CALL_U){
        handle_syscall();
        epc = (uintptr_t) (epc + 4);
    }
    else if ((mcause & MCAUSE_INT) && ((mcause & MCAUSE_CAUSE)  == IRQ_M_EXT)){
        handle_m_ext_interrupt();
    }
    else if ((mcause & MCAUSE_INT) && ((mcause & MCAUSE_CAUSE)  == IRQ_M_TIMER)){
        handle_m_timer_interrupt();
    }
    else if ((mcause & MCAUSE_INT) && ((mcause & MCAUSE_CAUSE)  == IRQ_M_SOFT)){
        handle_m_soft_interrupt();
    }
    else{
        write(1, "trap\n", 5);
        _exit(mcause);
    }
    return epc;
}

#ifdef __cplusplus
}
#endif
