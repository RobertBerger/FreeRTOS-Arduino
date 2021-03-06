/*
  FreeRTOS.org V5.0.4 - Copyright (C) 2003-2008 Richard Barry.

  This file is part of the FreeRTOS.org distribution.

  FreeRTOS.org is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  FreeRTOS.org is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with FreeRTOS.org; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  A special exception to the GPL can be applied should you wish to distribute
  a combined work that includes FreeRTOS.org, without being obliged to provide
  the source code for any proprietary components.  See the licensing section
  of http://www.FreeRTOS.org for full details of how and when the exception
  can be applied.

    ***************************************************************************
    ***************************************************************************
    *                                                                         *
    * SAVE TIME AND MONEY!  We can port FreeRTOS.org to your own hardware,    *
    * and even write all or part of your application on your behalf.          *
    * See http://www.OpenRTOS.com for details of the services we provide to   *
    * expedite your project.                                                  *
    *                                                                         *
    ***************************************************************************
    ***************************************************************************

  Please ensure to read the configuration and relevant port sections of the
  online documentation.

  http://www.FreeRTOS.org - Documentation, latest information, license and
  contact details.

  http://www.SafeRTOS.com - A version that is certified for use in safety
  critical systems.

  http://www.OpenRTOS.com - Commercial support, development, porting,
  licensing and training services.
*/

/* Arduino includes */
#include <Arduino.h>
/* FreeRTOS.org includes. */
#include "FreeRTOS_ARM.h"

/* Demo includes. */
#include "basic_io_arm.h"

/* from the task */
/*
 * USE_SERIAL 0, TOGGLE_IN_TASK 0
 *
 */
#define USE_SERIAL 0
#define TOGGLE_IN_TASK 0
#define TOGGLE_IN_TICK_HOOK 0

/* The task function. */
void vTaskFunction1( void *pvParameters );
void vTaskFunction2( void *pvParameters );

/* Define the strings that will be passed in as the task parameters.  These are
defined const and off the stack to ensure they remain valid when the tasks are
executing. */
const char *pcTextForTask1 = "Task 1 is running\r\n";
const char *pcTextForTask2 = "Task 2 is running\t\n";

/* pin to measure for jitter */
const uint8_t outputPin = 45;

/*-----------------------------------------------------------*/

/* simple function which toggles an outputPin */
void toggle ( void )
{
  volatile static boolean toggle = false;

  /* toggle the flag */
  toggle = !toggle;

  /* toggle the I/O pin here */
  digitalWrite(outputPin,toggle);
}

/*-----------------------------------------------------------*/

void setup( void )
{
   /* here we want to use the serial */
   Serial.begin(9600);

  /* Create the first task at priority 1... */
  xTaskCreate( vTaskFunction1, "Task 1", 200, (void*)pcTextForTask1, 1, NULL );

  /* ... and the second task at priority 2.  The priority is the second to
  last parameter. */
  xTaskCreate( vTaskFunction2, "Task 2", 200, (void*)pcTextForTask2, 4, NULL );

  /* initialize the output Pin */
  pinMode(outputPin, OUTPUT);
  digitalWrite(outputPin, HIGH);

  /* Start the scheduler so our tasks start executing. */
  vTaskStartScheduler();

  for( ;; );
}

/*-----------------------------------------------------------*/

void vTaskFunction1( void *pvParameters )
{
  char *pcTaskName;
  TickType_t xLastWakeTime1;

  /* The string to print out is passed in via the parameter.  Cast this to a
  character pointer. */
  pcTaskName = ( char * ) pvParameters;

  /* The xLastWakeTime variable needs to be initialized with the current tick
  count.  Note that this is the only time we access this variable.  From this
  point on xLastWakeTime is managed automatically by the vTaskDelayUntil()
  API function. */
  xLastWakeTime1 = xTaskGetTickCount();

  /* As per most tasks, this task is implemented in an infinite loop. */
  for( ;; )
  {
    /* Print out the name of this task. */
#if USE_SERIAL > 0
	  vPrintString( pcTaskName );
#endif

    /* We want this task to execute exactly every 250 milliseconds.  As per
    the vTaskDelay() function, time is measured in ticks, and the
    portTICK_PERIOD_MS constant is used to convert this to milliseconds.
    xLastWakeTime is automatically updated within vTaskDelayUntil() so does not
    have to be updated by this task code.  */
    vTaskDelayUntil( &xLastWakeTime1, ( 250 / portTICK_PERIOD_MS ) );
  }
}

/*-----------------------------------------------------------*/

void vTaskFunction2( void *pvParameters )
{
  char *pcTaskName;
  TickType_t xLastWakeTime2;

  /* The string to print out is passed in via the parameter.  Cast this to a
  character pointer. */
  pcTaskName = ( char * ) pvParameters;

  /* The xLastWakeTime variable needs to be initialized with the current tick
  count.  Note that this is the only time we access this variable.  From this
  point on xLastWakeTime is managed automatically by the vTaskDelayUntil()
  API function. */
  xLastWakeTime2 = xTaskGetTickCount();

  /* Print out the name of this task. */
#if USE_SERIAL > 0
  vPrintString( pcTaskName );
#endif

  /* As per most tasks, this task is implemented in an infinite loop. */
  for( ;; )
  {
    /* We want this task to execute exactly every 1 tick.  As per
    the vTaskDelay() function, time is measured in ticks, and the
    portTICK_PERIOD_MS constant is used to convert this to milliseconds.
    xLastWakeTime is automatically updated within vTaskDelayUntil() so does not
    have to be updated by this task code. */

    /* delay as short and periodic as possible */
    vTaskDelayUntil( &xLastWakeTime2, ( 1 ) );

#if TOGGLE_IN_TASK > 0
    /* toggle outputPin */
    toggle();
#endif
  }
}

/*-----------------------------------------------------------*/

/* Application tick hook */
#if TOGGLE_IN_TICK_HOOK > 0
	/* Application tick hook */
	#ifdef __cplusplus
	extern "C"{
	#endif // __cplusplus
		void vApplicationTickHookMala( )
		  {
			/* toggle outputPin */
			toggle(); /* in the system tick ISR - as quickly as possible */
		  }
	#ifdef __cplusplus
	} // extern "C"
	#endif // __cplusplus
#endif

	void vApplicationTickHookMine(  )
	  {
		/* toggle outputPin */
		toggle(); /* in the system tick ISR - as quickly as possible */
	  }

/*-----------------------------------------------------------*/

/* vApplicationIdleHook calls loop below */
void loop() {}

/*-----------------------------------------------------------*/
