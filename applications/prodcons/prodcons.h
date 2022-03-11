#ifndef __PROD_CONS_H__
#define __PROD_CONS_H__
#include "../applications.h"

#define PRODCONS_ITERATIONS 223

#define cons 1
#define prod 0

volatile static int MIGRATE = 0xFFFFFFFE;

// Default function to check if a migration is required!
int checkMigration(){

    static char txt[] = "Starting Migration Process!\n";

	if (MIGRATE == 0xFFFFFFFF){
		sys_Prints((unsigned int)&txt);
		sys_Migrate();
	}
	return 0; // this return will never happen if the task will migrate
}

// Default function to check if this is a migrated task.
int isMigration(){
	static char txt[] = "Restarting after Migration Process!\n";

	if( MIGRATE == 0xFFFFFFFF ){
		MIGRATE = 0xFFFFFFFE;
		sys_Prints((unsigned int)&txt);
		return 1; // yes, the task was migrated
	}
	return 0; // no, the task is starting from zero
}


#endif /*__PROD_CONS_H__*/
