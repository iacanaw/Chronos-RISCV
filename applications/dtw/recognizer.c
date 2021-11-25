#include "dtw.h"

volatile static Message msg;

static char start_print[]="Recognizer INIT\n";
static char finish_print[]="Recognizer FINISHED\n";
static char warning_print[]="Test sys_Sendedd to all tasks\n";
static char progress_print[]=" of ";
static char progress2_print[]=" ... ;\n";

int main(){
	int i, j;

    static const int test[MATX_SIZE][MATX_SIZE] = { {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0},
                                                    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0},
                                                    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0},
                                                    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0},
                                                    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0},
                                                    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0},
                                                    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0},
                                                    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0},
                                                    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0},
                                                    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0},
                                                    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0}};

	msg.length = MATX_SIZE * MATX_SIZE;
	
	sys_Prints((unsigned int)&start_print);
	dtw_memcpy(msg.msg, test, sizeof(test));

	for (i = 0; i < TOTAL_TASKS; i++){
		sys_Send(&msg, P[i]);
	}

	sys_Prints((unsigned int)&warning_print);

	for (j = 0; j < PATTERN_PER_TASK; j++){
		for (i = 0; i < TOTAL_TASKS; i++){
			sys_Receive(&msg, P[i]);
		}
        sys_Printi(j);
        sys_Prints((unsigned int)&progress_print);
		sys_Printi(PATTERN_PER_TASK);
		sys_Prints((unsigned int)&progress2_print);
	}

	/*j = distances[0];

	for(i=1; i<TOTAL_TASKS; i++){
		if(j<distances[i])
			j = distances[i];
	}*/

	sys_Prints((unsigned int)&finish_print);
	/*sys_Prints(dtw_itoa(j));
	sys_Prints("tempo: ");
	sys_Prints("%d\n", clock());*/

	sys_Finish();
}
