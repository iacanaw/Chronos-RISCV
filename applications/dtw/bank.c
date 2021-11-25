#include "dtw.h"

/*int pattern[MATX_SIZE][MATX_SIZE] = {
	{0,1,2,3,4,5,6,7,8,9,0},
	{0,1,2,3,4,5,6,7,8,9,0},
	{0,1,2,3,4,5,6,7,8,9,0},
	{0,1,2,3,4,5,6,7,8,9,0},
	{0,1,2,3,4,5,6,7,8,9,0},
	{0,1,2,3,4,5,6,7,8,9,0},
	{0,1,2,3,4,5,6,7,8,9,0},
	{0,1,2,3,4,5,6,7,8,9,0},
	{0,1,2,3,4,5,6,7,8,9,0},
	{0,1,2,3,4,5,6,7,8,9,0},
	{0,1,2,3,4,5,6,7,8,9,0}
};*/
static char start_print[]="Bank INIT\n";
static char finish_print[]="Bank sys_Sended all patterns\n";

volatile static Message theMsg;

static int pattern[MATX_SIZE][MATX_SIZE];

int main(){
	int i, j;

    sys_Prints((unsigned int)&start_print);

	//theMsg.length = MATX_SIZE * MATX_SIZE; //MATX_SIZE*MATX_SIZE nao pode ser maior que 128, senao usar o sys_SendData
	
	for (j = 0; j<PATTERN_PER_TASK; j++){
		for (i = 0; i<TOTAL_TASKS; i++){
			randPattern(pattern); //gera uma matriz de valores aleatorios, poderiam ser coeficientes MFCC
			theMsg.length = MATX_SIZE * MATX_SIZE;
			dtw_memcpy(theMsg.msg, pattern, sizeof(pattern));
			sys_Send(&theMsg, P[i]);
		}
	}

	sys_Prints((unsigned int)&finish_print);
    sys_Finish();
}
