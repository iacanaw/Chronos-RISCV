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
    //unsigned int actualTime, deltaTime;
    unsigned int Voltage = 2;
    unsigned int loads, stores, others, total;
    unsigned int dynamicEnergy_PE, dynamicEnergy_MEM, dynamicEnergy_Router;
    unsigned int totalEnergy;
    
    if(!thermalPacket_pending){
        thermalPacket_pending = TRUE;

        // change the PE frequency to the selected one
        API_applyFreqScale();

        // reads the number of each type of instruction executed in the last window
        loads = HW_get_32bit_reg(LOADS_COUNT);
        stores = HW_get_32bit_reg(STORES_COUNT);
        others = HW_get_32bit_reg(OTHERS_COUNT);
        resetExecutedInstructions();

        // number of instructions executed in the last window
        total = loads + stores + others; 
        printsv("total instructions~~~> ", total);

        // calculates the PE dynamic energy
        dynamicEnergy_PE = ((arithDyn[Voltage] * others)) + ((loadStoreDyn[Voltage] * (loads + stores)));
        //printsv("dynamicEnergy_PE: ", dynamicEnergy_PE);
        dynamicEnergy_PE = dynamicEnergy_PE >> 6;
        printsv("dynamicEnergy_PE >> 6: ", dynamicEnergy_PE);

        // calculates the PE leakage energy
        //leakEnergy_PE = (PE_LEAKAGE * 1000000) >> 6; // mW => pW * s => pJlsls

        // calculates the MEM dynamic energy
        dynamicEnergy_MEM = 0;//((readEnergyMemory[Voltage] * loads) >> 6) + ((writeEnergyMemory[Voltage] * stores) >> 6);
        printsv("dynamicEnergy_MEM >> 6: ", dynamicEnergy_MEM);

        // calculates the ROUTER dynamic energy
        dynamicEnergy_Router = 0; // TODO

        // the amount of energy spent by this tile in the last window
        totalEnergy = (dynamicEnergy_MEM + dynamicEnergy_PE + dynamicEnergy_Router);
        printsv("TotalEnergy: ", totalEnergy);

        // Fill the ThermalPacket slot...
        ThermalPacket.header.header           = makeAddress(0, 0) | PERIPH_WEST;
        ThermalPacket.header.payload_size     = PKT_SERVICE_SIZE;
        ThermalPacket.header.service          = ENERGY_PACKET;
        ThermalPacket.header.source_addr      = ProcessorAddr;
        ThermalPacket.header.spent_energy     = totalEnergy;
        API_PushSendQueue(THERMAL, 0);
    }
    return;
}