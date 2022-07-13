#include "globalMaster.h"

#include "chronos.h"
#include "message.h"
#include "packet.h"
#include "services.h"


unsigned int API_getMaxIdxfromRow(float *policyTable, unsigned int row, unsigned int n_collumns, unsigned int n_rows){
    unsigned int max = 0, i;
    for( i = 0; i < n_collumns; i++){
        if( *(policyTable + row*n_collumns + i) >= max){
            max = i;
        }
    }
    return max;
}

////////////////////////////////////////////////////////////
// Informs the Repository that the GLOBALMASTER is ready to receive the application info
void API_RepositoryWakeUp(){
    unsigned int mySlot;

#if THERMAL_MANAGEMENT != 4 // CHARACTERIZE
    // Enters in idle to wait the first FIT value ~50ms
    API_setFreqIdle();
    API_applyFreqScale();
    while(Tiles[0][0].fit == 0){ 
        /* Waits here until the FIT value is not updated */ 
        vTaskDelay(1);
    }
#endif

    do{
        mySlot = API_GetServiceSlot();
        if(mySlot == PIPE_FULL){
            // Runs the NI Handler to send/receive packets, opening space in the PIPE
            prints("Estou preso aqui5...\n");
            API_NI_Handler();
        }
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
unsigned int API_AddApplication(unsigned int appID, unsigned int appPeriod, unsigned int appExec, unsigned int appNTasks){
    int i, slot = API_GetApplicationFreeSlot();
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
    for(i = 0; i < appNTasks; i++){
        applications[slot].tasks[i].status = TASK_TO_ALLOCATE;
    }
    printsv("New application registered - ID: ", appID);
    return slot;
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
    prints("returning erro3\n");
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
    prints("returning erro4\n");
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
            Tiles[m][n].fit = 0;
            Tiles[m][n].taskSlots = NUM_MAX_TASKS;
            Tiles[m][n].taskType = -1;
            random(); // using this loop to iterate the random algorithm...
        }
    }    
    return;
}

////////////////////////////////////////////////////////////
// Update tiles temperature
void API_UpdateTemperature(){
    int m, n, i = 0;
    for (m = 0; m < DIM_X; m++){
        for (n = 0; n < DIM_Y; n++){
            Tiles[m][n].temperatureVariation = SystemTemperature[i] - Tiles[m][n].temperature;
            Tiles[m][n].temperature = SystemTemperature[i];
            i++;
        }
    }
    return;
}


////////////////////////////////////////////////////////////
// Update FIT value
void API_UpdateFIT(){
    int m, n, i = 0;
    for (m = 0; m < DIM_X; m++){
        for (n = 0; n < DIM_Y; n++){
            Tiles[m][n].fit = SystemFIT[i];
            i++;
        }
    }
    return;
}

// Get FIT from PE
unsigned int API_getFIT(unsigned int addr){
    return Tiles[getXpos(addr)][getYpos(addr)].fit;
}

void API_UpdatePIDTM(){
    int i;
    for (i = 0; i < DIM_X * DIM_Y; i++) {
        if ( measuredWindows >= INT_WINDOW ){
            pidStatus.integral[i] = pidStatus.integral[i] - pidStatus.integral_prev[measuredWindows % INT_WINDOW][i];
        }

        pidStatus.integral_prev[measuredWindows % INT_WINDOW][i] = SystemTemperature[i];

        pidStatus.derivative[i] = SystemTemperature[i] - pidStatus.Temperature_prev[i];
        pidStatus.integral[i] = pidStatus.integral[i] + SystemTemperature[i];
        pidStatus.control_signal[i] = KP * SystemTemperature[i] + KI * pidStatus.integral[i] / INT_WINDOW + KD * pidStatus.derivative[i];
        pidStatus.Temperature_prev[i] = SystemTemperature[i];
    }
    return;
}

void API_UpdatePriorityTable(unsigned int score_source[DIM_X*DIM_Y]){
    int i;
    int x, y;
    int index;
    int coolest = -1;
    int addr;
    unsigned int score[DIM_X*DIM_Y];

    memcpy(score, score_source, (DIM_X*DIM_Y*4));

    for (i = (DIM_X * DIM_Y) - 1; i >= 0; i--) {
        // reset the coolest
        coolest = 0;

        for ( y = 0; y < DIM_Y; y++ ) {
            for ( x = 0; x < DIM_X; x++ ) {
                index = x + y * DIM_X;

                if (score[index] < score[coolest] && score[index] != -1)
                    coolest = index;
            }
        }

        addr = (coolest % DIM_X << 8) | coolest / DIM_X;

        priorityMatrix[i] = addr;
    
        score[coolest] = -1;
    }

}

void generateSpiralMatrix() {
    int i, cont = 0, k = 0, l = 0, m = DIM_X, n = DIM_Y;

    while (k < m && l < n) {
        // Print the first row from the remaining rows
        for (i = l; i < n; ++i) {
            priorityMatrix[cont] = (k << 8) | i;
            cont++;
        }
        k++;

        // Print the last column from the remaining columns 
        for (i = k; i < m; ++i) {
            priorityMatrix[cont] = (i << 8) | (n - 1);
            cont++;
        }
        n--;

        // Print the last row from the remaining rows 
        if (k < m) {
            for (i = n - 1; i >= l; --i) {
                priorityMatrix[cont] = ((m - 1) << 8) | i;
                cont++;
            }
            m--;
        }

        // Print the first column from the remaining columns 
        if (l < n) {
            for (i = m - 1; i >= k; --i) {
                priorityMatrix[cont] = (i << 8) | l;
                cont++;
            }
            l++;
        }
    }
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

unsigned int API_GetNextTaskToAllocate(unsigned int tick){
    unsigned int ret = 0xFFFFFFFF;
    int i;
    int j;
    for (i = 0; i < NUM_MAX_APPS; i++){
        // If the application is valid
        if (applications[i].occupied == TRUE){
            // If the nextRun of this application is right now, then release each task!
            if(applications[i].nextRun <= tick){ // && applications[i].nextRun != applications[i].lastStart){
                // Gets the task that needs to be allocated
                for(j = 0; j < applications[i].numTasks; j++){
                    if(applications[i].tasks[j].status == TASK_TO_ALLOCATE){
                        applications[i].tasks[j].status = TASK_ALLOCATING;
                        return (0x00000000 | (i << 16 | j));
                    }
                }
            }
        }
    }
    return ret;
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
                printsv("Starting to release the application ", i);
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

unsigned int API_CheckTaskToAllocate(unsigned int tick){
    int i;
    // Iterate around every possible application
    for (i = 0; i < NUM_MAX_APPS; i++){
        // If the application is valid
        if (applications[i].occupied == TRUE){
            // If the nextRun of this application is right now, then release each task!
            if(applications[i].nextRun <= tick && applications[i].nextRun != applications[i].lastStart){
                return 1;
            }
        }
    }
    return 0;
}

void API_PrintPolicyTable(){
    int i, j;
    vTaskEnterCritical();
    prints("policyTable ");
    for(i = 0; i < N_TASKTYPE; i++){
        //prints(" [");
        for(j = 0; j < N_ACTIONS; j++){
            prints(";");
            printi((int)(policyTable[i][j]*1000));
        }
    }
    prints("\n");
    vTaskExitCritical();
    return;
}

void API_PrintScoreTable(){
    int i, j;
    vTaskEnterCritical();
    prints("policyTable ");
    for(i = 0; i < N_TASKTYPE; i++){
        //prints(" [");
        for(j = 0; j < N_STATES; j++){
            prints("; ");
            printi((int)(scoreTable[i][j]*1000));
        }
    }
    prints("\n");
    vTaskExitCritical();
    return;
}


void API_DealocateTask(unsigned int task_id, unsigned int app_id){
    unsigned int i, tick;
    volatile int flag;
    float newvalue, oldvalue, next_max, reward;
    unsigned int tasktype, takedAct, next_maxid;
    // Hyperparameters
    float alpha = 0.1;
    float gamma = 0.6;
    //////////////////
    applications[app_id].tasks[task_id].status = TASK_FINISHED;
    ////////////////////////////////////
    // Q-learning
    API_PrintPolicyTable();
    reward = (int)(10000000/API_getFIT(applications[app_id].tasks[task_id].addr));
    printsv("reward: ", reward);
    takedAct = applications[app_id].takedAction[task_id];
    tasktype = applications[app_id].taskType[task_id];
    oldvalue = policyTable[tasktype][takedAct];
    next_maxid = API_getMaxIdxfromRow(policyTable, tasktype, N_ACTIONS, N_TASKTYPE);
    next_max = policyTable[tasktype][next_maxid];
    newvalue = (1 - alpha) * oldvalue + alpha * ( reward + gamma * next_max);
    policyTable[tasktype][takedAct] = newvalue;
    ////////////////////////////////////
    // verify if every task has finished
    flag = 1;
    for (i = 0; i < applications[app_id].numTasks; i++){
        printsvsv("checking ", i, "task is: ", applications[app_id].tasks[i].status);
        printsvsv("from app: ", app_id, "running at addr: ", applications[app_id].tasks[i].addr);
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
            for (i = 0; i < applications[app_id].numTasks; i++){
                applications[app_id].tasks[i].status = TASK_TO_ALLOCATE;
            }
            printsv("\t\tThis application still need to run: ", (applications[app_id].appExec - applications[app_id].executed));
            applications[app_id].nextRun = tick + applications[app_id].appPeriod;
        } else { // if the application has finished its runs
            prints("\t\tThis application is DONE!\n");
            applications[app_id].occupied = FALSE;

            // To finish the simulation!!
            for(i = 0; i < NUM_MAX_APPS; i++){
                if (applications[i].occupied == TRUE){
                    return;
                }
            }
            API_SystemFinish = TRUE;
            //_exit(0xfe10);
        }        
    }
    return;
}

// Gets the address of the next tile in the priority list 
unsigned int getNextPriorityAddr(){
    int i;
    unsigned int addr = makeAddress(0,0);
    for(;;){
        // Checks if it's a valid address
        if (priorityMatrix[priorityPointer] != makeAddress(0,0)){
            if (Tiles[getXpos(addr)][getYpos(addr)].taskSlots > 0){
            //for(i = 0; i < NUM_MAX_TASKS; i++){
                //if (Tiles[getXpos(addr)][getYpos(addr)].AppTask[i] == NONE)
                    addr = priorityMatrix[priorityPointer];
            //}
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
    return addr;
    //return 0x101;
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
    if(Tiles[getXpos(addr)][getYpos(addr)].taskSlots > 0 && addr != MASTER_ADDR){
        Tiles[getXpos(addr)][getYpos(addr)].taskSlots = Tiles[getXpos(addr)][getYpos(addr)].taskSlots - 1;
        if( Tiles[getXpos(addr)][getYpos(addr)].taskType < applications[app].taskType[task] || Tiles[getXpos(addr)][getYpos(addr)].taskType == -1){
            Tiles[getXpos(addr)][getYpos(addr)].taskType = applications[app].taskType[task];
        }
        return 1;
    }else {
        prints("returning erro5\n");
        return ERRO;
    }
}

// Clear a slot occupied by a given task
unsigned int API_ClearTaskSlotFromTile(unsigned int addr, unsigned int app, unsigned int task){
    Tiles[getXpos(addr)][getYpos(addr)].taskSlots++;
    if(Tiles[getXpos(addr)][getYpos(addr)].taskSlots == NUM_MAX_TASKS){
        Tiles[getXpos(addr)][getYpos(addr)].taskType = -1;
    }
    if(Tiles[getXpos(addr)][getYpos(addr)].taskSlots > NUM_MAX_TASKS){
        prints("returning erro6\n");
        return ERRO;
    }
    else{
        return 1;
    }
    prints("returning erro7\n");
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
            }
        }
    }
    printsv("The system has # task slots availables: ", sum);
    return sum;
}

void API_RepositoryAllocation(unsigned int app, unsigned int task, unsigned int dest_addr){
    unsigned int mySlot;
    do{
        mySlot = API_GetServiceSlot();
        if(mySlot == PIPE_FULL){
            // Runs the NI Handler to send/receive packets, opening space in the PIPE
            prints("Estou preso aqui6...\n");
            API_NI_Handler();
        }
    }while(mySlot == PIPE_FULL);
    printsv("I got a free service slot!! -> ", mySlot);

    ServicePipe[mySlot].holder = PIPE_SYS_HOLDER;

    ServicePipe[mySlot].header.header           = makeAddress(0, 0) | PERIPH_SOUTH;
    ServicePipe[mySlot].header.payload_size     = PKT_SERVICE_SIZE;
    ServicePipe[mySlot].header.service          = TASK_ALLOCATION_SEND;
    ServicePipe[mySlot].header.task_id          = task;
    ServicePipe[mySlot].header.app_id           = app;
    ServicePipe[mySlot].header.task_dest_addr   = dest_addr;

    API_PushSendQueue(SERVICE, mySlot);
    return;    
}

void API_TaskAllocated(unsigned int task_id, unsigned int app_id){
    unsigned int i;
    applications[app_id].tasks[task_id].status = TASK_ALLOCATED;
    return;
}

void API_ApplicationStart(unsigned int app_id){
    unsigned int i, j;
    printsv("Starting Application: ", app_id);
    for(i = 0; i < applications[app_id].numTasks; i++){
        while(ServiceMessage.status == PIPE_OCCUPIED){
            // Runs the NI Handler to send/receive packets, opening space in the PIPE
            prints("Estou preso aqui7...\n");
            API_NI_Handler();
        }
        // do{
        //     mySlot = API_GetMessageSlot();
        //     if(mySlot == PIPE_FULL){
        //         // Runs the NI Handler to send/receive packets, opening space in the PIPE
        //         API_NI_Handler();
        //     }
        // }while(mySlot == PIPE_FULL);
        printsv("Sending TASK_START to task ", i);
        ServiceMessage.status = PIPE_OCCUPIED;

        ServiceMessage.header.header           = applications[app_id].tasks[i].addr;
        ServiceMessage.header.payload_size     = PKT_SERVICE_SIZE + applications[app_id].numTasks + 1;
        ServiceMessage.header.service          = TASK_START;
        ServiceMessage.header.task_id          = i;
        ServiceMessage.header.app_id           = app_id;
        ServiceMessage.header.task_arg         = 0;
        ServiceMessage.msg.length              = applications[app_id].numTasks;
        for(j = 0; j < applications[app_id].numTasks; j++){
            ServiceMessage.msg.msg[j]          = applications[app_id].tasks[j].addr;
        }
        API_PushSendQueue(SYS_MESSAGE, 0);
    }
    return;
}

void API_StartMigration(unsigned int app_id, unsigned int task_id, unsigned int new_addr){
    unsigned int i, j;
    printsvsv("Starting Migration Process for app: ", app_id, "Task: ", task_id);
    applications[app_id].tasks[task_id].status  = TASK_MIGRATION_REQUEST;
    applications[app_id].newAddr                = new_addr;
    ServiceMessage.status                       = PIPE_OCCUPIED;
    ServiceMessage.header.header                = applications[app_id].tasks[task_id].addr;
    ServiceMessage.header.payload_size          = PKT_SERVICE_SIZE;
    ServiceMessage.header.service               = TASK_MIGRATION_REQUEST;
    ServiceMessage.header.task_id               = task_id;
    ServiceMessage.header.app_id                = app_id;
    API_PushSendQueue(SYS_MESSAGE, 0);
    return;
}


void API_StartTasks(){
    unsigned int j, i, start;
    for(j = 0; j < NUM_MAX_APPS; j++){
        if(applications[j].occupied == TRUE){
            start = TRUE;
            for(i = 0; i < applications[j].numTasks; i++){
                if(applications[j].tasks[i].status == TASK_RESUME){
                    API_ResumeApplication(j);
                    start = FALSE;
                    //break;
                } else if( applications[j].tasks[i].status == TASK_SEND_STALL){
                    start = FALSE;
                    API_Send_StallTask( j, i );
                }
                else if( applications[j].tasks[i].status != TASK_ALLOCATED){
                    start = FALSE;
                    //break;
                } 
            }
            if(start == TRUE){
                printsv("Application is allocated: ", j);
                API_ApplicationStart(j);
                prints("Start command sent to every task...\n");
                for(i = 0; i < applications[j].numTasks; i++){
                    applications[j].tasks[i].status = TASK_STARTED;
                }
            }
        }
    }
    return;
}    

void API_Migration_StallTasks(unsigned int app_id, unsigned int task_id){
    int i;
    printsv("Stalling application for migration: ", app_id);
    applications[app_id].tasks[task_id].status = TASK_MIGRATION_READY;
    for( i = 0; i < applications[app_id].numTasks; i++ ){
        if( i != task_id){
            applications[app_id].tasks[i].status = TASK_SEND_STALL;
            //API_Send_StallTask( app_id, i );
        }
    }
    return;
}

void API_Send_StallTask(unsigned int app_id, unsigned int task_id){
    unsigned int mySlot;
    do{
        mySlot = API_GetServiceSlot();
        if(mySlot == PIPE_FULL){
            // Runs the NI Handler to send/receive packets, opening space in the PIPE
            prints("Estou preso aqui69...\n");
        }
    }while(mySlot == PIPE_FULL);
    printsvsv("Stalling task: ", task_id, "from app: ", app_id);
    applications[app_id].tasks[task_id].status = TASK_STALL_REQUEST;

    ServicePipe[mySlot].holder = PIPE_SYS_HOLDER;

    ServicePipe[mySlot].header.header           = applications[app_id].tasks[task_id].addr;
    ServicePipe[mySlot].header.payload_size     = PKT_SERVICE_SIZE;
    ServicePipe[mySlot].header.service          = TASK_STALL_REQUEST;
    ServicePipe[mySlot].header.task_id          = task_id;
    ServicePipe[mySlot].header.app_id           = app_id;
    API_PushSendQueue(SERVICE, mySlot);
}

void API_StallTask_Ack(unsigned int app_id, unsigned int task_id){
    int i;
    int m_task_id;
    applications[app_id].tasks[task_id].status = TASK_STALL_ACK;
    for( i = 0; i < applications[app_id].numTasks; i++ ){
        if ( (applications[app_id].tasks[i].status != TASK_STALL_ACK) && (applications[app_id].tasks[i].status != TASK_MIGRATION_READY) ){
            printsvsv("Task ", i, "still not stalled. App: ", app_id);
            return;
        }
        if( applications[app_id].tasks[i].status == TASK_MIGRATION_READY ){
            m_task_id = i;
        }
    }
    API_Migration_ForwardTask(app_id, m_task_id, applications[app_id].newAddr);

    prints("Every task is stalled! Forwarding the migrating task... \n");
    return;
}

void API_Migration_ForwardTask( unsigned int app_id, unsigned int task_id, unsigned int newAddr){
    while(ServiceMessage.status == PIPE_OCCUPIED){
        // Runs the NI Handler to send/receive packets, opening space in the PIPE
        prints("Estou preso aqui17...\n");
        API_NI_Handler();
    }
    printsvsv("Forwarding the original task: ", task_id, "from app: ", app_id);
    applications[app_id].tasks[task_id].status = TASK_MIGRATION_FORWARD;
    ServiceMessage.status                   = PIPE_OCCUPIED;
    ServiceMessage.header.header            = applications[app_id].tasks[task_id].addr;
    ServiceMessage.header.payload_size      = PKT_SERVICE_SIZE;
    ServiceMessage.header.service           = TASK_MIGRATION_FORWARD;
    ServiceMessage.header.task_id           = task_id;
    ServiceMessage.header.app_id            = app_id;
    ServiceMessage.header.task_dest_addr    = applications[app_id].newAddr;
    API_PushSendQueue(SYS_MESSAGE, 0);
}

void API_ReleaseApplication(unsigned int app_id, unsigned int task_id){
    int i, j;
    for(i = 0; i< applications[app_id].numTasks; i++){
        applications[app_id].tasks[i].status = TASK_RESUME;
    }
    return;
}

void API_ResumeApplication(unsigned int app_id){
    int i, j;
    for(i = 0; i < applications[app_id].numTasks; i++){
        if(applications[app_id].tasks[i].status == TASK_RESUME){
            while(ServiceMessage.status == PIPE_OCCUPIED){
                // Runs the NI Handler to send/receive packets, opening space in the PIPE
                prints("Estou preso aqui78...\n");
            }

            printsv("Sending TASK_RESUME to task ", i);
            ServiceMessage.status = PIPE_OCCUPIED;

            ServiceMessage.header.header           = applications[app_id].tasks[i].addr;
            ServiceMessage.header.payload_size     = PKT_SERVICE_SIZE + applications[app_id].numTasks + 1;
            ServiceMessage.header.service          = TASK_RESUME;
            ServiceMessage.header.task_id          = i;
            ServiceMessage.header.app_id           = app_id;
            ServiceMessage.header.task_arg         = 0;
            ServiceMessage.msg.length              = applications[app_id].numTasks;
            for(j = 0; j < applications[app_id].numTasks; j++){
                ServiceMessage.msg.msg[j]          = applications[app_id].tasks[j].addr;
            }
            API_PushSendQueue(SYS_MESSAGE, 0);
            applications[app_id].tasks[i].status = TASK_STARTED;
        }
    }
}

void API_Forward_MsgReq(unsigned int requester_task_id, unsigned int app_id, unsigned int producer_task_id){
    unsigned int mySlot;
    if(applications[app_id].tasks[producer_task_id].status != TASK_FINISHED){
        do{
            mySlot = API_GetServiceSlot();
            if(mySlot == PIPE_FULL){
                // Runs the NI Handler to send/receive packets, opening space in the PIPE
                prints("Estou preso aqui121...\n");
            }
        }while(mySlot == PIPE_FULL);
        
        ServicePipe[mySlot].holder = PIPE_SYS_HOLDER;

        ServicePipe[mySlot].header.header           = applications[app_id].tasks[producer_task_id].addr;
        ServicePipe[mySlot].header.payload_size     = PKT_SERVICE_SIZE;
        ServicePipe[mySlot].header.service          = MESSAGE_REQUEST;
        ServicePipe[mySlot].header.task_id          = requester_task_id;
        ServicePipe[mySlot].header.app_id           = app_id;
        ServicePipe[mySlot].header.producer_task_id = producer_task_id;

        API_PushSendQueue(SERVICE, mySlot);
    }
    else {
        prints("ERRO!! MSG REQUEST PERDIDO!\n");
    }
    return;
}

// A utility function to swap two elements 
void swap(int* a, int* b) { 
    int t = *a; 
    *a = *b; 
    *b = t; 
}

/* This function takes last element as pivot, places 
the pivot element at its correct position in sorted 
array, and places all smaller (smaller than pivot) 
to left of pivot and all greater elements to right 
of pivot */
int partition (int arr[], int arr2[], int low, int high) { 
    int pivot = arr[high]; // pivot 
    int i = (low - 1); // Index of smaller element and indicates the right position of pivot found so far
    int j;
    for (j = low; j <= (high - 1); j++) { 
        // If current element is smaller than the pivot 
        if (arr[j] < pivot) { 
            i++; // increment index of smaller element 
            swap(&arr[i], &arr[j]);
            swap(&arr2[i], &arr2[j]);
        } 
    } 
    swap(&arr[i + 1], &arr[high]); 
    swap(&arr2[i + 1], &arr2[high]); 
    return (i + 1); 
} 

/* The main function that implements QuickSort 
arr[] --> Array to be sorted, 
low --> Starting index, 
high --> Ending index */
void quickSort(int arr[], int arr2[], int low, int high){ 
    if (low < high){ 
        /* pi is partitioning index, arr[p] is now 
        at right place */
        int pi = partition(arr, arr2, low, high); 
  
        // Separately sort elements before 
        // partition and after partition 
        quickSort(arr, arr2, low, (pi - 1)); 
        quickSort(arr, arr2, (pi + 1), high); 
    }
}

void API_SortAllocationVectors(unsigned int *temperature_list, int *tempVar_list, unsigned int *fit_list){
    unsigned int temperature[DIM_X*DIM_Y];
    unsigned int fit[DIM_X*DIM_Y];
    int tempVariation[DIM_X*DIM_Y];
    unsigned int addr;
    unsigned int i;
    prints("Creating allocation vectors...\n");
    for(i=0; i<DIM_X*DIM_Y; i++){
        addr = id2addr(i);
        temperature_list[i] = addr;
        tempVar_list[i] = addr;
        fit_list[i] = addr;
        temperature[i] =    Tiles[getXpos(addr)][getYpos(addr)].temperature;
        fit[i] =            Tiles[getXpos(addr)][getYpos(addr)].fit;
        tempVariation[i] =  Tiles[getXpos(addr)][getYpos(addr)].temperatureVariation;
    }
    quickSort(temperature, temperature_list, 0, (DIM_X*DIM_Y)-1);
    quickSort(fit, fit_list, 0, (DIM_X*DIM_Y)-1);
    quickSort(tempVariation, tempVar_list, 0, (DIM_X*DIM_Y)-1);
    /*prints("Vectors are sorted...\n");
    for(i=0; i<DIM_X*DIM_Y; i++){
        printsvsv("ADDR: ", temperature_list[i], "TEMP: ", temperature[i]);
        printsvsv("ADDR: ", fit_list[i], "FIT: ", fit[i]);
        //printsvsv("ADDR: ", temperature_list[i], "VAR: ", temperature[i]);    
    }*/
    return;
}

unsigned int API_getPEState(unsigned int id){
    unsigned int addr = id2addr(id);
    unsigned int x = getXpos(id);
    unsigned int y = getYpos(id); 
    unsigned int xi, yi;
    int state_x, state_y, state_xyz, state_a, state_b, state_abc, a, b, c, z, state;

    unsigned int t0_diagonal, t0_immediate, t1_diagonal, t1_immediate, t2_diagonal, t2_immediate;

    unsigned int immediate[3] = {0,0,0};
    unsigned int diagonal[3] = {0,0,0};
    
    // SOUTH
    if(getSouth(x,y) != -1)
        immediate[getSouth(x,y)]++;
    // SOUTH-WEST
    if(getSouthWest(x, y) != -1)
        diagonal[getSouthWest(x, y)]++;
    // SOUTH-EAST
    if(getSouthEast(x, y) != -1)
        diagonal[getSouthEast(x, y)]++;
    // NORTH
    if(getNorth(x, y) != -1)
        immediate[getNorth(x,y)]++;
    // NORTH-WEST
    if(getNorthWest(x, y) != -1)
        diagonal[getNorthWest(x, y)]++;
    // NORTH-EAST
    if(getNorthEast(x, y) != -1)
        diagonal[getNorthEast(x, y)]++;
    // WEST
    if(getWest(x, y) != -1)
        immediate[getWest(x,y)]++;
    // EAST
    if(getEast(x, y) != -1)
        immediate[getEast(x,y)]++;
     
    x = immediate[0];
    y = immediate[1];
    z = immediate[2];

    a = diagonal[0];
    b = diagonal[1];
    c = diagonal[2];


    state_x = (int)(x ? ((x*x*x - 18*x*x + 107*x) / 6) : 0);
    state_y = (int)(y ? ((11*y - y*y - 2*x*y) / 2) : 0);
    state_xyz = state_x + state_y + z;

    state_a = (int)(a ? ((a*a*a - 18*a*a + 107*a) / 6) : 0);
    state_b = (int)(b ? ((11*b - b*b - 2*a*b) / 2) : 0);
    state_abc = state_a + state_b + c;

    state = state_abc*35 + state_xyz;
    return(state);
}

int getSouth(int x, int y){
    if(y > 0){
        return(Tiles[x][y-1].taskType);
    } else {
        return(-1);
    }
}

int getNorth(int x, int y){
    if(y < DIM_Y-1){
        return(Tiles[x][y+1].taskType);
    } else {
        return(-1);
    }
}

int getEast(int x, int y){
    if(x < DIM_X-1){
        return(Tiles[x+1][y].taskType);
    } else {
        return(-1);
    }
}

int getWest(int x, int y){
    if(x > 0){
        return(Tiles[x-1][y].taskType);
    } else {
        return(-1);
    }
}

int getSouthWest(int x, int y){
    if(x > 0 && y > 0){
        return(Tiles[x-1][y-1].taskType);
    } else {
        return(-1);
    }
}

int getSouthEast(int x, int y){
    if(x > 0 && y > 0){
        return(Tiles[x+1][y-1].taskType);
    } else {
        return(-1);
    }
}

int getNorthWest(int x, int y){
    if(y < DIM_Y-1 && x > 0){
        return(Tiles[x-1][y+1].taskType);
    } else {
        return(-1);
    }
}

int getNorthEast(int x, int y){
    if(x < DIM_X-1 && y < DIM_Y-1){
        return(Tiles[x+1][y+1].taskType);
    } else {
        return(-1);
    }
}
 