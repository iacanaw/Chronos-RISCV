/*
 * cons.c
 *
 *  Created on: 07/03/2013
 *      Author: mruaro
 *
 *  Changed on: 16/09/2016 - Geancarlo Abich
 *
 */

#include "prodcons.h"

static char start_print[]=  "CONS Start";
static char end_print[]=    "CONS End";

Message msg;

int main()
{
    int i;

    volatile int p;

    //SYSCALL_PRINTF(0,0,0,start_print);
    sys_Testing(1,(unsigned int)&start_print,3,4,5,6);

    for (i=0; i<PRODCONS_ITERATIONS; i++) {
        // SYSCALL_PRINTF(0,0,0,"CONS");
        //SYSCALL_RCV(prod,0,0,&msg);
        sys_Testing(1,2,3,4,5,6);
    }

    //SYSCALL_PRINTF(0,0,0,end_print);

    //SYSCALL_DELETE(0,0,0,0);
}


