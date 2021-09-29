#include "packet.h"

#include "chronos.h"

extern volatile MessagePacket MessagePipe[PIPE_SIZE];
extern volatile ServicePacket ServicePipe[PIPE_SIZE];
extern unsigned int messageID;

////////////////////////////////////////////////////////////
// Initialize the PIPE, setting the status of each slot to FREE
void API_PipeInitialization(){
    int i;
    messageID = 0;
    for( i = 0; i < PIPE_SIZE; i++ ){
        MessagePipe[i].status = PIPE_FREE;
        MessagePipe[i].msgID  = 0;
        ServicePipe[i].status = PIPE_FREE;
    }
    return;
}

////////////////////////////////////////////////////////////
// Returns a free Message slot 
unsigned int API_GetMessageSlot(){
    int i;
    unsigned int sel = PIPE_FULL;
    for( i = 0; i < PIPE_SIZE; i++ ){
        if (MessagePipe[i].status == PIPE_FREE){
            MessagePipe[i].status = PIPE_OCCUPIED;
            MessagePipe[i].msgID = messageID;
            messageID++;
            sel = i;
            break;
        }
    }
    if(messageID > 16777215){
        messageID = 256;
        for( i = 0; i < PIPE_SIZE; i++ ){
            MessagePipe[i].msgID = (MessagePipe[i].msgID & 0x000000FF);
        }
    }
    return sel;
}

////////////////////////////////////////////////////////////
// Returns a free Service slot
unsigned int API_GetServiceSlot(){
    int i;
    for( i = 0; i < PIPE_SIZE; i++ ){
        if (ServicePipe[i].status == PIPE_FREE){
            ServicePipe[i].status = PIPE_OCCUPIED;
            return i;
        }
    }
    return PIPE_FULL;
}

////////////////////////////////////////////////////////////
// Clear one PipeSlot after send it
void API_ClearPipeSlot(unsigned int typeSlot){
    unsigned type = typeSlot & 0xFFFF0000;
    unsigned slot = typeSlot & 0x0000FFFF;
    if (type == SERVICE){
        ServicePipe[slot].status = PIPE_FREE;
    } else { // type == MESSAGE
        MessagePipe[slot].status = PIPE_FREE;
    }
    return;
}

unsigned int API_checkPipe(unsigned int taskSlot){
    unsigned int i;
    for(i = 0; i < PIPE_SIZE; i++){
        if(MessagePipe[i].status == PIPE_OCCUPIED && MessagePipe[i].holder == taskSlot){
            return 1;
        }
    }
    return 0;
}
