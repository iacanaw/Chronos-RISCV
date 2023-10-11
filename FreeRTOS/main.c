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

    float scoreTable[N_TASKTYPE][N_STATES] = {  {292818.0, 272202.0, 274580.0, 275294.0, 262023.0, 281695.0, 273615.0, 267744.0, 257679.0, 278115.0, 274214.0, 263663.0, 275538.0, 262892.0, 270704.0, 283972.0, 277759.0, 285016.0, 261837.0, 283386.0, 285139.0, 270246.0, 281223.0, 263793.0, 278332.0, 279578.0, 297328.0, 265750.0, 278043.0, 268531.0, 278266.0, 282416.0, 284622.0, 281228.0, 285836.0 },
    {262979.0, 244660.0, 240420.0, 228460.0, 202928.0, 251812.0, 243740.0, 249421.0, 213862.0, 248022.0, 233502.0, 205042.0, 234275.0, 212445.0, 228112.0, 253730.0, 250311.0, 240876.0, 226064.0, 256075.0, 246287.0, 232939.0, 240887.0, 231783.0, 225596.0, 257027.0, 248228.0, 232211.0, 247064.0, 236992.0, 231948.0, 249303.0, 237092.0, 241810.0, 241988.0 },
    {236292.0, 207428.0, 204433.0, 201601.0, 142434.0, 203811.0, 200778.0, 199711.0, 149979.0, 202245.0, 200275.0, 145661.0, 171209.0, 148276.0, 169952.0, 217595.0, 209854.0, 199324.0, 162379.0, 201806.0, 203415.0, 170237.0, 200512.0, 155393.0, 153675.0, 211347.0, 216175.0, 173497.0, 211577.0, 178321.0, 163358.0, 206199.0, 166736.0, 165664.0, 171335.0 } };

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
                cluster_size = k; //applications[app].cluster_size*applications[app].cluster_size;
                //vTaskEnterCritical();
                printsv("Sorting num_pes = ", cluster_size);
                quickSort(sorted_score, sorted_addr, 0, cluster_size);
                // for( i = 0; i < cluster_size; i++){
                //     printsvsv("score ", sorted_score[i], "addr: ", sorted_addr[i]);
                // }
                randPositions(sorted_score, sorted_addr, 0, cluster_size);
                // for( i = 0; i < cluster_size; i++){
                //     printsvsv("score ", sorted_score[i], "addr: ", sorted_addr[i]);
                // }
                
                //vTaskExitCritical();
                // try to get the best tile slot (higher score)
                for(i = cluster_size; i >= 0; i--){
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
    dc = objectiveState[tasktype][5]+1;

    while(1){
        if( a <= 4 ){
            if(b <= 4 ){
                if( c <= 4 ){
                    if( da <= 4 ){
                        if( db <= 4 ){
                            if( dc <= 4 ){
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
                                dc++;
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
    float scoreTable[N_TASKTYPE][N_STATES] = {  {236292.0, 248357.0, 227885.0, 205551.0, 35263.0, 251655.0, 240505.0, 222500.0, 23046.0, 220253.0, 147869.0, 13309.0, 33116.0, 5080.0, 8534.0, 238412.0, 239402.0, 226442.0, 96557.0, 250195.0, 244360.0, 50702.0, 160435.0, 24333.0, 13896.0, 244406.0, 239284.0, 112669.0, 240564.0, 96661.0, 4014.0, 239470.0, 128253.0, 15408.0, 21731.0, 245133.0, 241008.0, 217654.0, 219962.0, 82559.0, 251981.0, 236918.0, 217991.0, 68630.0, 187498.0, 191526.0, 51994.0, 66669.0, 8661.0, 7588.0, 246316.0, 234550.0, 217917.0, 152508.0, 238590.0, 225524.0, 152672.0, 191727.0, 80049.0, 7917.0, 243837.0, 233245.0, 170706.0, 221499.0, 149007.0, 33727.0, 241905.0, 135179.0, 46961.0, 39403.0, 268958.0, 240140.0, 218268.0, 190171.0, 40251.0, 245034.0, 238125.0, 208986.0, 76574.0, 180461.0, 120406.0, 21042.0, 35589.0, 8725.0, 1859.0, 253386.0, 229199.0, 219443.0, 131272.0, 246744.0, 220435.0, 114119.0, 139666.0, 40117.0, 8719.0, 238334.0, 233862.0, 157026.0, 228552.0, 130477.0, 20860.0, 209763.0, 131107.0, 47621.0, 34824.0, 210077.0, 216874.0, 202295.0, 109110.0, 26997.0, 137167.0, 147113.0, 103667.0, 34489.0, 38730.0, 37064.0, 28336.0, 27773.0, 5030.0, 1687.0, 229594.0, 213373.0, 183117.0, 74081.0, 150652.0, 150477.0, 78444.0, 40228.0, 35847.0, 6910.0, 203359.0, 176986.0, 100665.0, 99709.0, 84296.0, 32897.0, 101353.0, 104682.0, 35928.0, 23427.0, 81771.0, 90069.0, 63580.0, 35794.0, 14908.0, 40462.0, 42662.0, 27101.0, 35040.0, 20784.0, 22126.0, 14757.0, 1787.0, 5026.0, 5096.0, 66173.0, 79192.0, 54459.0, 86201.0, 31364.0, 33445.0, 108666.0, 8000.0, 12790.0, 1730.0, 70033.0, 58696.0, 150881.0, 12372.0, 103173.0, 22347.0, 9624.0, 72761.0, 27878.0, 11057.0, 252000.0, 244603.0, 238241.0, 179210.0, 50181.0, 240065.0, 221874.0, 175905.0, 34109.0, 99799.0, 87189.0, 15513.0, 10863.0, 1722.0, 3245.0, 252746.0, 243883.0, 215221.0, 83448.0, 221353.0, 208227.0, 58543.0, 58784.0, 9244.0, 10189.0, 250433.0, 221066.0, 67763.0, 169335.0, 59936.0, 9213.0, 137196.0, 44434.0, 22618.0, 9600.0, 258667.0, 252458.0, 232288.0, 172161.0, 67580.0, 183137.0, 206542.0, 179529.0, 61127.0, 68503.0, 74849.0, 25597.0, 23006.0, 7930.0, 1666.0, 258944.0, 243985.0, 223832.0, 97676.0, 210446.0, 217722.0, 87416.0, 102492.0, 31881.0, 7774.0, 238572.0, 234725.0, 143692.0, 184067.0, 85602.0, 22070.0, 154775.0, 88951.0, 45165.0, 37440.0, 213010.0, 219381.0, 194525.0, 94142.0, 45163.0, 130227.0, 167644.0, 140827.0, 42256.0, 43098.0, 47705.0, 35016.0, 12233.0, 14236.0, 2669.0, 195075.0, 225068.0, 190444.0, 82616.0, 153072.0, 162051.0, 83795.0, 43594.0, 37752.0, 8662.0, 206625.0, 184921.0, 97893.0, 107540.0, 68371.0, 16044.0, 86457.0, 100302.0, 41059.0, 14226.0, 95404.0, 97306.0, 79454.0, 53817.0, 62865.0, 42344.0, 70996.0, 55732.0, 43843.0, 12669.0, 12979.0, 17353.0, 23710.0, 5412.0, 1860.0, 77881.0, 141797.0, 68649.0, 123025.0, 64323.0, 48002.0, 73996.0, 12030.0, 16619.0, 2465.0, 74211.0, 104871.0, 163161.0, 39136.0, 100777.0, 7155.0, 35149.0, 102013.0, 36287.0, 22786.0, 91128.0, 164122.0, 131695.0, 60361.0, 15275.0, 52303.0, 87146.0, 40305.0, 20017.0, 14703.0, 10503.0, 4143.0, 10126.0, 2475.0, 2651.0, 150958.0, 162983.0, 140133.0, 25431.0, 66984.0, 86991.0, 33423.0, 17019.0, 15053.0, 7150.0, 64243.0, 78050.0, 28672.0, 56127.0, 33698.0, 6893.0, 26004.0, 15214.0, 4419.0, 8899.0, 86687.0, 127252.0, 140329.0, 75378.0, 21505.0, 57595.0, 90124.0, 85725.0, 24761.0, 14640.0, 31319.0, 27054.0, 13594.0, 1651.0, 1702.0, 129286.0, 155744.0, 134706.0, 40517.0, 63950.0, 101420.0, 27785.0, 11389.0, 16166.0, 6769.0, 108059.0, 106282.0, 51684.0, 38278.0, 44388.0, 3943.0, 47182.0, 28704.0, 11150.0, 3140.0, 61728.0, 76840.0, 71484.0, 42661.0, 16070.0, 18204.0, 45572.0, 41191.0, 34489.0, 18676.0, 11977.0, 3707.0, 6731.0, 1880.0, 1763.0, 42274.0, 72535.0, 44987.0, 66490.0, 28193.0, 39562.0, 36532.0, 3622.0, 19136.0, 19968.0, 45913.0, 53161.0, 83591.0, 7150.0, 33287.0, 12810.0, 14421.0, 31633.0, 11908.0, 4257.0, 26559.0, 39223.0, 23314.0, 16134.0, 12969.0, 8849.0, 11811.0, 14592.0, 2029.0, 3173.0, 14593.0, 5589.0, 6785.0, 3724.0, 3803.0, 19656.0, 29648.0, 20927.0, 4779.0, 12614.0, 9054.0, 3341.0, 5788.0, 3661.0, 8719.0, 18961.0, 14072.0, 8876.0, 4767.0, 3465.0, 5731.0, 8395.0, 12603.0, 5092.0, 2483.0, 36143.0, 45185.0, 32437.0, 29848.0, 23333.0, 31358.0, 34008.0, 20768.0, 10854.0, 8975.0, 13555.0, 1660.0, 4666.0, 5567.0, 9341.0, 8849.0, 22681.0, 13728.0, 3184.0, 10774.0, 1672.0, 7973.0, 2097.0, 8630.0, 2729.0, 11224.0, 10993.0, 10477.0, 5017.0, 4648.0, 4694.0, 6923.0, 12274.0, 5614.0, 12065.0, 34023.0, 14416.0, 29034.0, 23709.0, 6232.0, 15314.0, 6680.0, 3939.0, 15128.0, 6944.0, 3462.0, 5110.0, 5226.0, 1720.0, 5134.0, 8945.0, 4404.0, 5355.0, 3215.0, 5329.0, 1685.0, 4469.0, 2773.0, 1872.0, 7891.0, 5423.0, 1827.0, 3154.0, 5213.0, 3341.0, 4687.0, 5445.0, 4408.0, 1759.0, 2177.0, 244731.0, 247341.0, 226241.0, 234283.0, 62129.0, 252642.0, 233345.0, 219103.0, 46980.0, 179445.0, 188190.0, 25511.0, 13606.0, 9072.0, 6292.0, 241445.0, 242453.0, 218098.0, 125881.0, 247324.0, 223018.0, 138941.0, 135721.0, 41723.0, 3451.0, 246447.0, 246003.0, 169494.0, 224422.0, 139239.0, 34132.0, 247398.0, 132661.0, 52965.0, 40291.0, 244378.0, 244460.0, 224227.0, 205940.0, 111100.0, 247481.0, 226696.0, 213260.0, 96249.0, 181174.0, 194107.0, 58301.0, 23663.0, 20408.0, 4804.0, 241740.0, 235070.0, 214448.0, 181826.0, 245456.0, 218045.0, 167383.0, 180207.0, 91229.0, 27503.0, 247900.0, 234991.0, 209624.0, 233641.0, 156879.0, 28834.0, 238874.0, 171420.0, 62000.0, 62507.0, 252878.0, 243274.0, 221813.0, 194868.0, 69626.0, 233543.0, 220130.0, 197552.0, 81309.0, 143848.0, 116408.0, 37905.0, 26495.0, 28639.0, 1648.0, 249487.0, 227918.0, 205711.0, 142396.0, 217608.0, 221384.0, 145174.0, 90593.0, 91664.0, 5735.0, 252198.0, 222946.0, 175966.0, 210036.0, 132082.0, 37896.0, 184777.0, 124833.0, 59435.0, 56445.0, 149583.0, 188367.0, 143121.0, 98915.0, 72072.0, 86486.0, 116847.0, 71064.0, 86746.0, 52973.0, 15701.0, 61327.0, 6901.0, 6016.0, 4429.0, 170978.0, 200025.0, 154134.0, 193381.0, 57799.0, 117306.0, 190848.0, 12538.0, 90886.0, 16926.0, 161858.0, 154562.0, 192145.0, 66450.0, 184838.0, 51029.0, 72878.0, 184165.0, 63938.0, 65814.0, 254186.0, 250849.0, 226366.0, 196427.0, 41166.0, 208308.0, 227387.0, 188222.0, 68910.0, 89974.0, 73770.0, 36744.0, 32525.0, 4892.0, 3484.0, 257709.0, 239273.0, 231654.0, 78577.0, 221501.0, 208976.0, 82605.0, 41965.0, 30933.0, 15314.0, 238111.0, 237163.0, 113961.0, 157472.0, 102445.0, 29680.0, 169278.0, 81484.0, 23334.0, 9082.0, 256405.0, 235642.0, 212180.0, 177473.0, 45986.0, 189503.0, 203438.0, 177123.0, 78197.0, 79497.0, 83178.0, 37350.0, 11744.0, 4911.0, 3571.0, 254506.0, 245350.0, 220454.0, 137680.0, 206932.0, 213808.0, 132012.0, 69557.0, 62456.0, 10398.0, 229677.0, 233182.0, 143907.0, 173139.0, 103710.0, 42503.0, 178340.0, 109522.0, 40220.0, 23327.0, 133484.0, 168285.0, 157076.0, 87661.0, 101085.0, 84557.0, 115851.0, 95153.0, 106277.0, 17131.0, 43919.0, 39455.0, 16768.0, 3657.0, 4620.0, 177247.0, 202674.0, 162059.0, 189260.0, 129385.0, 152800.0, 166084.0, 30054.0, 65207.0, 11104.0, 154569.0, 174286.0, 189938.0, 76871.0, 162729.0, 7678.0, 60510.0, 180884.0, 53652.0, 73550.0, 56772.0, 143230.0, 142317.0, 91821.0, 20767.0, 78448.0, 102856.0, 69360.0, 12511.0, 14270.0, 14694.0, 6292.0, 6609.0, 6948.0, 7424.0, 137980.0, 167430.0, 116129.0, 40227.0, 80458.0, 99975.0, 37988.0, 15311.0, 7734.0, 1739.0, 105072.0, 132324.0, 33036.0, 59512.0, 29387.0, 7693.0, 27956.0, 15569.0, 4943.0, 7355.0, 59400.0, 85576.0, 90536.0, 42498.0, 52552.0, 44894.0, 69309.0, 51107.0, 16515.0, 16963.0, 15497.0, 11109.0, 11464.0, 6012.0, 4135.0, 102635.0, 150277.0, 82599.0, 148412.0, 31088.0, 60822.0, 83068.0, 13055.0, 13080.0, 5243.0, 75983.0, 79375.0, 128058.0, 35980.0, 36552.0, 1923.0, 20141.0, 53496.0, 13418.0, 9737.0, 38563.0, 32712.0, 30851.0, 26067.0, 9085.0, 15189.0, 12933.0, 11569.0, 2628.0, 9244.0, 1869.0, 7634.0, 6310.0, 5329.0, 3411.0, 31156.0, 22241.0, 7050.0, 12877.0, 7690.0, 12262.0, 18668.0, 1799.0, 3699.0, 2492.0, 6906.0, 4885.0, 13765.0, 6936.0, 11477.0, 3916.0, 2007.0, 13424.0, 1743.0, 7518.0, 245188.0, 255328.0, 240376.0, 216379.0, 59937.0, 257274.0, 235631.0, 213073.0, 74099.0, 112576.0, 164564.0, 25484.0, 25569.0, 42807.0, 1694.0, 250958.0, 243170.0, 222528.0, 118459.0, 247034.0, 229752.0, 122252.0, 142034.0, 53820.0, 16435.0, 256349.0, 246166.0, 149139.0, 189970.0, 86378.0, 30682.0, 198112.0, 109512.0, 16507.0, 33637.0, 254029.0, 244590.0, 232612.0, 213300.0, 81207.0, 232426.0, 234243.0, 208985.0, 84767.0, 115731.0, 146715.0, 46647.0, 50698.0, 27758.0, 7560.0, 256986.0, 238130.0, 216464.0, 162397.0, 230413.0, 218418.0, 157612.0, 113608.0, 67048.0, 21693.0, 253944.0, 234123.0, 155282.0, 199740.0, 147216.0, 35002.0, 197894.0, 144313.0, 38214.0, 51820.0, 176586.0, 189479.0, 181808.0, 107358.0, 151681.0, 126327.0, 167640.0, 90893.0, 143047.0, 43532.0, 67132.0, 63427.0, 11407.0, 22485.0, 1717.0, 186807.0, 213718.0, 191497.0, 199529.0, 135841.0, 134280.0, 196178.0, 51392.0, 114165.0, 9509.0, 149358.0, 175518.0, 204810.0, 71399.0, 197516.0, 40276.0, 82771.0, 192718.0, 92191.0, 62854.0, 226304.0, 226244.0, 213017.0, 159359.0, 11947.0, 120670.0, 158448.0, 149983.0, 36653.0, 17195.0, 61718.0, 15052.0, 23246.0, 14682.0, 3388.0, 245894.0, 238335.0, 202625.0, 76409.0, 159110.0, 183938.0, 53447.0, 65742.0, 30626.0, 21335.0, 179420.0, 187716.0, 62059.0, 98145.0, 60329.0, 10485.0, 127619.0, 76205.0, 18625.0, 6798.0, 147902.0, 179417.0, 175121.0, 110418.0, 132186.0, 83688.0, 140944.0, 81196.0, 114440.0, 20681.0, 19595.0, 67565.0, 11736.0, 10309.0, 1985.0, 151206.0, 192416.0, 159252.0, 185329.0, 83943.0, 112067.0, 181384.0, 38791.0, 59649.0, 8346.0, 142594.0, 155211.0, 188113.0, 48304.0, 141275.0, 35059.0, 38435.0, 148935.0, 49371.0, 50983.0, 24055.0, 84000.0, 45502.0, 27930.0, 44684.0, 25746.0, 33662.0, 13593.0, 21261.0, 22438.0, 11827.0, 16614.0, 5030.0, 3515.0, 3635.0, 51893.0, 68914.0, 65756.0, 109323.0, 24117.0, 18907.0, 10451.0, 9431.0, 12637.0, 7081.0, 20718.0, 40014.0, 76197.0, 19922.0, 35947.0, 7370.0, 13833.0, 16107.0, 2095.0, 6274.0, 257935.0, 240254.0, 227800.0, 133576.0, 51105.0, 134133.0, 190928.0, 148108.0, 36884.0, 53342.0, 70563.0, 30784.0, 17297.0, 13015.0, 12015.0, 242475.0, 238730.0, 211805.0, 103048.0, 151140.0, 193154.0, 61581.0, 37450.0, 38530.0, 20228.0, 219504.0, 218853.0, 88321.0, 107225.0, 45631.0, 17806.0, 90165.0, 35815.0, 13192.0, 3673.0, 126701.0, 184969.0, 152936.0, 65943.0, 110102.0, 103591.0, 125329.0, 83836.0, 102286.0, 24384.0, 44208.0, 59534.0, 16866.0, 14570.0, 5400.0, 160444.0, 192233.0, 154567.0, 166157.0, 82656.0, 105452.0, 159647.0, 26670.0, 26426.0, 3347.0, 117591.0, 124655.0, 192171.0, 59455.0, 137688.0, 29608.0, 77477.0, 124661.0, 41642.0, 54390.0, 37871.0, 67224.0, 70347.0, 57013.0, 80258.0, 34481.0, 45230.0, 66588.0, 49501.0, 10576.0, 34099.0, 12293.0, 2978.0, 5869.0, 4445.0, 74398.0, 94673.0, 98124.0, 92499.0, 37463.0, 60830.0, 72684.0, 8810.0, 14057.0, 3763.0, 39405.0, 40570.0, 73626.0, 29570.0, 61608.0, 15563.0, 13985.0, 17530.0, 5785.0, 9321.0, 32665.0, 82883.0, 54828.0, 16909.0, 22351.0, 17711.0, 18953.0, 14475.0, 27614.0, 15973.0, 8494.0, 9057.0, 6764.0, 10811.0, 8294.0, 25709.0, 73636.0, 57463.0, 51439.0, 26856.0, 19288.0, 29934.0, 17904.0, 1880.0, 8711.0, 25299.0, 25096.0, 54677.0, 18110.0, 11127.0, 1750.0, 9344.0, 6392.0, 31104.0, 5852.0  },
                                                {280497.0, 246125.0, 226794.0, 134263.0, 14505.0, 257605.0, 220866.0, 117462.0, 19430.0, 110562.0, 82950.0, 36916.0, 10566.0, 1794.0, 1760.0, 268651.0, 224363.0, 206467.0, 28746.0, 237001.0, 175566.0, 33089.0, 47617.0, 26947.0, 2702.0, 258394.0, 239053.0, 39353.0, 118675.0, 31396.0, 25819.0, 148539.0, 31860.0, 6657.0, 3689.0, 276700.0, 229768.0, 211845.0, 172857.0, 29544.0, 229310.0, 216332.0, 171469.0, 19534.0, 138468.0, 131389.0, 51208.0, 17462.0, 6830.0, 10702.0, 269013.0, 215315.0, 211420.0, 62570.0, 219423.0, 209806.0, 77133.0, 132818.0, 68653.0, 4348.0, 245693.0, 214258.0, 95702.0, 148120.0, 68090.0, 19716.0, 153768.0, 93727.0, 18800.0, 15227.0, 234993.0, 243675.0, 218479.0, 129948.0, 20825.0, 139171.0, 184983.0, 136851.0, 42694.0, 90468.0, 115602.0, 24638.0, 7952.0, 10091.0, 1812.0, 229536.0, 223173.0, 183380.0, 55258.0, 149178.0, 178575.0, 67365.0, 106561.0, 53489.0, 19362.0, 198287.0, 191857.0, 87747.0, 120727.0, 62562.0, 31537.0, 92133.0, 53063.0, 23313.0, 10346.0, 120454.0, 165102.0, 128307.0, 56748.0, 20244.0, 93370.0, 82310.0, 56807.0, 14449.0, 40650.0, 67896.0, 15909.0, 22162.0, 7435.0, 7581.0, 140806.0, 162032.0, 93714.0, 28949.0, 71391.0, 88029.0, 35633.0, 54218.0, 31149.0, 5394.0, 75966.0, 95486.0, 25328.0, 42122.0, 33991.0, 8929.0, 37581.0, 25238.0, 12296.0, 33235.0, 59731.0, 38103.0, 44191.0, 21441.0, 8633.0, 30708.0, 22007.0, 3971.0, 22558.0, 5294.0, 10872.0, 1738.0, 3607.0, 3529.0, 2058.0, 45289.0, 50259.0, 18005.0, 34827.0, 13000.0, 15764.0, 37582.0, 10714.0, 7854.0, 4325.0, 25766.0, 24552.0, 46318.0, 5313.0, 30860.0, 7667.0, 6506.0, 14954.0, 11229.0, 17957.0, 253725.0, 230726.0, 181378.0, 76831.0, 17178.0, 135252.0, 186826.0, 109393.0, 21805.0, 40162.0, 53686.0, 27623.0, 9034.0, 8822.0, 8904.0, 239763.0, 217094.0, 152192.0, 39857.0, 133399.0, 135652.0, 37016.0, 29951.0, 19036.0, 4112.0, 153866.0, 128223.0, 48892.0, 50024.0, 21909.0, 3730.0, 38667.0, 16598.0, 12885.0, 17642.0, 207638.0, 259803.0, 200528.0, 120993.0, 24298.0, 118995.0, 160126.0, 111021.0, 30627.0, 49693.0, 77671.0, 17936.0, 9035.0, 17107.0, 3711.0, 189949.0, 226304.0, 183853.0, 42954.0, 141826.0, 146379.0, 63901.0, 65663.0, 34569.0, 6855.0, 155691.0, 164245.0, 81398.0, 84323.0, 56775.0, 19809.0, 75460.0, 62703.0, 9810.0, 10414.0, 108478.0, 164008.0, 144990.0, 50060.0, 16841.0, 56666.0, 110589.0, 76463.0, 24291.0, 27997.0, 63320.0, 28207.0, 17347.0, 10591.0, 5070.0, 117584.0, 148657.0, 125236.0, 42909.0, 76401.0, 119782.0, 38637.0, 58699.0, 34113.0, 2017.0, 71826.0, 121375.0, 55717.0, 74265.0, 54735.0, 13228.0, 57105.0, 28660.0, 12050.0, 17093.0, 43347.0, 54816.0, 46879.0, 27554.0, 16503.0, 15048.0, 34418.0, 28704.0, 6217.0, 25098.0, 25623.0, 7165.0, 1853.0, 8607.0, 2171.0, 42732.0, 48912.0, 27248.0, 29069.0, 32680.0, 11027.0, 32367.0, 25327.0, 11282.0, 6428.0, 18008.0, 46162.0, 50612.0, 9738.0, 49257.0, 3448.0, 18811.0, 41818.0, 20306.0, 40281.0, 50183.0, 75948.0, 70729.0, 58278.0, 25596.0, 98086.0, 106307.0, 52286.0, 19571.0, 60226.0, 75908.0, 16018.0, 12135.0, 9549.0, 5362.0, 59775.0, 88295.0, 46616.0, 9060.0, 76171.0, 61180.0, 40093.0, 86302.0, 19118.0, 11386.0, 35760.0, 50350.0, 21987.0, 32215.0, 28414.0, 13960.0, 25069.0, 11268.0, 5449.0, 18123.0, 87646.0, 54838.0, 47846.0, 20456.0, 17691.0, 108030.0, 103126.0, 22466.0, 11264.0, 132654.0, 126403.0, 26560.0, 14657.0, 16714.0, 5508.0, 72941.0, 64558.0, 66553.0, 18391.0, 122419.0, 58699.0, 23403.0, 115453.0, 24271.0, 5488.0, 49032.0, 53829.0, 34504.0, 62163.0, 37502.0, 13852.0, 52221.0, 25473.0, 7870.0, 16733.0, 40389.0, 54128.0, 25046.0, 26653.0, 12920.0, 66587.0, 44659.0, 17934.0, 20073.0, 53241.0, 31918.0, 8646.0, 1825.0, 1796.0, 5431.0, 44582.0, 25399.0, 10924.0, 7765.0, 59223.0, 24724.0, 25824.0, 35378.0, 13951.0, 5712.0, 24980.0, 10201.0, 11947.0, 8710.0, 14417.0, 8845.0, 17566.0, 13183.0, 3748.0, 15439.0, 11666.0, 17940.0, 26389.0, 11591.0, 7116.0, 11925.0, 11860.0, 18863.0, 11278.0, 3376.0, 8661.0, 3919.0, 3533.0, 3910.0, 1762.0, 3583.0, 13875.0, 16187.0, 3785.0, 11602.0, 2251.0, 15589.0, 8001.0, 1831.0, 2313.0, 9277.0, 7539.0, 1922.0, 8210.0, 1899.0, 4230.0, 8419.0, 3716.0, 1938.0, 9025.0, 17098.0, 12860.0, 16119.0, 19775.0, 23464.0, 24315.0, 14214.0, 13418.0, 14520.0, 10019.0, 6725.0, 5285.0, 1773.0, 5369.0, 4228.0, 7070.0, 9138.0, 7727.0, 5433.0, 6006.0, 12290.0, 5473.0, 2000.0, 4295.0, 4371.0, 1814.0, 17214.0, 10810.0, 6617.0, 3838.0, 3668.0, 1851.0, 2456.0, 5217.0, 11974.0, 2023.0, 18010.0, 18740.0, 13671.0, 5264.0, 8473.0, 5128.0, 2049.0, 7839.0, 1745.0, 5656.0, 2099.0, 1770.0, 1740.0, 5442.0, 1945.0, 14082.0, 1923.0, 1847.0, 3698.0, 6174.0, 5630.0, 1818.0, 5779.0, 5474.0, 6046.0, 7600.0, 6082.0, 7285.0, 11028.0, 3608.0, 7705.0, 3713.0, 9717.0, 4359.0, 274441.0, 236885.0, 227723.0, 170277.0, 28224.0, 245013.0, 230093.0, 158068.0, 19930.0, 98999.0, 117471.0, 71300.0, 18702.0, 8659.0, 4541.0, 259878.0, 236050.0, 202769.0, 53116.0, 231565.0, 189137.0, 64293.0, 110238.0, 64272.0, 6067.0, 242028.0, 227849.0, 77475.0, 157341.0, 71308.0, 14188.0, 121260.0, 36318.0, 7514.0, 31683.0, 253120.0, 242178.0, 216506.0, 185157.0, 53661.0, 201980.0, 207983.0, 190106.0, 47493.0, 144635.0, 136854.0, 66077.0, 39681.0, 27452.0, 3740.0, 244767.0, 230035.0, 209104.0, 85365.0, 213596.0, 207939.0, 98922.0, 153910.0, 81742.0, 12864.0, 229148.0, 221219.0, 120770.0, 154190.0, 85712.0, 47579.0, 150823.0, 67070.0, 20914.0, 41564.0, 199806.0, 236247.0, 208226.0, 136641.0, 16191.0, 126997.0, 171378.0, 128913.0, 51107.0, 79074.0, 93129.0, 20749.0, 21313.0, 7829.0, 9149.0, 209158.0, 226108.0, 188283.0, 83972.0, 146734.0, 165679.0, 54830.0, 79035.0, 61029.0, 9989.0, 165720.0, 189297.0, 55843.0, 94109.0, 45221.0, 48332.0, 111227.0, 51868.0, 27639.0, 31566.0, 81386.0, 123498.0, 90094.0, 45076.0, 55560.0, 55914.0, 60061.0, 40484.0, 42703.0, 32523.0, 18970.0, 11950.0, 11222.0, 1129.0, 1907.0, 98880.0, 112683.0, 80397.0, 79063.0, 71785.0, 61937.0, 60371.0, 20325.0, 30954.0, 7291.0, 63089.0, 65896.0, 104037.0, 22086.0, 91319.0, 4166.0, 10666.0, 50505.0, 32744.0, 23769.0, 175184.0, 187612.0, 154205.0, 99525.0, 18006.0, 98507.0, 152218.0, 66750.0, 21672.0, 41680.0, 61137.0, 40896.0, 10268.0, 13702.0, 1812.0, 187978.0, 212722.0, 167925.0, 52240.0, 118269.0, 157597.0, 76756.0, 56959.0, 27470.0, 5248.0, 118703.0, 151123.0, 74762.0, 89651.0, 61763.0, 27552.0, 45194.0, 33302.0, 22017.0, 15843.0, 168656.0, 233572.0, 204292.0, 84525.0, 26917.0, 96005.0, 145280.0, 128111.0, 43376.0, 72840.0, 90425.0, 21318.0, 15623.0, 7191.0, 7140.0, 181508.0, 194418.0, 186449.0, 51060.0, 124365.0, 159169.0, 77485.0, 59061.0, 54023.0, 11134.0, 137201.0, 163209.0, 65253.0, 105946.0, 74140.0, 9249.0, 79684.0, 59186.0, 33225.0, 13625.0, 49737.0, 110518.0, 79858.0, 51717.0, 42991.0, 47818.0, 61917.0, 43083.0, 36735.0, 18341.0, 40319.0, 2043.0, 13279.0, 3449.0, 1923.0, 65489.0, 106750.0, 74983.0, 81557.0, 57925.0, 45367.0, 49141.0, 26077.0, 19186.0, 2116.0, 53751.0, 71832.0, 106856.0, 33237.0, 55052.0, 10230.0, 33103.0, 52162.0, 35361.0, 21628.0, 55703.0, 85107.0, 64078.0, 45056.0, 26777.0, 90126.0, 130124.0, 38585.0, 21911.0, 82527.0, 89006.0, 3470.0, 10437.0, 1955.0, 1828.0, 67099.0, 72322.0, 69699.0, 18185.0, 81471.0, 92546.0, 30102.0, 69707.0, 30200.0, 5819.0, 48574.0, 42401.0, 19181.0, 32062.0, 48862.0, 6161.0, 22018.0, 13163.0, 15717.0, 11250.0, 72677.0, 45023.0, 48016.0, 28021.0, 21269.0, 90983.0, 83972.0, 33182.0, 26659.0, 80257.0, 61093.0, 7433.0, 6991.0, 15756.0, 4665.0, 48164.0, 36182.0, 45636.0, 11149.0, 61275.0, 48353.0, 14386.0, 47565.0, 17930.0, 2921.0, 38150.0, 25742.0, 44288.0, 27197.0, 15829.0, 9138.0, 23654.0, 27774.0, 14347.0, 11519.0, 5347.0, 8549.0, 17811.0, 16968.0, 5138.0, 8944.0, 12163.0, 5198.0, 9034.0, 14764.0, 3451.0, 2030.0, 2360.0, 3656.0, 5753.0, 13412.0, 3313.0, 1876.0, 10406.0, 3412.0, 4112.0, 15354.0, 8076.0, 1816.0, 7010.0, 5894.0, 12678.0, 5976.0, 3945.0, 9158.0, 2821.0, 3846.0, 7854.0, 3494.0, 13178.0, 242992.0, 237271.0, 227668.0, 151785.0, 27093.0, 122664.0, 186785.0, 142017.0, 44700.0, 61888.0, 62983.0, 17224.0, 3570.0, 14321.0, 7130.0, 239953.0, 234373.0, 193530.0, 58828.0, 184402.0, 168207.0, 66219.0, 101090.0, 55428.0, 3703.0, 206967.0, 167585.0, 65222.0, 92128.0, 47220.0, 14465.0, 113143.0, 52862.0, 15293.0, 8046.0, 194151.0, 218543.0, 208242.0, 141966.0, 37302.0, 91479.0, 141547.0, 113622.0, 28519.0, 68701.0, 95804.0, 23782.0, 1757.0, 11477.0, 3244.0, 214845.0, 214240.0, 189338.0, 52849.0, 140417.0, 162936.0, 67519.0, 59556.0, 61626.0, 2099.0, 174581.0, 179248.0, 69817.0, 118044.0, 53618.0, 14241.0, 86279.0, 25887.0, 12092.0, 5555.0, 72898.0, 125407.0, 78214.0, 45523.0, 59161.0, 27427.0, 69224.0, 52692.0, 52888.0, 23711.0, 22336.0, 22677.0, 7940.0, 1814.0, 5499.0, 79195.0, 110974.0, 87589.0, 88032.0, 54235.0, 55273.0, 87403.0, 26035.0, 31481.0, 6282.0, 68698.0, 83359.0, 106555.0, 26837.0, 91819.0, 15545.0, 34574.0, 81357.0, 13702.0, 42799.0, 71061.0, 116305.0, 100823.0, 71603.0, 23521.0, 26503.0, 59930.0, 71177.0, 16536.0, 34564.0, 30924.0, 21373.0, 25569.0, 8318.0, 6945.0, 98235.0, 116304.0, 111416.0, 22826.0, 55546.0, 80048.0, 36779.0, 25540.0, 30223.0, 8968.0, 61279.0, 71409.0, 46643.0, 45637.0, 21878.0, 7284.0, 18133.0, 22479.0, 11513.0, 16485.0, 51896.0, 144879.0, 103687.0, 34688.0, 34835.0, 33493.0, 30859.0, 47809.0, 19971.0, 24710.0, 22154.0, 8819.0, 19233.0, 1938.0, 10383.0, 31765.0, 104540.0, 66135.0, 50014.0, 56771.0, 41075.0, 35630.0, 15813.0, 4518.0, 6348.0, 42715.0, 55611.0, 80977.0, 36615.0, 46101.0, 3751.0, 14170.0, 60224.0, 41593.0, 14037.0, 35303.0, 35793.0, 31952.0, 9143.0, 17699.0, 38290.0, 28101.0, 25036.0, 1687.0, 46452.0, 25452.0, 5291.0, 12235.0, 5487.0, 4001.0, 24407.0, 15436.0, 4541.0, 17344.0, 33322.0, 11215.0, 13629.0, 19586.0, 3464.0, 6161.0, 16159.0, 17380.0, 25901.0, 18844.0, 5797.0, 3741.0, 5024.0, 5511.0, 16004.0, 9998.0, 114761.0, 105308.0, 124977.0, 63628.0, 5184.0, 53709.0, 65218.0, 54081.0, 13021.0, 18815.0, 46583.0, 15258.0, 3677.0, 4058.0, 2262.0, 132698.0, 109313.0, 91461.0, 14523.0, 54441.0, 88890.0, 29226.0, 15139.0, 19307.0, 15813.0, 74729.0, 74339.0, 11329.0, 43519.0, 21092.0, 7385.0, 32960.0, 14184.0, 11824.0, 7880.0, 79665.0, 84366.0, 78888.0, 39920.0, 27962.0, 19854.0, 41633.0, 26974.0, 23026.0, 19065.0, 13941.0, 9797.0, 2931.0, 13140.0, 5771.0, 50316.0, 68090.0, 73080.0, 61857.0, 27620.0, 43724.0, 25332.0, 3473.0, 30040.0, 1893.0, 66629.0, 35023.0, 89823.0, 7740.0, 39225.0, 9908.0, 2223.0, 48939.0, 21717.0, 34385.0, 16392.0, 30002.0, 41848.0, 26137.0, 26986.0, 11174.0, 27340.0, 23550.0, 26688.0, 3432.0, 4093.0, 724.0, 5992.0, 6576.0, 4054.0, 27816.0, 31187.0, 23870.0, 21047.0, 7874.0, 16372.0, 35591.0, 12033.0, 5931.0, 3780.0, 16498.0, 22371.0, 27345.0, 8330.0, 6059.0, 4714.0, 7063.0, 3948.0, 7467.0, 9266.0, 13678.0, 15746.0, 23806.0, 8748.0, 16245.0, 2049.0, 6628.0, 5584.0, 20735.0, 3444.0, 8725.0, 11545.0, 3719.0, 3509.0, 3626.0, 13442.0, 24783.0, 2498.0, 24989.0, 11619.0, 6088.0, 17653.0, 9061.0, 3569.0, 5789.0, 10347.0, 18294.0, 4385.0, 1707.0, 4545.0, 5765.0, 4281.0, 6397.0, 9467.0, 2837.0  },
                                                {209269.0, 173608.0, 164749.0, 145590.0, 21922.0, 173966.0, 201302.0, 151022.0, 26531.0, 177603.0, 133882.0, 38395.0, 19172.0, 6753.0, 3739.0, 198408.0, 165707.0, 166030.0, 71509.0, 164948.0, 184835.0, 76424.0, 129305.0, 59728.0, 3014.0, 170223.0, 160362.0, 90022.0, 165880.0, 90398.0, 34714.0, 176835.0, 77199.0, 10162.0, 26795.0, 194351.0, 166151.0, 157986.0, 146500.0, 37237.0, 168153.0, 151396.0, 151119.0, 57111.0, 147597.0, 148197.0, 56069.0, 46525.0, 17899.0, 9051.0, 176008.0, 147435.0, 155666.0, 96295.0, 167792.0, 152961.0, 116304.0, 145838.0, 73041.0, 18605.0, 162826.0, 156179.0, 127576.0, 162997.0, 114408.0, 30489.0, 159205.0, 101619.0, 47667.0, 46660.0, 174277.0, 171078.0, 158114.0, 140594.0, 36661.0, 170439.0, 161423.0, 149728.0, 60973.0, 117851.0, 120014.0, 58510.0, 46935.0, 30199.0, 2531.0, 173806.0, 159960.0, 151984.0, 97786.0, 161046.0, 153315.0, 115415.0, 120622.0, 78448.0, 30202.0, 178297.0, 161655.0, 121707.0, 161270.0, 115548.0, 46162.0, 162677.0, 115379.0, 53026.0, 51226.0, 154631.0, 151277.0, 139545.0, 82372.0, 9850.0, 128335.0, 124106.0, 88788.0, 41234.0, 57771.0, 51423.0, 9263.0, 25393.0, 7088.0, 2518.0, 166040.0, 157683.0, 135035.0, 53507.0, 133568.0, 133974.0, 64021.0, 75520.0, 30171.0, 12260.0, 146740.0, 138070.0, 90624.0, 102147.0, 98260.0, 30019.0, 96136.0, 50809.0, 34536.0, 22440.0, 36684.0, 45097.0, 40098.0, 19652.0, 14802.0, 18047.0, 38326.0, 14713.0, 30740.0, 17664.0, 11091.0, 12785.0, 9748.0, 2106.0, 3214.0, 54698.0, 72996.0, 38657.0, 54896.0, 46413.0, 49139.0, 77734.0, 22631.0, 16912.0, 19114.0, 54836.0, 61660.0, 114374.0, 33747.0, 81798.0, 23053.0, 14961.0, 79837.0, 70181.0, 43095.0, 187285.0, 177056.0, 156787.0, 116883.0, 15444.0, 172650.0, 145582.0, 124435.0, 35940.0, 86590.0, 95098.0, 17889.0, 1351.0, 8382.0, 1252.0, 182229.0, 154456.0, 157165.0, 43475.0, 150871.0, 159244.0, 62218.0, 39091.0, 24850.0, 1409.0, 174483.0, 161939.0, 60431.0, 133140.0, 55491.0, 11474.0, 124341.0, 27134.0, 18136.0, 25764.0, 207846.0, 212508.0, 178478.0, 121916.0, 28403.0, 165498.0, 155845.0, 132729.0, 42372.0, 67589.0, 71082.0, 38753.0, 20429.0, 13013.0, 1712.0, 176366.0, 176741.0, 148917.0, 73585.0, 160489.0, 152513.0, 96389.0, 96916.0, 55346.0, 11197.0, 169924.0, 157839.0, 92906.0, 127960.0, 83767.0, 30418.0, 127535.0, 75652.0, 44102.0, 28724.0, 131356.0, 158650.0, 133347.0, 87531.0, 20006.0, 112772.0, 129873.0, 104878.0, 30318.0, 49071.0, 57947.0, 22740.0, 10410.0, 6966.0, 1261.0, 162536.0, 156919.0, 139668.0, 52566.0, 129841.0, 131322.0, 71227.0, 58045.0, 50388.0, 3955.0, 152380.0, 135965.0, 76622.0, 80766.0, 77203.0, 12575.0, 99572.0, 56004.0, 28355.0, 18078.0, 40524.0, 96842.0, 71431.0, 30856.0, 17885.0, 44652.0, 52198.0, 31987.0, 34172.0, 22089.0, 13645.0, 16722.0, 4510.0, 2564.0, 1280.0, 97684.0, 90398.0, 51203.0, 54590.0, 68771.0, 63816.0, 82288.0, 28498.0, 20361.0, 6374.0, 59187.0, 55985.0, 85053.0, 49927.0, 89459.0, 15726.0, 30418.0, 80721.0, 34381.0, 47588.0, 119834.0, 166581.0, 115892.0, 41388.0, 8674.0, 45353.0, 67996.0, 28833.0, 9893.0, 6144.0, 25584.0, 10245.0, 1266.0, 1273.0, 1241.0, 113680.0, 149420.0, 100529.0, 18526.0, 56058.0, 59199.0, 20762.0, 25239.0, 18680.0, 1230.0, 79212.0, 76189.0, 33462.0, 19511.0, 26759.0, 12378.0, 29208.0, 21366.0, 10012.0, 2139.0, 57134.0, 115344.0, 83305.0, 52620.0, 8313.0, 32478.0, 62597.0, 46833.0, 23346.0, 11474.0, 28094.0, 2467.0, 1497.0, 2780.0, 3561.0, 90231.0, 136227.0, 100935.0, 27212.0, 38753.0, 73866.0, 40342.0, 21987.0, 21106.0, 3022.0, 87526.0, 100903.0, 35195.0, 33488.0, 27151.0, 2538.0, 40768.0, 18679.0, 11513.0, 13397.0, 51153.0, 59840.0, 40186.0, 9532.0, 14429.0, 27495.0, 38484.0, 18850.0, 22826.0, 15235.0, 4922.0, 3892.0, 1312.0, 5065.0, 5660.0, 56757.0, 62281.0, 39669.0, 53114.0, 32930.0, 33175.0, 34047.0, 8137.0, 2670.0, 1355.0, 38730.0, 31197.0, 75440.0, 20304.0, 50241.0, 3056.0, 11311.0, 61132.0, 24070.0, 18853.0, 70190.0, 43959.0, 12116.0, 6572.0, 3801.0, 6442.0, 10409.0, 11984.0, 5669.0, 5960.0, 2505.0, 1302.0, 1251.0, 3384.0, 1253.0, 9303.0, 24759.0, 6058.0, 1303.0, 7076.0, 7444.0, 2481.0, 1236.0, 1252.0, 1214.0, 1287.0, 9457.0, 1236.0, 7138.0, 3770.0, 2898.0, 6313.0, 2468.0, 2457.0, 0.0, 25279.0, 15735.0, 17137.0, 17381.0, 13582.0, 6070.0, 3745.0, 6139.0, 3513.0, 2546.0, 5939.0, 6218.0, 3907.0, 1242.0, 2558.0, 8730.0, 15566.0, 3086.0, 25955.0, 5665.0, 12129.0, 7420.0, 1245.0, 6700.0, 2479.0, 3361.0, 2724.0, 17878.0, 1251.0, 6947.0, 5685.0, 11797.0, 28605.0, 4365.0, 9241.0, 24178.0, 170917.0, 50686.0, 1920.0, 1302.0, 4709.0, 6185.0, 1308.0, 1261.0, 2724.0, 1272.0, 1239.0, 3796.0, 1257.0, 2543.0, 1243.0, 2073.0, 3732.0, 3134.0, 3876.0, 1291.0, 2426.0, 3136.0, 1429.0, 3636.0, 7325.0, 2443.0, 2839.0, 1262.0, 2941.0, 2408.0, 5276.0, 2418.0, 4293.0, 1754.0, 198146.0, 170611.0, 164016.0, 158310.0, 59418.0, 174558.0, 185744.0, 154595.0, 47289.0, 161640.0, 135824.0, 41493.0, 30999.0, 16443.0, 2688.0, 189280.0, 158032.0, 157457.0, 85095.0, 155282.0, 166527.0, 95958.0, 148398.0, 47743.0, 3961.0, 166176.0, 155081.0, 113926.0, 163804.0, 96477.0, 36540.0, 163924.0, 97577.0, 21664.0, 43115.0, 181743.0, 165249.0, 155262.0, 157644.0, 49211.0, 173767.0, 160475.0, 144099.0, 89166.0, 157663.0, 146543.0, 51666.0, 21471.0, 20875.0, 6465.0, 185334.0, 153599.0, 150482.0, 135062.0, 169451.0, 152624.0, 123218.0, 147594.0, 89588.0, 25291.0, 165056.0, 162497.0, 149056.0, 153655.0, 120391.0, 36739.0, 167083.0, 147895.0, 58853.0, 34078.0, 173496.0, 161397.0, 150050.0, 131238.0, 33824.0, 171345.0, 161418.0, 141916.0, 52825.0, 86298.0, 119403.0, 31703.0, 50457.0, 22078.0, 1332.0, 170830.0, 163056.0, 145558.0, 101885.0, 155494.0, 151689.0, 95434.0, 101662.0, 88970.0, 17447.0, 161395.0, 164981.0, 133962.0, 150570.0, 123605.0, 35706.0, 150115.0, 110161.0, 49747.0, 57506.0, 96237.0, 134331.0, 111466.0, 53345.0, 63256.0, 56181.0, 110007.0, 66366.0, 82230.0, 49148.0, 40146.0, 46181.0, 18306.0, 7300.0, 1244.0, 141840.0, 134618.0, 124935.0, 130933.0, 97033.0, 87579.0, 127019.0, 56266.0, 108948.0, 20563.0, 121990.0, 112527.0, 141126.0, 52146.0, 138104.0, 37966.0, 68533.0, 134671.0, 99150.0, 56960.0, 188701.0, 169607.0, 159795.0, 135507.0, 23071.0, 156535.0, 159825.0, 135017.0, 44931.0, 69235.0, 67728.0, 18828.0, 9634.0, 3950.0, 3204.0, 179605.0, 165008.0, 151711.0, 64212.0, 149150.0, 155322.0, 65971.0, 62297.0, 35149.0, 2463.0, 167906.0, 156843.0, 94382.0, 113315.0, 71895.0, 14973.0, 136963.0, 49357.0, 27228.0, 13092.0, 202193.0, 190664.0, 151908.0, 128834.0, 42768.0, 139676.0, 146947.0, 124148.0, 36466.0, 65542.0, 68965.0, 25231.0, 13252.0, 19756.0, 1241.0, 180545.0, 174318.0, 148958.0, 81261.0, 147201.0, 154436.0, 93342.0, 95743.0, 51740.0, 19213.0, 161789.0, 162443.0, 106339.0, 134113.0, 92057.0, 23356.0, 113398.0, 74485.0, 42366.0, 21803.0, 98546.0, 126197.0, 110499.0, 69960.0, 94488.0, 81073.0, 96931.0, 52761.0, 96550.0, 30099.0, 33902.0, 25229.0, 8820.0, 6721.0, 1350.0, 135146.0, 137525.0, 118094.0, 133491.0, 98346.0, 94088.0, 133676.0, 28537.0, 42967.0, 6351.0, 112565.0, 114040.0, 132976.0, 59892.0, 128826.0, 18278.0, 73314.0, 133495.0, 89925.0, 73892.0, 85405.0, 147041.0, 113584.0, 55901.0, 15868.0, 47465.0, 83367.0, 39204.0, 6308.0, 20443.0, 15350.0, 6187.0, 1318.0, 3786.0, 5342.0, 100299.0, 138722.0, 111363.0, 18694.0, 63236.0, 69377.0, 24639.0, 15860.0, 6467.0, 2763.0, 78287.0, 80638.0, 55745.0, 34360.0, 26692.0, 3765.0, 31764.0, 11463.0, 8358.0, 4889.0, 62668.0, 74435.0, 62734.0, 31534.0, 42017.0, 8791.0, 39853.0, 30235.0, 21456.0, 14496.0, 19801.0, 10960.0, 8577.0, 5544.0, 4433.0, 65901.0, 82965.0, 74838.0, 91883.0, 42000.0, 54228.0, 87994.0, 21966.0, 10233.0, 11681.0, 45884.0, 52442.0, 91499.0, 24384.0, 55861.0, 14599.0, 28284.0, 48220.0, 23071.0, 11338.0, 50678.0, 9283.0, 15208.0, 2433.0, 1316.0, 6666.0, 5048.0, 5154.0, 3740.0, 7518.0, 1556.0, 1275.0, 2579.0, 2527.0, 2464.0, 2463.0, 21325.0, 3998.0, 16781.0, 5159.0, 2505.0, 1343.0, 4190.0, 2582.0, 3210.0, 14267.0, 8137.0, 18991.0, 3016.0, 1747.0, 1769.0, 16503.0, 6107.0, 7821.0, 3501.0, 196660.0, 173315.0, 162299.0, 147674.0, 28529.0, 179995.0, 172878.0, 147331.0, 36857.0, 133435.0, 95097.0, 23230.0, 8163.0, 12640.0, 6305.0, 177092.0, 158688.0, 163610.0, 75551.0, 170668.0, 162785.0, 99870.0, 99021.0, 26937.0, 4318.0, 181879.0, 160352.0, 115007.0, 148925.0, 65734.0, 20910.0, 144029.0, 104054.0, 38110.0, 15030.0, 179512.0, 167304.0, 159038.0, 141674.0, 44283.0, 175527.0, 172439.0, 135598.0, 59262.0, 94788.0, 94216.0, 28165.0, 24546.0, 5609.0, 1236.0, 177049.0, 148973.0, 149789.0, 110490.0, 158304.0, 149372.0, 100520.0, 92945.0, 58384.0, 7981.0, 177824.0, 159204.0, 117220.0, 142916.0, 89503.0, 26846.0, 150334.0, 94151.0, 43138.0, 38381.0, 128460.0, 149398.0, 123563.0, 80104.0, 74482.0, 69750.0, 83359.0, 68215.0, 117231.0, 42206.0, 38845.0, 40456.0, 12580.0, 4010.0, 1245.0, 137560.0, 152156.0, 130156.0, 126897.0, 93795.0, 92193.0, 134818.0, 38087.0, 90657.0, 7085.0, 126651.0, 132743.0, 139520.0, 71651.0, 146578.0, 40153.0, 59525.0, 141098.0, 74189.0, 79444.0, 135901.0, 162240.0, 140226.0, 96149.0, 24182.0, 124431.0, 115786.0, 105618.0, 29540.0, 16079.0, 24479.0, 15231.0, 2665.0, 2688.0, 2872.0, 168769.0, 159890.0, 136150.0, 50566.0, 118016.0, 117341.0, 53584.0, 41041.0, 15209.0, 3890.0, 138259.0, 142247.0, 83294.0, 62444.0, 31168.0, 5232.0, 75363.0, 58427.0, 3735.0, 15126.0, 86873.0, 139090.0, 133569.0, 64277.0, 102093.0, 67718.0, 92183.0, 43481.0, 80672.0, 29983.0, 18149.0, 17718.0, 15718.0, 10208.0, 1276.0, 131172.0, 142264.0, 118280.0, 127522.0, 66349.0, 98417.0, 137083.0, 23661.0, 56130.0, 1765.0, 92385.0, 115602.0, 139806.0, 38391.0, 127760.0, 7630.0, 36998.0, 126780.0, 56023.0, 51382.0, 22616.0, 45871.0, 44835.0, 30615.0, 33805.0, 6716.0, 22301.0, 21415.0, 24415.0, 18593.0, 14821.0, 1443.0, 3198.0, 2647.0, 4988.0, 27541.0, 43536.0, 35278.0, 53233.0, 13727.0, 32561.0, 21420.0, 4988.0, 8205.0, 3761.0, 31309.0, 33205.0, 53836.0, 9297.0, 26527.0, 2621.0, 21964.0, 25260.0, 10670.0, 5390.0, 176566.0, 182131.0, 142008.0, 111797.0, 25160.0, 162291.0, 151655.0, 104516.0, 20033.0, 88703.0, 46204.0, 4029.0, 4009.0, 4354.0, 1470.0, 178991.0, 156748.0, 151430.0, 49036.0, 103597.0, 135491.0, 55233.0, 35003.0, 8036.0, 6469.0, 141732.0, 152375.0, 67914.0, 73340.0, 26416.0, 27443.0, 72142.0, 18385.0, 11105.0, 4915.0, 82367.0, 125789.0, 96268.0, 36975.0, 79760.0, 41613.0, 81379.0, 57869.0, 94284.0, 10715.0, 24533.0, 50756.0, 4354.0, 3510.0, 7801.0, 122099.0, 140177.0, 103642.0, 135658.0, 58258.0, 90413.0, 118297.0, 19333.0, 71963.0, 3159.0, 76620.0, 106336.0, 141844.0, 45234.0, 120196.0, 10676.0, 32637.0, 117920.0, 27025.0, 48544.0, 19817.0, 71678.0, 60523.0, 16432.0, 21435.0, 23551.0, 17391.0, 21230.0, 37887.0, 10425.0, 12301.0, 11086.0, 1282.0, 2713.0, 5566.0, 61584.0, 79842.0, 31945.0, 92115.0, 13987.0, 35101.0, 53827.0, 6270.0, 18272.0, 7495.0, 44126.0, 47762.0, 96609.0, 16238.0, 32629.0, 5970.0, 10657.0, 53761.0, 3822.0, 3694.0, 34410.0, 58294.0, 40094.0, 20005.0, 24296.0, 23320.0, 27165.0, 7495.0, 3180.0, 1484.0, 8939.0, 21470.0, 5254.0, 1306.0, 3932.0, 32346.0, 45011.0, 40410.0, 26354.0, 11136.0, 16115.0, 45771.0, 8823.0, 6244.0, 3675.0, 20784.0, 37262.0, 64900.0, 5349.0, 48697.0, 4825.0, 10184.0, 23346.0, 2363.0, 7316.0  } };
    
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
    int zeros[3];
    
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
        if(zeros[tp] > 0){
            while(scoreTable[tp][API_calculateState(objectiveState[tp][0],objectiveState[tp][1],objectiveState[tp][2],objectiveState[tp][3],objectiveState[tp][4],objectiveState[tp][5])] != 0.0){
                objective_next(tp);
            }
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

                // printsv("TASK0 Objective state: ", API_calculateState(objectiveState[0][0],objectiveState[0][1],objectiveState[0][2],objectiveState[0][3],objectiveState[0][4],objectiveState[0][5]));
                // // printsvsv("a: ", objectiveState[0][0], "da: ", objectiveState[0][3]);
                // // printsvsv("b: ", objectiveState[0][1], "db: ", objectiveState[0][4]);
                // // printsvsv("c: ", objectiveState[0][2], "dc: ", objectiveState[0][5]);
                // printsv("TASK1 Objective state: ", API_calculateState(objectiveState[1][0],objectiveState[1][1],objectiveState[1][2],objectiveState[1][3],objectiveState[1][4],objectiveState[1][5]));
                // printsv("TASK2 Objective state: ", API_calculateState(objectiveState[2][0],objectiveState[2][1],objectiveState[2][2],objectiveState[2][3],objectiveState[2][4],objectiveState[2][5]));

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
                        if( currentState[2][0] < objectiveState[2][0] ){
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
                        else if( currentState[0][0] < objectiveState[0][0] ){
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
                    }

                    else if (applications[app].taskType[task] == 1){
                        if( currentState[2][1] < objectiveState[2][1] ){
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
                        else if( currentState[0][1] < objectiveState[0][1] ){
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
                    }

                    else if (applications[app].taskType[task] == 2){
                        if( currentState[2][2] < objectiveState[2][2] ){
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
                        else if( currentState[0][2] < objectiveState[0][2] ){
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
                                zeros[tp]--;
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
                                zeros[tp]--;
                                if(zeros[tp] > 0){
                                    while(scoreTable[tp][API_calculateState(objectiveState[tp][0],objectiveState[tp][1],objectiveState[tp][2],objectiveState[tp][3],objectiveState[tp][4],objectiveState[tp][5])] != 0.0){
                                        objective_next(tp);
                                    }
                                } else{
                                    objective_next(tp);
                                }
                            }
                            else if(addr == 0x508){
                                tp = 2;
                                zeros[tp]--;
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
    float scoreTable[N_TASKTYPE][N_STATES] = {  {236292.0, 248357.0, 227885.0, 205551.0, 35263.0, 251655.0, 240505.0, 222500.0, 23046.0, 220253.0, 147869.0, 13309.0, 33116.0, 5080.0, 8534.0, 238412.0, 239402.0, 226442.0, 96557.0, 250195.0, 244360.0, 50702.0, 160435.0, 24333.0, 13896.0, 244406.0, 239284.0, 112669.0, 240564.0, 96661.0, 4014.0, 239470.0, 128253.0, 15408.0, 21731.0, 245133.0, 241008.0, 217654.0, 219962.0, 82559.0, 251981.0, 236918.0, 217991.0, 68630.0, 187498.0, 191526.0, 51994.0, 66669.0, 8661.0, 7588.0, 246316.0, 234550.0, 217917.0, 152508.0, 238590.0, 225524.0, 152672.0, 191727.0, 80049.0, 7917.0, 243837.0, 233245.0, 170706.0, 221499.0, 149007.0, 33727.0, 241905.0, 135179.0, 46961.0, 39403.0, 268958.0, 240140.0, 218268.0, 190171.0, 40251.0, 245034.0, 238125.0, 208986.0, 76574.0, 180461.0, 120406.0, 21042.0, 35589.0, 8725.0, 1859.0, 253386.0, 229199.0, 219443.0, 131272.0, 246744.0, 220435.0, 114119.0, 139666.0, 40117.0, 8719.0, 238334.0, 233862.0, 157026.0, 228552.0, 130477.0, 20860.0, 209763.0, 131107.0, 47621.0, 34824.0, 210077.0, 216874.0, 202295.0, 109110.0, 26997.0, 137167.0, 147113.0, 103667.0, 34489.0, 38730.0, 37064.0, 28336.0, 27773.0, 5030.0, 1687.0, 229594.0, 213373.0, 183117.0, 74081.0, 150652.0, 150477.0, 78444.0, 40228.0, 35847.0, 6910.0, 203359.0, 176986.0, 100665.0, 99709.0, 84296.0, 32897.0, 101353.0, 104682.0, 35928.0, 23427.0, 81771.0, 90069.0, 63580.0, 35794.0, 14908.0, 40462.0, 42662.0, 27101.0, 35040.0, 20784.0, 22126.0, 14757.0, 1787.0, 5026.0, 5096.0, 66173.0, 79192.0, 54459.0, 86201.0, 31364.0, 33445.0, 108666.0, 8000.0, 12790.0, 1730.0, 70033.0, 58696.0, 150881.0, 12372.0, 103173.0, 22347.0, 9624.0, 72761.0, 27878.0, 11057.0, 252000.0, 244603.0, 238241.0, 179210.0, 50181.0, 240065.0, 221874.0, 175905.0, 34109.0, 99799.0, 87189.0, 15513.0, 10863.0, 1722.0, 3245.0, 252746.0, 243883.0, 215221.0, 83448.0, 221353.0, 208227.0, 58543.0, 58784.0, 9244.0, 10189.0, 250433.0, 221066.0, 67763.0, 169335.0, 59936.0, 9213.0, 137196.0, 44434.0, 22618.0, 9600.0, 258667.0, 252458.0, 232288.0, 172161.0, 67580.0, 183137.0, 206542.0, 179529.0, 61127.0, 68503.0, 74849.0, 25597.0, 23006.0, 7930.0, 1666.0, 258944.0, 243985.0, 223832.0, 97676.0, 210446.0, 217722.0, 87416.0, 102492.0, 31881.0, 7774.0, 238572.0, 234725.0, 143692.0, 184067.0, 85602.0, 22070.0, 154775.0, 88951.0, 45165.0, 37440.0, 213010.0, 219381.0, 194525.0, 94142.0, 45163.0, 130227.0, 167644.0, 140827.0, 42256.0, 43098.0, 47705.0, 35016.0, 12233.0, 14236.0, 2669.0, 195075.0, 225068.0, 190444.0, 82616.0, 153072.0, 162051.0, 83795.0, 43594.0, 37752.0, 8662.0, 206625.0, 184921.0, 97893.0, 107540.0, 68371.0, 16044.0, 86457.0, 100302.0, 41059.0, 14226.0, 95404.0, 97306.0, 79454.0, 53817.0, 62865.0, 42344.0, 70996.0, 55732.0, 43843.0, 12669.0, 12979.0, 17353.0, 23710.0, 5412.0, 1860.0, 77881.0, 141797.0, 68649.0, 123025.0, 64323.0, 48002.0, 73996.0, 12030.0, 16619.0, 2465.0, 74211.0, 104871.0, 163161.0, 39136.0, 100777.0, 7155.0, 35149.0, 102013.0, 36287.0, 22786.0, 91128.0, 164122.0, 131695.0, 60361.0, 15275.0, 52303.0, 87146.0, 40305.0, 20017.0, 14703.0, 10503.0, 4143.0, 10126.0, 2475.0, 2651.0, 150958.0, 162983.0, 140133.0, 25431.0, 66984.0, 86991.0, 33423.0, 17019.0, 15053.0, 7150.0, 64243.0, 78050.0, 28672.0, 56127.0, 33698.0, 6893.0, 26004.0, 15214.0, 4419.0, 8899.0, 86687.0, 127252.0, 140329.0, 75378.0, 21505.0, 57595.0, 90124.0, 85725.0, 24761.0, 14640.0, 31319.0, 27054.0, 13594.0, 1651.0, 1702.0, 129286.0, 155744.0, 134706.0, 40517.0, 63950.0, 101420.0, 27785.0, 11389.0, 16166.0, 6769.0, 108059.0, 106282.0, 51684.0, 38278.0, 44388.0, 3943.0, 47182.0, 28704.0, 11150.0, 3140.0, 61728.0, 76840.0, 71484.0, 42661.0, 16070.0, 18204.0, 45572.0, 41191.0, 34489.0, 18676.0, 11977.0, 3707.0, 6731.0, 1880.0, 1763.0, 42274.0, 72535.0, 44987.0, 66490.0, 28193.0, 39562.0, 36532.0, 3622.0, 19136.0, 19968.0, 45913.0, 53161.0, 83591.0, 7150.0, 33287.0, 12810.0, 14421.0, 31633.0, 11908.0, 4257.0, 26559.0, 39223.0, 23314.0, 16134.0, 12969.0, 8849.0, 11811.0, 14592.0, 2029.0, 3173.0, 14593.0, 5589.0, 6785.0, 3724.0, 3803.0, 19656.0, 29648.0, 20927.0, 4779.0, 12614.0, 9054.0, 3341.0, 5788.0, 3661.0, 8719.0, 18961.0, 14072.0, 8876.0, 4767.0, 3465.0, 5731.0, 8395.0, 12603.0, 5092.0, 2483.0, 36143.0, 45185.0, 32437.0, 29848.0, 23333.0, 31358.0, 34008.0, 20768.0, 10854.0, 8975.0, 13555.0, 1660.0, 4666.0, 5567.0, 9341.0, 8849.0, 22681.0, 13728.0, 3184.0, 10774.0, 1672.0, 7973.0, 2097.0, 8630.0, 2729.0, 11224.0, 10993.0, 10477.0, 5017.0, 4648.0, 4694.0, 6923.0, 12274.0, 5614.0, 12065.0, 34023.0, 14416.0, 29034.0, 23709.0, 6232.0, 15314.0, 6680.0, 3939.0, 15128.0, 6944.0, 3462.0, 5110.0, 5226.0, 1720.0, 5134.0, 8945.0, 4404.0, 5355.0, 3215.0, 5329.0, 1685.0, 4469.0, 2773.0, 1872.0, 7891.0, 5423.0, 1827.0, 3154.0, 5213.0, 3341.0, 4687.0, 5445.0, 4408.0, 1759.0, 2177.0, 244731.0, 247341.0, 226241.0, 234283.0, 62129.0, 252642.0, 233345.0, 219103.0, 46980.0, 179445.0, 188190.0, 25511.0, 13606.0, 9072.0, 6292.0, 241445.0, 242453.0, 218098.0, 125881.0, 247324.0, 223018.0, 138941.0, 135721.0, 41723.0, 3451.0, 246447.0, 246003.0, 169494.0, 224422.0, 139239.0, 34132.0, 247398.0, 132661.0, 52965.0, 40291.0, 244378.0, 244460.0, 224227.0, 205940.0, 111100.0, 247481.0, 226696.0, 213260.0, 96249.0, 181174.0, 194107.0, 58301.0, 23663.0, 20408.0, 4804.0, 241740.0, 235070.0, 214448.0, 181826.0, 245456.0, 218045.0, 167383.0, 180207.0, 91229.0, 27503.0, 247900.0, 234991.0, 209624.0, 233641.0, 156879.0, 28834.0, 238874.0, 171420.0, 62000.0, 62507.0, 252878.0, 243274.0, 221813.0, 194868.0, 69626.0, 233543.0, 220130.0, 197552.0, 81309.0, 143848.0, 116408.0, 37905.0, 26495.0, 28639.0, 1648.0, 249487.0, 227918.0, 205711.0, 142396.0, 217608.0, 221384.0, 145174.0, 90593.0, 91664.0, 5735.0, 252198.0, 222946.0, 175966.0, 210036.0, 132082.0, 37896.0, 184777.0, 124833.0, 59435.0, 56445.0, 149583.0, 188367.0, 143121.0, 98915.0, 72072.0, 86486.0, 116847.0, 71064.0, 86746.0, 52973.0, 15701.0, 61327.0, 6901.0, 6016.0, 4429.0, 170978.0, 200025.0, 154134.0, 193381.0, 57799.0, 117306.0, 190848.0, 12538.0, 90886.0, 16926.0, 161858.0, 154562.0, 192145.0, 66450.0, 184838.0, 51029.0, 72878.0, 184165.0, 63938.0, 65814.0, 254186.0, 250849.0, 226366.0, 196427.0, 41166.0, 208308.0, 227387.0, 188222.0, 68910.0, 89974.0, 73770.0, 36744.0, 32525.0, 4892.0, 3484.0, 257709.0, 239273.0, 231654.0, 78577.0, 221501.0, 208976.0, 82605.0, 41965.0, 30933.0, 15314.0, 238111.0, 237163.0, 113961.0, 157472.0, 102445.0, 29680.0, 169278.0, 81484.0, 23334.0, 9082.0, 256405.0, 235642.0, 212180.0, 177473.0, 45986.0, 189503.0, 203438.0, 177123.0, 78197.0, 79497.0, 83178.0, 37350.0, 11744.0, 4911.0, 3571.0, 254506.0, 245350.0, 220454.0, 137680.0, 206932.0, 213808.0, 132012.0, 69557.0, 62456.0, 10398.0, 229677.0, 233182.0, 143907.0, 173139.0, 103710.0, 42503.0, 178340.0, 109522.0, 40220.0, 23327.0, 133484.0, 168285.0, 157076.0, 87661.0, 101085.0, 84557.0, 115851.0, 95153.0, 106277.0, 17131.0, 43919.0, 39455.0, 16768.0, 3657.0, 4620.0, 177247.0, 202674.0, 162059.0, 189260.0, 129385.0, 152800.0, 166084.0, 30054.0, 65207.0, 11104.0, 154569.0, 174286.0, 189938.0, 76871.0, 162729.0, 7678.0, 60510.0, 180884.0, 53652.0, 73550.0, 56772.0, 143230.0, 142317.0, 91821.0, 20767.0, 78448.0, 102856.0, 69360.0, 12511.0, 14270.0, 14694.0, 6292.0, 6609.0, 6948.0, 7424.0, 137980.0, 167430.0, 116129.0, 40227.0, 80458.0, 99975.0, 37988.0, 15311.0, 7734.0, 1739.0, 105072.0, 132324.0, 33036.0, 59512.0, 29387.0, 7693.0, 27956.0, 15569.0, 4943.0, 7355.0, 59400.0, 85576.0, 90536.0, 42498.0, 52552.0, 44894.0, 69309.0, 51107.0, 16515.0, 16963.0, 15497.0, 11109.0, 11464.0, 6012.0, 4135.0, 102635.0, 150277.0, 82599.0, 148412.0, 31088.0, 60822.0, 83068.0, 13055.0, 13080.0, 5243.0, 75983.0, 79375.0, 128058.0, 35980.0, 36552.0, 1923.0, 20141.0, 53496.0, 13418.0, 9737.0, 38563.0, 32712.0, 30851.0, 26067.0, 9085.0, 15189.0, 12933.0, 11569.0, 2628.0, 9244.0, 1869.0, 7634.0, 6310.0, 5329.0, 3411.0, 31156.0, 22241.0, 7050.0, 12877.0, 7690.0, 12262.0, 18668.0, 1799.0, 3699.0, 2492.0, 6906.0, 4885.0, 13765.0, 6936.0, 11477.0, 3916.0, 2007.0, 13424.0, 1743.0, 7518.0, 245188.0, 255328.0, 240376.0, 216379.0, 59937.0, 257274.0, 235631.0, 213073.0, 74099.0, 112576.0, 164564.0, 25484.0, 25569.0, 42807.0, 1694.0, 250958.0, 243170.0, 222528.0, 118459.0, 247034.0, 229752.0, 122252.0, 142034.0, 53820.0, 16435.0, 256349.0, 246166.0, 149139.0, 189970.0, 86378.0, 30682.0, 198112.0, 109512.0, 16507.0, 33637.0, 254029.0, 244590.0, 232612.0, 213300.0, 81207.0, 232426.0, 234243.0, 208985.0, 84767.0, 115731.0, 146715.0, 46647.0, 50698.0, 27758.0, 7560.0, 256986.0, 238130.0, 216464.0, 162397.0, 230413.0, 218418.0, 157612.0, 113608.0, 67048.0, 21693.0, 253944.0, 234123.0, 155282.0, 199740.0, 147216.0, 35002.0, 197894.0, 144313.0, 38214.0, 51820.0, 176586.0, 189479.0, 181808.0, 107358.0, 151681.0, 126327.0, 167640.0, 90893.0, 143047.0, 43532.0, 67132.0, 63427.0, 11407.0, 22485.0, 1717.0, 186807.0, 213718.0, 191497.0, 199529.0, 135841.0, 134280.0, 196178.0, 51392.0, 114165.0, 9509.0, 149358.0, 175518.0, 204810.0, 71399.0, 197516.0, 40276.0, 82771.0, 192718.0, 92191.0, 62854.0, 226304.0, 226244.0, 213017.0, 159359.0, 11947.0, 120670.0, 158448.0, 149983.0, 36653.0, 17195.0, 61718.0, 15052.0, 23246.0, 14682.0, 3388.0, 245894.0, 238335.0, 202625.0, 76409.0, 159110.0, 183938.0, 53447.0, 65742.0, 30626.0, 21335.0, 179420.0, 187716.0, 62059.0, 98145.0, 60329.0, 10485.0, 127619.0, 76205.0, 18625.0, 6798.0, 147902.0, 179417.0, 175121.0, 110418.0, 132186.0, 83688.0, 140944.0, 81196.0, 114440.0, 20681.0, 19595.0, 67565.0, 11736.0, 10309.0, 1985.0, 151206.0, 192416.0, 159252.0, 185329.0, 83943.0, 112067.0, 181384.0, 38791.0, 59649.0, 8346.0, 142594.0, 155211.0, 188113.0, 48304.0, 141275.0, 35059.0, 38435.0, 148935.0, 49371.0, 50983.0, 24055.0, 84000.0, 45502.0, 27930.0, 44684.0, 25746.0, 33662.0, 13593.0, 21261.0, 22438.0, 11827.0, 16614.0, 5030.0, 3515.0, 3635.0, 51893.0, 68914.0, 65756.0, 109323.0, 24117.0, 18907.0, 10451.0, 9431.0, 12637.0, 7081.0, 20718.0, 40014.0, 76197.0, 19922.0, 35947.0, 7370.0, 13833.0, 16107.0, 2095.0, 6274.0, 257935.0, 240254.0, 227800.0, 133576.0, 51105.0, 134133.0, 190928.0, 148108.0, 36884.0, 53342.0, 70563.0, 30784.0, 17297.0, 13015.0, 12015.0, 242475.0, 238730.0, 211805.0, 103048.0, 151140.0, 193154.0, 61581.0, 37450.0, 38530.0, 20228.0, 219504.0, 218853.0, 88321.0, 107225.0, 45631.0, 17806.0, 90165.0, 35815.0, 13192.0, 3673.0, 126701.0, 184969.0, 152936.0, 65943.0, 110102.0, 103591.0, 125329.0, 83836.0, 102286.0, 24384.0, 44208.0, 59534.0, 16866.0, 14570.0, 5400.0, 160444.0, 192233.0, 154567.0, 166157.0, 82656.0, 105452.0, 159647.0, 26670.0, 26426.0, 3347.0, 117591.0, 124655.0, 192171.0, 59455.0, 137688.0, 29608.0, 77477.0, 124661.0, 41642.0, 54390.0, 37871.0, 67224.0, 70347.0, 57013.0, 80258.0, 34481.0, 45230.0, 66588.0, 49501.0, 10576.0, 34099.0, 12293.0, 2978.0, 5869.0, 4445.0, 74398.0, 94673.0, 98124.0, 92499.0, 37463.0, 60830.0, 72684.0, 8810.0, 14057.0, 3763.0, 39405.0, 40570.0, 73626.0, 29570.0, 61608.0, 15563.0, 13985.0, 17530.0, 5785.0, 9321.0, 32665.0, 82883.0, 54828.0, 16909.0, 22351.0, 17711.0, 18953.0, 14475.0, 27614.0, 15973.0, 8494.0, 9057.0, 6764.0, 10811.0, 8294.0, 25709.0, 73636.0, 57463.0, 51439.0, 26856.0, 19288.0, 29934.0, 17904.0, 1880.0, 8711.0, 25299.0, 25096.0, 54677.0, 18110.0, 11127.0, 1750.0, 9344.0, 6392.0, 31104.0, 5852.0  },
                                                {280497.0, 246125.0, 226794.0, 134263.0, 14505.0, 257605.0, 220866.0, 117462.0, 19430.0, 110562.0, 82950.0, 36916.0, 10566.0, 1794.0, 1760.0, 268651.0, 224363.0, 206467.0, 28746.0, 237001.0, 175566.0, 33089.0, 47617.0, 26947.0, 2702.0, 258394.0, 239053.0, 39353.0, 118675.0, 31396.0, 25819.0, 148539.0, 31860.0, 6657.0, 3689.0, 276700.0, 229768.0, 211845.0, 172857.0, 29544.0, 229310.0, 216332.0, 171469.0, 19534.0, 138468.0, 131389.0, 51208.0, 17462.0, 6830.0, 10702.0, 269013.0, 215315.0, 211420.0, 62570.0, 219423.0, 209806.0, 77133.0, 132818.0, 68653.0, 4348.0, 245693.0, 214258.0, 95702.0, 148120.0, 68090.0, 19716.0, 153768.0, 93727.0, 18800.0, 15227.0, 234993.0, 243675.0, 218479.0, 129948.0, 20825.0, 139171.0, 184983.0, 136851.0, 42694.0, 90468.0, 115602.0, 24638.0, 7952.0, 10091.0, 1812.0, 229536.0, 223173.0, 183380.0, 55258.0, 149178.0, 178575.0, 67365.0, 106561.0, 53489.0, 19362.0, 198287.0, 191857.0, 87747.0, 120727.0, 62562.0, 31537.0, 92133.0, 53063.0, 23313.0, 10346.0, 120454.0, 165102.0, 128307.0, 56748.0, 20244.0, 93370.0, 82310.0, 56807.0, 14449.0, 40650.0, 67896.0, 15909.0, 22162.0, 7435.0, 7581.0, 140806.0, 162032.0, 93714.0, 28949.0, 71391.0, 88029.0, 35633.0, 54218.0, 31149.0, 5394.0, 75966.0, 95486.0, 25328.0, 42122.0, 33991.0, 8929.0, 37581.0, 25238.0, 12296.0, 33235.0, 59731.0, 38103.0, 44191.0, 21441.0, 8633.0, 30708.0, 22007.0, 3971.0, 22558.0, 5294.0, 10872.0, 1738.0, 3607.0, 3529.0, 2058.0, 45289.0, 50259.0, 18005.0, 34827.0, 13000.0, 15764.0, 37582.0, 10714.0, 7854.0, 4325.0, 25766.0, 24552.0, 46318.0, 5313.0, 30860.0, 7667.0, 6506.0, 14954.0, 11229.0, 17957.0, 253725.0, 230726.0, 181378.0, 76831.0, 17178.0, 135252.0, 186826.0, 109393.0, 21805.0, 40162.0, 53686.0, 27623.0, 9034.0, 8822.0, 8904.0, 239763.0, 217094.0, 152192.0, 39857.0, 133399.0, 135652.0, 37016.0, 29951.0, 19036.0, 4112.0, 153866.0, 128223.0, 48892.0, 50024.0, 21909.0, 3730.0, 38667.0, 16598.0, 12885.0, 17642.0, 207638.0, 259803.0, 200528.0, 120993.0, 24298.0, 118995.0, 160126.0, 111021.0, 30627.0, 49693.0, 77671.0, 17936.0, 9035.0, 17107.0, 3711.0, 189949.0, 226304.0, 183853.0, 42954.0, 141826.0, 146379.0, 63901.0, 65663.0, 34569.0, 6855.0, 155691.0, 164245.0, 81398.0, 84323.0, 56775.0, 19809.0, 75460.0, 62703.0, 9810.0, 10414.0, 108478.0, 164008.0, 144990.0, 50060.0, 16841.0, 56666.0, 110589.0, 76463.0, 24291.0, 27997.0, 63320.0, 28207.0, 17347.0, 10591.0, 5070.0, 117584.0, 148657.0, 125236.0, 42909.0, 76401.0, 119782.0, 38637.0, 58699.0, 34113.0, 2017.0, 71826.0, 121375.0, 55717.0, 74265.0, 54735.0, 13228.0, 57105.0, 28660.0, 12050.0, 17093.0, 43347.0, 54816.0, 46879.0, 27554.0, 16503.0, 15048.0, 34418.0, 28704.0, 6217.0, 25098.0, 25623.0, 7165.0, 1853.0, 8607.0, 2171.0, 42732.0, 48912.0, 27248.0, 29069.0, 32680.0, 11027.0, 32367.0, 25327.0, 11282.0, 6428.0, 18008.0, 46162.0, 50612.0, 9738.0, 49257.0, 3448.0, 18811.0, 41818.0, 20306.0, 40281.0, 50183.0, 75948.0, 70729.0, 58278.0, 25596.0, 98086.0, 106307.0, 52286.0, 19571.0, 60226.0, 75908.0, 16018.0, 12135.0, 9549.0, 5362.0, 59775.0, 88295.0, 46616.0, 9060.0, 76171.0, 61180.0, 40093.0, 86302.0, 19118.0, 11386.0, 35760.0, 50350.0, 21987.0, 32215.0, 28414.0, 13960.0, 25069.0, 11268.0, 5449.0, 18123.0, 87646.0, 54838.0, 47846.0, 20456.0, 17691.0, 108030.0, 103126.0, 22466.0, 11264.0, 132654.0, 126403.0, 26560.0, 14657.0, 16714.0, 5508.0, 72941.0, 64558.0, 66553.0, 18391.0, 122419.0, 58699.0, 23403.0, 115453.0, 24271.0, 5488.0, 49032.0, 53829.0, 34504.0, 62163.0, 37502.0, 13852.0, 52221.0, 25473.0, 7870.0, 16733.0, 40389.0, 54128.0, 25046.0, 26653.0, 12920.0, 66587.0, 44659.0, 17934.0, 20073.0, 53241.0, 31918.0, 8646.0, 1825.0, 1796.0, 5431.0, 44582.0, 25399.0, 10924.0, 7765.0, 59223.0, 24724.0, 25824.0, 35378.0, 13951.0, 5712.0, 24980.0, 10201.0, 11947.0, 8710.0, 14417.0, 8845.0, 17566.0, 13183.0, 3748.0, 15439.0, 11666.0, 17940.0, 26389.0, 11591.0, 7116.0, 11925.0, 11860.0, 18863.0, 11278.0, 3376.0, 8661.0, 3919.0, 3533.0, 3910.0, 1762.0, 3583.0, 13875.0, 16187.0, 3785.0, 11602.0, 2251.0, 15589.0, 8001.0, 1831.0, 2313.0, 9277.0, 7539.0, 1922.0, 8210.0, 1899.0, 4230.0, 8419.0, 3716.0, 1938.0, 9025.0, 17098.0, 12860.0, 16119.0, 19775.0, 23464.0, 24315.0, 14214.0, 13418.0, 14520.0, 10019.0, 6725.0, 5285.0, 1773.0, 5369.0, 4228.0, 7070.0, 9138.0, 7727.0, 5433.0, 6006.0, 12290.0, 5473.0, 2000.0, 4295.0, 4371.0, 1814.0, 17214.0, 10810.0, 6617.0, 3838.0, 3668.0, 1851.0, 2456.0, 5217.0, 11974.0, 2023.0, 18010.0, 18740.0, 13671.0, 5264.0, 8473.0, 5128.0, 2049.0, 7839.0, 1745.0, 5656.0, 2099.0, 1770.0, 1740.0, 5442.0, 1945.0, 14082.0, 1923.0, 1847.0, 3698.0, 6174.0, 5630.0, 1818.0, 5779.0, 5474.0, 6046.0, 7600.0, 6082.0, 7285.0, 11028.0, 3608.0, 7705.0, 3713.0, 9717.0, 4359.0, 274441.0, 236885.0, 227723.0, 170277.0, 28224.0, 245013.0, 230093.0, 158068.0, 19930.0, 98999.0, 117471.0, 71300.0, 18702.0, 8659.0, 4541.0, 259878.0, 236050.0, 202769.0, 53116.0, 231565.0, 189137.0, 64293.0, 110238.0, 64272.0, 6067.0, 242028.0, 227849.0, 77475.0, 157341.0, 71308.0, 14188.0, 121260.0, 36318.0, 7514.0, 31683.0, 253120.0, 242178.0, 216506.0, 185157.0, 53661.0, 201980.0, 207983.0, 190106.0, 47493.0, 144635.0, 136854.0, 66077.0, 39681.0, 27452.0, 3740.0, 244767.0, 230035.0, 209104.0, 85365.0, 213596.0, 207939.0, 98922.0, 153910.0, 81742.0, 12864.0, 229148.0, 221219.0, 120770.0, 154190.0, 85712.0, 47579.0, 150823.0, 67070.0, 20914.0, 41564.0, 199806.0, 236247.0, 208226.0, 136641.0, 16191.0, 126997.0, 171378.0, 128913.0, 51107.0, 79074.0, 93129.0, 20749.0, 21313.0, 7829.0, 9149.0, 209158.0, 226108.0, 188283.0, 83972.0, 146734.0, 165679.0, 54830.0, 79035.0, 61029.0, 9989.0, 165720.0, 189297.0, 55843.0, 94109.0, 45221.0, 48332.0, 111227.0, 51868.0, 27639.0, 31566.0, 81386.0, 123498.0, 90094.0, 45076.0, 55560.0, 55914.0, 60061.0, 40484.0, 42703.0, 32523.0, 18970.0, 11950.0, 11222.0, 1129.0, 1907.0, 98880.0, 112683.0, 80397.0, 79063.0, 71785.0, 61937.0, 60371.0, 20325.0, 30954.0, 7291.0, 63089.0, 65896.0, 104037.0, 22086.0, 91319.0, 4166.0, 10666.0, 50505.0, 32744.0, 23769.0, 175184.0, 187612.0, 154205.0, 99525.0, 18006.0, 98507.0, 152218.0, 66750.0, 21672.0, 41680.0, 61137.0, 40896.0, 10268.0, 13702.0, 1812.0, 187978.0, 212722.0, 167925.0, 52240.0, 118269.0, 157597.0, 76756.0, 56959.0, 27470.0, 5248.0, 118703.0, 151123.0, 74762.0, 89651.0, 61763.0, 27552.0, 45194.0, 33302.0, 22017.0, 15843.0, 168656.0, 233572.0, 204292.0, 84525.0, 26917.0, 96005.0, 145280.0, 128111.0, 43376.0, 72840.0, 90425.0, 21318.0, 15623.0, 7191.0, 7140.0, 181508.0, 194418.0, 186449.0, 51060.0, 124365.0, 159169.0, 77485.0, 59061.0, 54023.0, 11134.0, 137201.0, 163209.0, 65253.0, 105946.0, 74140.0, 9249.0, 79684.0, 59186.0, 33225.0, 13625.0, 49737.0, 110518.0, 79858.0, 51717.0, 42991.0, 47818.0, 61917.0, 43083.0, 36735.0, 18341.0, 40319.0, 2043.0, 13279.0, 3449.0, 1923.0, 65489.0, 106750.0, 74983.0, 81557.0, 57925.0, 45367.0, 49141.0, 26077.0, 19186.0, 2116.0, 53751.0, 71832.0, 106856.0, 33237.0, 55052.0, 10230.0, 33103.0, 52162.0, 35361.0, 21628.0, 55703.0, 85107.0, 64078.0, 45056.0, 26777.0, 90126.0, 130124.0, 38585.0, 21911.0, 82527.0, 89006.0, 3470.0, 10437.0, 1955.0, 1828.0, 67099.0, 72322.0, 69699.0, 18185.0, 81471.0, 92546.0, 30102.0, 69707.0, 30200.0, 5819.0, 48574.0, 42401.0, 19181.0, 32062.0, 48862.0, 6161.0, 22018.0, 13163.0, 15717.0, 11250.0, 72677.0, 45023.0, 48016.0, 28021.0, 21269.0, 90983.0, 83972.0, 33182.0, 26659.0, 80257.0, 61093.0, 7433.0, 6991.0, 15756.0, 4665.0, 48164.0, 36182.0, 45636.0, 11149.0, 61275.0, 48353.0, 14386.0, 47565.0, 17930.0, 2921.0, 38150.0, 25742.0, 44288.0, 27197.0, 15829.0, 9138.0, 23654.0, 27774.0, 14347.0, 11519.0, 5347.0, 8549.0, 17811.0, 16968.0, 5138.0, 8944.0, 12163.0, 5198.0, 9034.0, 14764.0, 3451.0, 2030.0, 2360.0, 3656.0, 5753.0, 13412.0, 3313.0, 1876.0, 10406.0, 3412.0, 4112.0, 15354.0, 8076.0, 1816.0, 7010.0, 5894.0, 12678.0, 5976.0, 3945.0, 9158.0, 2821.0, 3846.0, 7854.0, 3494.0, 13178.0, 242992.0, 237271.0, 227668.0, 151785.0, 27093.0, 122664.0, 186785.0, 142017.0, 44700.0, 61888.0, 62983.0, 17224.0, 3570.0, 14321.0, 7130.0, 239953.0, 234373.0, 193530.0, 58828.0, 184402.0, 168207.0, 66219.0, 101090.0, 55428.0, 3703.0, 206967.0, 167585.0, 65222.0, 92128.0, 47220.0, 14465.0, 113143.0, 52862.0, 15293.0, 8046.0, 194151.0, 218543.0, 208242.0, 141966.0, 37302.0, 91479.0, 141547.0, 113622.0, 28519.0, 68701.0, 95804.0, 23782.0, 1757.0, 11477.0, 3244.0, 214845.0, 214240.0, 189338.0, 52849.0, 140417.0, 162936.0, 67519.0, 59556.0, 61626.0, 2099.0, 174581.0, 179248.0, 69817.0, 118044.0, 53618.0, 14241.0, 86279.0, 25887.0, 12092.0, 5555.0, 72898.0, 125407.0, 78214.0, 45523.0, 59161.0, 27427.0, 69224.0, 52692.0, 52888.0, 23711.0, 22336.0, 22677.0, 7940.0, 1814.0, 5499.0, 79195.0, 110974.0, 87589.0, 88032.0, 54235.0, 55273.0, 87403.0, 26035.0, 31481.0, 6282.0, 68698.0, 83359.0, 106555.0, 26837.0, 91819.0, 15545.0, 34574.0, 81357.0, 13702.0, 42799.0, 71061.0, 116305.0, 100823.0, 71603.0, 23521.0, 26503.0, 59930.0, 71177.0, 16536.0, 34564.0, 30924.0, 21373.0, 25569.0, 8318.0, 6945.0, 98235.0, 116304.0, 111416.0, 22826.0, 55546.0, 80048.0, 36779.0, 25540.0, 30223.0, 8968.0, 61279.0, 71409.0, 46643.0, 45637.0, 21878.0, 7284.0, 18133.0, 22479.0, 11513.0, 16485.0, 51896.0, 144879.0, 103687.0, 34688.0, 34835.0, 33493.0, 30859.0, 47809.0, 19971.0, 24710.0, 22154.0, 8819.0, 19233.0, 1938.0, 10383.0, 31765.0, 104540.0, 66135.0, 50014.0, 56771.0, 41075.0, 35630.0, 15813.0, 4518.0, 6348.0, 42715.0, 55611.0, 80977.0, 36615.0, 46101.0, 3751.0, 14170.0, 60224.0, 41593.0, 14037.0, 35303.0, 35793.0, 31952.0, 9143.0, 17699.0, 38290.0, 28101.0, 25036.0, 1687.0, 46452.0, 25452.0, 5291.0, 12235.0, 5487.0, 4001.0, 24407.0, 15436.0, 4541.0, 17344.0, 33322.0, 11215.0, 13629.0, 19586.0, 3464.0, 6161.0, 16159.0, 17380.0, 25901.0, 18844.0, 5797.0, 3741.0, 5024.0, 5511.0, 16004.0, 9998.0, 114761.0, 105308.0, 124977.0, 63628.0, 5184.0, 53709.0, 65218.0, 54081.0, 13021.0, 18815.0, 46583.0, 15258.0, 3677.0, 4058.0, 2262.0, 132698.0, 109313.0, 91461.0, 14523.0, 54441.0, 88890.0, 29226.0, 15139.0, 19307.0, 15813.0, 74729.0, 74339.0, 11329.0, 43519.0, 21092.0, 7385.0, 32960.0, 14184.0, 11824.0, 7880.0, 79665.0, 84366.0, 78888.0, 39920.0, 27962.0, 19854.0, 41633.0, 26974.0, 23026.0, 19065.0, 13941.0, 9797.0, 2931.0, 13140.0, 5771.0, 50316.0, 68090.0, 73080.0, 61857.0, 27620.0, 43724.0, 25332.0, 3473.0, 30040.0, 1893.0, 66629.0, 35023.0, 89823.0, 7740.0, 39225.0, 9908.0, 2223.0, 48939.0, 21717.0, 34385.0, 16392.0, 30002.0, 41848.0, 26137.0, 26986.0, 11174.0, 27340.0, 23550.0, 26688.0, 3432.0, 4093.0, 724.0, 5992.0, 6576.0, 4054.0, 27816.0, 31187.0, 23870.0, 21047.0, 7874.0, 16372.0, 35591.0, 12033.0, 5931.0, 3780.0, 16498.0, 22371.0, 27345.0, 8330.0, 6059.0, 4714.0, 7063.0, 3948.0, 7467.0, 9266.0, 13678.0, 15746.0, 23806.0, 8748.0, 16245.0, 2049.0, 6628.0, 5584.0, 20735.0, 3444.0, 8725.0, 11545.0, 3719.0, 3509.0, 3626.0, 13442.0, 24783.0, 2498.0, 24989.0, 11619.0, 6088.0, 17653.0, 9061.0, 3569.0, 5789.0, 10347.0, 18294.0, 4385.0, 1707.0, 4545.0, 5765.0, 4281.0, 6397.0, 9467.0, 2837.0  },
                                                {209269.0, 173608.0, 164749.0, 145590.0, 21922.0, 173966.0, 201302.0, 151022.0, 26531.0, 177603.0, 133882.0, 38395.0, 19172.0, 6753.0, 3739.0, 198408.0, 165707.0, 166030.0, 71509.0, 164948.0, 184835.0, 76424.0, 129305.0, 59728.0, 3014.0, 170223.0, 160362.0, 90022.0, 165880.0, 90398.0, 34714.0, 176835.0, 77199.0, 10162.0, 26795.0, 194351.0, 166151.0, 157986.0, 146500.0, 37237.0, 168153.0, 151396.0, 151119.0, 57111.0, 147597.0, 148197.0, 56069.0, 46525.0, 17899.0, 9051.0, 176008.0, 147435.0, 155666.0, 96295.0, 167792.0, 152961.0, 116304.0, 145838.0, 73041.0, 18605.0, 162826.0, 156179.0, 127576.0, 162997.0, 114408.0, 30489.0, 159205.0, 101619.0, 47667.0, 46660.0, 174277.0, 171078.0, 158114.0, 140594.0, 36661.0, 170439.0, 161423.0, 149728.0, 60973.0, 117851.0, 120014.0, 58510.0, 46935.0, 30199.0, 2531.0, 173806.0, 159960.0, 151984.0, 97786.0, 161046.0, 153315.0, 115415.0, 120622.0, 78448.0, 30202.0, 178297.0, 161655.0, 121707.0, 161270.0, 115548.0, 46162.0, 162677.0, 115379.0, 53026.0, 51226.0, 154631.0, 151277.0, 139545.0, 82372.0, 9850.0, 128335.0, 124106.0, 88788.0, 41234.0, 57771.0, 51423.0, 9263.0, 25393.0, 7088.0, 2518.0, 166040.0, 157683.0, 135035.0, 53507.0, 133568.0, 133974.0, 64021.0, 75520.0, 30171.0, 12260.0, 146740.0, 138070.0, 90624.0, 102147.0, 98260.0, 30019.0, 96136.0, 50809.0, 34536.0, 22440.0, 36684.0, 45097.0, 40098.0, 19652.0, 14802.0, 18047.0, 38326.0, 14713.0, 30740.0, 17664.0, 11091.0, 12785.0, 9748.0, 2106.0, 3214.0, 54698.0, 72996.0, 38657.0, 54896.0, 46413.0, 49139.0, 77734.0, 22631.0, 16912.0, 19114.0, 54836.0, 61660.0, 114374.0, 33747.0, 81798.0, 23053.0, 14961.0, 79837.0, 70181.0, 43095.0, 187285.0, 177056.0, 156787.0, 116883.0, 15444.0, 172650.0, 145582.0, 124435.0, 35940.0, 86590.0, 95098.0, 17889.0, 1351.0, 8382.0, 1252.0, 182229.0, 154456.0, 157165.0, 43475.0, 150871.0, 159244.0, 62218.0, 39091.0, 24850.0, 1409.0, 174483.0, 161939.0, 60431.0, 133140.0, 55491.0, 11474.0, 124341.0, 27134.0, 18136.0, 25764.0, 207846.0, 212508.0, 178478.0, 121916.0, 28403.0, 165498.0, 155845.0, 132729.0, 42372.0, 67589.0, 71082.0, 38753.0, 20429.0, 13013.0, 1712.0, 176366.0, 176741.0, 148917.0, 73585.0, 160489.0, 152513.0, 96389.0, 96916.0, 55346.0, 11197.0, 169924.0, 157839.0, 92906.0, 127960.0, 83767.0, 30418.0, 127535.0, 75652.0, 44102.0, 28724.0, 131356.0, 158650.0, 133347.0, 87531.0, 20006.0, 112772.0, 129873.0, 104878.0, 30318.0, 49071.0, 57947.0, 22740.0, 10410.0, 6966.0, 1261.0, 162536.0, 156919.0, 139668.0, 52566.0, 129841.0, 131322.0, 71227.0, 58045.0, 50388.0, 3955.0, 152380.0, 135965.0, 76622.0, 80766.0, 77203.0, 12575.0, 99572.0, 56004.0, 28355.0, 18078.0, 40524.0, 96842.0, 71431.0, 30856.0, 17885.0, 44652.0, 52198.0, 31987.0, 34172.0, 22089.0, 13645.0, 16722.0, 4510.0, 2564.0, 1280.0, 97684.0, 90398.0, 51203.0, 54590.0, 68771.0, 63816.0, 82288.0, 28498.0, 20361.0, 6374.0, 59187.0, 55985.0, 85053.0, 49927.0, 89459.0, 15726.0, 30418.0, 80721.0, 34381.0, 47588.0, 119834.0, 166581.0, 115892.0, 41388.0, 8674.0, 45353.0, 67996.0, 28833.0, 9893.0, 6144.0, 25584.0, 10245.0, 1266.0, 1273.0, 1241.0, 113680.0, 149420.0, 100529.0, 18526.0, 56058.0, 59199.0, 20762.0, 25239.0, 18680.0, 1230.0, 79212.0, 76189.0, 33462.0, 19511.0, 26759.0, 12378.0, 29208.0, 21366.0, 10012.0, 2139.0, 57134.0, 115344.0, 83305.0, 52620.0, 8313.0, 32478.0, 62597.0, 46833.0, 23346.0, 11474.0, 28094.0, 2467.0, 1497.0, 2780.0, 3561.0, 90231.0, 136227.0, 100935.0, 27212.0, 38753.0, 73866.0, 40342.0, 21987.0, 21106.0, 3022.0, 87526.0, 100903.0, 35195.0, 33488.0, 27151.0, 2538.0, 40768.0, 18679.0, 11513.0, 13397.0, 51153.0, 59840.0, 40186.0, 9532.0, 14429.0, 27495.0, 38484.0, 18850.0, 22826.0, 15235.0, 4922.0, 3892.0, 1312.0, 5065.0, 5660.0, 56757.0, 62281.0, 39669.0, 53114.0, 32930.0, 33175.0, 34047.0, 8137.0, 2670.0, 1355.0, 38730.0, 31197.0, 75440.0, 20304.0, 50241.0, 3056.0, 11311.0, 61132.0, 24070.0, 18853.0, 70190.0, 43959.0, 12116.0, 6572.0, 3801.0, 6442.0, 10409.0, 11984.0, 5669.0, 5960.0, 2505.0, 1302.0, 1251.0, 3384.0, 1253.0, 9303.0, 24759.0, 6058.0, 1303.0, 7076.0, 7444.0, 2481.0, 1236.0, 1252.0, 1214.0, 1287.0, 9457.0, 1236.0, 7138.0, 3770.0, 2898.0, 6313.0, 2468.0, 2457.0, 0.0, 25279.0, 15735.0, 17137.0, 17381.0, 13582.0, 6070.0, 3745.0, 6139.0, 3513.0, 2546.0, 5939.0, 6218.0, 3907.0, 1242.0, 2558.0, 8730.0, 15566.0, 3086.0, 25955.0, 5665.0, 12129.0, 7420.0, 1245.0, 6700.0, 2479.0, 3361.0, 2724.0, 17878.0, 1251.0, 6947.0, 5685.0, 11797.0, 28605.0, 4365.0, 9241.0, 24178.0, 170917.0, 50686.0, 1920.0, 1302.0, 4709.0, 6185.0, 1308.0, 1261.0, 2724.0, 1272.0, 1239.0, 3796.0, 1257.0, 2543.0, 1243.0, 2073.0, 3732.0, 3134.0, 3876.0, 1291.0, 2426.0, 3136.0, 1429.0, 3636.0, 7325.0, 2443.0, 2839.0, 1262.0, 2941.0, 2408.0, 5276.0, 2418.0, 4293.0, 1754.0, 198146.0, 170611.0, 164016.0, 158310.0, 59418.0, 174558.0, 185744.0, 154595.0, 47289.0, 161640.0, 135824.0, 41493.0, 30999.0, 16443.0, 2688.0, 189280.0, 158032.0, 157457.0, 85095.0, 155282.0, 166527.0, 95958.0, 148398.0, 47743.0, 3961.0, 166176.0, 155081.0, 113926.0, 163804.0, 96477.0, 36540.0, 163924.0, 97577.0, 21664.0, 43115.0, 181743.0, 165249.0, 155262.0, 157644.0, 49211.0, 173767.0, 160475.0, 144099.0, 89166.0, 157663.0, 146543.0, 51666.0, 21471.0, 20875.0, 6465.0, 185334.0, 153599.0, 150482.0, 135062.0, 169451.0, 152624.0, 123218.0, 147594.0, 89588.0, 25291.0, 165056.0, 162497.0, 149056.0, 153655.0, 120391.0, 36739.0, 167083.0, 147895.0, 58853.0, 34078.0, 173496.0, 161397.0, 150050.0, 131238.0, 33824.0, 171345.0, 161418.0, 141916.0, 52825.0, 86298.0, 119403.0, 31703.0, 50457.0, 22078.0, 1332.0, 170830.0, 163056.0, 145558.0, 101885.0, 155494.0, 151689.0, 95434.0, 101662.0, 88970.0, 17447.0, 161395.0, 164981.0, 133962.0, 150570.0, 123605.0, 35706.0, 150115.0, 110161.0, 49747.0, 57506.0, 96237.0, 134331.0, 111466.0, 53345.0, 63256.0, 56181.0, 110007.0, 66366.0, 82230.0, 49148.0, 40146.0, 46181.0, 18306.0, 7300.0, 1244.0, 141840.0, 134618.0, 124935.0, 130933.0, 97033.0, 87579.0, 127019.0, 56266.0, 108948.0, 20563.0, 121990.0, 112527.0, 141126.0, 52146.0, 138104.0, 37966.0, 68533.0, 134671.0, 99150.0, 56960.0, 188701.0, 169607.0, 159795.0, 135507.0, 23071.0, 156535.0, 159825.0, 135017.0, 44931.0, 69235.0, 67728.0, 18828.0, 9634.0, 3950.0, 3204.0, 179605.0, 165008.0, 151711.0, 64212.0, 149150.0, 155322.0, 65971.0, 62297.0, 35149.0, 2463.0, 167906.0, 156843.0, 94382.0, 113315.0, 71895.0, 14973.0, 136963.0, 49357.0, 27228.0, 13092.0, 202193.0, 190664.0, 151908.0, 128834.0, 42768.0, 139676.0, 146947.0, 124148.0, 36466.0, 65542.0, 68965.0, 25231.0, 13252.0, 19756.0, 1241.0, 180545.0, 174318.0, 148958.0, 81261.0, 147201.0, 154436.0, 93342.0, 95743.0, 51740.0, 19213.0, 161789.0, 162443.0, 106339.0, 134113.0, 92057.0, 23356.0, 113398.0, 74485.0, 42366.0, 21803.0, 98546.0, 126197.0, 110499.0, 69960.0, 94488.0, 81073.0, 96931.0, 52761.0, 96550.0, 30099.0, 33902.0, 25229.0, 8820.0, 6721.0, 1350.0, 135146.0, 137525.0, 118094.0, 133491.0, 98346.0, 94088.0, 133676.0, 28537.0, 42967.0, 6351.0, 112565.0, 114040.0, 132976.0, 59892.0, 128826.0, 18278.0, 73314.0, 133495.0, 89925.0, 73892.0, 85405.0, 147041.0, 113584.0, 55901.0, 15868.0, 47465.0, 83367.0, 39204.0, 6308.0, 20443.0, 15350.0, 6187.0, 1318.0, 3786.0, 5342.0, 100299.0, 138722.0, 111363.0, 18694.0, 63236.0, 69377.0, 24639.0, 15860.0, 6467.0, 2763.0, 78287.0, 80638.0, 55745.0, 34360.0, 26692.0, 3765.0, 31764.0, 11463.0, 8358.0, 4889.0, 62668.0, 74435.0, 62734.0, 31534.0, 42017.0, 8791.0, 39853.0, 30235.0, 21456.0, 14496.0, 19801.0, 10960.0, 8577.0, 5544.0, 4433.0, 65901.0, 82965.0, 74838.0, 91883.0, 42000.0, 54228.0, 87994.0, 21966.0, 10233.0, 11681.0, 45884.0, 52442.0, 91499.0, 24384.0, 55861.0, 14599.0, 28284.0, 48220.0, 23071.0, 11338.0, 50678.0, 9283.0, 15208.0, 2433.0, 1316.0, 6666.0, 5048.0, 5154.0, 3740.0, 7518.0, 1556.0, 1275.0, 2579.0, 2527.0, 2464.0, 2463.0, 21325.0, 3998.0, 16781.0, 5159.0, 2505.0, 1343.0, 4190.0, 2582.0, 3210.0, 14267.0, 8137.0, 18991.0, 3016.0, 1747.0, 1769.0, 16503.0, 6107.0, 7821.0, 3501.0, 196660.0, 173315.0, 162299.0, 147674.0, 28529.0, 179995.0, 172878.0, 147331.0, 36857.0, 133435.0, 95097.0, 23230.0, 8163.0, 12640.0, 6305.0, 177092.0, 158688.0, 163610.0, 75551.0, 170668.0, 162785.0, 99870.0, 99021.0, 26937.0, 4318.0, 181879.0, 160352.0, 115007.0, 148925.0, 65734.0, 20910.0, 144029.0, 104054.0, 38110.0, 15030.0, 179512.0, 167304.0, 159038.0, 141674.0, 44283.0, 175527.0, 172439.0, 135598.0, 59262.0, 94788.0, 94216.0, 28165.0, 24546.0, 5609.0, 1236.0, 177049.0, 148973.0, 149789.0, 110490.0, 158304.0, 149372.0, 100520.0, 92945.0, 58384.0, 7981.0, 177824.0, 159204.0, 117220.0, 142916.0, 89503.0, 26846.0, 150334.0, 94151.0, 43138.0, 38381.0, 128460.0, 149398.0, 123563.0, 80104.0, 74482.0, 69750.0, 83359.0, 68215.0, 117231.0, 42206.0, 38845.0, 40456.0, 12580.0, 4010.0, 1245.0, 137560.0, 152156.0, 130156.0, 126897.0, 93795.0, 92193.0, 134818.0, 38087.0, 90657.0, 7085.0, 126651.0, 132743.0, 139520.0, 71651.0, 146578.0, 40153.0, 59525.0, 141098.0, 74189.0, 79444.0, 135901.0, 162240.0, 140226.0, 96149.0, 24182.0, 124431.0, 115786.0, 105618.0, 29540.0, 16079.0, 24479.0, 15231.0, 2665.0, 2688.0, 2872.0, 168769.0, 159890.0, 136150.0, 50566.0, 118016.0, 117341.0, 53584.0, 41041.0, 15209.0, 3890.0, 138259.0, 142247.0, 83294.0, 62444.0, 31168.0, 5232.0, 75363.0, 58427.0, 3735.0, 15126.0, 86873.0, 139090.0, 133569.0, 64277.0, 102093.0, 67718.0, 92183.0, 43481.0, 80672.0, 29983.0, 18149.0, 17718.0, 15718.0, 10208.0, 1276.0, 131172.0, 142264.0, 118280.0, 127522.0, 66349.0, 98417.0, 137083.0, 23661.0, 56130.0, 1765.0, 92385.0, 115602.0, 139806.0, 38391.0, 127760.0, 7630.0, 36998.0, 126780.0, 56023.0, 51382.0, 22616.0, 45871.0, 44835.0, 30615.0, 33805.0, 6716.0, 22301.0, 21415.0, 24415.0, 18593.0, 14821.0, 1443.0, 3198.0, 2647.0, 4988.0, 27541.0, 43536.0, 35278.0, 53233.0, 13727.0, 32561.0, 21420.0, 4988.0, 8205.0, 3761.0, 31309.0, 33205.0, 53836.0, 9297.0, 26527.0, 2621.0, 21964.0, 25260.0, 10670.0, 5390.0, 176566.0, 182131.0, 142008.0, 111797.0, 25160.0, 162291.0, 151655.0, 104516.0, 20033.0, 88703.0, 46204.0, 4029.0, 4009.0, 4354.0, 1470.0, 178991.0, 156748.0, 151430.0, 49036.0, 103597.0, 135491.0, 55233.0, 35003.0, 8036.0, 6469.0, 141732.0, 152375.0, 67914.0, 73340.0, 26416.0, 27443.0, 72142.0, 18385.0, 11105.0, 4915.0, 82367.0, 125789.0, 96268.0, 36975.0, 79760.0, 41613.0, 81379.0, 57869.0, 94284.0, 10715.0, 24533.0, 50756.0, 4354.0, 3510.0, 7801.0, 122099.0, 140177.0, 103642.0, 135658.0, 58258.0, 90413.0, 118297.0, 19333.0, 71963.0, 3159.0, 76620.0, 106336.0, 141844.0, 45234.0, 120196.0, 10676.0, 32637.0, 117920.0, 27025.0, 48544.0, 19817.0, 71678.0, 60523.0, 16432.0, 21435.0, 23551.0, 17391.0, 21230.0, 37887.0, 10425.0, 12301.0, 11086.0, 1282.0, 2713.0, 5566.0, 61584.0, 79842.0, 31945.0, 92115.0, 13987.0, 35101.0, 53827.0, 6270.0, 18272.0, 7495.0, 44126.0, 47762.0, 96609.0, 16238.0, 32629.0, 5970.0, 10657.0, 53761.0, 3822.0, 3694.0, 34410.0, 58294.0, 40094.0, 20005.0, 24296.0, 23320.0, 27165.0, 7495.0, 3180.0, 1484.0, 8939.0, 21470.0, 5254.0, 1306.0, 3932.0, 32346.0, 45011.0, 40410.0, 26354.0, 11136.0, 16115.0, 45771.0, 8823.0, 6244.0, 3675.0, 20784.0, 37262.0, 64900.0, 5349.0, 48697.0, 4825.0, 10184.0, 23346.0, 2363.0, 7316.0  } };

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

