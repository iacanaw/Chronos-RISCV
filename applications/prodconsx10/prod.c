#include "prodconsx10.h"

static char start_print[] = "PROD1 Start \n";
static char end_print[] =   "PROD1 End \n";
static char iteration_print[] =   "Iteration: ";
static char barra_ene[] = "... \n";

int main(){
    volatile static int i;              // relevant to the task context
    volatile static Message mensagem;   // relevant to the task context
    int j;                              // not relevant to the task context

    if ( !isMigration() ){   // if isn't a migrated task 
        i = 0;                      // stores the iteration value 
        for (j=0;j<MSG_SIZE;j++) {  // creates the first message
            mensagem.msg[i] = i;      
        }
        mensagem.length = 25;       // defines the message size
    }

    sys_Prints((unsigned int)&start_print);

    /* main loop */
    for (/* i = 0 */; i<PRODCONS_ITERATIONS; i++) {

        checkMigration();

        sys_Prints((unsigned int)&iteration_print);
        sys_Printi(i);
        sys_Prints((unsigned int)&barra_ene);

        mensagem.msg[9] = 0xB0A + i;
        sys_Send(&mensagem, cons);
    }

    sys_Prints((unsigned int)&end_print);
    sys_Finish();
}
