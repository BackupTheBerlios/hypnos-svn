/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Target Related Stuff
*/

#ifndef __TARGET_H__
#define __TARGET_H__

#include "common_libs.h"
#include "structs.h"
#include "inlines.h"

class cTarget {

private:
	static uint32_t serial_current;

protected:
	sLocation loc;
	uint16_t model;
	pSerializable clicked;		//!< Object (char or item) clicked

public:
	uint32_t serial;
	bool type;

	uint32_t buffer[4];
	std::string buffer_str[4];
	processTarget code_callback;

	cTarget( bool selectLocation=true );
	virtual ~cTarget();

	void send( pClient client );
	void receive( pClient client );
	virtual bool isValid();
	virtual void error( pClient client );

	const sLocation cTarget::getPosition() const
	{ return loc; }

	const pSerializable cTarget::getClicked() const
	{ return clicked; }

	const uint16_t cTarget::getModel() const
	{ return model; }
};

class cObjectTarget : public cTarget {

public:

	cObjectTarget();
	~cObjectTarget();

	virtual bool isValid();
	virtual void error( pClient client );
};

class cCharTarget : public cObjectTarget {

public:

	cCharTarget();
	~cCharTarget();

	virtual bool isValid();
	virtual void error( pClient client );
};

class cItemTarget : public cObjectTarget {
	
public:

	cItemTarget();
	~cItemTarget();

	virtual bool isValid();
	virtual void error( pClient client );
};

class cLocationTarget : public cTarget {

public:

	cLocationTarget();
	~cLocationTarget();

	virtual bool isValid();
	virtual void error( pClient client );
};

class cTarget
{
private:
	pChar	m_pc;
	int	m_pcSerial;
	pItem	m_pi;
	int	m_piSerial;
	sLocation loc;
	void   init(sLocation l);
	void   init(pChar pc);
	void   init(pItem pi);

public:
	//!creates a target loc from a character
	sTarget(pChar pc) { init(pc); }
	//!creates a target loc from an item
	sTarget(pItem pi) { init(pi); }
	//!creates a target loc from a target net packet
	sTarget(pTarget pp);
	//!creates a target loc from a xyz position in the map
	sTarget(sLocation l) { init(l); }

	//!recalculates item&char from their serial
	void revalidate();

	//!gets the targetted char if any, NULL otherwise
	inline pChar getChar() { return m_pc; }
	
	//!gets the targetted item if any, NULL otherwise
	inline pItem getItem() { return m_pi; }
	
	//! Gets the location of this target
	inline sLocation getLocation() const
	{ return loc; }
	
	//!gets the XYZ location of this target location
	inline void getXYZ(uint16_t& x, uint16_t& y, int8_t& z) { x = loc.x; y = loc.y; z = loc.z; }
	
	//!extends a pItem data to pChar and x,y,z
	void extendItemTarget();
};

#endif
