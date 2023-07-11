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
    5) CHRONOS2
    6) CHRONOS3
    7) WORST */
#define THERMAL_MANAGEMENT 3
#define MIGRATION_AVAIL 1
#define THRESHOLD_TEMP 78

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
	myItoa(API_getProcessorAddr(), str, 16);
    UART_polled_tx_string( &g_uart, (const uint8_t *)str); UART_polled_tx_string( &g_uart, (const uint8_t *)"\n");
	
	/* Create the NI Handler task */
	xTaskCreate( vNI_RX_HandlerTask, "RX_TASK", 1024, NULL, (tskIDLE_PRIORITY + 2), &NI_RX_Handler );
	xTaskCreate( vNI_TX_HandlerTask, "TX_TASK", 1024, NULL, (tskIDLE_PRIORITY + 3), &NI_TX_Handler );
	xTaskCreate( vNI_TMR_HandlerTask, "TMR_TASK", 1024, NULL, (tskIDLE_PRIORITY + 3), &NI_TMR_Handler );

	if (API_getProcessorAddr() == 0x0000){
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
                                             incommingPacket.application_n_tasks,
                                             incommingPacket.deadline);
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
                        printsv("Mensagem encontrada no pipe slot: ", (0x000000FF & aux));
                        API_PushSendQueue(MESSAGE, aux);
                        // API_Try2Send();
                    }
                    //HW_set_32bit_reg(NI_RX, DONE);
                    prints("8NI_RX DONE!\n");
                    break;
                
                case MESSAGE_DELIVERY:
                    prints("Tem uma mensagem chegando...\n");
                    aux = API_GetTaskSlot(incommingPacket.destination_task, incommingPacket.application_id);
                    if(aux != ERRO) {
                        incommingPacket.service = MESSAGE_DELIVERY_FINISH;
                        HW_set_32bit_reg(NI_RX, TaskList[aux].MsgToReceive);
                    }
                    else{
                        prints("WARNING! - Destinatário não presente!\n");
                        while(ServiceMessage.status == PIPE_OCCUPIED){
                            // Runs the NI Handler to send/receive packets, opening space in the PIPE
                            prints("Estou preso aqui777...\n");
                            vTaskExitCritical();
                            asm("nop");
                            vTaskEnterCritical();
                        }
                        ServiceMessage.status = PIPE_OCCUPIED;

                        if(API_getProcessorAddr() == MASTER_ADDR){
                            ServiceMessage.header.header           = applications[incommingPacket.application_id].tasks[incommingPacket.destination_task].addr;
                        } else {
                            ServiceMessage.header.header           = MASTER_ADDR;
                        }
                        ServiceMessage.header.payload_size     = incommingPacket.payload_size;
                        ServiceMessage.header.service          = MESSAGE_DELIVERY;
                        ServiceMessage.header.application_id   = incommingPacket.application_id;
                        ServiceMessage.header.producer_task    = incommingPacket.producer_task;
                        ServiceMessage.header.destination_task = incommingPacket.destination_task;
                        incommingPacket.service                = MESSAGE_RETARGET;
                        HW_set_32bit_reg(NI_RX, (unsigned int)&ServiceMessage.msg);
                        API_PushSendQueue(SYS_MESSAGE, 0);
                    }
                    prints("9NI_RX DONE!\n");
                    break;
                
                case MESSAGE_RETARGET:
                    printsv("Reencaminhando mensagem para PE: ", ServiceMessage.header.header );
                    API_PushSendQueue(SYS_MESSAGE, 0);
                    prints("9.1NI_RX_DONE!");
                    break;


                case MESSAGE_DELIVERY_FINISH:
                    //prints("Terminou de entregar a mensagem!!\n");
                    aux = API_GetTaskSlot(incommingPacket.destination_task, incommingPacket.application_id);
                    TaskList[aux].waitingMsg = FALSE;
                    prints("10NI_RX DONE!\n");
                    vTaskResume(TaskList[aux].TaskHandler);
                    //xTaskResumeFromISR(TaskList[aux].TaskHandler);
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
                        printsvsv("=====\nx: ", getXpos(id2addr(aux)), "y: ", getYpos(id2addr(aux)));
                        printsv("temp: ", SystemTemperature[aux]);
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
                    API_UpdateFIT();
                    /*for(aux = 0; aux < DIM_X*DIM_Y; aux++){ 
                        printsvsv("=====\nx: ", getXpos(id2addr(aux)), "y: ", getYpos(id2addr(aux)));
                        //printsv("temp: ", SystemTemperature[aux]);
                        printsv("FIT: ", Tiles[getXpos(id2addr(aux))][getYpos(id2addr(aux))].fit);
                        printsv("getFit: ", API_getFIT(id2addr(aux)));
                    }*/
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
                    API_InformMigration(incommingPacket.app_id, incommingPacket.task_id, incommingPacket.task_migration_addr);
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

                    //vTaskEnterCritical();
                    //printsvsv("fmig~~~> ", incommingPacket.app_id, "task ", incommingPacket.task_id);
                    //vTaskExitCritical();
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
                    
                    log_migration_success(xTaskGetTickCount());


                    API_ClearTaskSlotFromTile(applications[incommingPacket.app_id].tasks[incommingPacket.task_id].addr,
                                              incommingPacket.app_id, 
                                              incommingPacket.task_id );

                    applications[incommingPacket.app_id].tasks[incommingPacket.task_id].addr = applications[incommingPacket.app_id].newAddr;
                    API_ReleaseApplication(incommingPacket.app_id, incommingPacket.task_id);
                    break;
                
                case TASK_RESUME:
                    prints("26NI_RX DONE!\n");
                    prints("Chegou um TASK_RESUME!\n");
                    aux = API_GetTaskSlot(incommingPacket.task_id, incommingPacket.app_id);
                    if(aux!=ERRO){
                        // Informs the NI were to write the application
                        HW_set_32bit_reg(NI_RX, (unsigned int)&TaskList[aux].appNumTasks);
                    } else {
                        prints("Task nao encontrada!\n");
                        aux = API_GetFreeTaskSlot();
                        HW_set_32bit_reg(NI_RX, (unsigned int)&TaskList[aux].appNumTasks);
                    }
                    incommingPacket.service = TASK_RESUME_FINISH;
                    //HW_set_32bit_reg(NI_RX, DONE);
                    break;
                
                case TASK_RESUME_FINISH:
                    prints("27NI_RX DONE!\n");
                    API_UpdatePipe(incommingPacket.task_id, incommingPacket.app_id);
                    //API_setFreqScale(1000);
                    API_ResumeTask(incommingPacket.task_id, incommingPacket.app_id);
                    break;

                case LOST_MESSAGE_REQ:
                    prints("28NI_RX_DONE!\n");
                    API_Forward_MsgReq(incommingPacket.task_id, incommingPacket.app_id, incommingPacket.producer_task_id);
                    break;

                case TASK_REFUSE_MIGRATION:
                    prints("29NI_RX_DONE!\n");
                    printsv("Migration refused for address: ", incommingPacket.task_migration_addr);
                    API_Migration_Refused(incommingPacket.task_id, incommingPacket.app_id, incommingPacket.why, incommingPacket.task_migration_addr);
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
        
		// exits the critical mode
		vTaskExitCritical();

    }
}

/* A task that blocks waiting to be notified that the peripheral
needs servicing. */
void vNI_TMR_HandlerTask( void *pvParameters ){
	BaseType_t xEvent;
    int slot;
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


        // informs if any task has finished in the last period
        for(slot = 0; slot < NUM_MAX_TASKS; slot++){
            if( TaskList[slot].status == TASK_SLOT_TO_FREE &&  TaskList[slot].TaskHandler == 0xFFFFFFFF ){
                TaskList[slot].status = TASK_SLOT_EMPTY;
                API_SendFinishTask(TaskList[slot].TaskID, TaskList[slot].AppID);
            }
        }

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

        // prints the occupation
        API_PrintOccupation(tick);    

		// Checks if there is some task to allocate...
		API_AllocateTasks(tick, -1);
		
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

        // prints the occupation
        API_PrintOccupation(tick);

		// Checks if there is some task to allocate...
		API_AllocateTasks(tick, -1);
		
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

#elif THERMAL_MANAGEMENT == 2 // PIDTM

static void GlobalManagerTask( void *pvParameters ){
    ( void ) pvParameters;
    unsigned int tick, k;
	char str[20];
    unsigned int migrate, migtick, mig_app, mig_task, mig_addr, mig_slot;
    migtick = 0;

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

        // prints the occupation
        API_PrintOccupation(tick);

        // Update the system temperature
        API_UpdateTemperature();

        // Update PID
        API_UpdatePIDTM();

        // Update priority table based on the PID value of each PE
        API_UpdatePriorityTable(pidStatus.control_signal);

        // Checks if there is some task to allocate...
		API_AllocateTasks(tick, -1);

        // Checks if there is some task to start...
		API_StartTasks();

        // Check migration
#if MIGRATION_AVAIL
        migrate = API_SelectTask_Migration_Temperature(THRESHOLD_TEMP);

        if (migrate != -1  && !API_CheckTaskToAllocate(xTaskGetTickCount()) && migtick+5 <= xTaskGetTickCount()){
            printsvsv("Got a app to migrate: ",(migrate>>16)," task: ", (migrate & 0x0000FFFF));
            mig_app = migrate >> 16;
            mig_task = migrate & 0x0000FFFF;
            for(k=0;k<(DIM_X*DIM_Y)-1;k++){
                mig_addr = getNextPriorityAddr(-1);
                printsv("mig_addr = ", mig_addr);
                mig_slot = API_GetTaskSlotFromTile(mig_addr, mig_app, mig_task);
                if (mig_slot != ERRO){
                    printsvsv("Migrating it to ", getXpos(mig_addr), "-", getYpos(mig_addr));
                    API_StartMigration(mig_app, mig_task, mig_addr);
                    migtick = xTaskGetTickCount();
                    break;
                }   
            }
        }
#endif
        
        // Enters in idle
        API_setFreqIdle();
        API_applyFreqScale();
        
		if(API_SystemFinish){
			//vTaskDelay(100); // to cool down the system
			_exit(0xfe10);
		}
		else {
			vTaskDelay(1);
		}

    }
}


#elif THERMAL_MANAGEMENT == 3 // CHRONOS

static void GlobalManagerTask( void *pvParameters ){
    ( void ) pvParameters;
	unsigned int tick, toprint;

    // avg window (32 fit values)
    float scoreTable[N_TASKTYPE][N_STATES] ={ {158752.0, 151306.0, 141767.0, 101257.0, 30320.0, 150988.0, 139441.0, 128386.0, 46420.0, 136751.0, 133097.0, 87598.0, 128093.0, 48736.0, 40427.0, 156500.0, 143513.0, 129063.0, 71294.0, 140630.0, 133150.0, 114801.0, 135749.0, 118135.0, 104357.0, 147431.0, 141911.0, 108081.0, 140140.0, 130474.0, 118725.0, 151058.0, 138647.0, 125856.0, 81869.0  },
                                              {134928.0, 123406.0, 105842.0, 72358.0, 2556.0, 123309.0, 105027.0, 96300.0, 16705.0, 112223.0, 101299.0, 63861.0, 106332.0, 82031.0, 60822.0, 115340.0, 103802.0, 100406.0, 63490.0, 123720.0, 105601.0, 99110.0, 108154.0, 88274.0, 93688.0, 121426.0, 105191.0, 87878.0, 108250.0, 98963.0, 105731.0, 111918.0, 100972.0, 107116.0, 62499.0  },
                                              {155664.0, 145902.0, 124081.0, 126903.0, 8709.0, 140733.0, 125602.0, 112981.0, 33636.0, 111199.0, 105202.0, 41548.0, 104730.0, 23392.0, 0.0, 143522.0, 125388.0, 127966.0, 18118.0, 122550.0, 115151.0, 98306.0, 129990.0, 104634.0, 40216.0, 129079.0, 120941.0, 108770.0, 129943.0, 110096.0, 110038.0, 134345.0, 99074.0, 96193.0, 96179.0  } };

    char str[20];
    int x, y;
    unsigned int apptask, app, task, slot, taskType, cluster, base_addr, cluster_size, last_app;
    unsigned int addr, j, i, k, currentAddr;
    unsigned int state_stability[DIM_X*DIM_Y];
    unsigned int state_last[DIM_X*DIM_Y];

    int starting_fit[DIM_X*DIM_Y];
    int current_fit;

    static unsigned int sorted_addr[DIM_X*DIM_Y];
    static unsigned int sorted_score[DIM_X*DIM_Y];

    unsigned int checkMigration, migrate, migtick, mig_app, mig_task, mig_addr, num_pes;
    migtick = 0;
    float candidate_score;
                        
    checkMigration = 200;
    //---------------------------------------
    // --------- Q-learning stuff -----------
    unsigned int tp, state, maxid;
    // Hyperparameters
    float epsilon = 0.1;
    float alpha = 0.1;
    float gamma = 0.6;
    float oldvalue, maxval, reward, delta;

    //policy table initialization
    for(tp = 0; tp < N_TASKTYPE; tp++){
        for(state = 0; state < N_STATES; state++){
            scoreTable[tp][state] = scoreTable[tp][state]/1000;
        }
    }
    API_PrintScoreTable(scoreTable);

	// Initialize the System Tiles Info
	API_TilesReset();

    Tiles[getXpos(GLOBAL_MASTER_ADDR)][getYpos(GLOBAL_MASTER_ADDR)].taskType = 1;
    Tiles[getXpos(GLOBAL_MASTER_ADDR)][getYpos(GLOBAL_MASTER_ADDR)].clusterCount = 1;

	// Initialize the applications vector
    API_ApplicationsReset();

	// Informs the Repository that the GLOBALMASTER is ready to receive the application info
	API_RepositoryWakeUp();

	for(;;){
        // sets the frequency to 100% available (1GHz)
		API_setFreqScale(1000);
        API_applyFreqScale();

        tick = xTaskGetTickCount();
        if(tick >= SIMULATION_MAX_TICKS) _exit(0xfe10);
		myItoa(tick, str, 10);
		UART_polled_tx_string( &g_uart, (const uint8_t *)str);
		printsv("GlobalMasterActive", tick);
		UART_polled_tx_string( &g_uart, (const uint8_t *)" GlobalMasterRoutine...\r\n" );

        // prints the occupation
        API_PrintOccupation(tick);

        // Checks if there is some task to allocate
        if(API_CheckTaskToAllocate(xTaskGetTickCount())){
            
            last_app = -1;

            while(1){

                apptask = API_GetNextTaskToAllocate(xTaskGetTickCount());
                if (apptask == 0xFFFFFFFF) break;
                app = (apptask & 0xFFFF0000) >> 16;
                task = (apptask & 0x0000FFFF);
                printsvsv("Allocating task ", task, "from app ", app);
                if(last_app != app){
                    API_FindSmallerFITCluster(app);
                    base_addr = applications[app].cluster_addr;
                    cluster_size = applications[app].cluster_size;
                    printsvsv("Cluster addr: ", base_addr, " cluster size: ", cluster_size);
                }

                // fill the auxiliar vectors
                k = 0;
                for( i = getXpos(base_addr); i < (getXpos(base_addr)+cluster_size); i++){
                    for( j = getYpos(base_addr); j < (getYpos(base_addr)+cluster_size); j++){
                        if(Tiles[i][j].taskSlots > 0 && makeAddress(i, j) != MASTER_ADDR){
                            sorted_addr[k] = makeAddress(i, j);
                            sorted_score[k] = (int)(scoreTable[applications[app].taskType[task]][API_getPEState(addr2id(makeAddress(i,j)), -1)]*1000);
                            k++;
                        }
                    }
                }
                
                // sort addrs by score
                num_pes = k-1; 

                printsv("Sorting num_pes = ", num_pes);
                quickSort(sorted_score, sorted_addr, 0, num_pes);
                
                // try to get the best tile slot (higher score)
                for(i = num_pes; i >= 0; i--){
                    addr = sorted_addr[i];
                    slot = API_GetTaskSlotFromTile(addr, app, task);
                    if (slot != ERRO) break;
                }
            

                // register the application allocation
                applications[app].tasks[task].addr = addr;
                printsvsv("X: ", getXpos(addr), "Y: ", getYpos(addr));
                applications[app].tasks[task].slot = slot;
                applications[app].lastStart = applications[app].nextRun;
                API_RepositoryAllocation(app, task, addr);
                debug_task_alloc(tick, app, task, addr);

                last_app = app;

                // Checks if there is some task to start...
                API_StartTasks();
            }

        }
#if MIGRATION_AVAIL
        else{
            migrate = API_SelectTask_Migration_Temperature(THRESHOLD_TEMP);
            if (migrate != ERRO  && !API_CheckTaskToAllocate(xTaskGetTickCount()) && migtick+5 <= xTaskGetTickCount() ){
                printsvsv("Got a app to migrate: ",(migrate>>16)," task: ", (migrate & 0x0000FFFF));
                mig_app = migrate >> 16;
                mig_task = migrate & 0x0000FFFF;
                currentAddr = applications[mig_app].tasks[mig_task].addr;
                // expands the cluster to the system size
                base_addr = makeAddress(0,0); //applications[mig_app].cluster_addr;
                cluster_size = DIM_X; //applications[mig_app].cluster_size;

                // fill the auxiliar vectors
                k = 0;
                for( i = getXpos(base_addr); i < (getXpos(base_addr)+cluster_size); i++){
                    for( j = getYpos(base_addr); j < (getYpos(base_addr)+cluster_size); j++){
                        if(Tiles[i][j].taskSlots > 0 && makeAddress(i, j) != MASTER_ADDR && Tiles[i][j].temperature < (THRESHOLD_TEMP+273)*100){
                            sorted_addr[k] = makeAddress(i, j);
                            sorted_score[k] =  Tiles[i][j].temperature;
                            k++;
                        }
                    }
                }
                if( k > 0 ){ 
                    // sort addrs by temperature
                    num_pes = k-1;
                    printsv("Sorting temp num_pes = ", num_pes);
                    quickSort(sorted_score, sorted_addr, 0, num_pes);
                    
                    // gets the Q-value for the first quartile
                    if( num_pes > 16){
                        num_pes = (num_pes+1) >> 2; // divides by four
                    } else if( num_pes > 8){
                        num_pes = (num_pes+1) >> 1; // divides by two
                    } else {
                        num_pes = num_pes + 1;
                    }

                    for (i = 0; i < num_pes; i++){
                        sorted_score[i] = (int)(scoreTable[applications[mig_app].taskType[mig_task]][API_getPEState(addr2id(sorted_addr[i]), currentAddr)]*1000);                    
                    }

                    // sorts the lower quartile based in the Q-value
                    num_pes = num_pes-1;
                    printsv("Sorting q-value num_pes = ", num_pes);
                    quickSort(sorted_score, sorted_addr, 0, num_pes);

                    // try to get the best tile slot (lower temperature)
                    mig_addr = 0xFFFFFFFF;
                    prints("Checking... \n");
                    for(i = num_pes; i >= 0; i--){
                        printsv("checking sorted_addr[", i);
                        addr = sorted_addr[i];
                        slot = API_GetTaskSlotFromTile(addr, mig_app, mig_task);
                        if (slot != ERRO){
                            mig_addr = addr;
                            break;
                        }
                    }
                    if(mig_addr != 0xFFFFFFFF){
                        API_StartMigration(mig_app, mig_task, mig_addr);
                        printsvsv("Migrating it to ", getXpos(mig_addr), "- ", getYpos(mig_addr));
                        migtick = xTaskGetTickCount();
                    } else {
                        prints("Not found!\n");
                    }
                }
            }
        }
#endif

		// Checks if there is some task to start...
	    API_StartTasks();

        // Enters in idle
        API_setFreqIdle();
        API_applyFreqScale();
        if(API_SystemFinish){
            vTaskEnterCritical();
            for(i = 0; i <= (DIM_X); i++){
                for(j = 0; j <= (DIM_Y); j++){
                    printsvsv("PE: ", addr2id(i<<8|j), "cluster count: ", Tiles[i][j].clusterCount);
                }
            }
            vTaskExitCritical();
            //API_PrintPolicyTable();
            API_PrintScoreTable(scoreTable);
            //vTaskDelay(100); // to cool down the system
            _exit(0xfe10);
        }
        else if(!API_CheckTaskToAllocate(xTaskGetTickCount())){
            vTaskDelay(1);
        }
	}
}

#elif THERMAL_MANAGEMENT == 5 // CHRONOS2

static unsigned int objectiveState[3][6];
static unsigned int currentState[3][6];

void objective_next(int tasktype){
    int a, b, c, da, db, dc;
    a = objectiveState[tasktype][0];
    b = objectiveState[tasktype][1];
    c = objectiveState[tasktype][2];
    da = objectiveState[tasktype][3];
    db = objectiveState[tasktype][4];
    dc = objectiveState[tasktype][5];

    while(1){
        if( a <= 4 ){
            if(b <= 4 ){
                if( c <= 4 ){
                    if( da <= 4 ){
                        if( db <= 4 ){
                            if( dc <= 4 ){
                                dc++;
                                if( (a + b + c) <= 4 && (da + db + dc) <= 4 ){
                                    objectiveState[tasktype][0] = a;
                                    objectiveState[tasktype][1] = b;
                                    objectiveState[tasktype][2] = c; 
                                    objectiveState[tasktype][3] = da;
                                    objectiveState[tasktype][4] = db;
                                    objectiveState[tasktype][5] = dc;
                                    printsv("Proximo estado para task do tipo ", tasktype);
                                    printsvsv("a: ", a, "da: ", da);
                                    printsvsv("b: ", b, "db: ", db);
                                    printsvsv("c: ", c, "dc: ", dc);
                                    return;
                                }
                            } else{
                                dc = 0;
                                db++;
                            }
                        } else{
                            db = 0;
                            da++;
                        }
                    } else{
                        da = 0;
                        c++;
                    } 
                } else{ 
                    c = 0;
                    b++;
                }
            } else{
                b = 0;
                a++;
            }
        } else{
            a = 0;
            b = 0;
            c = 0;
            da = 0;
            db = 0;
            dc = 0;
        }
    }
}

static void GlobalManagerTask( void *pvParameters ){
    ( void ) pvParameters;
	unsigned int tick, toprint;
    float scoreTable[N_TASKTYPE][N_STATES] = {  {271347.0, 254031.0, 236328.0, 205831.0, 14294.0, 264462.0, 250463.0, 215664.0, 4333.0, 209411.0, 92699.0, 3564.0, 14946.0, 0.0, 0.0, 274843.0, 243447.0, 246065.0, 63284.0, 259235.0, 248745.0, 29062.0, 118821.0, 5774.0, 0.0, 252736.0, 241465.0, 74298.0, 237132.0, 65440.0, 0.0, 215270.0, 111361.0, 9092.0, 13318.0, 261298.0, 242690.0, 227420.0, 206546.0, 38600.0, 263261.0, 240122.0, 220126.0, 13987.0, 95716.0, 158259.0, 9986.0, 42201.0, 2637.0, 7588.0, 259951.0, 231373.0, 223292.0, 99011.0, 253208.0, 234203.0, 91245.0, 167121.0, 33123.0, 1991.0, 246577.0, 245460.0, 124013.0, 205955.0, 100378.0, 7129.0, 206431.0, 82496.0, 7170.0, 16275.0, 256268.0, 252098.0, 228037.0, 168778.0, 17971.0, 218810.0, 200490.0, 166793.0, 35580.0, 60555.0, 76474.0, 3296.0, 7350.0, 1671.0, 1859.0, 250966.0, 241735.0, 214900.0, 74305.0, 221601.0, 223638.0, 47319.0, 87012.0, 2878.0, 3512.0, 238493.0, 216606.0, 94660.0, 190412.0, 90122.0, 0.0, 174323.0, 103223.0, 22169.0, 14437.0, 179203.0, 173711.0, 154154.0, 65469.0, 10722.0, 91391.0, 67905.0, 36044.0, 1762.0, 12388.0, 10287.0, 4503.0, 3527.0, 1765.0, 1687.0, 184053.0, 209401.0, 132890.0, 51973.0, 83027.0, 87822.0, 43566.0, 1716.0, 12723.0, 5131.0, 140858.0, 102360.0, 71908.0, 36134.0, 58809.0, 5828.0, 54152.0, 74150.0, 17816.0, 7386.0, 17418.0, 43762.0, 15305.0, 7213.0, 4590.0, 4429.0, 8099.0, 1558.0, 28033.0, 9966.0, 12326.0, 13071.0, 1787.0, 1662.0, 5096.0, 39025.0, 25478.0, 24244.0, 82199.0, 15882.0, 17677.0, 103339.0, 1698.0, 9129.0, 1730.0, 48231.0, 37243.0, 144166.0, 1945.0, 98239.0, 19596.0, 0.0, 66271.0, 24956.0, 11057.0, 252895.0, 262748.0, 241666.0, 131801.0, 27200.0, 198852.0, 185983.0, 125226.0, 13394.0, 46521.0, 36974.0, 1265.0, 0.0, 0.0, 0.0, 270070.0, 254757.0, 209674.0, 27592.0, 150050.0, 179972.0, 11850.0, 18739.0, 2593.0, 0.0, 239184.0, 168198.0, 25945.0, 98657.0, 12852.0, 0.0, 91722.0, 14909.0, 5003.0, 0.0, 248065.0, 256640.0, 206847.0, 119751.0, 34151.0, 142878.0, 115633.0, 120127.0, 18811.0, 7752.0, 18377.0, 3614.0, 3490.0, 3143.0, 1666.0, 242113.0, 239756.0, 214314.0, 45686.0, 149652.0, 185354.0, 28434.0, 42713.0, 5219.0, 2037.0, 201569.0, 214502.0, 84384.0, 144430.0, 28389.0, 0.0, 101990.0, 23286.0, 26156.0, 20399.0, 173521.0, 187565.0, 152096.0, 32727.0, 8239.0, 62432.0, 91367.0, 70398.0, 9737.0, 11987.0, 6546.0, 7647.0, 4805.0, 3362.0, 2669.0, 134665.0, 198209.0, 137680.0, 51445.0, 77925.0, 75862.0, 51329.0, 5266.0, 5318.0, 3294.0, 148892.0, 106231.0, 52000.0, 46409.0, 22543.0, 3028.0, 37257.0, 57665.0, 17878.0, 0.0, 29198.0, 28829.0, 22286.0, 14063.0, 43988.0, 6024.0, 15005.0, 12103.0, 41043.0, 6129.0, 4065.0, 15686.0, 6971.0, 5412.0, 1860.0, 43021.0, 88344.0, 45534.0, 119838.0, 23546.0, 15591.0, 68175.0, 5364.0, 11007.0, 2465.0, 27197.0, 50456.0, 161892.0, 6892.0, 93072.0, 0.0, 5782.0, 94412.0, 36287.0, 21060.0, 33947.0, 57966.0, 76113.0, 13241.0, 2362.0, 8491.0, 12674.0, 8537.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 61083.0, 77706.0, 63213.0, 2596.0, 18210.0, 25504.0, 0.0, 0.0, 0.0, 0.0, 15640.0, 32863.0, 1366.0, 15801.0, 0.0, 0.0, 8433.0, 10947.0, 0.0, 0.0, 31031.0, 31441.0, 43213.0, 9152.0, 4161.0, 17848.0, 25930.0, 13274.0, 9144.0, 2813.0, 4906.0, 5119.0, 1721.0, 1651.0, 1702.0, 53633.0, 66216.0, 37873.0, 3621.0, 12079.0, 36942.0, 3525.0, 1699.0, 6187.0, 0.0, 34222.0, 31402.0, 12947.0, 2445.0, 9732.0, 0.0, 27079.0, 3055.0, 0.0, 0.0, 22305.0, 26225.0, 18186.0, 1803.0, 7037.0, 2051.0, 16081.0, 3505.0, 18172.0, 8862.0, 8149.0, 2216.0, 3359.0, 1880.0, 1763.0, 13434.0, 20981.0, 5158.0, 63524.0, 6786.0, 13377.0, 30734.0, 3622.0, 13461.0, 0.0, 22327.0, 12224.0, 78064.0, 0.0, 27757.0, 0.0, 10389.0, 22732.0, 8558.0, 0.0, 1879.0, 18765.0, 4203.0, 0.0, 0.0, 0.0, 1885.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 12312.0, 9679.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 11010.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 7931.0, 5281.0, 4219.0, 10123.0, 4776.0, 12524.0, 2176.0, 1785.0, 2798.0, 3224.0, 8461.0, 1660.0, 4666.0, 5567.0, 9341.0, 8849.0, 8722.0, 8990.0, 1393.0, 4527.0, 1672.0, 4389.0, 2097.0, 8630.0, 0.0, 5696.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 12274.0, 0.0, 10579.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 276815.0, 247284.0, 240688.0, 221613.0, 19463.0, 255535.0, 255729.0, 222005.0, 21670.0, 128692.0, 148424.0, 16665.0, 0.0, 0.0, 0.0, 266251.0, 244971.0, 229214.0, 65784.0, 251474.0, 239351.0, 80075.0, 67719.0, 22756.0, 0.0, 260681.0, 248116.0, 117686.0, 195086.0, 76527.0, 5549.0, 213368.0, 56191.0, 44129.0, 15887.0, 264993.0, 256451.0, 240890.0, 207614.0, 61521.0, 256321.0, 246439.0, 207506.0, 40069.0, 109780.0, 142421.0, 10048.0, 3849.0, 2596.0, 2524.0, 258068.0, 244140.0, 233894.0, 135976.0, 256108.0, 235493.0, 120634.0, 128463.0, 43787.0, 0.0, 258389.0, 231082.0, 159464.0, 209454.0, 87080.0, 1923.0, 210108.0, 106698.0, 20711.0, 32189.0, 249313.0, 239223.0, 222945.0, 165795.0, 34256.0, 206198.0, 199340.0, 143047.0, 50842.0, 23688.0, 50302.0, 13474.0, 2344.0, 3348.0, 1648.0, 257467.0, 245732.0, 209853.0, 73742.0, 164326.0, 205600.0, 79932.0, 37901.0, 35868.0, 0.0, 226533.0, 221966.0, 126178.0, 172593.0, 61434.0, 3730.0, 156083.0, 73688.0, 13714.0, 35583.0, 91721.0, 134120.0, 85692.0, 58013.0, 57083.0, 25105.0, 46601.0, 26665.0, 77796.0, 23583.0, 3290.0, 52722.0, 3387.0, 3517.0, 4429.0, 117493.0, 156360.0, 119715.0, 193117.0, 5688.0, 62104.0, 179875.0, 1231.0, 88961.0, 16926.0, 98215.0, 109259.0, 194898.0, 31743.0, 180392.0, 36575.0, 36390.0, 181264.0, 62916.0, 64698.0, 266188.0, 259018.0, 217614.0, 157242.0, 12775.0, 155983.0, 167045.0, 134240.0, 2350.0, 9163.0, 24830.0, 0.0, 0.0, 0.0, 0.0, 249653.0, 232221.0, 214821.0, 29830.0, 132253.0, 127033.0, 14990.0, 5768.0, 4285.0, 0.0, 172521.0, 182099.0, 53122.0, 69217.0, 33592.0, 0.0, 100221.0, 16370.0, 0.0, 4106.0, 235969.0, 221637.0, 199132.0, 118075.0, 16083.0, 93061.0, 90699.0, 98536.0, 8174.0, 19419.0, 9539.0, 7387.0, 2967.0, 3057.0, 1687.0, 226030.0, 224347.0, 197778.0, 77488.0, 100868.0, 139028.0, 50019.0, 27324.0, 1579.0, 0.0, 182361.0, 217255.0, 98741.0, 83609.0, 18870.0, 0.0, 121953.0, 68532.0, 13897.0, 0.0, 89768.0, 79639.0, 77040.0, 30176.0, 83698.0, 22304.0, 56209.0, 47932.0, 90844.0, 2633.0, 13369.0, 27165.0, 3307.0, 1838.0, 4620.0, 96685.0, 152184.0, 114402.0, 187873.0, 47076.0, 66936.0, 160964.0, 2139.0, 55732.0, 0.0, 87991.0, 116607.0, 190270.0, 15427.0, 155207.0, 4155.0, 32081.0, 173417.0, 49772.0, 67243.0, 20365.0, 48762.0, 43907.0, 12186.0, 5035.0, 11566.0, 10132.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 48843.0, 77650.0, 45507.0, 15756.0, 2810.0, 26662.0, 0.0, 0.0, 0.0, 0.0, 15402.0, 51765.0, 4119.0, 6186.0, 0.0, 0.0, 9996.0, 7826.0, 0.0, 2292.0, 4302.0, 28482.0, 7241.0, 4967.0, 35152.0, 5850.0, 20930.0, 12652.0, 8672.0, 5091.0, 2621.0, 7833.0, 6795.0, 6012.0, 4135.0, 46345.0, 74252.0, 15565.0, 146093.0, 9978.0, 9149.0, 76989.0, 4883.0, 11531.0, 0.0, 17027.0, 21414.0, 115619.0, 0.0, 24510.0, 0.0, 6833.0, 50189.0, 13418.0, 1449.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 3172.0, 0.0, 7748.0, 0.0, 2328.0, 16425.0, 0.0, 0.0, 0.0, 0.0, 0.0, 9838.0, 0.0, 0.0, 0.0, 2007.0, 10432.0, 0.0, 0.0, 271484.0, 246342.0, 239503.0, 183136.0, 4139.0, 251985.0, 241034.0, 166569.0, 19733.0, 45085.0, 108948.0, 7080.0, 0.0, 13580.0, 0.0, 263692.0, 251588.0, 224600.0, 67833.0, 231966.0, 201922.0, 42817.0, 66084.0, 1197.0, 0.0, 259326.0, 231577.0, 97699.0, 134619.0, 16780.0, 11439.0, 146609.0, 51489.0, 0.0, 14899.0, 255974.0, 267525.0, 235875.0, 174450.0, 12314.0, 182798.0, 205830.0, 185601.0, 16835.0, 18854.0, 87569.0, 3598.0, 3295.0, 4302.0, 3055.0, 258227.0, 233497.0, 213250.0, 103501.0, 167543.0, 211654.0, 91028.0, 46313.0, 23343.0, 0.0, 227812.0, 222535.0, 114345.0, 137410.0, 70252.0, 12258.0, 139400.0, 94849.0, 15505.0, 15675.0, 116001.0, 115581.0, 97016.0, 62227.0, 141441.0, 65212.0, 107321.0, 35089.0, 135237.0, 3653.0, 7715.0, 58778.0, 3320.0, 4930.0, 1717.0, 123775.0, 166397.0, 121770.0, 193110.0, 75068.0, 86550.0, 196032.0, 13466.0, 97869.0, 0.0, 94168.0, 134216.0, 201943.0, 19803.0, 195443.0, 27678.0, 57704.0, 189969.0, 77467.0, 62854.0, 167625.0, 173340.0, 151123.0, 103527.0, 2029.0, 29849.0, 65534.0, 44422.0, 12507.0, 0.0, 10593.0, 7563.0, 0.0, 0.0, 0.0, 193609.0, 197831.0, 155245.0, 31950.0, 46580.0, 82676.0, 9262.0, 23690.0, 0.0, 0.0, 77528.0, 113283.0, 23517.0, 57971.0, 9360.0, 0.0, 103395.0, 38298.0, 281.0, 1411.0, 80243.0, 97752.0, 108329.0, 41449.0, 119452.0, 29595.0, 36478.0, 24825.0, 106182.0, 1640.0, 2340.0, 57872.0, 8985.0, 1885.0, 1985.0, 79276.0, 129728.0, 94093.0, 181098.0, 22783.0, 29414.0, 173374.0, 3167.0, 48340.0, 0.0, 93670.0, 98965.0, 186980.0, 12745.0, 130841.0, 12449.0, 22038.0, 139688.0, 43875.0, 49635.0, 2960.0, 9442.0, 12466.0, 3019.0, 35189.0, 0.0, 4524.0, 1754.0, 14569.0, 0.0, 0.0, 16614.0, 0.0, 0.0, 0.0, 14872.0, 26131.0, 12676.0, 104031.0, 0.0, 0.0, 4359.0, 0.0, 0.0, 0.0, 10637.0, 4953.0, 76197.0, 0.0, 29871.0, 0.0, 0.0, 14500.0, 0.0, 0.0, 244681.0, 216742.0, 154714.0, 96335.0, 11092.0, 88503.0, 96557.0, 95011.0, 8377.0, 9487.0, 22750.0, 1855.0, 0.0, 0.0, 0.0, 201819.0, 215739.0, 175947.0, 56708.0, 76620.0, 156392.0, 18497.0, 1161.0, 7228.0, 0.0, 138678.0, 179471.0, 44461.0, 42606.0, 4803.0, 0.0, 42259.0, 24216.0, 0.0, 0.0, 72743.0, 103506.0, 84536.0, 18938.0, 88315.0, 28403.0, 32108.0, 21161.0, 86679.0, 7020.0, 7792.0, 44699.0, 6576.0, 3339.0, 5400.0, 89493.0, 133647.0, 95868.0, 163084.0, 10551.0, 54361.0, 152798.0, 5854.0, 8737.0, 0.0, 61367.0, 56606.0, 186901.0, 23268.0, 119960.0, 21208.0, 41814.0, 108900.0, 38518.0, 50538.0, 0.0, 21318.0, 27607.0, 9937.0, 74448.0, 14630.0, 6862.0, 6952.0, 46520.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 32092.0, 30546.0, 39797.0, 87346.0, 6551.0, 10936.0, 59703.0, 0.0, 12354.0, 0.0, 25516.0, 15574.0, 66612.0, 6262.0, 53104.0, 10178.0, 0.0, 16198.0, 0.0, 0.0, 12322.0, 36763.0, 14544.0, 0.0, 12826.0, 0.0, 0.0, 0.0, 17213.0, 10097.0, 0.0, 5544.0, 0.0, 0.0, 0.0, 5782.0, 22014.0, 13065.0, 43310.0, 7231.0, 3293.0, 23933.0, 2220.0, 1880.0, 0.0, 2412.0, 13349.0, 51941.0, 0.0, 0.0, 0.0, 0.0, 0.0, 20044.0, 0.0  },
                                                {278116.0, 252512.0, 227294.0, 95551.0, 0.0, 254305.0, 230435.0, 47910.0, 0.0, 34323.0, 12093.0, 6451.0, 0.0, 0.0, 0.0, 268059.0, 240069.0, 168069.0, 14849.0, 209989.0, 116286.0, 6342.0, 11160.0, 3476.0, 0.0, 252842.0, 193171.0, 12053.0, 69435.0, 10703.0, 3814.0, 75721.0, 11650.0, 0.0, 0.0, 251660.0, 242441.0, 211231.0, 112365.0, 4955.0, 211279.0, 200201.0, 117640.0, 2971.0, 48739.0, 45103.0, 1654.0, 0.0, 0.0, 0.0, 263849.0, 218376.0, 174436.0, 26949.0, 189920.0, 149244.0, 22875.0, 59530.0, 5950.0, 0.0, 229058.0, 176440.0, 26538.0, 77127.0, 17199.0, 2758.0, 108237.0, 53188.0, 0.0, 1573.0, 194785.0, 192304.0, 119567.0, 69281.0, 4884.0, 63244.0, 107703.0, 57137.0, 13187.0, 11761.0, 24312.0, 0.0, 0.0, 0.0, 0.0, 176061.0, 196800.0, 90882.0, 11938.0, 72761.0, 98629.0, 12850.0, 14442.0, 0.0, 3520.0, 127302.0, 114397.0, 31930.0, 23034.0, 11022.0, 0.0, 51156.0, 24677.0, 809.0, 0.0, 60465.0, 73305.0, 30205.0, 8329.0, 3304.0, 19190.0, 14795.0, 3357.0, 4011.0, 6722.0, 3539.0, 1431.0, 10313.0, 0.0, 0.0, 66443.0, 75728.0, 37181.0, 7905.0, 17843.0, 15363.0, 7341.0, 2120.0, 0.0, 1676.0, 22167.0, 32085.0, 7199.0, 4242.0, 2532.0, 0.0, 8432.0, 904.0, 0.0, 7824.0, 16004.0, 2289.0, 2087.0, 1953.0, 3587.0, 9237.0, 3689.0, 613.0, 19171.0, 3372.0, 0.0, 0.0, 0.0, 0.0, 0.0, 14276.0, 5356.0, 10214.0, 26967.0, 1511.0, 6894.0, 32594.0, 0.0, 0.0, 0.0, 5443.0, 10118.0, 41583.0, 0.0, 17107.0, 0.0, 0.0, 14954.0, 9342.0, 12460.0, 232854.0, 205286.0, 129905.0, 8389.0, 0.0, 70028.0, 143412.0, 34607.0, 2722.0, 0.0, 0.0, 6730.0, 5520.0, 0.0, 0.0, 187328.0, 183072.0, 78652.0, 6650.0, 34033.0, 55350.0, 3193.0, 0.0, 1847.0, 0.0, 109961.0, 68575.0, 38980.0, 20047.0, 3295.0, 1808.0, 15798.0, 5596.0, 0.0, 6876.0, 121596.0, 148434.0, 79613.0, 64437.0, 4744.0, 26573.0, 60772.0, 23209.0, 10662.0, 7012.0, 3322.0, 3450.0, 0.0, 0.0, 0.0, 91002.0, 138568.0, 100301.0, 10173.0, 23417.0, 54703.0, 7070.0, 5013.0, 8451.0, 0.0, 67247.0, 91453.0, 35775.0, 0.0, 5253.0, 0.0, 17618.0, 18377.0, 0.0, 0.0, 40433.0, 49159.0, 28873.0, 7148.0, 5075.0, 5923.0, 19921.0, 4696.0, 6888.0, 5241.0, 11019.0, 0.0, 0.0, 0.0, 0.0, 59858.0, 40809.0, 55232.0, 9852.0, 13085.0, 28129.0, 4612.0, 0.0, 0.0, 0.0, 16376.0, 44668.0, 18887.0, 15377.0, 3435.0, 0.0, 6892.0, 9209.0, 3758.0, 0.0, 7437.0, 19366.0, 10671.0, 5616.0, 3560.0, 2287.0, 3328.0, 2297.0, 6217.0, 8777.0, 0.0, 0.0, 0.0, 0.0, 0.0, 12181.0, 6860.0, 0.0, 16982.0, 5074.0, 1652.0, 29294.0, 10378.0, 738.0, 0.0, 1959.0, 11889.0, 35682.0, 0.0, 34045.0, 0.0, 7085.0, 37421.0, 13573.0, 35390.0, 0.0, 13256.0, 5124.0, 1665.0, 0.0, 4973.0, 0.0, 3515.0, 0.0, 0.0, 1646.0, 0.0, 0.0, 0.0, 0.0, 15456.0, 11761.0, 8249.0, 5113.0, 0.0, 9454.0, 1641.0, 3418.0, 0.0, 0.0, 0.0, 15982.0, 12163.0, 1734.0, 1686.0, 0.0, 0.0, 0.0, 3343.0, 8349.0, 7296.0, 5264.0, 9572.0, 3016.0, 3237.0, 5230.0, 2897.0, 1672.0, 1736.0, 7072.0, 3512.0, 0.0, 0.0, 0.0, 0.0, 8718.0, 4433.0, 23990.0, 9956.0, 8227.0, 17896.0, 1641.0, 16083.0, 3268.0, 0.0, 12490.0, 14477.0, 22377.0, 5322.0, 8672.0, 6829.0, 3608.0, 7670.0, 0.0, 0.0, 6620.0, 4898.0, 1646.0, 4999.0, 2745.0, 11369.0, 1734.0, 5257.0, 8016.0, 3340.0, 0.0, 0.0, 0.0, 0.0, 0.0, 6768.0, 3586.0, 5663.0, 499.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 8566.0, 0.0, 1990.0, 0.0, 0.0, 2432.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 3653.0, 1631.0, 5108.0, 8072.0, 7883.0, 5330.0, 3540.0, 5034.0, 5829.0, 5666.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 8052.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 269954.0, 248616.0, 222449.0, 133622.0, 13659.0, 205388.0, 192304.0, 85407.0, 9722.0, 14571.0, 31782.0, 8134.0, 0.0, 0.0, 0.0, 256616.0, 238490.0, 171884.0, 19294.0, 209804.0, 130312.0, 13913.0, 3409.0, 3577.0, 0.0, 225172.0, 168523.0, 35222.0, 78121.0, 12739.0, 0.0, 62629.0, 6551.0, 0.0, 0.0, 240243.0, 233314.0, 209859.0, 137095.0, 13787.0, 143656.0, 143034.0, 120473.0, 2704.0, 45691.0, 29876.0, 2318.0, 0.0, 1958.0, 0.0, 235773.0, 232430.0, 162097.0, 29586.0, 155130.0, 137516.0, 37044.0, 29520.0, 17289.0, 0.0, 196803.0, 167129.0, 65580.0, 77118.0, 16636.0, 3458.0, 97117.0, 24509.0, 3325.0, 0.0, 124034.0, 139299.0, 111499.0, 71943.0, 2293.0, 41484.0, 61746.0, 48137.0, 15618.0, 7162.0, 13331.0, 0.0, 0.0, 0.0, 0.0, 145093.0, 168718.0, 119952.0, 30703.0, 44362.0, 87539.0, 18568.0, 23331.0, 4479.0, 0.0, 119943.0, 135980.0, 14516.0, 14432.0, 11628.0, 1737.0, 59853.0, 29130.0, 6934.0, 7185.0, 26039.0, 33887.0, 32001.0, 2230.0, 46806.0, 20644.0, 20270.0, 1568.0, 29769.0, 2787.0, 1715.0, 10397.0, 0.0, 1129.0, 0.0, 29672.0, 31877.0, 29615.0, 79063.0, 20379.0, 15349.0, 56880.0, 6272.0, 23684.0, 0.0, 39660.0, 40352.0, 102366.0, 3904.0, 66517.0, 0.0, 387.0, 40310.0, 17879.0, 18596.0, 120576.0, 118048.0, 80449.0, 36714.0, 2827.0, 23227.0, 53553.0, 14263.0, 3767.0, 0.0, 3418.0, 5399.0, 0.0, 0.0, 0.0, 119224.0, 138453.0, 93366.0, 8468.0, 25013.0, 76657.0, 24846.0, 2491.0, 5165.0, 0.0, 40506.0, 59411.0, 29894.0, 31458.0, 6960.0, 6146.0, 0.0, 5209.0, 0.0, 0.0, 73523.0, 96492.0, 78702.0, 38695.0, 3329.0, 20196.0, 40387.0, 42080.0, 13348.0, 17996.0, 2559.0, 0.0, 0.0, 0.0, 0.0, 91606.0, 82350.0, 94115.0, 8142.0, 16768.0, 55093.0, 17057.0, 5539.0, 9679.0, 0.0, 27133.0, 86296.0, 24947.0, 42310.0, 17176.0, 0.0, 29974.0, 10325.0, 0.0, 3797.0, 6073.0, 27643.0, 37501.0, 14379.0, 29096.0, 24422.0, 3417.0, 5193.0, 17987.0, 1651.0, 0.0, 0.0, 0.0, 0.0, 0.0, 20032.0, 15720.0, 22551.0, 71196.0, 5475.0, 8897.0, 20491.0, 0.0, 5622.0, 0.0, 17940.0, 10750.0, 84435.0, 6408.0, 27323.0, 0.0, 8141.0, 39168.0, 27376.0, 12956.0, 7426.0, 1490.0, 12932.0, 0.0, 0.0, 2079.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 4610.0, 15139.0, 19520.0, 3442.0, 0.0, 26951.0, 0.0, 0.0, 5261.0, 0.0, 2410.0, 21518.0, 1633.0, 0.0, 19571.0, 0.0, 0.0, 3777.0, 0.0, 6403.0, 4900.0, 3543.0, 7395.0, 2686.0, 2732.0, 1651.0, 1214.0, 1701.0, 4932.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 14730.0, 0.0, 10436.0, 5801.0, 5189.0, 7682.0, 0.0, 0.0, 5153.0, 0.0, 8330.0, 0.0, 18921.0, 0.0, 6742.0, 0.0, 5951.0, 11505.0, 0.0, 0.0, 0.0, 4031.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 218918.0, 228896.0, 171359.0, 98806.0, 4799.0, 61527.0, 122602.0, 57492.0, 5353.0, 14852.0, 16319.0, 950.0, 0.0, 0.0, 0.0, 210122.0, 198298.0, 152831.0, 14121.0, 99632.0, 82968.0, 25064.0, 25568.0, 16599.0, 0.0, 157124.0, 111966.0, 31852.0, 34073.0, 17540.0, 3552.0, 67888.0, 24535.0, 1068.0, 2715.0, 147948.0, 169277.0, 118711.0, 69448.0, 3675.0, 52332.0, 41820.0, 53800.0, 3560.0, 9117.0, 19183.0, 923.0, 0.0, 0.0, 0.0, 161506.0, 159979.0, 135206.0, 17523.0, 35082.0, 99460.0, 8976.0, 7401.0, 1653.0, 0.0, 101977.0, 138732.0, 37697.0, 76338.0, 20224.0, 0.0, 54369.0, 3149.0, 0.0, 0.0, 10912.0, 33531.0, 21842.0, 14199.0, 35736.0, 457.0, 13438.0, 6710.0, 45916.0, 0.0, 1623.0, 17934.0, 0.0, 0.0, 0.0, 30931.0, 43956.0, 45554.0, 81359.0, 13994.0, 9556.0, 80658.0, 1937.0, 19239.0, 0.0, 39954.0, 40716.0, 82563.0, 3973.0, 77370.0, 7477.0, 13163.0, 68054.0, 4237.0, 29693.0, 40078.0, 57459.0, 30608.0, 8892.0, 9153.0, 0.0, 5205.0, 18615.0, 0.0, 0.0, 0.0, 0.0, 3446.0, 0.0, 0.0, 43822.0, 48280.0, 38099.0, 7889.0, 11797.0, 16846.0, 3515.0, 4046.0, 0.0, 0.0, 12143.0, 22485.0, 11561.0, 0.0, 0.0, 0.0, 1913.0, 0.0, 0.0, 5634.0, 3596.0, 7591.0, 18507.0, 2383.0, 31611.0, 2527.0, 1687.0, 8688.0, 7744.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 13904.0, 34045.0, 24787.0, 40145.0, 5681.0, 2863.0, 13105.0, 0.0, 0.0, 0.0, 6788.0, 6686.0, 74923.0, 0.0, 30423.0, 0.0, 2148.0, 40455.0, 16788.0, 6895.0, 0.0, 5091.0, 1759.0, 0.0, 8912.0, 0.0, 1926.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 7139.0, 5241.0, 2573.0, 15652.0, 0.0, 4981.0, 0.0, 0.0, 0.0, 0.0, 0.0, 3538.0, 16503.0, 0.0, 2191.0, 0.0, 0.0, 0.0, 0.0, 0.0, 78132.0, 50012.0, 46492.0, 36269.0, 0.0, 27350.0, 16239.0, 11258.0, 1470.0, 2276.0, 10341.0, 0.0, 0.0, 0.0, 0.0, 66833.0, 54692.0, 52992.0, 3751.0, 17297.0, 59148.0, 7943.0, 3499.0, 8222.0, 0.0, 31829.0, 44071.0, 4173.0, 18702.0, 0.0, 0.0, 29548.0, 0.0, 0.0, 1281.0, 9760.0, 31545.0, 27277.0, 6636.0, 18397.0, 3515.0, 11419.0, 1185.0, 12384.0, 0.0, 2920.0, 8073.0, 0.0, 0.0, 0.0, 21697.0, 20936.0, 30805.0, 51325.0, 7385.0, 9982.0, 18905.0, 0.0, 23863.0, 0.0, 39200.0, 11291.0, 79807.0, 2217.0, 28276.0, 0.0, 2223.0, 34249.0, 7398.0, 22580.0, 0.0, 0.0, 15051.0, 9118.0, 11019.0, 0.0, 0.0, 0.0, 17994.0, 3432.0, 0.0, 724.0, 0.0, 0.0, 0.0, 4681.0, 0.0, 4460.0, 16254.0, 0.0, 0.0, 12305.0, 0.0, 5931.0, 0.0, 5051.0, 0.0, 24038.0, 0.0, 0.0, 0.0, 0.0, 3948.0, 0.0, 0.0, 0.0, 0.0, 0.0, 866.0, 13085.0, 2049.0, 2909.0, 1570.0, 19185.0, 0.0, 1677.0, 11545.0, 0.0, 0.0, 0.0, 0.0, 4926.0, 771.0, 24989.0, 6401.0, 796.0, 10748.0, 3673.0, 0.0, 0.0, 3083.0, 13099.0, 2555.0, 0.0, 4545.0, 0.0, 0.0, 589.0, 0.0, 0.0  },
                                                {203642.0, 182052.0, 186022.0, 157625.0, 9972.0, 226623.0, 223927.0, 159489.0, 15345.0, 160190.0, 90330.0, 10706.0, 2506.0, 0.0, 0.0, 204270.0, 182672.0, 193621.0, 28391.0, 215797.0, 176114.0, 49212.0, 83769.0, 14506.0, 0.0, 197609.0, 198436.0, 40409.0, 168936.0, 36800.0, 8050.0, 185357.0, 36734.0, 4071.0, 16232.0, 219887.0, 183178.0, 178054.0, 159271.0, 15454.0, 220263.0, 205982.0, 156061.0, 25260.0, 117490.0, 135420.0, 2331.0, 10973.0, 0.0, 0.0, 201628.0, 183225.0, 179673.0, 52378.0, 206679.0, 178206.0, 53704.0, 128979.0, 8240.0, 0.0, 194784.0, 184632.0, 85078.0, 172249.0, 64776.0, 13684.0, 166126.0, 54588.0, 9285.0, 29028.0, 215536.0, 205316.0, 181706.0, 100855.0, 17572.0, 170685.0, 174310.0, 125377.0, 16592.0, 61078.0, 72839.0, 13300.0, 13305.0, 4006.0, 0.0, 204871.0, 191243.0, 164132.0, 51866.0, 162976.0, 160225.0, 59122.0, 75116.0, 15633.0, 5897.0, 184364.0, 166308.0, 73792.0, 143102.0, 50068.0, 0.0, 137000.0, 39989.0, 0.0, 17187.0, 119103.0, 127368.0, 106564.0, 47883.0, 0.0, 77286.0, 75309.0, 32034.0, 7364.0, 4753.0, 7116.0, 3185.0, 5240.0, 0.0, 0.0, 127070.0, 141538.0, 80915.0, 19581.0, 77552.0, 62955.0, 15074.0, 6031.0, 4772.0, 0.0, 91124.0, 103854.0, 24730.0, 17390.0, 50752.0, 1581.0, 17591.0, 12396.0, 6191.0, 724.0, 19828.0, 26066.0, 13293.0, 6774.0, 12052.0, 5565.0, 10623.0, 6174.0, 27540.0, 2243.0, 2624.0, 5092.0, 0.0, 2106.0, 0.0, 31202.0, 31387.0, 18953.0, 50837.0, 8111.0, 22070.0, 71392.0, 0.0, 11612.0, 6874.0, 11017.0, 39645.0, 112811.0, 22788.0, 77402.0, 9522.0, 6079.0, 75097.0, 62131.0, 27795.0, 234393.0, 222463.0, 191275.0, 100099.0, 7306.0, 188079.0, 126094.0, 92818.0, 13219.0, 51034.0, 45030.0, 0.0, 0.0, 0.0, 0.0, 231643.0, 202288.0, 169445.0, 16543.0, 141862.0, 140774.0, 31667.0, 8268.0, 9643.0, 0.0, 194811.0, 151918.0, 33720.0, 111841.0, 23327.0, 6712.0, 97956.0, 9927.0, 12738.0, 15957.0, 222780.0, 206425.0, 175190.0, 73443.0, 10414.0, 143364.0, 120050.0, 106140.0, 3069.0, 13950.0, 21128.0, 6300.0, 0.0, 0.0, 0.0, 190967.0, 194709.0, 146935.0, 30172.0, 129662.0, 137401.0, 26606.0, 36542.0, 15432.0, 1400.0, 153733.0, 150231.0, 41791.0, 89149.0, 21984.0, 0.0, 82700.0, 33853.0, 11644.0, 2377.0, 101721.0, 121228.0, 88960.0, 42489.0, 4359.0, 57418.0, 49534.0, 33943.0, 13488.0, 5039.0, 8774.0, 1043.0, 0.0, 0.0, 0.0, 120821.0, 135459.0, 111040.0, 20112.0, 44592.0, 53804.0, 30209.0, 1286.0, 5015.0, 0.0, 113788.0, 82284.0, 41034.0, 20679.0, 16600.0, 1262.0, 50546.0, 19170.0, 3686.0, 11139.0, 16783.0, 52710.0, 23897.0, 11797.0, 16135.0, 12100.0, 19262.0, 7123.0, 27029.0, 0.0, 0.0, 12082.0, 0.0, 0.0, 0.0, 61490.0, 31172.0, 25412.0, 49989.0, 19369.0, 28616.0, 68259.0, 2204.0, 11502.0, 0.0, 32706.0, 19719.0, 79234.0, 12888.0, 81793.0, 10479.0, 8090.0, 65979.0, 23071.0, 32320.0, 49028.0, 90096.0, 51604.0, 11227.0, 1992.0, 6335.0, 3768.0, 833.0, 6473.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 25032.0, 66768.0, 42271.0, 1635.0, 14053.0, 19395.0, 1676.0, 0.0, 0.0, 0.0, 43164.0, 19926.0, 8294.0, 0.0, 2744.0, 0.0, 11049.0, 7218.0, 0.0, 0.0, 19810.0, 49856.0, 10018.0, 17936.0, 2002.0, 6438.0, 10476.0, 13861.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 47925.0, 71852.0, 34895.0, 13034.0, 4005.0, 495.0, 5632.0, 0.0, 0.0, 0.0, 19303.0, 29532.0, 14193.0, 5216.0, 4034.0, 0.0, 7398.0, 4180.0, 0.0, 0.0, 19753.0, 18848.0, 4657.0, 0.0, 11334.0, 9057.0, 1246.0, 6124.0, 20882.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 33794.0, 23797.0, 3084.0, 50170.0, 6380.0, 1301.0, 17147.0, 0.0, 0.0, 0.0, 6908.0, 5317.0, 73385.0, 4936.0, 37048.0, 0.0, 0.0, 57360.0, 12465.0, 10254.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 5050.0, 4360.0, 1098.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1355.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 19915.0, 0.0, 0.0, 2506.0, 6625.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 8144.0, 0.0, 25955.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 17878.0, 0.0, 6947.0, 0.0, 0.0, 27645.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 219989.0, 192266.0, 194660.0, 170340.0, 25660.0, 230341.0, 205284.0, 163122.0, 15022.0, 113311.0, 103142.0, 4470.0, 16463.0, 0.0, 0.0, 205212.0, 188257.0, 190000.0, 43062.0, 213712.0, 179971.0, 43414.0, 97673.0, 9190.0, 0.0, 203492.0, 192420.0, 68465.0, 170114.0, 50097.0, 827.0, 164065.0, 62419.0, 5876.0, 16259.0, 212003.0, 199005.0, 187893.0, 151166.0, 21009.0, 209433.0, 191458.0, 146992.0, 31705.0, 85109.0, 93677.0, 17428.0, 6179.0, 4931.0, 0.0, 206661.0, 196414.0, 169700.0, 83130.0, 206163.0, 173493.0, 75568.0, 98695.0, 18874.0, 7601.0, 206116.0, 172645.0, 111212.0, 149550.0, 60066.0, 6557.0, 156009.0, 90546.0, 15866.0, 10492.0, 205896.0, 179504.0, 156819.0, 85937.0, 3294.0, 148476.0, 145384.0, 114892.0, 23617.0, 23901.0, 37346.0, 8199.0, 15358.0, 0.0, 0.0, 194246.0, 171019.0, 149504.0, 67104.0, 131972.0, 149518.0, 38881.0, 27980.0, 32057.0, 0.0, 156276.0, 159146.0, 95576.0, 121449.0, 66910.0, 10696.0, 126482.0, 52257.0, 15525.0, 34866.0, 31860.0, 78208.0, 65573.0, 18595.0, 57415.0, 27269.0, 66193.0, 34867.0, 79501.0, 16141.0, 12191.0, 40340.0, 0.0, 0.0, 0.0, 102418.0, 89921.0, 75392.0, 131704.0, 47224.0, 62718.0, 123750.0, 11197.0, 97609.0, 3695.0, 77427.0, 85291.0, 140608.0, 19658.0, 131453.0, 24006.0, 29501.0, 130259.0, 83322.0, 37505.0, 213167.0, 205849.0, 175756.0, 104778.0, 11982.0, 123652.0, 133426.0, 105446.0, 3735.0, 21671.0, 9385.0, 0.0, 0.0, 0.0, 0.0, 213284.0, 192103.0, 154980.0, 27588.0, 100689.0, 125693.0, 17554.0, 16160.0, 4047.0, 0.0, 169497.0, 161155.0, 68442.0, 40145.0, 36696.0, 0.0, 101577.0, 23571.0, 11060.0, 2198.0, 173708.0, 165418.0, 145793.0, 84050.0, 18565.0, 75098.0, 91511.0, 85630.0, 5950.0, 11963.0, 22343.0, 1210.0, 3345.0, 0.0, 0.0, 168497.0, 168955.0, 135140.0, 41021.0, 103287.0, 113388.0, 32658.0, 32750.0, 13330.0, 5213.0, 136495.0, 144587.0, 70849.0, 78046.0, 45784.0, 1298.0, 77854.0, 44291.0, 9466.0, 5334.0, 47712.0, 79408.0, 68454.0, 46813.0, 89362.0, 35390.0, 39349.0, 18060.0, 96338.0, 11938.0, 6179.0, 17886.0, 2651.0, 0.0, 0.0, 86138.0, 100337.0, 70632.0, 126299.0, 40637.0, 41506.0, 135285.0, 0.0, 25866.0, 0.0, 44468.0, 84708.0, 131022.0, 17503.0, 128528.0, 14066.0, 32912.0, 127733.0, 75567.0, 60712.0, 33296.0, 55140.0, 19255.0, 12286.0, 9528.0, 1518.0, 8571.0, 3824.0, 0.0, 957.0, 0.0, 0.0, 0.0, 0.0, 0.0, 36410.0, 46200.0, 30455.0, 10176.0, 1319.0, 7507.0, 0.0, 3311.0, 0.0, 0.0, 21839.0, 27417.0, 24546.0, 0.0, 6461.0, 0.0, 3294.0, 2006.0, 0.0, 2317.0, 22880.0, 31824.0, 13321.0, 14029.0, 35888.0, 0.0, 0.0, 9398.0, 18158.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 5039.0, 24722.0, 19362.0, 89979.0, 9255.0, 3118.0, 83894.0, 6105.0, 0.0, 0.0, 19754.0, 16070.0, 87528.0, 4033.0, 45638.0, 11584.0, 7619.0, 37666.0, 20878.0, 5943.0, 0.0, 0.0, 3222.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 16781.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 18991.0, 0.0, 0.0, 0.0, 10732.0, 0.0, 0.0, 0.0, 230068.0, 204484.0, 187160.0, 149705.0, 13661.0, 174028.0, 171968.0, 139845.0, 17838.0, 43740.0, 62517.0, 1341.0, 1277.0, 1363.0, 0.0, 217364.0, 202583.0, 173580.0, 40658.0, 188810.0, 159391.0, 49491.0, 55055.0, 10336.0, 3086.0, 189525.0, 165861.0, 72369.0, 129087.0, 40079.0, 10748.0, 112159.0, 82992.0, 23847.0, 5290.0, 198149.0, 188126.0, 166165.0, 120554.0, 27200.0, 153432.0, 132170.0, 124806.0, 28988.0, 50741.0, 53111.0, 15424.0, 0.0, 0.0, 0.0, 200348.0, 182043.0, 157964.0, 81463.0, 143121.0, 151474.0, 53040.0, 52493.0, 15058.0, 0.0, 168028.0, 155346.0, 88637.0, 117741.0, 61258.0, 509.0, 127857.0, 60026.0, 24338.0, 23028.0, 87849.0, 86078.0, 78813.0, 41447.0, 68605.0, 30671.0, 43343.0, 24103.0, 110652.0, 15253.0, 14911.0, 28400.0, 1931.0, 0.0, 0.0, 73212.0, 131055.0, 77600.0, 126344.0, 43399.0, 54247.0, 132819.0, 11245.0, 78831.0, 0.0, 68259.0, 96357.0, 138674.0, 40625.0, 141902.0, 27290.0, 33274.0, 137460.0, 69689.0, 70966.0, 103193.0, 127012.0, 100928.0, 50416.0, 13110.0, 74039.0, 49169.0, 43414.0, 5813.0, 5354.0, 0.0, 0.0, 0.0, 0.0, 0.0, 155818.0, 127113.0, 106921.0, 22690.0, 55176.0, 71297.0, 12059.0, 7382.0, 10625.0, 0.0, 88999.0, 106807.0, 56016.0, 11653.0, 7769.0, 1519.0, 46482.0, 42991.0, 0.0, 1716.0, 50596.0, 83406.0, 75363.0, 43240.0, 98903.0, 23353.0, 34337.0, 7960.0, 72586.0, 0.0, 4341.0, 15377.0, 0.0, 0.0, 0.0, 75111.0, 97889.0, 71600.0, 124383.0, 14750.0, 58308.0, 136302.0, 5020.0, 29368.0, 0.0, 45314.0, 79553.0, 139204.0, 15617.0, 127444.0, 7630.0, 7920.0, 123658.0, 42663.0, 41334.0, 10783.0, 5403.0, 12366.0, 5939.0, 30017.0, 0.0, 0.0, 4949.0, 19656.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 4204.0, 16349.0, 8763.0, 49367.0, 1496.0, 0.0, 18156.0, 0.0, 0.0, 0.0, 8152.0, 16859.0, 51029.0, 0.0, 16341.0, 0.0, 2438.0, 22191.0, 8093.0, 3119.0, 173124.0, 174415.0, 123655.0, 76988.0, 11180.0, 67278.0, 91498.0, 74471.0, 13740.0, 26875.0, 25172.0, 2433.0, 0.0, 0.0, 0.0, 153742.0, 157523.0, 136076.0, 27071.0, 55449.0, 104691.0, 33723.0, 19002.0, 0.0, 0.0, 112320.0, 139854.0, 46275.0, 30827.0, 9033.0, 5061.0, 41773.0, 12341.0, 7222.0, 0.0, 41738.0, 69266.0, 46326.0, 3721.0, 78820.0, 14753.0, 38523.0, 29632.0, 93622.0, 0.0, 10164.0, 50105.0, 0.0, 3510.0, 0.0, 66678.0, 102309.0, 70971.0, 136136.0, 12658.0, 45402.0, 116654.0, 6552.0, 67378.0, 0.0, 35196.0, 73994.0, 140339.0, 21675.0, 118660.0, 814.0, 23582.0, 115862.0, 15575.0, 33328.0, 8324.0, 38416.0, 12604.0, 1494.0, 20427.0, 4135.0, 9092.0, 11735.0, 22143.0, 0.0, 3279.0, 8770.0, 0.0, 0.0, 0.0, 33175.0, 35416.0, 8069.0, 89470.0, 4920.0, 3767.0, 47578.0, 0.0, 14253.0, 0.0, 14932.0, 24568.0, 95529.0, 4879.0, 23469.0, 0.0, 0.0, 47448.0, 0.0, 2352.0, 17737.0, 19715.0, 15103.0, 1662.0, 19237.0, 8521.0, 14538.0, 1317.0, 3180.0, 0.0, 8939.0, 21470.0, 0.0, 0.0, 0.0, 21251.0, 27098.0, 30368.0, 22613.0, 0.0, 5596.0, 40031.0, 4455.0, 2484.0, 0.0, 4488.0, 25345.0, 57677.0, 2438.0, 48697.0, 0.0, 6035.0, 17732.0, 0.0, 0.0  } };
    
    char str[20];
    int x, y;
    unsigned int apptask, app, task, slot, taskType, cluster, base_addr, cluster_size, last_app;
    unsigned int addr, j, i, k;
    unsigned int state_stability[DIM_X*DIM_Y];
    unsigned int state_last[DIM_X*DIM_Y];

    int starting_fit[DIM_X*DIM_Y];
    int current_fit;

    static unsigned int sorted_addr[DIM_X*DIM_Y];
    static unsigned int sorted_score[DIM_X*DIM_Y];

    unsigned int scoreIterations = 0;
    unsigned int defined;
    unsigned int zeros[3];
    
    unsigned int sel, addrs[4];

    //---------------------------------------
    // --------- Q-learning stuff -----------
    unsigned int tp, state, maxid;
    // Hyperparameters
    float epsilon = 0.1;
    float alpha = 0.01;
    float gamma = 0.6;
    float oldvalue, maxval, reward, delta;


    //policy table initialization
    for(tp = 0; tp < N_TASKTYPE; tp++){
        zeros[tp] = 0;
        for(state = 0; state < N_STATES; state++){
            scoreTable[tp][state] = scoreTable[tp][state]/1000;
            if(scoreTable[tp][state] == 0.0){
                zeros[tp]++;
            }
            //scoreTable[tp][state] = 0.0;//random();
        }
        printsvsv("ScoreTable[", tp, "] - zeros: ", zeros[tp]);
        while(scoreTable[tp][API_calculateState(objectiveState[tp][0],objectiveState[tp][1],objectiveState[tp][2],objectiveState[tp][3],objectiveState[tp][4],objectiveState[tp][5])] != 0.0){
            objective_next(tp);
        }
        zeros[tp]--;
    }

	// Initialize the System Tiles Info
	API_TilesReset();

    Tiles[getXpos(GLOBAL_MASTER_ADDR)][getYpos(GLOBAL_MASTER_ADDR)].taskType = 1;
    Tiles[getXpos(GLOBAL_MASTER_ADDR)][getYpos(GLOBAL_MASTER_ADDR)].clusterCount = 1;

	// Initialize the applications vector
    API_ApplicationsReset();

	// Informs the Repository that the GLOBALMASTER is ready to receive the application info
	API_RepositoryWakeUp();

    API_PrintScoreTable(scoreTable);
	for(;;){
		API_setFreqScale(1000);
        API_applyFreqScale();
        tick = xTaskGetTickCount();
        if(tick >= SIMULATION_MAX_TICKS) _exit(0xfe10);
		myItoa(tick, str, 10);
		UART_polled_tx_string( &g_uart, (const uint8_t *)str);
		printsv("GlobalMasterActive", tick);
		UART_polled_tx_string( &g_uart, (const uint8_t *)" GlobalMasterRoutine...\r\n" );

        // prints the occupation
        API_PrintOccupation(tick);

        // Checks if there is some task to allocate
        if(API_CheckTaskToAllocate(xTaskGetTickCount())){
            
            last_app = -1;

            while(1){
                apptask = API_GetNextTaskToAllocate(xTaskGetTickCount());
                if (apptask == 0xFFFFFFFF) break;
                app = (apptask & 0xFFFF0000) >> 16;
                task = (apptask & 0x0000FFFF);
                printsvsv("Allocating task ", task, "from app ", app);
                if(last_app != app){
                    API_FindSmallerFITCluster(app);
                    base_addr = makeAddress(0,0); //applications[app].cluster_addr;
                    cluster_size = DIM_X; //applications[app].cluster_size;
                    printsvsv("Cluster addr: ", base_addr, " cluster size: ", cluster_size);
                }

                for(i = 0; i < 3; i++){
                    for( j = 0; j < 6; j++){
                        currentState[i][j] = 0;
                    }
                }

                x = 3; y = 3;
                if(getSouth(x,y) != -1)
                    currentState[0][getSouth(x,y)]++;
                // NORTH
                if(getNorth(x, y) != -1)
                    currentState[0][getNorth(x,y)]++;
                // WEST
                if(getWest(x, y) != -1)
                    currentState[0][getWest(x,y)]++;
                // EAST
                if(getEast(x, y) != -1)
                    currentState[0][getEast(x,y)]++;
                // SouthWest
                if(getSouthWest(x,y) != -1)
                    currentState[0][getSouthWest(x,y)+3]++;
                // SouthEast
                if(getSouthEast(x,y) != -1)
                    currentState[0][getSouthEast(x,y)+3]++;
                // NorthWest
                if(getNorthWest(x, y) != -1)
                    currentState[0][getNorthWest(x,y)+3]++;
                // NorthEast
                if(getNorthEast(x, y) != -1)
                    currentState[0][getNorthEast(x,y)+3]++;

                x = 8; y = 3;
                if(getSouth(x,y) != -1)
                    currentState[1][getSouth(x,y)]++;
                // NORTH
                if(getNorth(x, y) != -1)
                    currentState[1][getNorth(x,y)]++;
                // WEST
                if(getWest(x, y) != -1)
                    currentState[1][getWest(x,y)]++;
                // EAST
                if(getEast(x, y) != -1)
                    currentState[1][getEast(x,y)]++;
                // SouthWest
                if(getSouthWest(x,y) != -1)
                    currentState[1][getSouthWest(x,y)+3]++;
                // SouthEast
                if(getSouthEast(x,y) != -1)
                    currentState[1][getSouthEast(x,y)+3]++;
                // NorthWest
                if(getNorthWest(x, y) != -1)
                    currentState[1][getNorthWest(x,y)+3]++;
                // NorthEast
                if(getNorthEast(x, y) != -1)
                    currentState[1][getNorthEast(x,y)+3]++;

                x = 5; y = 8;
                if(getSouth(x,y) != -1)
                    currentState[2][getSouth(x,y)]++;
                // NORTH
                if(getNorth(x, y) != -1)
                    currentState[2][getNorth(x,y)]++;
                // WEST
                if(getWest(x, y) != -1)
                    currentState[2][getWest(x,y)]++;
                // EAST
                if(getEast(x, y) != -1)
                    currentState[2][getEast(x,y)]++;
                // SouthWest
                if(getSouthWest(x,y) != -1)
                    currentState[2][getSouthWest(x,y)+3]++;
                // SouthEast
                if(getSouthEast(x,y) != -1)
                    currentState[2][getSouthEast(x,y)+3]++;
                // NorthWest
                if(getNorthWest(x, y) != -1)
                    currentState[2][getNorthWest(x,y)+3]++;
                // NorthEast
                if(getNorthEast(x, y) != -1)
                    currentState[2][getNorthEast(x,y)+3]++;

                printsv("TASK0 Objective state: ", API_calculateState(objectiveState[0][0],objectiveState[0][1],objectiveState[0][2],objectiveState[0][3],objectiveState[0][4],objectiveState[0][5]));
                // printsvsv("a: ", objectiveState[0][0], "da: ", objectiveState[0][3]);
                // printsvsv("b: ", objectiveState[0][1], "db: ", objectiveState[0][4]);
                // printsvsv("c: ", objectiveState[0][2], "dc: ", objectiveState[0][5]);
                printsv("TASK1 Objective state: ", API_calculateState(objectiveState[1][0],objectiveState[1][1],objectiveState[1][2],objectiveState[1][3],objectiveState[1][4],objectiveState[1][5]));
                printsv("TASK2 Objective state: ", API_calculateState(objectiveState[2][0],objectiveState[2][1],objectiveState[2][2],objectiveState[2][3],objectiveState[2][4],objectiveState[2][5]));

                defined = 0;
                if( applications[app].taskType[task] == 0 ){
                    if( API_getPEState(addr2id(makeAddress(3,3)), -1) == API_calculateState(objectiveState[0][0],objectiveState[0][1],objectiveState[0][2],objectiveState[0][3],objectiveState[0][4],objectiveState[0][5]) ){ // pode ir no special slot...
                        addr = makeAddress(3,3);
                        slot = API_GetTaskSlotFromTile(addr, app, task);
                        if (slot != ERRO) defined++;
                    }
                }
                else if( applications[app].taskType[task] == 1 ){
                    if( API_getPEState(addr2id(makeAddress(8,3)), -1) == API_calculateState(objectiveState[1][0],objectiveState[1][1],objectiveState[1][2],objectiveState[1][3],objectiveState[1][4],objectiveState[1][5]) ){ // pode ir no special slot...
                        addr = makeAddress(8,3);
                        slot = API_GetTaskSlotFromTile(addr, app, task);
                        if (slot != ERRO) defined++;
                    }
                }
                else if( applications[app].taskType[task] == 2 ){
                    if( API_getPEState(addr2id(makeAddress(5,8)), -1) == API_calculateState(objectiveState[2][0],objectiveState[2][1],objectiveState[2][2],objectiveState[2][3],objectiveState[2][4],objectiveState[2][5]) ){ // pode ir no special slot...
                        addr = makeAddress(5,8);
                        slot = API_GetTaskSlotFromTile(addr, app, task);
                        if (slot != ERRO) defined++;
                    }
                } 

                if(defined == 0){ 
                    if (applications[app].taskType[task] == 0){
                        if( currentState[0][0] < objectiveState[0][0] ){
                            addrs[0] = makeAddress(3,2);
                            addrs[1] = makeAddress(4,3);
                            addrs[2] = makeAddress(2,3);
                            addrs[3] = makeAddress(3,4);
                            sel = random();
                            for( i = 0; i < 4; i++){
                                if(defined == 0){
                                    addr = addrs[((i+sel)%4)];
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                            }
                        }
                        else if( currentState[0][3] < objectiveState[0][3] ){
                            addrs[0] = makeAddress(2,2);
                            addrs[1] = makeAddress(4,2);
                            addrs[2] = makeAddress(4,4);
                            addrs[3] = makeAddress(2,4);
                            sel = random();
                            for( i = 0; i < 4; i++){
                                if(defined == 0){
                                    addr = addrs[((i+sel)%4)];
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                            }
                        }
                        else if( currentState[1][0] < objectiveState[1][0] ){
                            addrs[0] = makeAddress(8,2);
                            addrs[1] = makeAddress(9,3);
                            addrs[2] = makeAddress(8,4);
                            addrs[3] = makeAddress(7,3);
                            sel = random();
                            for( i = 0; i < 4; i++){
                                if(defined == 0){
                                    addr = addrs[((i+sel)%4)];
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                            }
                        }
                        else if( currentState[1][3] < objectiveState[1][3] ){
                            addrs[0] = makeAddress(7,2);
                            addrs[1] = makeAddress(9,2);
                            addrs[2] = makeAddress(9,4);
                            addrs[3] = makeAddress(7,4);
                            sel = random();
                            for( i = 0; i < 4; i++){
                                if(defined == 0){
                                    addr = addrs[((i+sel)%4)];
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                            }
                        }
                        else if( currentState[2][0] < objectiveState[2][0] ){
                            addrs[0] = makeAddress(5,7);
                            addrs[1] = makeAddress(6,8);
                            addrs[2] = makeAddress(5,9);
                            addrs[3] = makeAddress(4,8);
                            sel = random();
                            for( i = 0; i < 4; i++){
                                if(defined == 0){
                                    addr = addrs[((i+sel)%4)];
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                            }
                        }
                        else if( currentState[2][3] < objectiveState[2][3] ){
                            addrs[0] = makeAddress(4,7);
                            addrs[1] = makeAddress(6,7);
                            addrs[2] = makeAddress(6,9);
                            addrs[3] = makeAddress(4,9);
                            sel = random();
                            for( i = 0; i < 4; i++){
                                if(defined == 0){
                                    addr = addrs[((i+sel)%4)];
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                            }
                        }
                    }

                    else if (applications[app].taskType[task] == 1){
                        if( currentState[0][1] < objectiveState[0][1] ){
                            addrs[0] = makeAddress(3,2);
                            addrs[1] = makeAddress(4,3);
                            addrs[2] = makeAddress(2,3);
                            addrs[3] = makeAddress(3,4);
                            sel = random();
                            for( i = 0; i < 4; i++){
                                if(defined == 0){
                                    addr = addrs[((i+sel)%4)];
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                            }
                        }
                        else if( currentState[0][4] < objectiveState[0][4] ){
                            addrs[0] = makeAddress(2,2);
                            addrs[1] = makeAddress(4,2);
                            addrs[2] = makeAddress(4,4);
                            addrs[3] = makeAddress(2,4);
                            sel = random();
                            for( i = 0; i < 4; i++){
                                if(defined == 0){
                                    addr = addrs[((i+sel)%4)];
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                            }
                        }
                        else if( currentState[1][1] < objectiveState[1][1] ){
                            addrs[0] = makeAddress(8,2);
                            addrs[1] = makeAddress(9,3);
                            addrs[2] = makeAddress(8,4);
                            addrs[3] = makeAddress(7,3);
                            sel = random();
                            for( i = 0; i < 4; i++){
                                if(defined == 0){
                                    addr = addrs[((i+sel)%4)];
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                            }
                        }
                        else if( currentState[1][4] < objectiveState[1][4] ){
                            addrs[0] = makeAddress(7,2);
                            addrs[1] = makeAddress(9,2);
                            addrs[2] = makeAddress(9,4);
                            addrs[3] = makeAddress(7,4);
                            sel = random();
                            for( i = 0; i < 4; i++){
                                if(defined == 0){
                                    addr = addrs[((i+sel)%4)];
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                            }
                        }
                        else if( currentState[2][1] < objectiveState[2][1] ){
                            addrs[0] = makeAddress(5,7);
                            addrs[1] = makeAddress(6,8);
                            addrs[2] = makeAddress(5,9);
                            addrs[3] = makeAddress(4,8);
                            sel = random();
                            for( i = 0; i < 4; i++){
                                if(defined == 0){
                                    addr = addrs[((i+sel)%4)];
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                            }
                        }
                        else if( currentState[2][4] < objectiveState[2][4] ){
                            addrs[0] = makeAddress(4,7);
                            addrs[1] = makeAddress(6,7);
                            addrs[2] = makeAddress(6,9);
                            addrs[3] = makeAddress(4,9);
                            sel = random();
                            for( i = 0; i < 4; i++){
                                if(defined == 0){
                                    addr = addrs[((i+sel)%4)];
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                            }
                        }
                    }

                    else if (applications[app].taskType[task] == 2){
                        if( currentState[0][2] < objectiveState[0][2] ){
                            addrs[0] = makeAddress(3,2);
                            addrs[1] = makeAddress(4,3);
                            addrs[2] = makeAddress(2,3);
                            addrs[3] = makeAddress(3,4);
                            sel = random();
                            for( i = 0; i < 4; i++){
                                if(defined == 0){
                                    addr = addrs[((i+sel)%4)];
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                            }
                        }
                        else if( currentState[0][5] < objectiveState[0][5] ){
                            addrs[0] = makeAddress(2,2);
                            addrs[1] = makeAddress(4,2);
                            addrs[2] = makeAddress(4,4);
                            addrs[3] = makeAddress(2,4);
                            sel = random();
                            for( i = 0; i < 4; i++){
                                if(defined == 0){
                                    addr = addrs[((i+sel)%4)];
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                            }
                        }
                        else if( currentState[1][2] < objectiveState[1][2] ){
                            addrs[0] = makeAddress(8,2);
                            addrs[1] = makeAddress(9,3);
                            addrs[2] = makeAddress(8,4);
                            addrs[3] = makeAddress(7,3);
                            sel = random();
                            for( i = 0; i < 4; i++){
                                if(defined == 0){
                                    addr = addrs[((i+sel)%4)];
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                            }
                        }
                        else if( currentState[1][5] < objectiveState[1][5] ){
                            addrs[0] = makeAddress(7,2);
                            addrs[1] = makeAddress(9,2);
                            addrs[2] = makeAddress(9,4);
                            addrs[3] = makeAddress(7,4);
                            sel = random();
                            for( i = 0; i < 4; i++){
                                if(defined == 0){
                                    addr = addrs[((i+sel)%4)];
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                            }
                        }
                        else if( currentState[2][2] < objectiveState[2][2] ){
                            addrs[0] = makeAddress(5,7);
                            addrs[1] = makeAddress(6,8);
                            addrs[2] = makeAddress(5,9);
                            addrs[3] = makeAddress(4,8);
                            sel = random();
                            for( i = 0; i < 4; i++){
                                if(defined == 0){
                                    addr = addrs[((i+sel)%4)];
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                            }
                        }
                        else if( currentState[2][5] < objectiveState[2][5] ){
                            addrs[0] = makeAddress(4,7);
                            addrs[1] = makeAddress(6,7);
                            addrs[2] = makeAddress(6,9);
                            addrs[3] = makeAddress(4,9);
                            sel = random();
                            for( i = 0; i < 4; i++){
                                if(defined == 0){
                                    addr = addrs[((i+sel)%4)];
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                            }
                        }
                    }

                }
                if(defined != 0){
                    prints("Forced allocation!! ");
                    printsvsv("App: ", app, " task: ", task);
                    printsv("Tasktype: ", applications[app].taskType[task]);
                    printsvsv("addr(x): ", getXpos(addr), "addr(y): ", getYpos(addr));
                }
                else{

                    // fill the auxiliar vectors
                    k = 0;
                    for( i = getXpos(base_addr); i < (getXpos(base_addr)+cluster_size); i++){
                        for( j = getYpos(base_addr); j < (getYpos(base_addr)+cluster_size); j++){
                            if( (i == 2 || i == 3 || i == 4) && (j == 2 || j == 3 || j == 4) ) continue;
                            if( (i == 7 || i == 8 || i == 9) && (j == 2 || j == 3 || j == 4) ) continue;
                            if( (i == 4 || i == 5 || i == 6) && (j == 7 || j == 8 || j == 9) ) continue;
                            sorted_addr[k] = makeAddress(i, j);
                            sorted_score[k] = (int)(scoreTable[applications[app].taskType[task]][API_getPEState(addr2id(makeAddress(i,j)), -1)]*1000);
                            k++;
                        }
                    }

                    // sort addrs by score
                    quickSort(sorted_score, sorted_addr, 0, ((cluster_size*cluster_size)-(9*3)));
                    
                    if( (int)(epsilon*100) < random()%100 ){ // try something new
                        //////////////////////////////////////////////
                        // gets a random tile 
                        do{
                            addr = sorted_addr[random()%((cluster_size*cluster_size)-(9*3))];
                            slot = API_GetTaskSlotFromTile(addr, app, task);
                        }while (slot == ERRO);
                    } else{ // uses the learned information
                        // try to get the best tile slot
                        for(i = ((cluster_size*cluster_size)-(9*3)); i >= 0; i--){
                            addr = sorted_addr[i];
                            slot = API_GetTaskSlotFromTile(addr, app, task);
                            if (slot != ERRO) break;
                        }
                    }

                }

                // register the application allocation
                applications[app].tasks[task].addr = addr;
                applications[app].tasks[task].slot = slot;
                applications[app].lastStart = applications[app].nextRun;
                API_RepositoryAllocation(app, task, addr);
                debug_task_alloc(tick, app, task, addr);

                last_app = app;

                // Checks if there is some task to start...
                API_StartTasks();
            }

        } else if(tick > 200) { // updates the score table
            
            for(i = 0; i < DIM_X*DIM_Y; i++){
                if(API_CheckTaskToAllocate(xTaskGetTickCount())) break;
                addr = id2addr(i);
                
                // gets the tasktype that is running in the PE 
                taskType = Tiles[getXpos(addr)][getYpos(addr)].taskType;

                
                // if the PE is running some task
                if ( taskType != -1 ) {
                    
                    // gets the current PE state
                    state = API_getPEState(i, -1);

                    // checks if this PE did not changed its state
                    if( state != state_last[i] ){ 
                        
                        state_last[i] = state;
                        starting_fit[i] = API_getFIT(addr);
                        state_stability[i] = 0;
                    
                    } else if ( state_stability[i] < 50 ){ // waits until the PE is in a stable state
                        
                        state_stability[i]++;

                    } else if ( state == state_last[i] && state_stability[i] >= 50 ) {
                        // calculates the reward
                        current_fit = (int)API_getFIT(addr);
                        delta = (float)((float)(current_fit/100) - (float)(starting_fit[i]/100));
                        if(delta > 0)
                            printsv("delta FIT: +", (int)delta);
                        else
                            printsv("delta FIT: -", (int)(delta*-1));
                        //reward =  (Q_rsqrt(7000+(delta*delta)) * delta * -500) + 500;
                        reward =  (Q_rsqrt(10000+(delta*delta)) * delta * -100) + 100;
                        printsvsv("state ", state, "woned reward: ", (int)reward);

                        // gets the old value
                        oldvalue = scoreTable[taskType][state];

                        // gets the max value from the table
                        maxid = API_getMaxIdxfromRow(scoreTable, taskType, N_STATES, N_TASKTYPE);
                        maxval = scoreTable[taskType][maxid];

                        // updates the score table
                        scoreTable[taskType][state] = (1 - alpha) * oldvalue + alpha * ( reward + gamma * maxval);
                        //tableUpdate[taskType][state] = tableUpdate[taskType][state] + 1;
                        
                        if(addr == 0x303 || addr == 0x803 || addr == 0x508){
                            printsv("Forced update tasktype: ", taskType);
                            printsvsv("oldValue: ", (int)(oldvalue*1000), "newValue: ", (int)(scoreTable[taskType][state]*1000));
                            if(addr == 0x303){
                                tp = 0;
                                if(zeros[tp] > 0){
                                    while(scoreTable[tp][API_calculateState(objectiveState[tp][0],objectiveState[tp][1],objectiveState[tp][2],objectiveState[tp][3],objectiveState[tp][4],objectiveState[tp][5])] != 0.0){
                                        objective_next(tp);
                                    }
                                } else{
                                    objective_next(tp);
                                }
                            }
                            else if(addr == 0x803){
                                tp = 1;
                                if(zeros[tp] > 0){
                                    while(scoreTable[tp][API_calculateState(objectiveState[tp][0],objectiveState[tp][1],objectiveState[tp][2],objectiveState[tp][3],objectiveState[tp][4],objectiveState[tp][5])] != 0.0){
                                        objective_next(tp);
                                    }
                                } else{
                                    objective_next(tp);
                                }
                            }
                            else if(addr == 0x503){
                                tp = 2;
                                if(zeros[tp] > 0){
                                    while(scoreTable[tp][API_calculateState(objectiveState[tp][0],objectiveState[tp][1],objectiveState[tp][2],objectiveState[tp][3],objectiveState[tp][4],objectiveState[tp][5])] != 0.0){
                                        objective_next(tp);
                                    }
                                } else{
                                    objective_next(tp);
                                }
                            }
                        }

                        // saves the current FIT for the next update
                        state_stability[i] = 0;
                        starting_fit[i] = API_getFIT(addr);

                        // print score table
                        toprint++;
                        if(toprint > 100){
                            API_PrintScoreTable(scoreTable);
                            //API_PrintScoreTable(tableUpdate);
                            toprint=0;
                        }
                    }
                } else {
                    state_last[i] = -1;
                }
            }
        }
		
		// Checks if there is some task to start...
		API_StartTasks();

        // Enters in idle
        API_setFreqIdle();
        API_applyFreqScale();
        if(API_SystemFinish){
            vTaskEnterCritical();
            for(i = 0; i <= (DIM_X); i++){
                for(j = 0; j <= (DIM_Y); j++){
                    printsvsv("PE: ", addr2id(i<<8|j), "cluster count: ", Tiles[i][j].clusterCount);
                }
            }
            vTaskExitCritical();
            //API_PrintPolicyTable();
            API_PrintScoreTable(scoreTable);
            //vTaskDelay(100); // to cool down the system
            _exit(0xfe10);
        }
        else if(!API_CheckTaskToAllocate(xTaskGetTickCount())){
            vTaskDelay(1);
        }
	}
}


#elif THERMAL_MANAGEMENT == 6 // CHRONOS3

static void GlobalManagerTask( void *pvParameters ){
    ( void ) pvParameters;
	unsigned int tick, toprint;
    float scoreTable[N_TASKTYPE][N_STATES] = {  {259954.0, 277400.0, 259567.0, 198289.0, 14294.0, 273156.0, 253422.0, 215292.0, 2525.0, 204696.0, 84259.0, 3564.0, 14946.0, 0.0, 0.0, 285755.0, 270245.0, 255020.0, 60088.0, 264292.0, 246146.0, 29062.0, 110405.0, 5774.0, 0.0, 264116.0, 251196.0, 60434.0, 227804.0, 65440.0, 0.0, 208713.0, 108184.0, 9092.0, 5579.0, 270650.0, 270735.0, 246100.0, 202683.0, 35478.0, 257955.0, 242407.0, 219609.0, 3602.0, 82377.0, 154738.0, 9986.0, 42201.0, 2637.0, 0.0, 270509.0, 261472.0, 245070.0, 88620.0, 255217.0, 237771.0, 87523.0, 165724.0, 28197.0, 0.0, 263843.0, 255764.0, 112840.0, 201163.0, 94403.0, 7129.0, 198632.0, 73531.0, 7170.0, 12190.0, 261326.0, 256488.0, 231838.0, 166933.0, 16474.0, 213696.0, 200574.0, 160567.0, 34108.0, 57793.0, 76474.0, 3296.0, 7350.0, 0.0, 0.0, 249239.0, 243572.0, 218749.0, 68428.0, 219658.0, 225565.0, 41227.0, 81131.0, 2878.0, 0.0, 227548.0, 222779.0, 90714.0, 185651.0, 90122.0, 0.0, 164573.0, 92784.0, 19093.0, 9963.0, 175300.0, 165013.0, 139445.0, 61834.0, 9099.0, 87106.0, 65727.0, 31984.0, 1762.0, 10796.0, 10287.0, 4503.0, 0.0, 0.0, 0.0, 169515.0, 202473.0, 124973.0, 47754.0, 80526.0, 87822.0, 43566.0, 1716.0, 12723.0, 0.0, 132603.0, 96550.0, 71908.0, 36134.0, 58809.0, 5828.0, 52042.0, 68090.0, 17816.0, 7386.0, 17418.0, 41077.0, 10408.0, 7213.0, 4590.0, 4429.0, 8099.0, 1558.0, 28033.0, 0.0, 4007.0, 9342.0, 0.0, 0.0, 0.0, 39025.0, 25478.0, 24244.0, 82199.0, 15882.0, 10213.0, 103339.0, 0.0, 9129.0, 0.0, 30269.0, 33223.0, 144166.0, 1945.0, 98239.0, 19596.0, 0.0, 65239.0, 24956.0, 11057.0, 260687.0, 265428.0, 240187.0, 128547.0, 15167.0, 197704.0, 170341.0, 118819.0, 13394.0, 46521.0, 36974.0, 1265.0, 0.0, 0.0, 0.0, 270511.0, 261544.0, 207593.0, 27592.0, 134387.0, 177512.0, 10132.0, 18739.0, 2593.0, 0.0, 239727.0, 156760.0, 19901.0, 98657.0, 12852.0, 0.0, 86527.0, 14909.0, 5003.0, 0.0, 245047.0, 247422.0, 194456.0, 113005.0, 27922.0, 134212.0, 97561.0, 115502.0, 18811.0, 7752.0, 18377.0, 3614.0, 0.0, 0.0, 0.0, 242055.0, 238825.0, 207175.0, 45686.0, 139617.0, 180939.0, 20820.0, 42713.0, 0.0, 0.0, 190273.0, 215341.0, 74547.0, 140822.0, 15548.0, 0.0, 100959.0, 23286.0, 13651.0, 20399.0, 169166.0, 177366.0, 141375.0, 27695.0, 2559.0, 29250.0, 70420.0, 61916.0, 6584.0, 11987.0, 6546.0, 0.0, 0.0, 0.0, 0.0, 121180.0, 185181.0, 129863.0, 40634.0, 72891.0, 60514.0, 39588.0, 0.0, 5318.0, 0.0, 140974.0, 104959.0, 49439.0, 35949.0, 21027.0, 3028.0, 35379.0, 54588.0, 12071.0, 0.0, 22218.0, 28829.0, 16162.0, 14063.0, 43988.0, 4352.0, 8017.0, 2040.0, 41043.0, 0.0, 4065.0, 15686.0, 0.0, 0.0, 0.0, 25089.0, 82450.0, 45534.0, 119838.0, 23546.0, 14014.0, 67137.0, 0.0, 11007.0, 0.0, 23733.0, 43692.0, 161854.0, 4981.0, 93072.0, 0.0, 2485.0, 92351.0, 36287.0, 19548.0, 27766.0, 43223.0, 62261.0, 13241.0, 2362.0, 8491.0, 10625.0, 8537.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 58236.0, 54139.0, 63213.0, 2596.0, 16542.0, 22271.0, 0.0, 0.0, 0.0, 0.0, 15640.0, 25535.0, 1366.0, 15801.0, 0.0, 0.0, 8433.0, 10947.0, 0.0, 0.0, 28655.0, 23428.0, 34861.0, 3339.0, 0.0, 11016.0, 23811.0, 5693.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 41093.0, 53076.0, 26907.0, 3621.0, 12079.0, 30623.0, 0.0, 0.0, 0.0, 0.0, 29858.0, 29939.0, 12947.0, 2445.0, 6504.0, 0.0, 27079.0, 3055.0, 0.0, 0.0, 17603.0, 15791.0, 13132.0, 1803.0, 7037.0, 2051.0, 8093.0, 1778.0, 18172.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 10027.0, 20981.0, 5158.0, 62403.0, 6786.0, 3501.0, 30734.0, 0.0, 13461.0, 0.0, 17358.0, 10398.0, 78064.0, 0.0, 27757.0, 0.0, 10389.0, 22732.0, 8558.0, 0.0, 1879.0, 14542.0, 4203.0, 0.0, 0.0, 0.0, 1885.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 12312.0, 9679.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 11010.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 8849.0, 2326.0, 8990.0, 1393.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 5696.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 12274.0, 0.0, 10579.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 279600.0, 277365.0, 257179.0, 213884.0, 19463.0, 267169.0, 259582.0, 218462.0, 21670.0, 118063.0, 147808.0, 7797.0, 0.0, 0.0, 0.0, 276927.0, 269038.0, 243383.0, 58418.0, 253138.0, 243000.0, 71758.0, 67719.0, 9138.0, 0.0, 256527.0, 250949.0, 104671.0, 182329.0, 68336.0, 2148.0, 199673.0, 41753.0, 40865.0, 15887.0, 262053.0, 263765.0, 246210.0, 203785.0, 59117.0, 246928.0, 243487.0, 196563.0, 40069.0, 103007.0, 131337.0, 10048.0, 3849.0, 0.0, 0.0, 261305.0, 272668.0, 233537.0, 118386.0, 260077.0, 232266.0, 116938.0, 125538.0, 43787.0, 0.0, 257411.0, 234421.0, 151924.0, 200870.0, 84830.0, 1923.0, 203487.0, 99761.0, 11187.0, 32189.0, 247174.0, 246102.0, 217693.0, 155563.0, 25783.0, 197391.0, 188140.0, 136789.0, 47032.0, 21548.0, 44786.0, 9213.0, 2344.0, 0.0, 0.0, 252524.0, 240301.0, 212300.0, 68665.0, 149068.0, 204922.0, 79932.0, 37901.0, 33032.0, 0.0, 224698.0, 217926.0, 113437.0, 171893.0, 55314.0, 3730.0, 151939.0, 73688.0, 13714.0, 35583.0, 85928.0, 124620.0, 74647.0, 50427.0, 57083.0, 25105.0, 37626.0, 23427.0, 77796.0, 23583.0, 3290.0, 52722.0, 0.0, 0.0, 0.0, 111886.0, 146731.0, 117911.0, 193117.0, 5688.0, 56614.0, 179875.0, 1231.0, 88961.0, 16926.0, 93397.0, 102393.0, 194898.0, 29417.0, 180526.0, 36575.0, 31364.0, 181264.0, 62916.0, 64698.0, 263476.0, 255044.0, 214005.0, 148090.0, 12775.0, 150415.0, 162099.0, 128659.0, 2350.0, 9163.0, 19864.0, 0.0, 0.0, 0.0, 0.0, 250050.0, 239072.0, 213149.0, 28456.0, 111242.0, 118727.0, 14990.0, 5768.0, 0.0, 0.0, 164310.0, 176479.0, 50739.0, 64754.0, 18108.0, 0.0, 94010.0, 7773.0, 0.0, 4106.0, 233813.0, 215428.0, 193383.0, 106640.0, 16083.0, 62464.0, 73455.0, 92962.0, 6430.0, 3840.0, 6061.0, 7387.0, 0.0, 0.0, 0.0, 213887.0, 221259.0, 199135.0, 74586.0, 73427.0, 135483.0, 42830.0, 27324.0, 1579.0, 0.0, 170712.0, 210482.0, 96912.0, 73200.0, 12199.0, 0.0, 111518.0, 62145.0, 5526.0, 0.0, 75606.0, 62601.0, 70428.0, 30176.0, 83698.0, 20134.0, 53931.0, 44990.0, 89268.0, 0.0, 8746.0, 25735.0, 0.0, 0.0, 0.0, 88654.0, 129651.0, 107720.0, 188122.0, 43518.0, 49830.0, 157949.0, 0.0, 55732.0, 0.0, 74375.0, 106480.0, 190426.0, 8775.0, 154896.0, 4155.0, 32081.0, 173417.0, 49772.0, 67243.0, 17280.0, 34604.0, 43907.0, 12186.0, 5035.0, 11566.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 44128.0, 64219.0, 45507.0, 12660.0, 2810.0, 16141.0, 0.0, 0.0, 0.0, 0.0, 10730.0, 51765.0, 4119.0, 310.0, 0.0, 0.0, 9996.0, 7826.0, 0.0, 2292.0, 4302.0, 25864.0, 7241.0, 4967.0, 35152.0, 0.0, 1099.0, 4983.0, 8672.0, 0.0, 2621.0, 7833.0, 0.0, 0.0, 0.0, 36854.0, 71894.0, 12449.0, 146093.0, 8340.0, 5306.0, 74538.0, 0.0, 11531.0, 0.0, 17027.0, 18569.0, 115619.0, 0.0, 24510.0, 0.0, 6833.0, 50189.0, 13418.0, 1449.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 3172.0, 0.0, 7748.0, 0.0, 2328.0, 16425.0, 0.0, 0.0, 0.0, 0.0, 0.0, 9838.0, 0.0, 0.0, 0.0, 2007.0, 10432.0, 0.0, 0.0, 279769.0, 263442.0, 247937.0, 180024.0, 2362.0, 249268.0, 237015.0, 159788.0, 19733.0, 43652.0, 105057.0, 4259.0, 0.0, 13580.0, 0.0, 263897.0, 253763.0, 226043.0, 58378.0, 227561.0, 197371.0, 36241.0, 58968.0, 1197.0, 0.0, 258629.0, 229316.0, 90970.0, 125334.0, 15222.0, 6352.0, 133284.0, 42412.0, 0.0, 14899.0, 258159.0, 267409.0, 229106.0, 172498.0, 12314.0, 166580.0, 197007.0, 183700.0, 16835.0, 14882.0, 68384.0, 0.0, 0.0, 0.0, 0.0, 252363.0, 247533.0, 214082.0, 92790.0, 161480.0, 209828.0, 84934.0, 44011.0, 16840.0, 0.0, 218812.0, 223086.0, 106404.0, 130372.0, 62249.0, 12258.0, 132924.0, 89883.0, 12988.0, 15675.0, 107428.0, 103458.0, 89787.0, 59842.0, 141441.0, 56658.0, 98667.0, 35089.0, 135237.0, 3653.0, 7715.0, 56350.0, 0.0, 4930.0, 0.0, 112449.0, 162019.0, 115714.0, 191330.0, 75068.0, 77411.0, 196032.0, 13466.0, 97869.0, 0.0, 83289.0, 130901.0, 202609.0, 12701.0, 192710.0, 27678.0, 50198.0, 189969.0, 77467.0, 62854.0, 167358.0, 154889.0, 145873.0, 87386.0, 2029.0, 29849.0, 45508.0, 41976.0, 4514.0, 0.0, 10593.0, 7563.0, 0.0, 0.0, 0.0, 187369.0, 171963.0, 153627.0, 31950.0, 37878.0, 78984.0, 9262.0, 23690.0, 0.0, 0.0, 59077.0, 100629.0, 19780.0, 57971.0, 4987.0, 0.0, 103395.0, 35349.0, 281.0, 1411.0, 77793.0, 90373.0, 103877.0, 39816.0, 119452.0, 14541.0, 32421.0, 24825.0, 106182.0, 0.0, 2340.0, 57872.0, 0.0, 0.0, 0.0, 74994.0, 124956.0, 93329.0, 180392.0, 16838.0, 29414.0, 173374.0, 0.0, 46655.0, 0.0, 87682.0, 94636.0, 186980.0, 4337.0, 130841.0, 12449.0, 14675.0, 139688.0, 43875.0, 49635.0, 2960.0, 3519.0, 9682.0, 3019.0, 35189.0, 0.0, 4524.0, 0.0, 14569.0, 0.0, 0.0, 16614.0, 0.0, 0.0, 0.0, 14872.0, 24640.0, 12676.0, 104031.0, 0.0, 0.0, 4359.0, 0.0, 0.0, 0.0, 10637.0, 4953.0, 76197.0, 0.0, 29871.0, 0.0, 0.0, 14500.0, 0.0, 0.0, 237252.0, 211002.0, 139563.0, 89242.0, 9399.0, 84490.0, 92028.0, 91199.0, 8377.0, 2208.0, 21179.0, 0.0, 0.0, 0.0, 0.0, 186616.0, 208048.0, 167484.0, 55324.0, 72529.0, 154723.0, 18497.0, 1161.0, 7228.0, 0.0, 132715.0, 172865.0, 41828.0, 41246.0, 2513.0, 0.0, 42259.0, 22208.0, 0.0, 0.0, 66627.0, 94441.0, 80565.0, 18938.0, 83380.0, 13633.0, 32108.0, 21161.0, 86679.0, 0.0, 0.0, 44699.0, 0.0, 0.0, 0.0, 78306.0, 126593.0, 94258.0, 163084.0, 7106.0, 51715.0, 151669.0, 5854.0, 8737.0, 0.0, 59688.0, 52619.0, 186901.0, 23268.0, 116126.0, 21208.0, 41814.0, 108900.0, 38518.0, 50538.0, 0.0, 21318.0, 24866.0, 9937.0, 74448.0, 11156.0, 4296.0, 5319.0, 46520.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 29815.0, 18747.0, 37563.0, 86499.0, 2685.0, 10936.0, 59703.0, 0.0, 12354.0, 0.0, 25516.0, 15574.0, 66612.0, 6262.0, 53104.0, 10178.0, 0.0, 16198.0, 0.0, 0.0, 9449.0, 26989.0, 14544.0, 0.0, 12826.0, 0.0, 0.0, 0.0, 17213.0, 0.0, 0.0, 5544.0, 0.0, 0.0, 0.0, 5782.0, 22014.0, 11344.0, 43310.0, 7231.0, 3293.0, 23933.0, 2220.0, 1880.0, 0.0, 2412.0, 13349.0, 51941.0, 0.0, 0.0, 0.0, 0.0, 0.0, 20044.0, 0.0  },
                                                {258214.0, 269326.0, 251195.0, 95551.0, 0.0, 259822.0, 226294.0, 45461.0, 0.0, 34323.0, 12093.0, 6451.0, 0.0, 0.0, 0.0, 274477.0, 249585.0, 165056.0, 13318.0, 206960.0, 103771.0, 4737.0, 9354.0, 3476.0, 0.0, 250309.0, 187269.0, 12053.0, 67835.0, 10703.0, 0.0, 71505.0, 9930.0, 0.0, 0.0, 250327.0, 246136.0, 216262.0, 106756.0, 0.0, 199094.0, 197849.0, 109335.0, 2971.0, 48739.0, 37226.0, 1654.0, 0.0, 0.0, 0.0, 257542.0, 227941.0, 174677.0, 26949.0, 186044.0, 140106.0, 18392.0, 46452.0, 0.0, 0.0, 221768.0, 169563.0, 21535.0, 75745.0, 17199.0, 0.0, 108237.0, 52058.0, 0.0, 1573.0, 194720.0, 188241.0, 118119.0, 68346.0, 3274.0, 63244.0, 103681.0, 43332.0, 13187.0, 0.0, 24312.0, 0.0, 0.0, 0.0, 0.0, 173491.0, 192051.0, 89247.0, 11938.0, 55292.0, 86255.0, 5961.0, 5971.0, 0.0, 0.0, 127302.0, 106631.0, 31930.0, 17198.0, 7531.0, 0.0, 51156.0, 24677.0, 809.0, 0.0, 52420.0, 71407.0, 28633.0, 6031.0, 0.0, 14149.0, 14795.0, 3357.0, 673.0, 0.0, 0.0, 1431.0, 0.0, 0.0, 0.0, 58764.0, 75728.0, 37181.0, 7905.0, 8883.0, 15363.0, 7341.0, 0.0, 0.0, 0.0, 18885.0, 32085.0, 7199.0, 4242.0, 2532.0, 0.0, 8432.0, 904.0, 0.0, 7824.0, 14444.0, 2289.0, 2087.0, 1953.0, 0.0, 0.0, 3689.0, 613.0, 19171.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 14276.0, 5356.0, 10214.0, 26967.0, 1511.0, 6894.0, 32594.0, 0.0, 0.0, 0.0, 5443.0, 8240.0, 41583.0, 0.0, 17107.0, 0.0, 0.0, 14954.0, 9342.0, 12460.0, 229005.0, 199879.0, 125084.0, 8389.0, 0.0, 68408.0, 72053.0, 8226.0, 2722.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 187328.0, 183312.0, 74640.0, 1585.0, 24228.0, 27229.0, 0.0, 0.0, 0.0, 0.0, 108822.0, 64658.0, 28341.0, 13727.0, 0.0, 0.0, 13700.0, 0.0, 0.0, 0.0, 115776.0, 147170.0, 73349.0, 52893.0, 3096.0, 26573.0, 19216.0, 18586.0, 5799.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 84015.0, 116937.0, 72198.0, 8625.0, 18508.0, 41427.0, 3551.0, 0.0, 0.0, 0.0, 56109.0, 78740.0, 30660.0, 0.0, 0.0, 0.0, 17618.0, 10516.0, 0.0, 0.0, 40433.0, 43109.0, 28873.0, 1896.0, 0.0, 4054.0, 17721.0, 4696.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 58700.0, 36532.0, 42649.0, 7542.0, 4275.0, 8033.0, 771.0, 0.0, 0.0, 0.0, 11614.0, 35380.0, 8903.0, 2153.0, 0.0, 0.0, 3892.0, 2586.0, 3758.0, 0.0, 7437.0, 19366.0, 10671.0, 1315.0, 3560.0, 2287.0, 0.0, 0.0, 6217.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 12181.0, 6860.0, 0.0, 16982.0, 0.0, 0.0, 27962.0, 0.0, 738.0, 0.0, 1959.0, 8581.0, 34114.0, 0.0, 34045.0, 0.0, 5441.0, 37421.0, 13573.0, 35390.0, 0.0, 13256.0, 5124.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 11561.0, 6208.0, 6660.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1694.0, 3778.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 7296.0, 5264.0, 1371.0, 3016.0, 0.0, 5230.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 8718.0, 4433.0, 4499.0, 1707.0, 3956.0, 0.0, 0.0, 0.0, 0.0, 0.0, 10931.0, 4708.0, 0.0, 0.0, 0.0, 0.0, 0.0, 4171.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 688.0, 499.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 8566.0, 0.0, 0.0, 0.0, 0.0, 2432.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 7883.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 8052.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 266731.0, 252991.0, 223895.0, 116246.0, 12052.0, 197471.0, 184053.0, 82310.0, 8106.0, 11129.0, 25651.0, 1772.0, 0.0, 0.0, 0.0, 261737.0, 238254.0, 167495.0, 15982.0, 205408.0, 97934.0, 9318.0, 0.0, 0.0, 0.0, 223431.0, 165301.0, 35222.0, 62502.0, 11209.0, 0.0, 58216.0, 6551.0, 0.0, 0.0, 246370.0, 236698.0, 208359.0, 134834.0, 6099.0, 131869.0, 125686.0, 101799.0, 1044.0, 27920.0, 27109.0, 2318.0, 0.0, 0.0, 0.0, 241904.0, 227062.0, 157524.0, 29586.0, 139709.0, 130010.0, 30864.0, 14049.0, 1279.0, 0.0, 191634.0, 164578.0, 62643.0, 72304.0, 6381.0, 0.0, 92794.0, 22995.0, 0.0, 0.0, 115820.0, 133331.0, 105162.0, 67301.0, 2293.0, 38434.0, 47789.0, 42709.0, 13942.0, 2077.0, 2797.0, 0.0, 0.0, 0.0, 0.0, 139102.0, 166264.0, 114888.0, 26583.0, 25484.0, 70750.0, 6134.0, 3767.0, 1202.0, 0.0, 117412.0, 129404.0, 14516.0, 14432.0, 8296.0, 0.0, 56086.0, 29130.0, 6934.0, 7185.0, 21719.0, 19821.0, 29081.0, 2230.0, 42273.0, 20644.0, 20270.0, 1568.0, 29769.0, 0.0, 0.0, 10397.0, 0.0, 1129.0, 0.0, 29672.0, 31877.0, 26786.0, 79063.0, 18469.0, 10038.0, 56880.0, 0.0, 23684.0, 0.0, 34644.0, 40352.0, 102366.0, 3904.0, 66517.0, 0.0, 387.0, 40310.0, 17879.0, 18596.0, 116924.0, 108908.0, 67621.0, 33598.0, 2827.0, 21529.0, 23907.0, 0.0, 471.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 110215.0, 126625.0, 72329.0, 8468.0, 16090.0, 44221.0, 9363.0, 0.0, 0.0, 0.0, 39232.0, 57858.0, 7180.0, 24624.0, 0.0, 0.0, 0.0, 1918.0, 0.0, 0.0, 70717.0, 90812.0, 72230.0, 38695.0, 3329.0, 17644.0, 9766.0, 32536.0, 13348.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 86517.0, 65073.0, 89053.0, 8142.0, 6588.0, 36405.0, 4726.0, 0.0, 0.0, 0.0, 27133.0, 74355.0, 7866.0, 32541.0, 7310.0, 0.0, 29974.0, 4435.0, 0.0, 3797.0, 6073.0, 24760.0, 29991.0, 12775.0, 29096.0, 11713.0, 0.0, 1108.0, 17987.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 20032.0, 15720.0, 18109.0, 71196.0, 0.0, 1970.0, 20491.0, 0.0, 3919.0, 0.0, 17940.0, 5726.0, 83639.0, 3035.0, 27323.0, 0.0, 6240.0, 36572.0, 27376.0, 12956.0, 7426.0, 1490.0, 11244.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 4610.0, 1698.0, 4263.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 8766.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 2774.0, 0.0, 5681.0, 0.0, 1123.0, 0.0, 1214.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 9145.0, 0.0, 3845.0, 2586.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 3501.0, 0.0, 15872.0, 0.0, 3038.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 218594.0, 222930.0, 160015.0, 90758.0, 4799.0, 55360.0, 110110.0, 57492.0, 2744.0, 0.0, 625.0, 950.0, 0.0, 0.0, 0.0, 203094.0, 195956.0, 145461.0, 11748.0, 95706.0, 74914.0, 21393.0, 8637.0, 1816.0, 0.0, 150756.0, 106262.0, 31852.0, 31252.0, 9754.0, 0.0, 67888.0, 24535.0, 1068.0, 2715.0, 142058.0, 156899.0, 114510.0, 53493.0, 2025.0, 41240.0, 28808.0, 50246.0, 3560.0, 0.0, 3360.0, 923.0, 0.0, 0.0, 0.0, 145189.0, 150461.0, 133459.0, 10628.0, 31996.0, 93713.0, 8976.0, 4166.0, 0.0, 0.0, 97887.0, 136067.0, 37697.0, 68010.0, 9789.0, 0.0, 54369.0, 3149.0, 0.0, 0.0, 7852.0, 27393.0, 21842.0, 14199.0, 35736.0, 457.0, 8283.0, 6710.0, 45916.0, 0.0, 0.0, 17934.0, 0.0, 0.0, 0.0, 27699.0, 43956.0, 43107.0, 81359.0, 13994.0, 9556.0, 80658.0, 0.0, 19239.0, 0.0, 39954.0, 36417.0, 82563.0, 2291.0, 75586.0, 7477.0, 13163.0, 68054.0, 4237.0, 29693.0, 35005.0, 42601.0, 28743.0, 8892.0, 9153.0, 0.0, 0.0, 12618.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 26365.0, 43349.0, 23186.0, 4670.0, 11797.0, 1344.0, 1881.0, 0.0, 0.0, 0.0, 8207.0, 19124.0, 6681.0, 0.0, 0.0, 0.0, 1913.0, 0.0, 0.0, 0.0, 0.0, 5954.0, 8687.0, 2383.0, 31611.0, 2527.0, 0.0, 3576.0, 7744.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 13904.0, 18188.0, 18584.0, 40145.0, 0.0, 2863.0, 13105.0, 0.0, 0.0, 0.0, 5036.0, 6686.0, 74923.0, 0.0, 30423.0, 0.0, 2148.0, 40455.0, 16788.0, 6895.0, 0.0, 0.0, 0.0, 0.0, 8912.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 7139.0, 0.0, 2573.0, 15652.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 16503.0, 0.0, 2191.0, 0.0, 0.0, 0.0, 0.0, 0.0, 71891.0, 50012.0, 44245.0, 36269.0, 0.0, 15357.0, 5337.0, 11258.0, 1470.0, 0.0, 10341.0, 0.0, 0.0, 0.0, 0.0, 55676.0, 39974.0, 52992.0, 3751.0, 17297.0, 53885.0, 7943.0, 1241.0, 3152.0, 0.0, 31829.0, 37257.0, 4173.0, 18702.0, 0.0, 0.0, 29548.0, 0.0, 0.0, 1281.0, 6652.0, 28800.0, 24187.0, 6636.0, 18397.0, 3515.0, 7793.0, 1185.0, 12384.0, 0.0, 1135.0, 8073.0, 0.0, 0.0, 0.0, 15230.0, 20936.0, 30805.0, 51325.0, 4038.0, 8196.0, 18905.0, 0.0, 17624.0, 0.0, 39200.0, 11291.0, 79807.0, 2217.0, 28276.0, 0.0, 2223.0, 34249.0, 7398.0, 22580.0, 0.0, 0.0, 13311.0, 9118.0, 11019.0, 0.0, 0.0, 0.0, 17994.0, 0.0, 0.0, 724.0, 0.0, 0.0, 0.0, 4681.0, 0.0, 4460.0, 16254.0, 0.0, 0.0, 12305.0, 0.0, 5931.0, 0.0, 5051.0, 0.0, 24038.0, 0.0, 0.0, 0.0, 0.0, 3948.0, 0.0, 0.0, 0.0, 0.0, 0.0, 866.0, 13085.0, 2049.0, 2909.0, 1570.0, 19185.0, 0.0, 0.0, 11545.0, 0.0, 0.0, 0.0, 0.0, 4926.0, 771.0, 24989.0, 3110.0, 796.0, 7252.0, 0.0, 0.0, 0.0, 3083.0, 3516.0, 2555.0, 0.0, 4545.0, 0.0, 0.0, 589.0, 0.0, 0.0  },
                                                {259121.0, 271655.0, 257866.0, 153900.0, 3598.0, 268160.0, 246220.0, 151112.0, 10975.0, 138492.0, 86728.0, 7616.0, 2506.0, 0.0, 0.0, 280833.0, 262803.0, 208201.0, 21705.0, 236747.0, 178850.0, 44196.0, 66471.0, 11403.0, 0.0, 261215.0, 225036.0, 37454.0, 157009.0, 26867.0, 8051.0, 177671.0, 36734.0, 2738.0, 14401.0, 265937.0, 266567.0, 209072.0, 152234.0, 8330.0, 246622.0, 225529.0, 152384.0, 25260.0, 100137.0, 132205.0, 558.0, 10973.0, 0.0, 0.0, 269203.0, 269663.0, 184258.0, 43133.0, 224848.0, 183141.0, 51960.0, 125540.0, 8240.0, 0.0, 235262.0, 189295.0, 72576.0, 167618.0, 50941.0, 1340.0, 162053.0, 49638.0, 9285.0, 26822.0, 235559.0, 239034.0, 173767.0, 84628.0, 4310.0, 153962.0, 164888.0, 116760.0, 13943.0, 61078.0, 56794.0, 10751.0, 10784.0, 0.0, 0.0, 213867.0, 217281.0, 166245.0, 46539.0, 153401.0, 144972.0, 49970.0, 59518.0, 14158.0, 0.0, 178893.0, 161884.0, 56166.0, 140951.0, 46442.0, 0.0, 131749.0, 35168.0, 0.0, 12758.0, 109665.0, 113772.0, 84178.0, 43359.0, 0.0, 68118.0, 62972.0, 16704.0, 2266.0, 3391.0, 5803.0, 1909.0, 5240.0, 0.0, 0.0, 113731.0, 129260.0, 74796.0, 13411.0, 56323.0, 51666.0, 15074.0, 2214.0, 3242.0, 0.0, 84363.0, 95941.0, 23419.0, 17390.0, 50752.0, 1581.0, 13810.0, 10376.0, 6191.0, 724.0, 15491.0, 24901.0, 10884.0, 6774.0, 12052.0, 5565.0, 8042.0, 6174.0, 27540.0, 2243.0, 0.0, 3748.0, 0.0, 2106.0, 0.0, 29103.0, 26882.0, 16172.0, 50837.0, 8111.0, 10650.0, 70222.0, 0.0, 10370.0, 6874.0, 11017.0, 34336.0, 112811.0, 22788.0, 77402.0, 9522.0, 6079.0, 75097.0, 62131.0, 27795.0, 250188.0, 238343.0, 194744.0, 84505.0, 7306.0, 188618.0, 120663.0, 88651.0, 13219.0, 51034.0, 45030.0, 0.0, 0.0, 0.0, 0.0, 239557.0, 205324.0, 162306.0, 14158.0, 132924.0, 136871.0, 11482.0, 4220.0, 3675.0, 0.0, 193275.0, 148172.0, 32037.0, 110776.0, 11329.0, 6712.0, 92395.0, 5662.0, 12738.0, 14658.0, 226513.0, 211932.0, 175025.0, 50065.0, 7388.0, 133528.0, 108164.0, 102190.0, 3069.0, 5652.0, 21128.0, 0.0, 0.0, 0.0, 0.0, 192723.0, 192614.0, 145086.0, 25135.0, 113164.0, 132595.0, 18689.0, 24083.0, 8250.0, 0.0, 147462.0, 142276.0, 32948.0, 85026.0, 17387.0, 0.0, 76502.0, 33853.0, 11644.0, 2377.0, 98727.0, 112065.0, 75422.0, 38867.0, 3117.0, 47879.0, 20816.0, 27391.0, 9882.0, 0.0, 0.0, 1043.0, 0.0, 0.0, 0.0, 105841.0, 128642.0, 95878.0, 20112.0, 41292.0, 47310.0, 26164.0, 0.0, 1055.0, 0.0, 97369.0, 71067.0, 39084.0, 11348.0, 12435.0, 1262.0, 41271.0, 16812.0, 2333.0, 11139.0, 8844.0, 40279.0, 15351.0, 11797.0, 16135.0, 10788.0, 16297.0, 7123.0, 27029.0, 0.0, 0.0, 12082.0, 0.0, 0.0, 0.0, 51112.0, 20740.0, 20255.0, 48377.0, 13575.0, 21857.0, 68259.0, 891.0, 9448.0, 0.0, 32706.0, 16964.0, 76041.0, 8139.0, 81793.0, 10479.0, 6461.0, 65979.0, 23071.0, 32320.0, 29731.0, 77140.0, 36856.0, 9992.0, 1992.0, 6335.0, 426.0, 833.0, 6473.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 25032.0, 59131.0, 16471.0, 0.0, 9573.0, 15960.0, 1676.0, 0.0, 0.0, 0.0, 38247.0, 12131.0, 4498.0, 0.0, 726.0, 0.0, 11049.0, 2137.0, 0.0, 0.0, 12734.0, 45907.0, 4034.0, 17936.0, 2002.0, 6438.0, 10476.0, 10815.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 43390.0, 69357.0, 33623.0, 13034.0, 0.0, 495.0, 0.0, 0.0, 0.0, 0.0, 16768.0, 26935.0, 14193.0, 2380.0, 0.0, 0.0, 5576.0, 4180.0, 0.0, 0.0, 19753.0, 18848.0, 729.0, 0.0, 11334.0, 9057.0, 1246.0, 4891.0, 20882.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 33794.0, 23797.0, 3084.0, 50170.0, 6380.0, 1301.0, 16021.0, 0.0, 0.0, 0.0, 6908.0, 5317.0, 73385.0, 0.0, 37048.0, 0.0, 0.0, 57360.0, 11023.0, 10254.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 5050.0, 4360.0, 1098.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1355.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 2506.0, 6625.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 3465.0, 0.0, 25955.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 17878.0, 0.0, 6947.0, 0.0, 0.0, 27645.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 269284.0, 264489.0, 255224.0, 164736.0, 24241.0, 241106.0, 212125.0, 163016.0, 9481.0, 94960.0, 83805.0, 0.0, 11840.0, 0.0, 0.0, 270202.0, 265668.0, 197814.0, 41204.0, 217859.0, 177423.0, 43414.0, 94115.0, 7526.0, 0.0, 239390.0, 200813.0, 56491.0, 168095.0, 46743.0, 827.0, 163645.0, 54945.0, 5876.0, 16259.0, 257461.0, 261715.0, 216703.0, 143421.0, 21009.0, 218776.0, 191688.0, 141531.0, 31705.0, 66223.0, 85747.0, 4983.0, 1614.0, 850.0, 0.0, 272830.0, 235523.0, 172646.0, 75818.0, 199699.0, 168246.0, 69183.0, 90536.0, 8816.0, 1778.0, 203179.0, 166249.0, 99027.0, 147256.0, 53463.0, 5303.0, 149486.0, 84230.0, 15866.0, 10492.0, 215617.0, 181446.0, 134874.0, 75231.0, 3294.0, 134740.0, 129393.0, 106128.0, 23617.0, 11229.0, 22064.0, 4584.0, 9677.0, 0.0, 0.0, 202504.0, 177251.0, 139464.0, 62686.0, 126405.0, 138488.0, 36889.0, 18306.0, 21312.0, 0.0, 147391.0, 155179.0, 82397.0, 115623.0, 62309.0, 9356.0, 124455.0, 49660.0, 5984.0, 31835.0, 26200.0, 57404.0, 50917.0, 16115.0, 56638.0, 19621.0, 46172.0, 28643.0, 79501.0, 4858.0, 10867.0, 38398.0, 0.0, 0.0, 0.0, 100200.0, 79409.0, 66600.0, 131704.0, 42536.0, 55219.0, 121728.0, 8650.0, 94461.0, 3695.0, 75061.0, 77650.0, 140184.0, 13794.0, 131326.0, 24006.0, 27368.0, 130151.0, 83322.0, 37505.0, 204762.0, 211527.0, 173320.0, 99036.0, 5475.0, 120819.0, 118132.0, 102927.0, 3735.0, 21671.0, 6222.0, 0.0, 0.0, 0.0, 0.0, 210183.0, 186612.0, 148091.0, 14128.0, 88850.0, 117830.0, 13114.0, 12684.0, 2599.0, 0.0, 159021.0, 151645.0, 63878.0, 37749.0, 26613.0, 0.0, 96664.0, 21104.0, 11060.0, 2198.0, 165390.0, 157631.0, 131803.0, 71559.0, 13828.0, 50581.0, 69849.0, 71706.0, 5950.0, 10678.0, 4771.0, 1210.0, 0.0, 0.0, 0.0, 165511.0, 162558.0, 123801.0, 37221.0, 90046.0, 105750.0, 28597.0, 17866.0, 6494.0, 0.0, 124154.0, 132146.0, 63163.0, 72244.0, 40067.0, 1298.0, 68344.0, 43381.0, 6683.0, 5334.0, 47712.0, 66717.0, 54058.0, 45954.0, 88444.0, 24957.0, 37965.0, 11432.0, 96338.0, 0.0, 4791.0, 17886.0, 0.0, 0.0, 0.0, 73071.0, 87105.0, 66745.0, 126140.0, 33211.0, 30720.0, 135285.0, 0.0, 25866.0, 0.0, 37539.0, 82344.0, 130477.0, 16339.0, 128528.0, 14066.0, 20549.0, 127579.0, 75038.0, 60712.0, 22551.0, 48152.0, 19255.0, 9649.0, 9528.0, 1518.0, 8571.0, 0.0, 0.0, 957.0, 0.0, 0.0, 0.0, 0.0, 0.0, 30112.0, 35050.0, 24411.0, 3240.0, 1319.0, 7507.0, 0.0, 3311.0, 0.0, 0.0, 8667.0, 19600.0, 17382.0, 0.0, 0.0, 0.0, 3294.0, 2006.0, 0.0, 0.0, 22880.0, 17345.0, 13321.0, 8083.0, 35888.0, 0.0, 0.0, 6980.0, 18158.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 5039.0, 24722.0, 19362.0, 89979.0, 2789.0, 3118.0, 83894.0, 0.0, 0.0, 0.0, 19754.0, 14717.0, 87528.0, 4033.0, 45638.0, 11584.0, 7619.0, 32658.0, 20878.0, 5943.0, 0.0, 0.0, 3222.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 16781.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 18991.0, 0.0, 0.0, 0.0, 10732.0, 0.0, 0.0, 0.0, 262340.0, 244454.0, 198051.0, 145259.0, 10482.0, 170582.0, 166472.0, 130086.0, 14562.0, 29809.0, 52467.0, 1341.0, 0.0, 1363.0, 0.0, 241523.0, 212513.0, 170499.0, 35577.0, 188463.0, 156565.0, 34942.0, 42774.0, 0.0, 3086.0, 193814.0, 160725.0, 70301.0, 122529.0, 33943.0, 10748.0, 104195.0, 78334.0, 23847.0, 5290.0, 204465.0, 195053.0, 161353.0, 115495.0, 19468.0, 149481.0, 123901.0, 121275.0, 25768.0, 48370.0, 43904.0, 12618.0, 0.0, 0.0, 0.0, 209328.0, 179299.0, 149224.0, 78412.0, 131480.0, 138343.0, 44158.0, 48710.0, 9879.0, 0.0, 150510.0, 148252.0, 78229.0, 114615.0, 52119.0, 509.0, 117579.0, 60026.0, 24338.0, 16212.0, 77436.0, 75178.0, 74427.0, 39565.0, 68605.0, 28384.0, 43343.0, 24103.0, 110652.0, 3990.0, 5150.0, 28400.0, 1931.0, 0.0, 0.0, 65449.0, 112941.0, 74243.0, 126344.0, 38296.0, 53198.0, 132729.0, 5111.0, 78831.0, 0.0, 58398.0, 92539.0, 138845.0, 40625.0, 141984.0, 27290.0, 28618.0, 137078.0, 69689.0, 70966.0, 99033.0, 115941.0, 89117.0, 41441.0, 11783.0, 71847.0, 30507.0, 36412.0, 5813.0, 5354.0, 0.0, 0.0, 0.0, 0.0, 0.0, 149774.0, 112320.0, 100849.0, 22690.0, 52187.0, 66258.0, 6266.0, 7382.0, 10625.0, 0.0, 84986.0, 97498.0, 56016.0, 10352.0, 0.0, 1519.0, 41869.0, 42991.0, 0.0, 0.0, 46695.0, 65718.0, 69431.0, 34411.0, 98302.0, 15046.0, 22986.0, 5484.0, 72586.0, 0.0, 4341.0, 15377.0, 0.0, 0.0, 0.0, 71428.0, 91398.0, 70981.0, 124383.0, 13526.0, 53478.0, 136302.0, 0.0, 29368.0, 0.0, 44335.0, 72414.0, 139187.0, 10297.0, 127444.0, 7630.0, 7920.0, 123658.0, 42663.0, 41334.0, 5885.0, 5403.0, 7828.0, 5939.0, 30017.0, 0.0, 0.0, 4949.0, 19656.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 4204.0, 16349.0, 8763.0, 49367.0, 0.0, 0.0, 18156.0, 0.0, 0.0, 0.0, 4973.0, 13970.0, 50231.0, 0.0, 16341.0, 0.0, 2438.0, 22191.0, 8093.0, 3119.0, 172551.0, 169487.0, 116414.0, 70678.0, 11180.0, 63285.0, 86409.0, 73329.0, 13740.0, 12938.0, 22511.0, 2433.0, 0.0, 0.0, 0.0, 145111.0, 152402.0, 132038.0, 19200.0, 41385.0, 100806.0, 32658.0, 17789.0, 0.0, 0.0, 102461.0, 137320.0, 41115.0, 30827.0, 6017.0, 3681.0, 39701.0, 12341.0, 7222.0, 0.0, 36905.0, 67002.0, 35837.0, 3721.0, 78820.0, 13588.0, 30814.0, 24500.0, 93622.0, 0.0, 5722.0, 50105.0, 0.0, 3510.0, 0.0, 60597.0, 97973.0, 61249.0, 135055.0, 12658.0, 42136.0, 116654.0, 772.0, 66011.0, 0.0, 33052.0, 68695.0, 138047.0, 21675.0, 116610.0, 814.0, 21669.0, 115862.0, 15575.0, 33328.0, 8324.0, 37383.0, 9350.0, 1494.0, 17505.0, 901.0, 7481.0, 7123.0, 21018.0, 0.0, 0.0, 8770.0, 0.0, 0.0, 0.0, 26607.0, 29498.0, 6549.0, 89470.0, 4920.0, 863.0, 47578.0, 0.0, 14253.0, 0.0, 13627.0, 20350.0, 95529.0, 3584.0, 21255.0, 0.0, 0.0, 47448.0, 0.0, 2352.0, 17737.0, 12985.0, 15103.0, 1662.0, 19237.0, 6492.0, 10755.0, 0.0, 3180.0, 0.0, 2446.0, 21470.0, 0.0, 0.0, 0.0, 20112.0, 27098.0, 26934.0, 22613.0, 0.0, 5596.0, 40031.0, 0.0, 2484.0, 0.0, 4488.0, 25345.0, 57677.0, 2438.0, 48697.0, 0.0, 6035.0, 17732.0, 0.0, 0.0  } };
    
    char str[20];
    int x, y;
    unsigned int apptask, app, task, slot, taskType, cluster, base_addr, cluster_size, last_app;
    unsigned int addr, j, i, k;
    unsigned int state_stability[DIM_X*DIM_Y];
    unsigned int state_last[DIM_X*DIM_Y];

    int starting_fit[DIM_X*DIM_Y];
    int current_fit;

    static unsigned int sorted_addr[DIM_X*DIM_Y];
    static unsigned int sorted_score[DIM_X*DIM_Y];

    //---------------------------------------
    // --------- Q-learning stuff -----------
    unsigned int tp, state, maxid;
    // Hyperparameters
    float epsilon = 0.1;
    float alpha = 0.01;
    float gamma = 0.6;
    float oldvalue, maxval, reward, delta;

    //policy table initialization
    for(tp = 0; tp < N_TASKTYPE; tp++){
        for(state = 0; state < N_STATES; state++){
            scoreTable[tp][state] = scoreTable[tp][state]/1000;
            //scoreTable[tp][state] = 0.0;//random();
        }
    }

	// Initialize the System Tiles Info
	API_TilesReset();

    Tiles[getXpos(GLOBAL_MASTER_ADDR)][getYpos(GLOBAL_MASTER_ADDR)].taskType = 1;
    Tiles[getXpos(GLOBAL_MASTER_ADDR)][getYpos(GLOBAL_MASTER_ADDR)].clusterCount = 1;

	// Initialize the applications vector
    API_ApplicationsReset();

	// Informs the Repository that the GLOBALMASTER is ready to receive the application info
	API_RepositoryWakeUp();

    API_PrintScoreTable(scoreTable);
	for(;;){
		API_setFreqScale(1000);
        API_applyFreqScale();
        tick = xTaskGetTickCount();
        if(tick >= SIMULATION_MAX_TICKS) _exit(0xfe10);
		myItoa(tick, str, 10);
		UART_polled_tx_string( &g_uart, (const uint8_t *)str);
		printsv("GlobalMasterActive", tick);
		UART_polled_tx_string( &g_uart, (const uint8_t *)" GlobalMasterRoutine...\r\n" );

        // prints the occupation
        API_PrintOccupation(tick);

        // Checks if there is some task to allocate
        if(API_CheckTaskToAllocate(xTaskGetTickCount())){
            
            last_app = -1;

            while(1){
                apptask = API_GetNextTaskToAllocate(xTaskGetTickCount());
                if (apptask == 0xFFFFFFFF) break;
                app = (apptask & 0xFFFF0000) >> 16;
                task = (apptask & 0x0000FFFF);
                printsvsv("Allocating task ", task, "from app ", app);
                if(last_app != app){
                    API_FindSmallerFITCluster(app);
                    base_addr = makeAddress(0,0); //applications[app].cluster_addr;
                    cluster_size = DIM_X; //applications[app].cluster_size;
                    printsvsv("Cluster addr: ", base_addr, " cluster size: ", cluster_size);
                }

                // fill the auxiliar vectors
                k = 0;
                for( i = getXpos(base_addr); i < (getXpos(base_addr)+cluster_size); i++){
                    for( j = getYpos(base_addr); j < (getYpos(base_addr)+cluster_size); j++){
                        sorted_addr[k] = makeAddress(i, j);
                        sorted_score[k] = (int)(scoreTable[applications[app].taskType[task]][API_getPEState(addr2id(makeAddress(i,j)), -1)]*1000);
                        k++;
                    }
                }
                
                // sort addrs by score
                quickSort(sorted_score, sorted_addr, 0, cluster_size*cluster_size);
                
                if( (int)(epsilon*100) < random()%100 ){ // try something new
                    //////////////////////////////////////////////
                    // gets a random tile 
                    do{
                        addr = sorted_addr[random()%(cluster_size*cluster_size)];
                        slot = API_GetTaskSlotFromTile(addr, app, task);
                    }while (slot == ERRO);
                } else{ // uses the learned information
                    // try to get the best tile slot
                    for(i = (cluster_size*cluster_size-1); i >= 0; i--){
                        addr = sorted_addr[i];
                        slot = API_GetTaskSlotFromTile(addr, app, task);
                        if (slot != ERRO) break;
                    }
                }

                // register the application allocation
                applications[app].tasks[task].addr = addr;
                applications[app].tasks[task].slot = slot;
                applications[app].lastStart = applications[app].nextRun;
                API_RepositoryAllocation(app, task, addr);
                debug_task_alloc(tick, app, task, addr);

                last_app = app;

                // Checks if there is some task to start...
                API_StartTasks();
            }

        } else if(tick > 200) { // updates the score table
            
            for(i = 0; i < DIM_X*DIM_Y; i++){
                if(API_CheckTaskToAllocate(xTaskGetTickCount())) break;
                addr = id2addr(i);
                
                // gets the tasktype that is running in the PE 
                taskType = Tiles[getXpos(addr)][getYpos(addr)].taskType;

                // if the PE is running some task
                if ( taskType != -1 ) {
                    
                    // gets the current PE state
                    state = API_getPEState(i, -1);

                    // checks if this PE did not changed its state
                    if( state != state_last[i] ){ 
                        
                        state_last[i] = state;
                        starting_fit[i] = API_getFIT(addr);
                        state_stability[i] = 0;
                    
                    } else if ( state_stability[i] < 50 ){ // waits until the PE is in a stable state
                        
                        state_stability[i]++;

                    } else if ( state == state_last[i] && state_stability[i] >= 50 ) {
                        // calculates the reward
                        current_fit = (int)API_getFIT(addr);
                        delta = (float)((float)(current_fit/100) - (float)(starting_fit[i]/100));
                        if(delta > 0)
                            printsv("delta FIT: +", (int)delta);
                        else
                            printsv("delta FIT: -", (int)(delta*-1));
                        //reward =  (Q_rsqrt(7000+(delta*delta)) * delta * -500) + 500;
                        reward =  (Q_rsqrt(10000+(delta*delta)) * delta * -100) + 100;
                        printsvsv("state ", state, "woned reward: ", (int)reward);

                        // gets the old value
                        oldvalue = scoreTable[taskType][state];

                        // gets the max value from the table
                        maxid = API_getMaxIdxfromRow(scoreTable, taskType, N_STATES, N_TASKTYPE);
                        maxval = scoreTable[taskType][maxid];

                        // updates the score table
                        scoreTable[taskType][state] = (1 - alpha) * oldvalue + alpha * ( reward + gamma * maxval);
                        //tableUpdate[taskType][state] = tableUpdate[taskType][state] + 1;
                        
                        // saves the current FIT for the next update
                        state_stability[i] = 0;
                        starting_fit[i] = API_getFIT(addr);

                        // print score table
                        toprint++;
                        if(toprint > 100){
                            API_PrintScoreTable(scoreTable);
                            //API_PrintScoreTable(tableUpdate);
                            toprint=0;
                        }
                    }
                } else {
                    state_last[i] = -1;
                }
            }
        }
		
		// Checks if there is some task to start...
		API_StartTasks();

        // Enters in idle
        API_setFreqIdle();
        API_applyFreqScale();
        if(API_SystemFinish){
            vTaskEnterCritical();
            for(i = 0; i <= (DIM_X); i++){
                for(j = 0; j <= (DIM_Y); j++){
                    printsvsv("PE: ", addr2id(i<<8|j), "cluster count: ", Tiles[i][j].clusterCount);
                }
            }
            vTaskExitCritical();
            //API_PrintPolicyTable();
            API_PrintScoreTable(scoreTable);
            //vTaskDelay(100); // to cool down the system
            _exit(0xfe10);
        }
        else if(!API_CheckTaskToAllocate(xTaskGetTickCount())){
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
    int options[4] = {(6<<8)|4, (5<<8)|5, (4<<8)|4, (4<<8)|6};
    for(i=0; i<(DIM_X*DIM_Y); i++){
        priorityMatrix[i] = options[i%4];//(5 << 8) | 5; //(x << 8) | y;
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
		API_AllocateTasks(tick, 0);

		
		// Checks if there is some task to start...
		API_StartTasks();

        // Enters in idle
        API_setFreqIdle();
        API_applyFreqScale();
        
		if(API_SystemFinish){
			////vTaskDelay(100); // to cool down the system
			_exit(0xfe10);
		}
		else{
			vTaskDelay(1);
		}
	}
}

/*-----------------------------------------------------------*/
#elif THERMAL_MANAGEMENT == 7 // WORST

static void GlobalManagerTask( void *pvParameters ){
	( void ) pvParameters;
	unsigned int tick;
	char str[20];

	// Initialize the priority vector with the spiral policy
	generateWorstMatrix();

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

        // prints the occupation
        API_PrintOccupation(tick);    

		// Checks if there is some task to allocate...
		API_AllocateTasks(tick, 0);
		
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


/*for(i = 0; i < NUM_MAX_APPS; i++){
                
                //if(API_CheckTaskToAllocate(xTaskGetTickCount())) break;

                if(applications[i].occupied == TRUE){
                    migrationPossible = TRUE;
                    for(j = 0; j < applications[i].numTasks; j++){
                        if( applications[i].tasks[j].status != TASK_STARTED ){
                            migrationPossible = FALSE;
                            break;
                        }
                    }
                    if(migrationPossible == TRUE){
                        candidate = 0xFFFFFFFF;
                        candidate_score = 0;
                        candidate_type = 0;
                        candidate_temp = 0;
                        for( j = 0; j < applications[i].numTasks; j++){
                            // if the task can migrate - get the "candidate" with the smallest score to migrate
                            if( applications[i].tasks[j].migration == TRUE ){
                                j_type = applications[i].taskType[j];
                                j_state = API_getPEState(addr2id(applications[i].tasks[j].addr), -1);

                                j_temp = Tiles[getXpos(applications[i].tasks[j].addr)][getYpos(applications[i].tasks[j].addr)].temperature;
                                if( candidate = 0xFFFFFFFF ){
                                    candidate_score = scoreTable[j_type][j_state];
                                    candidate = j;
                                    candidate_type = j_type;
                                    candidate_temp = j_temp;
                                } else{
                                    if( candidate_temp < j_temp ){
                                        candidate_score = scoreTable[j_type][j_state];
                                        candidate = j;
                                        candidate_type = j_type;
                                        candidate_temp = j_temp;
                                    }
                                }
                            }
                        }
                        if( candidate != 0xFFFFFFFF ){
                            // find the best PE within the application cluster to send the candidate
                            base_addr = applications[i].cluster_addr;
                            cluster_size = applications[i].cluster_size;
                            // fill the auxiliar vectors
                            k = 0;
                            for( i = getXpos(base_addr); i < (getXpos(base_addr)+cluster_size); i++){
                                for( j = getYpos(base_addr); j < (getYpos(base_addr)+cluster_size); j++){
                                    sorted_addr[k] = makeAddress(i, j);
                                    sorted_score[k] = (int)(scoreTable[applications[app].taskType[task]][API_getPEState(addr2id(makeAddress(i,j)),-1)]*1000);

                                    k++;
                                }
                            }
                            // sort addrs by score
                            quickSort(sorted_score, sorted_addr, 0, cluster_size*cluster_size);
                            
                            // try to get the best tile slot
                            for(i = (cluster_size*cluster_size-1); i >= 0; i--){
                                addr = sorted_addr[i];
                                slot = API_GetTaskSlotFromTile(addr, app, task);
                                if( candidate_score > sorted_score[i] ){
                                    addr = 0xFFFFFFFF;
                                    break;
                                }
                                else if (slot != ERRO){
                                    break;
                                }
                                else if (i == 0){
                                    addr = 0xFFFFFFFF;
                                }
                            }
                            if(addr != 0xFFFFFFFF){
                                API_StartMigration(i, candidate, addr);
                                printsv("Migrating it to ", addr);
                            }
                        }
                    }
                }
            }*/

