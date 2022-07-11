#include "applications.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "message.h"
#include "packet.h"
#include "chronos.h"

//extern TaskHandle_t KeeperTask_Handler;

void API_TaskListInit(){
    unsigned int i, j;
    for (i = 0; i < NUM_MAX_TASKS; i++){
        TaskList[i].status = TASK_SLOT_EMPTY;
        for (j = 0; j < NUM_MAX_APP_TASKS; j++){
            TaskList[i].PendingReq[j] = FALSE;
        }
    }
    return;
}

unsigned int API_GetCurrentTaskSlot(){
    // The handle of the currently running ( calling ) task on the kernel.
    TaskHandle_t xHandle = xTaskGetCurrentTaskHandle();
    unsigned int i = 0;
    for ( i = 0 ; i < NUM_MAX_TASKS ; i++ ) {
        if ( TaskList[ i ].TaskHandler == xHandle && TaskList[i].status != TASK_SLOT_EMPTY ) {
            return( i );
        }
    }
    prints("returning erro0\n");
    return ERRO;
}

unsigned int API_GetFreeTaskSlot(){
    unsigned int i;
    for (i = 0; i < NUM_MAX_TASKS; i++){
        if (TaskList[i].status == TASK_SLOT_EMPTY){
            return i;
        }
    }
    prints("returning erro1\n");
    return ERRO;
}

void API_InformMigration(unsigned int app_id, unsigned int task_id){
    unsigned int tslot = API_GetTaskSlot(task_id, app_id);
    volatile unsigned int *migrationVar = TaskList[tslot].migrationPointer;
    printsv("Antes: ", *migrationVar);
    *migrationVar = 0xFFFFFFFF;
    printsv("Depois: ", *migrationVar);
    return;
}

void API_SetMigrationVar(unsigned int slot, unsigned int value){
    volatile unsigned int *migrationVar = TaskList[slot].migrationPointer;
    printsv("Setting migration variable to: ", value);
    printsvsv("Task id: ", TaskList[slot].TaskID, "App id: ", TaskList[slot].AppID);
    *migrationVar = value;
    return;
}


unsigned int API_TaskAllocation(unsigned int task_id, unsigned int txt_size, unsigned int bss_size, unsigned int start_point, unsigned int app_id, unsigned int migration_addr){
    unsigned int tslot = API_GetFreeTaskSlot();
    int i;
    volatile unsigned int *header;
    if(tslot == ERRO){
        prints("DEU RUIM NO API_GetFreeTaskSlot()\n");
    }
    printsv("Got slot: ", tslot);
    TaskList[tslot].status = TASK_SLOT_WAITING_START;
    TaskList[tslot].waitingMsg = FALSE;
    TaskList[tslot].TaskID = task_id;
    TaskList[tslot].AppID = app_id;
    TaskList[tslot].taskSize = 4 * (txt_size + bss_size); // it multiply by four because each word has 32 bits and the memory is addressed by byte - so each word is composed by 4 addresses
    printsv("Task total size (txt+bss): ", TaskList[tslot].taskSize);
    TaskList[tslot].fullAddr = (unsigned int)pvPortMalloc(TaskList[tslot].taskSize+(PKT_HEADER_SIZE*4));
    printsv("Task fullAddr: ", TaskList[tslot].fullAddr );
    //
    header = TaskList[tslot].fullAddr;
    header[0]  = 0;
    header[1]  = txt_size + bss_size + PKT_SERVICE_SIZE;
    header[2]  = TASK_MIGRATION_TASK;
    header[3]  = task_id;        //task id
    header[4]  = txt_size;       //task size
    header[5]  = bss_size;       //bss size
    header[6]  = start_point;    //start point
    header[7]  = app_id;         //taskappid
    header[8]  = migration_addr; //migration variable
    header[9]  = -1;
    header[10] = -1;
    header[11] = -1;
    header[12] = -1;
    //
    TaskList[tslot].taskAddr = TaskList[tslot].fullAddr + (PKT_HEADER_SIZE*4);
    printsv("Task addr: ", TaskList[tslot].taskAddr );
    TaskList[tslot].mainAddr =  TaskList[tslot].taskAddr + (4 * start_point);
    TaskList[tslot].migrationPointer = TaskList[tslot].taskAddr + ( 4 * migration_addr);

    // filling the MemoryRegion_t struct
    //TaskList[tslot].memRegion.ulLengthInBytes = 0;// TaskList[tslot].taskSize;
    //TaskList[tslot].memRegion.pvBaseAddress = 0;//TaskList[tslot].taskAddr;

    for(i=0;i<NUM_MAX_APP_TASKS;i++){
        TaskList[tslot].PendingReq[i] = FALSE;
        TaskList[tslot].TasksMap[i] = 0;
    }

    return tslot;
}

unsigned int API_GetTaskSlot(unsigned int task_id, unsigned int app_id){
    unsigned int i;
    printsvsv("looking for task_id ", task_id, " from app_id ", app_id);
    for( i = 0; i < NUM_MAX_APP_TASKS; i++){
        if( task_id == TaskList[i].TaskID && app_id == TaskList[i].AppID ){
            printsv("current status: ", TaskList[i].status);
            if( TaskList[i].status != TASK_SLOT_EMPTY ){
                return i;
            }
        }
    }
    prints("returning erro2\n");
    return ERRO;
}

void API_TaskStart(unsigned int slot, unsigned int arg){
    BaseType_t xReturned;
    TaskList[slot].status = TASK_SLOT_RUNNING;

    /*xReturned = xTaskGenericCreate( TaskList[slot].mainAddr,    // pxTaskCode
                                    "LaTask",                   // pcName
                                    16384, //4096,                       // usStackDepth
                                    NULL,                       // pvParameters
                                    tskIDLE_PRIORITY+1,         //uxPriority
                                    &TaskList[slot].TaskHandler,// pxCreatedTask
                                    NULL,                       // puxStackBuffer
                                    &TaskList[slot].memRegion );//xRegions*/
    
    xReturned = xTaskCreate(TaskList[slot].mainAddr,
                            "LaTask",
                            4096,//8192,//4096,//16384,
                            arg,
                            tskIDLE_PRIORITY+1,
                            &TaskList[slot].TaskHandler);
    if( xReturned != pdPASS ){
        prints("ERROR - API_TaskStart!!!\n");
    }
    return;
}

void API_MigrationReady(){
    unsigned int mySlot = API_GetServiceSlot();
    unsigned int slot = API_GetCurrentTaskSlot();
    do{
        if(mySlot == PIPE_FULL){
            // Runs the NI Handler to send/receive packets, opening space in the PIPE
            prints("Estou preso aqui11...\n");
            API_NI_Handler();
        }
    }while(mySlot == PIPE_FULL);

    ServicePipe[mySlot].holder = PIPE_SYS_HOLDER;

    ServicePipe[mySlot].header.header           = makeAddress(0, 0);
    ServicePipe[mySlot].header.payload_size     = PKT_SERVICE_SIZE;
    ServicePipe[mySlot].header.service          = TASK_MIGRATION_READY;
    ServicePipe[mySlot].header.task_id          = TaskList[slot].TaskID;
    ServicePipe[mySlot].header.app_id      = TaskList[slot].AppID;

    API_PushSendQueue(SERVICE, mySlot);

    TaskList[slot].status = TASK_SLOT_BLOQUED;

    prints("Suspendo task para migração...\n");
    vTaskSuspend( NULL ); // the task suspend itself
}

void API_FinishRunningTask(){
    int i;
    //BaseType_t xHigherPriorityTaskWoken;
    unsigned int slot = API_GetCurrentTaskSlot();
    if (slot == ERRO) prints("ERRO VIOLENTO AQUI!\n");
    
    printsvsv("Finishing task:", TaskList[slot].TaskID, "app: ", TaskList[slot].AppID);
    printsv("From slot: ", slot);
    while(API_checkPipe(slot) == 1){
        vTaskDelay(1);
    }
    //vTaskEnterCritical();
    printsvsv("Task ", TaskList[slot].TaskID, " is being deleted! From application ", TaskList[slot].AppID);
    TaskList[slot].status = TASK_SLOT_FINISH; //TASK_SLOT_EMPTY;
    //xHigherPriorityTaskWoken = pdFALSE;
    //vTaskNotifyGiveFromISR( KeeperTask_Handler, &xHigherPriorityTaskWoken );
    /* Force a context switch if xHigherPriorityTaskWoken is now
    set to pdTRUE. The macro used to do this is dependent on
    the port and may be called portEND_SWITCHING_ISR. */
    //portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );

    //}
    
    for(i = 0; i < NUM_MAX_TASKS; i++){
        printsvsv("TaskList[", i, "]status: ", TaskList[i].status );
        if(TaskList[i].status != TASK_SLOT_EMPTY && TaskList[i].status != TASK_SLOT_FINISH){
            printsvsv("Returning because of: ", i, "TaskList[i].status ", TaskList[i].status);
            i = 0xffffffff;
            break;
        }
    }
    if(i != 0xffffffff){ API_setFreqIdle(); }
    API_SendFinishTask(TaskList[slot].TaskID, TaskList[slot].AppID);
    vPortFree(TaskList[slot].fullAddr);
    prints("free done \n");
    TaskList[slot].status = TASK_SLOT_EMPTY;
    vTaskExitCritical();
    prints("deleting... \n");
    vTaskDelete(TaskList[slot].TaskHandler);
    prints("deleted! \n");
    return;
}

void API_StallTask(unsigned int app_id, unsigned int task_id){
    int slot = API_GetTaskSlot(task_id, app_id);
    if ( slot != ERRO ){
        API_Ack_StallTask(app_id, task_id);
        TaskList[slot].status = TASK_SLOT_BLOQUED;
        vTaskSuspend( TaskList[slot].TaskHandler );
    } else { 
        prints("ERROR - Task not found!\n");
    }
    return;
}

API_Ack_StallTask(unsigned int app_id, unsigned int task_id){
    unsigned int mySlot = API_GetServiceSlot();
    do{
        if(mySlot == PIPE_FULL){
            // Runs the NI Handler to send/receive packets, opening space in the PIPE
            prints("Estou preso aqui12...\n");
            API_NI_Handler();
        }
    }while(mySlot == PIPE_FULL);

    ServicePipe[mySlot].holder = PIPE_SYS_HOLDER;

    ServicePipe[mySlot].header.header           = makeAddress(0, 0);
    ServicePipe[mySlot].header.payload_size     = PKT_SERVICE_SIZE;
    ServicePipe[mySlot].header.service          = TASK_STALL_ACK;
    ServicePipe[mySlot].header.task_id          = task_id;
    ServicePipe[mySlot].header.app_id           = app_id;

    API_PushSendQueue(SERVICE, mySlot);
    return;
}

void API_ForwardTask(unsigned int app_id, unsigned int task_id, unsigned int dest_addr){
    unsigned int mySlot, tslot = API_GetTaskSlot(task_id, app_id);
    volatile unsigned int *pointer = TaskList[tslot].fullAddr;
    pointer[0] = dest_addr;
    TaskList[tslot].TasksMap[task_id] = dest_addr;
    TaskList[tslot].status = TASK_MIGRATION_FORWARD;
    API_PushSendQueue(MIGRATION, tslot);
    return;
}

void API_SendPIPE(unsigned int app_id, unsigned int task_id, unsigned int task_dest_addr){
    unsigned int i, done, newer ,slot, taskslot = API_GetTaskSlot(task_id, app_id);
    do{
        newer = 0xEFFFFFFF;
        done = 1;
        // search for the newest packet available in the pipe
        for( i = 0; i < PIPE_SIZE; i++ ){
            if (TaskList[taskslot].MessagePipe[i].status == PIPE_OCCUPIED){
                printsv("PIPE slot occupied: ", i);
                if(TaskList[taskslot].MessagePipe[i].msgID <= newer){
                    newer = TaskList[taskslot].MessagePipe[i].msgID;
                    slot = i;
                    done = 0;
                }
            }
        }
        printsv("Older slot: ", slot);
        // if we found a packet, send it to the new address
        if(!done){
            prints("MIGRATION: Sending one message to the new address!\n");
            TaskList[taskslot].MessagePipe[slot].status                  = PIPE_TRANSMITTING;
            TaskList[taskslot].MessagePipe[slot].header.saved_addr       = TaskList[slot].MessagePipe[i].header.header; // save the original address of this message
            TaskList[taskslot].MessagePipe[slot].header.header           = task_dest_addr; // use the new PE address to forward the message
            TaskList[taskslot].MessagePipe[slot].header.service          = MESSAGE_MIGRATION;
            API_PushSendQueue(MESSAGE, (taskslot << 8 | slot));
        }
    }while(!done);
    return;
}

void API_SendPending_and_Address(unsigned int app_id, unsigned int task_id, unsigned int task_dest_addr){
    unsigned int i, taskslot = API_GetTaskSlot(task_id, app_id);
    while(ServiceMessage.status == PIPE_OCCUPIED){
        // Runs the NI Handler to send/receive packets, opening space in the PIPE
        prints("Estou preso aqui27...\n");
        API_NI_Handler();
    }
    ServiceMessage.status                   = PIPE_OCCUPIED;
    printsvsv("Forwarding the pending requests from task: ", task_id, "from app: ", app_id);
    ServiceMessage.header.header            = task_dest_addr;
    ServiceMessage.header.payload_size      = PKT_SERVICE_SIZE+(2*NUM_MAX_APP_TASKS)+1;
    ServiceMessage.header.service           = TASK_MIGRATION_PENDING;
    ServiceMessage.header.task_id           = task_id;
    ServiceMessage.header.app_id            = app_id;
    ServiceMessage.msg.length               = TaskList[taskslot].appNumTasks;
    for (i = 0; i < NUM_MAX_APP_TASKS; i++){
        ServiceMessage.msg.msg[i] = TaskList[taskslot].TasksMap[i];
        printsvsv("pending [", i, "] = ", TaskList[taskslot].PendingReq[i]);
        ServiceMessage.msg.msg[i+NUM_MAX_APP_TASKS] = TaskList[taskslot].PendingReq[i];
    }
    API_PushSendQueue(SYS_MESSAGE, 0);
    return;
}

void API_MigrationFinished(unsigned int app_id, unsigned int task_id){
    unsigned int mySlot, i;
    do{
        mySlot = API_GetServiceSlot();
        if(mySlot == PIPE_FULL){
            // Runs the NI Handler to send/receive packets, opening space in the PIPE
            prints("Estou preso aqui33...\n");
            API_NI_Handler();
        }
    }while(mySlot == PIPE_FULL);
    
    ServicePipe[mySlot].holder = PIPE_SYS_HOLDER;

    ServicePipe[mySlot].header.header           = makeAddress(0, 0);
    ServicePipe[mySlot].header.payload_size     = PKT_SERVICE_SIZE;
    ServicePipe[mySlot].header.service          = TASK_MIGRATION_FINISHED;
    ServicePipe[mySlot].header.task_id          = task_id;
    ServicePipe[mySlot].header.app_id           = app_id;
    API_PushSendQueue(SERVICE, mySlot);
    
    mySlot = API_GetTaskSlot(task_id, app_id);
    TaskList[mySlot].status = TASK_SLOT_EMPTY;
    
    for(i = 0; i < NUM_MAX_TASKS; i++){
        printsvsv("TaskList[", i, "]status: ", TaskList[i].status );
        if(TaskList[i].status != TASK_SLOT_EMPTY){
            printsvsv("Returning because of: ", i, "TaskList[i].status ", TaskList[i].status);
            i = 0xffffffff;
            break;
        }
    }
    if(i != 0xffffffff){
        API_setFreqIdle();
    }

    vPortFree(TaskList[mySlot].fullAddr);
    prints("free done \n");
    vTaskExitCritical();
    prints("deleting... \n");
    vTaskDelete(TaskList[mySlot].TaskHandler);
    prints("deleted! \n");

    return;
}

void API_UpdatePipe(unsigned int task_id, unsigned int app_id){
    unsigned int msg_destination_taskID, i, tslot = API_GetTaskSlot(task_id, app_id);
    for(i = 0; i < PIPE_SIZE; i++){
        // Update the messages that are inside the PIPE
        msg_destination_taskID = TaskList[tslot].MessagePipe[i].header.destination_task;
        TaskList[tslot].MessagePipe[i].header.header = TaskList[tslot].TasksMap[msg_destination_taskID];

        // Update every MESSAGE REQUEST that is inside the PIPE
        if(ServicePipe[i].header.app_id == app_id && ServicePipe[i].header.service == MESSAGE_REQUEST){
            msg_destination_taskID = ServicePipe[i].header.producer_task;
            ServicePipe[i].header.header = TaskList[tslot].TasksMap[msg_destination_taskID];
        }
    }
    return;
}

void API_ResumeTask(unsigned int task_id, unsigned int app_id){
    unsigned int tslot = API_GetTaskSlot(task_id, app_id);
    printsv("tslot: ", tslot);
    printsv("status: ", TaskList[tslot].status);
    if(TaskList[tslot].status == TASK_SLOT_MIGRATED){
        prints("Starting task after migration...\n");
        BaseType_t xReturned;
        TaskList[tslot].status = TASK_SLOT_RUNNING;

        /*xReturned = xTaskGenericCreate( TaskList[slot].mainAddr,    // pxTaskCode
                                        "LaTask",                   // pcName
                                        16384, //4096,                       // usStackDepth
                                        NULL,                       // pvParameters
                                        tskIDLE_PRIORITY+1,         //uxPriority
                                        &TaskList[slot].TaskHandler,// pxCreatedTask
                                        NULL,                       // puxStackBuffer
                                        &TaskList[slot].memRegion );//xRegions*/
        
        xReturned = xTaskCreate(TaskList[tslot].mainAddr,
                                "LaTask",
                                4096,//8192,//4096,//16384,
                                NULL,
                                tskIDLE_PRIORITY+1,
                                &TaskList[tslot].TaskHandler);
        if( xReturned != pdPASS ){
            prints("ERROR - API_TaskStart!!!\n");
        }
    }
    else{
        prints("Resuming task after migration...\n");
        vTaskResume(TaskList[tslot].TaskHandler);
    }
    return;
}
