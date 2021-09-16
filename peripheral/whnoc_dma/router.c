
////////////////////////////////////////////////////////////////////////////////
//
//                W R I T T E N   B Y   I M P E R A S   I G E N
//
//                             Version 20191106.0
//
////////////////////////////////////////////////////////////////////////////////

#include "router.igen.h"
#include "noc.h"
#include <stdlib.h>
#include<stdio.h>


#define __bswap_constant_32(x) \
     ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) |		      \
      (((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24))

unsigned int htonl(unsigned int x){
    return x; //__bswap_constant_32(x);
}

////////////////////////////////////////////////////////////////////////////////

#define __cswap_constant_32(x) \
     ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) |		      \
      (((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24))

unsigned int ntohl(unsigned int x){
    return x; //__cswap_constant_32(x);
}

// Local Address
unsigned int myAddress = 0xFFFFFFFF;
// local ID Router
int myID = 0xFFFFFFFF;
// Number of packets delivered to the Local port
unsigned int localDeliveredPckts = 0;

// Activity
unsigned int activity;

// Countdown value per Packet, informing how many flits are left to be transmitted 
unsigned int flitCountOut[N_PORTS] = {HEADER,HEADER,HEADER,HEADER,HEADER};
unsigned int flitCountIn = HEADER;

// Define a struct that stores each flit and the time that it arrive in the router
typedef struct{
    unsigned int data;
    unsigned long long int inTime;
}flit;

// One buffer for each port
flit buffers[N_PORTS][BUFFER_SIZE];
flit incomingFlit;

// Buffer read/write control pointers
unsigned int last[N_PORTS] = {0,0,0,0,0};
unsigned int first[N_PORTS] = {0,0,0,0,0};
unsigned int localStatus = GO;
unsigned int localBufferAmount = 0;

// Stores the control status of each port
unsigned int control[N_PORTS] = {GO,GO,GO,GO,GO};
unsigned int txCtrl;

// Routing Table
unsigned int routingTable[N_PORTS] = {ND,ND,ND,ND,ND};

#if ARBITER_RR
// Priority list 
unsigned int priority[N_PORTS] = {0,1,2,3,4};
#endif

#if ARBITER_HERMES
// The Arbiter implemented in Hermes NoC
unsigned int nextPort = ND;
unsigned int actualPort = LOCAL;
#endif

#if ARBITER_TTL
// Time to Live Arbitration
unsigned int contPriority[N_PORTS] = {0,0,0,0,1};
#endif

// Stores the atual router tick status
unsigned short int myIterationStatus = ITERATION_OFF;

// Gets the actual "time" of the system
unsigned long long int currentTime; // %llu

// Stores the "time" when a packet is transmitted to the network
unsigned long long int enteringTime;

// Maybe LEGACY!
unsigned int lastTickLocal = 0;

// BufferPop aux signals
unsigned int localBuffer_packetDest;

#if LOG_OUTPUTFLITS
// flit counter of each port, is reseted in each quantum in router.igen.c
int contFlits[N_PORTS];
unsigned int countTotalPackets[N_PORTS] = {0,0,0,0,0};
unsigned int countTotalFlits[N_PORTS] = {0,0,0,0,0};
#endif

////////////////////////////////////////////////////////////
/////////////////////// FUNCTIONS //////////////////////////
////////////////////////////////////////////////////////////

unsigned int xy2addr(unsigned int x, unsigned int y){
    return (y | (x << 8));
}

// Informs the ticker that this router needs ticks
void turn_TickOn(){
    unsigned short int iterAux = ITERATION_ON;
    myIterationStatus = ITERATION_ON;
    ppmPacketnetWrite(handles.iterationsPort, &iterAux, sizeof(iterAux));
}

// Informs the ticker that this router does not needs ticks
void turn_TickOff(){
    unsigned short int iterAux = ITERATION_OFF;
    myIterationStatus = ITERATION_OFF;
    ppmPacketnetWrite(handles.iterationsPort, &iterAux, sizeof(iterAux));
}

// Inform the iterator that the PE is waiting a packet 
void informIterator(){
    unsigned short int iterAux = ITERATION;
    ppmPacketnetWrite(handles.iterationsPort, &iterAux, sizeof(iterAux));
}

void iterateNI(){
    unsigned long long int iterate = 0xFFFFFFFFFFFFFFFFULL;
    ppmPacketnetWrite(handles.portControlLocal, &iterate, sizeof(iterate));
}

void iteratePeriph(){
    unsigned long long int iterate = 0xFFFFFFFFFFFFFFFFULL;
    // TODO: Descobrir quais são as portas periféricas
    ppmPacketnetWrite(handles.portControlWest, &iterate, sizeof(iterate));
    ppmPacketnetWrite(handles.portControlSouth, &iterate, sizeof(iterate));
}

// Extract the Y position for a given address
unsigned int positionY(unsigned int address){
    unsigned int mask =  0xFF;
    unsigned int masked_n = address & mask;
    return masked_n;
}

// Extract the X position for a given address
unsigned int positionX(unsigned int address){
    unsigned int mask =  0xFF00;
    unsigned int masked_n = address & mask;
    unsigned int thebyte = masked_n >> 8;
    return thebyte;
}

unsigned int peripheralPort(unsigned int address){
    unsigned int mask = 0x000F0000;
    unsigned int masked_n = address & mask;
    return masked_n;
}

// Variables to handle memory write and read
char chValue[4];
unsigned int usValue;

void vec2usi(){
    unsigned int auxFlit = 0x00000000;
    unsigned int aux;
    aux = 0x000000FF & chValue[3];
    auxFlit = ((aux << 24) & 0xFF000000);

    aux = 0x000000FF & chValue[2];
    auxFlit = auxFlit | ((aux << 16) & 0x00FF0000);

    aux = 0x000000FF & chValue[1];
    auxFlit = auxFlit | ((aux << 8) & 0x0000FF00);

    aux = 0x000000FF & chValue[0];
    auxFlit = auxFlit | ((aux) & 0x000000FF);

    usValue = auxFlit;
    return;
}

void usi2vec(){
    chValue[3] = (usValue >> 24) & 0x000000FF;
    chValue[2] = (usValue >> 16) & 0x000000FF;
    chValue[1] = (usValue >> 8) & 0x000000FF;
    chValue[0] = usValue & 0x000000FF;
}

// Function to write something in the memory
void writeMemm(unsigned int value, unsigned int addr){
    return;
    /*usValue = value;
    usi2vec();
    ppmAddressSpaceHandle h = ppmOpenAddressSpace("RWRITE");
    if(!h) {
        bhmMessage("I", "ROUTER_MEM_WRITE", "ERROR_WRITE h handling!");
        while(1){} // error handling
    }
    ppmWriteAddressSpace(h, addr, sizeof(chValue), chValue);
    ppmCloseAddressSpace(h);*/
}

// Calculates the output port for a given local address and a destination address
// using a XY routing algorithm
unsigned int XYrouting(unsigned int current, unsigned int dest){
    unsigned int destination = ntohl(dest);
    //bhmMessage("INFO", "XY", "dest: %d --- %d", dest, destination);
    if(positionX(current) == positionX(destination) && positionY(current) == positionY(destination)){
        if(peripheralPort(destination) == PERIPH_LOCAL)
            return LOCAL;
        else if(peripheralPort(destination) == PERIPH_EAST)
            return EAST;
        else if(peripheralPort(destination) == PERIPH_WEST)
            return WEST;
        else if(peripheralPort(destination) == PERIPH_NORTH)
            return NORTH;
        else if(peripheralPort(destination) == PERIPH_SOUTH)
            return SOUTH;
        else
            bhmMessage("I", "XYRouting", "Something is not quite right in the peripheral! ERROR!!!\n");
            return ND; // ERROR
    }
    else if(positionX(current) < positionX(destination)){
        return EAST;
    }
    else if(positionX(current) > positionX(destination)){
        return WEST;
    }
    else if(positionX(current) == positionX(destination) && positionY(current) < positionY(destination)){
        return NORTH;
    }
    else if(positionX(current) == positionX(destination) && positionY(current) > positionY(destination)){
        return SOUTH;
    }
    else{
        bhmMessage("I", "XYRouting", "Something is not quite right! ERROR!!!\n");
        return ND; // ERROR
    }
}

// Updates the buffer status
void bufferStatusUpdate(unsigned int port){
    unsigned int status = 0;
    //bhmMessage("INFO", "STATUSUPDT", "Enviando para packetnet port %d\n", port);
    // -- No available space in this buffer!
    if ((first[port] == 0 && last[port] == (BUFFER_SIZE-1)) || (first[port] == (last[port]+1))){
        status = STALL;
    }
    // -- There is space in this buffer!
    else{
        status = GO;
    }

    // Transmitt the new buffer status to the neighbor
    if (port == LOCAL){
        ppmPacketnetWrite(handles.portControlLocal, &status, sizeof(status));
    }
    else if (port == EAST){
        ppmPacketnetWrite(handles.portControlEast, &status, sizeof(status));
    }
    else if (port == WEST){
        ppmPacketnetWrite(handles.portControlWest, &status, sizeof(status));
    }
    else if (port == NORTH){
        ppmPacketnetWrite(handles.portControlNorth, &status, sizeof(status));
    }
    else if (port == SOUTH){
        ppmPacketnetWrite(handles.portControlSouth, &status, sizeof(status));
    }

}

void bufferPush(unsigned int port){
    // Write a new flit in the buffer
    buffers[port][last[port]] = incomingFlit;
    if(last[port] < BUFFER_SIZE-1){
        last[port]++;
    }
    else if(last[port] == BUFFER_SIZE-1){
        last[port] = 0;
    }

    if(port == LOCAL) localBufferAmount++;

    // Inform the ticker that this router has something to send
    //if(myIterationStatus == ITERATION_OFF) turn_TickOn(); 

    // Update the buffer status
    //bhmMessage("INFO", "PUSH", "Foi push! flit:%x\n", htonl(incomingFlit.data));
    bufferStatusUpdate(port);
}

unsigned int isEmpty(unsigned int port){
    if(last[port] != first[port]){
        return 0; // no, it is not empty
    }
    else{
        return 1; // yes, it is empty
    }
}

unsigned int bufferPop(unsigned int port){
    unsigned long long int value;
    unsigned int difX, difY;
    
    // Read the first flit from the buffer
    value = buffers[port][first[port]].data;
    // Increments the "first" pointer
    if(first[port] < BUFFER_SIZE-1){
        first[port]++;
    }
    else if(first[port] == BUFFER_SIZE-1){
        first[port] = 0;
    }

    if(port == LOCAL) localBufferAmount--;
    
    // Decreases the flitCountOut
    flitCountOut[port] = flitCountOut[port] - 1;
    
    // If the flitCountOut goes to EMPTY then the transmission is done!
    if (flitCountOut[port] == EMPTY){

        //Log info about the end of transmittion of a packet
        if (routingTable[port] == LOCAL){
            //value = ntohl(currentTime); // Register the out-iteration in the last flit COMENTADO
            localDeliveredPckts++;
            // bhmMessage("I", "Router", "Packet delivered at %d-(%d,%d) - total delivered: %d\n",myID, positionX(myAddress), positionY(myAddress), localDeliveredPckts);
        }

        // Updates the routing table, releasing the output port
        // bhmMessage("INFO", "ROUTER", ">>>>> %x - Porta de saída %d ficou livre\n", myAddress, port);
        routingTable[port] = ND;

        // Inform that the next flit will be a header
        flitCountOut[port] = HEADER;

        // If every buffer is empty this router does not need to be ticked
        /*if(myIterationStatus == ITERATION_ON && isEmpty(EAST) && isEmpty(WEST) && isEmpty(NORTH) && isEmpty(SOUTH) && isEmpty(LOCAL)){// && preBuffer_isEmpty()){
            turn_TickOff();
        }*/
        
#if ARBITER_RR
        // Reset it's priority
        priority[port] = 0;
#endif

/*#if ARBITER_TTL //////// ACHO QUE DA PRA TIRAR ISSO. JÁ PARECE ESTAR GARANTIDO PELA FUNCAO searchAndAllocate
        // Reset it's priority
        contPriority[port] = 0;
#endif*/    
    }
    else if(port == LOCAL && flitCountOut[port] == HEADER-1){
        localBuffer_packetDest = htonl(value);
        enteringTime = currentTime;
    }
    // If it is the packet size flit then we store the value for the countdown
    else if (flitCountOut[port] == SIZE){
        flitCountOut[port] = htonl(value);
    }
    else if(port == LOCAL && flitCountOut[port] == HOPS){
        // Calculate the number of hops to achiev the destination address
        // Calculate the X dif
        if(positionX(myAddress)>positionX(localBuffer_packetDest)) difX = positionX(myAddress) - positionX(localBuffer_packetDest);
        else difX = positionX(localBuffer_packetDest) - positionX(myAddress);
        // Calculate the Y dif
        if(positionY(myAddress)>positionY(localBuffer_packetDest)) difY = positionY(myAddress) - positionY(localBuffer_packetDest);
        else difY = positionY(localBuffer_packetDest) - positionY(myAddress);
        // Adds both difs to determine the amount of hops
        //value = ntohl(difX + difY); COMENTADO
    }
    //else if(port == LOCAL && flitCountOut[port] == IN_TIME){COMENTADO
        //value = ntohl(enteringTime);COMENTADO    }

    // Update the buffer status
    //bhmMessage("INFO", "POP", "Foi pop!\n");
    bufferStatusUpdate(port);
    
    return value;
}

/*void verifyLocalBuffer(){
    // The local buffer has a special behavior - to create ordenation in the packets transmittion
    unsigned int access;
    if(myIterationStatusLocal == ITERATION_OFF_LOCAL && localBufferAmount >= 3){ // using "greater or equal to 3" because the sendtime is registered in the 3th flit
        informIteratorLocalOn();
        switch (first[LOCAL]+4){ // calculates the sendtime position inside the buffer
            case (BUFFER_SIZE):
                access = 0;
                break;
            case (BUFFER_SIZE+1):
                access = 1;
                break;
            case (BUFFER_SIZE+2):
                access = 2;
                break;
            case (BUFFER_SIZE+3):
                access = 3;
                break;
            default:
                access = first[LOCAL]+4;
        }
        ppmPacketnetWrite(handles.iterationsPort, &buffers[LOCAL][access].data, sizeof(buffers[LOCAL][access].data)); // inform the sendtime to the iterator
    }
}*/

#if ARBITER_RR
// Select the port with the biggest priority
unsigned int selectPort(){
    unsigned int selected = ND; // Starts selecting none;
    unsigned int selPrio = 0; 
    int k;
    for(k = 0; k <= LOCAL; k++){
        // Increases the priority every time that this function runs
        priority[k] = priority[k] + 1;
        // If the port has a request then...
        if(!isEmpty(k) && routingTable[k]==ND){
            if(priority[k] > selPrio){
                selected = k;
                selPrio = priority[k];
            }
        }
    }
    return selected;
}

// Allocates the output port to the given selPort if it is available
void allocate(unsigned int port){
    unsigned int header, to, checkport, allowed;
    // In the first place, verify if the port is not connected to any thing and has something to transmitt 
    if(port != ND && routingTable[port] == ND && !isEmpty(port)){
        // Discover to wich port the packet must go
        header = buffers[port][first[port]].data;
        //bhmMessage("INFO", "ALLOCATE", "Pedindo roteamento para o header %x", htonl(header));
        to = XYrouting(myAddress, header);
        // Verify if any other port is using the selected one
        allowed = 1;
        for(checkport = 0; checkport <= LOCAL; checkport++){
            if (routingTable[checkport] == to){
                allowed = 0;
                // If the port can't get routed, then turn it's priority down
                if(priority[port]>5) priority[port] = priority[port] - 5;
            }
        }
        //If the requested output port is free
        if(allowed == 1){
            // Connect the buffer to the output
            // bhmMessage("INFO", "ROUTER", ">>>>> %x - Porta %d saindo pela porta %d\n", myAddress, port, to);
            routingTable[port] = to;
            // and compute a packet transmittion
            countTotalPackets[to] = countTotalPackets[to]+1;
            if(to==LOCAL)       writeMemm(htonl(countTotalPackets[to]), LOCAL_PACKETS_ADDR);
            else if(to==EAST)   writeMemm(htonl(countTotalPackets[to]), EAST_PACKETS_ADDR);
            else if(to==WEST)   writeMemm(htonl(countTotalPackets[to]), WEST_PACKETS_ADDR);
            else if(to==NORTH)  writeMemm(htonl(countTotalPackets[to]), NORTH_PACKETS_ADDR);
            else if(to==SOUTH)  writeMemm(htonl(countTotalPackets[to]), SOUTH_PACKETS_ADDR);
            // Once one port is attended, then reset it's priority.
            priority[port] = 1;
        }
    }
}
#endif //ARBITER_RR


#if ARBITER_TTL
// Checks if a given output port is available
int portIsAvailable(int port){ 
    int checkport;
    for(checkport=0; checkport<N_PORTS; checkport++){
        if (routingTable[checkport] == port){
            return 0; // The port is unvailable
        }
    }
    return 1; // The output port is available
}

// Select the longest waiting port which has a output port available 
unsigned int selLongestWaitingPortAvailable(){
    int max = 0;
    int port = 0;
    int selPort = ND;
    //Checks for each port if the port is waiting more than max and is not routed and the output port is available
    for(port=0;port<N_PORTS;port++){
        if((contPriority[port]>max) && (routingTable[port]==ND) && (portIsAvailable(XYrouting(myAddress,buffers[port][first[port]].data)))){      
            max = contPriority[port];
            selPort = port;
        }
    }
    return selPort;
}

// Route the longest waiting buffer to a given output port
void searchAndAllocate(){
    int port = 0;
    int selectedPort = 0;

    // Increases the priority for ports that has something to send 
    for(port=0;port<N_PORTS;port++){
        if((!isEmpty(port)) && (routingTable[port] == ND)){
            contPriority[port] ++; 
        }
    }
    /*Returns the port with the bigger priority */
    selectedPort = selLongestWaitingPortAvailable();

    // If some port was selected
    if(selectedPort!=ND){
        // Updates the routingTable and reset the port priority
        routingTable[selectedPort] = XYrouting(myAddress,buffers[selectedPort][first[selectedPort]].data);
        contPriority[selectedPort] = 0;
    }
}
#endif // ARBITER_TTL

#if ARBITER_HERMES
// Select the port with the biggest priority
void selectPort(){
    switch (actualPort){
        case LOCAL:
            if(!isEmpty(EAST) && routingTable[EAST]==ND) nextPort = EAST;
            else if(!isEmpty(WEST) && routingTable[WEST]==ND) nextPort = WEST;
            else if(!isEmpty(NORTH) && routingTable[NORTH]==ND) nextPort = NORTH;
            else if(!isEmpty(SOUTH) && routingTable[SOUTH]==ND) nextPort = SOUTH;
            else nextPort = LOCAL;
        break;

        case EAST:
            if(!isEmpty(WEST) && routingTable[WEST]==ND) nextPort = WEST;
            else if(!isEmpty(NORTH) && routingTable[NORTH]==ND) nextPort = NORTH;
            else if(!isEmpty(SOUTH) && routingTable[SOUTH]==ND) nextPort = SOUTH;
            else if(!isEmpty(LOCAL) && routingTable[LOCAL]==ND) nextPort = LOCAL;
            else nextPort = EAST;
        break;

        case WEST:
            if(!isEmpty(NORTH) && routingTable[NORTH]==ND) nextPort = NORTH;
            else if(!isEmpty(SOUTH) && routingTable[SOUTH]==ND) nextPort = SOUTH;
            else if(!isEmpty(LOCAL) && routingTable[LOCAL]==ND) nextPort = LOCAL;
            else if(!isEmpty(EAST) && routingTable[EAST]==ND) nextPort = EAST;
            else nextPort = WEST;
        break;

        case NORTH:
            if(!isEmpty(SOUTH) && routingTable[SOUTH]==ND) nextPort = SOUTH;
            else if(!isEmpty(LOCAL) && routingTable[LOCAL]==ND) nextPort = LOCAL;
            else if(!isEmpty(EAST) && routingTable[EAST]==ND) nextPort = EAST;
            else if(!isEmpty(WEST) && routingTable[WEST]==ND) nextPort = WEST;
            else nextPort = NORTH;
        break;

        case SOUTH:
            if(!isEmpty(LOCAL) && routingTable[LOCAL]==ND) nextPort = LOCAL;
            else if(!isEmpty(EAST) && routingTable[EAST]==ND) nextPort = EAST;
            else if(!isEmpty(WEST) && routingTable[WEST]==ND) nextPort = WEST;
            else if(!isEmpty(NORTH) && routingTable[NORTH]==ND) nextPort = NORTH;
            else nextPort = SOUTH;
        break;

        default:
            nextPort = LOCAL;
    }
    actualPort = nextPort;
}

// Allocates the output port to the givel selPort if it is available
void allocate(){
    unsigned int header, to, checkport, allowed;
    // In the first place, verify if the port is not connected to any thing and has something to transmitt 
    if(actualPort != ND && routingTable[actualPort] == ND && !isEmpty(actualPort)){
        // Discover to wich port the packet must go
        header = buffers[actualPort][first[actualPort]].data;
        to = XYrouting(myAddress, header);
        // Verify if any other port is using the selected one
        allowed = 1;
        for(checkport = 0; checkport <= LOCAL; checkport++){
            if (routingTable[checkport] == to){
                allowed = 0;
            }
        }
        if(allowed == 1){
            routingTable[actualPort] = to;
        }
    }
}
#endif // ARBITER_HERMES

void transmitt(){
    unsigned int port, flit;
    // For each port...
    for(port = 0; port <= LOCAL; port++){
        // Verify if the local port is allowed to transmitt
        //if(port != LOCAL || (port == LOCAL && myIterationStatusLocal == ITERATION_RELEASED_LOCAL)){
            // Verify if the port is routed and if it has something to transmmit 
            if((routingTable[port] < ND) && (!isEmpty(port))){

                // Checks if at least one tick has passed since the flit arrived in this router
                if ((currentTime > buffers[port][first[port]].inTime)||((port == LOCAL) && (lastTickLocal != currentTime))) { // MAYBE lastTickLocal IS LEGACY!

                    // Transmit it to the LOCAL router
                    if(routingTable[port] == LOCAL){
                        /*  If the receiver router has space AND there is flits to send AND still connected to the output port*/
                        if(control[routingTable[port]] == GO && !isEmpty(port) && routingTable[port] == LOCAL){
                            // Gets a flit from the buffer 
                            flit = bufferPop(port);
                            //bhmMessage("I", "LOCALOUT", "Enviando flit: %x do buffer %d",htonl(flit), port);
                            #if LOG_OUTPUTFLITS
                            contFlits[LOCAL] = contFlits[LOCAL]+1;
                            #endif
                            countTotalFlits[LOCAL] = countTotalFlits[LOCAL]+1;
                            writeMemm(htonl(countTotalFlits[LOCAL]), LOCAL_FLITS_ADDR);
                            // Send the flit transmission time followed by the data
                            ppmPacketnetWrite(handles.portDataLocal, &flit, sizeof(flit));
                            activity++;
                        }
                    }

                    // Transmit it to the EAST router
                    else if(routingTable[port] == EAST){
                        /*  If the receiver router has space AND there is flits to send AND still connected to the output port*/
                        if(control[routingTable[port]] == GO && !isEmpty(port) && routingTable[port] == EAST){
                            // Gets a flit from the buffer 
                            flit = bufferPop(port);
                            #if LOG_OUTPUTFLITS
                            contFlits[EAST] = contFlits[EAST]+1;
                            #endif
                            countTotalFlits[EAST] = countTotalFlits[EAST]+1;
                            writeMemm(htonl(countTotalFlits[EAST]), EAST_FLITS_ADDR);
                            // Send the flit transmission time followed by the data
                            ppmPacketnetWrite(handles.portControlEast, &currentTime, sizeof(currentTime));
                            ppmPacketnetWrite(handles.portDataEast, &flit, sizeof(flit));
                            activity++;
                        }
                    }

                    // Transmit it to the WEST router
                    else if(routingTable[port] == WEST){
                        /*  If the receiver router has space AND there is flits to send AND still connected to the output port*/
                        if(control[routingTable[port]] == GO && !isEmpty(port) && routingTable[port] == WEST){
                            // Gets a flit from the buffer 
                            flit = bufferPop(port);
                            #if LOG_OUTPUTFLITS
                            contFlits[WEST] = contFlits[WEST]+1;
                            #endif
                            countTotalFlits[WEST] = countTotalFlits[WEST]+1;
                            writeMemm(htonl(countTotalFlits[WEST]), WEST_FLITS_ADDR);
                            // Send the flit transmission time followed by the data
                            ppmPacketnetWrite(handles.portControlWest, &currentTime, sizeof(currentTime));
                            ppmPacketnetWrite(handles.portDataWest, &flit, sizeof(flit));
                            activity++;
                        }
                    }

                    // Transmit it to the NORTH router
                    else if(routingTable[port] == NORTH){
                        /*  If the receiver router has space AND there is flits to send AND still connected to the output port*/
                        if(control[routingTable[port]] == GO && !isEmpty(port) && routingTable[port] == NORTH){
                            // Gets a flit from the buffer 
                            flit = bufferPop(port);
                            #if LOG_OUTPUTFLITS
                            contFlits[NORTH] = contFlits[NORTH]+1;
                            #endif
                            countTotalFlits[NORTH] = countTotalFlits[NORTH]+1;
                            writeMemm(htonl(countTotalFlits[NORTH]), NORTH_FLITS_ADDR);
                            // Send the flit transmission time followed by the data
                            ppmPacketnetWrite(handles.portControlNorth, &currentTime, sizeof(currentTime));
                            ppmPacketnetWrite(handles.portDataNorth, &flit, sizeof(flit));
                            activity++;
                        }
                    }

                    // Transmit it to the SOUTH router
                    else if(routingTable[port] == SOUTH){
                        /*  If the receiver router has space AND there is flits to send AND still connected to the output port*/
                        if(control[routingTable[port]] == GO && !isEmpty(port) && routingTable[port] == SOUTH){
                            // Gets a flit from the buffer 
                            flit = bufferPop(port);
                            #if LOG_OUTPUTFLITS
                            contFlits[SOUTH] = contFlits[SOUTH]+1;
                            #endif
                            countTotalFlits[SOUTH] = countTotalFlits[SOUTH]+1;
                            writeMemm(htonl(countTotalFlits[SOUTH]), SOUTH_FLITS_ADDR);
                            // Send the flit transmission time followed by the data
                            ppmPacketnetWrite(handles.portControlSouth, &currentTime, sizeof(currentTime));
                            ppmPacketnetWrite(handles.portDataSouth, &flit, sizeof(flit));
                            activity++;
                        }
                    }
                }
            }
        //}
    }
}

// Stores the control signal for a given port
void controlUpdate(unsigned int port, unsigned int ctrlData){
    if (myID >= 0 && myID < 0xFFFFFFFF)
        control[port] = ctrlData;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// Interaction Function //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void iterate(){
    activity = 0;
    // Sends the iteration signal to the NI module
    iterateNI();
    if(myAddress == 0x0000) iteratePeriph();
    // Verify if the LOCAL buffer has something to send
    //verifyLocalBuffer();
    ////////////////////////////////////////////
    // Arbitration Process - defined in noc.h //
    ////////////////////////////////////////////
    #if ARBITER_RR
    unsigned int selPort;
    // Defines which port will be attended by the allocator
    selPort = selectPort();
    // Allocates the output port to the givel selPort if it is available
    allocate(selPort);
    #endif
    ////////////////////////////////////////////
    #if ARBITER_TTL
    // Search and allocate the packet which is waiting more time
    searchAndAllocate();
    #endif
    ////////////////////////////////////////////
    #if ARBITER_HERMES
    selectPort();
    allocate();
    #endif
    ////////////////////////////////////////////
    ////////////////////////////////////////////
    ////////////////////////////////////////////
    // Runs the transmittion of one flit to each direction (if there is a connection stablished)
    transmitt();
    //
    ppmPacketnetWrite(handles.iterationsPort, &activity, sizeof(activity));
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// Callback stubs ////////////////////////////////

PPM_REG_READ_CB(addressRead) {
    localPort_regs_data.myAddress.value = htonl(myAddress);
    return *(Uns32*)user;
}

PPM_REG_WRITE_CB(addressWrite) {
    // By define - the address start with 0xF...F
    if(myAddress == 0xFFFFFFFF){
        myID = htonl((unsigned int)data);
        int y = myID/DIM_X;
        int x = myID-(DIM_X*y);
        myAddress = xy2addr(x, y);
        bhmMessage("INFO", "MY_ADRESS", "My Address: %d %d", x, y);
        bhmMessage("INFO","MYADRESS","MY ID = %d", myID);
    }
    else{ // Display an error message when another write operation is made in this register!!
        bhmMessage("INFO", "MY_ADRESS", "ERROR: The address can not be changed!");
    }
    *(Uns32*)user = data;
}

PPM_PACKETNET_CB(controlEast) {
    // When receving a control value... (4 bytes info)
    if(bytes == 4){
        unsigned int ctrl = *(unsigned int *)data;
        controlUpdate(EAST, ctrl);
    }
    // When receving a time for the incoming flit... (8 bytes info)
    else if(bytes == 8){
        incomingFlit.inTime = *(unsigned long long int *)data;
    }
}

PPM_PACKETNET_CB(controlLocal) {
    // When receving a control value... (4 bytes info)
    if(bytes == 4){
        unsigned int ctrl = *(unsigned int *)data;
        controlUpdate(LOCAL, ctrl);
    }
    // When receving a time for the incoming flit... (8 bytes info)
    else if(bytes == 8){
        //incomingFlit.inTime = *(unsigned long long int *)data;
        //informIterator();
    }
}

PPM_PACKETNET_CB(controlNorth) {
    // When receving a control value... (4 bytes info)
    if(bytes == 4){
        unsigned int ctrl = *(unsigned int *)data;
        controlUpdate(NORTH, ctrl);
    }
    // When receving a time for the incoming flit... (8 bytes info)
    else if(bytes == 8){
        incomingFlit.inTime = *(unsigned long long int *)data;
    }
}

PPM_PACKETNET_CB(controlSouth) {
    // When receving a control value... (4 bytes info)
    if(bytes == 4){
        unsigned int ctrl = *(unsigned int *)data;
        controlUpdate(SOUTH, ctrl);
    }
    // When receving a time for the incoming flit... (8 bytes info)
    else if(bytes == 8){
        incomingFlit.inTime = *(unsigned long long int *)data;
    }
}

PPM_PACKETNET_CB(controlWest) {
    // When receving a control value... (4 bytes info)
    if(bytes == 4){
        unsigned int ctrl = *(unsigned int *)data;
        controlUpdate(WEST, ctrl);
    }
    // When receving a time for the incoming flit... (8 bytes info)
    else if(bytes == 8){
        incomingFlit.inTime = *(unsigned long long int *)data;
    }
}

PPM_PACKETNET_CB(dataEast) {
    unsigned int newFlit = *(unsigned int *)data;
    incomingFlit.data = newFlit;
    bufferPush(EAST);
}

PPM_PACKETNET_CB(dataLocal) {
    unsigned int newFlit = *(unsigned int *)data;
    incomingFlit.data = newFlit;
    incomingFlit.inTime = currentTime;
    bufferPush(LOCAL);
}

PPM_PACKETNET_CB(dataNorth) {
    unsigned int newFlit = *(unsigned int *)data;
    incomingFlit.data = newFlit;
    bufferPush(NORTH);
}

PPM_PACKETNET_CB(dataSouth) {
    unsigned int newFlit = *(unsigned int *)data;
    incomingFlit.data = newFlit;
    bufferPush(SOUTH);
}

PPM_PACKETNET_CB(dataWest) {
    unsigned int newFlit = *(unsigned int *)data;
    incomingFlit.data = newFlit;
    bufferPush(WEST);
}

PPM_PACKETNET_CB(iterationPort) {
    // Stores the actual iteration time
    currentTime = *(unsigned long long int *)data;

    //Checks if it is a local iteration
    /*if((currentTime >> 31) == 1){
        myIterationStatusLocal = ITERATION_RELEASED_LOCAL;
        currentTime = (unsigned long long int )(0x7FFFFFFFULL & currentTime);
    }*/
    //Runs iterate
    iterate();
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

