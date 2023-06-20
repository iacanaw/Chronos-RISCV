#include "dtw.h"

volatile static Message msg;

static char start_print[]="Recognizer INIT\n";
static char finish_print[]="Recognizer FINISHED\n";
static char warning_print[]="Test sys_Sendedd to all tasks\n";
static char progress_print[]=" of ";
static char progress2_print[]=" ... ;\n";

int main(){
	volatile static int i, j, prelude;
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

	if ( !isMigration() ){
        i = 0;     		// stores the last task it has sent a packet 
		j = 0;	   		// stores the amout of patterns sent
        prelude = 1;	// runs the prelude  
    }
	
	sys_Prints((unsigned int)&start_print);

    if(prelude){
		msg.length = MATX_SIZE * MATX_SIZE;
		
		dtw_memcpy(msg.msg, test, sizeof(test));

		for (/* i = 0 */; i < TOTAL_TASKS; i++){
			
			checkMigration(); // CHECKS FOR A MIGRATION REQUEST

			sys_Send(&msg, P[i]);
		}
		prelude = 0;
		i = 0;
		j = 0;
	}

	sys_Prints((unsigned int)&warning_print);

	for (/* j = 0 */; j < PATTERN_PER_TASK; j++){
		for (/* i = 0 */; i < TOTAL_TASKS; i++){

			checkMigration(); // CHECKS FOR A MIGRATION REQUEST

			sys_Receive(&msg, P[i]);
		}
        sys_Printi(j);
        sys_Prints((unsigned int)&progress_print);
		sys_Printi(PATTERN_PER_TASK);
		sys_Prints((unsigned int)&progress2_print);
		i = 0; // to reset the inner loop
	}

	sys_Prints((unsigned int)&finish_print);

	sys_Finish();
}
