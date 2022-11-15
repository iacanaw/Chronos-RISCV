#include "dijkstra_nodead.h"

static char start_print[] =   "Starting Dijkstra Divider.\n";
static char calculations[] =   "Starting Calculation distribution.\n";
static char kill[] =   "Starting Kill distribution.\n";
static char barraene[] =   "  \n";
static char finish_print[] =   "Finishing Dijkstra Divider.\n";
static char dijk_working[] = " dijkstra working.... \n";

volatile static Message msg;

int main(){
	static const int fpTrix[NUM_NODES*NUM_NODES] = { 1,    6,    3,    9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999,
										6,    1,    2,    5,    9999, 9999, 1,    9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999,
										3,    2,    1,    3,    4,    9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999,
										9999, 5,    3,    1,    2,    3,    9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999,
										9999, 9999, 4,    2,    1,    5,    9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999,
										9999, 9999, 9999, 3,    5,    1,    3,    2,    9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999,
										9999, 1,    9999, 9999, 9999, 3,    1,    4,    9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999,
										9999, 9999, 9999, 9999, 9999, 2,    4,    1,    7,    9999, 9999, 9999, 9999, 9999, 9999, 9999,
										9999, 9999, 9999, 9999, 9999, 9999, 9999, 7,    1,    5,    1,    9999, 9999, 9999, 9999, 9999,
										9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 5,    1,    9999, 3,    9999, 9999, 9999, 9999,
										9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 1,    9999, 1,    9999, 4,    9999, 9999, 8,
										9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 3,    9999, 1,    9999, 2,    9999, 9999,
										9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 4,    9999, 1,    1,    9999, 2,
										9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 2,    1,    1,    6,    9999,
										9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 6,    1,    3,
										9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 9999, 8,    9999, 2,    9999, 3,    1 };
	static int AdjMatrix[NUM_NODES][NUM_NODES];
	static int i, j, k, iter;

	if ( !isMigration() ){
		for (i=0;i<NUM_NODES;i++) {
			for (j=0;j<NUM_NODES;j++) {
				AdjMatrix[i][j]= fpTrix[k];
				k++;
			}
		}
		iter = 0; 		// counts the calculations
		i = 0;			// counts the number of lines
		k = 0;			// counts the number of message sent
	}

	sys_Prints((unsigned int)&start_print);

	/* sys_Send AdjMatrix[NUM_NODES][NUM_NODES] */
	msg.length = NUM_NODES;

	sys_Prints((unsigned int)&calculations);
	for(/* iter=0 */; iter < CALCULATIONS; iter++){
		for (/* i=0 */; i < NUM_NODES; i++) {
			for (j = 0; j < NUM_NODES; j++) {
				msg.msg[j] = AdjMatrix[i][j];
			}

			checkMigration(); // CHECKS FOR A MIGRATION REQUEST

			sys_Printi(k);
			sys_Prints((unsigned int)&dijk_working);
			sys_Send(&msg, dijkstra_0);
			sys_Send(&msg, dijkstra_1);
			sys_Send(&msg, dijkstra_2);
			sys_Send(&msg, dijkstra_3);
			sys_Send(&msg, dijkstra_4);
			k = k+5;
		}
		i = 0;
	}

	AdjMatrix[0][0] = KILL;
	for (i = 0; i < NUM_NODES; i++) {
		sys_Printi(i);
		sys_Prints((unsigned int)&kill);
		for (j=0; j<NUM_NODES; j++) {
			msg.msg[j] = AdjMatrix[i][j];
		}
		sys_Send(&msg, dijkstra_0);
		sys_Send(&msg, dijkstra_1);
		sys_Send(&msg, dijkstra_2);
		sys_Send(&msg, dijkstra_3);
		sys_Send(&msg, dijkstra_4);
	}

	sys_Prints((unsigned int)&finish_print);
	sys_Finish();
}
