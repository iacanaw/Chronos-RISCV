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

#define TASK_STALL_REQUEST              0x00000070U
#define TASK_STALL_ACK                  0x00000073U
#define TASK_RESUME                     0x00000079U

// Migration process
#define TASK_MIGRATION_REQUEST          0x00000060U
#define TASK_MIGRATION_READY            0x00000061U
#define TASK_MIGRATION_FORWARD          0x00000067U
#define TASK_MIGRATION_SENT             0x00000065U
#define TASK_MIGRATION_TASK             0x00000068U
#define TASK_MIGRATE                    0x00000062U
#define TASK_INFORM_STATUS              0x00000063U
#define TASK_RESTART                    0x00000064U


#define SOLVED                          0x0000FFFFU

#endif
