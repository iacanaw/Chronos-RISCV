#include "../FreeRTOS/FreeRTOS/include/message.h"
#include "../FreeRTOS/FreeRTOS/include/system_call.h"
//#include "../FreeRTOS/FreeRTOS/system_call.c"

#define IS_MIGRATION    0xFFFFFFFF
#define MIGRATION_RESET 0xFFFFFFFE

volatile static int MIGRATE = MIGRATION_RESET;
static int VAR_ITERATION = 0xFFFFFFFD;

// https://www.geeksforgeeks.org/write-memcpy/
void memcpy(void *dest, void *src, unsigned int n){
   // Typecast src and dest addresses to (char *)
   char *csrc = (char *)src;
   char *cdest = (char *)dest;
  
   // Copy contents of src[] to dest[]
   for (int i=0; i<n; i++)
       cdest[i] = csrc[i];
}

// Default function to check if a migration is required!
int checkMigration(){

    static char txt[] = "Starting Migration Process!\n";
	/*static char txt2[] = "No Migration Process!\n";*/

	if ( MIGRATE == IS_MIGRATION ){
		sys_Prints((unsigned int)&txt);
		sys_Migrate(); // the task stops here
	} /*else {
		sys_Prints((unsigned int)&txt2);
	}*/
	return 0; // this return will never happen if the task will migrate
}

// Default function to check if this is a migrated task.
int isMigration(){
	static char txt[] = "Restarting after Migration Process!\n";
	static char txt2[] = "Starting task from Start Point!\n";

	if( MIGRATE == IS_MIGRATION ){
		MIGRATE = MIGRATION_RESET;
		sys_Prints((unsigned int)&txt);
		return 1; // yes, the task was migrated
	} else {
		sys_Prints((unsigned int)&txt2);
		return 0; // no, the task is starting from zero
	}
}