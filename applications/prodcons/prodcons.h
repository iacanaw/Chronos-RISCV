#ifndef __PROD_CONS_H__
#define __PROD_CONS_H__
#include "../applications.h"

#define PRODCONS_ITERATIONS 223

#define cons 1
#define prod 0

volatile static int MIGRATE = -1;

// Default function to check if a migration is required!
int checkMigration(){

    static char txt[] = "Starting Migration Process!\n";

	if (MIGRATE == 1){
		sys_Prints((unsigned int)&txt);
		sys_Migrate();
	}
	return 0; // this return will never happen if the task will migrate
}

#endif /*__PROD_CONS_H__*/
