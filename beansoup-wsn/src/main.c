//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "diag/Trace.h"
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"

#include "FreeRTOS.h"
#include "task.h"


#include "usbd_core.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if_template.h"
#include "usbd_desc.h"

#include "TL_API.h"

#include "datagram_builders.h"
#include "datagram_defs.h"
#include "network_config.h"
#include "data_plane.h"
#include "PriorityQueue.h"
#include "UpstreamTable.h"

#include "cal_def.h"
#include "cal.h"
#include "network_config.h"
#include "net_apis_LL.h"

USBD_HandleTypeDef USBD_Device;
UpstreamTable uTable;

int counter;

// ----------------------------------------------------------------------------
//
// STM32F4 empty sample (trace via STDOUT).
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the STDOUT output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

struct NLS_HandleTypedef hNLS;
#include "timers.h"

uint8_t * packet;
uint8_t  packetDim = 12;
int obama;
PREQ_typedef pippo;
//void finalize();



static void vMainTask( void *pvParameters ){

	char* p = "Ciao\0";
	for( ;; ){
		vTaskDelay( 1000 );
		while(BSP_PB_GetState(BUTTON_KEY)!= 1){
			vTaskDelay(20);
		}
		while(BSP_PB_GetState(BUTTON_KEY)!= 0){
			vTaskDelay(20);
		}

		uint8_t its;
		uint8_t sampleSize;
		uint8_t stepTime;
		uint8_t maxTP;
		uint8_t minTP;

		int i, j;

		float time;
		for (i = 0 ; i < maxTP/stepTime ; i++)
		{
			counter = 0;
			time = xTaskGetTickCount/portTICK_PERIOD_MS;

			for (j = 0 ; j < sampleSize ; j++)
			{
				send_segment_best_async(p, strlen(p)+1);
				vTaskDelay( (1/((i+1)*stepTime + minTP)) /portTICK_PERIOD_MS);
			}

			time -= xTaskGetTickCount/portTICK_PERIOD_MS;

			printf("%f" , counter/time);
		}

#ifdef ROOT
		send_segment_asynchronous(p, strlen(p)+1, 0x0001);
#else
		send_segment_asynchronous(p, strlen(p)+1);
#endif
		BSP_LED_Toggle(LED6);
	}
}


int main(int argc, char* argv[]) {
	// At this stage the system clock should have already been configured
	// at high speed.
	BSP_LED_Init(LED3);
	BSP_LED_Init(LED4);
	BSP_LED_Init(LED5);
	BSP_LED_Init(LED6);
	BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_GPIO);

	USBD_Init(&USBD_Device, &VCP_Desc, 0);
	TL_init();


	USBD_RegisterClass(&USBD_Device, &USBD_CDC);
	USBD_CDC_RegisterInterface(&USBD_Device, &USBD_CDC_Template_fops);
	USBD_Start(&USBD_Device);
	HAL_Delay(4000);



	//inizializzazione strutture dati ecc
//	NLS_LL_enable_if(&hNLS, IF0);  	//SPI
//	NLS_LL_enable_if(&hNLS,IF1);  	//UART
//	NLS_LL_enable_if(&hNLS,IF2);	//UART3

	xTaskCreate( vMainTask, 		/* Pointer to the function that implements the task. */
	   	            "Main",		/* Text name for the task. For debugging only. */
	   	            300,				/* Stack depth in words. */
	   	            NULL,				/* We are not using the task parameter. */
	   	            tskIDLE_PRIORITY+2, /* Task Priority */
	   	            NULL 				/* We are not going to use the task handle. */
	   	            );


	BSP_LED_On(LED3);

    /* Wait for USER Button press before starting the Communication */
    while (BSP_PB_GetState(BUTTON_KEY) != 1)
	{
	}

    /* Wait for USER Button release before starting the Communication */
    while (BSP_PB_GetState(BUTTON_KEY) != 0)
	{
	}
#ifdef ROOT
    printf("CENTRO DI CALCOLO\n\r");
#else
    printf("Nodo con indirizzo %d \n\r",MY_ADDRESS);
#endif


	vTaskStartScheduler();
	for(;;){}

}




void vApplicationTickHook(){

}

void vApplicationStackOverflowHook(){
	printf("Stack overflow\n\r");
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
