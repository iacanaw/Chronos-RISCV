#include "fibonacci.h"

static char start_print[] =   "Starting fibonacci Divider.\n";
static char calculations[] =   "Starting Calculation distribution.\n";
static char kill[] =   "Starting Kill distribution.\n";
static char barraene[] =   "  \n";
static char finish_print[] =   "Finishing fibonacci Divider.\n";
static char dijk_working[] = " fibonacci working.... \n";

volatile static Message msg;

int main(){
	static int i=1;

	sys_Prints((unsigned int)&start_print);


	sys_Prints((unsigned int)&calculations);

	//sys_Printi(k);
	sys_Prints((unsigned int)&dijk_working);

	msg.length = 5;

	for (i = 1; i < 6; i++) {
		msg.msg[i-1] = i;
		//sys_Printi(i);
		//sys_Prints((unsigned int)&kill);
	}

	sys_Send(&msg, fibonacci_0);
	sys_Send(&msg, fibonacci_1);
	sys_Send(&msg, fibonacci_2);
	sys_Send(&msg, fibonacci_3);
	sys_Send(&msg, fibonacci_4);

	sys_Prints((unsigned int)&finish_print);
	sys_Finish();
}