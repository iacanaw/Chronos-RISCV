
////////////////////////////////////////////////////////////////////////////////
//
//                W R I T T E N   B Y   I M P E R A S   I G E N
//
//                             Version 20210408.0
//
////////////////////////////////////////////////////////////////////////////////

#include "repository.igen.h"
#include "../../applications/repository.h"
#include "../../FreeRTOS/FreeRTOS/include/services.h"
#include <string.h>

#define REPO_IDLE           0 // Waiting a command from the MASTER 
#define REPO_STARTING       1 // Sending to the MASTER the information about each available application
#define REPO_SENDING_TASK   2 // Responds to the command to send a task to a given PE

extern unsigned int appUtilization[NUM_APPS];
extern unsigned int appInfo[NUM_APPS][INFO_SIZE];
extern unsigned int tasksInfo[NUM_APPS][MAX_TASKS][INFO_SIZE];
extern unsigned int tasksCode[NUM_APPS][MAX_TASKS][BIG_CODE];

// random iteration vector source
unsigned int randomIterations_vector[256] = {1309, 2297, 2167, 3783, 3317, 2307, 2996, 1523, 1166, 2578, 313, 5411, 5211, 4298, 4544, 2026, 4587, 1944, 1177, 5801, 2928, 1863, 4823, 4569, 5697, 3340, 2585, 5571, 3853, 4701, 4066, 564, 3392, 660, 2126, 4359, 3942, 2845, 4272, 508, 5591, 1584, 1635, 3498, 5626, 2447, 1561, 5267, 2000, 2686, 2239, 5735, 471, 866, 2992, 3857, 2575, 3378, 2064, 994, 513, 4196, 2376, 5353, 2412, 1096, 1596, 740, 852, 4249, 4347, 2336, 1903, 1753, 2086, 1336, 4874, 1114, 3352, 310, 2516, 927, 2428, 126, 4181, 4907, 698, 2293, 3034, 217, 2774, 5891, 1696, 5040, 2485, 420, 946, 479, 5131, 4492, 1982, 1725, 1003, 3713, 923, 5303, 4114, 156, 3490, 5956, 793, 3906, 330, 563, 3162, 572, 4976, 3103, 4926, 4201, 1034, 774, 1966, 1618, 950, 414, 4160, 1630, 3615, 5240, 1474, 3785, 2536, 498, 251, 4687, 1172, 5706, 4425, 3111, 4443, 5658, 3066, 2878, 4753, 3469, 1143, 144, 5188, 2169, 3507, 4760, 1821, 2153, 1716, 2324, 3483, 3818, 1525, 4239, 1599, 755, 2408, 5661, 1382, 509, 4270, 4962, 135, 3239, 4825, 3096, 2855, 5145, 4818, 1038, 4943, 5682, 5575, 457, 5779, 3356, 2595, 5983, 5313, 4344, 3566, 5777, 318, 1733, 4448, 3932, 2208, 1881, 800, 103, 3406, 2103, 931, 1729, 527, 3427, 3598, 613, 4067, 2640, 4545, 3576, 4537, 1766, 4938, 3441, 3335, 227, 2881, 2829, 5593, 4361, 2714, 1713, 2146, 3739, 2987, 456, 4841, 3396, 2004, 351, 818, 1904, 3618, 3046, 836, 2946, 3772, 539, 3965, 5634, 1284, 4806, 2062, 2474, 2722, 553, 3422, 256, 4890, 3497, 5428, 247, 2427, 4074, 3181, 2843, 4598, 3916};

// Stores the credit of the local router port
unsigned int routerCredit = 0;

// Used to handle packets
volatile unsigned int inPacket[13+10];
unsigned int outPacket[13+BIG_CODE];
unsigned int sendingPacket = 0;
unsigned int out_packetPointer = 0;
unsigned int out_packetSize = 0;
unsigned int in_packetPointer = 0;
unsigned int in_packetSize = 0;
unsigned int packetReceived = 0;

// Repository control stuff
unsigned int status = REPO_IDLE; 
unsigned int sendApp = 0;
//////////////////////////////// Callback stubs ////////////////////////////////
void iterate(){
    int i;
    switch (status){
        // Waiting for commands from MASTER
        case REPO_IDLE:
            // if some packet was received...
            if (packetReceived == 1){
                in_packetPointer = 0;
                //bhmMessage("I", "REPO", "Terminou de chegar um pacote!");
                /*for (i = 0; i < in_packetSize+2; i++){
                    //bhmMessage("I", "REPO", "Flit %d: %x", i, inPacket[i]);
                }*/
                if (inPacket[2] == REPOSITORY_WAKEUP){
                    status = REPO_STARTING;

                    // Enables a new packet to get inside the Repository
                    packetReceived = 0;
                    ppmPacketnetWrite(handles.portControl, &packetReceived, sizeof(packetReceived));
                }
                else if (inPacket[2] == TASK_ALLOCATION_SEND){
                    if(sendingPacket == 0){
                        
                        outPacket[0] = inPacket[12];
                        outPacket[1] = tasksInfo[inPacket[7]][inPacket[3]][2] + tasksInfo[inPacket[7]][inPacket[3]][1] + 11; 
                        outPacket[2] = TASK_ALLOCATION_SEND;
                        outPacket[3] = tasksInfo[inPacket[7]][inPacket[3]][0]; //task id
                        outPacket[4] = tasksInfo[inPacket[7]][inPacket[3]][1]; //task size
                        outPacket[5] = tasksInfo[inPacket[7]][inPacket[3]][2]; //bss size
                        outPacket[6] = tasksInfo[inPacket[7]][inPacket[3]][3]; //start point
                        outPacket[7] = tasksInfo[inPacket[7]][inPacket[3]][4]; //taskappid
                        outPacket[8] = tasksInfo[inPacket[7]][inPacket[3]][5]; //migration variable
                        outPacket[9] = inPacket[9]; //global id
                        outPacket[10] = -1;
                        outPacket[11] = -1;
                        outPacket[12] = -1;
                        //bhmMessage("I", "REPO", "ENVIANDO UM PACOTE DE TASK");
                        for(i=13; i<((tasksInfo[inPacket[7]][inPacket[3]][1])+13); i++){
                            // indecifravel, eu sei ._.
                            outPacket[i] = tasksCode[tasksInfo[inPacket[7]][inPacket[3]][4]][tasksInfo[inPacket[7]][inPacket[3]][0]][i-13]; 
                            if(outPacket[i] == 0xFFFFFFFD){
                                //bhmMessage("I", "REPO", "app utiliation: %d -- info: %d ", i, tasksInfo[inPacket[7]][inPacket[3]][6]);
                                if((i-13) == tasksInfo[inPacket[7]][inPacket[3]][6]){ // verify if it matches
                                    outPacket[i] = randomIterations_vector[inPacket[7]+((appUtilization[inPacket[7]]/(appInfo[inPacket[7]][3]))%256)];
                                    bhmMessage("I", "REPO", "Application %d with VAR_ITERATIONS = %d!", inPacket[7], outPacket[i] );
                                }
                            }
                            
                        }
                        // increse the application usage counter
                        appUtilization[inPacket[7]]++;

                        // Set the flag to send the packet
                        sendingPacket = 1;

                        // Enables a new packet to get inside the Repository
                        packetReceived = 0;
                        ppmPacketnetWrite(handles.portControl, &packetReceived, sizeof(packetReceived));      
                    }
                }
            }
            break;

        // Sending to the MASTER each Application info
        case REPO_STARTING:
            if (sendingPacket == 0){
                if (sendApp < NUM_APPS){
                    outPacket[0] = MASTER_ADDR;
                    outPacket[1] = 11+appInfo[sendApp][3];
                    outPacket[2] = REPOSITORY_APP_INFO;
                    outPacket[3] = appInfo[sendApp][0];     // app id
                    outPacket[4] = appInfo[sendApp][1];     // app period
                    outPacket[5] = appInfo[sendApp][2];     // app executions
                    outPacket[6] = appInfo[sendApp][3];     // number of tasks
                    outPacket[7] = appInfo[sendApp][4];     // deadline
                    outPacket[8] = -1;
                    outPacket[9] = -1;
                    outPacket[10] = -1;
                    outPacket[11] = -1;
                    outPacket[12] = -1;
                    for(i = 13; i < 13+appInfo[sendApp][3]; i++){
                        outPacket[i] = tasksInfo[sendApp][i-13][7];
                    }
                    // Increment to send next app
                    sendApp++;
                    
                    // Set the flag to send the packet
                    sendingPacket = 1;
                } else {
                    // after send every app info to the master return to idle
                    status = REPO_IDLE;
                }
            }
            break;

        case REPO_SENDING_TASK:
            break;

        default:
            status = REPO_IDLE;
    }
    return;
}

PPM_PACKETNET_CB(controlUpdate) {
    // Input information with 4 bytes are about flow control
    if(bytes == 4){
        unsigned int ctrl = *(unsigned int *)data;
        routerCredit = ctrl;
        //bhmMessage("I", "REPO-CTRLUPDT", "Mudou o controle: %d", routerCredit);
    }
    // Information with 8 bytes are about iterations
    else if(bytes == 8) {
        //bhmMessage("I", "REPO", "ITERATION!!!");
        iterate();
        // When an iteration arrives
        if(sendingPacket){
            if(routerCredit == 0){
                // send one flit to the router
                //bhmMessage("I", "REPO", "Flit %d sent: %x", out_packetPointer, outPacket[out_packetPointer]);
                ppmPacketnetWrite(handles.portData, &outPacket[out_packetPointer], sizeof(outPacket[out_packetPointer]));
                
                // Stores the size of the packet
                if(out_packetPointer == 1){
                    out_packetSize = outPacket[out_packetPointer];
                }

                // Updates packet pointer
                if(out_packetPointer < out_packetSize+1){
                    out_packetPointer++;
                }
                else{
                    out_packetPointer = 0;
                    sendingPacket = 0;
                    //bhmMessage("I", "REPO", "===============================================================");
                }
            }
        }
    }
}

PPM_PACKETNET_CB(dataUpdate) {
    unsigned int newFlit = *(unsigned int *)data;
    unsigned int myStatus = 0;
    //bhmMessage("I", "REPO", "Chegou um flit: %x", newFlit);
    inPacket[in_packetPointer] = newFlit;           // stores the incoming flit
    in_packetPointer++;                             // increases the packet pointer
    
    if(in_packetPointer == 2){                      // if its the seccond flit, than stores the packet size
        in_packetSize = newFlit;
    }
    else if (in_packetPointer >= in_packetSize+2){  // register that the packet was received with success
        packetReceived = 1;
        myStatus = 1;
        ppmPacketnetWrite(handles.portControl, &myStatus, sizeof(myStatus));
    }

    // REMOVER- SÒ PRA N DAR ERRO NA COMPILAÇÂO!
    if (status == 10){
        bhmMessage("I", "REPO", "%d, %d, %d", appInfo[0][0], tasksInfo[0][0][0], tasksCode[0][0][0]);
    }
}

PPM_CONSTRUCTOR_CB(constructor) {
    int i = 0;
    for(i = 0; i < NUM_APPS; i++){
        appUtilization[i] = 0;
    }
    periphConstructor();
    // YOUR CODE HERE (post constructor)
}

PPM_DESTRUCTOR_CB(destructor) {
    // YOUR CODE HERE (destructor)
}


PPM_SAVE_STATE_FN(peripheralSaveState) {
    bhmMessage("E", "PPM_RSNI", "Model does not implement save/restore");
}

PPM_RESTORE_STATE_FN(peripheralRestoreState) {
    bhmMessage("E", "PPM_RSNI", "Model does not implement save/restore");
}
