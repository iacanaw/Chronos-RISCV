/*******************************************************************************
 * (c) Copyright 2016-2017 Microsemi SoC Products Group.  All rights reserved.
 *
 * @file entry.S
 * @author Microsemi SoC Products Group
 * @brief Mi-V soft processor vectors, trap handling and startup code.
 *
 * SVN $Revision: 9587 $
 * SVN $Date: 2017-11-16 12:53:31 +0530 (Thu, 16 Nov 2017) $
 */
#ifndef ENTRY_S
#define ENTRY_S

#include "encoding.h"

#if __riscv_xlen == 64
# define LREG ld
# define SREG sd
# define REGBYTES 8
#else
# define LREG lw
# define SREG sw
# define REGBYTES 4
#endif

  .section      .text.entry
  .globl _start

_start:
  j handle_reset

nmi_vector:
  j nmi_vector

trap_vector:
  j trap_entry

handle_reset:
  la t0, trap_entry
  csrw mtvec, t0
  csrwi mstatus, 0
  csrwi mie, 0

/*Flaoting point support configuration*/

#ifdef __riscv_flen
  csrr t0, mstatus
  lui t1, 0xffffa
  addi t1, t1, -1
  and t0, t0, t1
  lui t1, 0x4
  or t1, t0, t1
  csrw mstatus, t1

  lui t0, 0x0
  fscsr t0
#endif

  # initialize global pointer
  la gp, __global_pointer$

  # initialize stack pointer
  la sp, __stack_top

  # perform the rest of initialization in C
  j _init


trap_entry:
  addi sp, sp, -33*REGBYTES

  SREG x1, 0 * REGBYTES(sp)
  SREG x2, 1 * REGBYTES(sp)
  SREG x3, 2 * REGBYTES(sp)
  SREG x4, 3 * REGBYTES(sp)
  SREG x5, 4 * REGBYTES(sp)
  SREG x6, 5 * REGBYTES(sp)
  SREG x7, 6 * REGBYTES(sp)
  SREG x8, 7 * REGBYTES(sp)
  SREG x9, 8 * REGBYTES(sp)
  SREG x10, 9 * REGBYTES(sp)
  SREG x11, 10 * REGBYTES(sp)
  SREG x12, 11 * REGBYTES(sp)
  SREG x13, 12 * REGBYTES(sp)
  SREG x14, 13 * REGBYTES(sp)
  SREG x15, 14 * REGBYTES(sp)
  SREG x16, 15 * REGBYTES(sp)
  SREG x17, 16 * REGBYTES(sp)
  SREG x18, 17 * REGBYTES(sp)
  SREG x19, 18 * REGBYTES(sp)
  SREG x20, 19 * REGBYTES(sp)
  SREG x21, 20 * REGBYTES(sp)
  SREG x22, 21 * REGBYTES(sp)
  SREG x23, 22 * REGBYTES(sp)
  SREG x24, 23 * REGBYTES(sp)
  SREG x25, 24 * REGBYTES(sp)
  SREG x26, 25 * REGBYTES(sp)
  SREG x27, 26 * REGBYTES(sp)
  SREG x28, 27 * REGBYTES(sp)
  SREG x29, 28 * REGBYTES(sp)
  SREG x30, 29 * REGBYTES(sp)
  SREG x31, 30 * REGBYTES(sp)

  # Stores the return addr in the stack
  csrr t0, mepc
  SREG t0, 31 * REGBYTES(sp)

  # store the MIE value in the stack
  li t0, MSTATUS_MIE
  csrrc t0, mstatus, t0 
  SREG t0, 32 * REGBYTES(sp) 

  csrr a0, mcause
  csrr a1, mepc
  mv a2, sp
  call handle_trap
  csrw mepc, a0

  # Remain in M-mode after mret
  li t0, MSTATUS_MPP
  csrs mstatus, t0
  #li t0, MSTATUS_MPIE
  #csrs mstatus, t0

  # Restore the interruption status after MRET
  LREG x5, 32 * REGBYTES(sp)  # load the old MIE value to t0
  slli t0, t0, 4              # shifts it to the MPIE position
  csrs mstatus, t0            # restore the MIE value into the MPIE
  
  LREG x1, 0 * REGBYTES(sp)
  LREG x2, 1 * REGBYTES(sp)
  LREG x3, 2 * REGBYTES(sp)
  LREG x4, 3 * REGBYTES(sp)
  LREG x5, 4 * REGBYTES(sp)
  LREG x6, 5 * REGBYTES(sp)
  LREG x7, 6 * REGBYTES(sp)
  LREG x8, 7 * REGBYTES(sp)
  LREG x9, 8 * REGBYTES(sp)
  LREG x10, 9 * REGBYTES(sp)
  LREG x11, 10 * REGBYTES(sp)
  LREG x12, 11 * REGBYTES(sp)
  LREG x13, 12 * REGBYTES(sp)
  LREG x14, 13 * REGBYTES(sp)
  LREG x15, 14 * REGBYTES(sp)
  LREG x16, 15 * REGBYTES(sp)
  LREG x17, 16 * REGBYTES(sp)
  LREG x18, 17 * REGBYTES(sp)
  LREG x19, 18 * REGBYTES(sp)
  LREG x20, 19 * REGBYTES(sp)
  LREG x21, 20 * REGBYTES(sp)
  LREG x22, 21 * REGBYTES(sp)
  LREG x23, 22 * REGBYTES(sp)
  LREG x24, 23 * REGBYTES(sp)
  LREG x25, 24 * REGBYTES(sp)
  LREG x26, 25 * REGBYTES(sp)
  LREG x27, 26 * REGBYTES(sp)
  LREG x28, 27 * REGBYTES(sp)
  LREG x29, 28 * REGBYTES(sp)
  LREG x30, 29 * REGBYTES(sp)
  LREG x31, 30 * REGBYTES(sp)

  addi sp, sp, 33*REGBYTES
  mret

#endif

