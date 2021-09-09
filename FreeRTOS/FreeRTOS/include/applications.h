#ifndef __APPLICATIONS_H__
#define __APPLICATIONS_H__

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "message.h"
#include "packet.h"
#include "chronos.h"

typedef struct {
    unsigned int TaskID;
    unsigned int AppID;
    TaskHandle_t TaskHandler;
    unsigned int waitingMsg;
    Message* MsgToReceive;
} Task;

// Initiate the TaskList with NULL values
void API_TaskListInit();
// Returns the running taskID
unsigned int API_GetTaskID();
// Gets an empty slot inside the TaskList
unsigned int API_GetTaskSlot();
// Creates a new task filling the TaskList
void API_CreateTask();
#endif
