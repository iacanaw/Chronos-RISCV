#ifndef __REPOSITORY_H__
#define __REPOSITORY_H__

#define NUM_APPS 0
#define INFO_SIZE 10
#define MAX_TASKS 15
#define MASTER_ADDR 0x0000
#define BIG_CODE 0
// Application INFO
static unsigned int appInfo[NUM_APPS][INFO_SIZE];
static unsigned int tasksInfo[NUM_APPS][MAX_TASKS][INFO_SIZE];
static unsigned int tasksCode[NUM_APPS][MAX_TASKS][BIG_CODE];
#endif /*__REPOSITORY_H__*/
