#include "prodcons.h"

static char start_print[] = "PROD2 Start \n";
static char end_print[] =   "PROD2 End \n";

volatile static Message mensagem;

int main(){
    int i;
    
    sys_Prints((unsigned int)&start_print);

    for (i=0;i<MSG_SIZE;i++) {
        mensagem.msg[i]=i;
    }

    mensagem.length = 84;

    for (i=0; i<PRODCONS_ITERATIONS; i++) {
        mensagem.msg[9] = 0xC0A + i;
        sys_Send(&mensagem, cons);
    }

    sys_Prints((unsigned int)&end_print);
    sys_Finish();
}
