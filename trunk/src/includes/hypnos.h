/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Base Emulator's Functions
*/

#ifndef __HYPNOS_H_
#define __HYPNOS_H_

#include "typedefs.h"
#include "libhypnos/muls/mulfiles.h"

void loadServer();
void shutdownServer();

extern nMULFiles::fTiledataLand *tiledataLand;
extern nMULFiles::fTiledataStatic *tiledataStatic;

#endif
