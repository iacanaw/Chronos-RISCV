#include "prodcons.h"

static char start_print[] = "CONS2 Start \n";
static char end_print[]   = "CONS2 End \n";
static char txt_print1[]  = "\nChegou a mensagem numero: ";
static char txt_print2[]  = " - tamanho: ";
static char txt_n[]       = " .\n";

volatile static Message mensagem;

int main(){
    int i, j;

    sys_Prints((unsigned int)&start_print);

    for (i=0; i<PRODCONS_ITERATIONS; i++) {
        sys_Receive(&mensagem, prod);
        sys_Prints((unsigned int)&txt_print1);
        sys_Printi(i+1);
        sys_Prints((unsigned int)&txt_print2);
        sys_Printi(mensagem.length);
        sys_Prints((unsigned int)&txt_n);
    }

    sys_Prints((unsigned int)&end_print);
    sys_Finish();
}
