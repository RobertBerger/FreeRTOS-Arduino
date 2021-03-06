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

/* TODO: update FreeRTOS version? xTimerHandle not defined? */

/* Arduino includes */
#include "Arduino.h"

/* FreeRTOS.org includes. */
#include "FreeRTOS_ARM.h"

/* Demo includes. */
#include "basic_io_arm.h"

/* The task function. */
void vTaskFunction1( void *pvParameters );
void vTaskFunction2( void *pvParameters );

/* Define the strings that will be passed in as the task parameters.  These are
defined const and off the stack to ensure they remain valid when the tasks are
executing. */
const char *pcTextForTask1 = "Task 1 is running\r\n";
const char *pcTextForTask2 = "Task 2 is running\t\n";

/* pin to measure for jitter */
const uint8_t outputPin = 3;

/* how many software timers do we want */
#define NUM_TIMERS 1

/* how many times timer needs to expire before stopping it */
#define MAX_EXPIRY_COUNT_BEFORE_STOPPING 10

/* An array to hold handles to the created timers. */
xTimerHandle xTimers[ NUM_TIMERS ];

/* An array to hold a count of the number of times each timer expires. */
long lExpireCounters[ NUM_TIMERS ] = { 0 };

/*-----------------------------------------------------------*/

/* simple function which toggles an outputPin */
void toggle ( void )
{
   static boolean toggle = false;

  /* toggle the flag */
  toggle = !toggle;

  /* toggle the I/O pin here */
  digitalWrite(outputPin,toggle);
}

/*-----------------------------------------------------------*/

/* Define a callback function that will be used by multiple timer instances.
The callback function does nothing but count the number of times the
associated timer expires, and stop the timer once the timer has expired
10 times. */

void vTimerCallback( xTimerHandle pxTimer )
{
  long lArrayIndex;
  const long xMaxExpiryCountBeforeStopping = MAX_EXPIRY_COUNT_BEFORE_STOPPING;

  /* Optionally do something if the pxTimer parameter is NULL. */
  configASSERT( pxTimer );

  /* toggle outputPin */
  toggle();

  /* If the timer has expired n times then stop it from running. */
  if( lExpireCounters[ lArrayIndex ] == xMaxExpiryCountBeforeStopping )
  {
    vPrintString("vTimerCallback timer expired n times - stop");
    /* Do not use a block time if calling a timer API function from a
       timer callback function, as doing so could cause a deadlock! */
    xTimerStop( pxTimer, 0 );
  }
}

/*-----------------------------------------------------------*/

void setup( void )
{
	long x;

	Serial.begin(9600);

   /* Create then start some timers.  Starting the timers before the scheduler
      has been started means the timers will start running immediately that
      the scheduler starts. */

   for( x = 0; x < NUM_TIMERS; x++ )
   {
       xTimers[ x ] = xTimerCreate(  "Timer",        /* Just a text name, not used by the kernel. */
                                     ( 100 * x ),    /* The timer period in ticks. */
                                     pdTRUE,         /* The timers will auto-reload themselves when they expire. */
                                     ( void * ) x,   /* Assign each timer a unique id equal to its array index. */
                                     vTimerCallback  /* Each timer calls the same callback when it expires. */
                                   );

       if( xTimers[ x ] == NULL )
       {
           /* The timer was not created. */
       }
       else
       {
           /* Start the timer.  No block time is specified, and even if one was
           it would be ignored because the scheduler has not yet been
           started. */
           if( xTimerStart( xTimers[ x ], 0 ) != pdPASS )
           {
               /* The timer could not be set into the Active state. */
           }
       }
   }

  /* Create the first task at priority 1... */
  xTaskCreate( vTaskFunction1, "Task 1", 200, (void*)pcTextForTask1, 1, NULL );

  /* ... and the second task at priority 2.  The priority is the second to
  last parameter. */
  xTaskCreate( vTaskFunction2, "Task 2", 200, (void*)pcTextForTask2, 2, NULL );

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
  TickType_t xLastWakeTime;

  /* The string to print out is passed in via the parameter.  Cast this to a
  character pointer. */
  pcTaskName = ( char * ) pvParameters;

  /* The xLastWakeTime variable needs to be initialized with the current tick
  count.  Note that this is the only time we access this variable.  From this
  point on xLastWakeTime is managed automatically by the vTaskDelayUntil()
  API function. */
  xLastWakeTime = xTaskGetTickCount();

  /* As per most tasks, this task is implemented in an infinite loop. */
  for( ;; )
  {
    /* Print out the name of this task. */
    vPrintString( pcTaskName );

    /* We want this task to execute exactly every 250 milliseconds.  As per
    the vTaskDelay() function, time is measured in ticks, and the
    portTICK_PERIOD_MS constant is used to convert this to milliseconds.
    xLastWakeTime is automatically updated within vTaskDelayUntil() so does not
    have to be updated by this task code. */
    vTaskDelayUntil( &xLastWakeTime, ( 250 / portTICK_PERIOD_MS ) );
  }
}

/*-----------------------------------------------------------*/

void vTaskFunction2( void *pvParameters )
{
  char *pcTaskName;
  TickType_t xLastWakeTime;

  /* The string to print out is passed in via the parameter.  Cast this to a
  character pointer. */
  pcTaskName = ( char * ) pvParameters;

  /* The xLastWakeTime variable needs to be initialized with the current tick
  count.  Note that this is the only time we access this variable.  From this
  point on xLastWakeTime is managed automatically by the vTaskDelayUntil()
  API function. */
  xLastWakeTime = xTaskGetTickCount();

  /* Print out the name of this task. */
  vPrintString( pcTaskName );


  /* As per most tasks, this task is implemented in an infinite loop. */
  for( ;; )
  {
    /* Print out the name of this task. */
    vPrintString( pcTaskName );

    /* We want this task to execute exactly every 250 milliseconds.  As per
    the vTaskDelay() function, time is measured in ticks, and the
    portTICK_PERIOD_MS constant is used to convert this to milliseconds.
    xLastWakeTime is automatically updated within vTaskDelayUntil() so does not
    have to be updated by this task code. */

    /* periodic delay */
    vTaskDelayUntil( &xLastWakeTime, ( 250 / portTICK_PERIOD_MS ) );
  }
}

/*-----------------------------------------------------------*/

/* Application tick hook */
#if 0
	#ifdef __cplusplus
	extern "C"{
	#endif // __cplusplus
	void vApplicationTickHook( void )
	  {
		 toggle();
	  }
	#ifdef __cplusplus
	} // extern "C"
	#endif // __cplusplus
#endif

/*-----------------------------------------------------------*/

/* vApplicationIdleHook calls loop below */
void loop() {}

/*-----------------------------------------------------------*/
