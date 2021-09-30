#include "status.h"

/* Здесь описан статус и функции для него */
static DEV_STATUS_PACK status;

/**
 *  Инициализация в начале работы
 */ 
void status_init_first(void)
{
 status.main_status = 0x03;
 status.self_test0 = 0x1c;
 status.adc_error = -1;
 status.reset_cause = 1;
 status.freq = 164000000;
}

/**
 * Выдать статус для других модулей
 */
void status_get(DEV_STATUS_PACK * pack)
{
    memcpy(pack, &status, sizeof(DEV_STATUS_PACK));
}


/**
 * Выдать время прибора
 */
int32_t status_get_time(void)
{
    return status.gs_time;
}

/**
 * Установить время прибора
 * Записать в RTC
 */
void status_set_time(int32_t t)
{
    status.gs_time = t;		// надо установить rtc!             
}


void status_sync_time(int t)
{
   status.gs_time = t;		// надо установить rtc!             
   status.main_status &= ~0x01; /* Убираем нет времени */	 
}


/**
 * Выдать температуру прибора
 */
int16_t status_get_temp(void)
{
    return status.temperature;
}


/**
 * Установить температуру прибора (датчиком)
 */
void status_set_temp(int16_t t)
{
    status.temperature = t;
}



/**
 * Выдать относительную влажность воздуха
 */
int16_t status_get_humidity(void)
{
    return status.humidity;
}


/**
 * Установить относительную влажность воздуха (датчиком)
 */
void status_set_humidity(int16_t t)
{
    status.humidity = t;
}

/**
 * Давление выдать
 */
int32_t status_get_pressure(void)
{
    return status.pressure;	/* Давление кПа */
}

/**
 * Давление записать (датчиком)
 */
void status_set_pressure(int32_t t)
{
    status.pressure = t;
}


/**
 *
 * Ток потребления выдать. мА 
 */
int16_t status_get_current(void)
{
    return status.batt_curr;
}

/**
 * Ток потребления записать. мА 
 */
void status_set_current(int16_t t)
{
    status.batt_curr = t;
}

/**
 * Напряжение батареи питания. мВ
 */
int16_t status_get_power(void)
{
    return status.batt_pow;
}

/**
 * Напряжение батареи питания. мВ
 */
void status_set_power(int16_t t)
{
    status.batt_pow = t;
}


/**
 *  Включить насос - пока включить лампу
 */
void status_pump_on(void)
{
    status.devices |= 0x1;
    led_on(LED6);
}

/**
 *  вЫключить насос - пока вЫключить лампу
 */
void status_pump_off(void)
{
    status.devices &= ~0x1;
    led_off(LED6);
}

/* сброс CPU */
void status_dev_reset(void)
{
     SCB->AIRCR = 0x05FA0004; 
}