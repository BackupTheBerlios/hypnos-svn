/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Area Management stuff (used by spawn, region ecc ecc)
\author Endymion
*/

#ifndef _AREA_H
#define _AREA_H

#include "common_libs.h"

typedef struct {
	uint16_t x1;
	uint16_t y1;
	uint16_t x2;
	uint16_t y2;
} Area;

typedef map< uint32_t, Area > AREA_DB;
typedef AREA_DB::iterator AREA_ITER;


class cAreas {

private:

	AREA_DB allareas;
	uint32_t currarea;

public:

	friend class cSpawns;

	cAreas();
	~cAreas();

	uint32_t insert( Area& newarea, uint32_t index = UINVALID32 );
	void loadareas();


};

extern cAreas* Areas;


#endif
