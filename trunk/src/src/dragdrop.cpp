/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
// dragdrop.cpp: implementation of dragging and dropping
// cut from NoX-Wizard.cpp by Duke, 23.9.2000
//////////////////////////////////////////////////////////////////////

/* 									CHANGELOG
	--------------------------------------------------------------------------------------------------------------------
	Date		Developer	Description
	--------------------------------------------------------------------------------------------------------------------
 	20-02-2002  	Sparhawk 	Fixed disappearing items after dragging them on paperdoll, set checks in item_bouncex()
					from id1 >= 0x40 to id1 < 0x40
					Removed obsolete item checks in wear_item()
					Added bounce_item call when non gm player tries to drag item on paperdoll of gm.
	24-02-2002	Sparhawk	Fixed	compile error on void return values in CheckWhereItem and cleaned code
	15-03-2002	Sparhawk	Optimized abstract container size() in loops

*/


#include "common_libs.h"
#include "network.h"
#include "debug.h"
#include "basics.h"
#include "sndpkg.h"
#include "speech.h"
#include "itemid.h"
#include "bounty.h"
#include "set.h"
#include "npcai.h"
#include "nxw_utils.h"
#include "weight.h"
#include "data.h"
#include "boats.h"
#include "trade.h"
#include "map.h"
#include "inlines.h"
#include "skills.h"
#include "party.h"

typedef struct _PKGx08
{
//0x08 Packet
//Drop Item(s) (14 bytes)
//* uint8_t cmd
//* uint8_t[4] item id
	long Iserial;
//* uint8_t[2] xLoc
	short TxLoc;
//* uint8_t[2] yLoc
	short TyLoc;
//* uint8_t zLoc
	signed char TzLoc;
//* uint8_t[4] Move Into (FF FF FF FF if normal world)
	long Tserial;
} PKGx08;



static void Sndbounce5( NXWSOCKET socket )
{
	if ( socket >= 0 && socket < now)
	{
		uint8_t bounce[2]= { 0x27, 0x00 };
		bounce[1] = 5;
		Xsend(socket, bounce, 2);
	}
}
