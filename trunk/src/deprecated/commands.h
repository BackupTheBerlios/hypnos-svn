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
\author Akron
\brief Commands namespace
*/

#ifndef __COMMANDS__
#define __COMMANDS__

#include "typedefs.h"

/*!
\brief Commands related stuff
\author Akron (to namespace)
\since 0.82r3
\date 18/03/2003
\note ported from cCommands class
*/
namespace Commands
{
	extern int32_t cmd_offset;					//!< Command Offset

	void KillSpawn(pClient client, int r);
	void RegSpawnMax(pClient client, int r);
	void RegSpawnNum(pClient client, int r, int n);
	void KillAll(pClient client, int percent, char * sysmsg);
	void AddHere(pClient client, uint16_t id, char z);
	void ShowGMQue(pClient client, int type);
	void Wipe(pClient client);
	void CPage(pClient client, std::string reason);
	void GMPage(pClient client, std::string reason);
	void Command(pClient client, char * );
	void MakeShop(int c);
	void DyeItem(pClient client);
	void DupeItem(pClient client, int i, int amount);
	void Possess(pClient client);
};

#endif
