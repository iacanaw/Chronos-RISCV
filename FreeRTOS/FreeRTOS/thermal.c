#include "thermal.h"

void printExecutedInstructions(){
    prints("--------------------------\n");
    printsv("Loads:\t\t",       HW_get_32bit_reg(LOADS_COUNT));
    printsv("Stores:\t\t",      HW_get_32bit_reg(STORES_COUNT));
    printsv("Others:\t\t",      HW_get_32bit_reg(OTHERS_COUNT));
    prints("--------------------------\n");
}

void resetExecutedInstructions(){
    HW_set_32bit_reg(LOADS_COUNT, 0);
    HW_set_32bit_reg(STORES_COUNT, 0);
    HW_set_32bit_reg(OTHERS_COUNT, 0);
    return;
}

void powerEstimation(){
    unsigned int mySlot;
    //unsigned int actualTime, deltaTime;
    unsigned int Voltage = 3;
    unsigned int loads, stores, others, total;
    unsigned int dynamicEnergy_PE, dynamicEnergy_MEM, dynamicEnergy_Router;
    unsigned int totalEnergy;
    
    if(!thermalPacket_pending){
        thermalPacket_pending = TRUE;
        // actualTime = xTaskGetTickCount();
        // printsv("time: ", actualTime);

        //                              period in s                      in ns    * 100                     
        //unsigned int period_ns = (unsigned int)((float)((1/(1000000*API_getFreqScale())) * 1000000000) * 100); // period in (ns * 100) ---- 1 ns = 100
        
        // change the PE frequency to the selected one
        API_applyFreqScale();

        // reads the number of each type of instruction executed in the last window
        loads = HW_get_32bit_reg(LOADS_COUNT);
        stores = HW_get_32bit_reg(STORES_COUNT);
        others = HW_get_32bit_reg(OTHERS_COUNT);
        resetExecutedInstructions();

        // number of instructions executed in the last window
        total = loads + stores + others; 
        printsv("total:", total);

        // calculates the PE dynamic energy
        dynamicEnergy_PE = ((arithDyn[Voltage] * others) >> 6) + ((loadStoreDyn[Voltage] * (loads + stores)) >> 6);

        // calculates the MEM dynamic energy
        dynamicEnergy_MEM = ((readEnergyMemory[Voltage] * loads) >> 6) + ((writeEnergyMemory[Voltage] * stores) >> 6);

        // calculates the ROUTER dynamic energy
        dynamicEnergy_Router = 0; // TODO

        // the amount of energy spent by this tile in the last window
        totalEnergy = dynamicEnergy_MEM + dynamicEnergy_PE + dynamicEnergy_Router;
        printsv("TotalEnergy: ", totalEnergy);

        
        // Get a service slot...
        do{
            mySlot = API_GetServiceSlot();
            if(mySlot == PIPE_FULL){
                // Runs the NI Handler to send/receive packets, opening space in the PIPE
                API_NI_Handler();
            }
        }while(mySlot == PIPE_FULL);

        ServicePipe[mySlot].holder = PIPE_SYS_HOLDER;

        ServicePipe[mySlot].header.header           = makeAddress(0, 0) | PERIPH_WEST;
        ServicePipe[mySlot].header.payload_size     = PKT_SERVICE_SIZE;
        ServicePipe[mySlot].header.service          = ENERGY_PACKET;
        ServicePipe[mySlot].header.source_addr      = ProcessorAddr;
        ServicePipe[mySlot].header.spent_energy     = totalEnergy;
        API_PushSendQueue(SERVICE, mySlot);
    }
    return;    
}