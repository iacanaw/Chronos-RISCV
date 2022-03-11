#include "prodcons.h"

static char start_print[] = "CONS1 Start \n";
static char end_print[]   = "CONS1 End \n";
static char txt_print1[]  = "Chegou a mensagem numero: ";
static char txt_print2[]  = " - tamanho: ";
static char txt_printSum[]  = "The summed values is: ";
static char txt_printPASum[]  = "The summed values should be: ";
static char txt_n[]       = " .\n";
static char txt_printDeupau[] = ">>>>DEU MERD@!\n";

int main(){
    
    volatile static int i, sum, first, last;
    volatile static Message mensagem;
    
    if ( !isMigration() ){
        i = 0;     // stores the iteration value 
        sum = 0;   // starts the sum with zero
        first = 0; // defines the first value received
        last = 0;  // stores the last value received
    }
    
    sys_Prints((unsigned int)&start_print);

    /* main loop */
    for (/* i = 0 */; i < PRODCONS_ITERATIONS; i++) {
        
        checkMigration(i);

        sys_Receive(&mensagem, prod);
        sys_Prints((unsigned int)&txt_print1);
        sum += mensagem.msg[9]-0xb0a;
        sys_Printi(mensagem.msg[9]-0xb0a);
        sys_Prints((unsigned int)&txt_print2);
        sys_Printi(mensagem.length);
        sys_Prints((unsigned int)&txt_n);

        if( i == 0 )
            first = mensagem.msg[9]-0xb0a;
        else if( i == PRODCONS_ITERATIONS-1)
            last = mensagem.msg[9]-0xb0a;

    }

    // Prints the summed value
    sys_Prints((unsigned int)&txt_printSum);
    sys_Printi(sum);
    sys_Prints((unsigned int)&txt_n);

    sum = ((first+last)*PRODCONS_ITERATIONS)/2;
    sys_Prints((unsigned int)&txt_printPASum);
    sys_Printi(sum);
    sys_Prints((unsigned int)&txt_n);

    sys_Prints((unsigned int)&end_print);
    sys_Finish();

}
