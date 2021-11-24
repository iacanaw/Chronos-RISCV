#include "prodcons.h"

static char start_print[] = "CONS1 Start \n";
static char end_print[]   = "CONS1 End \n";
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
        sys_Prints((unsigned int)&txt_print1);
        sys_Printi(mensagem.msg[9]-0xb0a);
        sys_Prints((unsigned int)&txt_print2);
        sys_Printi(mensagem.length);
        sys_Prints((unsigned int)&txt_n);

        // if(mensagem.msg[9] == 0xB0A + i){
        //     sys_Prints((unsigned int)&txt_print1);
        //     sys_Printi(i+1);
        //     sys_Prints((unsigned int)&txt_print2);
        //     sys_Printi(mensagem.length);
        //     sys_Prints((unsigned int)&txt_n);
        // }
        // else{
        //     sys_Prints((unsigned int)&txt_printDeupau);
        //     for(j = 0; j < mensagem.length; j++){
        //         sys_Printi(mensagem.msg[j]);
        //         sys_Prints((unsigned int)&txt_n);
        //     }
        //     //sys_Printi(mensagem.msg[9]-0xB0A);
        // }
    }

    sys_Prints((unsigned int)&end_print);
    sys_Finish();
}
