#include "dtwx10.h"

static char start_print[]="Task P1 INIT\n";
static char finish_print[]="Task P1 FINISHED \n";

volatile static Message msg;

int main(){
	volatile static int test[MATX_SIZE][MATX_SIZE];
	volatile static int pattern[MATX_SIZE][MATX_SIZE];
	volatile static int result, j, prelude;

	if ( !isMigration() ){
        j = 0;			// stores the amout of worked patterns
		prelude = 0; 	// register if the prelude was executed
    }

	sys_Prints((unsigned int)&start_print);

	if(!prelude){
		sys_Receive(&msg, recognizer);

		dtw_memcpy(test, msg.msg, sizeof(test));
		
		prelude = 1;
	}

	for (/* j = 0 */; j < PATTERN_PER_TASK; j++){

		checkMigration(); // CHECKS FOR A MIGRATION REQUEST

		dtw_memset(msg.msg, 0, sizeof(int) * MESSAGE_MAX_SIZE);

		sys_Receive(&msg, bank);

		dtw_memcpy(pattern, msg.msg, sizeof(pattern));

		result = dtw_dynamicTimeWarping(test, pattern);

		msg.length = 1;

		msg.msg[0] = result;

		sys_Send(&msg, recognizer);
	}

	sys_Prints((unsigned int)&finish_print);
	sys_Finish();
}
