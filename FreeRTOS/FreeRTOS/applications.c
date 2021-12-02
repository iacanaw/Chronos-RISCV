#include "applications.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "message.h"
#include "packet.h"
#include "chronos.h"

// Stores information about each running task
//extern volatile Task TaskList[ NUM_MAX_TASKS ];

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

unsigned int API_TaskAllocation(unsigned int task_id, unsigned int txt_size, unsigned int bss_size, unsigned int start_point, unsigned int task_app_id){
    unsigned int tslot = API_GetFreeTaskSlot();
    int i;
    if(tslot == ERRO){
        prints("DEU RUIM NO API_GetFreeTaskSlot()\n");
    }
    printsv("Got slot: ", tslot);
    TaskList[tslot].status = TASK_SLOT_WAITING_START;
    TaskList[tslot].waitingMsg = FALSE;
    TaskList[tslot].TaskID = task_id;
    TaskList[tslot].AppID = task_app_id;
    TaskList[tslot].taskSize = 4 * (txt_size + bss_size); // it multiply by four because each word has 32 bits and the memory is addressed by byte - so each word is composed by 4 addresses
    printsv("Task total size (txt+bss): ", TaskList[tslot].taskSize);
    TaskList[tslot].taskAddr = (unsigned int)pvPortMalloc(TaskList[tslot].taskSize+64);
    printsv("Task addr: ", TaskList[tslot].taskAddr);
    TaskList[tslot].mainAddr =  TaskList[tslot].taskAddr + (4 * start_point);

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
    for( i = 0; i < NUM_MAX_APP_TASKS; i++){
        if(task_id == TaskList[i].TaskID && app_id == TaskList[i].AppID && TaskList[i].status != TASK_SLOT_EMPTY)
            return i;
    }
    prints("returning erro2\n");
    return ERRO;
}

void API_TaskStart(unsigned int slot){
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
                            9192,//8192,//4096,//16384,
                            NULL,
                            tskIDLE_PRIORITY+1,
                            &TaskList[slot].TaskHandler);
    if( xReturned != pdPASS ){
        prints("ERROR - API_TaskStart!!!\n");
    }
    return;
}


void API_FinishRunningTask(){
    int i;
    unsigned int slot = API_GetCurrentTaskSlot();
    printsvsv("Finishing task:", TaskList[slot].TaskID, "app: ", TaskList[slot].AppID);
    printsv("From slot: ", slot);
    while(API_checkPipe(slot) == 1){
        vTaskDelay(1);
    }
    vTaskEnterCritical();
    printsvsv("Task ", TaskList[slot].TaskID, "deleted with sucsess! From application ", TaskList[slot].AppID);
    TaskList[slot].status = TASK_SLOT_EMPTY;
    
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
    API_SendFinishTask(TaskList[slot].TaskID, TaskList[slot].AppID);
    vPortFree(TaskList[slot].taskAddr);
    vTaskExitCritical();
    vTaskDelete(TaskList[slot].TaskHandler);
    return;
}

