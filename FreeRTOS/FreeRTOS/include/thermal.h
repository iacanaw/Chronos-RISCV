#ifndef __THERMAL_H__
#define __THERMAL_H__

#define BRANCH_INST    0x8FFFFFF4u
#define ARITH_INST     0x8FFFFFF0u
#define JUMP_INST      0x8FFFFFECu
#define MOVE_INST      0x8FFFFFE8u
#define LOAD_INST      0x8FFFFFE4u
#define STORE_INST     0x8FFFFFE0u
#define SHIFT_INST     0x8FFFFFDCu
#define NOP_INST       0x8FFFFFD8u
#define LOGICAL_INST   0x8FFFFFD4u
#define MULT_DIV_INST  0x8FFFFFD0u
#define WEIRD_INST     0x8FFFFFCCu

void printExecutedInstructions();

void resetExecutedInstructions();

#endif