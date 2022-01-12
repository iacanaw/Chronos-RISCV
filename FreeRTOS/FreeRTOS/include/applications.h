#ifndef __APPLICATIONS_H__
#define __APPLICATIONS_H__

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "message.h"
#include "packet.h"
#include "chronos.h"
//#include "globalMaster.h"

#define TASK_SLOT_EMPTY             0
#define TASK_SLOT_RUNNING           1
#define TASK_SLOT_WAITING_START     2
#define TASK_SLOT_READY             3
#define TASK_SLOT_SUSPENDED         4
#define TASK_SLOT_BLOQUED           5

typedef struct {
    unsigned int status;
    MemoryRegion_t memRegion; 
    unsigned int TaskID;
    unsigned int AppID;
    TaskHandle_t TaskHandler;
    volatile unsigned int waitingMsg;
    unsigned int MsgToReceive;
    unsigned int taskSize;
    unsigned int taskAddr;
    unsigned int mainAddr;
    volatile MessagePacket MessagePipe[PIPE_SIZE];
    unsigned int PendingReq[NUM_MAX_APP_TASKS];
    unsigned int appNumTasks;
    unsigned int TasksMap[NUM_MAX_APP_TASKS];
} Task;

// Vector of Tasks running in a given PE
volatile Task TaskList[NUM_MAX_TASKS];

// Getts the slot were the task is located
unsigned int API_GetTaskSlot(unsigned int task_id, unsigned int app_id);
// Gets an empty taskslot
unsigned int API_GetFreeTaskSlot();
// Initiate the TaskList with NULL values
void API_TaskListInit();
// Returns the running taskID
unsigned int API_GetCurrentTaskSlot();
// Creates a new task filling the TaskList
unsigned int API_TaskAllocation(unsigned int task_id, unsigned int txt_size, unsigned int bss_size, unsigned int start_point, unsigned int task_app_id);
// Starts the execution of a task that is alocated in a given spot
void API_TaskStart(unsigned int slot);

void API_FinishRunningTask();
#endif
