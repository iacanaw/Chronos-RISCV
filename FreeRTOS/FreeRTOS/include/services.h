#ifndef __SERVICES_H__
#define __SERVICES_H__

#define MESSAGE_REQUEST					0x00000010U
#define LOST_MESSAGE_REQ                0x00000011U
#define MESSAGE_DELIVERY				0x00000020U
#define MESSAGE_DELIVERY_FINISH         0x00000021U
#define MESSAGE_MIGRATION               0x00000022U
#define MESSAGE_MIGRATION_FINISH        0x00000023U
#define REPOSITORY_WAKEUP               0x00000100U
#define REPOSITORY_APP_INFO             0x00000200U
#define REPOSITORY_APP_INFO_FINISH      0x00000201U
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
#define FIT_PACKET                      0x00000057U
#define FINISH_FIT_PACKET               0x00000058U

#define TASK_STALL_REQUEST              0x00000070U
#define TASK_SEND_STALL                 0x00000071U
#define TASK_STALL_ACK                  0x00000073U
#define TASK_RESUME                     0x00000078U
#define TASK_RESUME_FINISH              0x00000079U

// Migration process
#define TASK_MIGRATION_REQUEST          0x00000600U
#define TASK_MIGRATION_READY            0x00000610U
#define TASK_MIGRATION_FORWARD          0x00000670U
#define TASK_MIGRATION_SENT             0x00000650U
#define TASK_MIGRATION_TASK             0x00000680U
#define TASK_MIGRATION_RECEIVE          0x00000690U
#define TASK_MIGRATION_PIPE             0x00000691U
#define TASK_MIGRATION_PENDING          0x00000692U
#define TASK_MIGRATION_PENDING_FINISH   0x00000693U
#define TASK_MIGRATION_FINISHED         0x00000694U


#define TASK_MIGRATE                    0x00000062U
#define TASK_INFORM_STATUS              0x00000063U
#define TASK_RESTART                    0x00000064U


#define SOLVED                          0x0000FFFFU

#endif
