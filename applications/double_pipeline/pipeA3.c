#include "double_pipeline.h"

static char start_print[] = "Starting DOUBLE PIPELINE - pipe A3 \n";
static char end_print[]   = "DOUBLE PIPELINE has finished. \n";
static char iteration_print[] =   "Iteration: ";
static char resultado_print[] =   " resultado: ";
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
    for (/* i = 0 */; i < PRODCONS_ITERATIONS; i++) {
        
        checkMigration();

        sys_Receive(&mensagem, pipeA2);
        sum = mensagem.msg[100];

        for(inner = 0; inner < INNER_LOOP; inner = inner + 2){
            sum += 2;
        }
        
        sys_Prints((unsigned int)&iteration_print);
        sys_Printi(i);
        sys_Prints((unsigned int)&resultado_print);
        sys_Printi(sum);
        sys_Prints((unsigned int)&barra_ene);

    }

    // releases the master to finish the application
    sys_Send(&mensagem, master);

    sys_Prints((unsigned int)&end_print);
    sys_Finish();

}
