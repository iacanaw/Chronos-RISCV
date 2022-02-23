#include "dtw.h"

static char start_print[]="Task P4 INIT\n";
static char finish_print[]="Task P4 FINISHED \n";

volatile static Message msg;

int main(){
	int test[MATX_SIZE][MATX_SIZE];
	int pattern[MATX_SIZE][MATX_SIZE];
	int result, j;

	sys_Receive(&msg, recognizer);

	sys_Prints((unsigned int)&start_print);

	dtw_memcpy(test, msg.msg, sizeof(test));

	for (j = 0; j < PATTERN_PER_TASK; j++){
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
