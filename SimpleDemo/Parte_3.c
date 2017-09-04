/*
 * @file Parte_3.c
 * @author Asson Leandro <leoasson@gmail.com>
 */

/*This project is based on the use of the tool "Tracealyzer" applied on FreeRTOS.
 *  The objective is to be able to appreciate when the tasks are suspended and
 *  restarted because of the different priorities.
 *  This part of the project consists basically of three tasks.
 *  The first task periodically generates every 1000ms an integer value between 20 and 40
 *  (simulating to be the temperature measurement taken from a sensor) and adds it to the queue.
 *  The second task is to generate an aperiodic character string, then it is added to the same queue.
 *  The last task, of higher priority than the previous ones, obtains the elements of the queue
 *  and sends them to the PC via serial port.
 *  The Tracealyzer tool made it possible to appreciate the moments in which the tasks
 *  were suspended and resumed, the periodicity of the tasks, the execution times,
 *  among other characteristics. Being able to fulfill the objective of the project.
 *  "Tracealyzer" can be downloaded at https://percepio.com/tz/
*/

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "serial.h"
#include <time.h>
#include <stdlib.h>

/*
 * Prioridades (en función de IDLE que está seteada en 0).
 */
#define mainQUEUE_RECEIVE_TASK_PRIORITY		(tskIDLE_PRIORITY + 3)
#define	mainQUEUE_UART_TASK_PRIORITY		(tskIDLE_PRIORITY + 2)
#define	mainQUEUE_SEND_TASK_PRIORITY		(tskIDLE_PRIORITY + 1)

//Frecuencia con que se envían datos (1000 ms).
#define mainQUEUE_SEND_FREQUENCY_MS			(1000 / portTICK_RATE_MS)

//Funciones manejadoras de las tareas.
static void prvQueueReceiveTask( void *pvParameters );
static void prvQueueSendTask( void *pvParameters );
static void prvUARTTask(void *pvParameters);

//Estructura de datos a transmitir.
typedef enum INFO_TYPE{
	TempType_t,
	UserType_t
} eInfoType_t;
typedef struct TASK_INFO{
	eInfoType_t eInfoType;
	void* pvData;
} xTaskInfo;

//Definición de la cola.
static xQueueHandle xQueue = NULL;

//--------------------------------------------------------------------------------

int main(void)
{
	//Se inicializa el UART3 de la placa a 115200 baudios.
	Serial_begin(3, 115200);

	//Inicia el traicing de la memoria mediante TraceAnalizer
	vTraceEnable(TRC_START);

	//Se crea la cola indicando la cantidad de elementos maximos.
	xQueue = xQueueCreate(1, sizeof(struct xTaskInfo *));

	if(xQueue != NULL)
	{
		//Se inician las tareas
		xTaskCreate(prvQueueReceiveTask, (signed char*) "Rcv_Task",
				    configMINIMAL_STACK_SIZE, NULL, mainQUEUE_RECEIVE_TASK_PRIORITY, NULL);
		xTaskCreate(prvQueueSendTask, (signed char*) "Snd_Task",
				    configMINIMAL_STACK_SIZE, NULL, mainQUEUE_SEND_TASK_PRIORITY, NULL);
		xTaskCreate(prvUARTTask, (signed char*) "UART_Task",
				    configMINIMAL_STACK_SIZE, NULL, mainQUEUE_UART_TASK_PRIORITY, NULL);

		//Se lanza el scheduler.
		vTaskStartScheduler();
	}
}
//--------------------------------------------------------------------------------

// Función de la tarea "Rcv_Task".
// Encargada de transmitir los datos ingresados en la cola mediante UART
// Esta tarea tiene la maxima prioridad
static void prvQueueReceiveTask(void *pvParameters)
{
	static unsigned short usReceivedValue;
	static char* cReceivedValue;
	static xTaskInfo* Data;
	for( ;; ){
	// Se bloquea la tarea hasta que haya elemento en la cola
	// Luego se comprueba que tipo de dato es y realiza el envio.
		xQueueReceive(xQueue, &Data, portMAX_DELAY);

		if(Data->eInfoType == TempType_t)
		{
			usReceivedValue = *((unsigned short*) Data->pvData);
			Serial_write(3, usReceivedValue);
		}
		else if (Data->eInfoType == UserType_t)
		{
			cReceivedValue = ((char*) Data->pvData);
			//Si es de tipo UserType envia caracter por caracter.
			while(*cReceivedValue != '\0'){
				Serial_write(3, *cReceivedValue);
				cReceivedValue++;}
		}}
}
//--------------------------------------------------------------------------------

/*
 * Función handler de la tarea "UART_Task".
 * Coloca en la cola estructuras que simulan ser texto ingresado
 * por un usuario y que posteriormente se quiere transmitir.
 */
static void prvUARTTask(void *pvParameters)
{
	portTickType uiMAX = (5000 / portTICK_RATE_MS);
	portTickType uiMIN = (500 / portTICK_RATE_MS);
	srand(time(NULL));
	for(;;)
	{
		// Se duerme la tarea durante un tiempo aleatorio
		// generando la aperiodicidad de la tarea.
		portTickType random = (rand() % (uiMAX - uiMIN)) + uiMIN;
		vTaskDelay(random);
		int16_t lss = Serial_read(3);

		// se carga la estructura con los datos
		if(lss != 0 && lss != -1)
		{
		static xTaskInfo Data;
		static xTaskInfo* ptr = &Data;
		ptr->eInfoType = UserType_t;
		int r = rand() % 3;
		if(r == 0)
			ptr->pvData = "tp4";
		else if(r == 1)
			ptr->pvData = "Sistemas";
		else if (r == 2)
			ptr->pvData = "sistemasOperativos";
		// se envia a la cola para ser transmitido
		xQueueSend(xQueue, (void *)&ptr, 0);
		}

	}
}
//--------------------------------------------------------------------------------

//Función  de la tarea "Snd_Task".
//Coloca en la cola estructuras que simulan ser temperaturas
//capturadas por un sensor para ser transmitidas.
static void prvQueueSendTask(void *pvParameters)
{
	static unsigned short usValueToSend;
	portTickType xNextWakeTime = xTaskGetTickCount();

	srand(time(NULL));

	for( ;; )
	{
		// se duerme la tarea durante 1 segundo.
		vTaskDelayUntil(&xNextWakeTime, mainQUEUE_SEND_FREQUENCY_MS);

		//Genera la temperatura
		usValueToSend = (rand() % 20) + 20;

		//Creo la estructura
		static xTaskInfo Data;
		static xTaskInfo* ptr = &Data;
		//Asigno los valores a la estructura.
		ptr->eInfoType = TempType_t;
		ptr->pvData = &usValueToSend;
		//envio el dato a la cola.
		xQueueSend(xQueue, (void *)&ptr, 0);
	}
}
