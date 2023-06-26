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


#elif THERMAL_MANAGEMENT == 6 // CHRONOS3

static void GlobalManagerTask( void *pvParameters ){
    ( void ) pvParameters;
	unsigned int tick, toprint;
    float scoreTable[N_TASKTYPE][N_STATES] =  {  {272203.0, 248272.0, 238454.0, 135899.0, 14294.0, 249263.0, 241423.0, 165584.0, 2525.0, 153149.0, 34012.0, 3564.0, 4278.0, 0.0, 0.0, 276042.0, 245998.0, 224482.0, 23409.0, 247051.0, 203651.0, 9442.0, 53905.0, 0.0, 0.0, 257325.0, 220338.0, 36160.0, 192962.0, 52057.0, 0.0, 144280.0, 40221.0, 0.0, 5579.0, 259844.0, 250034.0, 228225.0, 180499.0, 29464.0, 231451.0, 214891.0, 196348.0, 1697.0, 57508.0, 136219.0, 9986.0, 40878.0, 2637.0, 0.0, 255168.0, 237109.0, 220791.0, 63374.0, 226109.0, 213064.0, 61006.0, 147788.0, 19771.0, 0.0, 238532.0, 230486.0, 68433.0, 142937.0, 48667.0, 0.0, 162416.0, 55638.0, 7170.0, 12190.0, 199203.0, 221782.0, 179423.0, 140211.0, 12939.0, 132694.0, 148358.0, 148027.0, 11191.0, 36759.0, 70854.0, 3296.0, 7350.0, 0.0, 0.0, 219246.0, 206163.0, 203357.0, 59671.0, 144827.0, 210631.0, 37319.0, 75504.0, 2878.0, 0.0, 190586.0, 211275.0, 84195.0, 161779.0, 71164.0, 0.0, 146804.0, 89246.0, 6766.0, 9963.0, 76060.0, 79814.0, 74427.0, 47402.0, 9099.0, 21987.0, 32815.0, 31984.0, 0.0, 10796.0, 5663.0, 4503.0, 0.0, 0.0, 0.0, 106422.0, 151576.0, 78227.0, 42498.0, 67576.0, 64158.0, 33099.0, 1716.0, 12723.0, 0.0, 88686.0, 85146.0, 57616.0, 24590.0, 49824.0, 1150.0, 46582.0, 47567.0, 17816.0, 7386.0, 4291.0, 28125.0, 6978.0, 3796.0, 4590.0, 4429.0, 8099.0, 1558.0, 28033.0, 0.0, 4007.0, 9342.0, 0.0, 0.0, 0.0, 8362.0, 21982.0, 24244.0, 82199.0, 7448.0, 5226.0, 103339.0, 0.0, 9129.0, 0.0, 25190.0, 31449.0, 144166.0, 1945.0, 98239.0, 19596.0, 0.0, 65241.0, 24956.0, 11057.0, 227520.0, 220874.0, 179912.0, 86579.0, 11916.0, 105806.0, 115891.0, 94072.0, 3483.0, 30034.0, 28288.0, 1265.0, 0.0, 0.0, 0.0, 239307.0, 201417.0, 187695.0, 22866.0, 91624.0, 147297.0, 10132.0, 12359.0, 0.0, 0.0, 184698.0, 112080.0, 13535.0, 77460.0, 1303.0, 0.0, 69027.0, 14909.0, 0.0, 0.0, 128511.0, 175355.0, 136022.0, 98625.0, 6608.0, 59001.0, 35994.0, 112970.0, 18811.0, 4444.0, 11066.0, 3614.0, 0.0, 0.0, 0.0, 183151.0, 177855.0, 189954.0, 38897.0, 82631.0, 147487.0, 7566.0, 40112.0, 0.0, 0.0, 128021.0, 192828.0, 59789.0, 121914.0, 13935.0, 0.0, 72357.0, 18008.0, 8824.0, 5323.0, 125863.0, 123071.0, 101214.0, 27695.0, 2559.0, 11685.0, 47413.0, 54753.0, 6584.0, 2446.0, 6546.0, 0.0, 0.0, 0.0, 0.0, 65310.0, 116784.0, 110975.0, 38197.0, 51054.0, 40646.0, 35113.0, 0.0, 5318.0, 0.0, 91403.0, 76848.0, 44952.0, 24206.0, 21027.0, 3028.0, 21111.0, 49916.0, 832.0, 0.0, 7926.0, 22409.0, 16162.0, 14063.0, 43988.0, 4352.0, 0.0, 2040.0, 41043.0, 0.0, 4065.0, 15686.0, 0.0, 0.0, 0.0, 19515.0, 61607.0, 35197.0, 119838.0, 23546.0, 14014.0, 63248.0, 0.0, 11007.0, 0.0, 14569.0, 40630.0, 161854.0, 4981.0, 93072.0, 0.0, 2485.0, 92351.0, 36287.0, 19548.0, 1912.0, 21457.0, 14987.0, 13241.0, 2362.0, 6542.0, 10625.0, 8537.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 5099.0, 18287.0, 57983.0, 2596.0, 7758.0, 7484.0, 0.0, 0.0, 0.0, 0.0, 8167.0, 12740.0, 1366.0, 15801.0, 0.0, 0.0, 2714.0, 10947.0, 0.0, 0.0, 1165.0, 20341.0, 17685.0, 3339.0, 0.0, 5979.0, 20456.0, 5693.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 26410.0, 35512.0, 26907.0, 3621.0, 6038.0, 19340.0, 0.0, 0.0, 0.0, 0.0, 23382.0, 27743.0, 9842.0, 2445.0, 6504.0, 0.0, 16243.0, 3055.0, 0.0, 0.0, 17603.0, 13388.0, 13132.0, 1803.0, 7037.0, 2051.0, 4811.0, 0.0, 18172.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 13032.0, 5158.0, 62403.0, 0.0, 3501.0, 30734.0, 0.0, 13461.0, 0.0, 8562.0, 10398.0, 78064.0, 0.0, 27757.0, 0.0, 10389.0, 22732.0, 8558.0, 0.0, 1879.0, 14542.0, 4203.0, 0.0, 0.0, 0.0, 1885.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 2386.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 8849.0, 2326.0, 8990.0, 1393.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 5696.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 12274.0, 0.0, 10579.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 274174.0, 247385.0, 226793.0, 191129.0, 7515.0, 240666.0, 230205.0, 193750.0, 10237.0, 65742.0, 139001.0, 7797.0, 0.0, 0.0, 0.0, 257960.0, 245331.0, 213321.0, 37493.0, 231414.0, 209854.0, 45540.0, 52202.0, 1171.0, 0.0, 260945.0, 222258.0, 73343.0, 161300.0, 57813.0, 2148.0, 174112.0, 33047.0, 36055.0, 15887.0, 260922.0, 239225.0, 224100.0, 193668.0, 42552.0, 195749.0, 212084.0, 185035.0, 32777.0, 57819.0, 112368.0, 5432.0, 3849.0, 0.0, 0.0, 255478.0, 243069.0, 209399.0, 94267.0, 211931.0, 211471.0, 103128.0, 122783.0, 29472.0, 0.0, 239136.0, 216705.0, 135259.0, 196298.0, 72657.0, 1923.0, 175765.0, 71214.0, 11187.0, 32189.0, 207302.0, 193944.0, 171406.0, 138575.0, 23958.0, 120235.0, 150964.0, 125613.0, 45452.0, 18253.0, 36129.0, 9213.0, 2344.0, 0.0, 0.0, 190073.0, 209419.0, 200391.0, 64595.0, 106181.0, 189187.0, 76437.0, 26853.0, 28242.0, 0.0, 190470.0, 207998.0, 101465.0, 157320.0, 51677.0, 3730.0, 138455.0, 68271.0, 13714.0, 35583.0, 55813.0, 70654.0, 67920.0, 36131.0, 57083.0, 11927.0, 27725.0, 23427.0, 77796.0, 20464.0, 3290.0, 52722.0, 0.0, 0.0, 0.0, 76442.0, 106414.0, 112817.0, 193117.0, 5688.0, 38145.0, 179527.0, 1231.0, 88961.0, 16926.0, 84872.0, 93645.0, 194898.0, 20328.0, 180526.0, 36575.0, 31364.0, 181264.0, 62916.0, 64699.0, 217589.0, 195500.0, 160882.0, 128858.0, 3637.0, 92864.0, 122004.0, 125630.0, 217.0, 1408.0, 19864.0, 0.0, 0.0, 0.0, 0.0, 217373.0, 212016.0, 185483.0, 14459.0, 64256.0, 85710.0, 4287.0, 5768.0, 0.0, 0.0, 105301.0, 154566.0, 41011.0, 58281.0, 18108.0, 0.0, 76712.0, 7773.0, 0.0, 1516.0, 162371.0, 131493.0, 140221.0, 77828.0, 16083.0, 25306.0, 50446.0, 75537.0, 6430.0, 3840.0, 6061.0, 1081.0, 0.0, 0.0, 0.0, 119740.0, 192255.0, 178420.0, 72263.0, 29512.0, 103409.0, 36758.0, 27324.0, 1579.0, 0.0, 132247.0, 188613.0, 87573.0, 61794.0, 12199.0, 0.0, 95667.0, 59252.0, 5526.0, 0.0, 9731.0, 40150.0, 62432.0, 30176.0, 83698.0, 16599.0, 32340.0, 28182.0, 89268.0, 0.0, 8746.0, 25735.0, 0.0, 0.0, 0.0, 58561.0, 97289.0, 96425.0, 188122.0, 30240.0, 49830.0, 157949.0, 0.0, 55732.0, 0.0, 62920.0, 90082.0, 190426.0, 5997.0, 154896.0, 4155.0, 32081.0, 172323.0, 49772.0, 67243.0, 0.0, 23301.0, 17493.0, 12186.0, 5035.0, 8578.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 15771.0, 33303.0, 26394.0, 12660.0, 2810.0, 16141.0, 0.0, 0.0, 0.0, 0.0, 5068.0, 48027.0, 4119.0, 310.0, 0.0, 0.0, 6203.0, 7826.0, 0.0, 2292.0, 4302.0, 6527.0, 7241.0, 4967.0, 35152.0, 0.0, 1099.0, 4983.0, 8672.0, 0.0, 2621.0, 7833.0, 0.0, 0.0, 0.0, 10333.0, 55115.0, 10570.0, 146093.0, 8340.0, 5306.0, 74538.0, 0.0, 11531.0, 0.0, 17027.0, 18569.0, 115619.0, 0.0, 24510.0, 0.0, 6833.0, 50189.0, 13418.0, 1449.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 3172.0, 0.0, 7748.0, 0.0, 2328.0, 16425.0, 0.0, 0.0, 0.0, 0.0, 0.0, 9838.0, 0.0, 0.0, 0.0, 0.0, 4642.0, 0.0, 0.0, 262160.0, 238258.0, 213188.0, 162760.0, 2362.0, 153886.0, 191750.0, 128146.0, 16225.0, 36776.0, 97928.0, 4259.0, 0.0, 13580.0, 0.0, 258743.0, 239580.0, 203302.0, 46716.0, 182126.0, 178680.0, 31304.0, 42393.0, 1197.0, 0.0, 239603.0, 202646.0, 85646.0, 100226.0, 2738.0, 6352.0, 124800.0, 29046.0, 0.0, 14899.0, 194938.0, 203811.0, 197878.0, 166444.0, 12314.0, 85858.0, 131642.0, 171644.0, 16835.0, 6684.0, 48366.0, 0.0, 0.0, 0.0, 0.0, 224405.0, 215904.0, 197269.0, 91209.0, 90787.0, 200906.0, 78764.0, 44011.0, 11172.0, 0.0, 184272.0, 205373.0, 99169.0, 123007.0, 52497.0, 12258.0, 132924.0, 89883.0, 8057.0, 15675.0, 37165.0, 74969.0, 70234.0, 54340.0, 141441.0, 29674.0, 57030.0, 28693.0, 135237.0, 3653.0, 7715.0, 56350.0, 0.0, 4930.0, 0.0, 74923.0, 132177.0, 108112.0, 191330.0, 52141.0, 67510.0, 196032.0, 10634.0, 97869.0, 0.0, 70805.0, 124647.0, 198751.0, 12701.0, 192710.0, 27678.0, 48693.0, 189969.0, 77467.0, 62854.0, 97292.0, 66564.0, 91534.0, 77003.0, 2029.0, 13334.0, 28604.0, 32654.0, 4514.0, 0.0, 10593.0, 7563.0, 0.0, 0.0, 0.0, 147624.0, 143676.0, 139983.0, 30420.0, 29121.0, 68852.0, 9262.0, 23690.0, 0.0, 0.0, 40855.0, 87479.0, 11799.0, 54353.0, 4987.0, 0.0, 99617.0, 35349.0, 281.0, 1411.0, 48725.0, 61797.0, 65638.0, 29252.0, 116946.0, 9990.0, 18559.0, 24825.0, 106182.0, 0.0, 2340.0, 57872.0, 0.0, 0.0, 0.0, 29702.0, 115129.0, 88128.0, 180392.0, 14705.0, 22103.0, 173374.0, 0.0, 46655.0, 0.0, 64677.0, 86968.0, 184670.0, 1253.0, 130841.0, 12449.0, 14675.0, 139688.0, 43875.0, 49635.0, 2960.0, 3519.0, 1497.0, 3019.0, 35189.0, 0.0, 4524.0, 0.0, 14569.0, 0.0, 0.0, 16614.0, 0.0, 0.0, 0.0, 5538.0, 6882.0, 12676.0, 104031.0, 0.0, 0.0, 4359.0, 0.0, 0.0, 0.0, 10637.0, 4953.0, 76197.0, 0.0, 29871.0, 0.0, 0.0, 14500.0, 0.0, 0.0, 156822.0, 146574.0, 89122.0, 75475.0, 9399.0, 45985.0, 42057.0, 82185.0, 8377.0, 2208.0, 21179.0, 0.0, 0.0, 0.0, 0.0, 147150.0, 161679.0, 164356.0, 55324.0, 59741.0, 150955.0, 18497.0, 1161.0, 0.0, 0.0, 120627.0, 172331.0, 36710.0, 41246.0, 2513.0, 0.0, 40313.0, 22208.0, 0.0, 0.0, 35694.0, 51265.0, 56193.0, 18938.0, 83380.0, 13633.0, 20667.0, 19315.0, 83627.0, 0.0, 0.0, 41448.0, 0.0, 0.0, 0.0, 27768.0, 92257.0, 85246.0, 163084.0, 1368.0, 48729.0, 151669.0, 5854.0, 8737.0, 0.0, 53761.0, 50349.0, 185467.0, 23268.0, 116126.0, 21208.0, 36446.0, 108900.0, 38518.0, 50538.0, 0.0, 17320.0, 18207.0, 9937.0, 74448.0, 11156.0, 0.0, 5319.0, 46520.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 22314.0, 15695.0, 34146.0, 86499.0, 2685.0, 10936.0, 59703.0, 0.0, 12354.0, 0.0, 23192.0, 15574.0, 66612.0, 3853.0, 53104.0, 10178.0, 0.0, 16198.0, 0.0, 0.0, 6404.0, 7240.0, 14544.0, 0.0, 12826.0, 0.0, 0.0, 0.0, 17213.0, 0.0, 0.0, 5544.0, 0.0, 0.0, 0.0, 5782.0, 12920.0, 9536.0, 43310.0, 2504.0, 3293.0, 23933.0, 2220.0, 1880.0, 0.0, 2412.0, 13349.0, 51941.0, 0.0, 0.0, 0.0, 0.0, 0.0, 20044.0, 0.0  },
                                                 {257004.0, 218352.0, 170092.0, 36980.0, 0.0, 178167.0, 137979.0, 26315.0, 0.0, 20684.0, 3956.0, 6451.0, 0.0, 0.0, 0.0, 233357.0, 202849.0, 80979.0, 4883.0, 160039.0, 42482.0, 0.0, 9354.0, 3476.0, 0.0, 186266.0, 90066.0, 9554.0, 27661.0, 5084.0, 0.0, 38149.0, 1372.0, 0.0, 0.0, 219168.0, 201267.0, 149585.0, 84459.0, 0.0, 117943.0, 137652.0, 86393.0, 2971.0, 36467.0, 36017.0, 1654.0, 0.0, 0.0, 0.0, 211802.0, 182479.0, 140312.0, 22292.0, 137257.0, 103367.0, 15535.0, 44172.0, 0.0, 0.0, 162662.0, 143481.0, 10404.0, 63118.0, 6680.0, 0.0, 72912.0, 11619.0, 0.0, 0.0, 93790.0, 93999.0, 51622.0, 54027.0, 3274.0, 19826.0, 64281.0, 34584.0, 13187.0, 0.0, 24312.0, 0.0, 0.0, 0.0, 0.0, 79205.0, 119453.0, 73301.0, 11938.0, 33705.0, 59992.0, 5961.0, 5971.0, 0.0, 0.0, 77259.0, 99232.0, 26049.0, 14309.0, 7531.0, 0.0, 38590.0, 21626.0, 809.0, 0.0, 16594.0, 28659.0, 18338.0, 4520.0, 0.0, 4092.0, 9736.0, 558.0, 673.0, 0.0, 0.0, 1431.0, 0.0, 0.0, 0.0, 11332.0, 40140.0, 29076.0, 4511.0, 8883.0, 15363.0, 7341.0, 0.0, 0.0, 0.0, 9501.0, 22091.0, 7199.0, 4242.0, 2532.0, 0.0, 4285.0, 904.0, 0.0, 6293.0, 2873.0, 0.0, 2087.0, 1953.0, 0.0, 0.0, 3689.0, 613.0, 19171.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 4278.0, 5356.0, 10214.0, 26967.0, 1511.0, 6894.0, 32594.0, 0.0, 0.0, 0.0, 5443.0, 8240.0, 41583.0, 0.0, 17107.0, 0.0, 0.0, 14954.0, 9342.0, 12460.0, 137017.0, 126219.0, 83668.0, 8389.0, 0.0, 32274.0, 62316.0, 6456.0, 2722.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 97003.0, 129494.0, 44864.0, 0.0, 5866.0, 25959.0, 0.0, 0.0, 0.0, 0.0, 91969.0, 57630.0, 25012.0, 13727.0, 0.0, 0.0, 6136.0, 0.0, 0.0, 0.0, 55991.0, 77673.0, 35793.0, 38168.0, 3096.0, 8240.0, 6885.0, 18586.0, 5799.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 34580.0, 59152.0, 59606.0, 8625.0, 18508.0, 38975.0, 3551.0, 0.0, 0.0, 0.0, 33805.0, 64144.0, 18292.0, 0.0, 0.0, 0.0, 11764.0, 0.0, 0.0, 0.0, 19075.0, 35007.0, 20858.0, 1896.0, 0.0, 4054.0, 13860.0, 4696.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 25673.0, 27733.0, 30282.0, 7542.0, 4275.0, 8033.0, 771.0, 0.0, 0.0, 0.0, 8054.0, 15312.0, 4238.0, 2153.0, 0.0, 0.0, 3892.0, 2586.0, 3758.0, 0.0, 7437.0, 6268.0, 5379.0, 1315.0, 3560.0, 2287.0, 0.0, 0.0, 6217.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 9605.0, 6860.0, 0.0, 16982.0, 0.0, 0.0, 27962.0, 0.0, 738.0, 0.0, 1959.0, 8581.0, 29811.0, 0.0, 34045.0, 0.0, 5441.0, 37421.0, 13573.0, 35390.0, 0.0, 1554.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1818.0, 6660.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 3778.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1371.0, 3016.0, 0.0, 5230.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 4433.0, 4499.0, 1707.0, 3956.0, 0.0, 0.0, 0.0, 0.0, 0.0, 10931.0, 4708.0, 0.0, 0.0, 0.0, 0.0, 0.0, 4171.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 688.0, 499.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 8566.0, 0.0, 0.0, 0.0, 0.0, 2432.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 7883.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 8052.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 232423.0, 212802.0, 174184.0, 79794.0, 10576.0, 117364.0, 147943.0, 60588.0, 2990.0, 11129.0, 25651.0, 1772.0, 0.0, 0.0, 0.0, 226617.0, 198409.0, 128426.0, 4715.0, 128132.0, 78855.0, 9318.0, 0.0, 0.0, 0.0, 166569.0, 120509.0, 17100.0, 46444.0, 11209.0, 0.0, 21679.0, 0.0, 0.0, 0.0, 165335.0, 163841.0, 137814.0, 111549.0, 3082.0, 70423.0, 89426.0, 77372.0, 1044.0, 17027.0, 27109.0, 2318.0, 0.0, 0.0, 0.0, 164465.0, 174084.0, 138635.0, 16333.0, 95921.0, 119137.0, 21414.0, 14049.0, 1279.0, 0.0, 125258.0, 141240.0, 40183.0, 71250.0, 6381.0, 0.0, 64144.0, 5864.0, 0.0, 0.0, 32825.0, 60596.0, 73330.0, 61085.0, 2293.0, 14202.0, 37999.0, 24620.0, 10580.0, 2077.0, 2797.0, 0.0, 0.0, 0.0, 0.0, 76421.0, 93730.0, 90040.0, 26583.0, 13145.0, 65934.0, 6134.0, 3767.0, 1202.0, 0.0, 75540.0, 123875.0, 11671.0, 14432.0, 6847.0, 0.0, 48860.0, 17327.0, 0.0, 7185.0, 451.0, 12270.0, 17168.0, 2230.0, 42273.0, 6025.0, 6091.0, 0.0, 29769.0, 0.0, 0.0, 10397.0, 0.0, 1129.0, 0.0, 6138.0, 27873.0, 24104.0, 79063.0, 4084.0, 7835.0, 56880.0, 0.0, 23684.0, 0.0, 34644.0, 40352.0, 102366.0, 3904.0, 66517.0, 0.0, 387.0, 40310.0, 17879.0, 18596.0, 46518.0, 48631.0, 37002.0, 16485.0, 2827.0, 3807.0, 21169.0, 0.0, 471.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 46634.0, 65307.0, 51076.0, 3073.0, 14474.0, 38587.0, 9363.0, 0.0, 0.0, 0.0, 23068.0, 47177.0, 7180.0, 24624.0, 0.0, 0.0, 0.0, 1918.0, 0.0, 0.0, 24030.0, 54210.0, 42296.0, 12986.0, 564.0, 7918.0, 9766.0, 26319.0, 1841.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 40185.0, 42110.0, 70187.0, 8142.0, 6588.0, 35148.0, 4726.0, 0.0, 0.0, 0.0, 24128.0, 56104.0, 4223.0, 32541.0, 7310.0, 0.0, 20721.0, 4435.0, 0.0, 3797.0, 6073.0, 13042.0, 26702.0, 11281.0, 29096.0, 11713.0, 0.0, 1108.0, 17987.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 8790.0, 15720.0, 12294.0, 68620.0, 0.0, 1970.0, 20491.0, 0.0, 3919.0, 0.0, 17940.0, 5726.0, 83639.0, 3035.0, 27323.0, 0.0, 6240.0, 36572.0, 27376.0, 12956.0, 4021.0, 1490.0, 11244.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1666.0, 1698.0, 992.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 8766.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 2071.0, 0.0, 1123.0, 0.0, 1214.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 2570.0, 0.0, 3845.0, 2586.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 3501.0, 0.0, 15872.0, 0.0, 3038.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 152736.0, 144878.0, 75836.0, 72212.0, 0.0, 14800.0, 66485.0, 44735.0, 2744.0, 0.0, 625.0, 950.0, 0.0, 0.0, 0.0, 92967.0, 133909.0, 101502.0, 11748.0, 50362.0, 71638.0, 14752.0, 8637.0, 1816.0, 0.0, 76363.0, 93047.0, 18458.0, 29191.0, 9754.0, 0.0, 58684.0, 18410.0, 1068.0, 2715.0, 93584.0, 83564.0, 76975.0, 53493.0, 2025.0, 9467.0, 15754.0, 39504.0, 1843.0, 0.0, 3360.0, 923.0, 0.0, 0.0, 0.0, 65490.0, 105192.0, 111870.0, 10628.0, 15930.0, 86282.0, 5171.0, 4166.0, 0.0, 0.0, 69794.0, 127205.0, 32256.0, 67128.0, 3429.0, 0.0, 54369.0, 3149.0, 0.0, 0.0, 1215.0, 5308.0, 21842.0, 8285.0, 33141.0, 457.0, 8283.0, 6710.0, 45916.0, 0.0, 0.0, 17934.0, 0.0, 0.0, 0.0, 10371.0, 23444.0, 35915.0, 78937.0, 11826.0, 5518.0, 79714.0, 0.0, 19239.0, 0.0, 22098.0, 36417.0, 82563.0, 2291.0, 75586.0, 7477.0, 13163.0, 68054.0, 4237.0, 29693.0, 5017.0, 22349.0, 16021.0, 8892.0, 9153.0, 0.0, 0.0, 3437.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 10843.0, 13899.0, 21554.0, 4670.0, 11797.0, 1344.0, 1881.0, 0.0, 0.0, 0.0, 1511.0, 11834.0, 6681.0, 0.0, 0.0, 0.0, 1913.0, 0.0, 0.0, 0.0, 0.0, 5954.0, 6926.0, 2383.0, 31611.0, 2527.0, 0.0, 3576.0, 7744.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 13325.0, 16980.0, 40145.0, 0.0, 2863.0, 13105.0, 0.0, 0.0, 0.0, 3482.0, 6686.0, 74923.0, 0.0, 30423.0, 0.0, 2148.0, 40455.0, 16788.0, 6895.0, 0.0, 0.0, 0.0, 0.0, 8912.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 7139.0, 0.0, 2573.0, 15652.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 16503.0, 0.0, 2191.0, 0.0, 0.0, 0.0, 0.0, 0.0, 37598.0, 29126.0, 35275.0, 29121.0, 0.0, 1179.0, 2016.0, 11258.0, 1470.0, 0.0, 10341.0, 0.0, 0.0, 0.0, 0.0, 23100.0, 27638.0, 43887.0, 3751.0, 13047.0, 52852.0, 7943.0, 1241.0, 3152.0, 0.0, 12601.0, 30420.0, 4173.0, 18702.0, 0.0, 0.0, 29548.0, 0.0, 0.0, 1281.0, 0.0, 0.0, 24187.0, 6636.0, 18397.0, 3515.0, 4096.0, 1185.0, 12384.0, 0.0, 1135.0, 8073.0, 0.0, 0.0, 0.0, 8164.0, 16405.0, 30805.0, 51325.0, 766.0, 8196.0, 18905.0, 0.0, 17624.0, 0.0, 30106.0, 9772.0, 79807.0, 2217.0, 28276.0, 0.0, 2223.0, 34249.0, 7398.0, 22580.0, 0.0, 0.0, 13311.0, 9118.0, 11019.0, 0.0, 0.0, 0.0, 17994.0, 0.0, 0.0, 724.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 16254.0, 0.0, 0.0, 12305.0, 0.0, 5931.0, 0.0, 5051.0, 0.0, 24038.0, 0.0, 0.0, 0.0, 0.0, 3948.0, 0.0, 0.0, 0.0, 0.0, 0.0, 866.0, 13085.0, 2049.0, 2909.0, 1570.0, 19185.0, 0.0, 0.0, 11545.0, 0.0, 0.0, 0.0, 0.0, 0.0, 771.0, 24989.0, 3110.0, 796.0, 7252.0, 0.0, 0.0, 0.0, 0.0, 3516.0, 2555.0, 0.0, 4545.0, 0.0, 0.0, 589.0, 0.0, 0.0  },
                                                 {179984.0, 159892.0, 156364.0, 94330.0, 235.0, 162312.0, 155350.0, 100376.0, 8767.0, 77081.0, 44322.0, 7616.0, 0.0, 0.0, 0.0, 172158.0, 149635.0, 129964.0, 11935.0, 154436.0, 139998.0, 13392.0, 45197.0, 7196.0, 0.0, 168212.0, 145290.0, 16918.0, 127888.0, 14978.0, 0.0, 104984.0, 19075.0, 385.0, 11084.0, 169879.0, 152008.0, 139776.0, 108543.0, 5700.0, 153110.0, 138675.0, 128422.0, 14124.0, 73988.0, 127169.0, 558.0, 10973.0, 0.0, 0.0, 163489.0, 149558.0, 135054.0, 21332.0, 148587.0, 141941.0, 31785.0, 111498.0, 2152.0, 0.0, 153558.0, 149602.0, 41349.0, 147570.0, 29249.0, 1340.0, 142691.0, 27207.0, 2677.0, 8782.0, 142756.0, 135033.0, 106062.0, 64011.0, 2275.0, 65254.0, 98412.0, 102859.0, 13943.0, 45579.0, 43968.0, 7229.0, 10784.0, 0.0, 0.0, 159198.0, 144434.0, 133412.0, 35718.0, 89956.0, 127819.0, 40852.0, 56883.0, 14158.0, 0.0, 116612.0, 149578.0, 50228.0, 126653.0, 44780.0, 0.0, 113446.0, 32539.0, 0.0, 12758.0, 56616.0, 66901.0, 62253.0, 28745.0, 0.0, 16630.0, 35151.0, 12524.0, 2266.0, 2215.0, 4664.0, 1909.0, 5240.0, 0.0, 0.0, 63184.0, 90414.0, 42769.0, 12391.0, 26337.0, 38874.0, 12119.0, 2214.0, 3242.0, 0.0, 58365.0, 89917.0, 23419.0, 6929.0, 41365.0, 1581.0, 8666.0, 10376.0, 6191.0, 724.0, 6247.0, 18998.0, 9754.0, 6774.0, 12052.0, 5565.0, 8042.0, 6174.0, 27540.0, 2243.0, 0.0, 3748.0, 0.0, 2106.0, 0.0, 23727.0, 23887.0, 16172.0, 50837.0, 6901.0, 10650.0, 70222.0, 0.0, 10370.0, 6874.0, 9965.0, 34336.0, 112811.0, 17598.0, 76954.0, 9522.0, 6079.0, 75097.0, 62131.0, 27795.0, 158866.0, 141488.0, 109594.0, 62583.0, 4007.0, 85014.0, 84716.0, 70018.0, 12111.0, 28723.0, 39699.0, 0.0, 0.0, 0.0, 0.0, 148217.0, 137763.0, 132131.0, 6326.0, 87825.0, 109337.0, 8700.0, 4220.0, 3675.0, 0.0, 142914.0, 110630.0, 19905.0, 105304.0, 10007.0, 0.0, 73913.0, 4497.0, 2595.0, 7456.0, 112795.0, 123587.0, 102162.0, 30190.0, 5202.0, 68108.0, 72567.0, 91101.0, 3069.0, 1775.0, 11106.0, 0.0, 0.0, 0.0, 0.0, 121301.0, 144888.0, 122692.0, 17902.0, 63460.0, 120223.0, 10215.0, 24083.0, 6987.0, 0.0, 74618.0, 134951.0, 25727.0, 64350.0, 15212.0, 0.0, 63251.0, 27617.0, 11644.0, 1181.0, 42113.0, 73624.0, 44948.0, 38867.0, 3117.0, 19716.0, 6851.0, 23990.0, 8714.0, 0.0, 0.0, 1043.0, 0.0, 0.0, 0.0, 62369.0, 95513.0, 74504.0, 7533.0, 23584.0, 38063.0, 20155.0, 0.0, 1055.0, 0.0, 75238.0, 62191.0, 36757.0, 9427.0, 12435.0, 1262.0, 30120.0, 16812.0, 610.0, 11139.0, 6347.0, 38669.0, 8172.0, 10314.0, 16135.0, 3393.0, 16297.0, 7123.0, 27029.0, 0.0, 0.0, 12082.0, 0.0, 0.0, 0.0, 29257.0, 20740.0, 20255.0, 48377.0, 3168.0, 16411.0, 68259.0, 891.0, 9448.0, 0.0, 29561.0, 16964.0, 76041.0, 5774.0, 81793.0, 10479.0, 6461.0, 65979.0, 23071.0, 32320.0, 14399.0, 15031.0, 10868.0, 9992.0, 1992.0, 6335.0, 426.0, 833.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 2993.0, 31651.0, 7200.0, 0.0, 9573.0, 14912.0, 1676.0, 0.0, 0.0, 0.0, 15238.0, 4741.0, 2613.0, 0.0, 726.0, 0.0, 9926.0, 2137.0, 0.0, 0.0, 6025.0, 18122.0, 2923.0, 11021.0, 2002.0, 6438.0, 10476.0, 10815.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 34206.0, 30750.0, 28262.0, 5531.0, 0.0, 495.0, 0.0, 0.0, 0.0, 0.0, 5409.0, 15340.0, 11874.0, 2380.0, 0.0, 0.0, 0.0, 4180.0, 0.0, 0.0, 2294.0, 15444.0, 729.0, 0.0, 11334.0, 5590.0, 0.0, 4891.0, 20882.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 12521.0, 19571.0, 3084.0, 50170.0, 6380.0, 1301.0, 16021.0, 0.0, 0.0, 0.0, 5692.0, 3268.0, 73385.0, 0.0, 37048.0, 0.0, 0.0, 57360.0, 11023.0, 10254.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 5050.0, 4360.0, 1098.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1355.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 2506.0, 6625.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 3465.0, 0.0, 25955.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 17878.0, 0.0, 6947.0, 0.0, 0.0, 27645.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 176436.0, 167490.0, 147415.0, 117049.0, 18584.0, 161840.0, 147662.0, 135738.0, 9481.0, 63779.0, 71096.0, 0.0, 11840.0, 0.0, 0.0, 164343.0, 157040.0, 134870.0, 14622.0, 152128.0, 142174.0, 26710.0, 76413.0, 7526.0, 0.0, 171552.0, 157919.0, 29187.0, 144024.0, 20614.0, 827.0, 130448.0, 29012.0, 4344.0, 10245.0, 173434.0, 157044.0, 134797.0, 139797.0, 16786.0, 138910.0, 135237.0, 121280.0, 25306.0, 31463.0, 77427.0, 4983.0, 1614.0, 850.0, 0.0, 168665.0, 147241.0, 143470.0, 51619.0, 141626.0, 142966.0, 54778.0, 78843.0, 4336.0, 1778.0, 158409.0, 141543.0, 90852.0, 137675.0, 30883.0, 5303.0, 136447.0, 59889.0, 12321.0, 6507.0, 132489.0, 134228.0, 100107.0, 61529.0, 3294.0, 64339.0, 94161.0, 102571.0, 23617.0, 8926.0, 17118.0, 4584.0, 9677.0, 0.0, 0.0, 146868.0, 144681.0, 129844.0, 55682.0, 92267.0, 129361.0, 33652.0, 9920.0, 16379.0, 0.0, 117344.0, 142564.0, 76269.0, 105982.0, 59832.0, 9356.0, 117393.0, 44570.0, 5984.0, 28895.0, 10460.0, 21719.0, 41028.0, 10185.0, 56638.0, 10670.0, 44676.0, 28643.0, 79501.0, 0.0, 10867.0, 38398.0, 0.0, 0.0, 0.0, 50148.0, 63902.0, 58049.0, 131704.0, 23670.0, 43229.0, 121157.0, 5057.0, 94461.0, 3695.0, 49841.0, 73538.0, 140825.0, 13794.0, 131326.0, 24006.0, 27368.0, 129955.0, 83322.0, 37505.0, 110323.0, 136269.0, 99279.0, 85012.0, 5475.0, 47439.0, 85695.0, 91953.0, 2455.0, 12131.0, 5119.0, 0.0, 0.0, 0.0, 0.0, 150298.0, 127993.0, 132318.0, 8569.0, 57886.0, 114443.0, 10299.0, 8165.0, 1387.0, 0.0, 111702.0, 135508.0, 59311.0, 36862.0, 23787.0, 0.0, 83664.0, 19165.0, 11060.0, 2198.0, 77037.0, 102606.0, 87395.0, 48712.0, 13828.0, 23068.0, 51133.0, 59810.0, 2910.0, 4732.0, 4771.0, 0.0, 0.0, 0.0, 0.0, 98011.0, 127461.0, 115443.0, 29110.0, 68288.0, 97141.0, 21836.0, 13653.0, 0.0, 0.0, 73938.0, 119904.0, 57818.0, 40433.0, 40067.0, 0.0, 41270.0, 35439.0, 6683.0, 5334.0, 19537.0, 54247.0, 40587.0, 35158.0, 88444.0, 12783.0, 15148.0, 11432.0, 94729.0, 0.0, 3234.0, 16818.0, 0.0, 0.0, 0.0, 42404.0, 74458.0, 62507.0, 126140.0, 18584.0, 27170.0, 135285.0, 0.0, 20664.0, 0.0, 32719.0, 76750.0, 130556.0, 9370.0, 128528.0, 14066.0, 19490.0, 126904.0, 75038.0, 60712.0, 14441.0, 10049.0, 8333.0, 9649.0, 9528.0, 1518.0, 8571.0, 0.0, 0.0, 957.0, 0.0, 0.0, 0.0, 0.0, 0.0, 17230.0, 28846.0, 19642.0, 3240.0, 1319.0, 4802.0, 0.0, 3311.0, 0.0, 0.0, 8667.0, 17236.0, 17382.0, 0.0, 0.0, 0.0, 1836.0, 2006.0, 0.0, 0.0, 9335.0, 16375.0, 9758.0, 5812.0, 35888.0, 0.0, 0.0, 6980.0, 18158.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 5039.0, 21079.0, 19362.0, 89979.0, 2789.0, 3118.0, 83894.0, 0.0, 0.0, 0.0, 19754.0, 10548.0, 87528.0, 4033.0, 45638.0, 11584.0, 3114.0, 32658.0, 20878.0, 5943.0, 0.0, 0.0, 3222.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 16781.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 18991.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 174450.0, 149846.0, 149087.0, 115884.0, 10482.0, 138371.0, 113914.0, 113338.0, 13151.0, 15817.0, 48864.0, 1341.0, 0.0, 1363.0, 0.0, 173420.0, 164704.0, 137409.0, 31753.0, 142423.0, 136250.0, 33287.0, 37484.0, 0.0, 0.0, 159184.0, 143812.0, 63129.0, 106363.0, 20996.0, 10748.0, 80948.0, 55870.0, 22222.0, 5290.0, 136465.0, 141270.0, 142052.0, 106350.0, 19468.0, 82592.0, 95027.0, 114918.0, 22849.0, 26775.0, 40695.0, 10098.0, 0.0, 0.0, 0.0, 152000.0, 140973.0, 137292.0, 70835.0, 106059.0, 131875.0, 38268.0, 47962.0, 9879.0, 0.0, 114201.0, 139624.0, 71809.0, 104501.0, 46702.0, 509.0, 116812.0, 57966.0, 23390.0, 12715.0, 38492.0, 50858.0, 59151.0, 37095.0, 68605.0, 18216.0, 35660.0, 24103.0, 110652.0, 1324.0, 5150.0, 28400.0, 1931.0, 0.0, 0.0, 46668.0, 89568.0, 68189.0, 126344.0, 38296.0, 48420.0, 132729.0, 5111.0, 78831.0, 0.0, 47997.0, 92236.0, 139034.0, 40625.0, 141984.0, 27290.0, 23703.0, 137121.0, 69689.0, 70966.0, 48559.0, 58293.0, 58070.0, 26053.0, 11783.0, 35280.0, 26510.0, 27574.0, 436.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 101844.0, 97069.0, 91628.0, 17709.0, 21066.0, 62481.0, 3467.0, 7382.0, 10625.0, 0.0, 69758.0, 82398.0, 45774.0, 10352.0, 0.0, 0.0, 40129.0, 41605.0, 0.0, 0.0, 16841.0, 43832.0, 52093.0, 31088.0, 98302.0, 14029.0, 20950.0, 5484.0, 72586.0, 0.0, 4341.0, 15377.0, 0.0, 0.0, 0.0, 42124.0, 80767.0, 67434.0, 124383.0, 9876.0, 52679.0, 136378.0, 0.0, 29368.0, 0.0, 36265.0, 68699.0, 139187.0, 10297.0, 127444.0, 7630.0, 6792.0, 123658.0, 42663.0, 41334.0, 0.0, 644.0, 7828.0, 5939.0, 30017.0, 0.0, 0.0, 4949.0, 19656.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 4204.0, 11156.0, 8763.0, 49367.0, 0.0, 0.0, 18156.0, 0.0, 0.0, 0.0, 4973.0, 12897.0, 50231.0, 0.0, 16341.0, 0.0, 2438.0, 22191.0, 8093.0, 3119.0, 82390.0, 100402.0, 74620.0, 70678.0, 8913.0, 13429.0, 71333.0, 71264.0, 11254.0, 12938.0, 22511.0, 2433.0, 0.0, 0.0, 0.0, 111405.0, 119899.0, 123234.0, 19200.0, 34695.0, 94677.0, 32658.0, 17789.0, 0.0, 0.0, 75282.0, 131223.0, 26321.0, 26199.0, 4878.0, 3681.0, 36099.0, 12341.0, 3730.0, 0.0, 21818.0, 35562.0, 25970.0, 3721.0, 78820.0, 11262.0, 29753.0, 24500.0, 93622.0, 0.0, 5722.0, 50105.0, 0.0, 3510.0, 0.0, 36718.0, 77921.0, 61249.0, 134683.0, 12658.0, 41051.0, 116654.0, 772.0, 64374.0, 0.0, 31374.0, 61952.0, 138165.0, 21675.0, 116610.0, 814.0, 19503.0, 115862.0, 15575.0, 33328.0, 2344.0, 21734.0, 9350.0, 1494.0, 17505.0, 901.0, 6098.0, 7123.0, 21018.0, 0.0, 0.0, 8770.0, 0.0, 0.0, 0.0, 6092.0, 24499.0, 6549.0, 89470.0, 2644.0, 863.0, 47578.0, 0.0, 14253.0, 0.0, 2201.0, 18070.0, 95529.0, 3584.0, 21255.0, 0.0, 0.0, 47448.0, 0.0, 2352.0, 11278.0, 12985.0, 14044.0, 1662.0, 19237.0, 6492.0, 7860.0, 0.0, 3180.0, 0.0, 0.0, 21470.0, 0.0, 0.0, 0.0, 15320.0, 26015.0, 23872.0, 22613.0, 0.0, 2201.0, 40031.0, 0.0, 2484.0, 0.0, 4488.0, 22251.0, 56972.0, 2438.0, 48697.0, 0.0, 6035.0, 17732.0, 0.0, 0.0  } };
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
    //API_PrintScoreTable(scoreTable);

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
                        
                        // saves the current FIT for the next update
                        state_stability[i] = 0;
                        starting_fit[i] = API_getFIT(addr);

                        // print score table
                        toprint++;
                        if(toprint > 100){
                            API_PrintScoreTable(scoreTable);
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

