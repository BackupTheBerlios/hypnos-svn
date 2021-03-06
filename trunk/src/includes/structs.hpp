/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Contains misc structs used here and there in the code
\todo complete the documentation of structs
\todo Rename structures ending in _st to starting with s
*/

#ifndef __STRUCTS_H__
#define __STRUCTS_H__

#include "common_libs.hpp"
#include "enums.hpp"
#include "libhypnos/hypstl/string.hpp"

//! Represent a location on one map
struct sLocation {
	uint16_t x;		//!< X-Coordinate of the location
	uint16_t y;		//!< Y-Coordinate of the location
	int8_t z;		//!< Z-Coordinate of the location
	int8_t dispz;		//!< Displayed Z of the location
	uint8_t map;		//!< Map the location (if 255 -> every map)
	
	inline sLocation()
	{ x = 0; y = 0; z = 0; dispz = 0; map = 0; }
	
	/*!	
	\brief Constructor for sLocation structure
	\param X x of the new location
	\param Y y of the new location
	\param Z z of the new location
	\param DISPZ dispz of the new location
	\param MAP map of the new location
	*/
	inline sLocation(uint16_t X, uint16_t Y, int8_t Z, int8_t DISPZ = 0, uint8_t MAP = 255)
	{
		x = X; y = Y; z = Z; dispz = DISPZ ? DISPZ : z; map = MAP;
	}

	//! == operator redefinition for sLocation
	inline bool operator ==(const sLocation &b) const
	{ return x == b.x && y == b.y && z == b.z; }
	
	//! != operator redefinition for sLocation
	inline bool operator !=(const sLocation &b) const
	{ return x != b.x || y != b.y || z != b.z; }
	
	//! + operator to add values to the coords in a pass
	inline sLocation operator +(const sLocation &b) const
	{ return sLocation( x + b.x, y + b.y, z + b.z ); }

	//! - operator to remove values to the coords in a pass
	inline sPositionOffset operator -(const sLocation &b) const
	{ return sPositionOffset( x - b.x, y - b.y, z - b.z ); }

	//! + operator to add values to the coords in a pass
	inline sLocation operator +(const sPositionOffset &b) const
	{ return sLocation( x + b.x, y + b.y, z + b.z ); }

	//! - operator to remove values to the coords in a pass
	inline sLocation operator -(const sPositionOffset &b) const
	{ return sLocation( x - b.x, y - b.y, z - b.z ); }

	//! Converts a location (x,y,z) into a point (x,y)
	inline operator sPoint() const
	{ return sPoint(x, y); }
	
	void move(Direction dir, uint8_t paces);
};

/*!
\brief Item in a container

\see cpacket.h
*/
struct sContainerItem
{
	//! Default null constructor
	sContainerItem() { }
	sContainerItem(pItem item);
	
	uint32_t serial;	//!< Serial of the item
	uint16_t id;		//!< ID of the item
	uint16_t amount;	//!< Amount of items
	uint16_t x, y;		//!< Position of the item
	uint16_t color;		//!< Color of the item
};

struct resource_st
{
	uint32_t logs;
	uint32_t logtime;
	uint32_t lograte;//AntiChrist
	uint32_t logarea;
	int32_t logtrigger;//AntiChrist
	int32_t logstamina;//AntiChrist
};

struct creat_st
{
	int32_t basesound;
	uint8_t soundflag;
	uint8_t who_am_i;
	int32_t icon;
};

/*!
\todo rename to sUnitile
*/
struct unitile_st
{
 int8_t basez;
 uint8_t type; // 0=Terrain, 1=Item
 uint16_t id;
 uint8_t flag1;
 uint8_t flag2;
 uint8_t flag3;
 uint8_t flag4;
 int8_t height;
 uint8_t weight;
} PACK_NEEDED;

/*!
\todo We really need it? It's only a value!
*/
struct spiritspeak_st
{
 uint32_t spiritspeaktimer;
};

/*!
\brief struct for manage custom titles
\todo Verify if it's actually used or not
*/
struct sTitle
{
	string fame;
	string skill;
	string prowess;
	string other;
};

struct sTeleLocations {
	sLocation destination, origem;
};


/*!
\brief Stat regeneration info
\author Endymion
\since 0.83
\note regeneration rage is in milliseconds
*/
struct sRegen {
	uint32_t timer; //!< the timer
	uint32_t rate_eff; //!< regeneration rate current
	uint32_t rate_real; //!< regeneration rate real
};

/*!
\brief Struct for a target callback
\author Chronodt
This struct is used as parameter of a target callback. It contains the data
about what was targeted by the client.
*/
struct sTarget
{
	sLocation loc;			//!< xyz clicked
	uint16_t id;			//!< visual id of object clicked
	pSerializable source;		//!< pSerializable to char or item that called the target
	pSerializable clicked;		//!< Object (char or item) clicked
	processTarget callback; 	//!< note: callback function must be static or global!
	TargetType type;		//!< item, char or location target?
};


#endif
