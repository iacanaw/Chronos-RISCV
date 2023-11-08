#include "double_pipeline.h"

static char start_print[] = "Starting DOUBLE PIPELINE - pipe B2 \n";
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

    /* main loop */
    for (/* i = 0 */; i < (VAR_ITERATION/65)+32; i++) {
        
        checkMigration();

        sys_Receive(&mensagem, pipeB1);
        sum = mensagem.msg[100];

        for(inner = 0; inner < INNER_LOOP; inner = inner + 3){
            sum += 3;
        }
        
        sys_Prints((unsigned int)&iteration_print);
        sys_Printi(i);
        sys_Prints((unsigned int)&barra_ene);

        mensagem.length = 112;
        mensagem.msg[100] = sum;
        sys_Send(&mensagem, pipeB3);

    }

    // waits until the master is done
    sys_Receive(&mensagem, pipeB1);

    sys_Prints((unsigned int)&end_print);
    sys_Finish();

}
