  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/


#include "nxwcommn.h"
#include "network.h"
#include "cmdtable.h"
#include "sregions.h"
#include "bounty.h"
#include "sndpkg.h"
#include "debug.h"
#include "amx/amxcback.h"
#include "calendar.h"
#include "race.h"
#include "scp_parser.h"
#include "commands.h"
#include "addmenu.h"
#include "telport.h"
#include "accounts.h"
#include "worldmain.h"
#include "data.h"
#include "spawn.h"
#include "trade.h"
#include "chars.h"
#include "items.h"
#include "basics.h"
#include "inlines.h"
#include "nox-wizard.h"
#include "archive.h"
#include "map.h"
#include "jail.h"
#include "skills.h"
#include "layer.h"
#include "scripts.h"


/* Actual commands go below. Define all the dynamic command_ functions as
 * the name of the function in lowercase, a void function, accepting one
 * integer, which is the player # that sent the command.
 * Like this:
 * void command_rain(int s)
 *
 * If you stick comments (double-slash comments) between the function definition
 * and the opening block, they will automatically be used as the
 * HTML documentation for that command. The first line will be used as the
 * short documentation, any subsequent lines will be used as a seperate
 * "more info" page for your command. HTML is allowed, but remember that
 * everything is fit into a template.
 */

// When player has been murdered and they are
// a ghost, they can use the bounty command to
// place a bounty on the murderers head

void command_bounty( pClient client )
{
	pChar pc = client->currChar();

	if( !SrvParms->bountysactive )
	{
		pc_cs->sysmsg(TRANSLATE("The bounty system is not active."));
		return;
	}

	if( !pc->dead )
	{
		pc->sysmsg(TRANSLATE("You can only place a bounty while you are a ghost."));
		pc->murdererSer = 0;
		return;
	}

	if( pc->murdererSer == 0 )
	{
		pc->sysmsg(TRANSLATE("You can only place a bounty once after someone has murdered you."));
		return;
	}

	if( tnum == 2 )
	{
		int nAmount = strtonum(1);
		if( BountyWithdrawGold( pc, nAmount ) )
		{
			P_CHAR pc_murderer = pointers::findCharBySerial(pc->murdererSer);
			if( BountyCreate( pc_murderer, nAmount ) )
			{
				if(ISVALIDPC(pc_murderer))
					pc->sysmsg(TRANSLATE("You have placed a bounty of %d gold coins on %s."),
						nAmount, pc_murderer->getCurrentNameC() );
			} else
				pc->sysmsg(TRANSLATE("You were not able to place a bounty (System Error)") );

			// Set murdererSer to 0 after a bounty has been
			// placed so it can only be done once
			pc->murdererSer = 0;
		}
		else
			pc->sysmsg(TRANSLATE("You do not have enough gold to cover the bounty."));
	}
	else
		pc_cs->sysmsg(TRANSLATE("To place a bounty on a murderer, use BOUNTY <amount>"));

}


void command_serversleep( pClient client )
{
	pChar pc = client->currChar();

	int seconds;

#ifdef __BEOS__
	pc->sysmsg("Command not supported under BeOS");
#else

	if (tnum==2)
	{
		seconds = strtonum(1);
		sysbroadcast(TRANSLATE("server is going to sleep for %i seconds!"),seconds); // broadcast server sleep

		NxwSocketWrapper sw;
		sw.fillOnline();

		for( ; !sw.getClient(); sw++ ) {
			NXWCLIENT ps=sw.getClient();
			if( ps!=NULL )
				Network->FlushBuffer(ps->toInt());
		}

		seconds=seconds*1000;
		Sleep(seconds);
		sysbroadcast(TRANSLATE("server is back from a %i second break"),seconds/1000);

		sw.clear();
		sw.fillOnline();

		for( ; !sw.getClient(); sw++ ) { //not absolutely necessary
			NXWCLIENT ps=sw.getClient();
			if( ps!=NULL )
				Network->FlushBuffer(ps->toInt());
		}
	}
	else
	{
		pc->sysmsg("Invalid number of arguments");
	}
#endif
}

//
// Sparhawk enable online reloading of race scripts, race scripts are NOT cached all data is converted to binary
//
void command_reloadracescript( pClient client )
{
	pChar pc = client->currChar();

	Race::reload( "scripts/race.xss" );
	Race::parse();
	pc->sysmsg("Racescripts reloaded.");
}

// Returns the current bulletin board posting mode for the player
void command_post( pClient client )
{
	pChar pc = client->currChar();

	string t ( "You are currently posting " );

	switch( pc->postType )
	{
		case MsgBoards::LOCALPOST:
			t += "a message to a single board [LOCAL].";
			break;

		case MsgBoards::REGIONALPOST:
			t += "a message to all boards in this area [REGIONAL].";
			break;

		case MsgBoards::GLOBALPOST:
			t += "a message to all boards in the world [GLOBAL].";
			break;

	}

	pc->sysmsg( t.c_str() );
}

// Sets the current bulletin board posting mode for the player to GLOBAL
// ALL bulletin boards will see the next message posted to any bulletin board
void command_gpost( pClient client )
{
	pChar pc = client->currChar();

	pc->postType = MsgBoards::GLOBALPOST;
	pc->sysmsg("Now posting GLOBAL messages." );
}

// Sets the current bulletin board posting mode for the player to REGIONAL
// All bulletin boards in the same region as the board posted to will show the
// next message posted
void command_rpost( pClient client )
{
	pChar pc = client->currChar();

	pc->postType = MsgBoards::REGIONALPOST;
	pc->sysmsg("Now posting REGIONAL messages." );
}

// Sets the current bulletin board posting mode for the player to LOCAL
// Only this bulletin board will have this post
void command_lpost( pClient client );
{
	pChar pc = client->currChar();

	pc->postType = MsgBoards::LOCALPOST;
	pc->sysmsg("Now posting LOCAL messages." );
}



void target_setMurderCount( P_TARGET t )
{
    P_CHAR pc = pointers::findCharBySerial( t->getClicked() );
	VALIDATEPC(pc);

	pc->kills = t->buffer[0];
    setcharflag(pc);

}

void command_setmurder( pClient client )
{
	if( tnum == 2 )
	{
		P_TARGET targ = clientInfo[ ps->toInt() ]->newTarget( new cCharTarget() );
		targ->buffer[0]=strtonum(1);
		targ->send( ps );
		ps->sysmsg( "Select the person to set the murder count of: ");
	}
}


void command_readaccounts( pClient client )
{
	pChar pc = client->currChar();
	Accounts->LoadAccounts();
	ps->sysmsg("Accounts reloaded...attention, if you changed exisiting(!) account numbers you should use the letusin command afterwards ");
}

//! Resends server data to client
void command_resend( pClient client )
{
	pChar pc = client->currChar();

	pc->teleport();
}

//! Teleport a client
void command_teleport( pClient client )
{
	pChar pc = client->currChar();

	pc->teleport();
}

//! Prints your current coordinates + region
void command_where( pClient client )
{
	pChar pc = client->currChar();

	Location pos = pc->getPosition();

	if (strlen(region[pc->region].name)>0)
		pc->sysmsg("You are at: %s", region[pc_cs->region].name);
	else
		pc->sysmsg("You are at: unknown area");

	pc->sysmsg("%d %d (%d)", pos.x, pos.y, pos.z);
}

//! Shows the GM or Counsellor queue.
void command_q( pClient client )
{
	pChar pc = client->currChar();

#error //FLAME!!!
	if (!pc->IsGM()) //They are not a GM
		Commands::ShowGMQue(s, 0);
	else
		Commands::ShowGMQue(s, 1); // They are a GM
}

//! For Counselors or GM's, goes to next call in queue.
void command_next( pClient client )
{
	pChar pc = client->currChar();

#error //FLAME!!!
	if (!pc->IsGM()) //They are not a GM
	   Commands::NextCall(s, 0);
	else
	   Commands::NextCall(s, 1); // They are a GM
}

//! For Counselor's and GM's, removes current call from queue.
void command_clear( pClient client )
{
	pChar pc = client->currChar();

#error //FLAME!!!
	if (!pc->IsGM()) //They are not a GM
	   donewithcall(s, 0);
	else
	   donewithcall(s, 1); // They are a GM
}

//! (d) Teleports you to a location from the LOCATIONS.SCP file.
void command_goplace( pClient client )
{
	pChar pc = client->currChar();

	if (tnum==2)
	{
		pc->doGmEffect();

		pc->goPlace( strtonum(1) );
		pc->teleport();

		pc->doGmEffect();
	}
}

//! (h h h h) Teleports you to another character.
void command_gochar( pClient client )
{
	pChar pc = client->currChar();

	PC_CHAR pc_i=NULL;
#error //FLAME !!!

	switch( tnum ) {
		case 5:	{
			Serial serial;
			serial.ser1=strtonum(1);
			serial.ser2=strtonum(2);
			serial.ser3=strtonum(3);
			serial.ser4=strtonum(4);

			pc_i = pointers::findCharBySerial( serial.serial32 );
			break;
		}
		case 2: {
			NXWCLIENT ps_target = getClientFromSocket( strtonum(1) );
			if( ps_target == NULL )
				return;

			pc_i = ps_target->currChar();
			break;
		}
		default:
			return;
	}

	if( ISVALIDPC(pc_i) ) {

		pc->doGmEffect();

		pc->MoveTo( pc_i->getPosition() );
		pc->teleport();

		pc->doGmEffect();
	}
}

//! Try to compensate for messed up Z coordinates. Use this if you find yourself half-embedded in the ground.
void command_fix( pClient client )
{
	pChar pc = client->currChar();

	Location pos = pc->getPosition();

	if (err)
		return;

	if (tnum == 2)
	{
		if (validtelepos(pc)==-1)
			pos.dispz = pos.z = strtonum(1);
		else
			pos.dispz = pos.z = validtelepos(pc);

		pc->teleport();
	}

	pc_cs->setPosition(pos);
}

void target_xgo( P_TARGET t )
{
	P_CHAR pc = pointers::findCharBySerial( t->getClicked() );
	if(ISVALIDPC(pc))
	{
		pc->MoveTo( t->buffer[0], t->buffer[1], t->buffer[2] );
		pc->teleport();
	}
}

//! (d) Send another character to a location in your LOCATIONS.SCP file.
void command_xgoplace( pClient client )
{
#error //FLAMEEEEE !!!!
	if (tnum==2)
	{
		int x, y, z;
		location2xyz( strtonum(1), x, y, z );
		if( x>0 )
		{
			NXWSOCKET s = ps->toInt();
			P_TARGET targ = clientInfo[s]->newTarget( new cCharTarget() );
			targ->code_callback = target_xgo;
			targ->buffer[0]=x;
			targ->buffer[1]=y;
			targ->buffer[2]=z;
			targ->send( ps );
			ps->sysmsg( "Select char to teleport.");
		}
	}

}

//! Display the serial number of every item on your screen.
void command_showids( pClient client )
{
	pChar pc = client->currChar();

	NxwCharWrapper sc;
	sc.fillCharsNearXYZ( pc->getPosition() );
	for( sc.rewind(); !sc.isEmpty(); sc++ ) {
		P_CHAR pc_sc=sc.getChar();
		if(ISVALIDPC(pc_sc)) {
			pc_sc->showLongName( pc, true );
		}
	}

}

//! (h h) Polymorph yourself into any other creature.
void command_poly( pClient client )
{
	pChar pc = client->currChar();

	if (tnum==3)
	{
		int k,c1;

                k = (strtonum(1)<<8) | strtonum(2);
                if (k>=0x000 && k<=0x3E1) // Lord Binary, body-values >0x3e crash the client
		{
			pc->setId(k);
			pc->setOldId(k);

			c1= pc->getColor();		// transparency for mosnters allowed, not for players,
							// if polymorphing from monster to player we have to switch from transparent to semi-transparent
							// or we have that sit-down-client crash

			if ((c1 & 0x4000) && (k >= BODY_MALE && k<= 0x03E1))
			{
				if (c1!=0x8000)
				{
					pc->setColor(0xF000);
					pc->setOldColor(0xF000);
				}
			}
		}
	}

	pc->teleport();
}

// (h h) Change the hue of your skin.
void command_skin( pClient client )
{
	if (tnum == 3)
	{
		UI16 k, body;

		pChar pc = client->currChar();

		body = pc->getId();
		k = (strtonum(1,16) << 8) | strtonum(2,16);
		if ((k & 0x4000) && (body >= BODY_MALE && body <= 0x03E1))
			k = 0xF000;

		if (k != 0x8000)
		{
			pc->setColor(k);
			pc->setOldColor(k);
			pc->teleport();
		}
	}
}

//! (h) Preform an animated action sequence.
void command_action( pClient client )
{
	if (tnum==2)
	{
		pChar pc = client->currChar();
		pc->playAction(strtonum(1));
	}
}

// Sets the season globally
// Season change packet structure:
// BYTE 0xBC	(Season change command)
// BYTE season	(0 spring, 1 summer, 2 fall, 3 winter, 4 dead, 5 unknown (rotating?))
// BYTE unknown	If 0, cannot change from undead, so 1 is the default
void command_setseason( pClient client )
{
	pChar pc = client->currChar();

#warn ///FLAME? 
	UI08 setseason[3]={ 0xBC, 0x00, 0x01 };

	pc->sysmsg("Plz, notice that setseason may or may not work correctly depending on current calendar status");

	if(tnum==2)
	{
		setseason[1]=strtonum(1);
		season=(int)setseason[1];
		NxwSocketWrapper sw;
		sw.fillOnline();
		for( sw.rewind(); !sw.isEmpty(); sw++ ) {
			Xsend( sw.getSocket(), setseason, 3);
//AoS/			Network->FlushBuffer(sw.getSocket());
		}
	}
	else
		pc->sysmsg("Setseason takes one argument.");
}

void target_xtele( pClient client, P_TARGET t )
{
	pChar pc = client->currChar();
	pChar pc_t = pointers::findCharBySerial( t->getClicked() );


	pc_t->MoveTo( pc->getPosition() );
	pc_t->teleport();
}

//! Teleport a player to your position.
void command_xtele( pClient client )
{
#error ///FLAMEEEE !!!!!! ^.^"
	P_TARGET targ = clientInfo[ps->toInt()]->newTarget( new cCharTarget() );
	targ->code_callback = target_xtele;
	targ->send( ps );
	ps->sysmsg( "Select char to teleport to your position." );
}

//! (d d d) Go to the specified X/Y/Z coordinates
void command_go( pClient client )
{
	if (tnum==4)
	{
		int x=strtonum(1);int y=strtonum(2);int z=strtonum(3); // LB
		if (x<6144 && y<4096 && z>-127 && z<128)
		{

			pChar pc = client->currChar();
			pc->doGmEffect(); 	// Adding the gmmove effects ..Aldur

			pc->MoveTo( x, y, z );
			pc->teleport();

			pc->doGmEffect();
		}
	}
}

/*!
\brief Sets all PK counters to 0.
\todo This function is commented out. Should be wrote or deleted.
*/
void command_zerokills( pClient client )
{
                        return;
                        /*sysmessage(s,"Zeroing all player kills...");
			for(int a=0;a<charcount;a++)
			{
				P_CHAR pc = MAKE_CHARREF_LR(a);
				pc->kills=0;
				setcharflag(pc);//AntiChrist
			}
			sysmessage(s,"All player kills are now 0.");*/
}

//! Shows character appetite
void command_appetite( pClient client )
{
	pChar pc = client->currChar();

	switch( pc->IsGMorCounselor()? 6 : pc->hunger )
	{
		case 6:
		case 5: pc->sysmsg(TRANSLATE("You are still stuffed from your last meal"));
						break;
		case 4: pc->sysmsg(TRANSLATE("You are not very hungry but could eat more"));
						break;
		case 3: pc->sysmsg(TRANSLATE("You are feeling fairly hungry"));
						break;
		case 2: pc->sysmsg(TRANSLATE("You are extremely hungry"));
						break;
		case 1: pc->sysmsg(TRANSLATE("You are very weak from starvation"));
						break;
		case 0:	pc->sysmsg(TRANSLATE("You must eat very soon or you will die!"));
						break;
	}
}

//! Adds an item when using 'add # #
void target_addTarget( P_TARGET t )
{
	P_ITEM pi = item::CreateFromScript( "$item_hardcoded" );
	VALIDATEPI(pi);

	UI16 id = DBYTE2WORD( t->buffer[0], t->buffer[1] );

	tile_st tile;
	data::seekTile( id, tile );

	pi->setId( id );
	pi->pileable = tile.flags&TILEFLAG_STACKABLE;
	pi->setDecay( false );

	Location pos = t->getLocation();
	pos.z += tileHeight(t->getModel());

	pi->MoveTo( pos );
	pi->Refresh();
}

//! (h h) Adds a new item, or opens the GM menu if no hex codes are specified.
void command_add( pClient client )
{
	pClient pc = client->currChar();

	if( tnum==1 )
	{
		showAddMenu( pc, 1 );
	}
	else if( tnum==3 )
	{
		UI32 a;
		UI32 b;
		if ( server_data.always_add_hex ) {
			a=hexnumber(1);
			b=hexnumber(2);
		}
		else {
			a=strtonum(1);
			b=strtonum(2);
		}
#error ///FLAMEEEEEEE !!!!!!!!!!! gh
		clientInfo[ps->toInt()]->resetTarget();

		P_TARGET trg = clientInfo[ps->toInt()]->newTarget( new cLocationTarget() );
		ps->sysmsg( "Select location for item..." );
		trg->buffer[0]=a;
		trg->buffer[1]=b;
		trg->code_callback = target_addTarget;
		trg->send( ps );
	}
	else
		ps->sysmsg( "Syntax error. Usage: /add <id1> <id2>" );
}

//! (h h) Adds a new item to your current location.
void command_addx( pClient client )
{
	pChar pc = client->currChar();

#error ///FLAMEEEEEEE !!!!!!!!!!! gh
//	NXWSOCKET s = ps->toInt();

	if (tnum==3)
		Commands::AddHere(s, DBYTE2WORD( strtonum(1), strtonum(2) ), pc->getPosition("z"));

	if (tnum==4)
		Commands::AddHere(s, DBYTE2WORD( strtonum(1), strtonum(2) ), strtonum(3));

}


void target_rename( P_TARGET t )
{
	P_OBJECT po = objects.findObject( t->getClicked() );
	VALIDATEPO( po );

	po->setCurrentName( t->buffer_str[0] );
}


//! (text) Renames any dynamic object in the game.
void command_rename( pClient client )
{
	if (tnum>1)
	{
#error ///FLAMEEEEEEE !!!!!!!!!!! gh
//	NXWSOCKET s = ps->toInt();
		P_TARGET targ = clientInfo[ps->toInt()]->newTarget( new cObjectTarget() );
		targ->buffer_str[0] = &tbuffer[Commands::cmd_offset+7];
		targ->code_callback = target_rename;
		targ->send( ps );
		ps->sysmsg( "Select object to rename..." );

	}
}


//! (text) Renames any dynamic item in the game.
void command_cfg( pClient client )
{
	pChar pc = client->currChar();

	if (tnum>1)
	{
		int n = ifcfg_command(&tbuffer[Commands::cmd_offset+4]);
		if (n != 0)
//			sysmessage(s, 0x799, "Error number : %d", n);
#warn !!! TODO !!! cChar::sysmsg(TXT_COLOR, "format", ... )
			pc->sysmsg(0x799, "Error number : %d", n);
	}
}


//! Saves the current world data into NXWITEMS.WSC and NXWCHARS.WSC.
void command_save()
{
	cwmWorldState->saveNewWorld();
}

void target_setpriv( pClient client, P_TARGET t )
{
	pChar pc, pc_t;

	pc = ps->currChar();
	pc_t = pointers::findCharBySerial( t->getClicked() );
	VALIDATEPC(pc_t);

	if (SrvParms->gm_log)   //Logging
		WriteGMLog(curr, "%s as given %s Priv [%x][%x]\n", pc->getCurrentNameC(), pc_t->getCurrentNameC(), t->buffer[0], t->buffer[1] );

    pc->SetPriv( t->buffer[0] );
    pc->SetPriv2( t->buffer[1] );
}

void target_setprivItem( P_TARGET t )
{
	P_ITEM pi=pointers::findItemBySerial( t->getClicked() );
	VALIDATEPI(pi);

	switch( t->buffer[0] )
	{
        case 0	:
			pi->setDecay( false );
			pi->setNewbie();
			pi->setDispellable();
			break;
        case 1	:
			pi->setDecay();
			break;
        case 3	:
			pi->priv = t->buffer[1];
			break;
	}
}


void command_setpriv( pClient client )
// (Chars/NPCs: h h, Objects: h) Sets priviliges on a Character or object.
{
	if (tnum==3)
	{
#error //FLAMEEEEEEEEE!
		P_TARGET targ = clientInfo[ ps->toInt() ]->newTarget( new cCharTarget() );
		targ->code_callback = target_setpriv;
		targ->buffer[0]=strtonum(1);
		targ->buffer[1]=strtonum(2);
		targ->send( ps );
		ps->sysmsg( "Select char to edit priv." );
	}
	else if (tnum==2)
	{
		P_TARGET targ = clientInfo[ ps->toInt() ]->newTarget( new cItemTarget() );
		targ->code_callback = target_setprivItem;
		targ->buffer[0]=3;
		targ->buffer[1]=strtonum(1);
		targ->send( ps );
		ps->sysmsg( "Select object to edit priv." );
	}
}

void command_nodecay( pClient client )
// Prevents an object from ever decaying.
{
#error //FLAMEEEEEEEEE!
	P_TARGET targ = clientInfo[ps->toInt()]->newTarget( new cItemTarget() );
	targ->code_callback=target_setprivItem;
	targ->buffer[0]=0;
	targ->send( ps );
	ps->sysmsg( "Select object to make permenant." );
}


// Displays the current UO time.
void command_showtime( pClient client )
{
	pChar pc = client->currChar();

	int hour = Calendar::g_nHour % 12;
	if (hour==0) hour = 12;
	int ampm = (Calendar::g_nHour>=12) ? 1 : 0;
	int minute = Calendar::g_nMinute;

	if (ampm || (!ampm && hour==12))
		pc->sysmsg("%s %2.2d %s %2.2d %s", "NoX-Wizard: Time: ", hour, ":", minute, "PM");
	else
		pc->sysmsg("%s %2.2d %s %2.2d %s", "NoX-Wizard: Time: ", hour, ":", minute, "AM");
}

// (d d) Sets the current UO time in hours and minutes.
void command_settime()
{
UI32 newhours, newminutes;

	if (tnum==3)
	{
		newhours = strtonum(1);
		newminutes = strtonum(2);

		if ((newhours < 25) && (newhours > 0) && (newminutes >= 0) && (newminutes <60))
		{
			Calendar::g_nHour = newhours;
			Calendar::g_nMinute = newminutes;
		}
	}
}

// (d) Shuts down the server. Argument is how many minutes until shutdown.
void command_shutdown()
{
	if (tnum==2)
	{
		endtime=uiCurrentTime+(MY_CLOCKS_PER_SEC*strtonum(1));
		if (strtonum(1)==0)
		{
			endtime=0;
			sysbroadcast(TRANSLATE("Shutdown has been interrupted."));
		}
		else endmessage(0);
	}
}

// List of all online player
void command_playerlist( pClient client )
{
#error ///FLAMEEEE !
	NXWSOCKET s = ps->toInt();

	static AmxFunction* cmd = NULL;
	if(!cmd)
		cmd = new AmxFunction( "command_playerlist" );

	cmd->Call( s );
}

void command_skills( pClient client )
{
#error ///FLAMEEEE !

	NXWSOCKET s = ps->toInt();

	static AmxFunction* cmd = NULL;
	if(!cmd)
		cmd = new AmxFunction( "command_skills" );

	cmd->Call( s );
}

// (h h) Plays the specified sound effect.
void command_sfx( pClient client )
{
	if (tnum==3)
		client->playSFX((strtonum(1)<<8)|(strtonum(2)%256));
}

// (h) Sets the light level. 0=brightest, 15=darkest, -1=enable day/night cycles.
void command_light()
{
	if (tnum==2)
	{
		worldfixedlevel=strtonum(1);

		if (worldfixedlevel!=255)
			setabovelight(worldfixedlevel);
		else 
			setabovelight(worldcurlevel);
	}
}

// Debugging command.
void command_web( pClient client )

	string urlz(&tbuffer[Commands::cmd_offset+4]);
	cPacketSendOpenBrowser openUrl(urlz);

	if (tnum>1)
		client->sendPacket(&openUrl);
}


// (d) Disconnects the user logged in under the specified slot.
void command_disconnect()
{
	if (tnum==2) 
		Network->Disconnect(strtonum(1));
}

// (d text) Sends an anonymous message to the user logged in under the specified slot.
void command_tell( pClient client )
{
#error ///FLAMEEEE

	NXWSOCKET s = ps->toInt();

	if (tnum>2)
	{
		int m=strtonum(1);
		if (m<0)
			sysbroadcast(&tbuffer[Commands::cmd_offset+6]);
		else 
			tellmessage(s, strtonum(1), &tbuffer[Commands::cmd_offset+6]);
	}
}

// Set weather to dry (no rain or snow).
void command_dry( pClient client )
{
	wtype=0;
	NxwSocketWrapper sw;
	sw.fillOnline();
	for( sw.rewind(); !sw.isEmpty(); sw++ ) {
		NXWCLIENT ps=sw.getClient();
		if( ps!=NULL )
			weather( ps->toInt(), 0 );
	}
}


// Sets the weather condition to rain.
void command_rain( pClient client )
{
	NxwSocketWrapper sw;

	if (wtype==2)
	{
		wtype=0;
		sw.fillOnline();
		for( sw.rewind(); !sw.isEmpty(); sw++ ) {
			NXWCLIENT ps=sw.getClient();
			if( ps!=NULL )
				weather( ps->toInt(), 0 );
		}
	}

	wtype=1;

	sw.clear();
	sw.fillOnline();
	for( sw.rewind(); !sw.isEmpty(); sw++ ) {
		NXWCLIENT ps=sw.getClient();
		if( ps!=NULL )
			weather( ps->toInt(), 0 );
	}
}

// Sets the weather condition to snow.
void command_snow( pClient client )
{
	NxwSocketWrapper sw;
	if (wtype==1)
	{
		wtype=0;
		sw.fillOnline();
		for( sw.rewind(); !sw.isEmpty(); sw++ ) {
			NXWCLIENT ps=sw.getClient();
			if( ps!=NULL )
				weather( ps->toInt(), 0 );
		}
	}

	wtype=2;
	sw.clear();
	sw.fillOnline();
	for( sw.rewind(); !sw.isEmpty(); sw++ ) {
		NXWCLIENT ps=sw.getClient();
		if( ps!=NULL )
			weather( ps->toInt(), 0 );
	}
}

// (d) Opens the specified GM Menu.
void command_gmmenu( pClient client )
{

	NXWSOCKET s = ps->toInt();

	if (tnum==2) gmmenu(s, strtonum(1));
}

// (d) Opens the specified Item Menu from ITEMS.SCP.
void command_itemmenu( pClient client )
{
	pChar pc = client->currChar();

	if (tnum==2)
		showAddMenu(pc, strtonum(1)); //itemmenu(s, strtonum(1));

}

void target_additem( pClient client, P_TARGET t )
{
	Location loc=t->getLocation();

	P_ITEM pi = item::CreateFromScript( t->buffer[0], NULL, INVALID );
	VALIDATEPI(pi);

	pi->MoveTo( loc );
	pi->Refresh();
}

void menu_additem( P_MENU menu, pClient client, SERIAL button )
{
	if( button<=MENU_CLOSE )
		return;

	if( button!=1 )
		return;

	cMenu* m = (cMenu*)menu;

	std::wstring* w = m->getText( 6 );
	if( w!=NULL ) {
		string define;
		wstring2string( *w, define );
	}
}

// (d) Adds the specified item from ITEMS.XSS
void command_additem( pClient client )
{
	pChar pc = client->currChar();

#error ///FLAMEEEE !
/*	NXWSOCKET s = ps->toInt();
	P_CHAR pc = MAKE_CHAR_REF(currchar[s]);
	VALIDATEPC(pc);    */
	if (tnum==2)
	{
		int item=0;
		if( tbuffer[Commands::cmd_offset+8] == '$')
		{
			if( !evaluateOneDefine(&tbuffer[Commands::cmd_offset+8]) )
			{
				pc->sysmsg("Item symbol %s undefined !", &tbuffer[Commands::cmd_offset+8]);
				return;
			}
			item = str2num(&tbuffer[Commands::cmd_offset+8],10);
		}
		else
		{
			item = strtonum(1);
		}

		P_TARGET targ=clientInfo[s]->newTarget( new cLocationTarget() );
		targ->buffer[0]=item;
		targ->code_callback=target_additem;
		targ->send( getClientFromSocket(s) );
		pc->sysmsg("Select location for item. [Number: %i]", item);
	}
	else {
		cMenu* menu = (cMenu*)Menus.insertMenu( new cMenu( MENUTYPE_CUSTOM, 50, 50, true, true, true ) );
		VALIDATEPM( menu );

		menu->hard = menu_additem;

		menu->addText( 10, 10, wstring( L"Search for" ) );
		menu->addInputField( 30, 30, 80, 80, 6, wstring( L"write here name" ) );
		menu->addButton( 5, 5, 0x0850, 0x0851, 1, true );

		menu->show( pc );
	}
}

void target_dupe( pClient client, P_TARGET t )
{
    int n = t->buffer[0];
	if( n>=1)
    {
        P_ITEM pi=pointers::findItemBySerial( t->getClicked() );
        if (ISVALIDPI(pi))
        {
            for (int j=0; j<n; j++ )
            {
                Commands::DupeItem( ps->toInt(), pi->getSerial32(), pi->amount );
                ps->sysmsg( "DupeItem done.");
            }
        }
    }
}

void command_dupe( pClient client )
// (d / nothing) Duplicates an item. If a parameter is specified, it's how many copies to make.
{
#error ///FLAME
	P_TARGET targ = clientInfo[ps->toInt()]->newTarget( new cItemTarget() );
	targ->buffer[0] = (tnum==2)? strtonum(1) : 1;
	targ->code_callback=target_dupe;
	targ->send( ps );
	ps->sysmsg( "Select an item to dupe." );
}


// Executes a trigger scripting command.
void command_command( pClient client )
{
#error ///FLAMEEE

	NXWSOCKET s = ps->toInt();

			int i;
			int loopexit=0;
			char c=0;
			if (tnum>1)
			{
				i=0;
				script1[0]=0;
				script2[0]=0;
				while(tbuffer[Commands::cmd_offset+8+i]!=' ' && tbuffer[Commands::cmd_offset+8+i]!=0 && (++loopexit < MAXLOOPS) ) i++;
				strncpy(script1,&tbuffer[Commands::cmd_offset+8],i);
				script1[i]=0;
				if ((script1[0]!='}')&&(c!=0)) strcpy(script2, &tbuffer[Commands::cmd_offset+8+i+1]);
				scriptcommand(s, script1, script2);
			}
			return;
}

// Runs garbage collection routines.
void command_gcollect( pClient client )
{
	pChar pc = client->currChar();

	gcollect();
	pc->sysmsg("command succesfull");
}

// Enables GM ability to pick up all objects.
void command_allmoveon( pClient client )
{
	pChar pc = client->currChar();

	pc->setAllMove(true);
	pc->teleport();
	pc->sysmsg("ALLMOVE enabled.");
}

// Disables GM ability to pick up all objects.
void command_allmoveoff( pClient client )
{
	pChar pc = client->currChar();

	pc->setAllMove(false);
	pc->teleport();
	pc->sysmsg("ALLMOVE disabled.");
}

// Makes houses appear as deeds. (The walls disappear and there's a deed on the ground in their place.)
void command_showhs( pClient client )
{
	pChar pc = client->currChar();

	pc->setViewHouseIcon(true);
	pc->teleport();
	pc->sysmsg("House icons visible. (Houses invisible)");
}

// Makes houses appear as houses (opposite of /SHOWHS).
void command_hidehs( pClient client )
{
	pChar pc = client->currChar();

	pc->setViewHouseIcon(false);
	pc->teleport();
	pc->sysmsg("House icons hidden. (Houses visible)");
}


void target_allSet( pClient client, P_TARGET t )
{
#error ///FLAMEEEE
	P_CHAR pc = pointers::findCharBySerial( t->getClicked() );
	VALIDATEPC(pc);

	NXWSOCKET k = ps->toInt();

	if( t->buffer_str[0]=="STR" ) {
		pc->setStrength( t->buffer[0] );
		pc->st3=t->buffer[0];
		for( int j=0;j<TRUESKILLS;j++)
		{
			Skills::updateSkillLevel(pc,j);
			updateskill(k,j);
		}
		statwindow(pc,pc);
	}
	else if( t->buffer_str[0]=="DEX" ) {
		pc->dx=t->buffer[0];
		pc->dx3=t->buffer[0];
		for( int j=0;j<TRUESKILLS;j++)
		{
			Skills::updateSkillLevel(pc,j);
			updateskill(k,j);
		}
		statwindow(pc,pc);
	}
	else if( t->buffer_str[0]=="INT" ) {
		pc->in=t->buffer[0];
		pc->in3=t->buffer[0];
		for( int j=0;j<TRUESKILLS;j++)
		{
			Skills::updateSkillLevel(pc,j);
			updateskill(k,j);
		}
		statwindow(pc,pc);
	}
	else if( t->buffer_str[0]=="FAME" ) {
		pc->SetFame(t->buffer[0]);
	}
	else if( t->buffer_str[0]=="KARMA" ) {
		pc->SetKarma(t->buffer[0]);
	}
	else {
		for( int j=0;j<TRUESKILLS;j++)
		{
			if( t->buffer_str[0]==skillname[j] ) {
				pc->baseskill[j]=t->buffer[0];
				Skills::updateSkillLevel(pc,j);
				updateskill(k,j);
				statwindow(pc,pc);

				break;
			}
		}
	}
}

// (text, d) Set STR/DEX/INT/Skills on yourself arguments are skill & amount.
void command_set( pClient client )
{
	pChar pc = client->currChar();

	if (tnum==3)
	{
		std::string prop;
		int i=Commands::cmd_offset+4;
		int loopexit=0;
		while( tbuffer[i++]!=0 && (++loopexit < MAXLOOPS) );

		char b[TEMP_STR_SIZE];
		memcpy( b, &tbuffer[Commands::cmd_offset+4], i-(Commands::cmd_offset+4)+1 );
		prop = b;
		strupr(prop);
		std::string propName, propValue;
		splitLine( prop, propName, propValue );

#error ///FLAMEEE
		P_TARGET targ=clientInfo[s]->newTarget( new cCharTarget() );
		targ->buffer_str[0]=propName;
		targ->buffer[0]=str2num( propValue );
		targ->code_callback=target_allSet;
		targ->send( getClientFromSocket(s) );
		pc->sysmsg("Select character to modify.");

	}
}


void target_addNpc( pClient client, P_TARGET t )
{
	if( t->buffer[0]==0 ) {

		pChar pc = new cChar(cChar::nextSerial());

		pc->setCurrentName("Dummy");
		pc->setId( DBYTE2WORD( t->buffer[0], t->buffer[1] ) );
		pc->setOldId( DBYTE2WORD( t->buffer[0], t->buffer[1] ) );
		pc->setColor(0);
		pc->setOldColor(0);
		pc->SetPriv(0x10);

		Location loc = t->getLocation();
		loc.z += tileHeight( t->getModel() );
		pc->MoveTo( loc );
		pc->npc=1;
		pc->teleport();
	}
	else {
#error ///FLAMEE!
		npcs::AddNPCxyz( ps->toInt(), t->buffer[1], t->getLocation() );
	}
}

void command_addnpc( pClient client )
{
	pChar pc = client->currChar();

#error ///FLAME
	NXWSOCKET s = ps->toInt();

	P_TARGET targ = clientInfo[s]->newTarget( new cLocationTarget() );
	if (tnum==3)
	{
		targ->buffer[0]=0;
		targ->buffer[1]=strtonum(1);
		targ->buffer[2]=strtonum(2);
	}
	else if (tnum==2)
	{
		if ( tbuffer[Commands::cmd_offset+7] == '$')
		{
			if( !evaluateOneDefine(&tbuffer[Commands::cmd_offset+7]) )
			{
				pc->sysmsg("Char symbol %s undefined !", &tbuffer[Commands::cmd_offset+8]);
				clientInfo[s]->resetTarget();
				return;
			}
			targ->buffer[1] = str2num(&tbuffer[Commands::cmd_offset+7],10);
		}
		else
		{
			targ->buffer[1]=strtonum(1);
		}
		targ->buffer[0]=1;
	}
	targ->code_callback=target_addNpc;
	targ->send( ps );
	ps->sysmsg( "Select location for the NPC.");
}


void command_tweak( pClient client )
{
#error ///FLAME
	NXWSOCKET s = ps->toInt();

	static AmxFunction* cmd = NULL;
	if( cmd==NULL )
		cmd = new AmxFunction( "command_tweak" );

	cmd->Call( s );
}


// (d) Sets the number of real-world seconds that pass for each UO minute.
void command_secondsperuominute( pClient client )
{
	pChar pc = client->currChar();

	if (tnum==2)
	{
		secondsperuominute = strtonum(1);
		pc->sysmsg("Seconds per UO minute set.");
	}
}

// (h) Sets default daylight level.
void command_brightlight( pClient client )
{
	pChar pc = client->currChar();

	if (tnum==2)
	{
		worldbrightlevel = strtonum(1);
		pc->sysmg("World bright light level set.");
	}
}

// (h) Sets default nighttime light level.
void command_darklight( pClient client )
{
	pChar pc = client->currChar();

	if (tnum==2)
	{
		worlddarklevel = strtonum(1);
		pc->sysmsg("World dark light level set.");
	}
}

// (h) Sets default dungeon light level.
void command_dungeonlight( pClient client )
{
	pChar pc = client->currChar();

	if (tnum==2)
	{
		dungeonlightlevel = qmin(strtonum(1), 27);
		pc->sysmsg("Dungeon light level set.");
	}
}

// Forces a manual vendor restock.
void command_restock( pClient client )
{
	pChar pc = client->currChar();

	Restocks->doRestock();
	pc->sysmsg("Manual shop restock has occurred.");
}

// Forces a manual vendor restock to maximum values.
void command_restockall( pClient client )
{
	pChar pc = client->currChar();

	Restocks->doRestockAll();
	pc->sysmsg("Restocking all shops to their maximums");
}

// (d) Sets the universe's shop restock rate.
void command_setshoprestockrate( pClient client )
{
	pChar pc = client->currChar();

	if (tnum==2)
		pc->sysmsg("NPC shop restock rate IS DISABLED.");
	else
		pc->sysmsg("Invalid number of parameters.");

}

// (d d) Plays the specified MIDI file.
void command_midi( pClient client )
{
	pChar pc = client->currChar();
	UI16 midi = DBYTE2WORD(strtonum(1), strtonum(2));

	cPacketSendPlayMidi pkt_midi(midi);
	client->sendPacket(&pkt_midi);	
/*			if (tnum==3) playmidi(s, strtonum(1), strtonum(2));
			return;   */
}

// Forces a respawn.
void command_respawn( pClient client )
{
	pChar pc = client->currChar();

	sysbroadcast(TRANSLATE("World is now respawning, expect some lag!"));
	LogMessage("Respawn command called by %s.\n", pc->getCurrentNameC());
	//Respawn->Start();
}

// (d) Spawns in all regions up to the specified maximum number of NPCs/Items.
void command_regspawnmax( pClient client )
{
#error // FLAMEE !
	pChar pc = client->currChar();
/*	NXWSOCKET s = ps->toInt();
	P_CHAR pc=MAKE_CHAR_REF(currchar[s]);
	VALIDATEPC(pc); */

	if (tnum==2)
	{
		LogMessage("MAX Region Respawn command called by %s.\n", pc->getCurrentNameC());//AntiChrist
		Commands::RegSpawnMax(s, strtonum(1));
	}
}

// (d d) Preforms a region spawn. First argument is region, second argument is max # of items/NPCs to spawn in that region.
void command_regspawn( pClient client )
{
#error // FLAMEE !
	pChar pc = client->currChar();

/*	NXWSOCKET s = ps->toInt();
	P_CHAR pc=MAKE_CHAR_REF(currchar[s]);
	VALIDATEPC(pc);*/

	if (tnum==3)
	{
		LogMessage("Specific Region Respawn command called by %s.\n", pc->getCurrentNameC());
		Commands::RegSpawnNum(s, strtonum(1), strtonum(2));
	}
}

void command_regedit( pClient client ) // O_o" to rewrite/trash ?
{
	pChar pc = client->currChar();

	LogMessage("Region edit command called by %s.\n", pc->getCurrentNameC());
	//newAmxEvent("gui_rgnList")->Call( pc->getSerial32(), 0 );
}

// Reloads the SERVER.cfg file.
void command_reloadserver( pClient client )
{
	pChar pc = client->currChar();

	loadserverscript();
	pc->sysmsg("server.cfg reloaded.");
}

// Loads the server defaults.
void command_loaddefaults( pClient client )
{
	pChar pc = client->currChar();

	loadserverdefaults();
	pc->sysmsg("server defaults reloaded.");
}

// Display the counselor queue.
void command_cq( pClient client )
{
#error ///FLAMEEE
	NXWSOCKET s = ps->toInt();

	Commands::ShowGMQue(s, 0); // Show the Counselor queue, not GM queue
}

/*GH!*/
// Attend to the next call in the counselor queue.
void command_cnext( pClient client )
{

	NXWSOCKET s = ps->toInt();

	Commands::NextCall(s, 0); // Show the Counselor queue, not GM queue

}

// Remove the current call from the counselor queue.
void command_cclear( pClient client )
{

	NXWSOCKET s = ps->toInt();

	donewithcall(s, 0); // Show the Counselor queue, not GM queue
}

// (d) Set the server mine check interval in minutes.
void command_minecheck( pClient client )
{
	if (tnum==2)
		SrvParms->minecheck=strtonum(1);
}


void target_invul( pClient client, P_TARGET t )
{
	P_CHAR pc=pointers::findCharBySerial( t->getClicked() );
	VALIDATEPC(pc);

	if( t->buffer[0]==1 )
		pc->MakeInvulnerable();
	else
		pc->MakeVulnerable();
}

// Makes the targeted character immortal.
void command_invul( pClient client )
{

	NXWSOCKET s = ps->toInt();

	P_TARGET targ=clientInfo[s]->newTarget( new cCharTarget() );
	targ->code_callback=target_invul;
	targ->buffer[0]=1;
	targ->send( getClientFromSocket(s) );
	sysmessage( s, "Select creature to make invulnerable.");
}

// Makes the targeted character mortal.
void command_noinvul( pClient client )
{

	NXWSOCKET s = ps->toInt();

	P_TARGET targ=clientInfo[s]->newTarget( new cCharTarget() );
	targ->code_callback=target_invul;
	targ->buffer[0]=0;
	targ->send( getClientFromSocket(s) );
	sysmessage( s, "Select creature to make mortal again.");

}

// Activates town guards.
void command_guardson( pClient client )
{
	SrvParms->guardsactive=1;
	sysbroadcast(TRANSLATE("Guards have been reactivated."));

}

// Deactivates town guards.
void command_guardsoff( pClient client )
{
	SrvParms->guardsactive=0;
	sysbroadcast(TRANSLATE("Warning: Guards have been deactivated globally."));

}

// Enables decay on an object.
void command_decay( pClient client )
{
	P_TARGET targ=clientInfo[ps->toInt()]->newTarget( new cItemTarget() );
	targ->code_callback=target_setprivItem;
	targ->buffer[0]=1;
	targ->send( ps );
	ps->sysmsg( "Select object to decay.");

}

// Display some performance information.
void command_pdump( pClient client )
{

	NXWSOCKET s = ps->toInt();

	sysmessage(s, "Performace Dump:");

	sprintf(s_szCmdTableTemp, "Network code: %fmsec [%i]" , (float)((float)networkTime/(float)networkTimeCount) , networkTimeCount);
	sysmessage(s, s_szCmdTableTemp);

	sprintf(s_szCmdTableTemp, "Timer code: %fmsec [%i]" , (float)((float)timerTime/(float)timerTimeCount) , timerTimeCount);
	sysmessage(s, s_szCmdTableTemp);

	sprintf(s_szCmdTableTemp, "Auto code: %fmsec [%i]" , (float)((float)autoTime/(float)autoTimeCount) , autoTimeCount);
	sysmessage(s, s_szCmdTableTemp);

	sprintf(s_szCmdTableTemp, "Loop Time: %fmsec [%i]" , (float)((float)loopTime/(float)loopTimeCount) , loopTimeCount);
	sysmessage(s, s_szCmdTableTemp);

	sprintf(s_szCmdTableTemp, "Simulation Cycles/Sec: %f" , (1000.0*(1.0/(float)((float)loopTime/(float)loopTimeCount))));
	sysmessage(s, s_szCmdTableTemp);
}

// (text) GM Yell - Announce a message to all online GMs.
void command_gy( pClient client )
{

	NXWSOCKET s = ps->toInt();

	P_CHAR pc=MAKE_CHAR_REF(currchar[s]);
	VALIDATEPC(pc);

	if(now==1) {
		pc->sysmsg("There are no other users connected.");
		return;
	}

	std::string message( "(GM ONLY): " );
	message+=&tbuffer[Commands::cmd_offset+3];

	UI32 id;
	UI16 model, color, font;

	id = pc->getSerial32();
	model = pc->getId();
	color = ShortFromCharPtr(buffer[s] +4);
	font = (buffer[s][6]<<8)|(pc->fonttype%256);

	UI08 name[30]={ 0x00, };
	strcpy((char *)name, pc->getCurrentNameC());


	NxwSocketWrapper sw;
	sw.fillOnline();
	for( sw.rewind(); !sw.isEmpty(); sw++ ) {
		NXWCLIENT ps_i=sw.getClient();
		if( ps_i==NULL )
			continue;

		P_CHAR pc_i = ps_i->currChar();
		if (ISVALIDPC(pc_i) && pc_i->IsGM())
		{
			NXWSOCKET allz = ps_i->toInt();
			SendSpeechMessagePkt(allz, id, model, 1, color, font, name, (char*)message.c_str() );
		}
	}
}

// (text) GM Yell - Announce a message to all online players.
void command_yell( pClient client )
{

	NXWSOCKET s = ps->toInt();

	P_CHAR pc=MAKE_CHAR_REF(currchar[s]);
	VALIDATEPC(pc);

	if(now==1) {
		pc->sysmsg("There are no other users connected.");
		return;
	}

	std::string message( "(GM MSG): " );
	message += &tbuffer[Commands::cmd_offset+3];

	UI32 id;
	UI16 model, color, font;

	id = pc->getSerial32();
	model = pc->getId();
	color = ShortFromCharPtr(buffer[s] +4);
	font = (buffer[s][6]<<8)|(pc->fonttype%256);

	UI08 name[30]={ 0x00, };
	strcpy((char *)name, pc->getCurrentNameC());

	NxwSocketWrapper sw;
	sw.fillOnline();
	for( sw.rewind(); !sw.isEmpty(); sw++ ) {
		NXWCLIENT ps_i=sw.getClient();
		if( ps_i==NULL )
			continue;

		P_CHAR pc_i = ps_i->currChar();
		if (ISVALIDPC(pc_i) )
		{
			NXWSOCKET allz = ps_i->toInt();
			SendSpeechMessagePkt(allz, id, model, 1, color, font, name, (char*)message.c_str() );
		}
	}
}

void target_squelch( pClient client, P_TARGET t )
{
	P_CHAR curr = ps->currChar();
	VALIDATEPC(curr);

	P_CHAR pc =pointers::findCharBySerial( t->getClicked() );
	if(ISVALIDPC(pc))
    {
        if(pc->IsGM())
        {
            curr->sysmsg(TRANSLATE("You cannot squelch GMs."));
            return;
        }

        if (pc->squelched)
        {
            pc->squelched=0;
            curr->sysmsg(TRANSLATE("Un-squelching..."));
            pc->sysmsg(TRANSLATE("You have been unsquelched!"));
            pc->mutetime=0;
        }
        else
        {
            pc->mutetime=0;
            curr->sysmsg( TRANSLATE("Squelching...") );
            pc->sysmsg( TRANSLATE("You have been squelched!") );

            if( t->buffer[0]!=INVALID )
            {
                pc->mutetime = uiCurrentTime+ t->buffer[0]*MY_CLOCKS_PER_SEC;
                pc->squelched=2;
            }
			else
				pc->squelched=1;
        }
    }
}


void command_squelch( pClient client )
// (d / nothing) Squelchs specified player. (Makes them unnable to speak.)
{

	P_TARGET targ = clientInfo[ ps->toInt() ]->newTarget( new cCharTarget() );

    if (tnum==2)
	{
		int value = strtonum(1);
		if( value>0 )
		{
			targ->buffer[0]=strtonum(1);
		}
		else
			targ->buffer[0]=INVALID;
	}

	targ->code_callback = target_squelch;
	targ->send( ps );
	ps->sysmsg( "Select character to squelch." );

}

// (d) Kills spawns from the specified spawn region in SPAWN.SCP.
void command_spawnkill( pClient client )
{

	NXWSOCKET s = ps->toInt();

	if (tnum==2)
	{
		Commands::KillSpawn(s, strtonum(1));
	}
}

void command_stats( pClient client )
{

	NXWSOCKET s = ps->toInt();

	AmxFunction* cmd=NULL;
	if( cmd==NULL )
		cmd = new AmxFunction("command_stats");

	cmd->Call( s );
}

void command_options( pClient client )
{

	NXWSOCKET s = ps->toInt();

	AmxFunction* cmd=NULL;
	if( cmd==NULL )
		cmd = new AmxFunction("command_options");

	cmd->Call( s );
}


// Goes to the current call in the GM/Counsellor Queue
void command_gotocur( pClient client )
{

	NXWSOCKET s = ps->toInt();

	P_CHAR pc=MAKE_CHAR_REF(currchar[s]);
	VALIDATEPC(pc);

	if(pc->callnum==0)
	{
		pc->sysmsg("You are not currently on a call.");
	}
	else
	{
		P_CHAR pc_i = pointers::findCharBySerial( gmpages[pc->callnum].serial.serial32 );
		if(ISVALIDPC(pc_i))
		{
			Location charpos= pc_i->getPosition();

			pc->MoveTo( charpos );
			pc->sysmsg("Transporting to your current call.");
			pc->teleport();
			return;
		}

		pc_i = pointers::findCharBySerial( counspages[pc->callnum].serial.serial32 );
		if(ISVALIDPC(pc_i))
		{
			Location charpos= pc_i->getPosition();

			pc->MoveTo( charpos );
			sysmessage(s,"Transporting to your current call.");
			pc->teleport();
		}
	}
}

// Escilate a Counsellor Page into the GM Queue
void command_gmtransfer( pClient client )
{

	NXWSOCKET s = ps->toInt();

	P_CHAR pc=MAKE_CHAR_REF(currchar[s]);
	VALIDATEPC(pc);

	int i;
	int x2=0;

	if(pc->callnum!=0)
	{
		if(!pc->IsGM()) //Char is a counselor
		{
			for(i=1;i<MAXPAGES;i++)
			{
				if(gmpages[i].handled==1)
				{
					gmpages[i].handled=0;
					strcpy(gmpages[i].name,counspages[pc->callnum].name);
					strcpy(gmpages[i].reason,counspages[pc->callnum].reason);

					gmpages[i].serial= counspages[pc->callnum].serial;
					time_t current_time = time(0);
					struct tm *local = localtime(&current_time);
					sprintf(gmpages[i].timeofcall, "%02d:%02d:%02d", local->tm_hour, local->tm_min, local->tm_sec);
					x2++;
					break;
				}
			}
			if (x2==0)
			{
				pc->sysmsg("The GM Queue is currently full. Contact the shard operator");
				pc->sysmsg("and ask them to increase the size of the queue.");
			}
			else
			{
				pc->sysmsg("Call successfully transferred to the GM queue.");
				donewithcall(s,1);
			}
		}
		else
		{
			pc->sysmsg("Only Counselors may use this command.");
		}
	}
	else
	{
		pc->sysmsg("You are not currently on a call");
	}
}

// Displays a list of users currently online.
void command_who( pClient client )
{

	NXWSOCKET s = ps->toInt();

	P_CHAR pc=MAKE_CHAR_REF(currchar[s]);
	VALIDATEPC(pc);

	if(now==1) {
		pc->sysmsg("There are no other users connected.");
		return;
	}

	int j=0;
	pc->sysmsg("Current Users in the World:");

	NxwSocketWrapper sw;
	sw.fillOnline();
	for( sw.rewind(); !sw.isEmpty(); sw++ ) {
		NXWCLIENT ps_i=sw.getClient();
		if(ps_i!=NULL)
		{
			P_CHAR pc_i=ps_i->currChar();
			if(ISVALIDPC(pc_i)) {
				j++;
				sprintf(s_szCmdTableTemp, "%i) %s [%x]", (j-1), pc_i->getCurrentNameC(), pc_i->getSerial32());
				pc->sysmsg(s_szCmdTableTemp);
			}
		}
	}
	sprintf(s_szCmdTableTemp,"Total Users Online: %d\n", j);
	pc->sysmsg(s_szCmdTableTemp);
	pc->sysmsg("End of userlist");
}

void command_gms( pClient client )
{

	NXWSOCKET s = ps->toInt();

	int j=0;

	P_CHAR pc=MAKE_CHAR_REF(currchar[s]);
	VALIDATEPC(pc);

	pc->sysmsg("Current GMs and Counselors in the world:");

	NxwSocketWrapper sw;
	sw.fillOnline();
	for( sw.rewind(); !sw.isEmpty(); sw++ ) {
		NXWCLIENT ps_i=sw.getClient();
		if(ps_i!=NULL)
		{
			P_CHAR pc_i=ps_i->currChar();
			if(ISVALIDPC(pc_i) && pc_i->IsGMorCounselor() ) {
				j++;
				pc->sysmsg("%s", pc_i->getCurrentNameC());
			}
		}
	}
	sprintf(s_szCmdTableTemp, "Total Staff Online: %d\n", j);
	pc->sysmsg(s_szCmdTableTemp);
	pc->sysmsg("End of stafflist");
}

void command_regspawnall( pClient client )
{

	NXWSOCKET s = ps->toInt();

	sysbroadcast(TRANSLATE("ALL Regions Spawning to MAX, this will cause some lag."));

	Spawns->doSpawnAll();

	sysmessage(s, "[DONE] All NPCs/items spawned in regions");
}

void command_sysm( pClient client )
{

	NXWSOCKET s = ps->toInt();

	if (now == 1)
	{
		sysmessage(s, "There are no other users connected.");
		return;
	}
	sysbroadcast(&tbuffer[Commands::cmd_offset + 5]);
}

void target_jail( pClient client, P_TARGET t )
{
	P_CHAR pc=pointers::findCharBySerial( t->getClicked() );
	VALIDATEPC( pc );

	prison::jail( ps->currChar(), pc, t->buffer[0] );
}

void command_jail( pClient client )
// (d) Jails the target with given secs.
{
	P_TARGET targ=clientInfo[ ps->toInt() ]->newTarget( new cCharTarget() );
	targ->code_callback=target_jail;

	if (tnum==2 )
	{
		targ->buffer[0]=strtonum(1);
		ps->sysmsg( "Select Character to jail. [Jailtime: %i secs]", targ->buffer[0] );
	}
	else
	{
		targ->buffer[0]=24*60*60;
		ps->sysmsg( "Select Character to jail. [Jailtime: 1 day]" );
	}

	targ->send( ps );
}

// handler for the movement effect
// Aldur
//
// (h) set your movement effect.
void command_setGmMoveEff( pClient client )
{

	NXWSOCKET s = ps->toInt();

	P_CHAR pc_cs=MAKE_CHAR_REF(currchar[s]);
	VALIDATEPC(pc_cs);
	 if (tnum==2)
		pc_cs->gmMoveEff = strtonum(1);
	return;
}


// (text) Changes the account password
void command_password( pClient client )
{

	NXWSOCKET s = ps->toInt();


	P_CHAR pc=MAKE_CHAR_REF(currchar[s]);
	VALIDATEPC(pc);

	if (tnum>1)
	{
		char pwd[200];
		int ret;
		sprintf(pwd, "%s", &tbuffer[Commands::cmd_offset+9]);
		if ((!isalpha(pwd[0]))&&(!isdigit(pwd[0]))) {
			sysmessage(s, "Passwords must start with a letter or a number\n");
			return;
		}

		ret = Accounts->ChangePassword(pc->account, pwd);

		if (ret==0)
			sprintf(pwd, "Password changed to %s", &tbuffer[Commands::cmd_offset+9]);
		else
			sprintf(pwd, "Some Error occured while changing password!");

		sysmessage(s, pwd);
	}
	else
		sysmessage(s, "You must digit 'PASSWORD <newpassword>");
		return;
}

void target_tele( pClient client, P_TARGET t )
{

	P_CHAR pc= ps->currChar();
	VALIDATEPC(pc);

	Location location = t->getLocation();
	Location charpos= pc->getPosition();

	if( line_of_sight( ps->toInt(), charpos.x, charpos.y, charpos.z, location.x, location.y, location.z, WALLS_CHIMNEYS+DOORS+FLOORS_FLAT_ROOFING ) ||
		pc->IsGM() )
	{
		pc->doGmEffect();
		location.z+=tileHeight( t->getModel() );
		pc->MoveTo( location );
		pc->teleport();
		pc->doGmEffect();
	}

}

void target_remove( pClient client, P_TARGET t )
{

	SERIAL serial = t->getClicked();

	if( isCharSerial( serial ) )	{
		P_CHAR pc=pointers::findCharBySerial( serial );
		VALIDATEPC(pc);

		if (pc->amxevents[EVENT_CHR_ONDISPEL]) {
			pc->amxevents[EVENT_CHR_ONDISPEL]->Call( pc->getSerial32(), INVALID, DISPELTYPE_GMREMOVE );
		}

        if (pc->account>INVALID && !pc->npc)
        {
            ps->sysmsg( TRANSLATE("You cant delete players") );
            return;
        }

        ps->sysmsg( TRANSLATE("Removing character.") );
		pc->Delete();
	}
	else {
		P_ITEM pi=pointers::findItemBySerial( serial );
		VALIDATEPI(pi);

		ps->sysmsg( TRANSLATE("Removing item.") );
        if( pi->amxevents[EVENT_IONDECAY] )
			pi->amxevents[EVENT_IONDECAY]->Call( pi->getSerial32(), DELTYPE_GMREMOVE );
        ps->sysmsg( TRANSLATE("Removing item.") );
        pi->Delete();
	}

}


void target_dye( pClient client, P_TARGET t )
{

	SERIAL serial = t->getClicked();

	UI16 color = t->buffer[0];

	P_CHAR curr = ps->currChar();
	VALIDATEPC(curr);

    if( isItemSerial(serial) ) {
		P_ITEM pi=pointers::findItemBySerial(serial);
		if (ISVALIDPI(pi))
		{
			if( color==UINVALID16 ) //open dye vat
			{
				SndDyevat( ps->toInt(), pi->getSerial32(), pi->getId() );
			}
			else {
				if (! ((color & 0x4000) || (color & 0x8000)) )
				{
					pi->setColor( color );
				}

				if (color == 0x4631)
				{
					pi->setColor( color );
				}

				pi->Refresh();
			}
		}
	}
	else {
		P_CHAR pc=pointers::findCharBySerial(serial);
		if(ISVALIDPC(pc))
		{
			if( color==UINVALID16 ) //open dye vat
			{
				SndDyevat( ps->toInt(), pc->getSerial32(), 0x2106 );
			}
			else {

				UI16 body = pc->getId();

				if(  color < 0x8000  && body >= BODY_MALE && body <= BODY_DEADFEMALE ) color |= 0x8000; // why 0x8000 ?! ^^;

				if ((color & 0x4000) && (body >= BODY_MALE && body<= 0x03E1)) color = 0xF000; // but assigning the only "transparent" value that works, namly semi-trasnparency.

				if (color != 0x8000)
				{
					pc->setColor(color);
					pc->setOldColor(color);
					pc->teleport( TELEFLAG_NONE );

				}
			}
		}
    }

}




void command_dye( pClient client )
// (h h/nothing) Dyes an item a specific color, or brings up a dyeing menu if no color is specified.
{

	P_TARGET targ = clientInfo[ ps->toInt() ]->newTarget( new cObjectTarget() );


	if (tnum==3)
	{
		if ( server_data.always_add_hex ) {
			targ->buffer[0]=DBYTE2WORD( hexnumber(1), hexnumber(2) );
		}
		else {
			targ->buffer[0]=DBYTE2WORD( strtonum(1), strtonum(2) );
		}
	}
	else
	{
		targ->buffer[0]=INVALID;
	}

	targ->code_callback = target_dye;
	targ->send( ps );
	ps->sysmsg( "Select item to dye..." );

}

void target_newz( pClient client, P_TARGET t )
{

    SERIAL serial = t->getClicked();

	if( isCharSerial( serial ) ) {
		P_CHAR pc=pointers::findCharBySerial( serial );
		VALIDATEPC(pc);

		Location location = pc->getPosition();
		location.z = location.dispz = t->buffer[0];
		pc->setPosition( location );
		pc->teleport();
	}
	else {
		P_ITEM pi=pointers::findItemBySerial( serial );
		VALIDATEPI(pi);

		Location location = pi->getPosition();
		location.z = t->buffer[0];
		pi->setPosition( location );
		pi->Refresh();
	}

};

void command_newz( pClient client )
{
	if (tnum==2) {
		P_TARGET targ = clientInfo[ ps->toInt() ]->newTarget( new cObjectTarget() );
		targ->buffer[0] = strtonum(1);
		targ->code_callback = target_newz;
		targ->send( ps );
		ps->sysmsg( "Select item to reposition..." );
	}

}


void target_setid( pClient client, P_TARGET t )
{

    SERIAL serial = t->getClicked();
	UI16 value = DBYTE2WORD( t->buffer[0], t->buffer[1] );

	if( isCharSerial( serial ) ) {
		P_CHAR pc=pointers::findCharBySerial( serial );
		VALIDATEPC(pc);

		pc->setId( value );
		pc->setOldId( value );
		pc->teleport();
	}
	else {
		P_ITEM pi=pointers::findItemBySerial( serial );
		VALIDATEPI(pi);

		pi->setId( value );
        pi->Refresh();
	}
}

void target_spy( pClient client, P_TARGET t )
{
	P_CHAR curr= ps->currChar();

	P_CHAR pc=pointers::findCharBySerial( t->getClicked() );
	VALIDATEPC(pc);

	if( pc->getSerial32()!=curr->getSerial32() ) {
		NXWCLIENT victim = pc->getClient();
		if( victim!=NULL ) {
			clientInfo[victim->toInt()]->spyTo=curr->getSerial32();
		}
	}
	else {
		for( int s=0; s<now; s++ )
			if( clientInfo[s]->spyTo==curr->getSerial32() )
				clientInfo[s]->spyTo=INVALID;
	}
}

void target_ban( pClient client, P_TARGET t )
{
	//todo
}

void target_emptypack( pClient client, P_TARGET t )
{
	SERIAL serial = t->getClicked();
	P_ITEM pack=NULL;

	if( isCharSerial( serial ) ) {
		P_CHAR pc=pointers::findCharBySerial( serial );
		VALIDATEPC(pc);

		P_ITEM backpack=pc->getBackpack();
		VALIDATEPI(backpack);

		pack=backpack;
	}
	else if( isItemSerial( serial ) ) {
		P_ITEM pi=pointers::findItemBySerial( serial );
		VALIDATEPI(pi);

		pack=pi;
	}

	if( ISVALIDPI(pack) ) {
		NxwItemWrapper si;
		si.fillItemsInContainer( pack, false );
		for( si.rewind(); !si.isEmpty(); si++ ) {
			P_ITEM item=si.getItem();
			if( ISVALIDPI(item) )
				item->Delete();
		}
	}
}

void target_possess( pClient client, P_TARGET t )
{
	P_CHAR pc=pointers::findCharBySerial( t->getClicked() );
	VALIDATEPC(pc);

	P_CHAR curr=ps->currChar();

	curr->possess( pc );
}


void target_hide( pClient client, P_TARGET t )
{
	P_CHAR pc = MAKE_CHAR_REF(t->getClicked());
	VALIDATEPC(pc);

	Location pcpos= pc->getPosition();

	if( pc->IsHidden() )
	{
		if( pc->getSerial32() == ps->currCharIdx() )
			ps->sysmsg( TRANSLATE("You are already hiding.") );
		else
			ps->sysmsg(  TRANSLATE("He is already hiding.") );
	}
	else {
		pc->setPermaHidden(true);
		staticeffect3( pcpos.x+1, pcpos.y+1, pcpos.z+10, 0x37, 0x09, 0x09, 0x19, 0);
		pc->playSFX(0x0208);
		tempfx::add(pc, pc, tempfx::GM_HIDING, 1, 0, 0);
	}

}

void target_unhide( pClient client, P_TARGET t )
{
	P_CHAR pc = MAKE_CHAR_REF( t->getClicked() );
	VALIDATEPC(pc);

	Location pcpos= pc->getPosition();

	if( !pc->IsHidden() )
	{
		if( pc->getSerial32()==ps->currCharIdx() )
			ps->sysmsg( TRANSLATE("You are not hiding."));
		else
			ps->sysmsg( TRANSLATE("He is not hiding."));
	}
	else {
		pc->setPermaHidden(false);
		staticeffect3( pcpos.x+1, pcpos.y+1, pcpos.z+10, 0x37, 0x09, 0x09, 0x19, 0);
		pc->playSFX(0x0208);
		tempfx::add(pc, pc, tempfx::GM_UNHIDING, 1, 0, 0);
	}
}

void target_fullstats( pClient client, P_TARGET t )
{
    P_CHAR pc = pointers::findCharBySerial( t->getClicked() );
    if (ISVALIDPC(pc))
    {
        pc->playSFX( 0x01F2);
        staticeffect( pc->getSerial32(), 0x37, 0x6A, 0x09, 0x06);
        pc->mn=pc->in;
        pc->hp=pc->getStrength();
        pc->stm=pc->dx;
        pc->updateStats(0);
        pc->updateStats(1);
        pc->updateStats(2);
    }
}

void target_heal( pClient client, P_TARGET t )
{
    P_CHAR pc = pointers::findCharBySerial( t->getClicked() );
    if (ISVALIDPC(pc))
    {
        pc->playSFX( 0x01F2);
        staticeffect( pc->getSerial32(), 0x37, 0x6A, 0x09, 0x06);
        pc->hp=pc->getStrength();
        pc->updateStats(0);
    }
}

void target_mana( pClient client, P_TARGET t )
{
    P_CHAR pc = pointers::findCharBySerial( t->getClicked() );
    if (ISVALIDPC(pc))
    {
        pc->playSFX( 0x01F2);
        staticeffect( pc->getSerial32(), 0x37, 0x6A, 0x09, 0x06);
        pc->mn=pc->in;
        pc->updateStats(1);
    }
}

void target_stamina( pClient client, P_TARGET t )
{
    P_CHAR pc = pointers::findCharBySerial( t->getClicked() );
    if( ISVALIDPC(pc) )
    {
        pc->playSFX( 0x01F2);
        staticeffect( pc->getSerial32(), 0x37, 0x6A, 0x09, 0x06);
        pc->stm=pc->dx;
        pc->updateStats(2);
    }
}

void target_tiledata( pClient client, P_TARGET t )
{

	Location loc = t->getLocation();
	UI16 tilenum=t->getModel();

    if( tilenum==0 )   // damn osi not me why the tilenum is only send for static tiles
    {   // manually calculating the ID's if it's a maptype

		map_st map1;
	    land_st land;

		data::seekMap( loc.x, loc.y, map1 );
        data::seekLand( map1.id, land );
        ConOut("type: map-tile\n");
        ConOut("tilenum: %i\n",map1.id);
        ConOut("Flags:%x\n", land.flags);
        ConOut("Name:%s\n", land.name);
	    ConOut("\n");
    }
    else
    {
		tile_st tile;
        data::seekTile(tilenum, tile);
        ConOut("type: static-tile\n");
        ConOut("tilenum: %i\n",tilenum);
        ConOut("Flags:%x\n", tile.flags);
        ConOut("Weight:%x\n", tile.weight);
        ConOut("Layer:%x\n", tile.quality);
        ConOut("Anim:%lx\n", tile.animid);
        ConOut("Unknown1:%lx\n", tile.unknown);
        ConOut("Unknown2:%x\n", tile.unknown2);
        ConOut("Unknown3:%x\n", tile.unknown3);
        ConOut("Height:%x\n", tile.height);
        ConOut("Name:%s\n", tile.name);
	    ConOut("\n");
    }

	ps->sysmsg( TRANSLATE("Item info has been dumped to the console.") );
}

void target_freeze( pClient client, P_TARGET t )
{
	P_CHAR pc=pointers::findCharBySerial( t->getClicked() );
	if( ISVALIDPC(pc) ) {
		pc->freeze();
		pc->teleport();
	};
}

void target_unfreeze( pClient client, P_TARGET t )
{
	P_CHAR pc=pointers::findCharBySerial( t->getClicked() );
	if( ISVALIDPC(pc) ) {
		pc->unfreeze();
		pc->teleport();
	}
}

void target_setamount( pClient client, P_TARGET t )
{

	P_ITEM pi= MAKE_ITEM_REF( t->getClicked() );
	VALIDATEPI(pi);

    pi->amount=t->buffer[0];
    pi->Refresh();

}

void target_npcaction( pClient client, P_TARGET t )
{
	P_CHAR pc = pointers::findCharBySerial( t->getClicked() );
	VALIDATEPC( pc );

	pc->playAction( t->buffer[0] );
}

void target_kick( pClient client, P_TARGET t )
{
	P_CHAR pc=pointers::findCharBySerial( t->getClicked() );
    if( ISVALIDPC(pc) )
    {
		ps->sysmsg( TRANSLATE("Kicking player"));
        pc->kick();
    }
}

void target_kill( pClient client, P_TARGET t )
{
    P_CHAR pc = pointers::findCharBySerial( t->getClicked() );
    if(ISVALIDPC(pc) )
    {
        if(!pc->dead)
        {
			pc->attackerserial=pc->getSerial32();
			pc->boltFX( true );
			pc->playSFX( 0x0029 );
			pc->Kill();
        }
        else
            ps->sysmsg( TRANSLATE("That player is already dead.") );
    }
}


void target_bolt( pClient client, P_TARGET t )
{
    P_CHAR pc=pointers::findCharBySerial( t->getClicked() );
	if( ISVALIDPC(pc) ) {
        pc->boltFX(true);
    }
}

void target_resurrect( pClient client, P_TARGET t )
{
    P_CHAR pc = pointers::findCharBySerial( t->getClicked() );
	if(ISVALIDPC(pc) && pc->dead)
		pc->resurrect();
}

void target_killhair( pClient client, P_TARGET t )
{
	P_CHAR pc=pointers::findCharBySerial( t->getClicked() );
	if( ISVALIDPC(pc) ) {
		P_ITEM pi = pc->getHairItem();
		if( ISVALIDPI(pi) )
			pi->Delete();
	}
}

void target_killbeard( pClient client, P_TARGET t )
{
	P_CHAR pc=pointers::findCharBySerial( t->getClicked() );
	if( ISVALIDPC(pc) ) {
		P_ITEM pi = pc->getBeardItem();
		if( ISVALIDPI(pi) )
			pi->Delete();
	}
}


void target_makegm( pClient client, P_TARGET t )
{

	P_CHAR pc=pointers::findCharBySerial( t->getClicked() );
	VALIDATEPC(pc);

    if (pc->dead)
		return;

	P_CHAR curr=ps->currChar();
	VALIDATEPC(curr);

    if (SrvParms->gm_log)
		WriteGMLog(curr, "%s [ serial %i ] as made %s [ serial %i ] a GM.\n", curr->getCurrentNameC(), curr->getSerial32(), pc->getCurrentNameC(), pc->getSerial32() );

    pc->unmountHorse();
    pc->gmrestrict = 0;
    pc->setId(BODY_GMSTAFF);
    pc->setOldId(BODY_GMSTAFF);
    pc->setColor(0x8021);
    pc->setOldColor(0x8021);
    pc->SetPriv(0xF7);
    pc->SetPriv2(0xD9);

    for( int j=0; j<TRUESKILLS; j++ )
    {
        pc->baseskill[j]=1000;
        pc->skill[j]=1000;
    }

    // All stats to 100
    pc->setStrength(100);
    pc->st2 = 100;
    pc->hp  = 100;
    pc->stm = 100;
    pc->in  = 100;
    pc->in2 = 100;
    pc->mn  = 100;
    pc->mn2 = 100;
    pc->dx  = 100;
    pc->dx2 = 100;

	pc->setCurrentName( "GM %s", pc->getCurrentNameC() );

	P_ITEM pack=pc->getBackpack();

	NxwItemWrapper si;
	si.fillItemWeared( pc, true, true, true );
	for( si.rewind(); !si.isEmpty(); si++ ) {
		P_ITEM pi=si.getItem();
		if( ISVALIDPI(pi) ) {
			if( pi->layer!=LAYER_BEARD && pi->layer!=LAYER_HAIR ) {
				if( ISVALIDPI(pack) )
					pack->AddItem( pi );
				else {
					pi->MoveTo( pc->getPosition() );
					pi->Refresh();
				}
			}
			else
				pi->Delete();
		}

	}

	pc->teleport();

}

void target_makecns( pClient client, P_TARGET t )
{

	P_CHAR pc=pointers::findCharBySerial( t->getClicked() );
	VALIDATEPC(pc);

	P_CHAR curr=ps->currChar();
	VALIDATEPC(curr);

    if (SrvParms->gm_log)
		WriteGMLog(curr, "%s [ serial %i ] as made %s [ serial %i ] a Counselor.\n", curr->getCurrentNameC(), curr->getSerial32(), pc->getCurrentNameC(), pc->getSerial32() );

    pc->setId(BODY_GMSTAFF);
    pc->setOldId(BODY_GMSTAFF);
    pc->setColor(0x8003);
    pc->setOldColor(0x8002);
    pc->SetPriv(0xB6);
    pc->SetPriv2(0x008D);
    pc->gmrestrict = 0;
	pc->setCurrentName("Counselor %s", pc->getCurrentNameC());

	P_ITEM pack=pc->getBackpack();

	NxwItemWrapper si;
	si.fillItemWeared( pc, true, true, true );
	for( si.rewind(); !si.isEmpty(); si++ ) {
		P_ITEM pi=si.getItem();
		if( ISVALIDPI(pi) ) {
			if( pi->layer!=LAYER_BEARD && pi->layer!=LAYER_HAIR ) {
				if( ISVALIDPI(pack) )
					pack->AddItem( pi );
				else {
					pi->MoveTo( pc->getPosition() );
					pi->Refresh();
				}
			}
			else
				pi->Delete();
		}

	}

	pc->teleport();
}

void target_xbank( pClient client, P_TARGET t )
{

	P_CHAR pc2 = pointers::findCharBySerial( t->getClicked() );
	VALIDATEPC(pc2);

	P_CHAR pc = ps->currChar();

	pc->openBankBox(pc2);
}

void target_xsbank( pClient client, P_TARGET t )
{

	P_CHAR pc2 = pointers::findCharBySerial( t->getClicked() );
	VALIDATEPC(pc2);

	P_CHAR pc = ps->currChar();

    pc->openSpecialBank(pc2);
}

void target_release( pClient client, P_TARGET t )
{

	P_CHAR pc = pointers::findCharBySerial( t->getClicked() );
	VALIDATEPC(pc);

	P_CHAR rel = ps->currChar();

	prison::release( rel, pc  );
}

void target_title( pClient client, P_TARGET t )
{

	P_CHAR pc = pointers::findCharBySerial( t->getClicked() );
	VALIDATEPC(pc);

	pc->title = t->buffer_str[0];
}
