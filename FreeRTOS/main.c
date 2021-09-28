#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include <stdio.h>
#include "hw_platform.h"
#include "core_uart_apb.h"
#include "chronos.h"
#include "system_call.h"
#include "globalMaster.h"

const char * g_hello_msg = "\r\nFreeRTOS Example\r\n";

/* A block time of zero simply means "don't block". */
#define mainDONT_BLOCK						( 0UL )

/******************************************************************************
 * CoreUARTapb instance data.
 *****************************************************************************/
UART_instance_t g_uart;
/*-----------------------------------------------------------*/

static void vUartAliveTask( void *pvParameters );
static void GlobalManagerTask( void *pvParameters );

/*
 * FreeRTOS hook for when malloc fails, enable in FreeRTOSConfig.
 */
void vApplicationMallocFailedHook( void );

/*
 * FreeRTOS hook for when FreeRtos is idling, enable in FreeRTOSConfig.
 */
void vApplicationIdleHook( void );

/*
 * FreeRTOS hook for when a stack overflow occurs, enable in FreeRTOSConfig.
 */
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );

/*
 * Stores the processor address
 */
unsigned int ProcessorAddr;

/*-----------------------------------------------------------*/

int main( void )
{
	char str[20];
    PLIC_init();

    /**************************************************************************
    * Initialize CoreUART with its base address, baud value, and line
    * configuration.
    *************************************************************************/
    UART_init(&g_uart, COREUARTAPB0_BASE_ADDR, BAUD_VALUE_115200,
             (DATA_8_BITS | NO_PARITY) );
    
	/**************************************************************************
    * Initialize the Chronos platform stuff
    *************************************************************************/
	UART_polled_tx_string( &g_uart, (const uint8_t *)"\n Chronos platform initializing... \n" );
	Chronos_init();
	UART_polled_tx_string( &g_uart, (const uint8_t *)"\n This terminal belongs to the address: " );
	myItoa(ProcessorAddr, str, 16);
    UART_polled_tx_string( &g_uart, (const uint8_t *)str); UART_polled_tx_string( &g_uart, (const uint8_t *)"\n");
	
	//AppsToMap = 0;
	
	if (ProcessorAddr == 0x0000){
		UART_polled_tx_string( &g_uart, (const uint8_t *)"\n This processor is the Global Master: \n" );
		/* Create the GlobalManager task */
		xTaskCreate( GlobalManagerTask, "GlobalMaster", 1000, NULL, (configMAX_PRIORITIES - 1), NULL );
	} else {
		UART_polled_tx_string( &g_uart, (const uint8_t *)"\n This processor is a Slave: \n" );
		
		/* Create the two test tasks. */
		xTaskCreate( vUartAliveTask, "Alive", 1000, NULL, 0, NULL );
	}	

	/* Start the kernel.  From here on, only tasks and interrupts will run. */
	vTaskStartScheduler();

	/* Exit FreeRTOS */
	return 0;
}

/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	task.  It is essential that code added to this hook function never attempts
	to block in any way (for example, call xQueueReceive() with a block time
	specified, or call vTaskDelay()).  If the application makes use of the
	vTaskDelete() API function (as this demo application does) then it is also
	important that vApplicationIdleHook() is permitted to return to its calling
	function, because it is the responsibility of the idle task to clean up
	memory allocated by the kernel to any task that has since been deleted. */
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/

static void vUartAliveTask( void *pvParameters )
{
	( void ) pvParameters;
	char str[20];
	unsigned int i;
	for( i = 0 ;; i++ ){
		myItoa(ProcessorAddr, str, 16);
		UART_polled_tx_string( &g_uart, (const uint8_t *)str);
		UART_polled_tx_string( &g_uart, (const uint8_t *)" PE is alive - " );
		myItoa(i, str, 10);
		UART_polled_tx_string( &g_uart, (const uint8_t *)str);
		UART_polled_tx_string( &g_uart, (const uint8_t *)" -\r\n" );
	    vTaskDelay(1);
	}
}

/*-----------------------------------------------------------*/

static void GlobalManagerTask( void *pvParameters ){
	( void ) pvParameters;
	int tick;
	// Initialize the priority vector with the pattern policy
	GeneratePatternMatrix();

	// Initialize the System Tiles Info
	API_TilesReset();

	// Initialize the applications vector
    API_ApplicationsReset();

	// Informs the Repository that the GLOBALMASTER is ready to receive the application info
	API_RepositoryWakeUp();

	for(;;){
		tick = xTaskGetTickCount();
		printsv("GlobalMasterActive", tick);
		UART_polled_tx_string( &g_uart, (const uint8_t *)"GlobalMasterRoutine...\r\n" );

		// Checks if there is some task to allocate...
		API_AllocateTasks(tick);




		vTaskDelay(1);
	}
}
