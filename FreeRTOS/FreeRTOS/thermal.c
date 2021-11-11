#include "thermal.h"


void printExecutedInstructions(){
    vPortEnterCritical();
    prints("--------------------------\n");
    printsv("Branch:\t\t",      HW_get_32bit_reg(BRANCH_INST));
    printsv("Arithmetic:\t",    HW_get_32bit_reg(ARITH_INST));
    printsv("Jump:\t\t",        HW_get_32bit_reg(JUMP_INST));
    printsv("Move:\t\t",        HW_get_32bit_reg(MOVE_INST));
    printsv("Load:\t\t",        HW_get_32bit_reg(LOAD_INST));
    printsv("Store:\t\t",       HW_get_32bit_reg(STORE_INST));
    printsv("Shift:\t\t",       HW_get_32bit_reg(SHIFT_INST));
    printsv("Nop:\t\t",         HW_get_32bit_reg(NOP_INST));
    printsv("Logical:\t",       HW_get_32bit_reg(LOGICAL_INST));
    printsv("MultDiv:\t",       HW_get_32bit_reg(MULT_DIV_INST));
    printsv("Weird:\t\t",       HW_get_32bit_reg(WEIRD_INST));
    prints("--------------------------\n");
    vPortExitCritical();
    return;
}

void resetExecutedInstructions(){
    HW_set_32bit_reg(BRANCH_INST, 0);
    HW_set_32bit_reg(ARITH_INST, 0);
    HW_set_32bit_reg(JUMP_INST, 0);
    HW_set_32bit_reg(MOVE_INST, 0);
    HW_set_32bit_reg(LOAD_INST, 0);
    HW_set_32bit_reg(STORE_INST, 0);
    HW_set_32bit_reg(SHIFT_INST, 0);
    HW_set_32bit_reg(NOP_INST, 0);
    HW_set_32bit_reg(LOGICAL_INST, 0);
    HW_set_32bit_reg(MULT_DIV_INST, 0);
    HW_set_32bit_reg(WEIRD_INST, 0);
    return;
}
