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

void API_DealocateTask(unsigned int task_id, unsigned int app_id){
    unsigned int i, tick;
    volatile int flag;
    applications[app_id].tasks[task_id].status = TASK_FINISHED;
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
    if(Tiles[getXpos(addr)][getYpos(addr)].taskSlots > 0){
        Tiles[getXpos(addr)][getYpos(addr)].taskSlots = Tiles[getXpos(addr)][getYpos(addr)].taskSlots - 1;
        return 1;
    }else {
        prints("returning erro5\n");
        return ERRO;
    }
}

// Clear a slot occupied by a given task
unsigned int API_ClearTaskSlotFromTile(unsigned int addr, unsigned int app, unsigned int task){
    Tiles[getXpos(addr)][getYpos(addr)].taskSlots++;
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
