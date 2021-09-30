#ifndef _UTILS_H
#define _UTILS_H

#include <arm_math.h>
#include <stdio.h>
#include "globdefs.h"


int fputc(int ch, FILE * f);
s64    get_msec_ticks(void);
long   get_sec_ticks(void);
int PRINTF(char*,...);
int  sec_to_tm(long, struct tm *);
long tm_to_sec(struct tm *);


#endif				/* utils.h */

