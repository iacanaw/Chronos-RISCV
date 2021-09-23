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
        if ( TaskList[ i ].TaskHandler == xHandle ) {
            return( i );
        }
    }
    return ERRO;
}

unsigned int API_GetFreeTaskSlot(){
    unsigned int i;
    for (i = 0; i < NUM_MAX_TASKS; i++){
        if (TaskList[i].status == TASK_SLOT_EMPTY){
            return i;
        }
    }
    return ERRO;
}

unsigned int API_TaskAllocation(unsigned int task_id, unsigned int txt_size, unsigned int bss_size, unsigned int start_point, unsigned int task_app_id){
    unsigned int tslot = API_GetFreeTaskSlot();
    TaskList[tslot].status = TASK_SLOT_WAITING_START;
    TaskList[tslot].waitingMsg = FALSE;
    TaskList[tslot].TaskID = task_id;
    TaskList[tslot].AppID = task_app_id;
    TaskList[tslot].taskSize = 4 * (txt_size + bss_size); // it multiply by four because each word has 32 bits and the memory is addressed by byte - so each word is composed by 4 addresses

    TaskList[tslot].taskAddr = pvPortMalloc(TaskList[tslot].taskSize); //vPortFree(TaskList[tslot].taskAddr);    
    TaskList[tslot].mainAddr =  TaskList[tslot].taskAddr + (4 * start_point);

    return tslot;
}

unsigned int API_GetTaskSlot(unsigned int task_id, unsigned int app_id){
    unsigned int i;
    for( i = 0; i < NUM_MAX_APP_TASKS; i++){
        if(task_id == TaskList[i].TaskID && app_id == TaskList[i].AppID && TaskList[i].status != TASK_SLOT_EMPTY)
            return i;
    }
    return ERRO;
}

void API_TaskStart(unsigned int slot){
    TaskList[slot].status = TASK_SLOT_RUNNING;
    xTaskCreate(TaskList[slot].mainAddr,
                "LaTask",
                1024,
                NULL,
                tskIDLE_PRIORITY+1,
                &TaskList[slot].TaskHandler);
    return;
}


