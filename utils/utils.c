/******************************************************************************
 * Функции перевода дат, проверка контрольной суммы т.ж. здесь 
 * Все функции считают время от начала Эпохи (01-01-1970)
 * Все функции с маленькой буквы
 *****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <arm_math.h>
#include "systick.h"
#include "utils.h"



int fputc(int ch, FILE * f)
{
	VCP_DataTx((uint8_t *) & ch, 1);
	return ch;
}


/* Выдапть число миллисекунд */
s64 get_msec_ticks(void)
{
	return get_msex();
}


/* Выдапть число секунд */
long get_sec_ticks(void)
{
	return get_msex() / 1000;
}


/* переопределение printf  */
int PRINTF(char* fmt,...)
{
    int r = 0;
    char str[256];
    va_list p_vargs;		/* return value from vsnprintf  */

    va_start(p_vargs, fmt);
    r = vsnprintf(str, sizeof(str), fmt, p_vargs);
    va_end(p_vargs); 

    r = printf(str);

    return r;		
}

/**
 * Переводит секунды (time_t) с начала Эпохи в формат struct tm
 */
int sec_to_tm(long ls, struct tm *time)
{
    struct tm *tm_ptr;

    if ((int) ls != -1 && time != NULL) {
	tm_ptr = gmtime((time_t *) & ls);

	/* Записываем дату, что получилось */
	memcpy(time, tm_ptr, sizeof(struct tm));
	return 0;
    } else
	return -1;
}

/**
 * Время struct tm в секунды 
 */
long tm_to_sec(struct tm *tm_time)
{
    long r;

    /* Делаем секунды */
    r = mktime(tm_time);
    return r;			/* -1 или нормальное число  */
}


