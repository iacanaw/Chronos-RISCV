// Cardinal ports 
#define EAST 	0
#define WEST 	1
#define NORTH 	2
#define SOUTH 	3
#define LOCAL 	4
#define ND      5 // Not Defined
// Peripheral Ports
#define PERIPH_LOCAL        0x00000000
#define PERIPH_EAST         0x00010000
#define PERIPH_WEST         0x00020000
#define PERIPH_NORTH        0x00030000
#define PERIPH_SOUTH        0x00040000

// NoC Structure configuration
#define N_PORTS 	    5
#define BUFFER_SIZE 8

// Control flags
#define STALL   1
#define GO      0

#define REQ     3
#define ACK     0

// Misc
#define EMPTY       0x00000000
#define HEADER      0xFFFFFFFF
#define SIZE        0xFFFFFFFD
#define SENDTIME    0xFFFFFFFE
#define SERVICE     0xFFFFFFFC
#define PAYLOAD     0x00000000
#define HOPS        0x00000002
#define IN_TIME     0x00000001
#define OUT_TIME    0x00000000

// NoC Dimensions
#define DIM_X 5
#define DIM_Y 5
#define N_PES DIM_X*DIM_Y

//Ticks
#define ITERATION                0x5555
#define ITERATION_OFF            0x0FF
#define ITERATION_ON             0x011
#define ITERATION_BLOCKED_LOCAL  0x022
#define ITERATION_OFF_LOCAL      0x033
#define ITERATION_RELEASED_LOCAL 0X044

//NI
#define TRUE                1
#define FALSE               0
// states
#define IDLE                0x1111
#define WAIT_PE             0x4444
#define TX                  0x00002222U
#define RX                  0x00003333U
#define TX_RX               0x00007777U
#define DONE                0x80005555U
#define HOLD                0xFFFFFFF6U
#define INTERRUPTION_ACK    0x00001234U
#define RESET               0x00008888U
#define BLOCKED             0xF2F2
#define UNBLOCKED           0xF3F3
#define READY               0x6666

#define NI_STATUS_ON        0x0011
#define NI_STATUS_WAITING   0x0022
#define NI_STATUS_OFF       0x00FF
#define NI_STATUS_INTER     0x0F0F
#define NI_STATUS_STARTING  0x0ABC

#define NI_INT_TYPE_CLEAR   0x0000
#define NI_INT_TYPE_TX      0xF222
#define NI_INT_TYPE_RX      0xF333

#define INFORM_ITERATION    0xFFFFFFFFFFFFFFFFULL
#define FROM_IDLE           0x2525
#define FROM_RX             0x3535

// Memory information localization
#define EAST_FLITS_ADDR    0x8FFFFFC8
#define EAST_PACKETS_ADDR  0x8FFFFFC4
#define WEST_FLITS_ADDR    0x8FFFFFC0
#define WEST_PACKETS_ADDR  0x8FFFFFBC
#define NORTH_FLITS_ADDR   0x8FFFFFB8
#define NORTH_PACKETS_ADDR 0x8FFFFFB4
#define SOUTH_FLITS_ADDR   0x8FFFFFB0
#define SOUTH_PACKETS_ADDR 0x8FFFFFAC
#define LOCAL_FLITS_ADDR   0x8FFFFFA8
#define LOCAL_PACKETS_ADDR 0x8FFFFFA4

///// CODE DEFINES
// Arbitration
#define ARBITER_RR      1
#define ARBITER_HERMES  0
#define ARBITER_TTL     0
// Debug logs
#define LOG_OUTPUTFLITS 1
// Thermal stuff
#define USE_THERMAL     1

#if LOG_OUTPUTFLITS
extern int contFlits[N_PORTS];
#endif

extern int myID;
