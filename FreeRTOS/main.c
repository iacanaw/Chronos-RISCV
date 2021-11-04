#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include <stdio.h>
#include "hw_platform.h"
#include "core_uart_apb.h"
#include "chronos.h"
#include "system_call.h"
#include "applications.h"
#include "globalMaster.h"

const char * g_hello_msg = "\r\nFreeRTOS Example\r\n";

/* A block time of zero simply means "don't block". */
#define mainDONT_BLOCK						( 0UL )

/******************************************************************************
 * CoreUARTapb instance data.
 *****************************************************************************/
UART_instance_t g_uart;
/*-----------------------------------------------------------*/

static void vUartAliveTask( void *pvParameters );
static void GlobalManagerTask( void *pvParameters );

/*
 * Tasks to handle the NI interruptions
 */
static void NI_RX_Handler( void *pvParameters );
//static void NI_TX_Handler( void *pvParameters );

/*
 * FreeRTOS hook for when malloc fails, enable in FreeRTOSConfig.
 */
void vApplicationMallocFailedHook( void );

/*
 * FreeRTOS hook for when FreeRtos is idling, enable in FreeRTOSConfig.
 */
void vApplicationIdleHook( void );

/*
 * FreeRTOS hook for when a stack overflow occurs, enable in FreeRTOSConfig.
 */
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );

/*
 * Stores the processor address
 */
unsigned int ProcessorAddr;

extern volatile unsigned int NI_IRCount;

/*-----------------------------------------------------------*/

int main( void )
{
	char str[20];
    PLIC_init();

    /**************************************************************************
    * Initialize CoreUART with its base address, baud value, and line
    * configuration.
    *************************************************************************/
    UART_init(&g_uart, COREUARTAPB0_BASE_ADDR, BAUD_VALUE_115200,
             (DATA_8_BITS | NO_PARITY) );
    
	/**************************************************************************
    * Initialize the Chronos platform stuff
    *************************************************************************/
	UART_polled_tx_string( &g_uart, (const uint8_t *)"\n Chronos platform initializing... \n" );
	Chronos_init();
	UART_polled_tx_string( &g_uart, (const uint8_t *)"\n This terminal belongs to the address: " );
	myItoa(ProcessorAddr, str, 16);
    UART_polled_tx_string( &g_uart, (const uint8_t *)str); UART_polled_tx_string( &g_uart, (const uint8_t *)"\n");
	
	//xTaskCreate( NI_TX_Handler, "TXHandler", 1024*4, NULL, (tskIDLE_PRIORITY + 1), NULL);
	//xTaskCreate( NI_RX_Handler, "RXHandler", 1024*3, NULL, (tskIDLE_PRIORITY + 1), NULL);

	if (ProcessorAddr == 0x0000){
		UART_polled_tx_string( &g_uart, (const uint8_t *)"\n This processor is the Global Master: \n" );
		/* Create the GlobalManager task */
		xTaskCreate( GlobalManagerTask, "GlobalMaster", 1024*6, NULL, (tskIDLE_PRIORITY + 1), NULL );
	} else {
		UART_polled_tx_string( &g_uart, (const uint8_t *)"\n This processor is a Slave: \n" );
		
		/* Create the two test tasks. */
		xTaskCreate( vUartAliveTask, "Alive", 1024*3, NULL, (tskIDLE_PRIORITY + 1), NULL );
	}	

	/* Start the kernel.  From here on, only tasks and interrupts will run. */
	vTaskStartScheduler();

	/* Exit FreeRTOS */
	return 0;
}

/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	task.  It is essential that code added to this hook function never attempts
	to block in any way (for example, call xQueueReceive() with a block time
	specified, or call vTaskDelay()).  If the application makes use of the
	vTaskDelete() API function (as this demo application does) then it is also
	important that vApplicationIdleHook() is permitted to return to its calling
	function, because it is the responsibility of the idle task to clean up
	memory allocated by the kernel to any task that has since been deleted. */
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/

static void vUartAliveTask( void *pvParameters )
{
	( void ) pvParameters;
	char str[20];
	unsigned int i, j;
	/*register long sp1 asm("s2") = 0;
	register long sp2 asm("s3") = 0;
	asm("addi	s2, sp, 0");
	printsv("meu sp1: ", sp1);*/

	for( i = 0 ;; i++ ){
		/*asm("addi	s3, sp, 0");
		printsvsv("meu sp2: ", sp2, " dif: ", sp2-sp1);*/
		// Start every task that is ready!
		for(j = 0; j < NUM_MAX_TASKS; j++){
			if(TaskList[j].status == TASK_SLOT_READY){
				printsvsv("Starting Task ", TaskList[j].TaskID, " from app ", TaskList[j].AppID);
				API_TaskStart(j);
			}
		}
		//prints("hello!\n");
		/*myItoa(ProcessorAddr, str, 16);
		UART_polled_tx_string( &g_uart, (const uint8_t *)str);
		UART_polled_tx_string( &g_uart, (const uint8_t *)" PE is alive - " );
		myItoa(i, str, 10);
		UART_polled_tx_string( &g_uart, (const uint8_t *)str);
		UART_polled_tx_string( &g_uart, (const uint8_t *)" -\r\n" );*/
	    //asm("wfi");
		vPortYield();
		//vTaskDelay(1);
	}
}

/*-----------------------------------------------------------*/

static void GlobalManagerTask( void *pvParameters ){
	( void ) pvParameters;
	int tick;
	char str[20];

	// Initialize the priority vector with the pattern policy
	GeneratePatternMatrix();

	// Initialize the System Tiles Info
	API_TilesReset();

	// Initialize the applications vector
    API_ApplicationsReset();

	// Informs the Repository that the GLOBALMASTER is ready to receive the application info
	API_RepositoryWakeUp();

	for(;;){
		tick = xTaskGetTickCount();
		myItoa(tick, str, 10);
		UART_polled_tx_string( &g_uart, (const uint8_t *)str);
		printsv("GlobalMasterActive", tick);
		UART_polled_tx_string( &g_uart, (const uint8_t *)" GlobalMasterRoutine...\r\n" );

		// Checks if there is some task to allocate...
		API_AllocateTasks(tick);

		vTaskDelay(1);
		//asm ("wfi"); 
	}
}

/*-----------------------------------------------------------*/

/*static void NI_TX_Handler( void *pvParameters ){
	// Enables interruption from NI
    NI_enable_irq(TX_RX);
	for(;;){
		//prints("blá!\n");
		while(HW_get_32bit_reg(NI_TX) == NI_STATUS_INTER){
			vPortEnterCritical();
			printsv("TX interruption catched - ", NI_IRCount);
			API_ClearPipeSlot(SendingSlot); // clear the pipe slot that was transmitted
			HW_set_32bit_reg(NI_TX, DONE);  // releases the interruption
			API_Try2Send();                 // tries to send another packet (if available)
			vPortExitCritical();
		}
		//HW_set_32bit_reg(NI_TX, RESET);
		asm("wfi");
	}
}*/

// static void NI_RX_Handler( void *pvParameters ){
// 	unsigned int aux;
// 	NI_IRCount = 0;
// 	// Enables interruption from NI
//     NI_enable_irq(TX_RX);
// 	for(;;){

// 		vPortEnterCritical();
// 		while(HW_get_32bit_reg(NI_TX) == NI_STATUS_INTER){
// 			printsv("TX interruption catched - ", NI_IRCount);
// 			API_ClearPipeSlot(SendingSlot); // clear the pipe slot that was transmitted
// 			HW_set_32bit_reg(NI_TX, DONE);  // releases the interruption
// 			API_Try2Send();                 // tries to send another packet (if available)
// 		}

// 		while(HW_get_32bit_reg(NI_RX) == NI_STATUS_INTER || HW_get_32bit_reg(NI_RX) == NI_STATUS_WAITING){
// 			printsv("RX interruption catched - ", NI_IRCount);
// 			switch (incommingPacket.service){

// 				case REPOSITORY_APP_INFO: // When the repository informs the GM that exist a new Application available:
// 					//prints("REPOSITORY_APP_INFO\n");
// 					API_AddApplication(incommingPacket.application_id,
// 									incommingPacket.aplication_period, 
// 									incommingPacket.application_executions, 
// 									incommingPacket.application_n_tasks);
// 					break;
				
// 				case TASK_ALLOCATION_SEND: // When the GM asks one Slave to allocate one task
// 					prints("TASK_ALLOCATION_SEND\n");
// 					aux = API_TaskAllocation(incommingPacket.task_id,
// 											incommingPacket.task_txt_size,
// 											incommingPacket.task_bss_size,
// 											incommingPacket.task_start_point,
// 											incommingPacket.task_app_id);
// 					printsv("Task slot: ", aux);
// 					printsv("Task slot TaskAddr: ", TaskList[aux].taskAddr);
// 					// Informs the NI were to write the application
// 					incommingPacket.service = TASK_ALLOCATION_FINISHED;
// 					HW_set_32bit_reg(NI_RX, TaskList[aux].taskAddr);
// 					break;
				
// 				case TASK_FINISH:
// 					printsvsv("FINISHED: Task ", incommingPacket.task_id, "from application ", incommingPacket.task_app_id);
// 					API_ClearTaskSlotFromTile(incommingPacket.task_dest_addr, incommingPacket.task_app_id, incommingPacket.task_id);
// 					API_DealocateTask(incommingPacket.task_id, incommingPacket.task_app_id);
// 					break;

// 				case TASK_ALLOCATION_FINISHED:
// 					prints("TASK_ALLOCATION_FINISHED\n");
// 					API_AckTaskAllocation(incommingPacket.task_id, incommingPacket.task_app_id);
// 					/*for(aux = 0; aux < NUM_MAX_TASKS; aux++){
// 						prints("========\n");
// 						printsv("TaskSlot ", aux);
// 						printsv("status: ", TaskList[aux].status);
// 						printsv("taskSize: ", TaskList[aux].taskSize);
// 						printsv("taskAddr: ", TaskList[aux].taskAddr);
// 						printsv("mainAddr: ", TaskList[aux].mainAddr);
// 						printsv("taskHandler: ", (unsigned int)TaskList[aux].TaskHandler);
// 					}*/
// 					break;

// 				case TASK_ALLOCATION_SUCCESS:
// 					prints("TASK_ALLOCATION_SUCCESS\n");
// 					//printi(incommingPacket.task_id);
// 					API_TaskAllocated(incommingPacket.task_id, incommingPacket.task_app_id);
// 					break;

// 				case TASK_START:
// 					prints("Chegou um TASK_START!\n");
// 					aux = API_GetTaskSlot(incommingPacket.task_id, incommingPacket.task_app_id);
// 					// Informs the NI were to write the application
// 					incommingPacket.service = TASK_RUN;
// 					HW_set_32bit_reg(NI_RX, (unsigned int)&TaskList[aux].appNumTasks);
// 					break;
				
// 				case TASK_RUN:
// 					prints("Chegou um TASK_RUN!\n");
// 					aux = API_GetTaskSlot(incommingPacket.task_id, incommingPacket.task_app_id);
// 					TaskList[aux].status = TASK_SLOT_READY;
// 					printsvsv("Enabling Task: ", incommingPacket.task_id, "from app: ", incommingPacket.task_app_id);
// 					printsv("Slot: ", aux);
// 					break;

// 				case MESSAGE_REQUEST:
// 					// check the pipe
// 					printsvsv("Chegou um message request! App: ", incommingPacket.task_app_id, "Task: ", incommingPacket.task_id);
// 					aux = API_CheckMessagePipe(incommingPacket.task_id, incommingPacket.task_app_id);
// 					if (aux == ERRO){
// 						// register an messagerequest
// 						prints("Mensagem não encontrada, adicionando ao PendingReq!\n");
// 						API_AddPendingReq(incommingPacket.task_id, incommingPacket.task_app_id, incommingPacket.producer_task_id);
// 					} else {
// 						prints("Mensagem encontrada no pipe!\n");
// 						API_PushSendQueue(MESSAGE, aux);
// 						// API_Try2Send();
// 					}
// 					break;
				
// 				case MESSAGE_DELIVERY:
// 					prints("Tem uma mensagem chegando...\n");
// 					aux = API_GetTaskSlot(incommingPacket.destination_task, incommingPacket.application_id);
// 					incommingPacket.service = MESSAGE_DELIVERY_FINISH;
// 					//printsv("MESSAGE_DELIVERY addr: ", TaskList[aux].MsgToReceive);
// 					HW_set_32bit_reg(NI_RX, TaskList[aux].MsgToReceive);
// 					//prints("done...\n----------\n");
// 					break;
				
// 				case MESSAGE_DELIVERY_FINISH:
// 					//prints("Terminou de entregar a mensagem!!\n");
// 					aux = API_GetTaskSlot(incommingPacket.destination_task, incommingPacket.application_id);
// 					TaskList[aux].waitingMsg = FALSE;
// 					break;
					
// 				default:
// 					printsv("ERROR External_2_IRQHandler Unknown-Service ", incommingPacket.service);
// 					break;
// 			}
// 			HW_set_32bit_reg(NI_RX, DONE);
// 		}
// 		vPortExitCritical();
// 		asm("wfi");
// 		//vTaskDelay(1);
// 		//asm("nop");
// 	}
// }