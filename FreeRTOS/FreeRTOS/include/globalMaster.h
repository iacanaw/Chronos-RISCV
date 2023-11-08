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

unsigned int globalID;

// Finish 
volatile unsigned int API_SystemFinish;

// Migration
volatile unsigned int Migration_Status;

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
    volatile unsigned int fit;  // Failures in time * 100
    unsigned int tempWindow;    // stores the avg temperature in the previous 32ms
    int temperatureVariation;   // In Kelvin
    int taskSlots;
    int taskType;                //
    volatile unsigned int clusterCount;  // stores the amount of clusters that this PE is participating
} Tile;

// Tiles Info
volatile Tile Tiles[DIM_X][DIM_Y];

// Task info
typedef struct{ 
    unsigned int status; 
    unsigned int addr;
    unsigned int slot;
    unsigned int migration;
} TaskInfo;

// Application Struct
typedef struct{
    unsigned int occupied;
    unsigned int appID;
    unsigned int appPeriod;
    unsigned int arrived;
    unsigned int appExec;
    unsigned int numTasks;
    unsigned int cluster_addr;
    unsigned int cluster_size;
    TaskInfo     tasks[NUM_MAX_APP_TASKS];
    unsigned int nextRun;
    unsigned int deadline;
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

// returns one random iteration value (ranging from 16-3248)
unsigned int API_GetRandomIteration(int i);
// 
unsigned int API_getMaxIdxfromRow(float *policyTable, unsigned int row, unsigned int n_collumns, unsigned int n_rows);
// Informs the Repository that the GLOBALMASTER is ready to receive the application info
void API_RepositoryWakeUp();
// Add one Application in the Execution Queue
unsigned int API_AddApplication(unsigned int appID, unsigned int appPeriod, unsigned int appExec, unsigned int appNTasks, unsigned int appDeadline);
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
void API_AllocateTasks(unsigned int tick, unsigned int start);
// Gets the address of the next tile in the priority list 
unsigned int getNextPriorityAddr(unsigned int start);
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

void API_Clusterless(unsigned int app);

void API_FindBestCluster(unsigned int app);

unsigned int API_CheckCluster(unsigned int base_addr, unsigned int cluster_size, unsigned int size);

unsigned int API_GetClusterOccupation(unsigned int base_addr, unsigned int cluster_size);

unsigned int API_minClusterSize(unsigned int size);

void API_PrintOccupation(int tick);

void API_Migration_Refused(unsigned int task_id, unsigned int app_id, unsigned int why, unsigned int addr);

unsigned int API_SelectTaskFromPE_Migration(int pe_id);

unsigned int API_SelectTask_Migration_Temperature(int threshold);

unsigned int API_isApplicationReady(unsigned int app);

void randPositions(int arr[], int arr2[], int low, int high);

#endif