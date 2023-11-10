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
#define THRESHOLD_TEMP 70

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
		xTaskCreate( GlobalManagerTask, "GlobalMaster", 1024*8, NULL, (tskIDLE_PRIORITY + 1), NULL );
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
                                             incommingPacket.task_migration_addr,
                                             incommingPacket.id);
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
                    printsvsv( "app_id", incommingPacket.app_id, "taskid: ", incommingPacket.task_id);
                    //HW_set_32bit_reg(NI_RX, DONE);
                    prints("-5NI_RX DONE!\n");
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
                    printsv("TTT01: ", xTaskGetTickCount());
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
                                             incommingPacket.task_migration_addr,
                                             incommingPacket.id);
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

                    //printsv("TTT03: ", xTaskGetTickCount());
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
                    //API_setFreqScale(1000); // ???
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

        // generate a new pattern
        if((tick % 200) == 0){
            GeneratePatternMatrix();
        }

		// Checks if there is some task to allocate...
		API_AllocateTasks(tick, (random()%(DIM_X*DIM_Y)));
		
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
		API_AllocateTasks(tick, 0);

        // Checks if there is some task to start...
		API_StartTasks();

        // Check migration
#if MIGRATION_AVAIL
        if( !API_CheckTaskToAllocate(xTaskGetTickCount()) ){
            if( migtick+200 <= xTaskGetTickCount() ){
                migrate = API_SelectTask_Migration_Temperature(THRESHOLD_TEMP);
                if (migrate != -1){
                    vTaskEnterCritical();
                    printsvsv("Got a app to migrate: ",(migrate>>16)," task: ", (migrate & 0x0000FFFF));
                    vTaskExitCritical();
                    mig_app = migrate >> 16;
                    mig_task = migrate & 0x0000FFFF;
                    for(k=0;k<(DIM_X*DIM_Y)-1;k++){
                        mig_addr = getNextPriorityAddr(0);
                        printsv("mig_addr = ", mig_addr);
                        mig_slot = API_GetTaskSlotFromTile(mig_addr, mig_app, mig_task);
                        if (mig_slot != ERRO){
                            printsvsv("Migrating it to ", getXpos(mig_addr), "- ", getYpos(mig_addr));
                            API_StartMigration(mig_app, mig_task, mig_addr);
                            migtick = xTaskGetTickCount();
                            break;
                        }   
                    }
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

    float scoreTable[N_TASKTYPE][N_STATES] = {  {200930.0,163299.0,153924.0,152032.0,143144.0,187431.0,158990.0,155148.0,148787.0,175687.0,156715.0,149745.0,163511.0,152707.0,160510.0,192880.0,181290.0,152365.0,158228.0,179293.0,158255.0,156198.0,167618.0,156642.0,157622.0,183195.0,168738.0,158914.0,171140.0,157763.0,158392.0,172734.0,161355.0,161485.0,166068.0},
                                                {174861.0,155917.0,138803.0,133480.0,104562.0,161708.0,144738.0,136503.0,115055.0,151534.0,143261.0,111261.0,129130.0,106437.0,128365.0,165636.0,149066.0,138105.0,137787.0,155057.0,143327.0,138680.0,145320.0,131497.0,121202.0,155935.0,146036.0,143500.0,147702.0,142729.0,124546.0,149739.0,143651.0,129650.0,136029.0},
                                                {149001.0,122248.0,113373.0,107431.0,55673.0,127096.0,112741.0,109463.0,71924.0,120896.0,111614.0,63548.0,92456.0,55068.0,81858.0,136050.0,117441.0,112227.0,70524.0,118014.0,111816.0,82125.0,112042.0,69351.0,58755.0,120487.0,112643.0,78413.0,113053.0,80220.0,58352.0,112611.0,66097.0,63690.0,66159.0}};
                                                
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

    unsigned int migrate, migtick, mig_app, mig_task, mig_addr, num_pes;
    migtick = 0;
    float candidate_score;
                        
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
    printsv("Cluster Formation: ", CLUSTER_FORMATION);
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
		printsv("GlobalMasterActive ", tick);
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
#if CLUSTER_FORMATION == 0 // CLUSTERLESS
                    API_Clusterless(app);
#else // TEMP_CLUSTER TASK_CLUSTER FIT_CLUSTER 
                    API_FindBestCluster(app);
#endif
                }
                base_addr = applications[app].cluster_addr;
                cluster_size = applications[app].cluster_size;
                printsvsv("Cluster addr: ", base_addr, " cluster size: ", cluster_size);

                // fill the auxiliar vectors
                unsigned int high_score = 0;
                unsigned int high_addr = 0;
                for( i = getXpos(base_addr); i < (getXpos(base_addr)+cluster_size); i++){
                    for( j = getYpos(base_addr); j < (getYpos(base_addr)+cluster_size); j++){
                        // if the PE is capable to run the task...
                        if(Tiles[i][j].taskSlots > 0 && makeAddress(i, j) != MASTER_ADDR){
                            unsigned int score = (int)(scoreTable[applications[app].taskType[task]][API_getPEState(addr2id(makeAddress(i,j)), -1)]);
                            // better score
                            if(score > high_score){
                                high_score = score;
                                high_addr = makeAddress(i, j);
                            }
                            // same score - find the minimum hop-count
                            else if(score == high_score){
                                int hop_atual = 0;
                                int hop_new = 0;
                                int x_atual = getXpos(high_addr);
                                int y_atual = getYpos(high_addr);
                                for( int i_task = 0; i_task < task; i_task++ ){
                                    int deltaX = x_atual - getXpos(applications[app].tasks[i_task].addr);
                                    if (deltaX < 0) { deltaX = deltaX*-1; }
                                    int deltaY = y_atual - getYpos(applications[app].tasks[i_task].addr);
                                    if (deltaY < 0) { deltaY = deltaY*-1; }
                                    hop_atual += (deltaX + deltaY);
                                    
                                    deltaX = i - getXpos(applications[app].tasks[i_task].addr);
                                    if (deltaX < 0) { deltaX = deltaX*-1; }
                                    deltaY = j - getYpos(applications[app].tasks[i_task].addr);
                                    if (deltaY < 0) { deltaY = deltaY*-1; }
                                    hop_new += (deltaX + deltaY);
                                }
                                if(hop_new < hop_atual){
                                    high_score = score;
                                    high_addr = makeAddress(i, j);
                                }
                            }
                        }
                    }
                }
                
                slot = API_GetTaskSlotFromTile(high_addr, app, task);
                if (slot == ERRO) while(1) {
                    prints("ERRO ALLOCATING!");
                }

                // register the application allocation
                applications[app].tasks[task].addr = high_addr;
                printsvsv("X: ", getXpos(high_addr), "Y: ", getYpos(high_addr));
                applications[app].tasks[task].slot = slot;
                applications[app].lastStart = applications[app].nextRun;
                API_RepositoryAllocation(app, task, high_addr);
                debug_task_alloc(tick, app, task, high_addr);

                last_app = app;

                // Checks if there is some task to start...
                API_StartTasks();
            }

        }
#if MIGRATION_AVAIL
        else if (migtick+200 <= xTaskGetTickCount()){
            migrate = API_SelectTask_Migration_Temperature(THRESHOLD_TEMP);
            if (migrate != ERRO  && !API_CheckTaskToAllocate(xTaskGetTickCount()) ){
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
static void GlobalManagerTask( void *pvParameters ){
    ( void ) pvParameters;
	unsigned int tick, toprint;

    float scoreTable[N_TASKTYPE][N_STATES*4] = {{220983.0, 183400.0, 176736.0, 173356.0, 166769.0, 206434.0, 180681.0, 177830.0, 169379.0, 187242.0, 179326.0, 170828.0, 190267.0, 167729.0, 177127.0, 211756.0, 186062.0, 179650.0, 174625.0, 199715.0, 191349.0, 174712.0, 191592.0, 178685.0, 180354.0, 200661.0, 195655.0, 177959.0, 193195.0, 180278.0, 185919.0, 194619.0, 185027.0, 187444.0, 188912.0, 208998.0, 177893.0, 171584.0, 172869.0, 168257.0, 196499.0, 175786.0, 175392.0, 165152.0, 190848.0, 177020.0, 169236.0, 210049.0, 174545.0, 194167.0, 205247.0, 200634.0, 171833.0, 172044.0, 200039.0, 176032.0, 170335.0, 194389.0, 174904.0, 184628.0, 195960.0, 183759.0, 173712.0, 191958.0, 175129.0, 181911.0, 192019.0, 177417.0, 183235.0, 187005.0, 213932.0, 178093.0, 180230.0, 184550.0, 174807.0, 208619.0, 199374.0, 191458.0, 177561.0, 199076.0, 187609.0, 173701.0, 244634.0, 188841.0, 238015.0, 206968.0, 196343.0, 187175.0, 180339.0, 205032.0, 195865.0, 176302.0, 197392.0, 184779.0, 193860.0, 198137.0, 194785.0, 179054.0, 195892.0, 183010.0, 191584.0, 190546.0, 181693.0, 187612.0, 188.0, 217100.0, 171607.0, 163648.0, 154243.0, 0.0, 182674.0, 171506.0, 160732.0, 0.0, 179056.0, 163601.0, 0.0, 166907.0, 0.0, 0.0, 200516.0, 175552.0, 169936.0, 0.0, 182904.0, 172605.0, 0.0, 173651.0, 0.0, 0.0, 187319.0, 176712.0, 0.0, 179144.0, 0.0, 0.0, 177250.0, 0.0, 0.0, 0.0},
                                                {204129.0, 188119.0, 174578.0, 170866.0, 148280.0, 191045.0, 185262.0, 172127.0, 157570.0, 186703.0, 175942.0, 161520.0, 180134.0, 147910.0, 147712.0, 198479.0, 189206.0, 173854.0, 159655.0, 187625.0, 177514.0, 164103.0, 179591.0, 168217.0, 169886.0, 188539.0, 179302.0, 169547.0, 179486.0, 170592.0, 172547.0, 179703.0, 172240.0, 173196.0, 157253.0, 194710.0, 178195.0, 163870.0, 158460.0, 129490.0, 184083.0, 169455.0, 164510.0, 132583.0, 176441.0, 158085.0, 131767.0, 200081.0, 144197.0, 161859.0, 191796.0, 175332.0, 167781.0, 142307.0, 179484.0, 171111.0, 152178.0, 172156.0, 150199.0, 160709.0, 181744.0, 173202.0, 149627.0, 174812.0, 153985.0, 159282.0, 170411.0, 149666.0, 156287.0, 156152.0, 200952.0, 173531.0, 160755.0, 154881.0, 142282.0, 187134.0, 169241.0, 158027.0, 146388.0, 178135.0, 162879.0, 143356.0, 251347.0, 158351.0, 195518.0, 192631.0, 175003.0, 166118.0, 149864.0, 183671.0, 170948.0, 153186.0, 174479.0, 157148.0, 173430.0, 184782.0, 175545.0, 157514.0, 176778.0, 158773.0, 174395.0, 176166.0, 159125.0, 167427.0, 175739.0, 199228.0, 169284.0, 156741.0, 125435.0, 0.0, 181713.0, 162965.0, 141594.0, 0.0, 172789.0, 137610.0, 0.0, 128523.0, 0.0, 0.0, 186519.0, 170332.0, 147770.0, 0.0, 173126.0, 154319.0, 0.0, 148836.0, 0.0, 0.0, 173529.0, 160505.0, 0.0, 150854.0, 0.0, 0.0, 136175.0, 0.0, 0.0, 0.0},
                                                {152493.0, 133672.0, 124197.0, 121979.0, 99807.0, 138192.0, 129176.0, 123505.0, 106440.0, 130001.0, 123208.0, 107126.0, 110333.0, 93052.0, 77691.0, 142259.0, 130948.0, 123095.0, 110143.0, 130565.0, 123037.0, 112330.0, 124471.0, 121398.0, 103700.0, 129975.0, 123058.0, 115951.0, 123286.0, 119801.0, 121827.0, 123985.0, 118483.0, 95167.0, 65934.0, 139198.0, 125048.0, 120642.0, 106602.0, 56360.0, 128317.0, 122706.0, 118891.0, 59220.0, 123640.0, 107067.0, 67653.0, 107891.0, 80603.0, 103399.0, 131163.0, 124170.0, 122511.0, 84307.0, 123201.0, 122726.0, 95728.0, 113588.0, 86377.0, 91517.0, 126876.0, 123324.0, 80312.0, 123729.0, 83756.0, 88082.0, 103582.0, 75214.0, 75894.0, 96454.0, 146407.0, 129596.0, 123395.0, 100799.0, 65261.0, 132902.0, 124700.0, 105235.0, 70541.0, 122215.0, 103826.0, 79055.0, 158816.0, 89772.0, 122471.0, 134571.0, 127621.0, 112894.0, 71146.0, 130625.0, 121191.0, 75342.0, 116498.0, 84654.0, 97498.0, 126868.0, 115705.0, 68610.0, 118760.0, 74241.0, 87654.0, 99648.0, 69010.0, 80800.0, 92679.0, 144955.0, 125382.0, 107807.0, 62883.0, 0.0, 125979.0, 120783.0, 72714.0, 0.0, 122932.0, 62424.0, 0.0, 50957.0, 0.0, 0.0, 130296.0, 122477.0, 73695.0, 0.0, 122756.0, 83339.0, 0.0, 68120.0, 0.0, 0.0, 122913.0, 72426.0, 0.0, 64971.0, 0.0, 0.0, 57502.0, 0.0, 0.0, 0.0}};


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

    unsigned int migrate, migtick, mig_app, mig_task, mig_addr, num_pes;
    migtick = 0;
    float candidate_score;
                        
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
        for(state = 0; state < N_STATES*4; state++){
            scoreTable[tp][state] = scoreTable[tp][state]/1000;
        }
    }
    printsv("Cluster Formation: ", CLUSTER_FORMATION);
    API_PrintScoreTable(scoreTable);

	// Initialize the System Tiles Info
	API_TilesReset();

    Tiles[getXpos(GLOBAL_MASTER_ADDR)][getYpos(GLOBAL_MASTER_ADDR)].taskType = 2;
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
		printsv("GlobalMasterActive ", tick);
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
#if CLUSTER_FORMATION == 0 // CLUSTERLESS
                    API_Clusterless(app);
#else // TEMP_CLUSTER TASK_CLUSTER FIT_CLUSTER 
                    API_FindBestCluster(app);
#endif
                }
                base_addr = applications[app].cluster_addr;
                cluster_size = applications[app].cluster_size;
                printsvsv("Cluster addr: ", base_addr, " cluster size: ", cluster_size);

                // fill the auxiliar vectors
                unsigned int high_score = 0;
                unsigned int high_addr = 0;
                for( i = getXpos(base_addr); i < (getXpos(base_addr)+cluster_size); i++){
                    for( j = getYpos(base_addr); j < (getYpos(base_addr)+cluster_size); j++){
                        // if the PE is capable to run the task...
                        if(Tiles[i][j].taskSlots > 0 && makeAddress(i, j) != MASTER_ADDR){
                            unsigned int score = (int)(scoreTable[applications[app].taskType[task]][API_getPEStateX4(addr2id(makeAddress(i,j)), -1)]);
                            // better score
                            if(score > high_score){
                                high_score = score;
                                high_addr = makeAddress(i, j);
                            }
                            // same score - find the minimum hop-count
                            else if(score == high_score){
                                int hop_atual = 0;
                                int hop_new = 0;
                                int x_atual = getXpos(high_addr);
                                int y_atual = getYpos(high_addr);
                                for( int i_task = 0; i_task < task; i_task++ ){
                                    int deltaX = x_atual - getXpos(applications[app].tasks[i_task].addr);
                                    if (deltaX < 0) { deltaX = deltaX*-1; }
                                    int deltaY = y_atual - getYpos(applications[app].tasks[i_task].addr);
                                    if (deltaY < 0) { deltaY = deltaY*-1; }
                                    hop_atual += (deltaX + deltaY);
                                    
                                    deltaX = i - getXpos(applications[app].tasks[i_task].addr);
                                    if (deltaX < 0) { deltaX = deltaX*-1; }
                                    deltaY = j - getYpos(applications[app].tasks[i_task].addr);
                                    if (deltaY < 0) { deltaY = deltaY*-1; }
                                    hop_new += (deltaX + deltaY);
                                }
                                if(hop_new < hop_atual){
                                    high_score = score;
                                    high_addr = makeAddress(i, j);
                                }
                            }
                        }
                    }
                }
                
                slot = API_GetTaskSlotFromTile(high_addr, app, task);
                if (slot == ERRO) while(1) {
                    prints("ERRO ALLOCATING!");
                }

                // register the application allocation
                applications[app].tasks[task].addr = high_addr;
                printsvsv("X: ", getXpos(high_addr), "Y: ", getYpos(high_addr));
                applications[app].tasks[task].slot = slot;
                applications[app].lastStart = applications[app].nextRun;
                API_RepositoryAllocation(app, task, high_addr);
                debug_task_alloc(tick, app, task, high_addr);

                last_app = app;

                // Checks if there is some task to start...
                API_StartTasks();
            }

        }
#if MIGRATION_AVAIL
        else if (migtick+200 <= xTaskGetTickCount()){
            migrate = API_SelectTask_Migration_Temperature(THRESHOLD_TEMP);
            if (migrate != ERRO  && !API_CheckTaskToAllocate(xTaskGetTickCount()) ){
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
                        sorted_score[i] = (int)(scoreTable[applications[mig_app].taskType[mig_task]][API_getPEStateX4(addr2id(sorted_addr[i]), currentAddr)]*1000);                    
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


#elif THERMAL_MANAGEMENT == 6 // CHRONOS3

static void GlobalManagerTask( void *pvParameters ){
    ( void ) pvParameters;
	unsigned int tick, toprint;
    float scoreTable[N_TASKTYPE][N_STATES] = {  {192655.0, 164239.0, 156694.0, 153775.0, 145624.0, 183195.0, 159204.0, 155540.0, 147834.0, 172501.0, 162305.0, 149201.0, 163523.0, 153136.0, 163335.0, 183497.0, 175356.0, 154248.0, 157376.0, 179169.0, 157631.0, 157722.0, 164531.0, 157134.0, 158165.0, 182596.0, 166826.0, 159098.0, 170940.0, 158929.0, 158851.0, 176307.0, 163230.0, 164650.0, 166839.0 },
                                                {169002.0, 155648.0, 139249.0, 131138.0, 106196.0, 160475.0, 147492.0, 138419.0, 120385.0, 151185.0, 142277.0, 109210.0, 126598.0, 111721.0, 128691.0, 161548.0, 148576.0, 139609.0, 134983.0, 153915.0, 143760.0, 138028.0, 145367.0, 131663.0, 119567.0, 154961.0, 143982.0, 140031.0, 148302.0, 142114.0, 118773.0, 149415.0, 136188.0, 126077.0, 130840.0 },
                                                {137390.0, 121613.0, 113255.0, 112522.0, 58298.0, 121794.0, 112299.0, 111883.0, 70772.0, 119232.0, 112134.0, 63224.0, 89582.0, 56235.0, 81485.0, 122649.0, 115967.0, 112156.0, 67383.0, 118612.0, 112024.0, 82751.0, 112555.0, 71755.0, 61248.0, 121206.0, 112567.0, 79617.0, 112731.0, 80050.0, 61450.0, 114317.0, 67351.0, 69677.0, 64767.0 } };
    
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

    //    policy table initialization
    for(tp = 0; tp < N_TASKTYPE; tp++){
        for(state = 0; state < N_STATES; state++){
            scoreTable[tp][state] = scoreTable[tp][state]/1000;
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
		printsv("GlobalMasterActive ", tick);
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
                    //API_Clusterless(app);
                    base_addr = makeAddress(0,0);
                    cluster_size = DIM_X;
                    // base_addr = applications[app].cluster_addr;
                    // cluster_size = applications[app].cluster_size;
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

