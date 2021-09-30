#ifndef _COM_CMD_H
#define _COM_CMD_H

#include "globdefs.h"

/*******************************************************************
 *  Входящие команды
 *******************************************************************/
typedef enum {
    UART_CMD_NONE = 0,		// Нет команды
    UART_CMD_COMMAND_PC,	// 0x01 - Перейти в режим работы с PC или ответить Who Are you?
    UART_CMD_GET_DSP_STATUS,	// Получить статус: тестирование, данные с батареи и температуру с давлением
    UART_CMD_GET_COUNTS,	// Выдай счетчики обменов UART
    UART_CMD_GET_ADC_CONST,	// Получить Константы EEPROM
    UART_CMD_INIT_TEST,		// Запуск тестирования
    UART_CMD_DSP_RESET,		// Сброс DSP
    UART_CMD_POWER_OFF,		// Выключение
    UART_CMD_PUMP_ON,		// Включить насос
    UART_CMD_PUMP_OFF,		// вЫключить насос
    UART_CMD_DEV_START,		// Старт измерения
    UART_CMD_DEV_STOP,		// Стоп измерения 
    UART_CMD_GET_DATA,		// Выдать пачку данных
    UART_CMD_ZERO_ALL_EEPROM,	// занулить eeprom
    UART_CMD_SET_DSP_ADDR,	// Установить адрес станции
    UART_CMD_SYNC_RTC_CLOCK,	// Установить время прибора. Синхронизация RTC
    UART_CMD_SET_ADC_CONST,	// Установить / Записать в EEPROM константы
} UART_CMD_ENUM;


#endif				 /* _COM_CMD_H */
