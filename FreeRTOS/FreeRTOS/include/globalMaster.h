#ifndef _GLOBAL_MASTER_H_
#define _GLOBAL_MASTER_H_

#include "chronos.h"
#include "message.h"
#include "packet.h"
#include "services.h"

////////////////////////////////////////////////////////////
#define NONE                -1

// Define TASK status
#define TASK_FINISHED       2
#define TASK_ALLOCATING     3
#define TASK_ALLOCATED      4
#define TASK_STARTED        5
#define TASK_TO_ALLOCATE    6

// PIDTM defines
#define KP 10
#define KI 10
#define KD 10
#define INT_WINDOW 10

#define MASTER_ADDR 0x00000000

// q-learning stuff
#define N_TASKTYPE  3
#define N_STATES    35

// Finish 
volatile unsigned int API_SystemFinish;

// Stores the ADDRESS of each tile ordered by the current priotity policy
unsigned int priorityMatrix[DIM_X*DIM_Y];
unsigned int priorityPointer;

// Stores the estimated system temperature and FIT
volatile unsigned int SystemTemperature[DIM_X*DIM_Y];
volatile unsigned int SystemFIT[DIM_X*DIM_Y];
volatile unsigned int temperatureUpdated;

// Tile Struct
typedef struct{
    unsigned int temperature;   // In Kelvin
    unsigned int frequency;     // in MegaHertz
    unsigned int fit;           // Failures in time * 100
    int temperatureVariation;   // In Kelvin
    unsigned int taskSlots;
    int taskType                //
} Tile;

// Tiles Info
volatile Tile Tiles[DIM_X][DIM_Y];

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
    unsigned int newAddr;
    unsigned int taskType[NUM_MAX_APP_TASKS];
    unsigned int takedAction[NUM_MAX_APP_TASKS];
} Application;
 
// PIDTM stuff
volatile unsigned int measuredWindows;
typedef struct{
    // PID control variables
    unsigned int derivative[DIM_Y * DIM_X];
    unsigned int integral[DIM_Y * DIM_X];
    unsigned int integral_prev[INT_WINDOW][DIM_Y * DIM_X];
    unsigned int Temperature_prev[DIM_Y * DIM_X];
    unsigned int control_signal[DIM_Y * DIM_X];
} PIDinfo;
volatile PIDinfo pidStatus;


// Application Info
volatile Application applications[NUM_MAX_APPS];

// 
unsigned int API_getMaxIdxfromRow(float *policyTable, unsigned int row, unsigned int n_collumns, unsigned int n_rows);
// Informs the Repository that the GLOBALMASTER is ready to receive the application info
void API_RepositoryWakeUp();
// Add one Application in the Execution Queue
unsigned int API_AddApplication(unsigned int appID, unsigned int appPeriod, unsigned int appExec, unsigned int appNTasks);
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
// Sends the packet to the Repository, informing were it must send the task code 
void API_RepositoryAllocation(unsigned int app, unsigned int task, unsigned int dest_addr);

void API_DealocateTask(unsigned int task_id, unsigned int app_id);

unsigned int API_ClearTaskSlotFromTile(unsigned int addr, unsigned int app, unsigned int task);

void API_ApplicationStart(unsigned int app_id);

void API_TaskAllocated(unsigned int task_id, unsigned int app_id);

void API_StartTasks();

void API_UpdatePriorityTable(unsigned int score_source[DIM_X*DIM_Y]);

void API_UpdatePIDTM(); 

void API_UpdateTemperature();

void API_UpdateFIT();

void API_PrintPolicyTable();

unsigned int API_GetSmallerFITCluster(unsigned int size);

unsigned int API_CheckCluster(unsigned int base_addr, unsigned int cluster_size, unsigned int size);

unsigned int API_minClusterSize(unsigned int size);

#endif