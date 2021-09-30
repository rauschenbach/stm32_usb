/******************************************************************************
 * ������� �������� ���, �������� ����������� ����� �.�. ����� 
 * ��� ������� ������� ����� �� ������ ����� (01-01-1970)
 * ��� ������� � ��������� �����
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


/* ������� ����� ����������� */
s64 get_msec_ticks(void)
{
	return get_msex();
}


/* ������� ����� ������ */
long get_sec_ticks(void)
{
	return get_msex() / 1000;
}


/* ��������������� printf  */
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
 * ��������� ������� (time_t) � ������ ����� � ������ struct tm
 */
int sec_to_tm(long ls, struct tm *time)
{
    struct tm *tm_ptr;

    if ((int) ls != -1 && time != NULL) {
	tm_ptr = gmtime((time_t *) & ls);

	/* ���������� ����, ��� ���������� */
	memcpy(time, tm_ptr, sizeof(struct tm));
	return 0;
    } else
	return -1;
}

/**
 * ����� struct tm � ������� 
 */
long tm_to_sec(struct tm *tm_time)
{
    long r;

    /* ������ ������� */
    r = mktime(tm_time);
    return r;			/* -1 ��� ���������� �����  */
}


