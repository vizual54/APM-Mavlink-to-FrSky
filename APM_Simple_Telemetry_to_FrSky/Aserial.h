/* ============================================================
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================*/

// Author: Mike Blandford

struct t_sportData
{
	struct t_sportData *next ;
	uint8_t data[7] ;
	uint8_t dataLock ;
	uint8_t serialSent ;
} ;

void setNewData( struct t_sportData *pdata, uint16_t id, uint32_t value ) ;
void initSportUart( struct t_sportData *pdata ) ;

extern uint8_t DataSent;