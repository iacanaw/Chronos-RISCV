#include "pipeline_alpha.h"

static char start_print[] = "Starting PIPELINE APLHA - pipe 3 \n";
static char end_print[]   = "PIPELINE ALPHA has finished. \n";
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

    mensagem.length = 1;

    /* main loop */
    for (/* i = 0 */; i < (VAR_ITERATION/60)+10; i++) {
        
        checkMigration();

        sys_Receive(&mensagem, pipe2);
        sum = mensagem.msg[0];

        for(inner = 0; inner < INNER_LOOP; inner++){
            
            sum += 1;
            
        }
        
        sys_Prints((unsigned int)&iteration_print);
        sys_Printi(i);
        sys_Prints((unsigned int)&resultado_print);
        sys_Printi(sum);
        sys_Prints((unsigned int)&barra_ene);

    }

    // releases the previous stage
    sys_Send(&mensagem, pipe0);

    sys_Prints((unsigned int)&end_print);
    sys_Finish();

}
