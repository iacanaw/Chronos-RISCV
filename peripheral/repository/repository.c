
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
                        outPacket[9] = -1;
                        outPacket[10] = -1;
                        outPacket[11] = -1;
                        outPacket[12] = -1;
                        for(i=13; i<((tasksInfo[inPacket[7]][inPacket[3]][1])+13); i++){
                            // indecifravel, eu sei ._.
                            outPacket[i] = tasksCode[tasksInfo[inPacket[7]][inPacket[3]][4]][tasksInfo[inPacket[7]][inPacket[3]][0]][i-13]; 
                            //bhmMessage("I", "REPO", "flit %d = %x!", i, outPacket[i]);
                        }
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
    // YOUR CODE HERE (pre constructor)
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
