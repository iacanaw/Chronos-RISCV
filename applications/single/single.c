#include "single.h"

static char start_print[] = "Task Single Starting \n";
static char end_print[] =   "Task Single Ended \n";
static char iteration_print[] =   "Iteration: ";
static char barra_ene[] = "... \n";

int main(){
    volatile static int i;              // relevant to the task context
    volatile static int vec[VEC_SIZE];      // relevant to the task context
    int j, aux;

    if ( !isMigration() ){   // if isn't a migrated task 
        i = 1;                      // stores the iteration value 
        for (j=0;j<VEC_SIZE;j++) {  // creates the first message
            vec[j] = VEC_SIZE-j;
        }
    }

    sys_Prints((unsigned int)&start_print);

    /* main loop */
    for (/* i = 1 */; i<VEC_SIZE; i++) {

        checkMigration();
        sys_Prints((unsigned int)&iteration_print);
        sys_Printi(i);
        sys_Prints((unsigned int)&barra_ene);

        for(j = 0; j < VEC_SIZE-1; j++){
            if(vec[j] > vec[j+1]){
                aux = vec[j];
                vec[j] = vec[j+1];
                vec[j+1] = aux;
            }
        }
        
    }

    sys_Prints((unsigned int)&end_print);
    sys_Finish();
}
