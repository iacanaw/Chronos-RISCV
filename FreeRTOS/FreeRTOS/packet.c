#include "packet.h"

#include "chronos.h"
#include "applications.h"

//extern volatile MessagePacket MessagePipe[PIPE_SIZE];
extern volatile ServicePacket ServicePipe[PIPE_SIZE];
extern unsigned int messageID;

extern unsigned int thermalPacket_pending; // from thermal.h

////////////////////////////////////////////////////////////
// Initialize the PIPE, setting the status of each slot to FREE
void API_PipeInitialization(){
    int i, j;
    messageID = 1;
    ServiceMessage.status = PIPE_FREE;
    for( i = 0; i < PIPE_SIZE; i++ ){
        for(j = 0; j < NUM_MAX_TASKS; j++){
            TaskList[j].MessagePipe[i].status = PIPE_FREE;
            TaskList[j].MessagePipe[i].msgID  = 0;
        }        
        ServicePipe[i].status = PIPE_FREE;
    }
    return;
}

////////////////////////////////////////////////////////////
// Returns a free Message slot 
unsigned int API_GetMessageSlot(){
    int i;
    unsigned int sel = PIPE_FULL;
    vTaskEnterCritical();
    unsigned int currTask = API_GetCurrentTaskSlot();
    for( i = 0; i < PIPE_SIZE; i++ ){
        if (TaskList[currTask].MessagePipe[i].status == PIPE_FREE){
            TaskList[currTask].MessagePipe[i].status = PIPE_OCCUPIED;
            TaskList[currTask].MessagePipe[i].msgID = messageID;
            messageID++;
            sel = (currTask << 8) | i;
            break;
        }
    }
    if(messageID > 0X0FFFFFF0){
        messageID = 256;
        for( i = 0; i < PIPE_SIZE; i++ ){
            TaskList[currTask].MessagePipe[i].msgID = (TaskList[currTask].MessagePipe[i].msgID & 0x000000FF);
        }
    }
    vTaskExitCritical();
    return sel;
}

////////////////////////////////////////////////////////////
// Returns a free Service slot
unsigned int API_GetServiceSlot(){
    int i;
    vTaskEnterCritical();
    for( i = 0; i < PIPE_SIZE; i++ ){
        if (ServicePipe[i].status == PIPE_FREE){
            ServicePipe[i].status = PIPE_OCCUPIED;
            vTaskExitCritical();
            return i;
        }
    }
    vTaskExitCritical();
    return PIPE_FULL;
}

////////////////////////////////////////////////////////////
// Clear one PipeSlot after send it
void API_ClearPipeSlot(unsigned int typeSlot){
    unsigned int type =   typeSlot & 0xFFFF0000;
    unsigned int taskID = (typeSlot & 0x0000FF00) >> 8;
    unsigned int slot =   typeSlot & 0x000000FF;
    unsigned int i, j;
    
    if (type == SERVICE){
        ServicePipe[slot].status = PIPE_FREE;
        ServicePipe[slot].holder = PIPE_FREE;
    } else if ( type == THERMAL ){
        thermalPacket_pending = FALSE;
    } else if ( type == SYS_MESSAGE ){
        ServiceMessage.status = PIPE_FREE;
    } else if ( type == MIGRATION ){
        TaskList[slot].status = TASK_MIGRATION_SENT;
    } else { // type == MESSAGE
        //printsv("cleaning message pipe slot: ", slot);
        TaskList[taskID].MessagePipe[slot].status = PIPE_FREE;
        //TaskList[taskID].MessagePipe[slot].holder = PIPE_FREE;
    }

    // checks if some task must be released
    for(i=0; i < NUM_MAX_TASKS; i++){
        if( TaskList[i].status == TASK_SLOT_SUSPENDED ){
            for(j=0; j < PIPE_SIZE; j++){
                if( TaskList[i].MessagePipe[j].status == PIPE_FREE ){         
                    TaskList[i].status = TASK_SLOT_RUNNING;
                    vTaskResume( TaskList[i].TaskHandler );
                    printsv("Resumindo taskSlot ", i);
                    break;
                }    
            }
        }
    }
    return;
}

unsigned int API_checkPipe(unsigned int taskSlot){
    unsigned int i;
    printsv("Checking the PIPE of taskSlot: ", taskSlot);
    for(i = 0; i < PIPE_SIZE; i++){
        printsv("i: ", i);
        printsv("status: ", TaskList[taskSlot].MessagePipe[i].status);
        //printsv("holder: ", TaskList[taskSlot].MessagePipe[i].holder);
        prints("---\n");
        if(TaskList[taskSlot].MessagePipe[i].status == PIPE_OCCUPIED || TaskList[taskSlot].MessagePipe[i].status == PIPE_TRANSMITTING){
            //if(MessagePipe[i].holder == taskSlot){
                return 1;
            //}
        }
    }
    return 0;
}

