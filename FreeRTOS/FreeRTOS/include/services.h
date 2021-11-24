#ifndef __SERVICES_H__
#define __SERVICES_H__

#define MESSAGE_REQUEST					0x00000010U
#define MESSAGE_DELIVERY				0x00000020U
#define MESSAGE_DELIVERY_FINISH         0x00000021U
#define REPOSITORY_WAKEUP               0x00000100U
#define REPOSITORY_APP_INFO             0x00000200U
#define TASK_ALLOCATION_SEND            0x00000030U
#define TASK_ALLOCATION_FINISHED        0x00000033U
#define TASK_ALLOCATION_SUCCESS         0x00000035U
//#define TASK_ALLOCATION_FAIL            0x00000036U
#define REPOSITORY_TASK_REQUEST         0x00000300U
#define TASK_START                      0x00000400U
#define TASK_RUN                        0x00000401U
#define TASK_FINISH                     0x00000500U
#define ENERGY_PACKET                   0x00000050U
#define TEMPERATURE_PACKET              0x00000055U
#define FINISH_TEMPERATURE_PACKET       0x00000056U

#define SOLVED                          0x0000FFFFU

#endif
