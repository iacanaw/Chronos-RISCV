/*#include "include/system_call.h"

static void __internal_syscall(long n, long _a0, long _a1, long _a2, long _a3, long _a4, long _a5) {
	register long a0 asm("a0") = _a0;
	register long a1 asm("a1") = _a1;
	register long a2 asm("a2") = _a2;
	register long a3 asm("a3") = _a3;
	register long a4 asm("a4") = _a4;
	register long a5 asm("a5") = _a5;
	register long syscall_id asm("a7") = n;
	asm ("ecall");
	return;
}

/* Receives a message 
static int sys_Receive(unsigned int addr, unsigned int from){
	__internal_syscall(SYS_RECV_MSG, addr, from, 0, 0, 0, 0);
	return 1;
}

/* Sends a message 
static int sys_Send(unsigned int addr, unsigned int to){
	__internal_syscall(SYS_SEND_MSG, addr, to, 0, 0, 0, 0);
	return 1;
}

/* Finishes a task 
static int sys_Exit(){
	__internal_syscall(SYS_END_TASK, 0, 0, 0, 0, 0, 0);
	return 1;
}

/* Prints one value using the printer peripheral 
static int sys_Printi(unsigned int value){
	__internal_syscall(SYS_PRINTI, value, 0, 0, 0, 0, 0);
	return 1;	
}

/* Prints a string using the printer peripheral 
static int sys_Prints(unsigned int addr){
	__internal_syscall(SYS_PRINTS, addr, 0, 0, 0, 0, 0);
	return 1;
}

/* Testing syscall
static int sys_Testing(unsigned int arg0, unsigned int arg1, unsigned int arg2, unsigned int arg3, unsigned int arg4, unsigned int arg5){
	__internal_syscall(SYS_TESTING, arg0, arg1, arg2, arg3, arg4, arg5);
	return 1;
}
*/