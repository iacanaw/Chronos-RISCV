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

extern volatile unsigned int SystemTemperature[DIM_X*DIM_Y];
extern volatile unsigned int SystemFIT[DIM_X*DIM_Y];
extern volatile unsigned int temperatureUpdated;

static unsigned int XORTable[128] = {0xea67e5d5, 0x79a32729, 0xdc1b95b2, 0x03d66b2c, 0x34992d81, 
0xfe3a5cbb, 0x2fd12192, 0xa4ad71f0, 0xf6af45ff, 0x862e2584, 0x4ed69471, 0xa70b6e13, 0x0479746d, 
0x6ad61984, 0xfb0fde6b, 0x6ccb7b44, 0xcc0a4380, 0x65a2ad29, 0x0c140f65, 0xf282eb6c, 0x5ebf3a09,
0xa73aa442, 0x992f5c2e, 0x52b1ead3, 0xff08bdc9, 0x74d92572, 0xfcb55133, 0xa3dc83f7, 0xa1c5203f,
0x7abb8186, 0xa6097327, 0x328561ae, 0x0964de19, 0x9f94b6c5, 0x225e1f08, 0xf78812dc, 0x12b16ffd, 
0xa6b97aee, 0x0cdca795, 0x6e620685, 0xdddc48e9, 0x713bdb6a, 0x21bee58f, 0xd9670b69, 0xc10236f3, 
0x23c95808, 0x83cd7334, 0xa5a69190, 0x15163738, 0xea640e57, 0xadc905ca, 0xc949eb61, 0x020b18ed, 
0x11ae8593, 0xcd6fe0dd, 0xc05779b1, 0xfc9bad7e, 0x395abe38, 0x1595cbc0, 0x69d9c0e6, 0xaa999e82, 
0x4241f339, 0x6f4abbb7, 0x7dc9363a, 0xd976af82, 0x29cec2e8, 0x2c8e1364, 0xdfcd0284, 0x1cabcc1e, 
0x19041777, 0x87c97db0, 0x7f86af5b, 0xf4d735a5, 0xdf34a0c5, 0x86a93440, 0xca2b808f, 0xfdc43f07, 
0x924f6af9, 0x932de707, 0x74d337cb, 0x6a4eef0c, 0x45d7d4ee, 0x6615ab2d, 0x8a8e7bab, 0x850c97b8, 
0x7703adfb, 0xbecd7831, 0x1ab05d41, 0x1c066892, 0x9f568b85, 0x475b63bd, 0xbff7c794, 0xcadc938b, 
0xabf8a2a5, 0x1a435dde, 0x696c8b3a, 0x3c6a4fa5, 0xf668a682, 0xa4fc6d81, 0xd0df5f82, 0x199b33c4, 
0xdf83bdcd, 0x5b5f3157, 0x99feb290, 0x1a914da5, 0xed1c6b76, 0xcbae4d43, 0x2c65d4a4, 0xd4f6a2a9, 
0x89eef7d0, 0xded87233, 0x86a72590, 0x852729d1, 0xed3f4fe4, 0x756c871b, 0x2f94fa7c, 0x53a35888, 
0x6b190128, 0x57319f8d, 0x2a4b8304, 0x01372ad2, 0xa8d518d4, 0x4fae06c9, 0xbab6114a, 0x1946b712, 
0x325308dc, 0x056b2ffb, 0xfe3c6e88};

unsigned int RandomSeed = 0xace1ace1a;

unsigned int random(){
    unsigned bit = ((XORTable[RandomSeed & 0x7f] >> (RandomSeed & 0xF)) ^ (RandomSeed >> 2) ^ (RandomSeed >> 3) ^ (XORTable[RandomSeed & 0x7f] >> (RandomSeed & 0xF)) ) & 1;
    return RandomSeed = (RandomSeed >> 1) | (bit << 15);
}

////////////////////////////////////////////////////////////
// Initialize Chronos stuff
void Chronos_init(){
    // Resets the amount of each executed instruction
    resetExecutedInstructions();

    // Reset the window counter
    measuredWindows = 0;

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
    /* Clear the interrupt */
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

unsigned int id2addr(unsigned int id){
    unsigned int x = id % DIM_X;
    unsigned int y = id / DIM_Y;
    unsigned int address = 0x00000000;
    return (address | (x << 8) | y);
}

unsigned int addr2id(unsigned int addr){
    unsigned int x = getXpos(addr);
    unsigned int y = getYpos(addr);
    return (y*DIM_X + x);
}

////////////////////////////////////////////////////////////
// Pushes one slot to the sending queue
void API_PushSendQueue(unsigned int type, unsigned int slot){
    unsigned int mySlot;
    vTaskEnterCritical();
    if( type == THERMAL ){
        API_PrioritySend(type, slot);
    } else if ( type == MIGRATION ){
        API_PrioritySend(type, slot);
    } else {
        SendingQueue[SendingQueue_front] = (0xFFFF0000 & type) | slot;
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
    vTaskEnterCritical();
    if (HW_get_32bit_reg(NI_TX) == NI_STATUS_OFF){
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
                printsv("Sending message id: ", TaskList[taskID].MessagePipe[slot].msgID);
            }
            else if((toSend & 0xFFFF0000) == THERMAL){
                SendingSlot = THERMAL;
                SendRaw((unsigned int)&ThermalPacket.header);
            }
            else if((toSend & 0xFFFF0000) == SYS_MESSAGE){
                SendingSlot = SYS_MESSAGE;
                SendRaw((unsigned int)&ServiceMessage.header);
            }
            else if((toSend & 0xFFFF0000) == MIGRATION){
                SendingSlot = MIGRATION;
                SendRaw((unsigned int)TaskList[toSend & 0x0000FFFF].fullAddr);
            }
            else{
                printsv("ERROR! desconhecido!! ", toSend);
            }
            prints("API_Try2Send success!\n");
        } else {
            prints("API_Try2Send failed - empty SendQueue!\n");
        }
    } else {
        prints("API_Try2Send failed - NI_TX occupied!\n");
    }
    vTaskExitCritical();
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
    ServicePipe[mySlot].header.app_id      = app_id;

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
            //vTaskExitCritical();
            vTaskSuspend( TaskList[taskSlot].TaskHandler );
            prints("Volteiiii!\n");
            //vTaskEnterCritical();
            //vTaskExitCritical();
        } /*else {
            vTaskExitCritical();
        }*/
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
    ServicePipe[mySlot].header.app_id      = app_id;
    ServicePipe[mySlot].header.task_dest_addr   = ProcessorAddr;
    API_PushSendQueue(SERVICE, mySlot);
    return;    
}

void API_SendMessageReq(unsigned int addr, unsigned int taskID){
    unsigned int taskSlot, i;
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
    ServicePipe[mySlot].header.app_id           = TaskList[taskSlot].AppID;
    ServicePipe[mySlot].header.producer_task_id = taskID;

    prints("Esperando Mensagem!\n");

    API_PushSendQueue(SERVICE, mySlot);
    
    //
    for (i = 0; i < NUM_MAX_TASKS; i++){
        if ( TaskList[i].status == TASK_SLOT_RUNNING && TaskList[i].waitingMsg == FALSE ){
            i = 999;
            printsv("Not changing the frequency because task # is running: ", i);
            break;
        }
    }
    if (i < 999) {
        API_setFreqScale(100);
    }

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

unsigned int API_CheckMessagePipe(unsigned int requester_task_id, unsigned int app_id){
    unsigned int i,j;
    unsigned int sel = ERRO;
    unsigned int smallID = 268435455;
    vTaskEnterCritical();
    for(i = 0; i < NUM_MAX_TASKS; i++){
        if(TaskList[i].status != TASK_SLOT_EMPTY && TaskList[i].AppID == app_id){
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
        TaskList[sel >> 8].MessagePipe[sel & 0x000000FF].header.header = TaskList[sel>>8].TasksMap[requester_task_id];
    }
    vTaskExitCritical();
    return sel;
}

void API_AddPendingReq(unsigned int requester_task_id, unsigned int app_id, unsigned int producer_task_id){
    unsigned int slot = API_GetTaskSlot(producer_task_id, app_id);
    unsigned int mySlot;
    if (slot != ERRO) {
        TaskList[slot].PendingReq[requester_task_id] = TRUE;
    } else {
        printsvsv("Task ", requester_task_id, "solicitando pacote da task ", producer_task_id);
        printsv("no PE errado - applicacao: ", app_id);
        do{
            mySlot = API_GetServiceSlot();
            if(mySlot == PIPE_FULL){
                // Runs the NI Handler to send/receive packets, opening space in the PIPE
                prints("Estou preso aqui111...\n");
            }
        }while(mySlot == PIPE_FULL);
        
        ServicePipe[mySlot].holder = PIPE_SYS_HOLDER;

        ServicePipe[mySlot].header.header           = makeAddress(0, 0);
        ServicePipe[mySlot].header.payload_size     = PKT_SERVICE_SIZE;
        ServicePipe[mySlot].header.service          = LOST_MESSAGE_REQ;
        ServicePipe[mySlot].header.task_id          = requester_task_id;
        ServicePipe[mySlot].header.app_id           = app_id;
        ServicePipe[mySlot].header.producer_task_id = producer_task_id;

        API_PushSendQueue(SERVICE, mySlot);
    }
    return;
}

void API_NI_Handler(){
    return;
}

void debug_task_alloc(unsigned int tick, unsigned int app, unsigned int task, unsigned int addr){
    vTaskEnterCritical();
    prints("\nDebugger, ");
    printi(tick);
    prints(", ");
    printi(addr);
    prints(", ");
    printi(40);
    prints(", ");
    printi(0);
    prints(", ");
    printi(0);
    prints(", ");
    printi(0);
    prints(", ");
    printi(addr);
    prints(", ");
    printi((256*app)+task);
    prints("\n");
    vTaskExitCritical();
    return;
}

void debug_task_dealloc(unsigned int tick, unsigned int app, unsigned int task, unsigned int addr){
    vTaskEnterCritical();
    prints("\nDebugger, ");
    printi(tick);
    prints(", ");
    printi(addr);
    prints(", ");
    printi(70);
    prints(", ");
    printi(4);
    prints(", ");
    printi(0);
    prints(", ");
    printi(4);
    prints(", ");
    printi(-1);
    prints(", ");
    printi((256*app)+task);
    prints("\n");
    vTaskExitCritical();
    return;
}
