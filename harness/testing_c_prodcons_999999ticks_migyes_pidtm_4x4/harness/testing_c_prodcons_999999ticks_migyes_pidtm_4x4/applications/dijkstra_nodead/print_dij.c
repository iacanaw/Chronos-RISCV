#include "dijkstra_nodead.h"

static char finish_print[]="DIJKSTRA FINISHED";
static char start_print[] =   "Starting Print Dijkstra.\n";
static char node_print[] = "======= NODE: ";
static char space_print[] = ", ";
static char barra_ene[] = "  \n";

volatile static Message msg;

int main(){
	int k;
	int result[NUM_NODES*2];

	if ( !isMigration() ){
		asm("nop");
	}

	sys_Prints((unsigned int)&start_print);
	
	sys_Receive(&msg, dijkstra_0);
	sys_Prints((unsigned int)&node_print);
	sys_Printi(0);
	for (k=0; k < NUM_NODES*2; k++){
		result[k] = msg.msg[k];
        sys_Printi(result[k]);
		sys_Prints((unsigned int)&space_print);
    }
	sys_Prints((unsigned int)&barra_ene);

	sys_Receive(&msg, dijkstra_1);
	sys_Prints((unsigned int)&node_print);
	sys_Printi(1);
	for (k=0; k < NUM_NODES*2; k++){
		result[k] = msg.msg[k];
        sys_Printi(result[k]);
		sys_Prints((unsigned int)&space_print);
    }
	sys_Prints((unsigned int)&barra_ene);

	sys_Receive(&msg, dijkstra_2);
	sys_Prints((unsigned int)&node_print);
	sys_Printi(2);
	for (k=0; k < NUM_NODES*2; k++){
		result[k] = msg.msg[k];
        sys_Printi(result[k]);
		sys_Prints((unsigned int)&space_print);
    }
	sys_Prints((unsigned int)&barra_ene);
	
    sys_Receive(&msg, dijkstra_3);
	sys_Prints((unsigned int)&node_print);
	sys_Printi(3);
	for (k=0; k < NUM_NODES*2; k++){
		result[k] = msg.msg[k];
        sys_Printi(result[k]);
		sys_Prints((unsigned int)&space_print);
    }
	sys_Prints((unsigned int)&barra_ene);
	
    sys_Receive(&msg, dijkstra_4);
	sys_Prints((unsigned int)&node_print);
	sys_Printi(4);
	for (k=0; k < NUM_NODES*2; k++){
		result[k] = msg.msg[k];
        sys_Printi(result[k]);
		sys_Prints((unsigned int)&space_print);
    }
	sys_Prints((unsigned int)&barra_ene);

	sys_Prints((unsigned int)&finish_print);
	sys_Finish();
}