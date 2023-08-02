#include "thermal.h"
#include "chronos.h"

void printExecutedInstructions(){
    prints("--------------------------\n");
    printsv("Loads:\t\t",           HW_get_32bit_reg(LOADS_COUNT));
    printsv("Stores:\t\t",          HW_get_32bit_reg(STORES_COUNT));
    printsv("Branch:\t\t",          HW_get_32bit_reg(BRANCH_COUNT));
    printsv("Register:\t\t",        HW_get_32bit_reg(REG_COUNT));
    printsv("Immediate:\t\t",       HW_get_32bit_reg(IMM_COUNT));
    printsv("Jump:\t\t",            HW_get_32bit_reg(JUMP_COUNT));
    printsv("ProgramCounter:\t\t",  HW_get_32bit_reg(PC_COUNT));
    printsv("Others:\t\t",          HW_get_32bit_reg(OTHERS_COUNT));
    prints("--------------------------\n");
}

void resetExecutedInstructions(){
    HW_set_32bit_reg(LOADS_COUNT, 0);
    HW_set_32bit_reg(STORES_COUNT, 0);
    HW_set_32bit_reg(OTHERS_COUNT, 0);
    HW_set_32bit_reg(BRANCH_COUNT, 0);
    HW_set_32bit_reg(REG_COUNT, 0);
    HW_set_32bit_reg(IMM_COUNT, 0);
    HW_set_32bit_reg(JUMP_COUNT, 0);
    HW_set_32bit_reg(PC_COUNT, 0);
    return;
}

///////////////////////////////////////////////////////////////////
/* Get the number of ports in the router based in his address*/
unsigned int getNumberOfPorts(unsigned int address) {
    unsigned int x, y;
    x = getXpos(address);
    y = getYpos(address);

    if ((x == 0 || x == (DIM_X - 1)) && (y == 0 || y == (DIM_Y - 1))) {
        return 3;
    } else {
        if (x == 0 || x == (DIM_X - 1) || y == 0 || y == (DIM_Y - 1)) {
            return 4;
        } else {
            return 5;
        }
    }
}

///////////////////////////////////////////////////////////////////
/* Activity estimation based in the amount of flits and packets crossing the router */
unsigned int estimateNoCActivity() {
    unsigned int totalPackets, totalFlits;
    // calculate the difference between the last interruption and the current one
    totalPackets =  HW_get_32bit_reg(EAST_PACKETS) - eastPackets_last;
    totalPackets += HW_get_32bit_reg(WEST_PACKETS) - westPackets_last;
    totalPackets += HW_get_32bit_reg(SOUTH_PACKETS) - southPackets_last;
    totalPackets += HW_get_32bit_reg(LOCAL_PACKETS) - localPackets_last;
    totalPackets += HW_get_32bit_reg(NORTH_PACKETS) - northPackets_last;
    //totalPackets = eastPackets_dif + westPackets_dif + southPackets_dif + localPackets_dif + northPackets_dif;

    eastPackets_last  = HW_get_32bit_reg(EAST_PACKETS);
    westPackets_last  = HW_get_32bit_reg(WEST_PACKETS);
    southPackets_last = HW_get_32bit_reg(SOUTH_PACKETS);
    localPackets_last = HW_get_32bit_reg(LOCAL_PACKETS);
    northPackets_last = HW_get_32bit_reg(NORTH_PACKETS);

    // alculate the difference between the last interruption and the current one
    totalFlits =  HW_get_32bit_reg(EAST_FLITS) - eastFlits_last;
    totalFlits += HW_get_32bit_reg(WEST_FLITS) - westFlits_last;
    totalFlits += HW_get_32bit_reg(SOUTH_FLITS) - southFlits_last;
    memFlits = HW_get_32bit_reg(LOCAL_FLITS) - localFlits_last;
    totalFlits += memFlits;
    //totalFlits += HW_get_32bit_reg(LOCAL_FLITS) - localFlits_last;
    totalFlits += HW_get_32bit_reg(NORTH_FLITS) - northFlits_last;
    //totalFlits = eastFlits_dif + westFlits_dif + southFlits_dif + localFlits_dif + northFlits_dif;

    eastFlits_last  = HW_get_32bit_reg(EAST_FLITS);
    westFlits_last  = HW_get_32bit_reg(WEST_FLITS);
    southFlits_last = HW_get_32bit_reg(SOUTH_FLITS);
    localFlits_last = HW_get_32bit_reg(LOCAL_FLITS);
    northFlits_last = HW_get_32bit_reg(NORTH_FLITS);

    return ((totalPackets * 5) + totalFlits);
}

void powerEstimation(){
    //unsigned int actualTime, deltaTime;
    unsigned int Voltage = 2;
    unsigned int loads, stores, branch, reg, imm, jump, pc, others, total;
    unsigned int dynamicEnergy_PE, dynamicEnergy_MEM, dynamicEnergy_Router;
    unsigned int leakEnergy_PE;
    unsigned int totalEnergy, nPorts, nocActivity, energyActive, energyIdle, nocIdle, idleNoCEnergy, activeNoCEnergy;
    if(!thermalPacket_pending){
        thermalPacket_pending = TRUE;

        // gets the number of ports
        nPorts = getNumberOfPorts(API_getProcessorAddr());
        // estimate the noc activity
        nocActivity = estimateNoCActivity();
        if(nocActivity < 1000000)
            nocIdle = 1000000 - nocActivity;
        else
            nocIdle = 0;

        // calculates the energy
        energyActive = ((nPorts * powerAvgBufferActive[Voltage]) + powerSwitchControlActive[Voltage]);
        energyIdle   = ((nPorts * powerAvgBufferIdle[Voltage]) + powerSwitchControlIdle[Voltage]);
        // multiply by the period to get the power consumption
        idleNoCEnergy = nocIdle * energyIdle; // * DC_DC_CONVERTER_ENERGY_OVERHEAD / 10;
        activeNoCEnergy = nocActivity * energyActive + (memFlits * (readEnergyMemory[Voltage] + writeEnergyMemory[Voltage]));  //* DC_DC_CONVERTER_ENERGY_OVERHEAD / 10;
        // calculates the ROUTER dynamic energy
        dynamicEnergy_Router = (idleNoCEnergy + activeNoCEnergy)  >> 6;
        printsv("dynamicEnergy_Router >> 6: ", dynamicEnergy_Router);

        // reads the number of each type of instruction executed in the last window
        loads =     HW_get_32bit_reg(LOADS_COUNT);
        stores =    HW_get_32bit_reg(STORES_COUNT);
        others =    HW_get_32bit_reg(OTHERS_COUNT);
        branch =    HW_get_32bit_reg(BRANCH_COUNT);
        reg =       HW_get_32bit_reg(REG_COUNT);
        imm =       HW_get_32bit_reg(IMM_COUNT);
        jump =      HW_get_32bit_reg(JUMP_COUNT);
        pc =        HW_get_32bit_reg(PC_COUNT);
        resetExecutedInstructions();

        // number of instructions executed in the last window
        total = loads + stores + branch + reg + imm + jump + pc + others;
        vTaskEnterCritical();
        printsvsv("inst~~~> ", total, "tick ", xTaskGetTickCount());
        vTaskExitCritical();
        
        // calculates the PE dynamic energy
        dynamicEnergy_PE = (arithDyn[Voltage] * others) + \
                           (branchDyn[Voltage] * branch) + \
                           (regDyn[Voltage] * reg) + \
                           (immDyn[Voltage] * imm) + \
                           (loadDyn[Voltage] * loads) + \
                           (storeDyn[Voltage] * stores) + \
                           (jumpDyn[Voltage] * jump) + \
                           (pcDyn[Voltage] * pc);
        //printsv("dynamicEnergy_PE: ", dynamicEnergy_PE);
        dynamicEnergy_PE = dynamicEnergy_PE >> 6;
        printsv("dynamicEnergy_PE >> 6: ", dynamicEnergy_PE);

        // calculates the PE leakage energy
        //leakEnergy_PE = (unsigned int)((PE_LEAKAGE * 1000000) * 0.001) >> 6; // mW => pW * s => pJ
        //leakEnergy_PE = (unsigned int)((PE_LEAKAGE*100 * 1000000000) * 0.001) >> 6; // mW => pW * s => pJ
        leakEnergy_PE = 1687500; // ((PE_LEAKAGE * 1000000000) * 0.001) >> 6 // mW => pW * s = pJ
        printsv("leakEnergy_PE >> 6: ", leakEnergy_PE);

        // calculates the MEM dynamic energy
        dynamicEnergy_MEM = (readEnergyMemory[Voltage] * loads) + (writeEnergyMemory[Voltage] * stores);
        dynamicEnergy_MEM = dynamicEnergy_MEM >> 6;
        printsv("dynamicEnergy_MEM >> 6: ", dynamicEnergy_MEM);

        // the amount of energy spent by this tile in the last window
        totalEnergy = (dynamicEnergy_MEM + dynamicEnergy_PE + dynamicEnergy_Router) + (leakEnergy_PE);
        printsv("TotalEnergy: ", totalEnergy);

        // if(API_getProcessorAddr() != makeAddress(5,5))
        //     totalEnergy = 15760373;

        // ./RUN_FreeRTOS.sh -n test_power0 -x 11 -y 11 -t 9999999999 -s c_pipeline_beta -m characterize -g no

        // Fill the ThermalPacket slot...
        ThermalPacket.header.header           = makeAddress(0, 0) | PERIPH_WEST;
        ThermalPacket.header.payload_size     = PKT_SERVICE_SIZE;
        ThermalPacket.header.service          = ENERGY_PACKET;
        ThermalPacket.header.source_addr      = API_getProcessorAddr();
        ThermalPacket.header.spent_energy     = totalEnergy;
        API_PushSendQueue(THERMAL, 0);
        prints("EnergyPckt sent\n");
    } else {
        prints("EnergyPckt was not generated!\n");
    }
    // change the PE frequency to the selected one
    API_applyFreqScale();
    return;
}
