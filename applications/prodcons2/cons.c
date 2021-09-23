#include "prodcons.h"

static char start_print[]=  "CONS2 Start\n";
static char end_print[]=    "CONS2 End\n";

static Message mensagem;

int main(){
    int i;

    sys_Prints((unsigned int)&start_print);

    for (i=0; i<PRODCONS_ITERATIONS; i++) {
        sys_Receive(&mensagem, prod);
    }

    sys_Prints((unsigned int)&end_print);

}
