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
	unsigned int header;				//** Its the first flit of packet, keeps the target NoC router
	unsigned int payload_size;			//** Stores the number of flits that forms the remaining of packet
	unsigned int service;				//** Store the packet service code (see services.h file)
	union {								//--Union
		   unsigned int producer_task;  // Informs the producer task id
		   unsigned int task_ID;        // Informs the task id
		   unsigned int app_ID;         // Informs the application id
		   unsigned int total_inst;     //
	};
	union {								//--Union
	   unsigned int consumer_task;
	   unsigned int cluster_ID;
	   unsigned int master_ID;
	   unsigned int hops;
	   unsigned int period;
	   unsigned int router_injection; 
	   unsigned int power_estimator_error;
	};
	unsigned int source_PE;				// Store the packet source PE address
	unsigned int timestamp;				// Store the packet timestamp, filled automatically by send_packet function
	unsigned int transaction;			// Unused field
	union {								//--Union
		unsigned int msg_lenght;
		unsigned int resolution;
		unsigned int priority;
		unsigned int deadline;
		unsigned int pkt_latency;
		unsigned int stack_size;
		unsigned int requesting_task;
		unsigned int released_proc;
		unsigned int app_task_number;
		unsigned int app_descriptor_size;
		unsigned int allocated_processor;
		unsigned int requesting_processor;
		unsigned int real_sampling_window; //envia o sampling window real do pacote
		unsigned int power_mode_change;
	};
	union {								//--Union
		unsigned int tasks_of_same_app;
		unsigned int troughput_deadline;
		unsigned int pkt_size;
		unsigned int data_size;
		unsigned int energy_total; 
		unsigned int dyn_voltage; //Varia a tensão
		unsigned int insert_request;
	};
	union {								//--Union
		unsigned int code_size;
		unsigned int max_free_procs;
		unsigned int execution_time;
		unsigned int dyn_freq; // varia a frequencia
		unsigned int router_congestion; 
		unsigned int power_mode;
	};
	union {								//--Union
		unsigned int bss_size;
		unsigned int request_size;
		unsigned int DVFS_control; // varia a frequencia
		unsigned int cpu_slack_time;
		unsigned int idle_PEs_into_cluster;
	};
	union {								//--Union
		unsigned int initial_address;
		unsigned int program_counter;
		unsigned int utilization;
		unsigned int energy_leakage; 
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