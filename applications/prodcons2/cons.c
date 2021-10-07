#include "prodcons.h"

static char start_print[] = "CONS2 Start \n";
static char end_print[]   = "CONS2 End \n";
static char txt_print1[]  = "Chegou a mensagem numero: ";
static char txt_print2[]  = " - tamanho: ";
static char txt_n[]       = " .\n";
static char txt_printDeupau[] = ">>>>DEU MERD@!\n";

volatile static Message mensagem;

int main(){
    int i, j;

    sys_Prints((unsigned int)&start_print);

    for (i=0; i<PRODCONS_ITERATIONS; i++) {
        sys_Receive(&mensagem, prod);

        if(mensagem.msg[9] == 0xC0A + i){
            sys_Prints((unsigned int)&txt_print1);
            sys_Printi(i+1);
            sys_Prints((unsigned int)&txt_print2);
            sys_Printi(mensagem.length);
            sys_Prints((unsigned int)&txt_n);
        }
        else{
            sys_Printi(mensagem.msg[9]-0xC0A);
            sys_Prints((unsigned int)&txt_printDeupau);
        }
    }

    sys_Prints((unsigned int)&end_print);
    sys_Finish();
}
