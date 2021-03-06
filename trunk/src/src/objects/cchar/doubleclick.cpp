/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

/*!
\brief Character click & doubleclick (mount/unmount and paperdolls)
*/

#include "magic.hpp"
#include "npcs.hpp"
#include "constants.hpp"
#include "menu.hpp"
#include "logsystem.hpp"
#include "settings.hpp"
#include "objects/cobject.hpp"

/*!
\brief single clicks over PC/NPCs
\param client client of player who clicks "this"
*/

void cChar::singleClick( pClient client )
{
	pPC clickedBy = client->currChar();
	if ( ! clickedBy )
	{
		LogMessage("cChar::singleClick couldn't find char serial: %d\n", serial);
		return;
	}

	if ( events[evtChrOnClick] )
	{
		cVariantVector params = cVariantVector(2);
		params[0] = getSerial(); params[1] = clickedBy->getSerial();
		events[evtChrOnClick]->setParams(params);
		events[evtChrOnClick]->execute();
		if ( events[evtChrOnClick]->isBypassed() )
			return;
	}
	if ( nSettings::Server::shouldShowPCNames() || npc || clickedBy == this )
		showLongName( clickedBy, false );
}


/*!
\brief Double clicks over PC/NPCs
\param client client of player who dclicks \c this
*/

void cChar::doubleClick(pClient client)i
{
	if ( ! client ) return;
	pChar clicker = client->currChar();
	if ( ! clicker ) return;

	if( clicker->war && (clicker != this) )
	{
		clicker->attackStuff(this);
		return;
	}

	pContainer pack = getBackpack();

	switch( getId() )
	{
	// Handle pack animals
	case 0x0123	:
	case 0x0124	:
		if ( !npc ) return;
		if ( !pack )
		{
			LogWarning("Pack animal %08x has no backpack!\n",getSerial());
			return;
		}
		if ( getOwner() == clicker || clicker->IsGMorCounselor() )
		{
			clicker->showContainer(pack);
			SetTimerSec( &(clicker->objectdelay), nSettings::Actions::getObjectsDelay() );
			return;
		}

		SetTimerSec( &(clicker->objectdelay), nSettings::Actions::getObjectsDelay() + nSettings::Skills::getSnoopDelay() );
		if ( clicker->checkSkill( skSnooping, 0, 1000 ) )
		{
			client->sysmessage("You successfully snoop the pack animal.");
			clicker->showContainer(pack);
		} else {
			client->sysmessage("You failed to snoop the pack animal.");
			clicker->IncreaseKarma( - nSettings::Reputation::getSnoopKarmaLoss() );
			clicker->modifyFame( - nSettings::Reputation::getSnoopFameLoss() );
			clicker->setCrimGrey( nSettings::Reputation::getSnoopAction() );
		}
		return;

	// Handle humanoids

	case bodyMale		:
	case bodyFemale	:
	case bodyMaleDead	:
	case bodyFemaleDead	:
	case bodyGM	:

		if (npc && npcaitype==NPCAI_PLAYERVENDOR)//PlayerVendors

		{
			client->talk("Take a look at my goods.",0);
			if (pack)
				clicker->showContainer(pack);
		}

		else if ( clicker == this )
			if ( unmountHorse() ) return; //on horse

		//if not on horse, treat ourselves as any other char

		nPackets::Sent::OpenPaperdoll pk(this);
		client->sendPacket(&pk);
		return;
	}
	// Handle others

	/*	// Handle mountable npcs
		case 0x00c8	:
		case 0x00e2	:
		case 0x00e4	:
		case 0x00cc	:
		case 0x00dc	:
		case 0x00d2	:
		case 0x00da	:
		case 0x00db	:
		case 0x007a	:	// LBR	unicorn
		case 0x00bb	:	// 	ridgeback
		case 0x0317	:	//	giant beetle
		case 0x0319	:	//	skeletal mount
		case 0x031a	:	//	swamp dragon
		case 0x031f	:	//	armor dragon*/

	map<int32_t,int32_t>::iterator iter = mountinfo.find(getId());

	if ( iter == mountinfo.end() )
	{
		client->sysmessage("You cannot open monsters paperdolls.");
		return;
	}
	if ( !npc ) return;
	if ( clicker->distFrom(this) > 2 && !clicker->isGM() )
	{
		client->sysmessage("You need to get closer.");
		return;
	}

	//cannot ride animals under polymorph effect

	if ( clicker->isPolymorphed() )
		client->sysmessage("You cannot ride anything under polymorph effect.");
	else if ( clicker->isDead() )
		client->sysmessage("You are dead and cannot do that.");
	else if ( war )
		client->sysmessage("Your pet is in battle right now!");
	else
		clicker->mounthorse(this);
	return;
}
