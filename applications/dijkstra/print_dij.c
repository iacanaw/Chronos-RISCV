#include "dijkstra.h"

static char finish_print[]="DIJKSTRA FINISHED";
static char start_print[] =   "Starting Print Dijkstra.\n";

volatile static Message msg;

int main(){
	int k;
	int result[NUM_NODES*2];

	sys_Prints((unsigned int)&start_print);
	
	sys_Receive(&msg, dijkstra_0);
	for (k=0; k < NUM_NODES*2; k++){
		result[k] = msg.msg[k];
        sys_Printi(result[k]);
    }

	sys_Receive(&msg, dijkstra_1);
	for (k=0; k < NUM_NODES*2; k++){
		result[k] = msg.msg[k];
        sys_Printi(result[k]);
    }

	sys_Receive(&msg, dijkstra_2);
	for (k=0; k < NUM_NODES*2; k++){
		result[k] = msg.msg[k];
        sys_Printi(result[k]);
    }
	
    sys_Receive(&msg, dijkstra_3);
	for (k=0; k < NUM_NODES*2; k++){
		result[k] = msg.msg[k];
        sys_Printi(result[k]);
    }
	
    sys_Receive(&msg, dijkstra_4);
	for (k=0; k < NUM_NODES*2; k++){
		result[k] = msg.msg[k];
        sys_Printi(result[k]);
    }

	sys_Prints((unsigned int)&finish_print);
	sys_Finish();
}