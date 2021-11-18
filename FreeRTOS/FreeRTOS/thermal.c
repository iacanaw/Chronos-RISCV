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
