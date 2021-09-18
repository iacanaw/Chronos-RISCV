#include "globalMaster.h"

#include "chronos.h"
#include "message.h"
#include "packet.h"
#include "services.h"

// Informs the Repository that the GLOBALMASTER is ready to receive the application info
void API_RepositoryWakeUp(){
    unsigned int mySlot;
    do{
        mySlot = API_GetServiceSlot();
        vTaskDelay(1);
    }while(mySlot == PIPE_FULL);
    //printsv("I got a free service slot!! -> ", mySlot);

    ServicePipe[mySlot].holder = PIPE_SYS_HOLDER;

    ServicePipe[mySlot].header.header       = makeAddress(0, 0) | PERIPH_SOUTH;
    ServicePipe[mySlot].header.payload_size = PKT_SERVICE_SIZE;
    ServicePipe[mySlot].header.service      = REPOSITORY_WAKEUP;

    API_PushSendQueue(SERVICE, mySlot);
    return;    
}
