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

extern volatile unsigned int API_SystemFinish = FALSE;

/* A block time of zero simply means "don't block". */
#define mainDONT_BLOCK						( 0UL )

/* Defines the thermal management technique 
    0) Spiral (baseline)
    1) Pattern
    2) PIDTM
    3) CHRONOS 
    4) CHARACTERIZATION
    5) CHRONOS2 */
#define THERMAL_MANAGEMENT 5

/* Defines the amount of ticks to simulate - if zero then simulates until the scenario's end */
#define SIMULATION_MAX_TICKS 0

/******************************************************************************
 * CoreUARTapb instance data.
 *****************************************************************************/
UART_instance_t g_uart;
/*-----------------------------------------------------------*/

// NI Handler task
static void vNI_RX_HandlerTask( void *pvParameters );
static void vNI_TX_HandlerTask( void *pvParameters );
static void vNI_TMR_HandlerTask( void *pvParameters );
extern TaskHandle_t NI_RX_Handler, NI_TX_Handler, NI_TMR_Handler; // KeeperTask_Handler;


static void GlobalManagerTask( void *pvParameters );
//static void KeeperTask (void *pvParameters);

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

extern volatile unsigned int measuredWindows;

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
	
	/* Create the NI Handler task */
	xTaskCreate( vNI_RX_HandlerTask, "RX_TASK", 1024, NULL, (tskIDLE_PRIORITY + 2), &NI_RX_Handler );
	xTaskCreate( vNI_TX_HandlerTask, "TX_TASK", 1024, NULL, (tskIDLE_PRIORITY + 3), &NI_TX_Handler );
	xTaskCreate( vNI_TMR_HandlerTask, "TMR_TASK", 1024, NULL, (tskIDLE_PRIORITY + 4), &NI_TMR_Handler );

	if (ProcessorAddr == 0x0000){
		UART_polled_tx_string( &g_uart, (const uint8_t *)"\n This processor is the Global Master: \n" );
		/* Create the GlobalManager task */
		xTaskCreate( GlobalManagerTask, "GlobalMaster", 1024*6, NULL, (tskIDLE_PRIORITY + 1), NULL );
	} else {
		UART_polled_tx_string( &g_uart, (const uint8_t *)"\n This processor is a Slave: \n" );
        /* Creates the KeeperTask */
        //xTaskCreate( KeeperTask, "KeeperTask", 256, NULL, (tskIDLE_PRIORITY + 4), &KeeperTask_Handler );
	}
	//xTaskCreate( NI_Handler, "Handler", 1024*6, NULL, (tskIDLE_PRIORITY + 2), NULL );

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


/* A task that blocks waiting to be notified that the peripheral
needs servicing. */
void vNI_RX_HandlerTask( void *pvParameters ){
	BaseType_t xEvent;
	unsigned int aux, aux2, service, i;
	//const TickType_t xBlockTime = 10000;
	uint32_t ulNotifiedValue;
    //BaseType_t xHigherPriorityTaskWoken;

    for( ;; ){
		/* Blocks the task until the NI interrupts it */
        ulNotifiedValue = ulTaskNotifyTake( pdFALSE,
                                            portMAX_DELAY );

        // if( ulNotifiedValue > 0 ){
        //     /* Perform any processing necessitated by the interrupt. */
		// 	UART_polled_tx_string( &g_uart, (const uint8_t *)" Rotina RX...\r\n" );
        // }
        // else{
        //     /* Did not receive a notification within the expected time. */
		// 	UART_polled_tx_string( &g_uart, (const uint8_t *)" Time out NI RX...\r\n" );
        // }

		vTaskEnterCritical();
        if( HW_get_32bit_reg(NI_RX) == NI_STATUS_INTER || HW_get_32bit_reg(NI_RX) == NI_STATUS_WAITING) {
            prints("RX interruption catched\n");
            service = incommingPacket.service;
            incommingPacket.service = SOLVED;
            switch (service){
                case REPOSITORY_APP_INFO: // When the repository informs the GM that exist a new Application available:
                    prints("REPOSITORY_APP_INFO\n");
                    aux = API_AddApplication(incommingPacket.application_id,
                                        incommingPacket.aplication_period, 
                                        incommingPacket.application_executions, 
                                        incommingPacket.application_n_tasks);
                    incommingPacket.service = REPOSITORY_APP_INFO_FINISH;
                    HW_set_32bit_reg(NI_RX, &applications[aux].taskType[0]);
                    prints("1NI_RX DONE!\n");
                    break;
                
                case REPOSITORY_APP_INFO_FINISH: // When the repository informs the GM that exist a new Application available:
                    prints("REPOSITORY_APP_INFO_FINISH\n");
                    aux = API_GetApplicationSlot(incommingPacket.application_id);
                    for(i = 0; i < incommingPacket.application_n_tasks; i++){
                        printsvsv("> Task ", i, "type  ", applications[aux].taskType[i]);
                    }
                    prints("1.5NI_RX DONE!\n");
                    break;
                
                case TASK_ALLOCATION_SEND: // When the GM asks one Slave to allocate one task
                    prints("TASK_ALLOCATION_SEND\n");
                    aux = API_TaskAllocation(incommingPacket.task_id,
                                             incommingPacket.task_txt_size,
                                             incommingPacket.task_bss_size,
                                             incommingPacket.task_start_point,
                                             incommingPacket.app_id,
                                             incommingPacket.task_migration_addr);
                    printsv("Task slot: ", aux);
                    printsv("Task slot TaskAddr: ", TaskList[aux].taskAddr);
                    printsv("Migration start at: ", incommingPacket.task_migration_addr);
                    // Informs the NI were to write the application
                    HW_set_32bit_reg(NI_RX, TaskList[aux].taskAddr);
                    incommingPacket.service = TASK_ALLOCATION_FINISHED;
                    //HW_set_32bit_reg(NI_RX, DONE);
                    prints("2NI_RX DONE!\n");
                    break;
                
                case TASK_FINISH:
                    printsvsv("FINISHED: Task ", incommingPacket.task_id, "from application ", incommingPacket.app_id);
                    API_ClearTaskSlotFromTile(incommingPacket.task_dest_addr, incommingPacket.app_id, incommingPacket.task_id);
                    API_DealocateTask(incommingPacket.task_id, incommingPacket.app_id);
                    //HW_set_32bit_reg(NI_RX, DONE);
                    prints("3NI_RX DONE!\n");
                    break;

                case TASK_ALLOCATION_FINISHED:
                    prints("TASK_ALLOCATION_FINISHED\n");
                    API_AckTaskAllocation(incommingPacket.task_id, incommingPacket.app_id);
                    //HW_set_32bit_reg(NI_RX, DONE);
                    prints("4NI_RX DONE!\n");
                    break;

                case TASK_ALLOCATION_SUCCESS:
                    prints("TASK_ALLOCATION_SUCCESS\n");
                    API_TaskAllocated(incommingPacket.task_id, incommingPacket.app_id);
                    //HW_set_32bit_reg(NI_RX, DONE);
                    prints("5NI_RX DONE!\n");
                    break;

                case TASK_START:
                    prints("Chegou um TASK_START!\n");
                    aux = API_GetTaskSlot(incommingPacket.task_id, incommingPacket.app_id);
                    // Informs the NI were to write the application
                    HW_set_32bit_reg(NI_RX, (unsigned int)&TaskList[aux].appNumTasks);
                    incommingPacket.service = TASK_RUN;
                    //HW_set_32bit_reg(NI_RX, DONE);
                    prints("6NI_RX DONE!\n");
                    break;
                
                case TASK_RUN:
                    prints("Chegou um TASK_RUN!\n");
                    aux = API_GetTaskSlot(incommingPacket.task_id, incommingPacket.app_id);
                    TaskList[aux].status = TASK_SLOT_READY;
                    API_setFreqScale(1000);
                    printsvsv("Starting Task ", TaskList[aux].TaskID, " from app ", TaskList[aux].AppID);
                    printsv("taskSlot run: ", aux);
                    printsv("taskArg: ", incommingPacket.task_arg);
                    //HW_set_32bit_reg(NI_RX, DONE);
                    prints("7NI_RX DONE!\n");
                    API_TaskStart(aux, incommingPacket.task_arg);
                    break;

                case MESSAGE_REQUEST:
                    // check the pipe
                    printsvsv("Chegou um message request! App: ", incommingPacket.app_id, "Task: ", incommingPacket.task_id);
                    aux = API_CheckMessagePipe(incommingPacket.task_id, incommingPacket.app_id);
                    if (aux == ERRO){
                        // register an messagerequest
                        prints("Mensagem não encontrada, adicionando ao PendingReq!\n");
                        API_AddPendingReq(incommingPacket.task_id, incommingPacket.app_id, incommingPacket.producer_task_id);
                    } else {
                        prints("Mensagem encontrada no pipe!\n");
                        API_PushSendQueue(MESSAGE, aux);
                        // API_Try2Send();
                    }
                    //HW_set_32bit_reg(NI_RX, DONE);
                    prints("8NI_RX DONE!\n");
                    break;
                
                case MESSAGE_DELIVERY:
                    prints("Tem uma mensagem chegando...\n");
                    aux = API_GetTaskSlot(incommingPacket.destination_task, incommingPacket.application_id);
                    //printsv("MESSAGE_DELIVERY addr: ", TaskList[aux].MsgToReceive);
                    HW_set_32bit_reg(NI_RX, TaskList[aux].MsgToReceive);
                    incommingPacket.service = MESSAGE_DELIVERY_FINISH;
                    //prints("done...\n----------\n");
                    //HW_set_32bit_reg(NI_RX, DONE);
                    prints("9NI_RX DONE!\n");
                    break;
                
                case MESSAGE_DELIVERY_FINISH:
                    //prints("Terminou de entregar a mensagem!!\n");
                    aux = API_GetTaskSlot(incommingPacket.destination_task, incommingPacket.application_id);
                    TaskList[aux].waitingMsg = FALSE;
                    // if ( TaskList[aux].status == TASK_SLOT_SUSPENDED ){
                    //     printsv("Resumindo taskSlot ", aux);
                    //     TaskList[aux].status = TASK_SLOT_RUNNING;
                    //     vTaskResume( TaskList[aux].TaskHandler );
                    // }

                    // if(TaskList[aux].status == TASK_SLOT_BLOQUED){
                    //     TaskList[aux].status = TASK_SLOT_RUNNING;
                    //     /* xHigherPriorityTaskWoken must be initialised to pdFALSE.
                    //     If calling vTaskNotifyGiveFromISR() unblocks the handling
                    //     task, and the priority of the handling task is higher than
                    //     the priority of the currently running task, then
                    //     xHigherPriorityTaskWoken will be automatically set to pdTRUE. */
                    //     xHigherPriorityTaskWoken = pdFALSE;

                    //     /* Unblock the handling task so the task can perform
                    //     any processing necessitated by the interrupt.  xHandlingTask
                    //     is the task's handle, which was obtained when the task was
                    //     created.  vTaskNotifyGiveFromISR() also increments
                    //     the receiving task's notification value. */
                    //     vTaskNotifyGiveFromISR( TaskList[aux].TaskHandler, &xHigherPriorityTaskWoken );

                    //     /* Force a context switch if xHigherPriorityTaskWoken is now
                    //     set to pdTRUE. The macro used to do this is dependent on
                    //     the port and may be called portEND_SWITCHING_ISR. */
                    //     //portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
                    // }

                    //HW_set_32bit_reg(NI_RX, DONE);
                    prints("10NI_RX DONE!\n");
                    //vTaskResume(TaskList[aux].TaskHandler);
                    xTaskResumeFromISR(TaskList[aux].TaskHandler);
                    break;
                
                case TEMPERATURE_PACKET:
                    prints("Recebendo pacote de temperatura");
                    HW_set_32bit_reg(NI_RX, (unsigned int)&SystemTemperature);
                    incommingPacket.service = FINISH_TEMPERATURE_PACKET;
                    //HW_set_32bit_reg(NI_RX, DONE);
                    prints("11NI_RX DONE!\n");
                    break;
                
                case FINISH_TEMPERATURE_PACKET:
                    temperatureUpdated = 1;
                    measuredWindows++;
                    API_UpdateTemperature();
                    /*for(aux = 0; aux < DIM_X*DIM_Y; aux++){ 
                        printsvsv("pe", aux, "temp: ", SystemTemperature[aux]);
                    }*/
                    //HW_set_32bit_reg(NI_RX, DONE);
                    prints("12NI_RX DONE!\n");
                    break;

                case FIT_PACKET:
                    prints("Recebendo pacote de FIT");
                    HW_set_32bit_reg(NI_RX, (unsigned int)&SystemFIT);
                    incommingPacket.service = FINISH_FIT_PACKET;
                    prints("11.5NI_RX DONE!\n");
                    break;

                case FINISH_FIT_PACKET:
                    fitUpdated = 1;
                    API_UpdateFIT();
                    for(aux = 0; aux < DIM_X*DIM_Y; aux++){ 
                        printsvsv("pe", aux, "FIT: ", SystemFIT[aux]);
                    }
                    prints("12.5NI_RX DONE!\n");
                    break;

                case SOLVED:
                    //HW_set_32bit_reg(NI_RX, HOLD);
                    prints("13NI_RX HOLD!\n");
                    break;

                case TASK_MIGRATION_REQUEST:
                    prints("14NI_RX DONE!\n");
                    prints("Starting Migration Process...\n");
                    printsvsv("App: ", incommingPacket.app_id, "Task: ", incommingPacket.task_id);
                    API_InformMigration(incommingPacket.app_id, incommingPacket.task_id);
                    break;
                
                case TASK_MIGRATION_READY:
                    prints("15NI_RX DONE!\n");
                    prints("The processor is ready to start the migration!\n");
                    API_Migration_StallTasks(incommingPacket.app_id, incommingPacket.task_id);
                    printsvsv("App: ", incommingPacket.app_id, "Task: ", incommingPacket.task_id);
                    break;

                case TASK_STALL_REQUEST:
                    prints("16NI_RX_DONE!\n");
                    printsvsv("Stalling Task: ", incommingPacket.task_id, "from app: ", incommingPacket.app_id);
                    API_StallTask(incommingPacket.app_id, incommingPacket.task_id);
                    break;

                case TASK_STALL_ACK:
                    prints("17NI_RX_DONE!\n");
                    printsvsv("Task stalled: ", incommingPacket.task_id, "from app: ", incommingPacket.app_id);
                    API_StallTask_Ack(incommingPacket.app_id, incommingPacket.task_id);
                    break;

                case TASK_MIGRATION_FORWARD:
                    prints("18NI_RX_DONE!\n");
                    printsvsv("Task ", incommingPacket.task_id, "sending himself to PE: ", incommingPacket.task_dest_addr );
                    API_ForwardTask(incommingPacket.app_id, incommingPacket.task_id, incommingPacket.task_dest_addr);
                    prints("API_ForwardTask done\n");
                    API_SendPIPE(incommingPacket.app_id, incommingPacket.task_id, incommingPacket.task_dest_addr);
                    prints("API_SendPIPE done\n");
                    API_SendPending_and_Address(incommingPacket.app_id, incommingPacket.task_id, incommingPacket.task_dest_addr);
                    prints("API_SendPending done\n");
                    API_MigrationFinished(incommingPacket.app_id, incommingPacket.task_id);
                    prints("API_MigrationFinished done\n");
                    break;

                case TASK_MIGRATION_TASK:
                    prints("19NI_RX_DONE!\n");
                    printsvsv("Task ", incommingPacket.task_id, "migrated to this PE from app: ", incommingPacket.app_id );
                    aux = API_TaskAllocation(incommingPacket.task_id,
                                             incommingPacket.task_txt_size,
                                             incommingPacket.task_bss_size,
                                             incommingPacket.task_start_point,
                                             incommingPacket.app_id,
                                             incommingPacket.task_migration_addr);
                    printsv("Task slot: ", aux);
                    printsv("Task slot TaskAddr: ", TaskList[aux].taskAddr);
                    printsv("Migration start at: ", incommingPacket.task_migration_addr);
                    TaskList[aux].status = TASK_SLOT_MIGRATED;
                    API_SetMigrationVar(aux, 0xFFFFFFFF);
                    incommingPacket.service = TASK_MIGRATION_RECEIVE;
                    HW_set_32bit_reg(NI_RX, TaskList[aux].taskAddr);
                    break;

                case TASK_MIGRATION_RECEIVE:
                    prints("20NI_RX_DONE!\n");
                    prints("Task recebida com sucesso, aguardando o PIPE e os PendingRequests\n");
                    break;

                case MESSAGE_MIGRATION:
                    prints("21NI_RX_DONE!\n");
                    prints("Tem uma mensagem migrando...\n");
                    aux = API_GetTaskSlot(incommingPacket.producer_task, incommingPacket.application_id);
                    aux2 = API_GetMessageSlot_fromSlot(aux);
                    incommingPacket.service = MESSAGE_MIGRATION_FINISH;
                    //
                    TaskList[aux].MessagePipe[aux2].header.header           = incommingPacket.saved_addr;
                    TaskList[aux].MessagePipe[aux2].header.payload_size     = incommingPacket.payload_size;
                    TaskList[aux].MessagePipe[aux2].header.service          = MESSAGE_DELIVERY;
                    TaskList[aux].MessagePipe[aux2].header.application_id   = incommingPacket.application_id;
                    TaskList[aux].MessagePipe[aux2].header.producer_task    = incommingPacket.producer_task;
                    TaskList[aux].MessagePipe[aux2].header.destination_task = incommingPacket.destination_task;
                    
                    HW_set_32bit_reg(NI_RX, &TaskList[aux].MessagePipe[aux2].msg);
                    break;
                
                case MESSAGE_MIGRATION_FINISH:
                    prints("22NI_RX_DONE!\n");
                    prints("Message migration concluded!\n");
                    break;

                case TASK_MIGRATION_PENDING:
                    prints("23NI_RX_DONE!\n");
                    prints("Tem pending request chegando...\n");
                    aux = API_GetTaskSlot(incommingPacket.task_id, incommingPacket.app_id);
                    incommingPacket.service = TASK_MIGRATION_PENDING_FINISH;
                    HW_set_32bit_reg(NI_RX, (unsigned int)&TaskList[aux].appNumTasks);
                    break;

                case TASK_MIGRATION_PENDING_FINISH:
                    prints("24NI_RX_DONE!\n");
                    aux = API_GetTaskSlot(incommingPacket.task_id, incommingPacket.app_id);
                    for( i = 0; i < TaskList[aux].appNumTasks; i++){
                        printsvsv("received pending [", i, "] = ", TaskList[aux].PendingReq[i]);
                    }
                    prints("Pending requests and addresses received with success\n");
                    break;

                case TASK_MIGRATION_FINISHED:
                    prints("25NI_RX_DONE!\n");
                    printsvsv("Task: ", incommingPacket.task_id, "has migrated with success! App: ", incommingPacket.app_id);
                    aux = API_GetTaskSlot(incommingPacket.task_id, incommingPacket.app_id);
                    applications[incommingPacket.app_id].tasks[incommingPacket.task_id].addr = applications[incommingPacket.app_id].newAddr;
                    API_ReleaseApplication(incommingPacket.app_id, incommingPacket.task_id);
                    break;
                
                case TASK_RESUME:
                    prints("26NI_RX DONE!\n");
                    prints("Chegou um TASK_RESUME!\n");
                    aux = API_GetTaskSlot(incommingPacket.task_id, incommingPacket.app_id);
                    // Informs the NI were to write the application
                    HW_set_32bit_reg(NI_RX, (unsigned int)&TaskList[aux].appNumTasks);
                    incommingPacket.service = TASK_RESUME_FINISH;
                    //HW_set_32bit_reg(NI_RX, DONE);
                    break;
                
                case TASK_RESUME_FINISH:
                    prints("27NI_RX DONE!\n");
                    API_UpdatePipe(incommingPacket.task_id, incommingPacket.app_id);
                    API_setFreqScale(1000);
                    API_ResumeTask(incommingPacket.task_id, incommingPacket.app_id);
                    break;

                case LOST_MESSAGE_REQ:
                    prints("28NI_RX_DONE!\n");
                    API_Forward_MsgReq(incommingPacket.task_id, incommingPacket.app_id, incommingPacket.producer_task_id);
                    break;

                default:
                    printsv("ERROR External_2_IRQHandler Unknown-Service ", incommingPacket.service);
                    //HW_set_32bit_reg(NI_RX, DONE);
                    prints("66NI_RX DONE!\n");
                    break;
            }
        }
		HW_set_32bit_reg(NI_RX, DONE);
        vTaskExitCritical();
    }
}

/* A task that blocks waiting to be notified that the peripheral
needs servicing. */
void vNI_TX_HandlerTask( void *pvParameters ){
	BaseType_t xEvent;
	//const TickType_t xBlockTime = 1000000;
	uint32_t ulNotifiedValue;

    for( ;; ){
		/* Blocks the task until the NI interrupts it */
        ulNotifiedValue = ulTaskNotifyTake( pdFALSE,
                                            portMAX_DELAY );

        // if( ulNotifiedValue > 0 ){
        //     /* Perform any processing necessitated by the interrupt. */
		// 	UART_polled_tx_string( &g_uart, (const uint8_t *)" Rotina TX...\r\n" );
		// }
        // else{
        //     /* Did not receive a notification within the expected time. */
		// 	UART_polled_tx_string( &g_uart, (const uint8_t *)" Time out NI TX...\r\n" );
        // }

		// enters in critical mode
		vTaskEnterCritical();
		
		prints("TX interruption catched\n");
		API_ClearPipeSlot(SendingSlot); // clear the pipe slot that was transmitted
		HW_set_32bit_reg(NI_TX, DONE);  // releases the interruption
		API_Try2Send();                 // tries to send another packet (if available)
        
		// exits the criticla mode
		vTaskExitCritical();

    }
}

/* A task that blocks waiting to be notified that the peripheral
needs servicing. */
void vNI_TMR_HandlerTask( void *pvParameters ){
	BaseType_t xEvent;
	//const TickType_t xBlockTime = 1000000;
	uint32_t ulNotifiedValue;

    for( ;; ){
		/* Blocks the task until the NI interrupts it */
        ulNotifiedValue = ulTaskNotifyTake( pdFALSE,
                                            portMAX_DELAY );

        // if( ulNotifiedValue > 0 ){
        //     /* Perform any processing necessitated by the interrupt. */
		// 	UART_polled_tx_string( &g_uart, (const uint8_t *)" Rotina TIMER...\r\n" );
        // }
        // else{
        //     /* Did not receive a notification within the expected time. */
		// 	UART_polled_tx_string( &g_uart, (const uint8_t *)" Time out NI TIMER...\r\n" );
        // }

		// enters in critical mode 
		vTaskEnterCritical();
	
            powerEstimation(); // estimate the power consumed by this PE and send it to the TEA
            HW_set_32bit_reg(NI_TIMER, DONE);

		// exit critical mode
        vTaskExitCritical();

    }
}

/*-----------------------------------------------------------*/
#if THERMAL_MANAGEMENT == 0 // SPIRAL

static void GlobalManagerTask( void *pvParameters ){
	( void ) pvParameters;
	unsigned int tick;
	char str[20];

	// Initialize the priority vector with the spiral policy
	generateSpiralMatrix();

	// Initialize the System Tiles Info
	API_TilesReset();

	// Initialize the applications vector
    API_ApplicationsReset();

	// Informs the Repository that the GLOBALMASTER is ready to receive the application info
	API_RepositoryWakeUp();

	for(;;){
        // Returns from IDLE
		API_setFreqScale(1000);
        API_applyFreqScale();

        tick = xTaskGetTickCount();
        if(tick >= SIMULATION_MAX_TICKS) _exit(0xfe10);
		myItoa(tick, str, 10);
		UART_polled_tx_string( &g_uart, (const uint8_t *)str);
		printsv("GlobalMasterActive", tick);
		UART_polled_tx_string( &g_uart, (const uint8_t *)" GlobalMasterRoutine...\r\n" );

		// Checks if there is some task to allocate...
		API_AllocateTasks(tick);
		
		// Checks if there is some task to start...
		API_StartTasks();

        // Enters in idle
        API_setFreqIdle();
        API_applyFreqScale();
        
		if(API_SystemFinish){
			vTaskDelay(100); // to cool down the system
			_exit(0xfe10);
		}
		else{
			vTaskDelay(1);
		}
	}
}

#elif THERMAL_MANAGEMENT == 1 // PATTERN

static void GlobalManagerTask( void *pvParameters ){
	( void ) pvParameters;
	unsigned int tick;
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
		API_setFreqScale(1000);
        API_applyFreqScale();
        tick = xTaskGetTickCount();
        if(tick >= SIMULATION_MAX_TICKS) _exit(0xfe10);
		myItoa(tick, str, 10);
		UART_polled_tx_string( &g_uart, (const uint8_t *)str);
		printsv("GlobalMasterActive", tick);
		UART_polled_tx_string( &g_uart, (const uint8_t *)" GlobalMasterRoutine...\r\n" );

		// Checks if there is some task to allocate...
		API_AllocateTasks(tick);
		
		// Checks if there is some task to start...
		API_StartTasks();

        // Enters in idle
        API_setFreqIdle();
        API_applyFreqScale();
        
		if(API_SystemFinish){
			vTaskDelay(100); // to cool down the system
			_exit(0xfe10);
		}
		else{
			vTaskDelay(1);
		}
	}
}

#elif THERMAL_MANAGEMENT == 2 // PIDTM

static void GlobalManagerTask( void *pvParameters ){
    ( void ) pvParameters;
    unsigned int tick;
    int migrate = 1;
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
        API_setFreqScale(1000);
        API_applyFreqScale();
        tick = xTaskGetTickCount();
        if(tick >= SIMULATION_MAX_TICKS) _exit(0xfe10);
		myItoa(tick, str, 10);
		UART_polled_tx_string( &g_uart, (const uint8_t *)str);
		printsv("GlobalMasterActive", tick);
		UART_polled_tx_string( &g_uart, (const uint8_t *)" GlobalMasterRoutine...\r\n" );

        // Update the system temperature
        API_UpdateTemperature();

        // if(tick > 30 && migrate == 1){
        //     API_StartMigration(0, 3, 0x00000100);
        //     migrate = 0;
        // }

        // Update PID
        API_UpdatePIDTM();

        // Update priority table based on the PID value of each PE
        API_UpdatePriorityTable(pidStatus.control_signal);

        // Checks if there is some task to allocate...
		API_AllocateTasks(tick);
		
		// Checks if there is some task to start...
		API_StartTasks();
        
        // Enters in idle
        API_setFreqIdle();
        API_applyFreqScale();
        
		if(API_SystemFinish){
			vTaskDelay(100); // to cool down the system
			_exit(0xfe10);
		}
		else {
			vTaskDelay(1);
		}

    }
}


#elif THERMAL_MANAGEMENT == 3 // CHRONOS

#define N_TASKTYPE  3
#define N_ACTIONS   6

// ACTIONS
#define HIG_TEMPERATURE 0
#define LOW_TEMPERATURE 1
#define HIG_TEMP_VAR    2
#define LOW_TEMP_VAR    3
#define HIG_FIT         4
#define LOW_FIT         5

extern float policyTable[N_TASKTYPE][N_ACTIONS] = { {201.661, 205.822, 199.128, 206.386, 199.081, 228.746},
                                                    {261.975, 253.904, 256.355, 261.436, 311.695, 258.952},
                                                    {255.624, 250.257, 247.782, 232.598, 240.246, 277.564} };

static void GlobalManagerTask( void *pvParameters ){
    ( void ) pvParameters;
	unsigned int tick;
	char str[20];
    int x, y;
    unsigned int apptask, app, task, slot;
    unsigned int addr, j, i;
    //---------------------------------------
    // --------- Q-learning stuff -----------
    unsigned int tp, action;
    // Hyperparameters

    float epsilon = 0.1; // 0.100 in fixed point
    // lists to consult
    unsigned int temperature_list[DIM_X*DIM_Y];
    unsigned int tempVar_list[DIM_X*DIM_Y];
    unsigned int fit_list[DIM_X*DIM_Y];
    
    // policy table initialization
    /*for(tp = 0; tp < N_TASKTYPE; tp++){
        for(action = 0; action < N_ACTIONS; action++){
            policyTable[tp][action] = 0;
        }
    }*/

	// Initialize the System Tiles Info
	API_TilesReset();

	// Initialize the applications vector
    API_ApplicationsReset();

	// Informs the Repository that the GLOBALMASTER is ready to receive the application info
	API_RepositoryWakeUp();

	for(;;){
		API_setFreqScale(1000);
        API_applyFreqScale();
        tick = xTaskGetTickCount();
        if(tick >= SIMULATION_MAX_TICKS) _exit(0xfe10);
		myItoa(tick, str, 10);
		UART_polled_tx_string( &g_uart, (const uint8_t *)str);
		printsv("GlobalMasterActive", tick);
		UART_polled_tx_string( &g_uart, (const uint8_t *)" GlobalMasterRoutine...\r\n" );

        // Checks if there is some task to allocate
        if(API_CheckTaskToAllocate(tick)){
            // Sorts PEs by certain attributes (temperature, temperature variation, fit)
            API_SortAllocationVectors(temperature_list, tempVar_list, fit_list);

            apptask = API_GetNextTaskToAllocate(tick);
            printsv("apptask: ", apptask);
            do{

                app = (apptask & 0xFFFF0000) >> 16;
                task = (apptask & 0x0000FFFF);
                printsvsv("Allocating task ", task, "from app ", app);

                if( (int)(epsilon*100) < random()%100 ){
                    action = random() % N_ACTIONS; // Explore action space
                } else{
                    action = API_getMaxIdxfromRow(policyTable, applications[app].taskType[task], N_ACTIONS, N_TASKTYPE); // Exploit learned values
                }

                // register the taked action 
                applications[app].takedAction[task] = action;

                // Runs the selected action
                switch(action){
                    case HIG_TEMPERATURE: // allocate in the PE with the higher temperatures
                        for(i = (DIM_X*DIM_Y)-1; i >= 0; i--){
                            addr = temperature_list[i];
                            slot = API_GetTaskSlotFromTile(addr, app, task);
                            if (slot != ERRO) break;
                        }
                        break;
                    
                    case LOW_TEMPERATURE: // allocate in the PE with the lower temperature
                        for(i = 0; i < (DIM_X*DIM_Y); i++){
                            addr = temperature_list[i];
                            slot = API_GetTaskSlotFromTile(addr, app, task);
                            if (slot != ERRO) break;
                        }
                        break; 

                    case HIG_TEMP_VAR: // allocate in the PE with the higher temperature variation
                        for(i = (DIM_X*DIM_Y)-1; i >= 0; i--){
                            addr = tempVar_list[i];
                            slot = API_GetTaskSlotFromTile(addr, app, task);
                            if (slot != ERRO) break;
                        }
                        break;

                    case LOW_TEMP_VAR: // allocate in the PE with the lower temperature variation
                        for(i = 0; i < (DIM_X*DIM_Y); i++){
                            addr = tempVar_list[i];
                            slot = API_GetTaskSlotFromTile(addr, app, task);
                            if (slot != ERRO) break;
                        }
                        break; 

                    case HIG_FIT: // allocate in the PE with the higher FIT
                        for(i = (DIM_X*DIM_Y)-1; i >= 0; i--){
                            addr = fit_list[i];
                            slot = API_GetTaskSlotFromTile(addr, app, task);
                            if (slot != ERRO) break;
                        }
                        break;
                    
                    case LOW_FIT: // allocate in the PE with the lower FIT
                        for(i = 0; i < (DIM_X*DIM_Y); i++){
                            addr = fit_list[i];
                            slot = API_GetTaskSlotFromTile(addr, app, task);
                            if (slot != ERRO) break;
                        }
                        break; 

                    default:
                        prints("Erro ao escolher a acao\n");
                        break;
                }
                
                // register the application allocation
                applications[app].tasks[task].addr = addr;
                applications[app].tasks[task].slot = slot;
                applications[app].lastStart = applications[app].nextRun;
                API_RepositoryAllocation(app, task, addr);
                printsvsv("Allocated at: ", getXpos(addr), "- ", getYpos(addr));

                // gets the next task to allocate
                apptask = API_GetNextTaskToAllocate(tick);  
                printsv("new apptask: ", apptask);
            }while(apptask != 0xFFFFFFFF);

        }
		
		// Checks if there is some task to start...
		API_StartTasks();

        // Enters in idle
        API_setFreqIdle();
        API_applyFreqScale();
        
		if(API_SystemFinish){
            API_PrintPolicyTable();
			vTaskDelay(100); // to cool down the system
			_exit(0xfe10);
		}
		else{
			vTaskDelay(1);
		}
	}
}

#elif THERMAL_MANAGEMENT == 5 // CHRONOS2

#define N_TASKTYPE  3
#define S_EMPTY     N_TASKTYPE
#define S_1_DIAG    (1*N_TASKTYPE)*N_TASKTYPE
#define S_2_DIAG    (2*N_TASKTYPE)*N_TASKTYPE
#define S_3_DIAG    (3*N_TASKTYPE)*N_TASKTYPE
#define S_4_DIAG    (4*N_TASKTYPE)*N_TASKTYPE

#define N_ACTIONS   6

// ACTIONS
#define HIG_TEMPERATURE 0
#define LOW_TEMPERATURE 1
#define HIG_TEMP_VAR    2
#define LOW_TEMP_VAR    3
#define HIG_FIT         4
#define LOW_FIT         5

extern float policyTable[N_TASKTYPE][N_ACTIONS];

static void GlobalManagerTask( void *pvParameters ){
    ( void ) pvParameters;
	unsigned int tick;
	char str[20];
    int x, y;
    unsigned int apptask, app, task, slot;
    unsigned int addr, j, i;
    //---------------------------------------
    // --------- Q-learning stuff -----------
    unsigned int tp, action;
    // Hyperparameters

    float epsilon = 0.1; // 0.100 in fixed point
    // lists to consult
    unsigned int temperature_list[DIM_X*DIM_Y];
    unsigned int tempVar_list[DIM_X*DIM_Y];
    unsigned int fit_list[DIM_X*DIM_Y];
    
    // policy table initialization
    for(tp = 0; tp < N_TASKTYPE; tp++){
        for(action = 0; action < N_ACTIONS; action++){
            policyTable[tp][action] = 0;
        }
    }

	// Initialize the System Tiles Info
	API_TilesReset();

	// Initialize the applications vector
    API_ApplicationsReset();

	// Informs the Repository that the GLOBALMASTER is ready to receive the application info
	API_RepositoryWakeUp();

	for(;;){
		API_setFreqScale(1000);
        API_applyFreqScale();
        tick = xTaskGetTickCount();
        if(tick >= SIMULATION_MAX_TICKS) _exit(0xfe10);
		myItoa(tick, str, 10);
		UART_polled_tx_string( &g_uart, (const uint8_t *)str);
		printsv("GlobalMasterActive", tick);
		UART_polled_tx_string( &g_uart, (const uint8_t *)" GlobalMasterRoutine...\r\n" );

        // Checks if there is some task to allocate
        if(API_CheckTaskToAllocate(tick)){
            // Sorts PEs by certain attributes (temperature, temperature variation, fit)
            API_SortAllocationVectors(temperature_list, tempVar_list, fit_list);

            apptask = API_GetNextTaskToAllocate(tick);
            printsv("apptask: ", apptask);
            do{

                app = (apptask & 0xFFFF0000) >> 16;
                task = (apptask & 0x0000FFFF);
                printsvsv("Allocating task ", task, "from app ", app);

                if( (int)(epsilon*100) < random()%100 ){
                    action = random() % N_ACTIONS; // Explore action space
                } else{
                    action = API_getMaxIdxfromRow(policyTable, applications[app].taskType[task], N_ACTIONS, N_TASKTYPE); // Exploit learned values
                }

                // register the taked action 
                applications[app].takedAction[task] = action;

                // Runs the selected action
                switch(action){
                    case HIG_TEMPERATURE: // allocate in the PE with the higher temperatures
                        for(i = (DIM_X*DIM_Y)-1; i >= 0; i--){
                            addr = temperature_list[i];
                            slot = API_GetTaskSlotFromTile(addr, app, task);
                            if (slot != ERRO) break;
                        }
                        break;
                    
                    case LOW_TEMPERATURE: // allocate in the PE with the lower temperature
                        for(i = 0; i < (DIM_X*DIM_Y); i++){
                            addr = temperature_list[i];
                            slot = API_GetTaskSlotFromTile(addr, app, task);
                            if (slot != ERRO) break;
                        }
                        break; 

                    case HIG_TEMP_VAR: // allocate in the PE with the higher temperature variation
                        for(i = (DIM_X*DIM_Y)-1; i >= 0; i--){
                            addr = tempVar_list[i];
                            slot = API_GetTaskSlotFromTile(addr, app, task);
                            if (slot != ERRO) break;
                        }
                        break;

                    case LOW_TEMP_VAR: // allocate in the PE with the lower temperature variation
                        for(i = 0; i < (DIM_X*DIM_Y); i++){
                            addr = tempVar_list[i];
                            slot = API_GetTaskSlotFromTile(addr, app, task);
                            if (slot != ERRO) break;
                        }
                        break; 

                    case HIG_FIT: // allocate in the PE with the higher FIT
                        for(i = (DIM_X*DIM_Y)-1; i >= 0; i--){
                            addr = fit_list[i];
                            slot = API_GetTaskSlotFromTile(addr, app, task);
                            if (slot != ERRO) break;
                        }
                        break;
                    
                    case LOW_FIT: // allocate in the PE with the lower FIT
                        for(i = 0; i < (DIM_X*DIM_Y); i++){
                            addr = fit_list[i];
                            slot = API_GetTaskSlotFromTile(addr, app, task);
                            if (slot != ERRO) break;
                        }
                        break; 

                    default:
                        prints("Erro ao escolher a acao\n");
                        break;
                }
                
                // register the application allocation
                applications[app].tasks[task].addr = addr;
                applications[app].tasks[task].slot = slot;
                applications[app].lastStart = applications[app].nextRun;
                API_RepositoryAllocation(app, task, addr);
                printsvsv("Allocated at: ", getXpos(addr), "- ", getYpos(addr));

                // gets the next task to allocate
                apptask = API_GetNextTaskToAllocate(tick);  
                printsv("new apptask: ", apptask);
            }while(apptask != 0xFFFFFFFF);

        }
		
		// Checks if there is some task to start...
		API_StartTasks();

        // Enters in idle
        API_setFreqIdle();
        API_applyFreqScale();
        
		if(API_SystemFinish){
            API_PrintPolicyTable();
			vTaskDelay(100); // to cool down the system
			_exit(0xfe10);
		}
		else{
			vTaskDelay(1);
		}
	}
}

#elif THERMAL_MANAGEMENT == 4 // CHARACTERIZE

static void GlobalManagerTask( void *pvParameters ){
    ( void ) pvParameters;
	unsigned int tick;
	char str[20];
    int x, y, i;
	// Initialize the priority vector with the pattern policy
    x = DIM_X-1;
    y = DIM_Y-1;
    for(i=0; i<(DIM_X*DIM_Y); i++){
        priorityMatrix[i] = (x << 8) | y;
        x--;
        if ( x < 0 ){
            x = DIM_X-1;
            y--;
        }
    }
    //priorityMatrix = {0x404, 0x403, 0x402, 0x401, 0x400, 0x304, 0x303, 0x302, 0x301, 0x300, 0x204, 0x203, 0x202, 0x201, 0x200, 0x104, 0x103, 0x102, 0x101, 0x100, 0x004, 0x003, 0x002, 0x001, 0x000};

	// Initialize the System Tiles Info
	API_TilesReset();

	// Initialize the applications vector
    API_ApplicationsReset();

	// Informs the Repository that the GLOBALMASTER is ready to receive the application info
	API_RepositoryWakeUp();

    vTaskDelay(1);
	for(;;){
		API_setFreqScale(1000);
        API_applyFreqScale();
        tick = xTaskGetTickCount();
        printsv("tick: ", tick);
        if(tick >= SIMULATION_MAX_TICKS) _exit(0xfe10);
		myItoa(tick, str, 10);
		UART_polled_tx_string( &g_uart, (const uint8_t *)str);
		printsv("GlobalMasterActive", tick);
		UART_polled_tx_string( &g_uart, (const uint8_t *)" GlobalMasterRoutine...\r\n" );

		// Checks if there is some task to allocate...
		API_AllocateTasks(tick);
		
		// Checks if there is some task to start...
		API_StartTasks();

        // Enters in idle
        API_setFreqIdle();
        API_applyFreqScale();
        
		if(API_SystemFinish){
			//vTaskDelay(100); // to cool down the system
			_exit(0xfe10);
		}
		else{
			vTaskDelay(1);
		}
	}
}


#endif

/*static void KeeperTask( void *pvParameters ){
    BaseType_t xEvent;
	//const TickType_t xBlockTime = 1000000;
	uint32_t ulNotifiedValue;
    int i;
    for( ;; ){
		// Blocks the task until some task finishes up 
        ulNotifiedValue = ulTaskNotifyTake( pdFALSE,
                                            portMAX_DELAY );

        prints("KeeperTask running...\n");

        vTaskEnterCritical();

        for(i = 0; i < NUM_MAX_TASKS; i++){
            if(TaskList[i].status == TASK_SLOT_FINISH){
                vPortFree(TaskList[i].fullAddr);
                prints("free done \n");
                prints("deleting... \n");
                vTaskDelete(TaskList[i].TaskHandler);
                prints("deleted! \n");
                TaskList[i].status = TASK_SLOT_EMPTY;
                API_SendFinishTask(TaskList[i].TaskID, TaskList[i].AppID);
            }
        }

        vTaskExitCritical();

        // checks if it's necessary to reduce the frequency
        for(i = 0; i < NUM_MAX_TASKS; i++){
            printsvsv("TaskList[", i, "]status: ", TaskList[i].status );
            if(TaskList[i].status != TASK_SLOT_EMPTY){
                printsvsv("Returning because of: ", i, "TaskList[i].status ", TaskList[i].status);
                i = 0xffffffff;
                break;
            }
        }
        prints("Loops out \n");
        if(i != 0xffffffff){
            prints("Applying freq reduction \n");
            API_setFreqIdle();
        }
        prints("Done.!.\n");

    }
}*/

