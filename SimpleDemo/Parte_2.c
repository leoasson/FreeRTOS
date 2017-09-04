/*
 * @file Parte_2.c
 * @author Asson Leandro <leoasson@gmail.com>
 */

/*This project is based on the use of the tool "Tracealyzer" applied on FreeRTOS.
 *  The objective is to be able to appreciate when the tasks are suspended and
 *  restarted because of the different priorities.
 *  This part of the project consists basically of three tasks.
 *  This part of the project basically consists of two simple tasks.
 *  One that periodically adds elements to the queue and the other that brings out these elements.
 *  The Tracealyzer tool made it possible to appreciate the moments in which the tasks
 *  were suspended and resumed, the periodicity of the tasks, the execution times,
 *  among other characteristics. Being able to fulfill the objective of the project.
 *  "Tracealyzer" can be downloaded at https://percepio.com/tz/
*/


#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <time.h>
#include <stdlib.h>

//Prioridades (en función de IDLE que está seteada en 0).
#define mainQUEUE_RECEIVE_TASK_PRIORITY		(tskIDLE_PRIORITY + 2)
#define	mainQUEUE_SEND_TASK_PRIORITY		(tskIDLE_PRIORITY + 1)

//Pin de led.
#define mainLED_BIT 						(22)

//Frecuencia con que se envían datos (200 ms).
#define mainQUEUE_SEND_FREQUENCY_MS			(200 / portTICK_RATE_MS)

//Manejadores de tareas
static void prvQueueReceiveTask( void *pvParameters );
static void prvQueueSendTask( void *pvParameters );

//Funcion de parpadeo de led.
static void prvBlinkLED( void );

static xQueueHandle xQueue = NULL;

//--------------------------------------------------------------------------------

int main2(void)
{
	//configuracion pin 22
	LPC_PINCON->PINSEL1	&= (~( 3 << 12 ));
	LPC_GPIO0->FIODIR |= (1 << mainLED_BIT);

	//Inicia el traicing de la memoria mediante TraceAnalizer
	vTraceEnable(TRC_START);

	//Se crea la cola indicando la cantidad de elementos maximos.
	xQueue = xQueueCreate(1, sizeof(unsigned long));

	if(xQueue != NULL)
	{
		//Se crean las tareas
		xTaskCreate(prvQueueReceiveTask, (signed char*) "Rcv_Task", configMINIMAL_STACK_SIZE, NULL, mainQUEUE_RECEIVE_TASK_PRIORITY, NULL);
		xTaskCreate(prvQueueSendTask, (signed char*) "Snd_Task", configMINIMAL_STACK_SIZE, NULL, mainQUEUE_SEND_TASK_PRIORITY, NULL);

		//lanzo el scheduler
		vTaskStartScheduler();
	}
}
//--------------------------------------------------------------------------------

/*
 * Agrega un valor a la cola cada 200 ms.
 */
static void prvQueueSendTask(void *pvParameters)
{
	const unsigned long ulValueToSend = 2UL;
	portTickType xNextWakeTime = xTaskGetTickCount();

	/* Se bloquea la tarea hasta que se cumpla el tiempo, luego de
	* eso se agrega a la cola.
	*/
	for( ;; )
	{
		//vTaskDelay((unsigned int) mainQUEUE_SEND_FREQUENCY_MS);
		vTaskDelayUntil(&xNextWakeTime, mainQUEUE_SEND_FREQUENCY_MS);
		xQueueSend(xQueue, &ulValueToSend, 0);
	}
}
//--------------------------------------------------------------------------------
static void prvQueueReceiveTask(void *pvParameters){
	unsigned long ulReceivedValue;
	unsigned long ulValue = 0UL;// Contador
	for( ;; )
	{
		/* Bloquea la tarea hasta que haya un elemento en la cola. Por cada elemento
		 * agregado en la cola realiza esta funcion.
		 */
		xQueueReceive(xQueue, &ulReceivedValue, portMAX_DELAY);
	//cuando el contador esta entre [0-10], parpadea con QUEUE_SEND_FREQUENCY_MS
			prvBlinkLED();
	//cuando el contador esta entre [10-20], parpadea con el doble QUEUE_SEND_FREQUENCY_MS
			if(ulValue > 9UL && ulValue < 20UL ){
				vTaskDelay((unsigned int) mainQUEUE_SEND_FREQUENCY_MS/2);
				prvBlinkLED();
			}
	//cuando el contador esta entre [20-30], parpadea con el triple QUEUE_SEND_FREQUENCY_MS
			if(ulValue > 19UL && ulValue < 30UL){
				vTaskDelay((unsigned int) mainQUEUE_SEND_FREQUENCY_MS/3);
				prvBlinkLED();
				vTaskDelay((unsigned int) mainQUEUE_SEND_FREQUENCY_MS/3);
				prvBlinkLED();
			}
			if(ulValue == 30UL) // Resetear contador.
			{
				ulValue = 0UL;
			}
		ulValue += 1UL;
	}
}
//--------------------------------------------------------------------------------
/*
 * @brief Niega el valor del led conectado al pin 22.
 * @return
 */
static void prvBlinkLED(void)
{
	unsigned long ulLEDState;
	ulLEDState = LPC_GPIO0->FIOPIN;

	LPC_GPIO0->FIOCLR = ulLEDState & (1 << mainLED_BIT);
	LPC_GPIO0->FIOSET = (( ~ulLEDState ) & ( 1 << mainLED_BIT ));
}

//--------------------------------------------------------------------------------
