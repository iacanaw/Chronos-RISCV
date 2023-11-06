#include "globalMaster.h"

#include "chronos.h"
#include "message.h"
#include "packet.h"
#include "services.h"

unsigned int API_SelectTask_Migration_Temperature(int threshold){
    int thr_temp = (threshold+273)*100; // transforming to kelvin
    int sel_temp = 0;
    unsigned int i, j, i_temp, aux, selected = -1;
    if (Migration_Status == TRUE){
        selected = -1;
        prints("Ongoing migration...\n");
    } else {
        for( i = 0; i < NUM_MAX_APPS; i++){
            for ( j = 0; j < applications[i].numTasks; j++){
                if ( applications[i].tasks[j].status == TASK_MIGRATION_FORWARD || applications[i].tasks[j].status == TASK_RESUME ){
                    return ERRO;
                }
            }
        }

        for( i = 0; i < DIM_X*DIM_Y; i++ ){
            i_temp = Tiles[getXpos(id2addr(i))][getYpos(id2addr(i))].temperature;
            if (sel_temp < i_temp && i_temp >= thr_temp){
                // printsvsv("Looking for a task at PE ", i, " temperature: ", (i_temp/100)-273);
                aux = API_SelectTaskFromPE_Migration( i );
                // printsv("AUX: ", aux);
                if ( aux != ERRO ){
                    selected = aux;
                    sel_temp = i_temp;
                }
            }
        }
    }
    return selected;
}

unsigned int API_SelectTaskFromPE_Migration(int pe_id){
    int i, j, k;
    // iterates around every application
    for( i = 0; i <= NUM_MAX_APPS; i++ ){
        if(applications[i].occupied == TRUE){
            // iterates for every task
            for ( j = 0; j < applications[i].numTasks; j++){
                // checks if the task is running
                if( applications[i].tasks[j].status == TASK_STARTED ){
                    // checks if the task is running in the selected PE
                    if( applications[i].tasks[j].addr == id2addr(pe_id) ){
                        // checks if the task can migrate
                        if( applications[i].tasks[j].migration == TRUE ){
                            // checks if every task from this application is running
                            for (k = 0; k < applications[i].numTasks; k++){
                                // printsvsv("app ", i, "task ", j);
                                // printsv("status: ", applications[i].tasks[k].status);
                                if( applications[i].tasks[k].status != TASK_STARTED){
                                    k = ERRO;
                                    break;
                                }
                            }
                            if (k != ERRO){
                                return (i << 16) | j;
                            }
                        }
                    }
                }
            }
        }
    }
    return ERRO;
}

void API_PrintOccupation(int tick){
    int availableSlots, occupiedPES, i;
    availableSlots = 0;
    occupiedPES = 0;
    vTaskEnterCritical();
    for( i = 1; i < DIM_X*DIM_Y; i++){
        availableSlots += Tiles[getXpos(id2addr(i))][getYpos(id2addr(i))].taskSlots;
        if (Tiles[getXpos(id2addr(i))][getYpos(id2addr(i))].taskSlots != NUM_MAX_TASKS){
            occupiedPES++;
        }
    }
    availableSlots = ((((DIM_X*DIM_Y)*NUM_MAX_TASKS)-availableSlots)*100) / ((DIM_X*DIM_Y)*NUM_MAX_TASKS);
    occupiedPES = (occupiedPES*100) / (DIM_X*DIM_Y);
    printsvsv("---->Slot_occ: ", availableSlots, "tick: ", tick);
    printsvsv("---->PE_occ: ", occupiedPES, "tick: ", tick);
    vTaskExitCritical();
}

unsigned int API_getMaxIdxfromRow(float *policyTable, unsigned int row, unsigned int n_collumns, unsigned int n_rows){
    unsigned int i;
    float max = 0;
    for( i = 0; i < n_collumns; i++){
        if( *(policyTable + row*n_collumns + i) >= max){
            max = i;
        }
    }
    return (int)max;
}

////////////////////////////////////////////////////////////
// Informs the Repository that the GLOBALMASTER is ready to receive the application info
void API_RepositoryWakeUp(){
    unsigned int mySlot;
    API_setFreqIdle();
    API_applyFreqScale();
    while(xTaskGetTickCount() < 40){ /*waits here */ }
    globalID = 100;
    Migration_Status = FALSE;

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
unsigned int API_AddApplication(unsigned int appID, unsigned int appPeriod, unsigned int appExec, unsigned int appNTasks, unsigned int appDeadline){
    int i, slot = API_GetApplicationFreeSlot();
    applications[slot].occupied = TRUE;
    applications[slot].appID = appID;
    applications[slot].appPeriod = appPeriod;
    applications[slot].arrived = xTaskGetTickCount();
    applications[slot].appExec = appExec;
    applications[slot].numTasks = appNTasks;
    applications[slot].nextRun = xTaskGetTickCount();// + appPeriod;
    applications[slot].deadline = appDeadline;
    applications[slot].executed = 0;
    applications[slot].lastStart = -1;
    applications[slot].finishedTasks = 0;
    applications[slot].lastFinish = 0;

    if(applications[slot].appPeriod < applications[slot].deadline){
        prints("WARNING: The user provided an app Period smaller than the app Deadline => appPeriod = deadline\n");
        applications[slot].appPeriod = applications[slot].deadline;
    }
    printsvsv("APP: ", appID, "NEXTRUN: ", applications[slot].nextRun);
    printsvsv("DEADLINE:", applications[slot].deadline, "PERIOD: ", applications[slot].appPeriod);

    for(i = 0; i < appNTasks; i++){
        applications[slot].tasks[i].status = TASK_TO_ALLOCATE;
        applications[slot].tasks[i].migration = TRUE;
    }
    printsv("New application registered - ID: ", appID);
    printsv("Deadline ", applications[slot].deadline);
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
            Tiles[m][n].fit = 100000;
            Tiles[m][n].taskSlots = NUM_MAX_TASKS;
            Tiles[m][n].taskType = -1;
            Tiles[m][n].clusterCount = 0;
            random(); // using this loop to iterate the random algorithm...
        }
    }    
    return;
}

////////////////////////////////////////////////////////////
// Update tiles temperature
void API_UpdateTemperature(){
    int m, n, i = 0;
    for (m = 0; m < DIM_Y; m++){
        for (n = 0; n < DIM_X; n++){
            Tiles[n][m].temperatureVariation = SystemTemperature[i] - Tiles[n][m].temperature;
            Tiles[n][m].temperature = SystemTemperature[i];
            i++;
        }
    }
    return;
}


////////////////////////////////////////////////////////////
// Update FIT value
// void API_UpdateFIT(){
//     int m, n, i = 0;
//     for (m = 0; m < DIM_X; m++){
//         for (n = 0; n < DIM_Y; n++){
//             Tiles[n][m].fit = SystemFIT[i];
//             i++;
//         }
//     }
//     return;
// }

// Update FIT value
void API_UpdateFIT(){
    int m, n, i = 0;
    unsigned int avgFit = 0;
    unsigned int totalFit = 0;
    for (m = 0; m < DIM_Y; m++){
        for (n = 0; n < DIM_X; n++){
            avgFit = Tiles[n][m].fit;
            // printsv("avgFit ", avgFit);
            totalFit = Tiles[n][m].fit << 5;
            // printsv("totalFit1 ", totalFit);
            totalFit = totalFit - avgFit;
            // printsv("totalFit2 ", totalFit);
            totalFit = totalFit + SystemFIT[i];
            // printsv("totalFit3 ", totalFit);
            Tiles[n][m].fit = totalFit >> 5;
            // printsv("FIT ", Tiles[n][m].fit);
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

        addr = ((coolest % DIM_X) << 8) | (coolest / DIM_X);

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

void generateWorstMatrix() {
    int i;
    for(i = 0; i < DIM_X*DIM_Y; i++){
        priorityMatrix[i] = id2addr(i);
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
void API_AllocateTasks(unsigned int tick, unsigned int start){
    int i, j, k;
    int addr, slot;
    // Iterate around every possible application
    for (i = 0; i < NUM_MAX_APPS; i++){
        // If the application is valid
        if (applications[i].occupied == TRUE){

            // If the nextRun of this application is right now, then release each task!
            if(applications[i].nextRun <= tick && applications[i].nextRun != applications[i].lastStart && API_isApplicationReady(i)){
                printsv("Starting to release the application ", i);
                // If the system has space to accept every task
                if(applications[i].numTasks <= API_GetSystemTasksSlots()){
                    
                    // Iterates around each task of this application
                    for(j = 0; j < applications[i].numTasks; j++){
                        printsv("getting a slot for task ", j);
                        for(;;){
                            addr = getNextPriorityAddr(start);
                            slot = API_GetTaskSlotFromTile(addr, i, j);
                            if (slot != ERRO)
                                break;
                        }
                        printsv("selected slot at ", addr);
                        applications[i].tasks[j].status = TASK_ALLOCATING;
                        applications[i].tasks[j].addr = addr;
                        applications[i].tasks[j].slot = slot;
                        API_RepositoryAllocation(i, j, addr);
                        debug_task_alloc(tick, i, j, addr);
                    }

                    // Resets the lastStart
                    applications[i].lastStart = applications[i].nextRun;
                }
            }
        }
    }
    return;
}

unsigned int API_isApplicationReady(unsigned int app){
    unsigned int i;
    for(i = 0; i < applications[app].numTasks; i++){
        if(applications[app].tasks[i].status != TASK_TO_ALLOCATE){
            return FALSE;
        }
    }
    return TRUE;
}

unsigned int API_CheckTaskToAllocate(unsigned int tick){
    int i;
    // Iterate around every possible application
    for (i = 0; i < NUM_MAX_APPS; i++){
        // If the application is valid
        if (applications[i].occupied == TRUE){
            // If the nextRun of this application is right now, then release each task!
            if(applications[i].nextRun <= tick && applications[i].nextRun != applications[i].lastStart && API_isApplicationReady(i)){
                return TRUE;
            }
        }
    }
    return FALSE;
}

// Defines the hole system as the cluster
void API_Clusterless(unsigned int app){
    applications[app].cluster_addr = makeAddress(0,0);
    applications[app].cluster_size = DIM_X;
    return;
}

// Defines a cluster with the  as objetive
void API_FindBestCluster( unsigned int app){
    unsigned int cluster_size, base_addr, i, j;
    int smallScore = 0x7FFFFFFF;
    int score = 0;
    int occupation;
    int smallOccupation = 0x7FFFFFFF;

    unsigned int sel_cluster_size = 0;
    unsigned int sel_cluster_base_addr = 0;

    cluster_size = API_minClusterSize(applications[app].numTasks*2);
    printsvsv("requesting size: ", applications[app].numTasks*2, "calculated: ", cluster_size);
    do{
        for(i = 0; i <= (DIM_X-cluster_size); i++){
            for(j = 0; j <= (DIM_Y-cluster_size); j++){
                base_addr = makeAddress(i, j);
                score = API_CheckCluster(base_addr, cluster_size, applications[app].numTasks*2);
                printsvsv("baseAddr: ", base_addr, "clusterScore: ", score);
                if(score != 0) {
                    occupation = API_GetClusterOccupation(base_addr, cluster_size);
                    printsv("clusterOcc: ", occupation);
                    if( occupation < smallOccupation || (occupation == smallOccupation && 0 == (random()%2)) ){
                        if ( score < smallScore || (score == smallScore && 0 == (random()%2)) ){ 
                            //prints("Selected!\n");
                            smallScore = score;
                            sel_cluster_size = cluster_size;
                            sel_cluster_base_addr = base_addr;
                            smallOccupation = occupation;
                        }
                    }
                }
            }
        }
        cluster_size++;
        if(cluster_size >= DIM_X || cluster_size >= DIM_Y){
            sel_cluster_size = DIM_X;
            sel_cluster_base_addr = 0x0000;
            break;
        }
    }while(sel_cluster_size == 0);
    
    for(i = getXpos(sel_cluster_base_addr); i < (getXpos(sel_cluster_base_addr)+sel_cluster_size); i++){
        for(j = getYpos(sel_cluster_base_addr); j < (getYpos(sel_cluster_base_addr)+sel_cluster_size); j++){
            Tiles[i][j].clusterCount++;
        }
    }

    applications[app].cluster_addr = sel_cluster_base_addr;
    applications[app].cluster_size = sel_cluster_size;

    return;
}

unsigned int API_GetClusterOccupation(unsigned int base_addr, unsigned int cluster_size){
    unsigned int i, j, occupation;
    occupation = 0;
    for(i = getXpos(base_addr); i < (getXpos(base_addr)+cluster_size); i++){
        for(j = getYpos(base_addr); j < (getYpos(base_addr)+cluster_size); j++){
            //printsvsv("occ - addr: ", makeAddress(i, j), "occupation: ", Tiles[i][j].clusterCount);
            occupation = occupation + Tiles[i][j].clusterCount;
        }
    }
    return occupation;
}


unsigned int API_CheckCluster(unsigned int base_addr, unsigned int cluster_size, unsigned int size){
    unsigned int x;
    unsigned int y;
    unsigned int accumulated = 0;
    unsigned int FIT = 0;

    for(x = getXpos(base_addr); x < (getXpos(base_addr)+cluster_size); x++){
        for(y = getYpos(base_addr); y < (getYpos(base_addr)+cluster_size); y++){
            accumulated = accumulated + Tiles[x][y].taskSlots;
#if CLUSTER_FORMATION == 1 // TEMP
            FIT = FIT + Tiles[x][y].temperature;
#elif CLUSTER_FORMATION == 2 // OCCU
            FIT = accumulated;
#elif CLUSTER_FORMATION == 3 // FIT 
            FIT = FIT + Tiles[x][y].fit;
#endif
        }
    }
    if(accumulated >= size) return FIT;
    else return 0;
}

unsigned int API_minClusterSize(unsigned int size){
    //printf(("sqrt input should be non-negative", n > 0));

    // Xₙ₊₁
    unsigned int x = size;

    // cₙ
    unsigned int c = 0;

    // dₙ which starts at the highest power of four <= n
    unsigned int d = 1 << 30; // The second-to-top bit is set.
                         // Same as ((unsigned) INT32_MAX + 1) / 2.
    while (d > size)
        d >>= 2;

    // for dₙ … d₀
    while (d != 0) {
        if (x >= c + d) {      // if Xₘ₊₁ ≥ Yₘ then aₘ = 2ᵐ
            x -= c + d;        // Xₘ = Xₘ₊₁ - Yₘ
            c = (c >> 1) + d;  // cₘ₋₁ = cₘ/2 + dₘ (aₘ is 2ᵐ)
        }
        else {
            c >>= 1;           // cₘ₋₁ = cₘ/2      (aₘ is 0)
        }
        d >>= 2;               // dₘ₋₁ = dₘ/4
    }
    if((c*c)%size != 0) c++;
    if (c<=3){ // defines the minimum cluster size as 4
        c = 4;
    }
    return c;                  // c₋₁
}

/*void API_PrintPolicyTable(){
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
}*/

void API_PrintScoreTable(float scoreTable[N_TASKTYPE][N_STATES]){
    int i, j;
    vTaskEnterCritical();
    prints("scoreTable = { ");
    for(i = 0; i < N_TASKTYPE; i++){
        prints(" {");
        for(j = 0; j < N_STATES; j++){
            //printi((scoreTable[i][j]));
            printi((int)(scoreTable[i][j]*1000));
            if(j != N_STATES-1){
                prints(", ");
            }
        }
        prints(" }");
        if (i != N_TASKTYPE-1){
            prints(",\n");
        }
    }
    prints(" };\n");
    vTaskExitCritical();
    return;
}


void API_DealocateTask(unsigned int task_id, unsigned int app_id){
    unsigned int i, j, tick;
    volatile int flag;
    int time_execution;
    unsigned int tasktype, takedAct, next_maxid;
    //////////////////
    if(applications[app_id].tasks[task_id].status == TASK_MIGRATION_REQUEST){
        Migration_Status = FALSE;
        API_ClearTaskSlotFromTile(applications[app_id].newAddr, app_id, task_id);
    }

    applications[app_id].tasks[task_id].status = TASK_FINISHED;

    debug_task_dealloc(xTaskGetTickCount(), app_id, task_id, applications[app_id].tasks[task_id].addr);
    
    // ////////////////////////////////////
    // verify if every task has finished
    flag = 1;
    for (i = 0; i < applications[app_id].numTasks; i++){
        printsvsv("checking ", i, "task is: ", applications[app_id].tasks[i].status);
        printsvsv("from app: ", app_id, "running at addr: ", applications[app_id].tasks[i].addr);
        if(applications[app_id].tasks[i].status != TASK_FINISHED && applications[app_id].tasks[i].status != TASK_TO_ALLOCATE){
            flag = 0;
            break;
        }
    }
    // in positive case
    if(flag){

        for(i = getXpos(applications[app_id].cluster_addr); i < (getXpos(applications[app_id].cluster_addr)+applications[app_id].cluster_size); i++){
            for(j = getYpos(applications[app_id].cluster_addr); j < (getYpos(applications[app_id].cluster_addr)+applications[app_id].cluster_size); j++){
                Tiles[i][j].clusterCount = Tiles[i][j].clusterCount - 1;
            }
        }

        // register that the application has executed another time
        tick = xTaskGetTickCount();
        applications[app_id].executed++;
        time_execution = (tick - applications[app_id].lastStart);
        printsvsv("Application ", app_id, "was executed in ", time_execution);
        if(time_execution - applications[app_id].deadline < 0){
            printsv("The application has VIOLATED the deadline: ", applications[app_id].deadline);
        }else {
            printsv("The application was executed within the deadline: ", applications[app_id].deadline);
        }
        applications[app_id].lastFinish = tick;
        // if the application must run another time
        if(applications[app_id].appExec > applications[app_id].executed){
            for (i = 0; i < applications[app_id].numTasks; i++){
                applications[app_id].tasks[i].status = TASK_TO_ALLOCATE;
            }
            printsv("\t\tThis application still need to run: ", (applications[app_id].appExec - applications[app_id].executed));
            applications[app_id].nextRun = applications[app_id].arrived + (applications[app_id].executed * applications[app_id].appPeriod); //tick + applications[app_id].appPeriod;
        } else { // if the application has finished its runs
            prints("\t\tThis application is DONE!\n");
            applications[app_id].occupied = FALSE;

            // To finish the simulation!!
            for(i = 0; i < NUM_MAX_APPS; i++){
                if (applications[i].occupied == TRUE){
                    printsv("--------> not yet! Application still occupied: ", i);
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
unsigned int getNextPriorityAddr(unsigned int start){
    int i;
    unsigned int addr = makeAddress(0,0);
    
    if(start != -1){
        priorityPointer = start;
    }
    
    for(;;){
        //printsv("prioritypointer: ", priorityPointer);

        // Checks if it's a valid address
        if (priorityMatrix[priorityPointer] != makeAddress(0,0)){
            if (Tiles[getXpos(priorityMatrix[priorityPointer])][getYpos(priorityMatrix[priorityPointer])].taskSlots > 0){
                addr = priorityMatrix[priorityPointer];
            }
        }
        // Increments the priorityPointer
        priorityPointer++;
        if (priorityPointer == DIM_X*DIM_Y)
            priorityPointer = 0;
        
        //printsv("prioritypointer: ", priorityPointer);

        // Checks if it's a valid address
        if (priorityMatrix[priorityPointer] != makeAddress(0,0)){
            if (Tiles[getXpos(priorityMatrix[priorityPointer])][getYpos(priorityMatrix[priorityPointer])].taskSlots > 0){
                addr = priorityMatrix[priorityPointer];
            }
        }

        // If we found a new valid address, return
        if( addr != makeAddress(0,0)){
            //printsv("trying addr: ", addr);
            break;
        }
    }
    return addr;
    //return 0x101;
}

// Gets a free slot from one given tile
unsigned int API_GetTaskSlotFromTile(unsigned int addr, unsigned int app, unsigned int task){
    if(Tiles[getXpos(addr)][getYpos(addr)].taskSlots > 0 && addr != MASTER_ADDR){
        Tiles[getXpos(addr)][getYpos(addr)].taskSlots = Tiles[getXpos(addr)][getYpos(addr)].taskSlots - 1;
        printsvsv("addr: ", addr, "slots: ", Tiles[getXpos(addr)][getYpos(addr)].taskSlots);
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
    printsvsv("Cleaning TILE: ", addr, " slots: ", Tiles[getXpos(addr)][getYpos(addr)].taskSlots);
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
    ServicePipe[mySlot].header.id               = globalID;
    printsv("globalID: ", globalID);
    globalID++;
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
        }
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
    while(ServiceMessage.status == PIPE_OCCUPIED){
        // Runs the NI Handler to send/receive packets, opening space in the PIPE
        prints("Estou preso aqui70...\n");
    }
    printsvsv("Starting Migration Process for app: ", app_id, "Task: ", task_id);
    applications[app_id].tasks[task_id].status  = TASK_MIGRATION_REQUEST;
    applications[app_id].newAddr                = new_addr;
    ServiceMessage.status                       = PIPE_OCCUPIED;
    ServiceMessage.header.header                = applications[app_id].tasks[task_id].addr;
    ServiceMessage.header.payload_size          = PKT_SERVICE_SIZE;
    ServiceMessage.header.service               = TASK_MIGRATION_REQUEST;
    ServiceMessage.header.task_id               = task_id;
    ServiceMessage.header.app_id                = app_id;
    ServiceMessage.header.task_migration_addr   = new_addr;
    API_PushSendQueue(SYS_MESSAGE, 0);
    Migration_Status = TRUE;
    return;
}

void API_Migration_Refused(unsigned int task_id, unsigned int app_id, unsigned int why, unsigned int addr){
    if( why == CAN_NOT_MIGRATE ){ 
        applications[app_id].tasks[task_id].migration = FALSE;
    }
    if ( applications[app_id].tasks[task_id].status == TASK_MIGRATION_REQUEST ){
        //applications[app_id].tasks[task_id].status = TASK_STARTED; // old
        // if(task_id > 0){
        //     applications[app_id].tasks[task_id].status = applications[app_id].tasks[0].status;
        // }
        // else{
        //     applications[app_id].tasks[task_id].status = applications[app_id].tasks[1].status;
        // }
        API_DealocateTask(task_id, app_id);
    }
    if( Migration_Status == TRUE ){
        Migration_Status = FALSE;
        //API_ClearTaskSlotFromTile(addr, app_id, task_id);
    }
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
                } else if( applications[j].tasks[i].status == TASK_SEND_STALL){
                    start = FALSE;
                    API_Send_StallTask( j, i );
                }
                else if( applications[j].tasks[i].status != TASK_ALLOCATED){
                    start = FALSE;
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
    printsv("TTT02: ", xTaskGetTickCount());
    prints("Every task is stalled! Forwarding the migrating task... \n");
    return;
}

void API_Migration_ForwardTask( unsigned int app_id, unsigned int task_id, unsigned int newAddr){
    unsigned int mySlot;
    do{
        mySlot = API_GetServiceSlot();
        if(mySlot == PIPE_FULL){
            // Runs the NI Handler to send/receive packets, opening space in the PIPE
            prints("Estou preso aqui17...\n");
            vTaskExitCritical();
            asm("nop");
            vTaskEnterCritical();
        }
    }while(mySlot == PIPE_FULL);
    printsvsv("Forwarding the original task: ", task_id, "from app: ", app_id);
    applications[app_id].tasks[task_id].status = TASK_MIGRATION_FORWARD;
    ServicePipe[mySlot].header.header            = applications[app_id].tasks[task_id].addr;
    ServicePipe[mySlot].header.payload_size      = PKT_SERVICE_SIZE;
    ServicePipe[mySlot].header.service           = TASK_MIGRATION_FORWARD;
    ServicePipe[mySlot].header.task_id           = task_id;
    ServicePipe[mySlot].header.app_id            = app_id;
    ServicePipe[mySlot].header.task_dest_addr    = applications[app_id].newAddr;
    API_PushSendQueue(SERVICE, mySlot);
}

void API_ReleaseApplication(unsigned int app_id, unsigned int task_id){
    int i, j;
    Migration_Status = FALSE;
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
                prints("Estou preso aqui121...\n");
                vTaskExitCritical();
                asm("nop");
                vTaskEnterCritical();
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
        //printsv("PI: ", pi);
        // Separately sort elements before 
        // partition and after partition 
        quickSort(arr, arr2, low, (pi - 1)); 
        quickSort(arr, arr2, (pi + 1), high); 
    }
}

void randPositions(int arr[], int arr2[], int low, int high){
    for(int i = low; i < (high-1); i++){
        if(arr[i] == arr[i+1]){
            if( 60 > random()%100 ){
                swap(&arr[i], &arr[i+1]);
                swap(&arr2[i], &arr2[i+1]);
            }
        }
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

unsigned int API_getPEStateX4(unsigned int id, unsigned int excludeAddr){
    unsigned int addr = id2addr(id);
    unsigned int ex, ey;
    unsigned int x = getXpos(addr);
    unsigned int y = getYpos(addr);
    int state_0, state_1, state, a;
    int i0, i1, i2;
    int d0, d1, d2;

    unsigned int immediate[N_TASKTYPE];
    unsigned int diagonal[N_TASKTYPE]; //+

    if(excludeAddr != -1){
        ex = getXpos(excludeAddr);
        ey = getYpos(excludeAddr);
    } else {
        ex = -1;
        ey = -1;
    }

    for(a = 0; a < N_TASKTYPE; a++){
        immediate[a] = 0;
        diagonal[a] = 0; 
    }
    // SOUTH
    if(getSouth(x,y) != -1 && !(x == ex && (y-1) == ey ))
        immediate[getSouth(x,y)]++;
    // NORTH
    if(getNorth(x, y) != -1 && !(x == ex && (y+1) == ey ))
        immediate[getNorth(x,y)]++;
    // WEST
    if(getWest(x, y) != -1 && !((x-1) == ex && y == ey ))
        immediate[getWest(x,y)]++;
    // EAST
    if(getEast(x, y) != -1 && !((x+1) == ex && y == ey ))
        immediate[getEast(x,y)]++;
    
    // SouthWest
    if(getSouthWest(x,y) != -1 && !((x-1) == ex && (y-1) == ey ))
        diagonal[getSouthWest(x,y)]++;
    // SouthEast
    if(getSouthEast(x,y) != -1 && !((x+1) == ex && (y-1) == ey ))
        diagonal[getSouthEast(x,y)]++;
    // NorthWest
    if(getNorthWest(x, y) != -1 && !((x-1) == ex && (y+1) == ey ))
        diagonal[getNorthWest(x,y)]++;
    // NorthEast
    if(getNorthEast(x, y) != -1 && !((x+1) == ex && (y+1) == ey ))
        diagonal[getNorthEast(x,y)]++;
     
    i0 = immediate[0];
    i1 = immediate[1];
    i2 = immediate[2];
    // 4*35
    d0 = diagonal[0];
    d1 = diagonal[1];
    d2 = diagonal[2];

    state_0 = (int)(i0 ? ((i0*i0*i0 - 18*i0*i0 + 107*i0) / 6) : 0);
    state_1 = (int)(i1 ? ((11*i1 - i1*i1 - 2*i0*i1) / 2) : 0);
    state = state_0 + state_1 + i2;

    if(d2 >= 2) // diagonal com vizinhos tipo 2
        state = state+35*1;
    else if(d1 >= 2) // diagonal com vizinhos tipo 1
        state = state+35*2;
    else if(x == 0 || x == DIM_X-1 || y == 0 || y == DIM_Y-1) // bordas
        state = state+35*3;
        
    if(state >= N_STATES*4) printsv("ERRO CALCULANDO ESTADO: ", state);
    return(state);
}

unsigned int API_getPEState(unsigned int id, unsigned int excludeAddr){
    unsigned int addr = id2addr(id);
    unsigned int ex, ey;
    unsigned int x = getXpos(addr);
    unsigned int y = getYpos(addr);
    int state_x, state_y, z, state, a;
    unsigned int xd, yd, zd, state_xd, state_yd, stated;

    unsigned int immediate[N_TASKTYPE];

    if(excludeAddr != -1){
        ex = getXpos(excludeAddr);
        ey = getYpos(excludeAddr);
    } else {
        ex = -1;
        ey = -1;
    }

    for(a = 0; a < N_TASKTYPE; a++){
        immediate[a] = 0;
    }
    // SOUTH
    if(getSouth(x,y) != -1 && !(x == ex && (y-1) == ey ))
        immediate[getSouth(x,y)]++;
    // NORTH
    if(getNorth(x, y) != -1 && !(x == ex && (y+1) == ey ))
        immediate[getNorth(x,y)]++;
    // WEST
    if(getWest(x, y) != -1 && !((x-1) == ex && y == ey ))
        immediate[getWest(x,y)]++;
    // EAST
    if(getEast(x, y) != -1 && !((x+1) == ex && y == ey ))
        immediate[getEast(x,y)]++;
     
    x = immediate[0];
    y = immediate[1];
    z = immediate[2];

    state_x = (int)(x ? ((x*x*x - 18*x*x + 107*x) / 6) : 0);
    state_y = (int)(y ? ((11*y - y*y - 2*x*y) / 2) : 0);
    state = state_x + state_y + z;
    
    if(state >= N_STATES) printsv("ERRO CALCULANDO ESTADO: ", state);
    return(state);
}

int API_calculateState(int l0, int l1, int l2, int d0, int d1, int d2){
    int state_x, state_y, state, state_xd, state_yd, stated;
    state_x = (int)(l0 ? ((l0*l0*l0 - 18*l0*l0 + 107*l0) / 6) : 0);
    state_y = (int)(l1 ? ((11*l1 - l1*l1 - 2*l0*l1) / 2) : 0);
    state = state_x + state_y + l2;

    state_xd = (int)(d0 ? ((d0*d0*d0 - 18*d0*d0 + 107*d0) / 6) : 0);
    state_yd = (int)(d1 ? ((11*d1 - d1*d1 - 2*d0*d1) / 2) : 0);
    stated = state_xd + state_yd + d2;

    return (stated*35 + state);
}

int getSouth(int x, int y){
    if(y > 0){
        /*if(makeAddress(x,y-1) == GLOBAL_MASTER_ADDR) return 2;
        else*/ return(Tiles[x][y-1].taskType);
    } else {
        return(-1);
    }
}

int getNorth(int x, int y){
    if(y < DIM_Y-1){
        /*if(makeAddress(x,y+1) == GLOBAL_MASTER_ADDR) return 2;
        else*/ return(Tiles[x][y+1].taskType);
    } else {
        return(-1);
    }
}

int getEast(int x, int y){
    if(x < DIM_X-1){
        /*if(makeAddress(x+1,y) == GLOBAL_MASTER_ADDR) return 2;
        else*/ return(Tiles[x+1][y].taskType);
    } else {
        return(-1);
    }
}

int getWest(int x, int y){
    if(x > 0){
        /*if(makeAddress(x-1,y) == GLOBAL_MASTER_ADDR) return 2;
        else*/ return(Tiles[x-1][y].taskType);
    } else {
        return(-1);
    }
}

int getSouthWest(int x, int y){
    if(x > 0 && y > 0){
        /*if(makeAddress(x-1,y-1) == GLOBAL_MASTER_ADDR) return 2;
        else*/ return(Tiles[x-1][y-1].taskType);
    } else {
        return(-1);
    }
}

int getSouthEast(int x, int y){
    if(x > 0 && y > 0){
        /*if(makeAddress(x+1,y-1) == GLOBAL_MASTER_ADDR) return 2;
        else*/ return(Tiles[x+1][y-1].taskType);
    } else {
        return(-1);
    }
}

int getNorthWest(int x, int y){
    if(y < DIM_Y-1 && x > 0){
        /*if(makeAddress(x-1,y+1) == GLOBAL_MASTER_ADDR) return 2;
        else*/ return(Tiles[x-1][y+1].taskType);
    } else {
        return(-1);
    }
}

int getNorthEast(int x, int y){
    if(x < DIM_X-1 && y < DIM_Y-1){
        /*if(makeAddress(x+1,y+1) == GLOBAL_MASTER_ADDR) return 2;
        else*/ return(Tiles[x+1][y+1].taskType);
    } else {
        return(-1);
    }
}
 