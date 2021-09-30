#ifndef _GLOBDEFS_H
#define _GLOBDEFS_H

#include <signal.h>
#include <stdint.h>
#include <inttypes.h>
#include <time.h>

/* Сколько пакетов в пачке  */
#define 	NUM_ADS131_PACK	100

#define		SEC_IN_HOUR		3600

/* Включения для 504 блекфина, не 506!!!!  */
#ifndef _WIN32			/* Embedded platform */
#include <float.h>



#else				/* windows   */
#include <windows.h>
#include <tchar.h>
#endif


/*
#ifndef s8
#define s8 char
#endif

#ifndef c8
#define c8 char
#endif

*/

#ifndef u64
#define u64 uint64_t
#endif


#ifndef s64
#define s64 int64_t
#endif


/* Длинное время */
#ifndef	time64
#define time64	int64_t
#endif


#ifndef _WIN32			/* Embedded platform */

/* long double не поддержываеца  */
#ifndef f32
#define f32 float
#endif

#ifndef bool
#define bool uint8_t
#endif


#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

#else
/* long double не поддержываеца  */
#ifndef f32
#define f32 float
#endif

#endif


#ifndef IDEF
#define IDEF static inline
#endif

/* На этот адрес будет отвечать наше устройство */
#define 	BROADCAST_ADDR	0xff


#define 	ADC_CHAN			4	/* число каналов  */
#define		MAGIC				0x4b495245

/* Делители */
#define		TIMER_NS_DIVIDER	(1000000000UL)
#define		TIMER_US_DIVIDER	(1000000)
#define		TIMER_MS_DIVIDER	(1000)

/* Причины сброса прибора - перечислены здесь. Из запишем в eeprom  */
#define		CAUSE_POWER_OFF		0x12345678	/* Выключение питания */
#define		CAUSE_EXT_RESET		0xabcdef90	/* Внешний ресет */
#define		CAUSE_BROWN_OUT		0xaa55aa55	/* Снижение питания */
#define		CAUSE_WDT_RESET		0x07070707	/* WDT reset (во время регистрации) */
#define		CAUSE_NO_LINK		0xE7E7E7E7	/* Нет связи - самовыключение прибора */
#define		CAUSE_UNKNOWN_RESET	0xFFFFFFFF	/* Неизвестная причина-выдернули питание */


/*******************************************************************************
 * Состояние устройств для описания State machine
 *******************************************************************************/
typedef enum {
    DEV_POWER_ON_STATE = 0,
    DEV_CHOOSE_MODE_STATE = 1,
    DEV_INIT_STATE,
    DEV_TUNE_WITHOUT_GPS_STATE,	/*Запуск таймера БЕЗ GPS */
    DEV_GET_3DFIX_STATE,
    DEV_TUNE_Q19_STATE,		/* Первичная настройка кварца 19 МГц */
    DEV_SLEEP_AND_DIVE_STATE,	/* Погружение и сон перед началом работы */
    DEV_WAKEUP_STATE,
    DEV_REG_STATE,
    DEV_FINISH_REG_STATE,
    DEV_BURN_WIRE_STATE,
    DEV_SLEEP_AND_POPUP_STATE,
    DEV_WAIT_GPS_STATE,
    DEV_HALT_STATE,
    DEV_COMMAND_MODE_STATE,
    DEV_POWER_OFF_STATE,
    DEV_ERROR_STATE = -1	/* Авария */
} DEV_STATE_ENUM;


/* Главный статус */
#define		STATUS_NO_TIME		1
#define		STATUS_NO_CONST		2
#define		STATUS_NO_SD		4
#define		STATUS_CMD_ERROR	8
#define		STATUS_DEV_DEFECT	16
#define		STATUS_MEM_OVERFLOW	32
#define		STATUS_DEV_TEST		64
#define		STATUS_DEV_RUN		128

/** 
 * Статус и ошибки устройств на отправление 
 * Короткий статус (main_status) посылается при ошибке (только 1 байта + 1 len + 2 CRC16)
 * сместим на позицию для совместимости
 */
#pragma pack(4)
typedef struct {
    uint8_t main_status;	/* главный: нет времени, нет констант, нет SD, ошибка в команде, неисправен, переполнение памяти, тестирование, остановлен */
    uint8_t self_test0;		/* Самотестирование и ошибки0: 0 - часы, 1 - датчик температуры, 2 - датчик давления, 3 - EEPROM, 4 - карта SD, 5 - flash... */
    uint8_t self_test1;		/* прочие устройства: 0 - насос */
#define devices self_test1
    uint8_t reset_cause;	/* Причина предыдущего сброса */

    uint32_t adc_error;		/* ERROR ошибки 8 АЦП. Потом распишем */

    uint32_t eeprom;		/* Статус EEPROM. Выставленный бит - ошибка к/л параметра */

    int32_t gs_time;		/* Время прибора. Секнуды времени UNIX */


    int16_t temperature;	/* Температура внутри прибора * 10 */
    int16_t humidity;		/* Относительная влажность прибора * 10 % */

    uint32_t pressure;		/* Давление кПа */

    int16_t batt_curr;		/* Ток потребления. мА */
    int16_t batt_pow;		/* Напряжение батареи питания. мВ */
    uint32_t freq;		/* частота проца */

    uint32_t rsvd[8];
} DEV_STATUS_PACK;


/**
 * Описывает когда случился таймаут карты и прочие ошибки 
 */
typedef struct {
    uint32_t cmd_error;
    uint32_t read_timeout;
    uint32_t write_timeout;
    uint32_t any_error;
} SD_CARD_ERROR_STRUCT;


/**
 * Имя и адрес прибора.
 */
typedef struct {
    uint8_t addr;			/* Адрес платы */
    uint8_t ver;			/* Версия ПО: 1, 2, 3, 4 итд */
    uint8_t rev;			/* Ревизия ПО 0, 1, 2 итд */
    uint8_t res0;			/* Резерв */
    uint32_t time;			/* Время компиляции */
} DEV_ADDR_PACK;



/**
 * Счетчики обменов по УАРТ
 */
typedef struct {
    uint32_t rx_pack;		/* Принятые пакеты */
    uint32_t rx_cmd_err;		/* Ошибка в команде */
    uint32_t rx_stat_err;		/* Ошибки набегания, кадра (без стопов) и пр */
    uint32_t rx_crc_err;		/* Ошибки контрольной суммы */
    uint32_t tx_pack;		/* переданные пакеты */
    uint32_t tx_err;
} UART_COUNTS_PACK;


#endif				/* globdefs.h */
