/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
#include "common_libs.hpp"
#include "magic.hpp"
#include "sregions.hpp"
#include "sndpkg.hpp"
#include "commands.hpp"
#include "data.hpp"
#include "map.hpp"
#include "inlines.hpp"
#include "skills/skills.hpp"
#include "misc.hpp"

namespace npcs {	//Luxor


//<Anthalir>
static void SpawnGuard(pChar pc, pChar pc_i, sLocation where)
{
	if ( ! pc || ! pc_i ) return;

	int t;
	if (!(region[pc_i->region].priv&0x01)) return;
	if( pc->isDead() || pc_i->isDead() ) return; //AntiChrist
	
	if ( ! SrvParms->guardsactive || pc->isInvul() )
		return;

	t=RandomNum(1000,1001);
	t=region[pc_i->region].guardnum[(rand()%10)+1];
	pChar pc_c = npcs::AddNPCxyz(pc->getSocket(), t, x, y, z);

	if ( ! pc_c ) return;
	
	pc_c->npcaitype=NPCAI_TELEPORTGUARD;
	pc_c->setAttackFirst();
	pc_c->attackerserial=pc->getSerial();
	pc_c->targserial=pc->getSerial();
	pc_c->npcWander=cNPC::WANDER_FREELY_CIRCLE;  //set wander mode Tauriel
	pc_c->toggleCombat();
	pc_c->setNpcMoveTime();
	pc_c->summontimer=(getClockmSecs()+(SECS*25));

	pc_c->playSFX(0x01FE);
	staticFX(pc_c, 0x372A, 9, 6);

	pc_c->teleport();
	pc_c->talkAll("Thou shalt regret thine actions, swine!",1);
}
//</Anthalir>

static pItem AddRandomLoot(pItem pack, char * lootlist)
{
	if ( ! pack ) return NULL;
	string value( lootlist );
	string loot( cObject::getRandomScriptValue( "LOOTLIST", value ) );
	pItem pi = item::CreateFromScript( (SCRIPTID) str2num( loot ), pack );
	return pi;
}

static int AddRandomNPC(pClient client, char * npclist)
{
	string list( npclist );
	string sNpc = cObject::getRandomScriptValue( "NPCLIST", list );
	int npc = str2num( sNpc );
	return (npc!=0)? npc : INVALID;
}

//o---------------------------------------------------------------------------o
//| Function   : AddNPC (3 interfaces)
//| Programmer : Duke, 23.05.2000
//o---------------------------------------------------------------------------o
//| Purpose    : creates the scripted NPC given by npcNum
//| 			 The position of the NPC can be given in three different ways:
//| 			 1. by parms x1 y1 z1 (trigger)
//| 			 2. by passing a socket (GM add)
//| 			 3. by passing an item index (spawn rune)
//|
//| Remarks    : This function was created from the former AddRespawnNPC() and
//| 			 AddNPCxyz() that were 95% identical
//o---------------------------------------------------------------------------o
static pChar AddRespawnNPC(pItem pi, int npcNum)
{
        //type 1 remember
	if ( ! pi ) return;
        return AddNPC(INVALID, pi, npcNum, 0,0,0);
}

static pChar AddRespawnNPC(pChar pc, int npcNum)
{
	if ( ! pc ) return NULL;
        return AddNPC(pc->getSocket(), NULL, npcNum, 0,0,0);
}

//<Anthalir>
static pChar AddNPCxyz(pClient client, int npcNum, sLocation where)
{
	return AddNPCxyz(s, npcNum, where.x, where.y, where.dispz);
}
//</Anthalir>

static pChar AddNPCxyz(pClient client, int npcNum, int x1, int y1, signed char z1) //Morrolan - replacement for old npcs::AddNPCxyz(), fixes a LOT of problems.
{
	return AddNPC(s, NULL, npcNum, x1,y1,z1);
}


//<Anthalir>
static pChar AddNPC(pClient client, pItem pi, int npcNum, sLocation where)
{
	return AddNPC(s, pi, npcNum, where.x, where.y, where.z);
}
//</Anthalir>

// Xan -> compatible with new style scripts!! :D
static pChar AddNPC(pClient client, pItem pi, int npcNum, uint16_t x1, uint16_t y1, int8_t z1)
{
	int32_t	npcNumSave	= npcNum;
	short	postype		= 0;	// determines how xyz of the new NPC are set
	pChar	pc		= 0;

	if	(x1 > 0 && y1 > 0)
		postype = 3;	// take position from parms
	else if ( s > INVALID && !pi )
		postype = 2;	// take position from socket's buffer
	else if ( s == INVALID && pi )
		postype = 1;	// take position from items[i]

	if ( !postype )
	{
		LogError("AddNPC: bad parms in call (socket [%d], item[%d], npcNum[%d], x1[%d], y1[%d]\n",
			s, ((pi)? pi->getSerial() : INVALID), npcNum, x1, y1);
	}
	else
	{
		cScpIterator*	iter = Scripts::Npc->getNewIterator("SECTION NPC %i", npcNum);
		if ( !iter )
		{
			LogError("AddNpc: SECTION NPC %i not found\n", npcNum );
		}
		else
		{
			int		tmp,
					z,
					lovalue,
					hivalue,
					storeval		= INVALID,
					k			= 0,
					xos			= 0,
					yos			= 0,
					lb,
					haircolor		= INVALID,
					fx1			= 0,
					fx2			= 0,
					fy1			= 0,
					fy2			= 0,
					fz1			= 0,
					loopexit		= 0;

			pItem		pi_n			= 0,
					mypack			= 0;

			//char 		script1[1024],
			//		script2[1024];

			string	script1,
					script2,
					script3;

			ITEM 		buyRestockContainer 	= INVALID,
					buyNoRestockContainer	= INVALID,
					sellContainer		= INVALID;

			bool		error			= false;

			do
			{
				iter->parseLine(script1, script2);
				if ( script1[0]!='{' && script1[0]!='}' )
				{
					if ( "NPCLIST" == script1 )
					{
						npcNum=npcs::AddRandomNPC( s, const_cast<char*>(script2.c_str()));
						if ( npcNum == INVALID )
						{
							error = true;
						}
						break;
					}
				}
			} while ( script1[0] !='}' && ++loopexit < MAXLOOPS && !error );


			safedelete(iter);

			if	( loopexit == MAXLOOPS )
			{
				LogError("AddNpc: Missing { in SECTION NPC %i\n", npcNumSave );
			}
			else if ( error )
			{
				LogError("AddNpc: NPC %s in NPCLIST in SECTION NPC %i is invalid\n", script2.c_str(), npcNumSave );
			}
			else
			{
				iter = Scripts::Npc->getNewIterator("SECTION NPC %i", npcNum);
				if ( !iter )
				{
					if( npcNum != npcNumSave )
						LogError("AddNpc: NPC %i in NPCLIST in SECTION NPC %i is invalid\n", npcNum, npcNumSave );
					else
						LogError("AddNpc: SECTION NPC %i not found\n", npcNum );
				}
				else
				{
					//
					// Now lets spawn him/her
					//

					pc = new cChar(cChar::nextSerial());

					pc->SetPriv(0x10);
					pc->npc=1;
					pc->att=1;
					pc->def=1;

					loopexit=0;

					amxVS.setServerMode();
					do
					{
						iter->parseLine(script1, script2);

						if ( script1[0]!='}' && script1[0]!='{' )
						{
							switch(script1[0])
							{
							case '@':
								pc->loadEventFromScript( const_cast<char*>(script1.c_str()), const_cast<char*>(script2.c_str()));
								break;
							case 'A':
								if	( "ALCHEMY" == script1 )
									pc->baseskill[ALCHEMY] = getRangedValue(script2);
								else if ( "AMOUNT" == script1 )
								{
									if( pi_n )
										pi_n->amount = str2num( script2 );
								}
								//
								// Old style user variables
								//
								else if	( "AMXFLAG0" == script1 )
									amxVS.insertVariable( pc->getSerial(), 0, str2num( script2 ) );
								else if ( "AMXFLAG1" == script1 )
									amxVS.insertVariable( pc->getSerial(), 1, str2num( script2 ) );
								else if ( "AMXFLAG2" == script1 )
									amxVS.insertVariable( pc->getSerial(), 2, str2num( script2 ) );
								else if ( "AMXFLAG3" == script1 )
									amxVS.insertVariable( pc->getSerial(), 3, str2num( script2 ) );
								else if ( "AMXFLAG4" == script1 )
									amxVS.insertVariable( pc->getSerial(), 4, str2num( script2 ) );
								else if ( "AMXFLAG5" == script1 )
									amxVS.insertVariable( pc->getSerial(), 5, str2num( script2 ) );
								else if ( "AMXFLAG6" == script1 )
									amxVS.insertVariable( pc->getSerial(), 6, str2num( script2 ) );
								else if ( "AMXFLAG7" == script1 )
									amxVS.insertVariable( pc->getSerial(), 7, str2num( script2 ) );
								else if ( "AMXFLAG8" == script1 )
									amxVS.insertVariable( pc->getSerial(), 8, str2num( script2 ) );
								else if ( "AMXFLAG9" == script1 )
									amxVS.insertVariable( pc->getSerial(), 9, str2num( script2 ) );
								else if ( "AMXFLAGA" == script1 )
									amxVS.insertVariable( pc->getSerial(), 10, str2num( script2 ) );
								else if ( "AMXFLAGB" == script1 )
									amxVS.insertVariable( pc->getSerial(), 11, str2num( script2 ) );
								else if ( "AMXFLAGC" == script1 )
									amxVS.insertVariable( pc->getSerial(), 12, str2num( script2 ) );
								else if ( "AMXFLAGD" == script1 )
									amxVS.insertVariable( pc->getSerial(), 13, str2num( script2 ) );
								else if ( "AMXFLAGE" == script1 )
									amxVS.insertVariable( pc->getSerial(), 14, str2num( script2 ) );
								else if ( "AMXFLAGF" == script1 )
									amxVS.insertVariable( pc->getSerial(), 15, str2num( script2 ) );
								//
								// New style user variables
								//
								else if ( "AMXINT" == script1 )
								{
									string rha1;
									string rha2;
									splitLine( script2, rha1, rha2 );
									amxVS.insertVariable( pc->getSerial(), str2num( rha1 ), str2num( rha2 ) );
								}
								else if ( "AMXINTVEC" == script1 )
								{
									splitLine( script2, script1, script3 );
									int32_t vectorId = str2num( script1 );
									int32_t vectorSize = str2num( script3 );
									amxVS.insertVariable( pc->getSerial(), vectorId, vectorSize, 0 );
									int32_t vectorIndex = 0;
									do
									{
										iter->parseLine(script1, script2);
										switch( script1[0] )
										{
											case '[' :
											case ']' :
												break;
											default	 :
												if( vectorIndex < vectorSize )
												{
													amxVS.updateVariable( pc->getSerial(), vectorId, vectorIndex, str2num( script1 ) );
													++vectorIndex;
												}
												break;
										}
									}
									while( script1[0] != ']' );
								}
								else if ( "AMXSTR" == script1 )
								{
									string rha1;
									string rha2;
									splitLine( script2, rha1, rha2 );
									amxVS.insertVariable( pc->getSerial(), str2num( rha1 ), rha2 );
								}
								else if ( "skAnatomy" == script1 )
									pc->baseskill[skAnatomy] = getRangedValue(script2);
								else if ( "skAnimalLore" == script1 )
									pc->baseskill[skAnimalLore] = getRangedValue(script2);
								else if ( "skArchery" == script1 )
									pc->baseskill[skArchery] = getRangedValue(script2);
								else if ( "skArmsLore" == script1 )
									pc->baseskill[skArmsLore] = getRangedValue(script2);
								break;
							case 'B':
								if	( "BACKPACK" == script1 )
								{
									if (!mypack)
									{
										pi_n=item::CreateFromScript( "$item_backpack" );
										if( pi_n )
										{
											pc->packitemserial=pi_n->getSerial();
											pi_n->setPosition(0, 0, 0);
											pi_n->setContainer(pc);
											mypack=pi_n;
										}
										else
											LogWarning("AddNPC: cannot spawn item $item_backpack\n");
										script1 = "DUMMY";
									}
								}
								else if ( "skBegging" == script1 )
									pc->baseskill[skBegging] = getRangedValue(script2);
								else if ( "skBlacksmithing" == script1 )
									pc->baseskill[skBlacksmithing] = getRangedValue(script2);
								else if ( "skBowcraft" == script1 )
									pc->baseskill[skBowcraft] = getRangedValue(script2);
								break;
							case 'C':
								if	( "COLOR" == script1 )
								{
									if( pi_n )
									{
										pi_n->setColor( hex2num(script2) );
									}
								}
								else if ( "skCamping" == script1 )
									pc->baseskill[skCamping] = getRangedValue(script2);
								else if ( "CANTRAIN" == script1 )
									pc->cantrain=true;
								else if ( "CARVE" == script1 )
									pc->carve=str2num(script2);
								else if ( "" == script1 )
									pc->baseskill[] = getRangedValue(script2);
								else if ( "skCartography" == script1 )
									pc->baseskill[skCartography] = getRangedValue(script2);
								else if ( "COLORMATCHHAIR" == script1 )
								{
									if (pi_n && haircolor != INVALID )
									{
										pi_n->setColor( haircolor );
									}
								}
								else if ( "COLORLIST" == script1 )
								{
									if (pi_n)
									{
										string value( cObject::getRandomScriptValue("RANDOMCOLOR", script2)  );
										pi_n->setColor( hex2num( value ) );
									}
									script1 = "DUMMY";
								}
								else if ( "skCooking" == script1 )
									pc->baseskill[skCooking] = getRangedValue(script2);
								break;
							case 'D':
								if	("DAMAGE" == script1 || "ATT" == script1 )
								{
									gettokennum(script2, 0);
									lovalue=str2num(gettokenstr);
									gettokennum(script2, 1);
									hivalue=str2num(gettokenstr);
									pc->lodamage = lovalue;
									pc->hidamage = lovalue;
									if(hivalue)
										pc->hidamage = hivalue;
								}
								else if ( "DAMAGETYPE" == script1 )
									pc->damagetype = static_cast<DamageType>(str2num(script2));
								else if ( "DEF" == script1 )
									pc->def = getRangedValue(script2);
								else if ( "skDetectingHidden" == script1 )
									pc->baseskill[skDetectingHidden] = getRangedValue(script2);
								else if ( "DEX" == script1 || "DEXTERITY" == script1 )
								{
									pc->dx  = getRangedValue(script2);
									pc->dx2 = pc->dx;
									pc->dx3 = pc->dx;
									pc->stm = pc->dx;
								}
								else if ( "DIRECTION" == script1 )
								{
									if	( "NE" == script2 )
										pc->dir=1;
									else if ( "E" == script2 )
										pc->dir=2;
									else if ( "SE" == script2 )
										pc->dir=3;
									else if ( "S" == script2 )
										pc->dir=4;
									else if ( "SW" == script2 )
										pc->dir=5;
									else if ( "W" == script2 )
										pc->dir=6;
									else if ( "NW" == script2 )
										pc->dir=7;
									else if ( "N" == script2 )
										pc->dir=0;
								}
								else if ( "DOORUSE" == script1 )
									pc->doorUse = str2num( script2 );
								break;
							case 'E':
								if	( "EMOTECOLOR" == script1 )
								{
									pc->emotecolor = hex2num(script2);
								}
								else if ( "skEnticement" == script1 )
									pc->baseskill[skEnticement] = getRangedValue(script2);
								else if ( "skEvaluatingIntelligence" == script1 )
									pc->baseskill[skEvaluatingIntelligence] = getRangedValue(script2);
								break;
							case 'F':
								if	( "FAME" == script1 )
									pc->SetFame(str2num(script2));
								else if ( "FENCING" == script1 )
									pc->baseskill[FENCING] = getRangedValue(script2);
								else if ( "FISHING" == script1 )
									pc->baseskill[FISHING] = getRangedValue(script2);
								else if ( "FLEEAT" == script1 )
									pc->fleeat=str2num(script2);
								else if ( "FOLLOWSPEED" == script1)
									pc->npcFollowSpeed = (float) atof( script2.c_str() );
								else if ( "skForensics" == script1 )
									pc->baseskill[skForensics] = getRangedValue(script2);
								else if ( "FX1" == script1 )
									fx1=str2num(script2);
								else if ( "FX2" == script1 )
									fx2=str2num(script2);
								else if ( "FY1" == script1 )
									fy1=str2num(script2);
								else if ( "FY2" == script1 )
									fy2=str2num(script2);
								else if ( "FZ1" == script1 )
									fz1=str2num(script2);
								break;
							case 'G':
								if	("GOLD" == script1 )
								{
									if (mypack)
									{
										string 	lo,
												hi;

										splitLine( script2, lo, hi );
										int amt = RandomNum( str2num(lo), str2num(hi) );

										pItem pi_sp = item::CreateFromScript( "$item_gold_coin", mypack, amt );
										if( pi_sp )
										{
											pi_sp->priv|=0x01;
										}
										else
											LogWarning("AddNPC: cannot spawn item $item_gold_coin\n");
									}
									else
										LogWarning("Bad NPC Script %d with problem no backpack for gold.\n", npcNum);
								}
								break;
							case 'H':
								if	( "HAIRCOLOR" == script1 )
								{
									if (pi_n)
									{
										string value( cObject::getRandomScriptValue("RANDOMCOLOR", script2) );
										haircolor = hex2num( value );
										if (haircolor!=-1)
										{
											pi_n->setColor( haircolor );
										}
									}
									script1 = "DUMMY";
								}
								else if	( "HEALING" == script1 )
									pc->baseskill[HEALING] = getRangedValue(script2);
								else if ( "skHerding" == script1 )
									pc->baseskill[skHerding] = getRangedValue(script2);
								else if ( "HIDAMAGE" == script1 )
									pc->hidamage=str2num(script2);
								else if ( "HIDING" == script1 )
									pc->baseskill[HIDING] = getRangedValue(script2);
								else if ( "HOLYDAMAGED" == script1 )
									pc->setHolyDamaged(true);
								break;
							case 'I':
								if	( "ID" == script1 )
								{
									tmp=hex2num(script2);
									pc->setId(tmp);
									pc->setOldId(tmp);
								}
								else if ( "skInscription" == script1 )
									pc->baseskill[skInscription] = getRangedValue(script2);
								else if ( "INT" == script1 || "INTELLIGENCE" == script1 )
								{
									pc->in  = getRangedValue(script2);
									pc->in2 = pc->in;
									pc->in3 = pc->in;
									pc->mn  = pc->in;
								}
								else if ( "INVULNERABLE" == script1 )
									pc->MakeInvulnerable();
								else if ( "ITEM" == script1 )
								{
									storeval=str2num(script2);
									pi_n=item::CreateScriptItem( INVALID, storeval, 0, pc );
									if (pi_n)
									{
										if (pi_n->layer==0)
											LogWarning("Bad NPC Script %d with problem item %d executed!\n", npcNum, storeval);
									}
									script1 = "DUMMY";
								}
								else if ( "ITEMID" == script1 )
									pc->baseskill[ITEMID] = getRangedValue(script2);
								break;
							case 'K':
								if	( "KARMA" == script1 )
									pc->SetKarma(str2num(script2));
								break;
							case 'L':
								if	( "LIGHTDAMAGED" == script1 )
									pc->setLightDamaged(true);
								else if ( "skLockPicking" == script1 )
									pc->baseskill[skLockPicking] = getRangedValue(script2);
								else if ( "LODAMAGE" == script1 )
									pc->lodamage=str2num(script2);
								else if ( "LOOT" == script1 )
								{
									//
									// Sparhawk	:	Just In Time Loot handling
									//
									// Syntax	:	LOOT <lootlistid>
									//
									// Todo		:	get item from lootlist now
									//
									pc->lootVector.push_back( str2num( script2 ) );
								}
								else if ( "LOOTITEM" == script1 )
								{
									//
									// Sparhawk	:	Just In Time Loot handling
									//
									// Syntax	:	LOOTITEM <itemid>
									//			{
									//				AMOUNT <amount range>
									//				CHANCE <chance>
									//			}
									//
									/*
									string strLootItem = script2;
									do
									{
										iter->parseLine(script1, script2);
										if ( script1[0]!='{' && script1[0]!='}' )
										{
											if ( "AMOUNT" == script1 )
											{
												amount = getRangedValue( script2 );
												if( amount < 1 )
													amount = 1;
											}
											else if ( "CHANCE" == script1 )
											{
												chance = str2num( script2 );
												if( chance < 1 )
													chance = 1;
												else if ( chance > 100 )
													chance = 100;
											}
											else
											{
												unknown tag;
											}
										}
									} while ( script1[0] !='}' && ++loopexit < MAXLOOPS );

									if ( chance( chance ) )
										pc->lootVector.push_back( makepair( str2num( strLootItem ), chance ) );

									script1 = "DUMMY";
									*/

								}
								else if ( "skLumberjacking" == script1 )
									pc->baseskill[skLumberjacking] = getRangedValue(script2);
								break;
							case 'M':
								if	( "skMacefighting" == script1 )
									pc->baseskill[skMacefighting] = getRangedValue(script2);
								else if ( "skMagery" == script1 )
									pc->baseskill[skMagery] = getRangedValue(script2);
								else if ( "skMagicResistance" == script1 )
									pc->baseskill[skMagicResistance] = getRangedValue(script2);
								else if	( "MAGICSPHERE" == script1 )
									pc->magicsphere = str2num(script2);
								else if ( "MAGICLEVEL" == script1 )
								{
									if (ServerScp::g_nUseNewNpcMagic!=0)
										pc->spattack=str2num(script2);
								}
								else if ( "skMeditation" == script1 )
									pc->baseskill[skMeditation] = getRangedValue(script2);
								else if ( "skMining" == script1 )
									pc->baseskill[skMining] = getRangedValue(script2);
								else if ( "MOVESPEED" == script1)
									pc->npcMoveSpeed = (float) atof( script2.c_str() );
								else if ( "skMusicianship" == script1 )
									pc->baseskill[skMusicianship] = getRangedValue(script2);
								break;
							case 'N':
								if	( "NAME" == script1 )
								{
									pc->setCurrentName( script2 );
									pc->setRealName( script2 );
								}
								else if ( "NAMELIST" == script1 )
								{
									pc->setCurrentName( cObject::getRandomScriptValue("RANDOMNAME", script2 ) );
									pc->setRealName( pc->getCurrentName() );
									script1 = "DUMMY";
								}
								else if ( "NOTRAIN" == script1 )
									pc->cantrain=false;
								else if ( "NPCAI" == script1 )
									pc->npcaitype=str2num(script2);
								else if ( "NPCWANDER" == script1 )
									pc->npcWander=str2num(script2);
								else if ( "NXWFLAG0" == script1 )
									pc->nxwflags[0] = str2num(script2);
								else if ( "NXWFLAG1" == script1 )
									pc->nxwflags[1] = str2num(script2);
								else if ( "NXWFLAG2" == script1 )
									pc->nxwflags[2] = str2num(script2);
								else if ( "NXWFLAG3" == script1 )
									pc->nxwflags[3] = str2num(script2);
								break;
							case 'O':
								if	( "ONHORSE" == script1 )
									pc->setOnHorse();
								break;
							case 'P':
								if	( "PACKITEM" == script1)
								{
									if (mypack)
									{
										storeval=str2num(script2);
										pi_n=item::CreateFromScript( storeval, mypack);
										script1 = "DUMMY";
									}
									else
										LogWarning("Bad NPC Script %d with problem no backpack for packitem.\n", npcNum);
								}
								else if ( "skParrying" == script1)
									pc->baseskill[skParrying] = getRangedValue(script2);
								else if ( "skPeacemaking" == script1 )
									pc->baseskill[skPeacemaking] = getRangedValue(script2);
								else if ( "POISON" == script1)
									pc->poison=str2num(script2);
								else if ( "skPoisoning" == script1 )
									pc->baseskill[skPoisoning] = getRangedValue(script2);
								else if ( "PRIV1" == script1 )
									pc->SetPriv(str2num(script2));
								else if ( "PRIV2" == script1 )
									pc->SetPriv2(str2num(script2));
								else if ( "skProvocation" == script1 )
									pc->baseskill[skProvocation] = getRangedValue(script2);
								break;
							case 'R':
								if	( "RACE" == script1 )
									pc->race=str2num(script2);
								else if ( "REATTACKAT" == script1 )
									pc->reattackat=str2num(script2);
								else if ( "REGEN_HP" == script1 )
								{
									uint32_t v=str2num(script2);
									pc->setRegenRate( STAT_HP, v, VAR_REAL );
									pc->setRegenRate( STAT_HP, v, VAR_EFF );
								}
								else if ( "REGEN_ST" == script1 )
								{
									uint32_t v=str2num(script2);
									pc->setRegenRate( STAT_STAMINA, v, VAR_REAL );
									pc->setRegenRate( STAT_STAMINA, v, VAR_EFF );
								}
								else if ( "REGEN_MN" == script1 )
								{
									uint32_t v=str2num(script2);
									pc->setRegenRate( STAT_MANA, v, VAR_REAL );
									pc->setRegenRate( STAT_MANA, v, VAR_EFF );
								}
								else if ( "skRemoveTraps" == script1 )
									pc->baseskill[skRemoveTraps] = getRangedValue(script2);
								else if ( "RESISTS" == script1 )
								{
									int params[2];
									fillIntArray(const_cast<char*>(script2.c_str()), params, 2, 0, 10);
									if (params[0] < MAX_RESISTANCE_INDEX)
										pc->resists[params[0]] = params[1];
								}
								else if ( "RSHOPITEM" == script1 )
								{
									if ( buyRestockContainer == INVALID )
									{

										pItem pi_z=pc->GetItemOnLayer(LAYER_TRADE_RESTOCK);
										if (pi_z)
										{
											buyRestockContainer = pi_z->getSerial();
										}
									}
									if ( buyRestockContainer != INVALID )
									{
										string	itmnum,
												amount;

										splitLine( script2, itmnum, amount );

										int amt=str2num( amount );
										if( amt <= 0 )
											amt=server_data.defaultSelledItem;

										pi_n=item::CreateFromScript( str2num(itmnum), cSerializable::findItemBySerial( buyRestockContainer ), amt );
										if (pi_n)
										{
											if (pi_n->getSecondaryNameC() && (strcmp(pi_n->getSecondaryNameC(),"#")))
												pi_n->setCurrentName(pi_n->getSecondaryNameC()); // Item identified! -- by Magius(CHE) 				}
										}
										script1 = "DUMMY";
									}
									else
										LogWarning("Bad NPC Script %d with problem no buyRestockContainer for item %s.\n", npcNum, script2.c_str());
								}
								break;
							case 'S':
								if	( "SAYCOLOR" == script1 )
								{
									pc->saycolor = hex2num(script2);
								}
								else if ( "SELLITEM" == script1 )
								{
									if ( sellContainer == INVALID )
									{
										pItem pi_z=pc->GetItemOnLayer(0x1C);
										{
											sellContainer = pi_z->getSerial();
										}
									}
									if ( sellContainer != INVALID )
									{
										storeval=str2num(script2);
										pi_n=item::CreateFromScript( storeval, cSerializable::findItemBySerial(sellContainer));
										if (pi_n)
										{
											pi_n->value=pi_n->value/2;
											if (pi_n->getSecondaryNameC() && (strncmp(pi_n->getSecondaryNameC(),"#", 1)))
												pi_n->setCurrentName(pi_n->getSecondaryNameC()); // Item identified! -- by Magius(CHE) 				}
										}
										script1 = "DUMMY";
									}
									else
										LogWarning("Bad NPC Script %d with problem no sellContainer for item %s.\n", npcNum, script2.c_str());
								}
								else if ( "SHOPITEM" == script1 )
								{
									if ( buyNoRestockContainer == INVALID )
									{
										pItem pi_z=pc->GetItemOnLayer(0x1B);
										if (pi_z)
										{
											buyNoRestockContainer = pi_z->getSerial();
										}
									}
									if ( buyNoRestockContainer != INVALID )
									{
										storeval=str2num(script2);
										pi_n=item::CreateFromScript( storeval, cSerializable::findItemBySerial(buyNoRestockContainer));
										if (pi_n)
										{
											if (pi_n->getSecondaryNameC() && (strcmp(pi_n->getSecondaryNameC(),"#")))
												pi_n->setCurrentName(pi_n->getSecondaryNameC()); // Item identified! -- by Magius(CHE) 				}
										}
										script1 = "DUMMY";
									}
									else
										LogWarning("Bad NPC Script %d with problem no buyNoRestockContainer for item %s.\n", npcNum, script2.c_str());
								}
								else if ( "SHOPKEEPER" == script1 )
									Commands::MakeShop(pc);
								else if ( "SKILL" == script1 )
								{
									gettokennum(script2, 0);
									z=str2num(gettokenstr);
									gettokennum(script2, 1);
									pc->baseskill[z]=str2num(gettokenstr);
								}
								else if ( "SKIN" == script1 )
								{
									tmp=hex2num(script2);
									pc->setColor(tmp);
									pc->setOldColor(tmp);
								}
								else if ( "SKINLIST" == script1 )
								{
									string value( cObject::getRandomScriptValue("RANDOMCOLOR", script2 ) );
									pc->setColor( hex2num( value ) );
									pc->setOldColor( pc->getColor() );
									script1 = "DUMMY";
								}
								else if ( "skSnooping" == script1 )
								{
									pc->baseskill[skSnooping] = getRangedValue(script2);
								}
								else if ( "SPADELAY" == script1 )
									pc->spadelay=str2num(script2);
								else if ( "SPATTACK" == script1 )
								{
									if (ServerScp::g_nUseNewNpcMagic==0)
										pc->spattack=str2num(script2);
								}
								else if ( "SPEECH" == script1 )
									pc->speech=str2num(script2);
								else if ( "SPIRITSPEAK" == script1 )
								{
									pc->baseskill[SPIRITSPEAK] = getRangedValue(script2);
								}
								else if ( "SPLIT" == script1 )
									pc->split=str2num(script2);
								else if ( "SPLITCHANCE" == script1 )
									pc->splitchnc=str2num(script2);
								else if ( "STABLEMASTER" == script1 )
									pc->npc_type=1;
								else if ( "skStealing" == script1 )
								{
									pc->baseskill[skStealing] = getRangedValue(script2);
								}
								else if ( "skStealth" == script1 )
								{
									pc->baseskill[skStealth] = getRangedValue(script2);
								}
								else if ( "STR" == script1 || "STRENGTH" == script1 )
								{
									pc->setStrength( getRangedValue(script2) );
									pc->st2 = pc->getStrength();
									pc->st3 = pc->getStrength(); //Luxor
									pc->hp  = pc->getStrength();
								}
								else if ( "skSwordsmanship" == script1 )
								{
									pc->baseskill[skSwordsmanship] = getRangedValue(script2);
								}
								break;
							case 'T':
								if	( "skTactics" == script1 )
									pc->baseskill[skTactics] = getRangedValue(script2);
								else if ( "skTailoring" == script1 )
									pc->baseskill[skTailoring] = getRangedValue(script2);
								else if ( "skTaming" == script1 )
									pc->baseskill[skTaming] = getRangedValue(script2);
								else if ( "skTasteID" == script1 )
									pc->baseskill[skTasteID] = getRangedValue(script2);
								else if ( "skTinkering" == script1 )
									pc->baseskill[skTinkering] = getRangedValue(script2);
								else if ( "TITLE" == script1 )
									pc->title = script2;
								else if ( "TOTAME" == script1 || "skTaming" == script1 )
									pc->taming=str2num(script2);
								else if ( "skTracking" == script1 )
									pc->baseskill[skTracking] = getRangedValue(script2);
								else if ( "TRIGGER" == script1 )
									pc->trigger=str2num(script2);
								else if ( "TRIGWORD" == script1 )
									pc->trigword=script2;
								break;
							case 'V':
								if	( "VALUE" == script1 )
								{
									if ( pi_n )
										pi_n->value=(str2num(script2));
								}
								else if ( "skVeterinary" == script1 )
									pc->baseskill[skVeterinary] = getRangedValue(script2);
								else if ( "VULNERABLE" == script1 )
									pc->MakeVulnerable();
								break;
							case 'W':
								if	( "WATERWALK" == script1 )
									pc->nxwflags[0] |= cChar::flagWaterWalk;
								else if ( "skWrestling" == script1 )
									pc->baseskill[skWrestling] = getRangedValue(script2);
								break;
							default:
								SWITCH_FALLOUT;
								LogError("AddNPC( %d ), invalid script param [ %s ]\n", npcNum, script1.c_str());
							}
						}
					}
					while ( script1[0]!='}' && ++loopexit < MAXLOOPS );

					safedelete(iter);

					amxVS.setUserMode();

					pc->setScriptID( npcNum );

					// Now that we have created the NPC, lets place him
					switch (postype)
					{
					case 1:					// take position from (spawning) item
						{
						/*Zippy's Code chages for area spawns --> (Type 69) xos and yos (X OffSet, Y OffSet)
						are used to find a random number that is then added to the spawner's x and y (Using
						the spawner's z) and then place the NPC anywhere in a square around the spawner.
						This square is random anywhere from -10 to +10 from the spawner's location (for x and
						y) If the place chosen is not a valid position (the NPC can't walk there) then a new
						place will be chosen, if a valid place cannot be found in a certain # of tries (50),
							the NPC will be placed directly on the spawner and the server op will be warned. */

							pItem pi_i=pi;
							if (pi_i && ((pi_i->type==69 || pi_i->type==125)&& pi_i->isInWorld()) )
							{
								if (pi_i->more3==0) pi_i->more3=10;
								if (pi_i->more4==0) pi_i->more4=10;
								//signed char z, ztemp, found;

								k=0;

								do
								{
									if (k>=50) //this CAN be a bit laggy. adjust as nessicary
									{
										LogWarning("Problem area spawner found at [%i,%i,%i]. NPC placed at default location.\n",pi_i->getPosition().x, pi_i->getPosition().y, pi_i->getPosition().z);
										xos=0;
										yos=0;
										break;
									}
									xos=RandomNum(-pi_i->more3,pi_i->more3);
									yos=RandomNum(-pi_i->more4,pi_i->more4);
									//outPlain("AddNPC Spawning at Offset %i,%i (%i,%i,%i) [-%i,%i <-> -%i,%i]. [Loop #: %i]\n",xos,yos,items[i].x+xos,items[i].y+yos,items[i].z,items[i].more3,items[i].more3,items[i].more4,items[i].more4,k); /** lord binary, changed %s to %i, crash when uncommented ! **/
									k++;

									if ((pi_i->getPosition().x+xos<1) || (pi_i->getPosition().y+yos<1))
										lb=0; /* lord binary, fixes crash when calling npcvalid with negative coordiantes */
									else { //<Luxor>
										sLocation newpos = sLocation( pi_i->getPosition().x+xos, pi_i->getPosition().y+yos, pi_i->getPosition().z );
										lb = ( isWalkable( newpos ) != illegal_z );
									}//</Luxor>

									//Bug fix Monsters spawning on water:

									if ( isWaterTile(pi_i->getPosition().x+xos, pi_i->getPosition().y+yos) )
										break;
								} while (!lb);
							} // end Zippy's changes (exept for all the +xos and +yos around here....)

							/*
							pc->x= items[i].x + xos;
							pc->y= items[i].y + yos;
							pc->dispz= chars[c].z= items[i].z;
							*/
							pc->MoveTo( pi_i->getPosition() );

							} // end of if !triggerx
							break;
						case 2: // take position from Socket
							if (s > INVALID)
							{
								/*
								pc->x=(buffer[s][11]<<8)+buffer[s][12];
								pc->y=(buffer[s][13]<<8)+buffer[s][14];
								pc->dispz=chars[c].z=buffer[s][16]+Map->TileHeight((buffer[s][17]<<8)+buffer[s][18]);
								*/
								pc->MoveTo( (buffer[s][11]<<8)+buffer[s][12], (buffer[s][13]<<8)+buffer[s][14],
												buffer[s][16]+tileHeight((buffer[s][17]<<8)+buffer[s][18]) );
							}
							break;
						case 3: // take position from Parms
							/*
							pc->x=x1;
							pc->y=y1;
							pc->dispz=pc->z=z1;
							*/
							pc->MoveTo( x1, y1, z1 );
							break;
						} // no default coz we tested on entry to function

						sLocation charpos= pc->getPosition();
						// now we have a position, let's set the borders
						switch (pc->npcWander)
						{
						case 2: 	// circle
							pc->fx1= charpos.x;
							pc->fy1= charpos.y;
							pc->fz1= charpos.z;
							pc->fx2= (fx2>=0) ? fx2 : 2;	// radius; if not given from script,default=2
							break;
						case 3: 	// box
							if (fx1 || fx2 || fy1 || fy2)	// any offset for rectangle given from script ?
							{
								pc->fx1= charpos.x+fx1;
								pc->fx2= charpos.x+fx2;
								pc->fy1= charpos.y+fy1;
								pc->fy2= charpos.y+fy2;
								pc->fz1= -1;			// irrelevant for box
							}
							break;
						default:
							pc->fx1=fx1;
							pc->fx2=fx2;
							pc->fy1=fy1;
							pc->fy2=fy2;
							pc->fz1=fz1;
							break;
						}

						pc->region=static_cast<unsigned char>(calcRegionFromXY( pc->getPosition() ));

						//Now find real 'skill' based on 'baseskill' (stat modifiers)
						for(z=0;z<skTrueSkills;z++)
						{
							Skills::updateSkillLevel(pc,z);
						}

						if (donpcupdate==0)
						{
							pc->teleport();
						}

					// Dupois - Added April 4, 1999
					// After the NPC has been fully initialized, then post the message (if its a quest spawner) type==125
					if (postype==1) // lb crashfix
					{
							pItem pi_i=pi;
							if ( pi_i && pi_i->type == 125 )
							{
								pc->createEscortQuest();
							}
					}
					// End - Dupois
#ifdef SPAR_C_LOCATION_MAP
					pointers::updateLocationMap( pc ); // Spahawk: it has allready been added by MoveTo
#else
					//Char mapRegions
					mapRegions->add(pc);
#endif
					//
					safedelete(iter);
					pc->teleport();
				}
			}
		}
	}
	return pc;

}

pChar addNpc(int npcNum, int x, int y, int z) {
	return AddNPC(INVALID, NULL, npcNum, x, y, z);
}

pChar SpawnRandomMonster(pChar pc, char* cList, char* cNpcID)
{
	string	section( cList ),
			sectionId( cNpcID ),
			value( cObject::getRandomScriptValue( section, sectionId ) );

	return npcs::AddRespawnNPC( pc, str2num( value ) );
}

} // namespace

cCreatureInfo::cCreatureInfo()
{
	sounds[SND_STARTATTACK] = NULL;
	sounds[SND_IDLE] = NULL;
	sounds[SND_ATTACK] = NULL;
	sounds[SND_DEFEND] = NULL;
	sounds[SND_DIE] = NULL;
	flag=0;
	icon=0x20D1;
}

cCreatureInfo::~cCreatureInfo()
{
	for( int i=0; i<ALL_MONSTER_SOUND; ++i )
		if( sounds[i]!=NULL )
			delete sounds[i];
}

SOUND cCreatureInfo::getSound( MonsterSound type )
{
	if( ( sounds[ type ]==NULL ) )
		return INVALID;

	return (*sounds[ type ])[ rand()%( sounds[ type ]->size() ) ];

}

void cCreatureInfo::addSound( MonsterSound type, SOUND sound )
{
	if( sounds[ type ]==NULL )
		sounds[ type ] = new vector<SOUND>;

	sounds[ type ]->push_back( sound );
}



cAllCreatures creatures;

cAllCreatures::cAllCreatures()
{
}

cAllCreatures::~cAllCreatures()
{
	for( int i=0; i<CREATURE_COUNT; ++i )
		if( allCreatures[i]!=NULL )
			delete allCreatures[i];
}

void cAllCreatures::load()
{
	cScpIterator*	iter = 0;
	string	rha, lha;
	int id=0;

	do
	{
		safedelete(iter);
		iter = Scripts::Creatures->getNewIterator("SECTION CREATURE %i", id++);
		if( iter )
		{

			pCreatureInfo cinfo = new cCreatureInfo;
			int loopexit=0;

			do
			{
				iter->parseLine( lha, rha );
				if ( lha[0] != '}' && lha[0] !='{' )
				{
					if	( lha == "START_ATTACK")
					{
						cinfo->addSound( SND_STARTATTACK, str2num( rha ) );
					}
					else if ( lha == "IDLE" )
					{
						cinfo->addSound( SND_IDLE, str2num( rha ) );
					}
					else if ( lha == "ATTACK" )
					{
						cinfo->addSound( SND_ATTACK, str2num( rha ) );
					}
					else if ( lha == "DEFEND" )
					{
						cinfo->addSound( SND_DEFEND, str2num( rha ) );
					}
					else if ( lha == "DIE" )
					{
						cinfo->addSound( SND_DIE, str2num( rha ) );
					}
					else if ( lha == "ICON" )
					{
						cinfo->icon = str2num( rha );
					}
					else if ( lha == "ANTIBLINK" )
					{
						cinfo->flag |= CREATURE_ANTI_BLINK;
					}
					else if ( lha == "CANFLY" )
					{
						cinfo->flag |= CREATURE_CAN_FLY;
					}
					else
						LogError("Parsing creatures.xss [%s]\n", lha.c_str() );
				}
			}
			while ( lha[0] !='}' && ++loopexit < MAXLOOPS );

			this->allCreatures[id-1] = cinfo;

		}
    }
	while( id< CREATURE_COUNT );

    safedelete(iter);

}

pCreatureInfo cAllCreatures::getCreature( uint16_t id )
{
	if( id>=CREATURE_COUNT )
		return NULL;
	else
		return allCreatures[id];
}
