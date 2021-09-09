

/*
#define SYSCALL_RECEIVE(addr, from)         SBI_CALL(SYS_RECV_MSG,   addr,   from,   0)
#define SYSCALL_SEND(addr, to)              SBI_CALL(SYS_SEND_MSG,   addr,   to,     0)
#define SYSCALL_END_TASK(task)              SBI_CALL(SYS_END_TASK,   task,   0,      0)
#define SYSCALL_PRINTI(value)               SBI_CALL(SYS_PRINTI,     value,  0,      0)
#define SYSCALL_PRINTS(str)                 SBI_CALL(SYS_PRINTS,     str,    0,      0)


/* Copyright (c) 2017  SiFive Inc. All rights reserved.
   This copyrighted material is made available to anyone wishing to use,
   modify, copy, or redistribute it subject to the terms and conditions
   of the FreeBSD License.   This program is distributed in the hope that
   it will be useful, but WITHOUT ANY WARRANTY expressed or implied,
   including the implied warranties of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  A copy of this license is available at
   http://www.opensource.org/licenses.
*/

#ifndef __SYSTEM_CALL_H__
#define __SYSTEM_CALL_H__

#include "chronos.h"
#include <errno.h>

#define SYS_END_TASK        ( 10 )
#define SYS_SEND_MSG        ( 20 )
#define SYS_RECV_MSG        ( 30 )
#define SYS_PRINTI          ( 40 )
#define SYS_PRINTS          ( 50 )
#define SYS_TESTING         ( 66 )

static inline long __syscall_error(long a0) {
	errno = -a0;
	return -1;
}

static inline long __internal_syscall(long n, long _a0, long _a1, long _a2, long _a3, long _a4, long _a5) {
	register long a0 asm("a0") = _a0;
	register long a1 asm("a1") = _a1;
	register long a2 asm("a2") = _a2;
	register long a3 asm("a3") = _a3;
	register long a4 asm("a4") = _a4;
	register long a5 asm("a5") = _a5;
	register long syscall_id asm("a7") = n;

	asm volatile ("ecall"
			: "+r"(a0) : "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5), "r"(syscall_id));

	return a0;
}

static inline long syscall_errno(long n, long _a0, long _a1, long _a2, long _a3, long _a4, long _a5) {
	long a0 = __internal_syscall(n, _a0, _a1, _a2, _a3, _a4, _a5);

	if (a0 < 0)
		return __syscall_error (a0);
	else
		return a0;
}

/* Testing syscall*/
int sys_Testing(unsigned int arg0, unsigned int arg1, unsigned int arg2, unsigned int arg3, unsigned int arg4, unsigned int arg5);

/* Receives a message */
int sys_Receive(unsigned int addr, unsigned int from);

/* Sends a message */
int sys_Send(unsigned int addr, unsigned int to);

/* Finishes a task */
int sys_Exit();

/* Prints one value using the printer peripheral */
int sys_Printi(unsigned int value);

/* Prints a string using the printer peripheral */
int sys_Prints(unsigned int addr);

#endif