  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

/*!
\file
\brief Extraction skills related stuff
\author Endymion
*/

#ifndef __EXTRACTION_SKILLS_H
#define __EXTRACTION_SKILLS_H

#include "sndpkg.h"

class cResource {
public:

	cResource( ) { this->consumed=0; timer=uiCurrentTime; }

	TIMERVAL timer; //!< timer for respawn
	uint32_t consumed; //!< amount of resource consumed

};

typedef cResource* P_RESOURCE;

typedef std::map< uint64_t, cResource > RESOURCE_MAP;

class cResources {


private:

	RESOURCE_MAP resources; //!< all resources

	uint64_t getBlocks( Location location  );
	P_RESOURCE createBlock( Location location );
	void deleteBlock( Location Location );
	bool checkRes( P_RESOURCE res );

public:


	uint32_t		n; //!< number of resource max
	TIMERVAL	time; //!< 
	TIMERVAL	rate; //!< respawn rate
	int32_t		stamina; //!< stamina used
	uint32_t		area_width; //!< resource area width
	uint32_t		area_height; //!< resource area height

	cResources( uint32_t areawidth = 10, uint32_t areaheight = 10 );
	~cResources();

	void setDimArea( uint32_t areawidth, uint32_t areaheight );
	
	P_RESOURCE getResource( Location location );
	void checkResource( Location location, P_RESOURCE& res );
	void decreaseResource( Location location, P_RESOURCE res = NULL );
	bool thereAreSomething( P_RESOURCE res );

	void checkAll();

};


extern cResources ores;


#endif

