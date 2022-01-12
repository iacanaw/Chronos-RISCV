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
extern volatile Task TaskList[ NUM_MAX_TASKS ];

extern volatile unsigned int SendingQueue[PIPE_SIZE*2];
extern volatile unsigned int SendingQueue_front;
extern volatile unsigned int SendingQueue_tail;

extern volatile unsigned int SystemTemperature[DIM_Y*DIM_X];
extern unsigned int temperatureUpdated;


////////////////////////////////////////////////////////////
// Initialize Chronos stuff
void Chronos_init(){
    // Resets the amount of each executed instruction
    resetExecutedInstructions();

    //Informs the Router this tile ID, that is provided by Harness
    HW_set_32bit_reg(ROUTER_BASE, HW_get_32bit_reg(MY_ID)); 
    
    // Reads this processor address, calculated by the router
    ProcessorAddr = HW_get_32bit_reg(ROUTER_BASE);

    // Informs the PRINTER this tile address
    HW_set_32bit_reg(PRINTER_CHAR, getXpos(ProcessorAddr));
    HW_set_32bit_reg(PRINTER_CHAR, getYpos(ProcessorAddr));

    // Initialize the TaskList
    API_TaskListInit();
    
    // Initialize the Message & Service PIPE
    API_PipeInitialization();
    SendingQueue_front = 0;
    SendingQueue_tail = 0;

    // Configures the timer to interrupt at each ms
    HW_set_32bit_reg(NI_TIMER, 1000);

    // Informs the NI the address to store incoming packets
    HW_set_32bit_reg(NI_ADDR, (unsigned int)&incommingPacket.header);
    
    // Enables interruption from NI
    NI_enable_irq(TX_RX);

    // Set the system to Idle
    API_setFreqIdle();
    API_freqStepUp();
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
    prints(text1);
    printi(value1);
    prints("\n");
    return;
}

////////////////////////////////////////////////////////////
// Prints two strings and two integers interspersed
void printsvsv(char *text1, int value1, char *text2, int value2) {
    prints(text1);
    printi(value1);
    prints(text2);
    printi(value2);
    prints("\n");
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
    PLIC_EnableIRQ(NI_RX_IRQn);
    PLIC_SetPriority(NI_RX_IRQn, 1);
    PLIC_EnableIRQ(NI_TX_IRQn);
    PLIC_SetPriority(NI_TX_IRQn, 2);
    PLIC_EnableIRQ(Timer_IRQn);
    PLIC_SetPriority(Timer_IRQn, 3);
    return;
}

////////////////////////////////////////////////////////////
// Disables interruptions incomming from NI
void NI_disable_irq(int which){
    PLIC_DisableIRQ(NI_RX_IRQn);
    PLIC_DisableIRQ(NI_TX_IRQn);
    PLIC_DisableIRQ(Timer_IRQn);
    return;
}

////////////////////////////////////////////////////////////
// Interruption handler for the NI RX
uint8_t External_2_IRQHandler(void){    
    BaseType_t xHigherPriorityTaskWoken;
    prints("ENTREI NA INTERRUPCAO RX\n");
    /* Clear the interrupt. */
    HW_set_32bit_reg(NI_RX, HOLD);

    /* xHigherPriorityTaskWoken must be initialised to pdFALSE.
    If calling vTaskNotifyGiveFromISR() unblocks the handling
    task, and the priority of the handling task is higher than
    the priority of the currently running task, then
    xHigherPriorityTaskWoken will be automatically set to pdTRUE. */
    xHigherPriorityTaskWoken = pdFALSE;

    /* Unblock the handling task so the task can perform
    any processing necessitated by the interrupt.  xHandlingTask
    is the task's handle, which was obtained when the task was
    created.  vTaskNotifyGiveFromISR() also increments
    the receiving task's notification value. */
    vTaskNotifyGiveFromISR( NI_RX_Handler, &xHigherPriorityTaskWoken );

    /* Force a context switch if xHigherPriorityTaskWoken is now
    set to pdTRUE. The macro used to do this is dependent on
    the port and may be called portEND_SWITCHING_ISR. */
    portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );

    return 0;
}

////////////////////////////////////////////////////////////
// Interruption handler for the NI TX
uint8_t External_3_IRQHandler(void){    
    BaseType_t xHigherPriorityTaskWoken;
    prints("ENTREI NA INTERRUPCAO TX\n");
    /* Clear the interrupt. */
    HW_set_32bit_reg(NI_TX, HOLD);

    /* xHigherPriorityTaskWoken must be initialised to pdFALSE.
    If calling vTaskNotifyGiveFromISR() unblocks the handling
    task, and the priority of the handling task is higher than
    the priority of the currently running task, then
    xHigherPriorityTaskWoken will be automatically set to pdTRUE. */
    xHigherPriorityTaskWoken = pdFALSE;

    /* Unblock the handling task so the task can perform
    any processing necessitated by the interrupt.  xHandlingTask
    is the task's handle, which was obtained when the task was
    created.  vTaskNotifyGiveFromISR() also increments
    the receiving task's notification value. */
    vTaskNotifyGiveFromISR( NI_TX_Handler, &xHigherPriorityTaskWoken );

    /* Force a context switch if xHigherPriorityTaskWoken is now
    set to pdTRUE. The macro used to do this is dependent on
    the port and may be called portEND_SWITCHING_ISR. */
    portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );

    return 0;
}

////////////////////////////////////////////////////////////
// Interruption handler for the NI TMR
uint8_t External_4_IRQHandler(void){    
    BaseType_t xHigherPriorityTaskWoken;
    prints("ENTREI NA INTERRUPCAO TMR\n");
    /* Clear the interrupt. */
    HW_set_32bit_reg(NI_TIMER, HOLD);

    /* xHigherPriorityTaskWoken must be initialised to pdFALSE.
    If calling vTaskNotifyGiveFromISR() unblocks the handling
    task, and the priority of the handling task is higher than
    the priority of the currently running task, then
    xHigherPriorityTaskWoken will be automatically set to pdTRUE. */
    xHigherPriorityTaskWoken = pdFALSE;

    /* Unblock the handling task so the task can perform
    any processing necessitated by the interrupt.  xHandlingTask
    is the task's handle, which was obtained when the task was
    created.  vTaskNotifyGiveFromISR() also increments
    the receiving task's notification value. */
    vTaskNotifyGiveFromISR( NI_TMR_Handler, &xHigherPriorityTaskWoken );

    /* Force a context switch if xHigherPriorityTaskWoken is now
    set to pdTRUE. The macro used to do this is dependent on
    the port and may be called portEND_SWITCHING_ISR. */
    portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );

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
    vTaskEnterCritical();
    if(type == THERMAL){
        API_PrioritySend(type, slot);
    } else {
        SendingQueue[SendingQueue_front] = type | slot;
        printsv("SendingQueue_front: ", SendingQueue_front);
        if(SendingQueue_front == (PIPE_SIZE*2)-1){
            SendingQueue_front = 0;
        } else {
            SendingQueue_front++;
        }
        API_Try2Send();
    }
    vTaskExitCritical();
    return;
}


void API_PrioritySend(unsigned int type, unsigned int slot){
    unsigned int auxQ[PIPE_SIZE*2];
    unsigned int aux, i = 0;
    do{
        aux = API_PopSendQueue();
        auxQ[i] = aux;
        i++;
    }while(aux != EMPTY);
    API_PushSendQueue(type+1, slot);
    i = 0;
    do{
        aux = auxQ[i];
        if(aux != EMPTY)
            API_PushSendQueue((aux & 0xFFFF0000), (aux & 0x0000FFFF));
        i++;
    }while(aux != EMPTY);
    return;
}


////////////////////////////////////////////////////////////
// Pushes one slot to the sending queue
unsigned int API_PopSendQueue(){
    unsigned int element;
    vTaskEnterCritical();
    if (SendingQueue_front == SendingQueue_tail){
        vTaskExitCritical();
        return EMPTY;
    } else {
        element = SendingQueue[SendingQueue_tail];
        if (SendingQueue_tail == (PIPE_SIZE*2)-1){
            SendingQueue_tail = 0;
        } else {
            SendingQueue_tail++;
        }
        vTaskExitCritical();
        return element;
    }
}

////////////////////////////////////////////////////////////
// Try to send some packet! 
void API_Try2Send(){
    unsigned int toSend, taskID, slot, i;
    // Try to send the packet to NI if it's available
    // Checks if the NI is available to transmitt something
    if (HW_get_32bit_reg(NI_TX) == NI_STATUS_OFF){
        vTaskEnterCritical();
        toSend = API_PopSendQueue();
        if (toSend != EMPTY){
            SendingSlot = toSend;
            if((toSend & 0xFFFF0000) == SERVICE){
                SendRaw((unsigned int)&ServicePipe[toSend & 0x0000FFFF].header);
            }
            else if((toSend & 0xFFFF0000) == MESSAGE){
                taskID = (toSend & 0x0000FF00) >> 8;
                slot = toSend & 0x000000FF;
                SendRaw((unsigned int)&TaskList[taskID].MessagePipe[slot].header);
            }
            else if((toSend & 0xFFFF0000) == THERMAL){
                SendingSlot = THERMAL;
                SendRaw((unsigned int)&ThermalPacket.header);
            }
            else if((toSend & 0xFFFF0000) == SYS_MESSAGE){
                SendingSlot = SYS_MESSAGE;
                SendRaw((unsigned int)&ServiceMessage.header);
            } 
            else{
                printsv("ERROR! desconhecido!! ", toSend);
            }
            prints("API_Try2Send success!\n");
        } else {
            prints("API_Try2Send failed - empty SendQueue!\n");
        }
        vTaskExitCritical();
    } else {
        prints("API_Try2Send failed - NI_TX occupied!\n");
    }
    return;
}

void API_AckTaskAllocation(unsigned int task_id, unsigned int app_id){
    unsigned int mySlot;
    do{
        mySlot = API_GetServiceSlot();
        if(mySlot == PIPE_FULL){
            // Runs the NI Handler to send/receive packets, opening space in the PIPE
            prints("Estou preso aqui1...\n");
            API_NI_Handler();
        }
    }while(mySlot == PIPE_FULL);

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
    unsigned int mySlot, slot, taskSlot;
    unsigned int i;
    Message *theMessage;
    do{
        vTaskEnterCritical();
        mySlot = API_GetMessageSlot();
        if(mySlot == PIPE_FULL){
            // Runs the NI Handler to send/receive packets, opening space in the PIPE
            prints("Estou preso aqui2...\n");
            //API_NI_Handler();
            taskSlot = API_GetCurrentTaskSlot();
            TaskList[taskSlot].status = TASK_SLOT_SUSPENDED;
            vTaskExitCritical();
            vTaskSuspend( TaskList[taskSlot].TaskHandler );
            vTaskEnterCritical();
        }
    }while(mySlot == PIPE_FULL);
    
    theMessage = addr;

    taskSlot = (mySlot & 0x0000FF00) >> 8;//API_GetCurrentTaskSlot();
    slot = mySlot & 0x000000FF;
    printsv("taskSlot: ", taskSlot);
    printsvsv("Adding a msg to task ", taskID, " in the PIPE slot ", mySlot);
    printsv("from app: ", TaskList[taskSlot].AppID);
    //MessagePipe[mySlot].holder = taskSlot;

    TaskList[taskSlot].MessagePipe[slot].header.header           = TaskList[taskSlot].TasksMap[taskID];
    TaskList[taskSlot].MessagePipe[slot].header.payload_size     = PKT_SERVICE_SIZE + theMessage->length + 1;
    TaskList[taskSlot].MessagePipe[slot].header.service          = MESSAGE_DELIVERY;
    TaskList[taskSlot].MessagePipe[slot].header.application_id   = TaskList[taskSlot].AppID;
    TaskList[taskSlot].MessagePipe[slot].header.producer_task    = TaskList[taskSlot].TaskID;
    TaskList[taskSlot].MessagePipe[slot].header.destination_task = taskID;
    TaskList[taskSlot].MessagePipe[slot].msg.length              = theMessage->length;
    for (i = 0; i < theMessage->length; i++){
        TaskList[taskSlot].MessagePipe[slot].msg.msg[i]          = theMessage->msg[i];
    }
    
    if (TaskList[taskSlot].PendingReq[taskID] == TRUE){
        prints(">>>>>Achei aqui no pending!\n");
        TaskList[taskSlot].PendingReq[taskID] = FALSE;
        API_PushSendQueue(MESSAGE, mySlot);
    }
    vTaskExitCritical();
    return;
}

void API_SendFinishTask(unsigned int task_id, unsigned int app_id){
    unsigned int mySlot;
    do{
        mySlot = API_GetServiceSlot();
        if(mySlot == PIPE_FULL){
            // Runs the NI Handler to send/receive packets, opening space in the PIPE
            prints("Estou preso aqui3...\n");
            API_NI_Handler();
        }
    }while(mySlot == PIPE_FULL);

    ServicePipe[mySlot].holder = PIPE_SYS_HOLDER;

    ServicePipe[mySlot].header.header           = makeAddress(0, 0);
    ServicePipe[mySlot].header.payload_size     = PKT_SERVICE_SIZE;
    ServicePipe[mySlot].header.service          = TASK_FINISH;
    ServicePipe[mySlot].header.task_id          = task_id;
    ServicePipe[mySlot].header.task_app_id      = app_id;
    ServicePipe[mySlot].header.task_dest_addr   = ProcessorAddr;
    API_PushSendQueue(SERVICE, mySlot);
    return;    
}

void API_SendMessageReq(unsigned int addr, unsigned int taskID){
    unsigned int taskSlot;
    unsigned int mySlot;
    volatile unsigned int idle = 0;
    // Update task info
    taskSlot = API_GetCurrentTaskSlot();
    TaskList[taskSlot].waitingMsg = TRUE;
    TaskList[taskSlot].MsgToReceive = addr;

    //const TickType_t xBlockTime = 1000;
	//uint32_t ulNotifiedValue;

    //printsv("API_SendMessageReq addr: ", addr);

    // Sends the message request
    do{
        mySlot = API_GetServiceSlot();
        if(mySlot == PIPE_FULL){
            // Runs the NI Handler to send/receive packets, opening space in the PIPE
            prints("Estou preso aqui4...\n");
            API_NI_Handler();
        }
    }while(mySlot == PIPE_FULL);

    ServicePipe[mySlot].holder = PIPE_SYS_HOLDER;

    ServicePipe[mySlot].header.header           = TaskList[taskSlot].TasksMap[taskID];
    ServicePipe[mySlot].header.payload_size     = PKT_SERVICE_SIZE;
    ServicePipe[mySlot].header.service          = MESSAGE_REQUEST;
    ServicePipe[mySlot].header.task_id          = TaskList[taskSlot].TaskID;
    ServicePipe[mySlot].header.task_app_id      = TaskList[taskSlot].AppID;
    ServicePipe[mySlot].header.producer_task_id = taskID;

    prints("Esperando Mensagem!\n");

    API_PushSendQueue(SERVICE, mySlot);
    
    API_setFreqScale(100);
    // Bloquear a tarefa!
    while(TaskList[taskSlot].waitingMsg == TRUE){ 
        vTaskDelay(1);
    }

    // while( TaskList[taskSlot].waitingMsg == TRUE ){
    //     TaskList[taskSlot].status = TASK_SLOT_BLOQUED;
    //     // Blocks the task until the NI interrupts it
    //     ulNotifiedValue = ulTaskNotifyTake( pdFALSE, xBlockTime );
    // }


    API_setFreqScale(1000);

    prints("Mensagem Recebida!\n");
    return;
}

unsigned int API_CheckMessagePipe(unsigned int requester_task_id, unsigned int task_app_id){
    unsigned int i,j;
    unsigned int sel = ERRO;
    unsigned int smallID = 268435455;
    //vTaskEnterCritical();
    for(i = 0; i < NUM_MAX_TASKS; i++){
        if(TaskList[i].status != TASK_SLOT_EMPTY && TaskList[i].AppID == task_app_id){
            for(j = 0; j < PIPE_SIZE; j++){
                if(TaskList[i].MessagePipe[j].status == PIPE_OCCUPIED){
                    if(TaskList[i].MessagePipe[j].header.destination_task == requester_task_id){
                        if(TaskList[i].MessagePipe[j].msgID < smallID){
                            smallID = TaskList[i].MessagePipe[j].msgID;
                            sel = (i << 8) | j;
                        }
                    }
                }
            }
        }
    }
    if(sel != ERRO){
        TaskList[sel >> 8].MessagePipe[sel & 0x000000FF].status == PIPE_TRANSMITTING;
    }
    //vTaskExitCritical();
    return sel;
}

void API_AddPendingReq(unsigned int requester_task_id, unsigned int task_app_id, unsigned int producer_task_id){
    unsigned int slot = API_GetTaskSlot(producer_task_id, task_app_id);
    TaskList[slot].PendingReq[requester_task_id] = TRUE;
    return;
}


void API_NI_Handler(){
    return;
}