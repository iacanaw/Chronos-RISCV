#include "globalMaster.h"

#include "chronos.h"
#include "message.h"
#include "packet.h"
#include "services.h"

////////////////////////////////////////////////////////////
// Informs the Repository that the GLOBALMASTER is ready to receive the application info
void API_RepositoryWakeUp(){
    unsigned int mySlot;
    do{
        mySlot = API_GetServiceSlot();
        if(mySlot == PIPE_FULL) vTaskDelay(1);
    }while(mySlot == PIPE_FULL);
    //printsv("I got a free service slot!! -> ", mySlot);

    ServicePipe[mySlot].holder = PIPE_SYS_HOLDER;

    ServicePipe[mySlot].header.header       = makeAddress(0, 0) | PERIPH_SOUTH;
    ServicePipe[mySlot].header.payload_size = PKT_SERVICE_SIZE;
    ServicePipe[mySlot].header.service      = REPOSITORY_WAKEUP;

    API_PushSendQueue(SERVICE, mySlot);
    return;    
}

////////////////////////////////////////////////////////////
// Add one Application in the Execution Queue
void API_AddApplication(unsigned int appID, unsigned int appPeriod, unsigned int appExec, unsigned int appNTasks){
    int slot = API_GetApplicationFreeSlot();
    applications[slot].occupied = TRUE;
    applications[slot].appID = appID;
    applications[slot].appPeriod = appPeriod;
    applications[slot].appExec = appExec;
    applications[slot].numTasks = appNTasks;
    applications[slot].nextRun = xTaskGetTickCount() + appPeriod;
    applications[slot].executed = 0;
    applications[slot].lastStart = -1;
    applications[slot].finishedTasks = 0;
    applications[slot].lastFinish = 0;
    printsv("New application registered - ID: ", appID);
    return;
}

////////////////////////////////////////////////////////////
// Gets an application slot that is occipied by a specific application
unsigned int API_GetApplicationSlot(unsigned int appID){
    int i;
    for (i = 0; i < NUM_MAX_APPS; i++){
        if (applications[i].occupied == TRUE && applications[i].appID == appID){
            return i;
        }
    }
    return ERRO;
}


////////////////////////////////////////////////////////////
// Gets an application slot that is free
unsigned int API_GetApplicationFreeSlot(){
    int i;
    for (i = 0; i < NUM_MAX_APPS; i++){
        if (applications[i].occupied == FALSE){
            return i;
        }
    }
    return ERRO;
}

////////////////////////////////////////////////////////////
// Reset applications vector
void API_ApplicationsReset(){
    int i;
    for (i = 0; i < NUM_MAX_APPS; i++){
        applications[i].occupied = FALSE;
    }
    return;
}

////////////////////////////////////////////////////////////
// Initialize the Tiles Info
void API_TilesReset(){
    int i, m, n;
    for (m = 0; m < DIM_X; m++){
        for (n = 0; n < DIM_Y; n++){
            Tiles[m][n].temperature = 273;
            Tiles[m][n].frequency = 1000;
            Tiles[m][n].taskSlots = NUM_MAX_TASKS;
            /*for(i = 0; i < NUM_MAX_TASKS; i++){
                Tiles[m][n].AppTask[i] = NONE;
            }*/
        }
    }    
    return;
}

// Generates the Pattern Matrix for Pattern mapping
void GeneratePatternMatrix(){
    int i, aux;
    aux = 0;
    priorityPointer = 0;
    //prints("Testando Pattern: \n");
    for(i=0; i<(DIM_X*DIM_Y); i++){
        //printi(i); prints(": "); printi(aux); prints("\n");
        priorityMatrix[i] = (aux / DIM_X << 8) | aux % DIM_X;

        if(DIM_X%2 == 0){
            if(aux<=((DIM_X*DIM_Y)-3)){
                if((aux+2)%DIM_X==0){
                    aux += 3;
                }
                else if((aux+1)%DIM_X==0){
                    aux += 1;
                }
                else{
                    aux += 2;
                }
            }
            else{
                aux = 1;
            }
        } 
        else{
            if(aux<=((DIM_X*DIM_Y)-3))
                aux += 2;
            else
                aux = 1;
        }    
    }
    return;
}

// Checks if there is some task to allocate...
void API_AllocateTasks(unsigned int tick){
    int i, j;
    int addr, slot;
    // Iterate around every possible application
    for (i = 0; i < NUM_MAX_APPS; i++){
        // If the application is valid
        if (applications[i].occupied == TRUE){

            // If the nextRun of this application is right now, then release each task!
            if(applications[i].nextRun <= tick && applications[i].nextRun != applications[i].lastStart){

                // If the system has space to accept every task
                if(applications[i].numTasks <= API_GetSystemTasksSlots()){
                    
                    // Iterates around each task of this application
                    for(j = 0; j < applications[i].numTasks; j++){

                        for(;;){
                            addr = getNextPriorityAddr();
                            slot = API_GetTaskSlotFromTile(addr, i, j);
                            if (slot != ERRO)
                                break;
                        }
                        applications[i].tasks[j].status = TASK_ALLOCATING;
                        applications[i].tasks[j].addr = addr;
                        applications[i].tasks[j].slot = slot;
                        API_RepositoryAllocation(i, j, addr);
                    }

                    // Resets the lastStart
                    applications[i].lastStart = applications[i].nextRun;
                }
            }
        }
    }

    return;
}

void API_DealocateTask(unsigned int task_id, unsigned int app_id){
    unsigned int i, flag, tick;
    applications[app_id].tasks[task_id].status = TASK_FINISHED;
    // verify if every task has finished
    flag = 1;
    for (i = 0; i < applications[app_id].numTasks; i++){
        //printsvsv("checking ", i, "task is: ", applications[app_id].tasks[i].status);
        if(applications[app_id].tasks[i].status != TASK_FINISHED){
            flag = 0;
            break;
        }
    }
    // in positive case
    if(flag){
        // register that the application has executed another time
        tick = xTaskGetTickCount();
        applications[app_id].executed++;
        printsvsv("Application ", app_id, "was executed in ", (tick - applications[app_id].lastStart));
        applications[app_id].lastFinish = tick;
        // if the application must run another time
        if(applications[app_id].appExec > applications[app_id].executed){
            printsv("\t\tThis application still need to run: ", (applications[app_id].appExec - applications[app_id].executed));
            applications[app_id].nextRun = tick + applications[app_id].appPeriod;
        } else { // if the application has finished its runs
            prints("\t\tThis application is DONE!\n");
            applications[app_id].occupied = FALSE;
        }        
    }
    return;
}

// Gets the address of the next tile in the priority list 
unsigned int getNextPriorityAddr(){
    /*int i;
    unsigned int addr = makeAddress(0,0);
    for(;;){
        // Checks if it's a valid address
        if (priorityMatrix[priorityPointer] != makeAddress(0,0)){
            for(i = 0; i < NUM_MAX_TASKS; i++){
                if (Tiles[getXpos(addr)][getYpos(addr)].AppTask[i] == NONE)
                    addr = priorityMatrix[priorityPointer];
            }
        }

        // Increments the priorityPointer
        priorityPointer++;
        if (priorityPointer == DIM_X*DIM_Y)
            priorityPointer = 0;

        // If we found a new valid address, return
        if( addr != makeAddress(0,0))
            break;
    }
    return addr;*/
    return 0x101;
}

// Gets a free slot from one given tile
unsigned int API_GetTaskSlotFromTile(unsigned int addr, unsigned int app, unsigned int task){
    /*int i;
    printsv("procurnado um slot no tile ", addr);
    for(i = 0; i < NUM_MAX_TASKS; i++){
        if(Tiles[getXpos(addr)][getYpos(addr)].AppTask[i] == NONE){
            printsv("\t achei!! > ", i);
            Tiles[getXpos(addr)][getYpos(addr)].AppTask[i] = (app << 16) | task;
            return i;
        }
    }*/
    if(Tiles[getXpos(addr)][getYpos(addr)].taskSlots > 0){
        Tiles[getXpos(addr)][getYpos(addr)].taskSlots = Tiles[getXpos(addr)][getYpos(addr)].taskSlots - 1;
        return 1;
    }else {
        return ERRO;
    }
}

// Clear a slot occupied by a given task
unsigned int API_ClearTaskSlotFromTile(unsigned int addr, unsigned int app, unsigned int task){
    Tiles[getXpos(addr)][getYpos(addr)].taskSlots++;
    if(Tiles[getXpos(addr)][getYpos(addr)].taskSlots >= NUM_MAX_TASKS){
        return ERRO;
    }
    else{
        return 1;
    }
    /*int i;
    for(i = 0; i < NUM_MAX_TASKS; i++){
        if(Tiles[getXpos(addr)][getYpos(addr)].AppTask[i] == (app << 16) | task){
            printsv("limpei o slot tile", addr);
            Tiles[getXpos(addr)][getYpos(addr)].AppTask[i] = NONE;
            return 1;
        }
    }*/
    return ERRO;
}

// Iterates around the system tiles to sum the amount of tasks slots available
unsigned int API_GetSystemTasksSlots(){
    int m, n, i, sum;
    sum = 0;
    for(m = 0; m < DIM_X; m++){
        for(n = 0; n < DIM_Y; n++){
            if(makeAddress(m,n) != makeAddress(0,0)){
                sum += Tiles[m][n].taskSlots;
                /*for(i = 0; i < NUM_MAX_TASKS; i++){
                    if(Tiles[m][n].AppTask[i] == NONE){
                        sum++;
                    }
                }*/
            }
        }
    }
    return sum;
}

void API_RepositoryAllocation(unsigned int app, unsigned int task, unsigned int dest_addr){
    unsigned int mySlot;
    do{
        mySlot = API_GetServiceSlot();
        if(mySlot == PIPE_FULL){ 
            vTaskDelay(1);
            //prints("esperando slot\n");
        }
    }while(mySlot == PIPE_FULL);
    printsv("I got a free service slot!! -> ", mySlot);

    ServicePipe[mySlot].holder = PIPE_SYS_HOLDER;

    ServicePipe[mySlot].header.header           = makeAddress(0, 0) | PERIPH_SOUTH;
    ServicePipe[mySlot].header.payload_size     = PKT_SERVICE_SIZE;
    ServicePipe[mySlot].header.service          = TASK_ALLOCATION_SEND;
    ServicePipe[mySlot].header.task_id          = task;
    ServicePipe[mySlot].header.task_app_id      = app;
    ServicePipe[mySlot].header.task_dest_addr   = dest_addr;

    API_PushSendQueue(SERVICE, mySlot);
    return;    
}

void API_TaskAllocated(unsigned int task_id, unsigned int app_id){
    unsigned int i;
    applications[app_id].tasks[task_id].status = TASK_ALLOCATED;

    for(i = 0; i < applications[app_id].numTasks; i++){
        if(applications[app_id].tasks[i].status != TASK_ALLOCATED){
            return;
        }
    }
    printsv("Application allocated: ", app_id);
    API_ApplicationStart(app_id);
    prints("\tStart command sent to every task.\n");
    return;
}

void API_ApplicationStart(unsigned int app_id){
    unsigned int i, j;
    unsigned int mySlot;
    for(i = 0; i < applications[app_id].numTasks; i++){
        do{
            vPortEnterCritical();
            mySlot = API_GetMessageSlot();
            if(mySlot == PIPE_FULL){ 
                vPortExitCritical();
                vTaskDelay(1);
            }
        }while(mySlot == PIPE_FULL);

        MessagePipe[mySlot].holder = PIPE_SYS_HOLDER;

        MessagePipe[mySlot].header.header           = applications[app_id].tasks[i].addr;
        MessagePipe[mySlot].header.payload_size     = PKT_SERVICE_SIZE + applications[app_id].numTasks + 1;
        MessagePipe[mySlot].header.service          = TASK_START;
        MessagePipe[mySlot].header.task_id          = i;
        MessagePipe[mySlot].header.task_app_id      = app_id;
        MessagePipe[mySlot].msg.length              = applications[app_id].numTasks;
        for(j = 0; j < applications[app_id].numTasks; j++){
            MessagePipe[mySlot].msg.msg[j]          = applications[app_id].tasks[j].addr;
        }
        API_PushSendQueue(MESSAGE, mySlot);
        vPortExitCritical();
    }
}
