
////////////////////////////////////////////////////////////////////////////////
//
//                W R I T T E N   B Y   I M P E R A S   I G E N
//
//                             Version 20210408.0
//
////////////////////////////////////////////////////////////////////////////////

#include "networkInterface.igen.h"
#include "peripheral/impTypes.h"
#include "peripheral/bhm.h"
#include "peripheral/bhmHttp.h"
#include "peripheral/ppm.h"
#include "../whnoc_dma/noc.h"
#include <string.h>

/////////////////////////// Big endian/Little endian ///////////////////////////
#define __bswap_constant_32(x) \
     ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) |		      \
      (((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24))

unsigned int htonl(unsigned int x){
    return x;//__bswap_constant_32(x);
}

/////////////////////////////////// Variables ///////////////////////////////////

// Auxiliar address to store temporarily the address incomming address from the processor
unsigned int auxAddress;

// Stores the address to write the interruptionType
unsigned int intTypeAddr = 0xFFFFFFFF;

// Stores the vector adress to put received packets
unsigned int addressStart = 0xFFFFFFFF;

// Auxiliar pointer to store the current RX address position 
unsigned int receivingAddress = 0;

// Used to identify which packet field is the next to be read
unsigned int receivingField = HEADER;

// Counts the amount of remaining flits to be received
unsigned int receivingCount;

// Stores TX packet address
unsigned int transmittingAddress = 0;

// Flag to determine the transmittion end
unsigned int transmittionEnd = FALSE;

// Counts the amount of remaining flits to be transmitted
unsigned int transmittingCount = HEADER;

// Stores the input buffer status
unsigned int control_in_STALLGO = GO;

// Stores the availability to receive something from the NoC and write it in the memory
unsigned int myStatus = GO;

// TX Status
volatile unsigned int control_TX = NI_STATUS_OFF;

// RX Status
volatile unsigned int control_RX = NI_STATUS_OFF;

// Timer Status
volatile unsigned int control_TIMER = NI_STATUS_OFF;

// Stores the amount of time that the timer must wait until interrupt again
volatile double timer_us = (double)0.0;

// Global variable to handle a flit in VECTOR type
char chFlit[4];

// Global variable to handle a flit in UNSIGNED type
unsigned int usFlit;

//////////////////////////////  Funcions  //////////////////////////////

// Transform a flit from VECTOR to UNSIGNED INT (using the global variables)
void vec2usi(){
    unsigned int auxFlit = 0x00000000;
    unsigned int aux;
    aux = 0x000000FF & chFlit[3];
    auxFlit = ((aux << 24) & 0xFF000000);
    aux = 0x000000FF & chFlit[2];
    auxFlit = auxFlit | ((aux << 16) & 0x00FF0000);
    aux = 0x000000FF & chFlit[1];
    auxFlit = auxFlit | ((aux << 8) & 0x0000FF00);
    aux = 0x000000FF & chFlit[0];
    auxFlit = auxFlit | ((aux) & 0x000000FF);
    usFlit = auxFlit;
    return;
}

// Transform a flit from UNSIGNED INT to VECTOR (using the global variables)
void usi2vec(){
    chFlit[3] = (usFlit >> 24) & 0x000000FF;
    chFlit[2] = (usFlit >> 16) & 0x000000FF;
    chFlit[1] = (usFlit >> 8) & 0x000000FF;
    chFlit[0] = usFlit & 0x000000FF;
}

void interruptionOff(){
    if ( control_RX != NI_STATUS_INTER ){
        if ( control_TX != NI_STATUS_INTER ){
            if ( control_TIMER != NI_STATUS_INTER ){
                ppmWriteNet(handles.INT_NI, 0);
            }
        }
    }
}

void interruptionOn(){
    ppmWriteNet(handles.INT_NI, 1);
}

// Sets the local status to GO, allowing flits to be transmitted to the NI
void setGO(){
    ////bhmMessage("I", "CONTROL", "GO!\n");
    myStatus = GO;
    ppmPacketnetWrite(handles.controlPort, &myStatus, sizeof(myStatus));
}

// Sets the local status to STALL, blocking the flits inside the local router
void setSTALL(){
    ////bhmMessage("I", "CONTROL", "STALL!\n");
    myStatus = STALL;
    ppmPacketnetWrite(handles.controlPort, &myStatus, sizeof(myStatus));
}

// Inform a iteration to the Router - THIS IS USED ONLY FOR LOCAL ITERATIONS
void informIteration(){
    unsigned long long int iterate = 0xFFFFFFFFFFFFFFFFULL;
    ppmPacketnetWrite(handles.controlPort, &iterate, sizeof(iterate));
}

// Writes a flit inside an address in the processor memory
void writeMem(unsigned int flit, unsigned int addr){
    usFlit = flit;
    usi2vec();
    ppmAddressSpaceHandle h = ppmOpenAddressSpace("MWRITE");
    if(!h) {
        bhmMessage("I", "NI_ITERATOR", "ERROR_WRITE h handling!");
        while(1){} // error handling
    }
    ppmWriteAddressSpace(h, addr, sizeof(chFlit), chFlit);
    ppmCloseAddressSpace(h);
    return;
}

// Reads a flit from a given address
unsigned int readMem(unsigned int addr){
    ppmAddressSpaceHandle h = ppmOpenAddressSpace("MREAD");
    if(!h) {
        bhmMessage("I", "NI_ITERATOR", "ERROR_READ h handling!");
        while(1){} // error handling
    }
    ppmReadAddressSpace(h, addr, sizeof(chFlit), chFlit);
    ppmCloseAddressSpace(h);
    vec2usi(); // transform the data from vector to a single unsigned int
    return usFlit;
}

// Runs this function when the NI receives a Iteration from the ITERATOR
void niIteration(){
    // If there is something to send and the router has space, send a flit
    if(control_TX == NI_STATUS_ON && control_in_STALLGO == GO){
        // If the transmittion isn't finished yet...
        if(transmittingCount != EMPTY){
            // Reads a flit from the memory
            usFlit = readMem(transmittingAddress);
            bhmMessage("I", "TX", "Sending flit %x: %x", transmittingCount, usFlit);

            // Runs the logic to get the packet size and the end-of-packet 
            if(transmittingCount == HEADER){
                transmittingCount = SIZE;
            }
            else if(transmittingCount == SIZE){
                transmittingCount = htonl(usFlit);
            }
            else{
                transmittingCount = transmittingCount - 1;    
            }

            // Increments the memory pointer to get the next flit
            transmittingAddress = transmittingAddress + 4;

            // Sends the data to the local router
            ppmPacketnetWrite(handles.dataPort, &usFlit, sizeof(usFlit));
        }
        // If the packet transmittion is done, change the NI status to IDLE
        if(transmittingCount == EMPTY){
            control_TX = NI_STATUS_INTER; 
            //bhmMessage("I", "NI_RX", "Levantando interrupt TX - FINISHED TRANSMITTING\n");
            //ppmWriteNet(handles.INT_NI_TX, 1); // Turns the interruption on
            //ppmWriteNet(handles.INT_NI_RX, 1); // Turns the interruption on
            interruptionOn();
        }
    }
}

// Resets the receiving address to the first position of the array
void resetReceivingAddress(){
    receivingAddress = addressStart;
    //bhmMessage("INFO", "NI_RX", "Reseting addr: %x", receivingAddress);
    return;
}



//////////////////////////////// Callback stubs ////////////////////////////////

PPM_REG_READ_CB(addressRead) {
    if(bytes != 4) {
        //bhmMessage("E", "PPM_RNB", "Only 4 byte access permitted");
        return 0;
    }
    // YOUR CODE HERE (addressRead)
    return *(Uns32*)user;
}

PPM_REG_WRITE_CB(addressWrite) {
    if(bytes != 4) {
        return;
    }
    // In the beggining of everything the PE will write two addresses in the NI
    //  They will be used to write the incomming packet
    if(addressStart == 0xFFFFFFFF){
        addressStart = htonl(data);
        //bhmMessage("I", "NI", "Recebi endereco 1: %x", addressStart);
    }
    else{
        // When receiving the another address, then it is an address with a packet to transmitt
        auxAddress = htonl(data);
        //bhmMessage("I", "NI_ADDR", "Recebi endereco: %x", auxAddress);
    }
}

PPM_PACKETNET_CB(controlPortUpd) {
    // Input information with 4 bytes are about flow control
    if(bytes == 4){
        unsigned int ctrl = *(unsigned int *)data;
        control_in_STALLGO = ctrl;
    }
    // Information with 8 bytes are about iterations
    else if(bytes == 8) {
        niIteration();
    }
}

PPM_PACKETNET_CB(dataPortUpd) {
    unsigned int flit = *((unsigned int*)data);
    
    // This will happen if the NI is receiving a service packet when it is in a idle state
    if(control_RX == NI_STATUS_OFF){
        control_RX = NI_STATUS_ON;
        resetReceivingAddress();
        receivingField = HEADER;
        receivingCount = 0xFF; // qqrcoisa diferente de ZERO
    }

    // Receiving process
    if(control_RX == NI_STATUS_ON){
        bhmMessage("I", "NI", "Escrevendo dado %x na posicao %x\n", flit, receivingAddress);
        if(receivingField == HEADER){
            receivingField = SIZE;
            writeMem(flit, receivingAddress);
            receivingAddress = receivingAddress + 4;    // Increments the pointer, to write the next flit
            //bhmMessage("INFO", "NI_RX", "HEADER:\t\t%x", flit);
        }
        else if(receivingField == SIZE){
            receivingField = 1;
            receivingCount = htonl(flit);
            writeMem(flit, receivingAddress);
            receivingAddress = receivingAddress + 4;    // Increments the pointer, to write the next flit
            //bhmMessage("INFO", "NI_RX", "SIZE:\t\t\t%x", flit);
        }
        else{
            receivingField++;
            receivingCount = receivingCount - 1;
            writeMem(flit, receivingAddress);
            receivingAddress = receivingAddress + 4;    // Increments the pointer, to write the next flit
            //bhmMessage("INFO", "NI_RX", "PAYLOAD %i:\t\t%x @ %x", receivingField, flit, receivingAddress-4);
        }

        if(receivingField == 12 && receivingCount != EMPTY){ // end of the header reception!
            control_RX = NI_STATUS_WAITING;
            setSTALL();
            interruptionOn();
            //bhmMessage("I", "NI_RX", "Levantando interrupt RX - WAITING\n");
        }
    }

    // Detects the receiving finale
    if(receivingCount == EMPTY && control_RX == NI_STATUS_ON){
        setSTALL();
        control_RX = NI_STATUS_INTER;
        interruptionOn();
        //bhmMessage("I", "NI_RX", "Levantando interrupt RX - DELIVER\n");
    }
}

PPM_REG_READ_CB(statusRXRead) {
    if(bytes != 4) {
        bhmMessage("E", "PPM_RNB", "Only 4 byte access permitted");
        return 0;
    }
    //DMAC_ab8_data.statusRX.value = htonl(control_RX);
    return control_RX;
}

PPM_REG_WRITE_CB(statusRXWrite) {
    if(bytes != 4) {
        //bhmMessage("E", "PPM_WNB", "Only 4 byte access permitted");
        return;
    }
    unsigned int command = htonl(data);

    if(control_RX == NI_STATUS_ON){
        if(command == DONE){
            interruptionOff();
            setGO();
        }
    } else if(control_RX == NI_STATUS_INTER){
        if(command == DONE){
            control_RX = NI_STATUS_OFF;
            interruptionOff();
            setGO();
        }
    } else if(control_RX == NI_STATUS_WAITING){
        receivingAddress = htonl(data);
        control_RX = NI_STATUS_ON;
    }
    
    *(Uns32*)user = data;
}

PPM_REG_READ_CB(statusTXRead) {
    if(bytes != 4) {
        bhmMessage("E", "PPM_RNB", "Only 4 byte access permitted");
        return 0;
    }
    DMAC_ab8_data.statusTX.value = htonl(control_TX);
    //bhmMessage("I", "NI_TX", "Reading TX - status: %x", control_TX);
    if(control_TX == NI_STATUS_OFF || control_TX == NI_STATUS_STARTING){
        //bhmMessage("I", "NI_TX", "Recebi um TX_STARTING");
        control_TX = NI_STATUS_STARTING;
        return (unsigned int)NI_STATUS_OFF;
    }
    else
        return control_TX;
}

PPM_REG_WRITE_CB(statusTXWrite) {
    if(bytes != 4) {
        //bhmMessage("E", "PPM_WNB", "Only 4 byte access permitted");
        return;
    }
    unsigned int command = htonl(data);
    // if(command == INTERRUPTION_ACK){
    //     bhmMessage("I", "statusWrite", "Turning interruption off!");
    //     ppmWriteNet(handles.INT_NI_RX, 0);  // Turn the interruption signal down
    //     return;
    // }

    if(command == TX){
        //bhmMessage("I", "NI_TX", "Recebi um TX");
        if(control_TX == NI_STATUS_STARTING || control_TX == NI_STATUS_OFF){
            control_TX = NI_STATUS_ON;
            transmittingAddress = auxAddress;
            transmittingCount = HEADER;
            niIteration();  // Send a flit to the ROUTER, this way it will inform the iterator that this PE is waiting for "iterations"
        }
        else{
            bhmMessage("I", "statusWrite", "ERROR_TX: UNEXPECTED STATE REACHED"); while(1){}
        }
    }
    else if(command == DONE){
        //bhmMessage("I", "NI_TX", "Recebi um TX_DONE");
        if(control_TX == NI_STATUS_INTER){    
            control_TX = NI_STATUS_OFF;
            interruptionOff();
        }
        else{
            bhmMessage("I", "statusWrite", "ERROR_DONE_TX: UNEXPECTED STATE REACHED"); while(1){}
        }
    }
    else if (command == RESET){
        if(control_TX == NI_STATUS_STARTING){
            control_TX = NI_STATUS_OFF;
        }
    }
    *(Uns32*)user = data;
}

PPM_REG_READ_CB(timerRead) {
    if(bytes != 4) {
        bhmMessage("E", "PPM_RNB", "Only 4 byte access permitted");
        return 0;
    }
    DMAC_ab8_data.timer.value = htonl(control_TIMER);
    return (unsigned int)control_TIMER;
}

PPM_REG_WRITE_CB(timerWrite) {
    unsigned int command = htonl(data);
    if(bytes != 4) {
        return;
    } else { 
        if(command == DONE){
            control_TIMER = NI_STATUS_OFF;
            //bhmMessage("I", "TIMER", "Timer turning interruption down! at %lf",bhmGetCurrentTime());
            interruptionOff();
        }
        else{
            timer_us = (double)command;
            bhmMessage("I", "TIMER", "Timer set to interrupt the processor once every %.2lf us!",timer_us);
        }
    }
    *(Uns32*)user = data;
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

/////////////////////////////// Port Declarations //////////////////////////////

DMAC_ab8_dataT DMAC_ab8_data;

handlesT handles;

/////////////////////////////// Diagnostic level ///////////////////////////////

// Test this variable to determine what diagnostics to output.
// eg. if (diagnosticLevel >= 1) bhmMessage("I", "networkInterface", "Example");
//     Predefined macros PSE_DIAG_LOW, PSE_DIAG_MEDIUM and PSE_DIAG_HIGH may be used
Uns32 diagnosticLevel;

/////////////////////////// Diagnostic level callback //////////////////////////

static void setDiagLevel(Uns32 new) {
    diagnosticLevel = new;
}

///////////////////////////// MMR Generic callbacks ////////////////////////////

static PPM_VIEW_CB(view32) {  *(Uns32*)data = *(Uns32*)user; }

//////////////////////////////// Bus Slave Ports ///////////////////////////////

static void installSlavePorts(void) {
    handles.DMAC = ppmCreateSlaveBusPort("DMAC", 16);
    if (!handles.DMAC) {
        bhmMessage("E", "PPM_SPNC", "Could not connect port 'DMAC'");
    }

}

//////////////////////////// Memory mapped registers ///////////////////////////

static void installRegisters(void) {

    {
        ppmCreateRegister(
            "ab8_address",
            0,
            handles.DMAC,
            0x0,
            4,
            addressRead,
            addressWrite,
            view32,
            &(DMAC_ab8_data.address.value),
            True
        );
    }
    {
        ppmCreateRegister(
            "ab8_statusTX",
            0,
            handles.DMAC,
            0x4,
            4,
            statusTXRead,
            statusTXWrite,
            view32,
            &(DMAC_ab8_data.statusTX.value),
            True
        );
    }
    {
        ppmCreateRegister(
            "ab8_statusRX",
            0,
            handles.DMAC,
            0x8,
            4,
            statusRXRead,
            statusRXWrite,
            view32,
            &(DMAC_ab8_data.statusRX.value),
            True
        );
    }
    {
        ppmCreateRegister(
            "ab8_timer",
            0,
            handles.DMAC,
            0xc,
            4,
            timerRead,
            timerWrite,
            view32,
            &(DMAC_ab8_data.timer.value),
            True
        );
    }

}

/////////////////////////////// Bus Master Ports ///////////////////////////////

static void installMasterPorts(void) {
    handles.MREAD = ppmOpenAddressSpace("MREAD");
    handles.MWRITE = ppmOpenAddressSpace("MWRITE");
}

PPM_DOC_FN(installDocs){

    ppmDocNodeP Root1_node = ppmDocAddSection(0, "Root");
    {
        ppmDocNodeP doc2_node = ppmDocAddSection(Root1_node, "Description");
        ppmDocAddText(doc2_node, "A OVP DMA for a router");
    }
}
////////////////////////////////// Constructor /////////////////////////////////

PPM_CONSTRUCTOR_CB(periphConstructor) {
    installSlavePorts();
    installRegisters();
    installMasterPorts();
}

///////////////////////////////////// Main /////////////////////////////////////

int main(int argc, char *argv[]) {

    diagnosticLevel = 0;
    bhmInstallDiagCB(setDiagLevel);
    constructor();

    while(1){
        if(timer_us == (double)0.0){
            bhmWaitDelay(1000); // if the timer is unset then waits for 10 us to check if the timer was reprogrammed
        }
        else{
            //bhmMessage("I", "TIMER", "Timer set to turn interruption on in %lf us - current time: %lf",timer_us,bhmGetCurrentTime());
            bhmWaitDelay(timer_us); // Every time_us 
            //bhmMessage("I", "TIMER", "Timer turning interruption up! at %lf",bhmGetCurrentTime());
            control_TIMER = NI_STATUS_INTER;
            interruptionOn();
        }
    }

    bhmWaitEvent(bhmGetSystemEvent(BHM_SE_END_OF_SIMULATION));
    destructor();
    return 0;
}

