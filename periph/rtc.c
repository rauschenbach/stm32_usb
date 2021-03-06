#include <string.h>
#include <stdio.h>
#include <time.h>
#include "rtc.h"

#define RTC_CLOCK_SOURCE_LSE	/* LSE used as RTC source clock */
static int BACKUP_DATA	= 0x5A0A;
						

/* инициализация RTC */
int rtc_init(void* p)
{
  return 0;
#if 10
    int res = -1;
    RTC_InitTypeDef RTC_InitStructure;
    uint32_t AsynchPrediv = 0, SynchPrediv = 0;


    /* Первый раз запустили? */
   if (RTC_ReadBackupRegister(RTC_BKP_DR0) != BACKUP_DATA) {

	/* Enable the PWR clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	/* Allow access to RTC */
	PWR_BackupAccessCmd(ENABLE);
	RCC_BackupResetCmd(ENABLE);
	RCC_BackupResetCmd(DISABLE);

	/* Enable the LSE OSC */
	RCC_LSEConfig(RCC_LSE_ON);

	/* Wait till LSE is ready */
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) {}


	/* Select the RTC Clock Source */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

	SynchPrediv = 0xFF;
	AsynchPrediv = 0x7F;

	RTC_CalibOutputConfig(RTC_CalibOutput_1Hz);	/* 1 Hz */
	RTC_CalibOutputCmd(ENABLE);

	/* Enable the RTC Clock */
	RCC_RTCCLKCmd(ENABLE);

	/* Wait for RTC APB registers synchronisation */
	RTC_WaitForSynchro();

	/* Configure the RTC data register and RTC prescaler */
	RTC_StructInit(&RTC_InitStructure);
	RTC_InitStructure.RTC_AsynchPrediv = AsynchPrediv;
	RTC_InitStructure.RTC_SynchPrediv = SynchPrediv;
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
        
      	/* Enable BKPSRAM Clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_BKPSRAM, ENABLE);
        RTC_WriteBackupRegister(RTC_BKP_DR0, BACKUP_DATA);

        /* Ставим время */
 	rtc_set_time(p);
        
     	/* Check on RTC init */
	if (RTC_Init(&RTC_InitStructure) == SUCCESS) {
	    res = 0;
	}     
    } else {
	/* Wait for RTC APB registers synchronisation */     
	RTC_WaitForSynchro();       
	RTC_ClearITPendingBit(RTC_IT_WUT);
	EXTI_ClearITPendingBit(EXTI_Line22);
        res = 0;
    }
    return res;
#endif
return 0;
}



/* Установить время и дату */
void rtc_set_time(void *p)
{
    
#if 10  
    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;
    struct tm *t0;

    if (p != NULL) {
	t0 = (struct tm *) p;
      
	time.RTC_H12 = RTC_HourFormat_24;
	time.RTC_Hours = t0->tm_hour;
	time.RTC_Minutes = t0->tm_min;
	time.RTC_Seconds = t0->tm_sec;

        /* Configure the RTC time register */
        RTC_SetTime(RTC_Format_BIN, &time);


	date.RTC_Date = t0->tm_mday;
	date.RTC_Month = t0->tm_mon + 1;
	date.RTC_Year = ((int) t0->tm_year % 100);
	RTC_SetDate(RTC_Format_BIN, &date);
    }
#endif    
}

/* Получить время */
void rtc_get_time(void *p)
{
    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;
    struct tm *t0;

    if (p != NULL) {
	t0 = (struct tm *) p;

	RTC_GetDate(RTC_Format_BIN, &date);
	RTC_GetTime(RTC_Format_BIN, &time);

	t0->tm_hour = time.RTC_Hours;
	t0->tm_min = time.RTC_Minutes;
	t0->tm_sec = time.RTC_Seconds;
	t0->tm_mon = date.RTC_Month - 1;
	t0->tm_mday = date.RTC_Date;
	t0->tm_year = date.RTC_Year + 100;
    }
}


/* Get the current Alarm */
void rtc_get_alarm(void *p)
{
    RTC_AlarmTypeDef RTC_AlarmStructure;
    struct tm *t0;


    if (p != NULL) {
	t0 = (struct tm *) p;

	RTC_GetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);
	t0->tm_hour = RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours;
	t0->tm_min = RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes;
	t0->tm_sec = RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds;
    }
}


/* Set the current Alarm */
void rtc_set_alarm(void *p)
{
    RTC_AlarmTypeDef RTC_AlarmStructure;
    struct tm *t0;

    if (p != NULL) {
	t0 = (struct tm *) p;

	/* Disable the Alarm A */
	RTC_AlarmCmd(RTC_Alarm_A, DISABLE);

	RTC_AlarmStructure.RTC_AlarmTime.RTC_H12 = RTC_H12_AM;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours = t0->tm_hour;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = t0->tm_min;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = t0->tm_sec;

	/* Set the Alarm A */
	RTC_AlarmStructure.RTC_AlarmDateWeekDay = 0x31;
	RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
	RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;


	/* Configure the RTC Alarm A register */
	RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);

	/* Enable the RTC Alarm A Interrupt */
	RTC_ITConfig(RTC_IT_ALRA, ENABLE);

	/* Enable the alarm  A */
	RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
    }
}

/* RTC Alarm A Interrupt Configuration */
void set_alarm_irq(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;

    /* EXTI configuration */
    EXTI_ClearITPendingBit(EXTI_Line17);
    EXTI_InitStructure.EXTI_Line = EXTI_Line17;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Enable the RTC Alarm Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

}

/**
  * @brief  Configures the RTC Alarm.
  * @param  None
  * @retval None
  */
void rtc_alarm_config(void)
{
  EXTI_InitTypeDef EXTI_InitStructure;
  RTC_AlarmTypeDef RTC_AlarmStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* EXTI configuration */
  EXTI_ClearITPendingBit(EXTI_Line17);
  EXTI_InitStructure.EXTI_Line = EXTI_Line17;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  /* Enable the RTC Alarm Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
 
  /* Set the alarmA Masks */
  RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_All;
  RTC_SetAlarm(RTC_Format_BCD, RTC_Alarm_A, &RTC_AlarmStructure);
  
  /* Set AlarmA subseconds and enable SubSec Alarm : generate 8 interripts per Second */
  RTC_AlarmSubSecondConfig(RTC_Alarm_A, 0xFF, RTC_AlarmSubSecondMask_SS14_5);

  /* Enable AlarmA interrupt */
  RTC_ITConfig(RTC_IT_ALRA, ENABLE);
  
  RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
}
