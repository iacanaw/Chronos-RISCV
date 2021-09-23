#ifndef _PACKET_H_
#define _PACKET_H_

#include "message.h"

#define PKT_HEADER_SIZE     13
#define PKT_SERVICE_SIZE	PKT_HEADER_SIZE-2 // minus two because we do not count the both address & size flits
#define PACKET_MAX_SIZE     MSG_SIZE + PKT_HEADER_SIZE
////////////////////////////////////////////////////////////
// PIPE
#define PIPE_SIZE           4 			// Defines the PIPE size
#define PIPE_OCCUPIED       1			
#define PIPE_FREE           -1			
#define PIPE_TRANSMITTING   -2			
#define PIPE_SYS_HOLDER		0xFEFEFEFE	// System as holder of one pipe position
#define PIPE_FULL			0xFDFDFDFD 	// Returns this when the pipe is full
////////////////////////////////////////////////////////////
// SERVICE PACKET SLOTS
#define SPS_EMPTY           1
#define SPS_OCCUPIED        0
//////////////////////////////
//////////////////////////////


typedef struct {
	union{
		unsigned int flit0;
		unsigned int header;				// ** Its the first flit of packet, keeps the target NoC router
	};
	union{
		unsigned int flit1;
		unsigned int payload_size;			// ** Stores the number of flits that forms the remaining of packet
	};
	union{
		unsigned int flit2;
		unsigned int service;				// ** Store the packet service code (see services.h file)
	};
	union{
		unsigned int flit3;
		unsigned int application_id;		// ** Informs the application ID
		unsigned int task_id;				// ** Store the task ID
	};
	union{
		unsigned int flit4;
		unsigned int aplication_period;		// ** Informs the GlobalMaster the period of a given application (the time in miliseconds that the application must wait after its finish to start again)
		unsigned int task_txt_size;			// ** Informs the task .txt size (in words) 
		unsigned int producer_task;			// ** Informs the receiver the producer task
	};
	union{
		unsigned int flit5;
		unsigned int application_executions;// ** Informs the GlobalMaster the number of times that a given application must execute
		unsigned int task_bss_size;			// ** Informs the task .bss size (in words)
		unsigned int destination_task;		// ** Holds the destination taskID
		unsigned int producer_task_id;		// ** Holds the producer taskID
	};
	union{
		unsigned int flit6;
		unsigned int application_n_tasks;	// ** Informs the GlobalMaster the number of tasks that a given application have
		unsigned int task_start_point;		// ** Informs the main() start point for that task
	};
	union{
		unsigned int flit7;
		unsigned int task_app_id;			// ** Informs to witch application that task is from
	};
	union{
		unsigned int flit8;			
	};
	union{
		unsigned int flit9;
	};
	union{
		unsigned int flit10;
	};
	union{
		unsigned int flit11;
	};
	union{
		unsigned int flit12;
		unsigned int task_dest_addr;		// ** Informs the destination address for a given task
	};
	//Add new variables here ...
} ServiceHeader;

typedef struct{
	unsigned int	status;						// Stores this packet status
	unsigned int 	holder;
	ServiceHeader 	header;
	Message 		msg;
} MessagePacket;

typedef struct{
	unsigned int	status;						// Stores this packet status
	unsigned int 	holder;	
	ServiceHeader 	header;
} ServicePacket;

volatile MessagePacket MessagePipe[PIPE_SIZE];
volatile ServicePacket ServicePipe[PIPE_SIZE];

// Initialize the PIPE, setting the status of each slot to FREE
void API_PipeInitialization();

// Returns a free Message slot 
unsigned int API_GetMessageSlot();

// Returns a free Service slot
unsigned int API_GetServiceSlot();

// Clear one PipeSlot after send it ('typeSlot' is a combination of type (message/service) & slot)
void API_ClearPipeSlot(unsigned int typeSlot);

//API_createPacket(Message *theMessage, unsigned int dest_task_id, unsigned int service);

#endif