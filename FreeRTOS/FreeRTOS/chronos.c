#include "chronos.h"

#include "riscv_hal/riscv_plic.h"
#include "hw_reg_access.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "hwaddr.h"
#include "message.h"
#include "packet.h"
#include "services.h"
#include "applications.h"
#include "globalMaster.h"

// Stores the processorAddress
extern unsigned int ProcessorAddr;

// Stores information about each running task
extern Task TaskList[ NUM_MAX_TASKS ];

extern volatile unsigned int SendingQueue[PIPE_SIZE*2];
extern volatile unsigned int SendingQueue_front;
extern volatile unsigned int SendingQueue_tail;

////////////////////////////////////////////////////////////
// Initialize Chronos stuff
void Chronos_init(){
    //Informs the Router this tile ID, that is provided by Harness
    HW_set_32bit_reg(ROUTER_BASE, HW_get_32bit_reg(MY_ID)); 
    
    // Reads this processor address, calculated by the router
    ProcessorAddr = HW_get_32bit_reg(ROUTER_BASE);

    // Informs the PRINTER this tile address
    HW_set_32bit_reg(PRINTER_CHAR, getXpos(ProcessorAddr));
    HW_set_32bit_reg(PRINTER_CHAR, getYpos(ProcessorAddr));

    // Enables interruption from NI
    NI_enable_irq(TX_RX);

    // Informs the NI the address to store incoming packets
    HW_set_32bit_reg(NI_ADDR, (unsigned int)&incommingPacket.header);
    
    // Initialize the TaskList
    API_TaskListInit();

    // Initialize the Message & Service PIPE
    API_PipeInitialization();
    SendingQueue_front = 0;
    SendingQueue_tail = 0;

    return;
}

////////////////////////////////////////////////////////////
// Prints a string
void prints(char *text) {
    int i = 0;
    do {
        HW_set_32bit_reg(PRINTER_CHAR, text[i]);
        i++;
    } while (text[i - 1] != '\0');
    return;
}

////////////////////////////////////////////////////////////
// Prints an integer
void printi(int value) {
    HW_set_32bit_reg(PRINTER_INT, value);
    return;
}

////////////////////////////////////////////////////////////
// Prints a string followed by a integer
void printsv(char *text1, int value1) {
    //vPortEnterCritical();
        prints(text1);
        printi(value1);
        prints("\n");
    //vPortExitCritical();
    return;
}

////////////////////////////////////////////////////////////
// Prints two strings and two integers interspersed
void printsvsv(char *text1, int value1, char *text2, int value2) {
    //vPortEnterCritical();
        prints(text1);
        printi(value1);
        prints(text2);
        printi(value2);
        prints("\n");
    //vPortExitCritical();
    return;
}

////////////////////////////////////////////////////////////
// Gets the X coordinate from the address
unsigned int getXpos(unsigned int addr) { return ((addr & 0x0000FF00) >> 8); }

////////////////////////////////////////////////////////////
// Gets the Y coordinate from the address
unsigned int getYpos(unsigned int addr) { return (addr & 0x000000FF); }

///////////////////////////////////////////////////////////////////
// Configure the NI to transmitt a given packet
void SendRaw(unsigned int addr) {
    HW_set_32bit_reg(NI_ADDR, addr);
    HW_set_32bit_reg(NI_TX, TX);
    return;
}

////////////////////////////////////////////////////////////
// Enables interruptions incomming from NI
void NI_enable_irq(int which){
	if (which == TX || which == TX_RX){
        PLIC_EnableIRQ(NI_TX_IRQn);
        PLIC_SetPriority(NI_TX_IRQn, 1);
    }
    if (which == RX || which == TX_RX){
        PLIC_EnableIRQ(NI_RX_IRQn);
        PLIC_SetPriority(NI_RX_IRQn, 1);
    }
    return;
}

////////////////////////////////////////////////////////////
// Disables interruptions incomming from NI
void NI_disable_irq(int which){
	if (which == TX || which == TX_RX) PLIC_DisableIRQ(NI_TX_IRQn);
    if (which == RX || which == TX_RX) PLIC_DisableIRQ(NI_RX_IRQn);
    return;
}

////////////////////////////////////////////////////////////
// Interruptions handler for TX
uint8_t External_1_IRQHandler(void){ 
    prints("INTERRUPTION TX\n");
    API_ClearPipeSlot(SendingSlot);
    API_Try2Send();

    HW_set_32bit_reg(NI_TX, DONE);
    return 0;
}

////////////////////////////////////////////////////////////
// Interruptions handler for RX
uint8_t External_2_IRQHandler(void){
    // prints("==========================\n");
    // prints("INTERRUPTION RX\n");
    // printsv("Flit 0 : ", incommingPacket.header);
    // printsv("Flit 1 : ", incommingPacket.payload_size);
    // printsv("Flit 2 : ", incommingPacket.service);
    // printsv("Flit 3 : ", incommingPacket.flit3);
    // printsv("Flit 4 : ", incommingPacket.flit4);
    // printsv("Flit 5 : ", incommingPacket.flit5);
    // printsv("Flit 6 : ", incommingPacket.flit6);
    // printsv("Flit 7 : ", incommingPacket.flit7);
    // printsv("Flit 8 : ", incommingPacket.flit8);
    // printsv("Flit 9 : ", incommingPacket.flit9);
    // printsv("Flit 10: ", incommingPacket.flit10);
    // printsv("Flit 11: ", incommingPacket.flit11);
    // printsv("Flit 12: ", incommingPacket.flit12);


    switch (incommingPacket.service){
        unsigned int aux;

        case REPOSITORY_APP_INFO: // When the repository informs the GM that exist a new Application available:
            API_AddApplication(incommingPacket.application_id,
                               incommingPacket.aplication_period, 
                               incommingPacket.application_executions, 
                               incommingPacket.application_n_tasks);
            break;
        
        case TASK_ALLOCATION_SEND: // When the GM asks one Slave to allocate one task
            // aux will receive the taskslot 
            aux = API_TaskAllocation(incommingPacket.task_id,
                                     incommingPacket.task_txt_size,
                                     incommingPacket.task_bss_size,
                                     incommingPacket.task_start_point,
                                     incommingPacket.task_app_id);

            // Informs the NI were to write the application
            HW_set_32bit_reg(NI_ADDR, TaskList[aux].taskAddr);
            incommingPacket.service = TASK_ALLOCATION_FINISHED;
            break;

        case TASK_ALLOCATION_FINISHED:
            API_AckTaskAllocation(incommingPacket.task_id, incommingPacket.task_app_id);
            break;

        case TASK_ALLOCATION_SUCCESS:
            API_TaskAllocated(incommingPacket.task_id, incommingPacket.task_app_id);
            break;

        case TASK_START:
            aux = API_GetTaskSlot(incommingPacket.task_id, incommingPacket.task_app_id);
            // Informs the NI were to write the application
            HW_set_32bit_reg(NI_ADDR, &TaskList[aux].appNumTasks);
            incommingPacket.service = TASK_RUN;
            break;
        
        case TASK_RUN:
            aux = API_GetTaskSlot(incommingPacket.task_id, incommingPacket.task_app_id);
            API_TaskStart(aux);
            printsv("Starting Task: ", incommingPacket.task_id);
            break;

        case MESSAGE_REQUEST:
            // check the pipe
            prints("Chegou um message request! \n");
            aux = API_CheckMessagePipe(incommingPacket.application_id, incommingPacket.task_app_id);
            
            if (aux == ERRO){
                // register an messagerequest
                API_AddPendingReq(incommingPacket.task_id, incommingPacket.task_app_id, incommingPacket.producer_task_id);
            } else {
                API_PushSendQueue(MESSAGE, aux);
            }
            break;
        
        case MESSAGE_DELIVERY:
            prints("Tem uma mensagem chegando...\n");
            aux = API_GetTaskSlot(incommingPacket.application_id, incommingPacket.destination_task);
            incommingPacket.service = MESSAGE_DELIVERY_FINISH;
            HW_set_32bit_reg(NI_ADDR, &TaskList[aux].MsgToReceive);
            break;
        
        case MESSAGE_DELIVERY_FINISH:
            prints("Terminou de entregar a mensagem!!\n");
            aux = API_GetTaskSlot(incommingPacket.destination_task, incommingPacket.application_id);
            printsv("> taskslot: ", aux);
            TaskList[aux].waitingMsg = FALSE;
            break;
            

        default:
            printsv("ERROR External_2_IRQHandler Unknown-Service", incommingPacket.service);
            break;
    }
    // prints("==========================\n");
    HW_set_32bit_reg(NI_RX, DONE);
    return 0;
}

////////////////////////////////////////////////////////////
// https://www.techiedelight.com/implement-itoa-function-in-c/
// Function to swap two numbers
void mySwap(char *x, char *y) {
    char t = *x; *x = *y; *y = t;
}
// Function to reverse `buffer[i…j]`
char* reverse(char *buffer, int i, int j){
    while (i < j) {
        mySwap(&buffer[i++], &buffer[j--]);
    }
 
    return buffer;
}
// Iterative function to implement `itoa()` function in C
char* myItoa(int value, char* buffer, int base){
    // invalid input
    if (base < 2 || base > 32) {
        return buffer;
    }
    // consider the absolute value of the number
    int n;
    if (value < 0)
        n = value * -1;
    else 
        n = value;

    int i = 0;
    while (n){
        int r = n % base;
 
        if (r >= 10) {
            buffer[i++] = 65 + (r - 10);
        }
        else {
            buffer[i++] = 48 + r;
        }
 
        n = n / base;
    }
    // if the number is 0
    if (i == 0) {
        buffer[i++] = '0';
    }
    // If the base is 10 and the value is negative, the resulting string
    // is preceded with a minus sign (-)
    // With any other base, value is always considered unsigned
    if (value < 0 && base == 10) {
        buffer[i++] = '-';
    }
    buffer[i] = '\0'; // null terminate string
    // reverse the string and return it
    return reverse(buffer, 0, i - 1);
}

////////////////////////////////////////////////////////////
// Receives a message and alocates it in the application structure
void ReceiveMessage(Message *theMessage, unsigned int from){
    unsigned int taskID = API_GetCurrentTaskSlot();
    printsvsv("msg pointer: ", (unsigned int)theMessage, "from: ", from);
    printsv("Minha task ID: ", taskID);
    //SendMessageRequest(from);
    return;
}

////////////////////////////////////////////////////////////
// Returns the PE address for a giver pair of coords
unsigned int makeAddress(unsigned int x, unsigned int y) {
    unsigned int address = 0x00000000;
    return (address | (x << 8) | y);
}

////////////////////////////////////////////////////////////
// Pushes one slot to the sending queue
void API_PushSendQueue(unsigned int type, unsigned int slot){
    SendingQueue[SendingQueue_front] = type | slot;
    if(SendingQueue_front == (PIPE_SIZE*2)-1){
        SendingQueue_front = 0;
    } else {
        SendingQueue_front++;
    }
    API_Try2Send();
    return;
}

////////////////////////////////////////////////////////////
// Pushes one slot to the sending queue
unsigned int API_PopSendQueue(){
    unsigned int element;
    if (SendingQueue_front == SendingQueue_tail){
        return EMPTY;
    } else {
        element = SendingQueue[SendingQueue_tail];
        if (SendingQueue_tail == (PIPE_SIZE*2)-1){
            SendingQueue_tail = 0;
        } else {
            SendingQueue_tail++;
        }
        return element;
    }
}

////////////////////////////////////////////////////////////
// Try to send some packet! 
void API_Try2Send(){
    unsigned int toSend;
    //prints("API_Try2Send\n");
    // Try to send the packet to NI if it's available
    vPortEnterCritical();
        // Checks if the NI is available to transmitt something
        if (HW_get_32bit_reg(NI_TX) == NI_STATUS_OFF){
            toSend = API_PopSendQueue();
            //printsv("toSend ", toSend);
            if (toSend != EMPTY){
                if((toSend & 0xFFFF0000) ==  SERVICE){
                    SendRaw((unsigned int)&ServicePipe[toSend & 0x0000FFFF].header);
                }
                else if((toSend & 0xFFFF0000) ==  MESSAGE){
                    SendRaw((unsigned int)&MessagePipe[toSend & 0x0000FFFF].header);
                }
                SendingSlot = toSend;
            }
        } 
    vPortExitCritical();
    return;
}

void API_AckTaskAllocation(unsigned int task_id, unsigned int app_id){
    unsigned int mySlot;
    do{
        mySlot = API_GetServiceSlot();
        if(mySlot == PIPE_FULL) vTaskDelay(1);
    }while(mySlot == PIPE_FULL);
    //printsv("I got a free service slot!! -> ", mySlot);

    ServicePipe[mySlot].holder = PIPE_SYS_HOLDER;

    ServicePipe[mySlot].header.header           = makeAddress(0, 0);
    ServicePipe[mySlot].header.payload_size     = PKT_SERVICE_SIZE;
    ServicePipe[mySlot].header.service          = TASK_ALLOCATION_SUCCESS;
    ServicePipe[mySlot].header.task_id          = task_id;
    ServicePipe[mySlot].header.task_app_id      = app_id;

    API_PushSendQueue(SERVICE, mySlot);
    return;    
}

void API_SendMessage(unsigned int addr, unsigned int taskID){
    unsigned int mySlot;
    unsigned int taskSlot;
    unsigned int i;
    Message *theMessage;
    do{
        mySlot = API_GetMessageSlot();
        if(mySlot == PIPE_FULL) vTaskDelay(1);
    }while(mySlot == PIPE_FULL);
    
    theMessage = addr;

    taskSlot = API_GetCurrentTaskSlot();
    
    MessagePipe[mySlot].holder = taskSlot;

    MessagePipe[mySlot].header.header           = TaskList[taskSlot].TasksMap[taskID];
    MessagePipe[mySlot].header.payload_size     = PKT_SERVICE_SIZE + theMessage->length + 1;
    MessagePipe[mySlot].header.service          = MESSAGE_DELIVERY;
    MessagePipe[mySlot].header.application_id   = TaskList[taskSlot].AppID;
    MessagePipe[mySlot].header.producer_task    = TaskList[taskSlot].TaskID;
    MessagePipe[mySlot].header.destination_task = taskID;
    MessagePipe[mySlot].msg.length              = theMessage->length;
    for (i = (PKT_SERVICE_SIZE+1); i < theMessage->length; i++){
        MessagePipe[mySlot].msg.msg[i]          = theMessage->msg[i-PKT_SERVICE_SIZE-1];
    }
    
    if (TaskList[taskSlot].PendingReq[taskID] == TRUE){
        API_PushSendQueue(MESSAGE, mySlot);
    }

    return;
}

void API_SendMessageReq(unsigned int addr, unsigned int taskID){
    unsigned int taskSlot;
    unsigned int mySlot;

    // Update task info
    taskSlot = API_GetCurrentTaskSlot();
    TaskList[taskSlot].waitingMsg = TRUE;
    TaskList[taskSlot].MsgToReceive = addr;

    // Sends the message request
    do{
        mySlot = API_GetServiceSlot();
        if(mySlot == PIPE_FULL) vTaskDelay(1);
    }while(mySlot == PIPE_FULL);

    ServicePipe[mySlot].holder = PIPE_SYS_HOLDER;

    ServicePipe[mySlot].header.header           = TaskList[taskSlot].TasksMap[taskID];
    ServicePipe[mySlot].header.payload_size     = PKT_SERVICE_SIZE;
    ServicePipe[mySlot].header.service          = MESSAGE_REQUEST;
    ServicePipe[mySlot].header.task_id          = TaskList[taskSlot].TaskID;
    ServicePipe[mySlot].header.task_app_id      = TaskList[taskSlot].AppID;
    ServicePipe[mySlot].header.producer_task_id = taskID;

    API_PushSendQueue(SERVICE, mySlot);
    prints("Enviando um Message Request!\n");
    // Bloquear a tarefa!
    while(TaskList[taskSlot].waitingMsg == TRUE){ 
        printsvsv("taskslot ", taskSlot, " esperando mensgaem ", 0);
        vTaskDelay(1); 
    }
    prints("Pacote recebido!\n");
    return;
}

unsigned int API_CheckMessagePipe(unsigned int requester_task_id, unsigned int task_app_id){
    unsigned int i;
    for (i = 0; i < PIPE_SIZE; i++){
        if(MessagePipe->status == PIPE_OCCUPIED){
            if(MessagePipe->header.application_id == task_app_id){
                if(MessagePipe->header.destination_task == requester_task_id){
                    return i;
                }
            }
        }
    }
    return ERRO;
}

void API_AddPendingReq(unsigned int requester_task_id, unsigned int task_app_id, unsigned int producer_task_id){
    unsigned int slot = API_GetTaskSlot(producer_task_id, task_app_id);
    TaskList[slot].PendingReq[requester_task_id] = TRUE;
    return;
}
