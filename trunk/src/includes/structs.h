/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Contains misc structs used here and there in the code
\todo complete the documentation of structs
\todo Rename structures ending in _st to starting with s
\deprecated Many structs should be renamed / moved in proper way
*/

#ifndef __STRUCTS_H__
#define __STRUCTS_H__

//! Represent a location on one map
struct Location {
	uint16_t x;		//!< X-Coordinate of the location
	uint16_t y;		//!< Y-Coordinate of the location
	int8_t z;		//!< Z-Coordinate of the location
	int8_t dispz;		//!< Displayed Z of the location
	uint8_t map;		//!< Map the location (if 255 -> every map)
	
	/*!
	\brief Constructor for Location structure
	\param X x of the new location
	\param Y y of the new location
	\param Z z of the new location
	\param DISPZ dispz of the new location
	\param MAP map of the new location
	*/
	inline Location(uint16_t X, uint16_t Y, int8_t Z, int8_t DISPZ = 0, uint8_t MAP = 255)
	{
		x = X; y = Y; z = Z; dispz = DISPZ ? DISPZ : z; map = MAP;
	}

	//! == operator redefinition for Location
	inline bool operator ==(const Location &b)
	{ return x == b.x && y == b.y && z == b.z; }
	
	//! != operator redefinition for Location
	inline bool operator !=(const Location &b)
	{ return x != b.x || y != b.y || z != b.z; }

};

struct location_st
{
 int32_t x1;
 int32_t y1;
 int32_t x2;
 int32_t y2;
 //char region;
 uint8_t region;
};

struct logout_st//Instalog
{
        uint32_t x1;
        uint32_t y1;
        uint32_t x2;
        uint32_t y2;
};

struct path_st {
	uint32_t x;
	uint32_t y;
};

//REPSYS
struct repsys_st
{
	int32_t		murderdecay;
	uint32_t	maxkills;
	int32_t		crimtime;
};

struct resource_st
{
	uint32_t		logs;
	TIMERVAL	logtime;
	TIMERVAL	lograte;//AntiChrist
	uint32_t		logarea;
	int32_t		logtrigger;//AntiChrist
	int32_t		logstamina;//AntiChrist
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
\todo rename to sSkill
\todo use std::string instead of c-strings
*/
struct skill_st
{
 int32_t st;
 int32_t dx;
 int32_t in;
 int32_t unhide_onuse;
 int32_t unhide_onfail;
 int32_t advance_index;
 char madeword[50]; // Added by Magius(CHE)
};

/*!
\todo rename to sAdvance
\todo verify if the int32_t are too much for the values
*/
struct advance_st
{
 uint8_t skill;
 int32_t base;
 int32_t success;
 int32_t failure;
};

/*!
\todo We really need it? It's only a value!
*/
struct spiritspeak_st
{
 TIMERVAL spiritspeaktimer;
};

/*!
\brief struct for manage lag fix
\note extended by AntiChrist and Ripper
*/
struct sSpeed
{
	int32_t nice;
	float itemtime;
	TIMERVAL srtime;
	float npctime;
	float tamednpctime;
	float npcfollowtime;
	float npcaitime;
};


/*!
\brief struct for manage custom titles
\todo Verify if it's actually used or not
*/
struct sTitle
{
	std::string fame;
	std::string skill;
	std::string prowess;
	std::string other;
};

struct tele_locations_st {
	Location destination, origem;
};


/*!
\brief Stat regeneration info
\author Endymion
\since 0.83
\note regeneration rage is in milliseconds
*/
typedef struct {
	TIMERVAL timer; //!< the timer
	uint32_t rate_eff; //!< regeneration rate current
	uint32_t rate_real; //!< regeneration rate real
} regen_st;

#endif
