#ifndef _CRC16_H
#define _CRC16_H

#include "globdefs.h"


#define 	CRC16_TABLE_SIZE		256

uint16_t    check_crc16(uint8_t *, uint16_t);
uint16_t    get_crc16_table(uint8_t);
void   add_crc16(uint8_t*);

#endif /* crc16.h  */
