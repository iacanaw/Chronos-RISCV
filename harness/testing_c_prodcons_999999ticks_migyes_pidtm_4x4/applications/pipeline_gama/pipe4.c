#include "pipeline_gama.h"

static char start_print[] = "Starting PIPELINE GAMA - pipe 4 \n";
static char end_print[]   = "PIPELINE GAMA has finished. \n";
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

    mensagem.length = 1;

    /* main loop */
    for (/* i = 0 */; i < PRODCONS_ITERATIONS; i++) {
        
        checkMigration();

        sys_Receive(&mensagem, pipe3);
        sum = mensagem.msg[0];

        for(inner = 0; inner < INNER_LOOP; inner = inner + 5){
            sum += 5;
        }
        
        sys_Prints((unsigned int)&iteration_print);
        sys_Printi(i);
        sys_Prints((unsigned int)&barra_ene);

        mensagem.msg[0] = sum;
        sys_Send(&mensagem, pipe5);

    }

    // wait the previous stage to finish
    sys_Receive(&mensagem, pipe3);

    // releases the next stage
    sys_Send(&mensagem, pipe5);

    sys_Prints((unsigned int)&end_print);
    sys_Finish();

}
