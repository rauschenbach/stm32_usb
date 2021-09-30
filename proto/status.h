#ifndef _STATUS_H
#define _STATUS_H

#include "globdefs.h"
#include "main.h"


void status_pump_on(void);
void status_pump_off(void);


void status_init_first(void);
void status_get(DEV_STATUS_PACK*);
int32_t status_get_time(void);
void status_set_time(int32_t);
void status_sync_time(int);
int16_t status_get_temp(void);
void status_set_temp(int16_t);
int16_t status_get_humidity(void);
void status_set_humidity(int16_t);
s32 status_get_pressure(void);
void status_set_pressure(s32);
int16_t status_get_current(void);
void status_set_current(int16_t);
int16_t status_get_power(void);
void status_set_power(int16_t);

void status_dev_reset(void);

#endif /* status.h */
