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
                        mig_addr = getNextPriorityAddr(-1);
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

    float scoreTable[N_TASKTYPE][N_STATES] = {  {138687.0, 121503.0, 114575.0, 110699.0, 56526.0, 123195.0, 114392.0, 108767.0, 70227.0, 117352.0, 112982.0, 65062.0, 92423.0, 56379.0, 83650.0, 122469.0, 117447.0, 112243.0, 74195.0, 118925.0, 112351.0, 88156.0, 112701.0, 68565.0, 59386.0, 120331.0, 111951.0, 76220.0, 113415.0, 78969.0, 61138.0, 114166.0, 72913.0, 64721.0, 65374.0 },
    { 192063.0, 164134.0, 153183.0, 152831.0, 146412.0, 183707.0, 157880.0, 153422.0, 148380.0, 169954.0, 160432.0, 148673.0, 163905.0, 152640.0, 163539.0, 183171.0, 174365.0, 153691.0, 156239.0, 179699.0, 158222.0, 158010.0, 163538.0, 158345.0, 157932.0, 179175.0, 169595.0, 158141.0, 172728.0, 159839.0, 157295.0, 176149.0, 163381.0, 163964.0, 166269.0 },
    { 172801.0, 157451.0, 140754.0, 133983.0, 105723.0, 159712.0, 147747.0, 136851.0, 120163.0, 151967.0, 139685.0, 112248.0, 128679.0, 110277.0, 128750.0, 163953.0, 150386.0, 138846.0, 134899.0, 154624.0, 142563.0, 137863.0, 143251.0, 131151.0, 119338.0, 156963.0, 143618.0, 141308.0, 145729.0, 141556.0, 121474.0, 149340.0, 134820.0, 126593.0, 128648.0} };
                                                
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

    float scoreTable[N_TASKTYPE][N_STATES*4] = {  {223163.0, 183973.0, 176362.0, 174257.0, 165916.0, 208126.0, 180457.0, 178127.0, 168254.0, 188110.0, 180049.0, 170939.0, 190845.0, 167705.0, 178126.0, 214468.0, 188092.0, 180306.0, 175291.0, 193391.0, 185313.0, 177232.0, 192553.0, 179508.0, 181283.0, 200615.0, 195302.0, 179035.0, 196959.0, 181441.0, 187830.0, 195988.0, 184529.0, 187289.0, 188226.0, 209626.0, 178339.0, 173942.0, 174967.0, 169675.0, 196921.0, 174890.0, 176430.0, 166595.0, 193756.0, 177140.0, 167245.0, 211451.0, 172207.0, 194789.0, 207338.0, 199418.0, 174033.0, 171746.0, 198197.0, 175366.0, 171562.0, 194506.0, 176017.0, 182976.0, 199715.0, 188048.0, 174132.0, 193265.0, 173984.0, 183299.0, 190178.0, 177455.0, 183755.0, 187260.0, 216227.0, 179774.0, 180892.0, 184182.0, 176143.0, 207233.0, 198550.0, 189732.0, 179867.0, 198804.0, 188301.0, 174450.0, 245681.0, 191014.0, 239615.0, 211363.0, 203889.0, 185795.0, 179193.0, 203284.0, 197540.0, 180021.0, 195394.0, 183303.0, 192693.0, 197392.0, 194057.0, 179124.0, 197353.0, 180888.0, 190351.0, 191950.0, 182091.0, 186945.0, 188866.0, 215145.0, 172583.0, 165658.0, 154177.0, 0.0, 182447.0, 173499.0, 159356.0, 0.0, 179635.0, 162283.0, 0.0, 169120.0, 0.0, 0.0, 202869.0, 177783.0, 168973.0, 0.0, 180920.0, 171900.0, 0.0, 173601.0, 0.0, 0.0, 186347.0, 177091.0, 0.0, 180078.0, 0.0, 0.0, 177646.0, 0.0, 0.0, 0.0 },
                                                    {206149.0, 187643.0, 176009.0, 171846.0, 147191.0, 192177.0, 180695.0, 170950.0, 155992.0, 187101.0, 175076.0, 164733.0, 182207.0, 148674.0, 147713.0, 199617.0, 188458.0, 173183.0, 162735.0, 187906.0, 172898.0, 164847.0, 180603.0, 170023.0, 168832.0, 186892.0, 178597.0, 170979.0, 182440.0, 171958.0, 172828.0, 178922.0, 172255.0, 172708.0, 156657.0, 195133.0, 182923.0, 165583.0, 159438.0, 132320.0, 184726.0, 169354.0, 162037.0, 134599.0, 178725.0, 161904.0, 131455.0, 202136.0, 143209.0, 163199.0, 189860.0, 176745.0, 170482.0, 141378.0, 181779.0, 171632.0, 154541.0, 169703.0, 150463.0, 160743.0, 181466.0, 173683.0, 150552.0, 173387.0, 151400.0, 156373.0, 170278.0, 150731.0, 159983.0, 156461.0, 201905.0, 176963.0, 160753.0, 154342.0, 143019.0, 187368.0, 169282.0, 157399.0, 146132.0, 176837.0, 163791.0, 141082.0, 252617.0, 157839.0, 196247.0, 193422.0, 176380.0, 166080.0, 151279.0, 183397.0, 171464.0, 153903.0, 174542.0, 156237.0, 174866.0, 183130.0, 175482.0, 159999.0, 175672.0, 160518.0, 174358.0, 176303.0, 160066.0, 168302.0, 176981.0, 199754.0, 169136.0, 158245.0, 130939.0, 0.0, 181013.0, 164494.0, 140137.0, 0.0, 172136.0, 136066.0, 0.0, 126924.0, 0.0, 0.0, 186343.0, 171458.0, 150925.0, 0.0, 172692.0, 156899.0, 0.0, 147764.0, 0.0, 0.0, 174116.0, 156714.0, 0.0, 150666.0, 0.0, 0.0, 135418.0, 0.0, 0.0, 0.0 },
                                                    {152435.0, 130440.0, 124747.0, 121735.0, 98777.0, 135286.0, 126538.0, 122209.0, 105119.0, 130724.0, 123188.0, 113211.0, 108238.0, 96430.0, 79367.0, 142420.0, 130637.0, 122312.0, 108141.0, 130517.0, 122738.0, 111679.0, 124543.0, 120838.0, 100479.0, 130394.0, 123851.0, 117285.0, 123939.0, 119387.0, 122390.0, 124374.0, 118856.0, 93254.0, 65952.0, 138643.0, 123789.0, 117457.0, 105172.0, 55630.0, 127742.0, 121456.0, 119926.0, 59652.0, 123637.0, 111319.0, 66784.0, 108434.0, 79374.0, 103143.0, 132606.0, 123199.0, 121831.0, 83926.0, 124435.0, 122123.0, 96327.0, 112722.0, 87658.0, 93668.0, 124766.0, 122588.0, 81001.0, 123043.0, 85119.0, 90754.0, 104190.0, 74218.0, 76135.0, 98352.0, 146420.0, 128953.0, 122645.0, 100137.0, 65779.0, 134633.0, 125092.0, 103330.0, 68622.0, 120504.0, 105138.0, 79274.0, 159113.0, 89990.0, 123836.0, 137488.0, 127239.0, 116047.0, 67183.0, 128908.0, 123007.0, 71617.0, 117227.0, 84143.0, 97909.0, 125043.0, 119539.0, 71685.0, 119718.0, 72314.0, 87754.0, 101377.0, 69346.0, 81322.0, 92885.0, 144006.0, 122534.0, 109721.0, 54021.0, 0.0, 123914.0, 120081.0, 73184.0, 0.0, 122462.0, 64064.0, 0.0, 50631.0, 0.0, 0.0, 128076.0, 121807.0, 80160.0, 0.0, 122071.0, 82614.0, 0.0, 67503.0, 0.0, 0.0, 122321.0, 75279.0, 0.0, 66014.0, 0.0, 0.0, 58656.0, 0.0, 0.0, 0.0 } };
                                                
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

