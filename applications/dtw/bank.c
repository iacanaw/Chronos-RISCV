#include "dtw.h"

static char start_print[]="Bank INIT\n";
static char finish_print[]="Bank sys_Sended all patterns\n";
static int pattern[MATX_SIZE][MATX_SIZE];
volatile static Message theMsg;

int main(){
	volatile static int i, j;

	if ( !isMigration() ){
		i = 0;	// stores the number of tasks that a given pattern was sent
		j = 0;	// stores the number of patterns sent
	}
	
	sys_Prints((unsigned int)&start_print);
	
	for (/* j = 0 */; j < PATTERN_PER_TASK; j++){
		for (/* i = 0 */; i < TOTAL_TASKS; i++){

			checkMigration(); // CHECKS FOR A MIGRATION REQUEST
			
			randPattern(pattern); //gera uma matriz de valores aleatorios, poderiam ser coeficientes MFCC
			theMsg.length = MATX_SIZE * MATX_SIZE;
			dtw_memcpy(theMsg.msg, pattern, sizeof(pattern));
			sys_Send(&theMsg, P[i]);
		}
		i = 0; // to reset the inner loop
	}

	sys_Prints((unsigned int)&finish_print);
    sys_Finish();
}
