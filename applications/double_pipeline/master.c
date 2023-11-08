#include "double_pipeline.h"

static char start_print[] = "Starting DOUBLE PIPELINE - master \n";
static char end_print[]   = "DOUBLE PIPELINE has finished. \n";
static char iteration_print[] =   "Iteration: ";
static char barra_ene[] = "... \n";

int main(){
    
    static int i, inner;
    volatile static int sum;
    volatile static Message mensagem;
    
    if ( !isMigration() ){
        i = 0;     // stores the iteration value 
        sum = 0;   // starts the sum with zero
    }
    
    sys_Prints((unsigned int)&start_print);

    mensagem.length = 112;

    /* main loop */
    for (/* i = 0 */; i < (VAR_ITERATION/65)+32; i++) {
        
        checkMigration();
        for(inner = 0; inner < INNER_LOOP; inner++){
            sum += 1;
        }
        
        sys_Prints((unsigned int)&iteration_print);
        sys_Printi(i);
        sys_Prints((unsigned int)&barra_ene);

        mensagem.msg[100] = sum;
        sys_Send(&mensagem, pipeA1);

        mensagem.msg[100] = (sum<<1);
        sys_Send(&mensagem, pipeB1);

    }

    // waits until the pipeline is empty
    sys_Receive(&mensagem, pipeB3);
    sys_Receive(&mensagem, pipeA3);

    // releases the A1 and B1
    sys_Send(&mensagem, pipeA1);
    sys_Send(&mensagem, pipeB1);

    sys_Prints((unsigned int)&end_print);
    sys_Finish();

}
