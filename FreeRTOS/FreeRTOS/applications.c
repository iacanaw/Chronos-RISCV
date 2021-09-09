#include "applications.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "message.h"
#include "packet.h"
#include "chronos.h"

// Stores information about each running task
extern Task TaskList[ MAX_LOCAL_TASKS ];

void API_TaskListInit(){
    unsigned int i;
    for (i = 0; i < MAX_LOCAL_TASKS; i++){
        TaskList[ i ].TaskID = (unsigned int)NULL;
        TaskList[ i ].AppID = (unsigned int)NULL;
        TaskList[ i ].TaskHandler = NULL;
        TaskList[ i ].MsgToReceive = NULL;
        TaskList[ i ].waitingMsg = FALSE;
    }
    return;
}

unsigned int API_GetTaskID(){
    // The handle of the currently running ( calling ) task on the kernel.
    TaskHandle_t xHandle = xTaskGetCurrentTaskHandle();
    unsigned int i = 0;
    for ( i = 0 ; i < MAX_LOCAL_TASKS ; i++ ) {
        if ( TaskList[ i ].TaskHandler == xHandle ) {
            return( i );
        }
    }
    return ERRO;
}

unsigned int API_GetTaskSlot(){
    unsigned int i;
    for (i = 0; i < MAX_LOCAL_TASKS; i++){
        if (TaskList[ i ].TaskHandler == NULL){
            return i;
        }
    }
    return ERRO;
}

void API_CreateTask(){
    TaskHandle_t taskHandler;
    unsigned int tslot = API_GetTaskSlot();
    //xTaskCreate( vUartTestTask1, "UART1", 1000, NULL, 2, &taskHandler);
    TaskList[ tslot ].TaskID = 11;
    TaskList[ tslot ].AppID = 66;
    TaskList[ tslot ].TaskHandler = taskHandler;
    return;
}