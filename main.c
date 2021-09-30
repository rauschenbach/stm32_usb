/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include "systick.h"
#include "status.h"
#include "led.h"
#include "rtc.h"

/* Task priorities. */
#define MAIN_TEST_PRIORITY				(tskIDLE_PRIORITY + 1)


/**
 * «адача - просто моргание светодиодом
 * —тавим врем€ и мен€ем остальные данные в статусе
 */
static void task3(void *p)
{
    long sec;

    while (1) {
        //rtc_get_time(&t);
        //sec = tm_to_sec(&t);
        sec = status_get_time();
        sec++;       
        status_set_time(sec);
        
        status_set_temp(210 + rand() % 10);
        status_set_humidity(710 + rand() % 40);
        status_set_pressure(101234 + rand() % 500);
        status_set_power(12134 + rand() % 100);
        status_set_current(124 + rand() % 10);
        
       // printf("sec = %d\n",sec);
        led_toggle(LED4);
	vTaskDelay(1000);        
    }
}



/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
    //  systick_init();  
    led_init();
    vcp_init();
    rtc_init(NULL);
    status_init_first();

    /* —оздадим задачу дл€ разбора данных */
    xTaskCreate(task3, "task3", configMINIMAL_STACK_SIZE, NULL, MAIN_TEST_PRIORITY + 2, NULL);

    /* Start the scheduler. */
    vTaskStartScheduler();

    /* Will only get here if there was not enough heap space to create the  idle task. */
    return 0;
}



/* Decrement the TimingDelay variable */
void vApplicationTickHook(void)
{

}

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
void vApplicationMallocFailedHook(void)
{
    taskDISABLE_INTERRUPTS();
    for (;;);
}

/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
 * to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
 * task.  It is essential that code added to this hook function never attempts
 * to block in any way (for example, call xQueueReceive() with a block time
 * specified, or call vTaskDelay()).  If the application makes use of the
 * vTaskDelete() API function (as this demo application does) then it is also
 * important that vApplicationIdleHook() is permitted to return to its calling
 * function, because it is the responsibility of the idle task to clean up
 * memory allocated by the kernel to any task that has since been deleted. */
void vApplicationIdleHook(void)
{
}


    /* Run time stack overflow checking is performed if
       configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
       function is called if a stack overflow is detected. */
void vApplicationStackOverflowHook(xTaskHandle pxTask, signed char *pcTaskName)
{
    (void) pcTaskName;
    (void) pxTask;
    taskDISABLE_INTERRUPTS();
    for (;;);
}
