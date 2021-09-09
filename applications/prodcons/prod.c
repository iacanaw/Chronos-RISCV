/*
 * prod.c
 *
 *  Created on: 07/03/2013
 *      Author: mruaro
 *
 *  Changed on: 16/09/2016 - Geancarlo Abich
 *
 */

#include "prodcons.h"

static char start_print[]=  "PROD Start";
static char end_print[]=    "PROD End";

Message msg;

int main()
{

    int i;

    volatile int t;

    SYSCALL_PRINTF(0,0,0,start_print);

    for (i=0;i<128;i++) {
        msg.msg[i]=i;
    }

    msg.length = 10;

    msg.msg[9] = 0xB0A;

    for (i=0; i<SYNTHETIC_ITERATIONS; i++) {
        /* SYSCALL_PRINTF(1,0,0,i+10000); */
        SYSCALL_SEND(cons,0,0,&msg);
    }

    SYSCALL_PRINTF(0,0,0,end_print);

    SYSCALL_DELETE(0,0,0,0);

}


