#include "prodcons.h"

static char start_print[]=  "CONS1 Start\n";
static char end_print[]=    "CONS1 End\n";

static Message mensagem;

int main(){
    int i;

    sys_Prints((unsigned int)&start_print);

    for (i=0; i<PRODCONS_ITERATIONS; i++) {
        sys_Receive(&mensagem, prod);
        sys_Prints("Chegou msg de: ");
        sys_Printi(mensagem.length);
        sys_Prints(" flits \nFlit 9: ");
        sys_Printi(mensagem.msg[9]);
        sys_Prints("\n");
    }

    sys_Prints((unsigned int)&end_print);

}
