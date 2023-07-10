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
    float scoreTable[N_TASKTYPE][N_STATES] = {  {278719.0, 273911.0, 242453.0, 205229.0, 14294.0, 272926.0, 252437.0, 218642.0, 4333.0, 209411.0, 89309.0, 3564.0, 14946.0, 0.0, 0.0, 284882.0, 263597.0, 255677.0, 63284.0, 263296.0, 245950.0, 29062.0, 118821.0, 5774.0, 0.0, 268912.0, 250778.0, 60434.0, 237149.0, 65440.0, 0.0, 213302.0, 108184.0, 9092.0, 5579.0, 266734.0, 264977.0, 242706.0, 205656.0, 35478.0, 264676.0, 242464.0, 221038.0, 13987.0, 94611.0, 158259.0, 9986.0, 42201.0, 2637.0, 0.0, 265906.0, 251478.0, 238964.0, 94952.0, 253613.0, 237031.0, 87523.0, 165724.0, 33123.0, 0.0, 262777.0, 255931.0, 116827.0, 203627.0, 100378.0, 7129.0, 200775.0, 79353.0, 7170.0, 12190.0, 266567.0, 256867.0, 231240.0, 168507.0, 16474.0, 213696.0, 200099.0, 164654.0, 35580.0, 60555.0, 76474.0, 3296.0, 7350.0, 0.0, 0.0, 248006.0, 243448.0, 218367.0, 73237.0, 221373.0, 223800.0, 44207.0, 87012.0, 2878.0, 0.0, 233078.0, 221981.0, 90714.0, 185651.0, 90122.0, 0.0, 168191.0, 92784.0, 19093.0, 9963.0, 175300.0, 168799.0, 145255.0, 61834.0, 9099.0, 87106.0, 65727.0, 31984.0, 1762.0, 10796.0, 10287.0, 4503.0, 0.0, 0.0, 0.0, 176744.0, 205089.0, 127150.0, 47754.0, 80526.0, 87822.0, 43566.0, 1716.0, 12723.0, 0.0, 132603.0, 96550.0, 71908.0, 36134.0, 58809.0, 5828.0, 52042.0, 68090.0, 17816.0, 7386.0, 17418.0, 41077.0, 10408.0, 7213.0, 4590.0, 4429.0, 8099.0, 1558.0, 28033.0, 0.0, 4007.0, 9342.0, 0.0, 0.0, 0.0, 39025.0, 25478.0, 24244.0, 82199.0, 15882.0, 10213.0, 103339.0, 0.0, 9129.0, 0.0, 40997.0, 33223.0, 144166.0, 1945.0, 98239.0, 19596.0, 0.0, 65239.0, 24956.0, 11057.0, 255738.0, 268318.0, 242149.0, 131801.0, 24122.0, 197704.0, 184224.0, 122963.0, 13394.0, 46521.0, 36974.0, 1265.0, 0.0, 0.0, 0.0, 271134.0, 262474.0, 209474.0, 27592.0, 147348.0, 179761.0, 10132.0, 18739.0, 2593.0, 0.0, 240601.0, 166021.0, 19901.0, 98657.0, 12852.0, 0.0, 88540.0, 14909.0, 5003.0, 0.0, 246764.0, 252617.0, 198482.0, 113792.0, 27922.0, 136709.0, 104891.0, 117809.0, 18811.0, 7752.0, 18377.0, 3614.0, 0.0, 0.0, 0.0, 242393.0, 242854.0, 210326.0, 45686.0, 140937.0, 182280.0, 20820.0, 42713.0, 0.0, 0.0, 198269.0, 216323.0, 78104.0, 140822.0, 25385.0, 0.0, 101990.0, 23286.0, 23281.0, 20399.0, 170384.0, 183808.0, 149475.0, 29798.0, 2559.0, 52378.0, 80529.0, 61916.0, 6584.0, 11987.0, 6546.0, 0.0, 0.0, 0.0, 0.0, 129190.0, 192028.0, 134710.0, 45708.0, 76609.0, 60514.0, 44986.0, 0.0, 5318.0, 0.0, 146783.0, 104959.0, 49439.0, 38932.0, 21027.0, 3028.0, 35379.0, 56514.0, 14755.0, 0.0, 27644.0, 28829.0, 16162.0, 14063.0, 43988.0, 4352.0, 8017.0, 2040.0, 41043.0, 0.0, 4065.0, 15686.0, 0.0, 0.0, 0.0, 32606.0, 84433.0, 45534.0, 119838.0, 23546.0, 14014.0, 67137.0, 0.0, 11007.0, 0.0, 27197.0, 43692.0, 161854.0, 4981.0, 93072.0, 0.0, 2485.0, 94412.0, 36287.0, 19548.0, 27766.0, 44854.0, 74354.0, 13241.0, 2362.0, 8491.0, 12674.0, 8537.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 58236.0, 60779.0, 63213.0, 2596.0, 16542.0, 22271.0, 0.0, 0.0, 0.0, 0.0, 15640.0, 25535.0, 1366.0, 15801.0, 0.0, 0.0, 8433.0, 10947.0, 0.0, 0.0, 31031.0, 23428.0, 39054.0, 6087.0, 0.0, 11016.0, 23811.0, 5693.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 50801.0, 56216.0, 26907.0, 3621.0, 12079.0, 30623.0, 0.0, 0.0, 0.0, 0.0, 29858.0, 31402.0, 12947.0, 2445.0, 6504.0, 0.0, 27079.0, 3055.0, 0.0, 0.0, 17603.0, 15791.0, 18186.0, 1803.0, 7037.0, 2051.0, 8093.0, 1778.0, 18172.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 10027.0, 20981.0, 5158.0, 62403.0, 6786.0, 3501.0, 30734.0, 0.0, 13461.0, 0.0, 17358.0, 10398.0, 78064.0, 0.0, 27757.0, 0.0, 10389.0, 22732.0, 8558.0, 0.0, 1879.0, 16578.0, 4203.0, 0.0, 0.0, 0.0, 1885.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 12312.0, 9679.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 11010.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 8849.0, 8722.0, 8990.0, 1393.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 5696.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 12274.0, 0.0, 10579.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 279741.0, 270121.0, 263414.0, 220896.0, 19463.0, 265076.0, 257007.0, 221876.0, 21670.0, 127373.0, 148424.0, 7797.0, 0.0, 0.0, 0.0, 273265.0, 267480.0, 238477.0, 59659.0, 254685.0, 242355.0, 76748.0, 67719.0, 17393.0, 0.0, 257755.0, 252495.0, 115221.0, 191969.0, 74152.0, 2148.0, 202607.0, 51160.0, 40865.0, 15887.0, 265433.0, 262217.0, 240589.0, 206567.0, 59117.0, 252470.0, 248207.0, 203138.0, 40069.0, 109780.0, 135891.0, 10048.0, 3849.0, 0.0, 0.0, 266510.0, 263246.0, 236391.0, 124451.0, 257390.0, 235421.0, 118905.0, 125538.0, 43787.0, 0.0, 258225.0, 236438.0, 157340.0, 204148.0, 84830.0, 1923.0, 205013.0, 102798.0, 11187.0, 32189.0, 249721.0, 242636.0, 222163.0, 163266.0, 31169.0, 202100.0, 192692.0, 141022.0, 50842.0, 23688.0, 48873.0, 9213.0, 2344.0, 0.0, 0.0, 257857.0, 243714.0, 213903.0, 68665.0, 157056.0, 206663.0, 79932.0, 37901.0, 33032.0, 0.0, 226367.0, 219173.0, 116874.0, 172460.0, 56911.0, 3730.0, 153613.0, 73688.0, 13714.0, 35583.0, 88412.0, 128540.0, 74647.0, 50427.0, 57083.0, 25105.0, 39845.0, 23427.0, 77796.0, 23583.0, 3290.0, 52722.0, 0.0, 0.0, 0.0, 116625.0, 146731.0, 117911.0, 193117.0, 5688.0, 56614.0, 179875.0, 1231.0, 88961.0, 16926.0, 98215.0, 104274.0, 194898.0, 29417.0, 180526.0, 36575.0, 36390.0, 181264.0, 62916.0, 64698.0, 264386.0, 257529.0, 216416.0, 155844.0, 12775.0, 151158.0, 162270.0, 133530.0, 2350.0, 9163.0, 19864.0, 0.0, 0.0, 0.0, 0.0, 253085.0, 237498.0, 214296.0, 28456.0, 116387.0, 120039.0, 14990.0, 5768.0, 0.0, 0.0, 168595.0, 181344.0, 50739.0, 64753.0, 28239.0, 0.0, 100221.0, 12966.0, 0.0, 4106.0, 234766.0, 218874.0, 196423.0, 113995.0, 16083.0, 69853.0, 77129.0, 92962.0, 6430.0, 16338.0, 6061.0, 7387.0, 0.0, 0.0, 0.0, 218490.0, 220246.0, 199420.0, 74586.0, 88830.0, 136329.0, 42830.0, 27324.0, 1579.0, 0.0, 175270.0, 213423.0, 96912.0, 73200.0, 12199.0, 0.0, 121953.0, 68532.0, 5526.0, 0.0, 77362.0, 73530.0, 70428.0, 30176.0, 83698.0, 20134.0, 53931.0, 44990.0, 89268.0, 0.0, 8746.0, 25735.0, 0.0, 0.0, 0.0, 91206.0, 141371.0, 107720.0, 188122.0, 43518.0, 49830.0, 157949.0, 0.0, 55732.0, 0.0, 74375.0, 114520.0, 190426.0, 8775.0, 154896.0, 4155.0, 32081.0, 173417.0, 49772.0, 67243.0, 17280.0, 36954.0, 43907.0, 12186.0, 5035.0, 11566.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 46938.0, 68953.0, 45507.0, 12660.0, 2810.0, 16141.0, 0.0, 0.0, 0.0, 0.0, 10730.0, 51765.0, 4119.0, 310.0, 0.0, 0.0, 9996.0, 7826.0, 0.0, 2292.0, 4302.0, 25864.0, 7241.0, 4967.0, 35152.0, 0.0, 1099.0, 4983.0, 8672.0, 0.0, 2621.0, 7833.0, 0.0, 0.0, 0.0, 46345.0, 71894.0, 12449.0, 146093.0, 8340.0, 5306.0, 74538.0, 0.0, 11531.0, 0.0, 17027.0, 18569.0, 115619.0, 0.0, 24510.0, 0.0, 6833.0, 50189.0, 13418.0, 1449.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 3172.0, 0.0, 7748.0, 0.0, 2328.0, 16425.0, 0.0, 0.0, 0.0, 0.0, 0.0, 9838.0, 0.0, 0.0, 0.0, 2007.0, 10432.0, 0.0, 0.0, 279411.0, 264957.0, 240785.0, 182909.0, 4139.0, 250589.0, 241966.0, 164706.0, 19733.0, 43652.0, 105057.0, 4259.0, 0.0, 13580.0, 0.0, 266948.0, 258852.0, 229773.0, 65521.0, 230997.0, 200900.0, 40805.0, 58968.0, 1197.0, 0.0, 258660.0, 232585.0, 96180.0, 128038.0, 15222.0, 6352.0, 138933.0, 48601.0, 0.0, 14899.0, 260277.0, 265966.0, 232058.0, 174629.0, 12314.0, 171724.0, 205865.0, 185104.0, 16835.0, 14882.0, 68384.0, 0.0, 0.0, 0.0, 0.0, 255664.0, 247976.0, 214442.0, 94043.0, 167139.0, 208718.0, 87770.0, 44011.0, 18485.0, 0.0, 226205.0, 222593.0, 108979.0, 135591.0, 64025.0, 12258.0, 137052.0, 93911.0, 15505.0, 15675.0, 108760.0, 112458.0, 89787.0, 62227.0, 141441.0, 56658.0, 101913.0, 35089.0, 135237.0, 3653.0, 7715.0, 56350.0, 0.0, 4930.0, 0.0, 116805.0, 162019.0, 118510.0, 193591.0, 75068.0, 84369.0, 196032.0, 13466.0, 97869.0, 0.0, 84855.0, 130901.0, 202609.0, 18066.0, 192710.0, 27678.0, 56509.0, 189969.0, 77467.0, 62854.0, 167358.0, 165544.0, 148640.0, 102726.0, 2029.0, 29849.0, 54127.0, 41976.0, 12507.0, 0.0, 10593.0, 7563.0, 0.0, 0.0, 0.0, 189283.0, 183388.0, 153627.0, 31950.0, 45027.0, 78984.0, 9262.0, 23690.0, 0.0, 0.0, 66435.0, 105353.0, 19780.0, 57971.0, 4987.0, 0.0, 103395.0, 35349.0, 281.0, 1411.0, 80243.0, 90373.0, 105484.0, 39816.0, 119452.0, 14541.0, 32421.0, 24825.0, 106182.0, 0.0, 2340.0, 57872.0, 0.0, 0.0, 0.0, 74994.0, 124956.0, 93329.0, 181216.0, 16838.0, 29414.0, 173374.0, 0.0, 46655.0, 0.0, 89770.0, 97539.0, 186980.0, 4337.0, 130841.0, 12449.0, 22038.0, 139688.0, 43875.0, 49635.0, 2960.0, 3519.0, 9682.0, 3019.0, 35189.0, 0.0, 4524.0, 0.0, 14569.0, 0.0, 0.0, 16614.0, 0.0, 0.0, 0.0, 14872.0, 24640.0, 12676.0, 104031.0, 0.0, 0.0, 4359.0, 0.0, 0.0, 0.0, 10637.0, 4953.0, 76197.0, 0.0, 29871.0, 0.0, 0.0, 14500.0, 0.0, 0.0, 240989.0, 215166.0, 147278.0, 91062.0, 9399.0, 88503.0, 94342.0, 91199.0, 8377.0, 2208.0, 21179.0, 0.0, 0.0, 0.0, 0.0, 193404.0, 212015.0, 171026.0, 55324.0, 72529.0, 154723.0, 18497.0, 1161.0, 7228.0, 0.0, 134430.0, 177905.0, 41828.0, 42606.0, 4803.0, 0.0, 42259.0, 24216.0, 0.0, 0.0, 68976.0, 101265.0, 81689.0, 18938.0, 88315.0, 20061.0, 32108.0, 21161.0, 86679.0, 0.0, 0.0, 44699.0, 0.0, 0.0, 0.0, 85044.0, 127753.0, 94258.0, 163084.0, 7106.0, 51715.0, 151669.0, 5854.0, 8737.0, 0.0, 61367.0, 55314.0, 186901.0, 23268.0, 118887.0, 21208.0, 41814.0, 108900.0, 38518.0, 50538.0, 0.0, 21318.0, 27607.0, 9937.0, 74448.0, 11156.0, 4296.0, 5319.0, 46520.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 32092.0, 18747.0, 37563.0, 86499.0, 2685.0, 10936.0, 59703.0, 0.0, 12354.0, 0.0, 25516.0, 15574.0, 66612.0, 6262.0, 53104.0, 10178.0, 0.0, 16198.0, 0.0, 0.0, 9449.0, 26989.0, 14544.0, 0.0, 12826.0, 0.0, 0.0, 0.0, 17213.0, 0.0, 0.0, 5544.0, 0.0, 0.0, 0.0, 5782.0, 22014.0, 11344.0, 43310.0, 7231.0, 3293.0, 23933.0, 2220.0, 1880.0, 0.0, 2412.0, 13349.0, 51941.0, 0.0, 0.0, 0.0, 0.0, 0.0, 20044.0, 0.0  },
                                                {274740.0, 252700.0, 245719.0, 95551.0, 0.0, 259397.0, 229285.0, 45461.0, 0.0, 34323.0, 12093.0, 6451.0, 0.0, 0.0, 0.0, 268990.0, 246840.0, 166601.0, 13318.0, 207683.0, 109116.0, 4737.0, 11160.0, 3476.0, 0.0, 249668.0, 193171.0, 12053.0, 69435.0, 10703.0, 0.0, 73877.0, 9930.0, 0.0, 0.0, 249730.0, 246856.0, 213611.0, 107535.0, 1638.0, 203100.0, 197659.0, 115732.0, 2971.0, 48739.0, 37226.0, 1654.0, 0.0, 0.0, 0.0, 255774.0, 222491.0, 174534.0, 26949.0, 188271.0, 144320.0, 18392.0, 55578.0, 0.0, 0.0, 224256.0, 175893.0, 23106.0, 77127.0, 17199.0, 0.0, 108237.0, 52058.0, 0.0, 1573.0, 195324.0, 191876.0, 119567.0, 68346.0, 3274.0, 63244.0, 103681.0, 51744.0, 13187.0, 0.0, 24312.0, 0.0, 0.0, 0.0, 0.0, 173491.0, 197102.0, 90882.0, 11938.0, 60251.0, 86255.0, 5961.0, 5971.0, 0.0, 0.0, 127302.0, 113281.0, 31930.0, 17198.0, 7531.0, 0.0, 51156.0, 24677.0, 809.0, 0.0, 53968.0, 71407.0, 30205.0, 6031.0, 0.0, 14149.0, 14795.0, 3357.0, 673.0, 0.0, 0.0, 1431.0, 0.0, 0.0, 0.0, 64392.0, 75728.0, 37181.0, 7905.0, 8883.0, 15363.0, 7341.0, 0.0, 0.0, 0.0, 18885.0, 32085.0, 7199.0, 4242.0, 2532.0, 0.0, 8432.0, 904.0, 0.0, 7824.0, 16004.0, 2289.0, 2087.0, 1953.0, 0.0, 0.0, 3689.0, 613.0, 19171.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 14276.0, 5356.0, 10214.0, 26967.0, 1511.0, 6894.0, 32594.0, 0.0, 0.0, 0.0, 5443.0, 10118.0, 41583.0, 0.0, 17107.0, 0.0, 0.0, 14954.0, 9342.0, 12460.0, 231067.0, 205318.0, 126039.0, 8389.0, 0.0, 70028.0, 75789.0, 8226.0, 2722.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 187328.0, 183312.0, 74640.0, 1585.0, 30321.0, 27229.0, 0.0, 0.0, 0.0, 0.0, 108822.0, 67044.0, 28341.0, 13727.0, 0.0, 0.0, 13700.0, 3874.0, 0.0, 0.0, 120342.0, 147170.0, 77589.0, 57146.0, 3096.0, 26573.0, 31405.0, 18586.0, 5799.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 84015.0, 131196.0, 72198.0, 8625.0, 18508.0, 46880.0, 3551.0, 0.0, 0.0, 0.0, 62609.0, 78740.0, 30660.0, 0.0, 0.0, 0.0, 17618.0, 10516.0, 0.0, 0.0, 40433.0, 46697.0, 28873.0, 7148.0, 0.0, 4054.0, 17721.0, 4696.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 59858.0, 36532.0, 46895.0, 7542.0, 4275.0, 8033.0, 771.0, 0.0, 0.0, 0.0, 11614.0, 41586.0, 13805.0, 2153.0, 0.0, 0.0, 3892.0, 2586.0, 3758.0, 0.0, 7437.0, 19366.0, 10671.0, 1315.0, 3560.0, 2287.0, 0.0, 0.0, 6217.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 12181.0, 6860.0, 0.0, 16982.0, 0.0, 0.0, 27962.0, 0.0, 738.0, 0.0, 1959.0, 8581.0, 34114.0, 0.0, 34045.0, 0.0, 5441.0, 37421.0, 13573.0, 35390.0, 0.0, 13256.0, 5124.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 11561.0, 6208.0, 6660.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1694.0, 3778.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 7296.0, 5264.0, 1371.0, 3016.0, 0.0, 5230.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 8718.0, 4433.0, 4499.0, 1707.0, 3956.0, 0.0, 0.0, 0.0, 0.0, 0.0, 10931.0, 4708.0, 0.0, 0.0, 0.0, 0.0, 0.0, 4171.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 688.0, 499.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 8566.0, 0.0, 0.0, 0.0, 0.0, 2432.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 7883.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 8052.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 264777.0, 244552.0, 223469.0, 123277.0, 12052.0, 200296.0, 186831.0, 82310.0, 8106.0, 11129.0, 25651.0, 1772.0, 0.0, 0.0, 0.0, 263664.0, 239525.0, 171526.0, 15982.0, 206592.0, 105565.0, 9318.0, 0.0, 0.0, 0.0, 227840.0, 168431.0, 35222.0, 65340.0, 11209.0, 0.0, 62629.0, 6551.0, 0.0, 0.0, 246018.0, 238997.0, 209043.0, 135672.0, 13787.0, 137033.0, 132038.0, 115780.0, 1044.0, 35056.0, 27109.0, 2318.0, 0.0, 0.0, 0.0, 239203.0, 235155.0, 159574.0, 29586.0, 150371.0, 130009.0, 30864.0, 14049.0, 1279.0, 0.0, 196584.0, 167129.0, 65580.0, 72304.0, 6381.0, 0.0, 95640.0, 24509.0, 0.0, 0.0, 118720.0, 137420.0, 108192.0, 67301.0, 2293.0, 41484.0, 50882.0, 42709.0, 15618.0, 2077.0, 2797.0, 0.0, 0.0, 0.0, 0.0, 142912.0, 168161.0, 116617.0, 30703.0, 31484.0, 72000.0, 6134.0, 3767.0, 1202.0, 0.0, 117412.0, 135696.0, 14516.0, 14432.0, 11628.0, 0.0, 59853.0, 29130.0, 6934.0, 7185.0, 21719.0, 21696.0, 29081.0, 2230.0, 42273.0, 20644.0, 20270.0, 1568.0, 29769.0, 0.0, 0.0, 10397.0, 0.0, 1129.0, 0.0, 29672.0, 31877.0, 29615.0, 79063.0, 18469.0, 10038.0, 56880.0, 0.0, 23684.0, 0.0, 39660.0, 40352.0, 102366.0, 3904.0, 66517.0, 0.0, 387.0, 40310.0, 17879.0, 18596.0, 116924.0, 113174.0, 70510.0, 33598.0, 2827.0, 21529.0, 23907.0, 0.0, 471.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 116759.0, 134461.0, 72329.0, 8468.0, 16090.0, 44221.0, 9363.0, 0.0, 0.0, 0.0, 39232.0, 57858.0, 7180.0, 24624.0, 0.0, 0.0, 0.0, 1918.0, 0.0, 0.0, 70717.0, 90812.0, 72230.0, 38695.0, 3329.0, 17644.0, 9766.0, 32536.0, 13348.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 90692.0, 68245.0, 89053.0, 8142.0, 6588.0, 41465.0, 4726.0, 0.0, 0.0, 0.0, 27133.0, 74355.0, 7866.0, 32541.0, 7310.0, 0.0, 29974.0, 4435.0, 0.0, 3797.0, 6073.0, 24760.0, 29991.0, 12775.0, 29096.0, 24422.0, 0.0, 1108.0, 17987.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 20032.0, 15720.0, 18109.0, 71196.0, 0.0, 1970.0, 20491.0, 0.0, 3919.0, 0.0, 17940.0, 5726.0, 83639.0, 3035.0, 27323.0, 0.0, 6240.0, 36572.0, 27376.0, 12956.0, 7426.0, 1490.0, 11244.0, 0.0, 0.0, 2079.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 4610.0, 1698.0, 4263.0, 0.0, 0.0, 4997.0, 0.0, 0.0, 0.0, 0.0, 0.0, 8766.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 2774.0, 0.0, 5681.0, 0.0, 1123.0, 0.0, 1214.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 9145.0, 0.0, 3845.0, 2586.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 3501.0, 0.0, 15872.0, 0.0, 3038.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 218929.0, 229049.0, 165269.0, 95070.0, 4799.0, 55360.0, 110110.0, 57492.0, 2744.0, 0.0, 625.0, 950.0, 0.0, 0.0, 0.0, 210492.0, 197928.0, 150587.0, 11748.0, 97599.0, 74914.0, 21393.0, 8637.0, 1816.0, 0.0, 156947.0, 111966.0, 31852.0, 31252.0, 9754.0, 0.0, 67888.0, 24535.0, 1068.0, 2715.0, 146640.0, 169256.0, 115746.0, 69448.0, 3675.0, 41240.0, 28808.0, 53800.0, 3560.0, 0.0, 3360.0, 923.0, 0.0, 0.0, 0.0, 160288.0, 154931.0, 133836.0, 10628.0, 31996.0, 97703.0, 8976.0, 4166.0, 0.0, 0.0, 101086.0, 137918.0, 37697.0, 68010.0, 15726.0, 0.0, 54369.0, 3149.0, 0.0, 0.0, 7852.0, 33531.0, 21842.0, 14199.0, 35736.0, 457.0, 8283.0, 6710.0, 45916.0, 0.0, 0.0, 17934.0, 0.0, 0.0, 0.0, 27699.0, 43956.0, 43107.0, 81359.0, 13994.0, 9556.0, 80658.0, 0.0, 19239.0, 0.0, 39954.0, 36417.0, 82563.0, 2291.0, 75586.0, 7477.0, 13163.0, 68054.0, 4237.0, 29693.0, 35005.0, 49656.0, 28743.0, 8892.0, 9153.0, 0.0, 0.0, 12618.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 26365.0, 48280.0, 23186.0, 4670.0, 11797.0, 1344.0, 1881.0, 0.0, 0.0, 0.0, 8207.0, 19124.0, 6681.0, 0.0, 0.0, 0.0, 1913.0, 0.0, 0.0, 0.0, 0.0, 5954.0, 13579.0, 2383.0, 31611.0, 2527.0, 0.0, 3576.0, 7744.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 13904.0, 27698.0, 18584.0, 40145.0, 0.0, 2863.0, 13105.0, 0.0, 0.0, 0.0, 5036.0, 6686.0, 74923.0, 0.0, 30423.0, 0.0, 2148.0, 40455.0, 16788.0, 6895.0, 0.0, 0.0, 0.0, 0.0, 8912.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 7139.0, 0.0, 2573.0, 15652.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 16503.0, 0.0, 2191.0, 0.0, 0.0, 0.0, 0.0, 0.0, 72973.0, 50012.0, 46492.0, 36269.0, 0.0, 17278.0, 5337.0, 11258.0, 1470.0, 0.0, 10341.0, 0.0, 0.0, 0.0, 0.0, 62401.0, 54692.0, 52992.0, 3751.0, 17297.0, 56297.0, 7943.0, 1241.0, 3152.0, 0.0, 31829.0, 44071.0, 4173.0, 18702.0, 0.0, 0.0, 29548.0, 0.0, 0.0, 1281.0, 6652.0, 28800.0, 24187.0, 6636.0, 18397.0, 3515.0, 7793.0, 1185.0, 12384.0, 0.0, 1135.0, 8073.0, 0.0, 0.0, 0.0, 21697.0, 20936.0, 30805.0, 51325.0, 4038.0, 8196.0, 18905.0, 0.0, 17624.0, 0.0, 39200.0, 11291.0, 79807.0, 2217.0, 28276.0, 0.0, 2223.0, 34249.0, 7398.0, 22580.0, 0.0, 0.0, 13311.0, 9118.0, 11019.0, 0.0, 0.0, 0.0, 17994.0, 0.0, 0.0, 724.0, 0.0, 0.0, 0.0, 4681.0, 0.0, 4460.0, 16254.0, 0.0, 0.0, 12305.0, 0.0, 5931.0, 0.0, 5051.0, 0.0, 24038.0, 0.0, 0.0, 0.0, 0.0, 3948.0, 0.0, 0.0, 0.0, 0.0, 0.0, 866.0, 13085.0, 2049.0, 2909.0, 1570.0, 19185.0, 0.0, 0.0, 11545.0, 0.0, 0.0, 0.0, 0.0, 4926.0, 771.0, 24989.0, 3110.0, 796.0, 7252.0, 0.0, 0.0, 0.0, 3083.0, 13099.0, 2555.0, 0.0, 4545.0, 0.0, 0.0, 589.0, 0.0, 0.0  },
                                                {249342.0, 248233.0, 231607.0, 157687.0, 3598.0, 252266.0, 236742.0, 157639.0, 10975.0, 155782.0, 90330.0, 7616.0, 2506.0, 0.0, 0.0, 249944.0, 234218.0, 206676.0, 21705.0, 225430.0, 181585.0, 44196.0, 70371.0, 11403.0, 0.0, 240955.0, 218251.0, 37454.0, 165319.0, 30192.0, 8050.0, 182874.0, 36734.0, 2738.0, 14401.0, 253112.0, 228031.0, 200753.0, 163771.0, 11835.0, 238265.0, 215478.0, 155873.0, 25260.0, 105458.0, 134261.0, 558.0, 10973.0, 0.0, 0.0, 246244.0, 237456.0, 186807.0, 47053.0, 222386.0, 187998.0, 51960.0, 125540.0, 8240.0, 0.0, 226195.0, 204824.0, 76407.0, 171566.0, 62287.0, 1340.0, 166386.0, 49638.0, 9285.0, 26822.0, 240256.0, 230146.0, 186979.0, 96770.0, 15191.0, 163837.0, 168189.0, 119858.0, 13943.0, 61078.0, 66948.0, 10751.0, 10784.0, 0.0, 0.0, 213340.0, 211870.0, 170231.0, 46539.0, 160526.0, 153593.0, 56289.0, 61060.0, 14158.0, 0.0, 185691.0, 163692.0, 63665.0, 142029.0, 46442.0, 0.0, 137020.0, 39989.0, 0.0, 12758.0, 112216.0, 122314.0, 99166.0, 43359.0, 0.0, 68118.0, 65843.0, 16704.0, 2266.0, 3391.0, 5803.0, 1909.0, 5240.0, 0.0, 0.0, 121457.0, 136759.0, 77479.0, 18490.0, 69088.0, 58646.0, 15074.0, 2214.0, 3242.0, 0.0, 86010.0, 100550.0, 23419.0, 17390.0, 50752.0, 1581.0, 16354.0, 12396.0, 6191.0, 724.0, 15491.0, 24901.0, 10884.0, 6774.0, 12052.0, 5565.0, 8042.0, 6174.0, 27540.0, 2243.0, 0.0, 3748.0, 0.0, 2106.0, 0.0, 29103.0, 26882.0, 18953.0, 50837.0, 8111.0, 19696.0, 70222.0, 0.0, 10370.0, 6874.0, 11017.0, 35641.0, 112811.0, 22788.0, 77402.0, 9522.0, 6079.0, 75097.0, 62131.0, 27795.0, 244168.0, 236703.0, 193431.0, 92689.0, 7306.0, 189127.0, 122883.0, 89484.0, 13219.0, 51034.0, 45030.0, 0.0, 0.0, 0.0, 0.0, 239078.0, 205937.0, 165344.0, 14158.0, 136101.0, 140020.0, 17862.0, 4220.0, 3675.0, 0.0, 194752.0, 152498.0, 33720.0, 111841.0, 17878.0, 6712.0, 96544.0, 5662.0, 12738.0, 14658.0, 226467.0, 208502.0, 175577.0, 63648.0, 7388.0, 142385.0, 108164.0, 102824.0, 3069.0, 5652.0, 21128.0, 0.0, 0.0, 0.0, 0.0, 192926.0, 197615.0, 145910.0, 25135.0, 123481.0, 133080.0, 18689.0, 24083.0, 8250.0, 0.0, 152806.0, 147439.0, 32948.0, 85026.0, 17387.0, 0.0, 78892.0, 33853.0, 11644.0, 2377.0, 98727.0, 114962.0, 81322.0, 38867.0, 3117.0, 47879.0, 40593.0, 27391.0, 9882.0, 0.0, 0.0, 1043.0, 0.0, 0.0, 0.0, 116816.0, 133255.0, 101363.0, 20112.0, 42535.0, 47310.0, 26164.0, 0.0, 1055.0, 0.0, 106634.0, 75714.0, 39084.0, 13765.0, 12435.0, 1262.0, 50546.0, 16812.0, 2333.0, 11139.0, 8844.0, 47913.0, 18854.0, 11797.0, 16135.0, 10788.0, 19262.0, 7123.0, 27029.0, 0.0, 0.0, 12082.0, 0.0, 0.0, 0.0, 61490.0, 31172.0, 24218.0, 48377.0, 18102.0, 23320.0, 68259.0, 891.0, 9448.0, 0.0, 32706.0, 19719.0, 79234.0, 8139.0, 81793.0, 10479.0, 8091.0, 65979.0, 23071.0, 32320.0, 29731.0, 79380.0, 49890.0, 9992.0, 1992.0, 6335.0, 3768.0, 833.0, 6473.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 25032.0, 64086.0, 41286.0, 0.0, 9573.0, 15960.0, 1676.0, 0.0, 0.0, 0.0, 38247.0, 14635.0, 4498.0, 0.0, 726.0, 0.0, 11049.0, 7218.0, 0.0, 0.0, 12734.0, 48276.0, 7445.0, 17936.0, 2002.0, 6438.0, 10476.0, 10815.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 47925.0, 69357.0, 34895.0, 13034.0, 0.0, 495.0, 0.0, 0.0, 0.0, 0.0, 16768.0, 26935.0, 14193.0, 3963.0, 0.0, 0.0, 7398.0, 4180.0, 0.0, 0.0, 19753.0, 18848.0, 729.0, 0.0, 11334.0, 9057.0, 1246.0, 4891.0, 20882.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 33794.0, 23797.0, 3084.0, 50170.0, 6380.0, 1301.0, 16021.0, 0.0, 0.0, 0.0, 6908.0, 5317.0, 73385.0, 3638.0, 37048.0, 0.0, 0.0, 57360.0, 12465.0, 10254.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 5050.0, 4360.0, 1098.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1355.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 2506.0, 6625.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 3465.0, 0.0, 25955.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 17878.0, 0.0, 6947.0, 0.0, 0.0, 27645.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 251825.0, 238700.0, 230311.0, 169950.0, 25660.0, 238566.0, 211384.0, 164049.0, 9481.0, 96623.0, 92040.0, 0.0, 16463.0, 0.0, 0.0, 250869.0, 238442.0, 198970.0, 41204.0, 217760.0, 182486.0, 43414.0, 94115.0, 9190.0, 0.0, 236449.0, 203882.0, 61898.0, 169711.0, 46743.0, 827.0, 164719.0, 54945.0, 5876.0, 16259.0, 251589.0, 242701.0, 209286.0, 151290.0, 21009.0, 223022.0, 191727.0, 142206.0, 31705.0, 74377.0, 88227.0, 16193.0, 1614.0, 850.0, 0.0, 235115.0, 220892.0, 178099.0, 76708.0, 206814.0, 174360.0, 69183.0, 90536.0, 8816.0, 1778.0, 212901.0, 174193.0, 106663.0, 149445.0, 53463.0, 5303.0, 157134.0, 87110.0, 15866.0, 10492.0, 218638.0, 185388.0, 152090.0, 79961.0, 3294.0, 146242.0, 136546.0, 109596.0, 23617.0, 11229.0, 23507.0, 4584.0, 9677.0, 0.0, 0.0, 204983.0, 181673.0, 144241.0, 62686.0, 128736.0, 142774.0, 36889.0, 18306.0, 21312.0, 0.0, 159344.0, 161178.0, 88479.0, 119782.0, 63545.0, 9356.0, 126003.0, 51487.0, 15525.0, 31835.0, 29410.0, 66466.0, 61056.0, 16115.0, 56638.0, 19621.0, 46172.0, 28643.0, 79501.0, 4858.0, 10867.0, 38398.0, 0.0, 0.0, 0.0, 100200.0, 85113.0, 70194.0, 131704.0, 45659.0, 56424.0, 121728.0, 8650.0, 94461.0, 3695.0, 76877.0, 83074.0, 140608.0, 19658.0, 131326.0, 24006.0, 27368.0, 130151.0, 83322.0, 37505.0, 210196.0, 212543.0, 173202.0, 100378.0, 11982.0, 121969.0, 127786.0, 102927.0, 3735.0, 21671.0, 6222.0, 0.0, 0.0, 0.0, 0.0, 216895.0, 192020.0, 150785.0, 14128.0, 92591.0, 122860.0, 17554.0, 12684.0, 2599.0, 0.0, 165932.0, 158105.0, 64830.0, 37749.0, 30955.0, 0.0, 96664.0, 21104.0, 11060.0, 2198.0, 170127.0, 160667.0, 139245.0, 77733.0, 13828.0, 63668.0, 72570.0, 76060.0, 5950.0, 10678.0, 11924.0, 1210.0, 0.0, 0.0, 0.0, 170863.0, 165481.0, 131772.0, 37221.0, 97535.0, 105750.0, 28597.0, 17866.0, 8646.0, 0.0, 133906.0, 139346.0, 65726.0, 72244.0, 40067.0, 1298.0, 74836.0, 43381.0, 6683.0, 5334.0, 47712.0, 66717.0, 54058.0, 45954.0, 88444.0, 24957.0, 39349.0, 11432.0, 96338.0, 0.0, 4791.0, 17886.0, 0.0, 0.0, 0.0, 73071.0, 96405.0, 66745.0, 126140.0, 33211.0, 32903.0, 135285.0, 0.0, 25866.0, 0.0, 37539.0, 83727.0, 130998.0, 16339.0, 128528.0, 14066.0, 28454.0, 127579.0, 75038.0, 60712.0, 22551.0, 48152.0, 19255.0, 9649.0, 9528.0, 1518.0, 8571.0, 3824.0, 0.0, 957.0, 0.0, 0.0, 0.0, 0.0, 0.0, 30112.0, 35050.0, 26140.0, 3240.0, 1319.0, 7507.0, 0.0, 3311.0, 0.0, 0.0, 14898.0, 19600.0, 17382.0, 0.0, 0.0, 0.0, 3294.0, 2006.0, 0.0, 0.0, 22880.0, 29702.0, 13321.0, 8083.0, 35888.0, 0.0, 0.0, 6980.0, 18158.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 5039.0, 24722.0, 19362.0, 89979.0, 2789.0, 3118.0, 83894.0, 0.0, 0.0, 0.0, 19754.0, 14717.0, 87528.0, 4033.0, 45638.0, 11584.0, 7619.0, 37666.0, 20878.0, 5943.0, 0.0, 0.0, 3222.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 16781.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 18991.0, 0.0, 0.0, 0.0, 10732.0, 0.0, 0.0, 0.0, 250409.0, 241437.0, 198253.0, 150448.0, 10482.0, 171996.0, 170828.0, 137444.0, 17838.0, 29809.0, 55279.0, 1341.0, 0.0, 1363.0, 0.0, 234394.0, 210595.0, 172500.0, 35577.0, 192295.0, 157820.0, 45612.0, 42774.0, 0.0, 3086.0, 195161.0, 161912.0, 70301.0, 124592.0, 39074.0, 10748.0, 108215.0, 79269.0, 23847.0, 5290.0, 204578.0, 197294.0, 169555.0, 118418.0, 26058.0, 151826.0, 128390.0, 121275.0, 25768.0, 48370.0, 43904.0, 12618.0, 0.0, 0.0, 0.0, 210515.0, 185088.0, 153994.0, 78412.0, 138186.0, 148696.0, 50407.0, 48710.0, 9879.0, 0.0, 170111.0, 156372.0, 83966.0, 114615.0, 57617.0, 509.0, 122878.0, 60026.0, 24338.0, 23028.0, 83145.0, 84034.0, 75459.0, 39565.0, 68605.0, 28384.0, 43343.0, 24103.0, 110652.0, 3990.0, 5150.0, 28400.0, 1931.0, 0.0, 0.0, 71906.0, 119492.0, 77600.0, 126344.0, 38296.0, 53198.0, 132729.0, 5111.0, 78831.0, 0.0, 66376.0, 93734.0, 138845.0, 40625.0, 141984.0, 27290.0, 28618.0, 137078.0, 69689.0, 70966.0, 99033.0, 121821.0, 93009.0, 41441.0, 11783.0, 71847.0, 33123.0, 41812.0, 5813.0, 5354.0, 0.0, 0.0, 0.0, 0.0, 0.0, 152243.0, 120720.0, 100849.0, 22690.0, 54371.0, 66258.0, 6266.0, 7382.0, 10625.0, 0.0, 87340.0, 100831.0, 56016.0, 10352.0, 2053.0, 1519.0, 41869.0, 42991.0, 0.0, 0.0, 46695.0, 77748.0, 69431.0, 34411.0, 98302.0, 15046.0, 25438.0, 5484.0, 72586.0, 0.0, 4341.0, 15377.0, 0.0, 0.0, 0.0, 71428.0, 95574.0, 70981.0, 124383.0, 13526.0, 57504.0, 136302.0, 0.0, 29368.0, 0.0, 44335.0, 76605.0, 139187.0, 10297.0, 127444.0, 7630.0, 7920.0, 123658.0, 42663.0, 41334.0, 5885.0, 5403.0, 7828.0, 5939.0, 30017.0, 0.0, 0.0, 4949.0, 19656.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 4204.0, 16349.0, 8763.0, 49367.0, 0.0, 0.0, 18156.0, 0.0, 0.0, 0.0, 4973.0, 13970.0, 50231.0, 0.0, 16341.0, 0.0, 2438.0, 22191.0, 8093.0, 3119.0, 172551.0, 172272.0, 121568.0, 74192.0, 11180.0, 63285.0, 86409.0, 74471.0, 13740.0, 12938.0, 22511.0, 2433.0, 0.0, 0.0, 0.0, 152152.0, 156077.0, 133424.0, 20872.0, 51727.0, 101361.0, 32658.0, 17789.0, 0.0, 0.0, 106469.0, 137320.0, 45246.0, 30827.0, 7788.0, 3681.0, 39701.0, 12341.0, 7222.0, 0.0, 36905.0, 67002.0, 45445.0, 3721.0, 78820.0, 13588.0, 30814.0, 24500.0, 93622.0, 0.0, 5722.0, 50105.0, 0.0, 3510.0, 0.0, 63693.0, 99960.0, 63695.0, 136136.0, 12658.0, 42136.0, 116654.0, 772.0, 66011.0, 0.0, 33052.0, 71215.0, 138467.0, 21675.0, 118093.0, 814.0, 23582.0, 115862.0, 15575.0, 33328.0, 8324.0, 37383.0, 9350.0, 1494.0, 17505.0, 901.0, 7481.0, 11735.0, 21018.0, 0.0, 0.0, 8770.0, 0.0, 0.0, 0.0, 28275.0, 33550.0, 8070.0, 89470.0, 4920.0, 863.0, 47578.0, 0.0, 14253.0, 0.0, 13627.0, 20350.0, 95529.0, 3584.0, 21255.0, 0.0, 0.0, 47448.0, 0.0, 2352.0, 17737.0, 19715.0, 15103.0, 1662.0, 19237.0, 8521.0, 13390.0, 0.0, 3180.0, 0.0, 7614.0, 21470.0, 0.0, 0.0, 0.0, 20112.0, 27098.0, 30368.0, 22613.0, 0.0, 5596.0, 40031.0, 0.0, 2484.0, 0.0, 4488.0, 25345.0, 57677.0, 2438.0, 48697.0, 0.0, 6035.0, 17732.0, 0.0, 0.0  } };
    
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
            printsvsv("ScoreTable[", tp, "] - zeros: ", zeros[tp]);
            //scoreTable[tp][state] = 0.0;//random();
        }
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
                            addr = makeAddress(3,2);
                            slot = API_GetTaskSlotFromTile(addr, app, task);
                            if (slot != ERRO) defined++;
                            else{
                                addr = makeAddress(4,3);
                                slot = API_GetTaskSlotFromTile(addr, app, task);
                                if (slot != ERRO) defined++;
                                else{
                                    addr = makeAddress(2,3);
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                                else{
                                    addr = makeAddress(3,4);
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                            }
                        }
                        else if( currentState[0][3] < objectiveState[0][3] ){
                            addr = makeAddress(2,2);
                            slot = API_GetTaskSlotFromTile(addr, app, task);
                            if (slot != ERRO) defined++;
                            else{
                                addr = makeAddress(4,2);
                                slot = API_GetTaskSlotFromTile(addr, app, task);
                                if (slot != ERRO) defined++;
                                else{
                                    addr = makeAddress(4,4);
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                                else{
                                    addr = makeAddress(2,4);
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                            }
                        }
                        else if( currentState[1][0] < objectiveState[1][0] ){
                            addr = makeAddress(8,2);
                            slot = API_GetTaskSlotFromTile(addr, app, task);
                            if (slot != ERRO) defined++;
                            else{
                                addr = makeAddress(9,3);
                                slot = API_GetTaskSlotFromTile(addr, app, task);
                                if (slot != ERRO) defined++;
                                else{
                                    addr = makeAddress(8,4);
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                                else{
                                    addr = makeAddress(7,3);
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                            }
                        }
                        else if( currentState[1][3] < objectiveState[1][3] ){
                            addr = makeAddress(7,2);
                            slot = API_GetTaskSlotFromTile(addr, app, task);
                            if (slot != ERRO) defined++;
                            else{
                                addr = makeAddress(9,2);
                                slot = API_GetTaskSlotFromTile(addr, app, task);
                                if (slot != ERRO) defined++;
                                else{
                                    addr = makeAddress(9,4);
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                                else{
                                    addr = makeAddress(7,4);
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                            }
                        }
                        else if( currentState[2][0] < objectiveState[2][0] ){
                            addr = makeAddress(5,7);
                            slot = API_GetTaskSlotFromTile(addr, app, task);
                            if (slot != ERRO) defined++;
                            else{
                                addr = makeAddress(6,8);
                                slot = API_GetTaskSlotFromTile(addr, app, task);
                                if (slot != ERRO) defined++;
                                else{
                                    addr = makeAddress(5,9);
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                                else{
                                    addr = makeAddress(4,8);
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                            }
                        }
                        else if( currentState[2][3] < objectiveState[2][3] ){
                            addr = makeAddress(4,7);
                            slot = API_GetTaskSlotFromTile(addr, app, task);
                            if (slot != ERRO) defined++;
                            else{
                                addr = makeAddress(6,7);
                                slot = API_GetTaskSlotFromTile(addr, app, task);
                                if (slot != ERRO) defined++;
                                else{
                                    addr = makeAddress(6,9);
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                                else{
                                    addr = makeAddress(4,9);
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                            }
                        }
                    }

                    else if (applications[app].taskType[task] == 1){
                        if( currentState[0][1] < objectiveState[0][1] ){
                            addr = makeAddress(3,2);
                            slot = API_GetTaskSlotFromTile(addr, app, task);
                            if (slot != ERRO) defined++;
                            else{
                                addr = makeAddress(4,3);
                                slot = API_GetTaskSlotFromTile(addr, app, task);
                                if (slot != ERRO) defined++;
                                else{
                                    addr = makeAddress(2,3);
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                                else{
                                    addr = makeAddress(3,4);
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                            }
                        }
                        else if( currentState[0][4] < objectiveState[0][4] ){
                            addr = makeAddress(2,2);
                            slot = API_GetTaskSlotFromTile(addr, app, task);
                            if (slot != ERRO) defined++;
                            else{
                                addr = makeAddress(4,2);
                                slot = API_GetTaskSlotFromTile(addr, app, task);
                                if (slot != ERRO) defined++;
                                else{
                                    addr = makeAddress(4,4);
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                                else{
                                    addr = makeAddress(2,4);
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                            }
                        }
                        else if( currentState[1][1] < objectiveState[1][1] ){
                            addr = makeAddress(8,2);
                            slot = API_GetTaskSlotFromTile(addr, app, task);
                            if (slot != ERRO) defined++;
                            else{
                                addr = makeAddress(9,3);
                                slot = API_GetTaskSlotFromTile(addr, app, task);
                                if (slot != ERRO) defined++;
                                else{
                                    addr = makeAddress(8,4);
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                                else{
                                    addr = makeAddress(7,3);
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                            }
                        }
                        else if( currentState[1][4] < objectiveState[1][4] ){
                            addr = makeAddress(7,2);
                            slot = API_GetTaskSlotFromTile(addr, app, task);
                            if (slot != ERRO) defined++;
                            else{
                                addr = makeAddress(9,2);
                                slot = API_GetTaskSlotFromTile(addr, app, task);
                                if (slot != ERRO) defined++;
                                else{
                                    addr = makeAddress(9,4);
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                                else{
                                    addr = makeAddress(7,4);
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                            }
                        }
                        else if( currentState[2][1] < objectiveState[2][1] ){
                            addr = makeAddress(5,7);
                            slot = API_GetTaskSlotFromTile(addr, app, task);
                            if (slot != ERRO) defined++;
                            else{
                                addr = makeAddress(6,8);
                                slot = API_GetTaskSlotFromTile(addr, app, task);
                                if (slot != ERRO) defined++;
                                else{
                                    addr = makeAddress(5,9);
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                                else{
                                    addr = makeAddress(4,8);
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                            }
                        }
                        else if( currentState[2][4] < objectiveState[2][4] ){
                            addr = makeAddress(4,7);
                            slot = API_GetTaskSlotFromTile(addr, app, task);
                            if (slot != ERRO) defined++;
                            else{
                                addr = makeAddress(6,7);
                                slot = API_GetTaskSlotFromTile(addr, app, task);
                                if (slot != ERRO) defined++;
                                else{
                                    addr = makeAddress(6,9);
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                                else{
                                    addr = makeAddress(4,9);
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                            }
                        }
                    }

                    else if (applications[app].taskType[task] == 2){
                        if( currentState[0][2] < objectiveState[0][2] ){
                            addr = makeAddress(3,2);
                            slot = API_GetTaskSlotFromTile(addr, app, task);
                            if (slot != ERRO) defined++;
                            else{
                                addr = makeAddress(4,3);
                                slot = API_GetTaskSlotFromTile(addr, app, task);
                                if (slot != ERRO) defined++;
                                else{
                                    addr = makeAddress(2,3);
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                                else{
                                    addr = makeAddress(3,4);
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                            }
                        }
                        else if( currentState[0][5] < objectiveState[0][5] ){
                            addr = makeAddress(2,2);
                            slot = API_GetTaskSlotFromTile(addr, app, task);
                            if (slot != ERRO) defined++;
                            else{
                                addr = makeAddress(4,2);
                                slot = API_GetTaskSlotFromTile(addr, app, task);
                                if (slot != ERRO) defined++;
                                else{
                                    addr = makeAddress(4,4);
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                                else{
                                    addr = makeAddress(2,4);
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                            }
                        }
                        else if( currentState[1][2] < objectiveState[1][2] ){
                            addr = makeAddress(8,2);
                            slot = API_GetTaskSlotFromTile(addr, app, task);
                            if (slot != ERRO) defined++;
                            else{
                                addr = makeAddress(9,3);
                                slot = API_GetTaskSlotFromTile(addr, app, task);
                                if (slot != ERRO) defined++;
                                else{
                                    addr = makeAddress(8,4);
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                                else{
                                    addr = makeAddress(7,3);
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                            }
                        }
                        else if( currentState[1][5] < objectiveState[1][5] ){
                            addr = makeAddress(7,2);
                            slot = API_GetTaskSlotFromTile(addr, app, task);
                            if (slot != ERRO) defined++;
                            else{
                                addr = makeAddress(9,2);
                                slot = API_GetTaskSlotFromTile(addr, app, task);
                                if (slot != ERRO) defined++;
                                else{
                                    addr = makeAddress(9,4);
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                                else{
                                    addr = makeAddress(7,4);
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                            }
                        }
                        else if( currentState[2][2] < objectiveState[2][2] ){
                            addr = makeAddress(5,7);
                            slot = API_GetTaskSlotFromTile(addr, app, task);
                            if (slot != ERRO) defined++;
                            else{
                                addr = makeAddress(6,8);
                                slot = API_GetTaskSlotFromTile(addr, app, task);
                                if (slot != ERRO) defined++;
                                else{
                                    addr = makeAddress(5,9);
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                                else{
                                    addr = makeAddress(4,8);
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                            }
                        }
                        else if( currentState[2][5] < objectiveState[2][5] ){
                            addr = makeAddress(4,7);
                            slot = API_GetTaskSlotFromTile(addr, app, task);
                            if (slot != ERRO) defined++;
                            else{
                                addr = makeAddress(6,7);
                                slot = API_GetTaskSlotFromTile(addr, app, task);
                                if (slot != ERRO) defined++;
                                else{
                                    addr = makeAddress(6,9);
                                    slot = API_GetTaskSlotFromTile(addr, app, task);
                                }
                                if (slot != ERRO) defined++;
                                else{
                                    addr = makeAddress(4,9);
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

