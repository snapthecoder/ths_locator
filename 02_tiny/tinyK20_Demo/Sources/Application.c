/*
 * Application.c
 *      Author: Erich Styger
 */
#include "Platform.h"
#include "Application.h"
#include "LED1.h"
#include "FAT1.h"
#include "CLS1.h"
#include "WAIT1.h"
#include "UTIL1.h"
#include "FRTOS1.h"
#include "CDC1.h"
#include "Shell.h"
#include "Nano6e.h"

#if PL_USE_HW_RTC
  #include "RTC1.h"
#endif

/*LED Blinking Task - "controller is working"*/
static void led_task(void *param) {
  (void)param;
  for(;;) {
    LED1_Neg();
    vTaskDelay(pdMS_TO_TICKS(1000));
  } /* for */
}

void APP_Run(void) {
  SHELL_Init();

#if PL_HAS_STATUS_LED
  if (xTaskCreate(led_task, "Led", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL) != pdPASS) {
    for(;;){} /* error! probably out of memory */
  }
#endif
  //start the FreeRTOS Scheduler
  vTaskStartScheduler();
}

