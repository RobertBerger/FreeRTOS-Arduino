measure CPU usage:

Or more simply, you can measure the idle time by using an idle task hook
function to add code into the idle task, or a traceTASK_SWITCHED_IN/traceTASK_SWITCHED_OUT
macro to determine when and for how long the idle task is running. The
handle of the idle task being obtainable using the xTaskGetIdleTaskHandle()
API function.
