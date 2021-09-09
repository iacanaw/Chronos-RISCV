#include "system_call.h"

#include "chronos.h"
#include <errno.h>

/* Receives a message */
int sys_Receive(unsigned int addr, unsigned int from){
	long test;
	prints("1 testando receive!\n");
    printsv("SYS_RECV_MSG ", SYS_RECV_MSG);
    printsv("addr ", addr);
    printsv("from ", from);
	test = syscall_errno(SYS_RECV_MSG, addr, from, 0, 0, 0, 0);
	prints("2 testando receive!\n");
	return test;
}

/* Sends a message */
int sys_Send(unsigned int addr, unsigned int to){
	return syscall_errno(SYS_SEND_MSG, addr, to, 0, 0, 0, 0);
}

/* Finishes a task */
int sys_Exit(){
	return syscall_errno(SYS_END_TASK, 0, 0, 0, 0, 0, 0);
}

/* Prints one value using the printer peripheral */
int sys_Printi(unsigned int value){
	return syscall_errno(SYS_PRINTI, value, 0, 0, 0, 0, 0);
}

/* Prints a string using the printer peripheral */
int sys_Prints(unsigned int addr){
	return syscall_errno(SYS_PRINTS, addr, 0, 0, 0, 0, 0);
}

int sys_Testing(unsigned int arg0, unsigned int arg1, unsigned int arg2, unsigned int arg3, unsigned int arg4, unsigned int arg5){
    return syscall_errno(SYS_TESTING, arg0, arg1, arg2, arg3, arg4, arg5);
}

/* Syscall Handler */
void handle_syscall(unsigned int *svc_args){
    
    vPortEnterCritical();
    prints("Cheguei aqui!!!!!!! sucessooo :D\n");
    vPortExitCritical();
    return;
}