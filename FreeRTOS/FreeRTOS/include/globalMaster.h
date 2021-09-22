#ifndef _GLOBAL_MASTER_H_
#define _GLOBAL_MASTER_H_

#include "chronos.h"

////////////////////////////////////////////////////////////
#define NUM_MAX_APPS        10
#define NUM_MAX_APP_TASKS   10
#define NUM_MAX_TASKS       2

#define NONE                -1

// Define TASK status
#define TASK_RUNNING        1
#define TASK_FINISHED       2
#define TASK_ALLOCATING     3
#define TASK_WAITING        4
#define TASK_REALOCATE      5

// Stores the ADDRESS of each tile ordered by the current priotity policy
unsigned int priorityMatrix[DIM_X*DIM_Y];
unsigned int priorityPointer;

// Tile Struct
typedef struct{
    unsigned int temperature;   // In Kelvin
    unsigned int frequency;     // in MegaHertz
    unsigned int AppTask[NUM_MAX_TASKS];
} Tile;

// Tiles Info
Tile Tiles[DIM_X][DIM_Y];

// Task info
typedef struct{
    unsigned int status;
    unsigned int addr;
    unsigned int slot;
} TaskInfo;

// Application Struct
typedef struct{
    unsigned int occupied;
    unsigned int appID;
    unsigned int appPeriod;
    unsigned int appExec;
    unsigned int numTasks;
    TaskInfo     tasks[NUM_MAX_APP_TASKS];
    unsigned int nextRun;
    unsigned int executed;
    unsigned int lastStart;
    unsigned int finishedTasks;
    unsigned int lastFinish;
} Application;
 
// Application Info
Application applications[NUM_MAX_APPS];

// Informs the Repository that the GLOBALMASTER is ready to receive the application info
void API_RepositoryWakeUp();
// Add one Application in the Execution Queue
void API_AddApplication(unsigned int appID, unsigned int appPeriod, unsigned int appExec, unsigned int appNTasks);
// Reset applications vector
void API_ApplicationsReset();
// Gets an application slot that is occipied by a specific application
unsigned int API_GetApplicationSlot();
// Gets an application slot that is free
unsigned int API_GetApplicationFreeSlot();
// Initialize the Tiles Info
void API_TilesReset();
// Generates the Pattern Matrix for Pattern mapping
void GeneratePatternMatrix();
// Checks if there is some task to allocate...
void API_AllocateTasks(unsigned int tick);
// Gets the address of the next tile in the priority list 
unsigned int getNextPriorityAddr();
// Iterates around the system tiles to sum the amount of tasks slots available
unsigned int API_GetSystemTasksSlots();
// Gets a free slot from one given tile
unsigned int API_GetTaskSlotFromTile(unsigned int addr, unsigned int app, unsigned int task);



#endif