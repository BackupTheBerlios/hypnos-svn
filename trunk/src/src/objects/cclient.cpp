/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Implementation of cClient class
*/

#include "cclient.h"

static cClients cClient::clients;               //!< this keeps all online clients

cClient::cClient(SI32 sd, struct sockaddr_in* addr)
{
	sock = new cSocket(sd, addr);
	pc = NULL;
	acc = NULL:
        clients.push_back(this);
}

cClient::~cClient()
{
	if ( pc )
		pc->setClient(NULL);
	if ( acc )
		acc->setClient(NULL);

	delete sock;
        clients.erase(find(clients.begin(), clients.end(), this));
}

/*!
\brief Compress packet buffer
\param out_buffer buffer to send
\param out_len size of buffer to send
*/
void cClient::compress(UI08& *out_buffer, UI32& out_len)
{
UI08 *new_buffer = new UI08[out_len];
UI32 new_len=0, tmp_len=out_len;

	if(out_len <= 0)
		return;

	for(i = 0; i < out_len; i++ ) {

		n_bits = bit_table[out_buffer[i]][0];
		value = bit_table[out_buffer[i]][1];

		while(n_bits--) {
//			new_buffer[new_len] = (new_buffer[new_len] << 1) | (UI08)((value >> n_bits) & 1);
			new_buffer[new_len] <<= 1;
			new_buffer[new_len] |= (UI08)((value >> n_bits) & 1);

			bit_4_byte++;
			if(bit_4_byte / 8) {
				new_len++;
				bit_4_byte %= 8;
			}
		}
	}

	n_bits = bit_table[256][0];
	value = bit_table[256][1];

	if(n_bits <= 0) {
		out_len = 0;
		return;
	}

	while(n_bits--) {
//		new_buffer[new_len] = (new_buffer[new_len] << 1) | (UI08)((value >> n_bits) & 1);
		new_buffer[new_len] <<= 1;
		new_buffer[new_len] |= (UI08)((value >> n_bits) & 1);

		bit_4_byte++;
		if(bit_4_byte / 8) {
			new_len++;
			bit_4_byte %= 8;
		}
	}

	if(bit_4_byte) {
		while(bit_4_byte < 8) {
			new_buffer[new_len] <<= 1;
			bit_4_byte++;
		}
		new_len++;
	}

	delete out_buffer;
	out_buffer = new_buffer;
	out_len = new_len;
}

/*!
\brief Show a container to player
\author Kheru - rewrote by Flameeyes
\param pCont the container
*/
void cClient::showContainer(pItem pCont)
{
	if ( ! pCont )
		return;

	NxwItemWrapper si;
	si.fillItemsInContainer( pCont, false, false );
	SI32 count=si.size();

	cPacketSendDrawContainer pk(pCont->getSerial(), pCont->getGump());
	sendPacket(&pk);

	cPacketSendContainerItem pk2;

	for( si.rewind(); !si.isEmpty(); si++ )
	{
		pItem pi=si.getItem();
		if ( ! pi )
			continue;

		//fix location of items if they mess up. (needs tweaked for container types)
		//! \todo The position of the items should be bound to client-specific protocol
		if (pi->getPosition().x > 150) pi->setPosition("x", 150);
		if (pi->getPosition().y > 140) pi->setPosition("y", 140);

		pk2.addItem(pi);
	}

	sendPacket(&pk2);
}

/*!
\brief Show an item into a container
\author Flameeyes
\param item item to show
*/
void cClient::addItemToContainer(pItem item)
{
	if ( ! item || pc->distFrom(pi) > VISRANGE )
		return;

	cPacketSendAddContainerItem pk( item, item->getContainer()->getSerial() );

	sendPacket(&pk);

	weights::NewCalc(pc);
}

/*!
\brief Play a random midi file adapt to the current status
\author Flameeyes
\todo Write it after get working XML-Support
*/
void cClient::playMidi()
{
/*
    cScpIterator* iter = NULL;
    char script1[1024];
    char script2[1024];

	char sect[512];

	if (pc->war)
		strcpy(sect, "MIDILIST COMBAT");
	else
		sprintf(sect, "MIDILIST %i", region[pc->region].midilist);

	iter = Scripts::Regions->getNewIterator(sect);
	if (iter==NULL) return;

	char midiarray[50];
	int i=0;
	int loopexit=0;
	do
	{
		iter->parseLine(script1, script2);
		if ((script1[0]!='}')&&(script1[0]!='{'))
		{
			if (!(strcmp("MIDI",script1)))
			{
				midiarray[i]=str2num(script2);
				i++;
			}
		}
	}
	while ((script1[0]!='}') && (++loopexit < MAXLOOPS) );

	safedelete(iter);

	if (i!=0)
	{
		i=rand()%(i);
		playmidi(s, 0, midiarray[i]);
	}
*/
}

/*!
\brief Play a sound effect
\author Flameeyes
\todo Fix the set support after get working new sets
*/
void cClient::playSFX(UI16 sound, bool onlyMe)
{
	cPacketSendSoundFX pk(sound, pc->getPosition());

	if(onlyMe) {
		client->send(&pk);
		return;
	}

/*
	NxwSocketWrapper sw;
	sw.fillOnline( pc, false );

	for( sw.rewind(); !sw.isEmpty(); sw++ ) {
		NXWCLIENT ps=sw.getClient();
		if(ps!=NULL)
			ps->send(&pk);
	}
*/
}

/*!
\brief Set the light level
\param level base light level
*/
void cClient::light(UI08 level)
{
	if ( ! pc ) return;

	if (worldfixedlevel != 0xFF)
		level = worldfixedlevel;
	else if (pc->getFixedLight() != 0xFF)
		level = pc->getFixedLight();
	else if (pc->inDungeon())
		level = dungeonlightlevel;

	cPacketSendOverallLight pk(level);

	sendPacket(&pk);
}

void cClient::showBankBox(pPC dest)
{
	if ( ! dest || (dest != pc && acc->getPrivLevel() < privSeer ) )
		return;

	pItem bank = dest->getBankBox();
	if ( ! bank )
		return;

	showClient(bank);
}

/*!
\brief region specific bankbox
\author Endymion
\param pc character owner of bank

If activated, you can only put golds into normal banks
and there are special banks (for now we still use normal bankers,
but u have to say the SPECIALBANKTRIGGER word to open it)
where u can put all the items: one notice: the special bank
is caracteristic of regions....so in Britain you don't find
the items you leaved in Minoc!
All this for increasing pk-work and commerce! :)
(and surely the Mercenary work, so now have to pay strong
warriors to escort u during your travels!)
*/
void cClient::showSpecialBankBox(pPC dest)
{
	if ( ! dest || (dest != pc && acc->getPrivLevel() < privSeer ) )
		return;

	pItem bank = dest->getSpecialBankBox();
	if ( ! bank )
		return;

	showClient(bank);
}

void cClient::statusWindow(pChar target, bool extended) //, bool canrename)  will be calculated from client data
{

	VALIDATEPC(target);
        cChar pc = currChar();  // pc who will get the status window

        bool canrename;

	if ((pc->IsGM() || (target->getOwnerSerial32()==pc->getSerial32())) && (target!=pc)) canrename = true;
	else canrename = false;

	if ((pc->getBody()->getId() == BODY_DEADMALE) || (pc->getBody()->getId() == BODY_DEADFEMALE)) canrename = false;
       	UI08 ext;
        if (extended)
        {
                ext = 0x01;
	        if ( flags & flagClientIsUO3D )
		        ext = 0x03;
        	if ( flags & flagClientIsAoS )
	        	ext = 0x04;
        }
        else ext = 0x0;

	cPacketSendStatus pk(sorg, ext, canrename);
	sendPacket(&pk);
}

/*------------------------------------------------------------------------------
                        DRAG & DROP METHODS
------------------------------------------------------------------------------*/

/*!
\brief Get an item
\author Unknow, revamped by Endymion, then Chronodt (3/2/2004)
\param  pi item to get
\param amount amount of *pi to get
*/
void cClient::get_item( pItem pi, UI16 amount ) // Client grabs an item
{
	pChar pc_currchar = currChar();
	VALIDATEPC( pc_currchar );

	//Luxor: not-movable items
	/*if (pi->magic == 2 || (isCharSerial(pi->getContSerial()) && pi->getContSerial() != pc_currchar->getSerial32()) ) {
		if (isCharSerial(pi->getContSerial())) {
			P_CHAR pc_i = pointers::findCharBySerial(pi->getContSerial());
			if (ISVALIDPC(pc_i))
				pc_i->sysmsg("Warning, backpack bug located!");
		}
		if (client->isDragging()) {
        		client->resetDragging();
			UpdateStatusWindow(s,pi);
        	}
		pi->setContainer( pi->getOldContainer() );
		pi->setPosition( pi->getOldPosition() );
		pi->layer = pi->oldlayer;
		pi->Refresh();
		return;
	}*/

	pc_currchar->disturbMed(); // Meditation

	tile_st item;
 	data::seekTile( pi->getId(), item );

	// Check if item is equiped
 	if( pi->getContainer == pc_currchar && pi->layer == item.quality )
 	{
 		if( pc_currchar->UnEquip( pi, 1 ) == 1 )	// bypass called
 		{
 			if( isDragging() )
                        //! \todo the sendpacket stuff here
 			{
 				UI08 cmd[1]= {0x29};
 				resetDragging();
 				Xsend(s, cmd, 1);
//AoS/				Network->FlushBuffer(s);
 			}
 			return;
 		}
 	}


	pChar owner=NULL;
	pItem container=NULL;
	if ( !pi->isInWorld() ) { // Find character owning item

		if ( isCharSerial(pi->getContainer()->getSerial()) )  // or: isChar(pi->getContainer()) -.^?
		{
			owner = (pChar) pi->getContainer();
		}
		else  // its an item
		{
			//Endymion Bugfix:
			//before check the container.. but if this cont is a subcont?
			//so get the outmostcont and check it else:
			//can loot without lose karma in subcont
			//can steal in trade ecc
			//not very good :P
			container = pi->getOutMostCont();
			if( isCharSerial( container->getContainer()->getSerial() ) ) //see above ...
				owner = (pChar) container->getContainer();
		}

		if ( ISVALIDPC( owner ) && (owner != pc_currchar) )
		{
			if ( !pc_currchar->IsGM() && owner->getOwnerSerial32() != pc_currchar->getSerial() )
			{// Own serial stuff by Zippy -^ Pack aniamls and vendors.
                        //! \todo the sendpacket stuff here
				UI08 bounce[2]= { 0x27, 0x00 };
				Xsend(s, bounce, 2);
//AoS/				Network->FlushBuffer(s);
				if (isDragging())
				{
					resetDragging();
					pi->setOldContainer(pi->getContainer());
					item_bounce3(pi);
				}
				return;
			}
		}
	}

	if ( ISVALIDPI( container ) )
	{

		if ( container->layer == 0 && container->getId() == 0x1E5E)
		{
			// Trade window???
			SERIAL serial = calcserial( pi->moreb1, pi->moreb2, pi->moreb3, pi->moreb4);
			if ( serial == INVALID )
				return;

			pItem piz = pointers::findItemBySerial(serial );
			if ( ISVALIDPI( piz ) )
				if ( piz->morez || container->morez )
				{
                                //! \todo the sendpacket stuff here
					piz->morez = 0;
					container->morez = 0;
					sendtradestatus( piz, container );
				}


			//<Luxor>
			if (pi->amxevents[EVENT_ITAKEFROMCONTAINER]!=NULL)
			{
				g_bByPass = false;
				pi->amxevents[EVENT_ITAKEFROMCONTAINER]->Call( pi->getSerial32(), pi->getContSerial(), pc_currchar->getSerial32() );
				if (g_bByPass)
				{
                                //! \todo the sendpacket stuff here
					Sndbounce5(s);
					if (isDragging())
					{
						resetDragging();
						UpdateStatusWindow(s,pi);
					}
					pi->setContainer( pi->getOldContainer() );
					pi->setPosition( pi->getOldPosition() );
					pi->layer = pi->oldlayer;
					pi->Refresh();
					return;
                		}
			}
			//</Luxor>

			/*
			//<Luxor>
			g_bByPass = false;
			pi->runAmxEvent( EVENT_ITAKEFROMCONTAINER, pi->getSerial32(), pi->getContSerial(), s );
			if (g_bByPass)
			{
				Sndbounce5(s);
				if (client->isDragging())
				{
					client->resetDragging();
					UpdateStatusWindow(s,pi);
				}
				pi->setContainer( pi->getOldContainer() );
				pi->setPosition( pi->getOldPosition() );
				pi->layer = pi->oldlayer;
				pi->Refresh();
				return;
			}
			//</Luxor>
			*/

			if ( container->corpse )
			{
				if ( container->getOwnerSerial32() != pc_currchar->getSerial())
				{ //Looter :P

					pc_currchar->unHide();
					bool bCanLoot = false;
					if( pc_currchar->party!=INVALID ) {
						pChar dead = pointers::findCharBySerial( container->getOwnerSerial32() ) ;
						if( ISVALIDPC( dead ) && dead->party==pc_currchar->party ) {
							P_PARTY party = Partys.getParty( pc_currchar->party );
							if( party!=NULL ) {
								P_PARTY_MEMBER member = party->getMember( pc_currchar->getSerial32() );
								if( member!=NULL )
									bCanLoot = member->canLoot;
							}
						}
					}
					if ( !bCanLoot && container->more2==1 )
					{
						pc_currchar->IncreaseKarma(-5);
						pc_currchar->setCrimGrey(ServerScp::g_nLootingWillCriminal);
						pc_currchar->sysmsg( TRANSLATE("You are loosing karma!"));
					}
				}
			} // corpse stuff

			container->SetMultiSerial(INVALID);

			//at end reset decay of container
			container->setDecayTime();

		} // end cont valid
	}

	if ( !pi->corpse )
	{
                //! \todo the sendpacket stuff here
		UpdateStatusWindow(s, pi);

		tile_st tile;
		data::seekTile( pi->getId(), tile);

		if (!pc_currchar->IsGM() && (( pi->magic == 2 || ((tile.weight == 255) && ( pi->magic != 1))) && !pc_currchar->canAllMove() )  ||
			(( pi->magic == 3|| pi->magic == 4) && !pc_currchar->isOwnerOf( pi )))
		{
                        //! \todo the sendpacket stuff here
			UI08 bounce[2]={ 0x27, 0x00 };
			Xsend(s, bounce, 2);
//AoS/			Network->FlushBuffer(s);
			if (isDragging()) // only restore item if it got draggged before !!!
			{
				resetDragging();
				item_bounce4(s, pi );
			}
		} // end of can't get
		else
		{
			// AntiChrist bugfix for the bad bouncing bug ( disappearing items when bouncing )
			setDragging();
			pi->setOldPosition( pi->getPosition() ); // first let's save the position

			pi->oldlayer = pi->layer;	// then the layer
			pi->layer = 0;

			if (!pi->isInWorld())
				pc_currchar->playSFX(0x0057);

			if (pi->getAmount()>1)
			{
				if (amount > pi->getAmount())
					amount = pi->getAmount();
				else if (amount < pi->getAmount())
				{ //get not all but a part of item ( piled? ), but anyway make a new one item

					pItem pin = new cItem(cItem::nextSerial());
					(*pin)=(*pi);

					pin->setAmount(pi->getAmount() - amount);

					pin->setContainer(pi->getContainer());	//Luxor
					pin->setPosition( pi->getPosition() );

					/*if( !pin->isInWorld() && isItemSerial( pin->getContainer()->getSerial() ) )
						pin->SetRandPosInCont( (pItem)pin->getContainer() );*/

                                        //! \todo the sendpacket stuff here
					statwindow(pc_currchar,pc_currchar);
					pin->Refresh();//AntiChrist
				}

				if ( pi->getId() == ITEMID_GOLD)
				{
					pItem pack= pc_currchar->getBackpack();
                                        //! \todo the sendpacket stuff here
					if ( pack )
						if ( pi->getContainer() == pack )
							statwindow(pc_currchar, pc_currchar);
				}

				pi->setAmount(amount);

			} // end if corpse
#ifdef SPAR_I_LOCATION_MAP
			pointers::delFromLocationMap( pi );
#else
			mapRegions->remove( pi );
#endif
			pi->setPosition( 0, 0, 0 );
			pi->setContainer(0);
		}
	}

	int amt = 0, wgt;
	wgt = (int)weights::LockeddownWeight( pi, &amt);
	pc_currchar->weight += wgt;
        //! \todo the sendpacket stuff here
	statwindow(pc_currchar, pc_currchar);
}


/*!
\brief Drop an item
\author Unknown, moved here by Chronodt (3/2/2004)
\param pi item to be dropped (already in dragging mode)
\param loc position to drop item at (eventually in cont)
\param cont container into which *pi has to be dropped (-1 = world)
*/

void cClient::drop_item(pItem pi, Location &loc, pItem cont) // Item is dropped
{

    //#define debug_dragg

	if (clientDimension==3)
	{
	  // UO:3D clients send SOMETIMES two dragg packets for a single dragg action.
	  // sometimes we HAVE to swallow it, sometimes it has to be interpreted
	  // if UO:3D specific item loss problems are reported, this is probably the code to blame :)
	  // LB

	  #ifdef debug_dragg
	    if (ISVALIDPI(pi)) { sprintf(temp, "%04x %02x %02x %01x %04x i-name: %s EVILDRAG-old: %i\n",pi->getSerial32(), loc->x, loc->y, loc->z, cont->getSerial32(), pi->name, evilDrag); ConOut(temp); }
		else { sprintf(temp, "blocked: %04x %02x %02x %01x %04x i-name: invalid item EVILDRAG-old: %i\n",pi->getSerial32(), loc->x, loc->y, loc->z, cont->getSerial32(), evilDrag); ConOut(temp); }
	  #endif

	  if  ( (loc->x==-1) && (loc->y==-1) && (loc->z==0)  && (evilDrag) )
	  {
		  evilDrag=false;
          #ifdef debug_dragg
		    ConOut("Swallow only\n");
          #endif
		  return;
	  }	 // swallow! note: previous evildrag !

	  else if ( (loc->x==-1) && (loc->y==-1) && (loc->z==0)  && (!evilDrag) )
	  {
          #ifdef debug_dragg
		    ConOut("Bounce & Swallow\n");
          #endif
                  //! \todo the sendpacket stuff here
		  item_bounce6(ps, pi);
		  return;
	  }
	  else if ( ( (loc->x!=-1) && (loc->y!=-1) && ( cont->getSerial32()!=-1)) || ( (pi->getSerial32()>=0x40000000) && (cont->getSerial32()>=0x40000000) ) )
		  evilDrag=true; // calc new evildrag value
	  else evilDrag=false;
	}

	#ifdef debug_dragg
	  else
	  {
	     if (ISVALIDPI(pi)) { sprintf(temp, "blocked: %04x %02x %02x %01x %04x i-name: %s EVILDRAG-old: %i\n",pi->getSerial32(), loc->x, loc->y, loc->z, cont->getSerial32(), pi->name, evilDrag); ConOut(temp); }
	  }
	#endif


	if ( isItemSerial(cont->getSerial32()) && (cont->getSerial32() != INVALID)  ) // Invalid target => invalid container => put inWorld !!!
		pack_item(pi, loc, cont);
	else
		dump_item(pi, loc, cont);
}

/*!
\brief put item into a container
\author Unknown, moved here by Chronodt (3/2/2004)
\param pi item to be dropped (already in dragging mode)
\param loc position to drop item at (eventually in cont)
\param cont container into which *pi has to be dropped (-1 = world)
*/


void cClient::pack_item(pItem pi, Location &loc, pItem cont) // Item is put into container
{

	char temp[TEMP_STR_SIZE]; //xan -> this overrides the global temp var
	char temp2[TEMP_STR_SIZE]; //xan -> this overrides the global temp var
	int serial/*, serhash*/;
	tile_st tile;
//	bool abort=false;

	pChar pc= currChar();
	VALIDATEPC(pc);

	Location charpos = pc->getPosition();

	pItem pack;

	if (pi->getId() >= 0x4000)
	{
//		abort=true; // LB crashfix that prevents moving multi objcts in BP's
		sysmsg(TRANSLATE("Hey, putting houses in your pack crashes your back and client!"));
	}

	//ndEndy recurse only a time
	pItem contOutMost = cont->getOutMostCont();
//	pChar contOwner = ( !contOutMost->isInWorld() )? pointers::findCharBySerial( contOutMost->getContainer()->getSerial() ) : NULL;
	pChar contOwner = (pChar) contOutMost->getContainer();

	if( ISVALIDPC(contOwner) ) {
		//if ((contOwner->npcaitype==NPCAI_PLAYERVENDOR) && (contOwner->npc) && (contOwner->getOwnerSerial32()!=pc->getSerial32()) )
		if ( contOwner->getSerial32() != pc->getSerial32() && contOwner->getOwnerSerial32() != pc->getSerial32() && !pc->IsGM() ) { // Luxor
			sysmsg(TRANSLATE("This aint your backpack!"));
                        //! \todo the sendpacket stuff here
			Sndbounce5(s);
			if (isDragging()) {
				resetDragging();
				item_bounce3(pi);
				if (cont->getId() >= 0x4000)
					senditem(s, cont);
			}
			return;
		}
	}

	if (cont->amxevents[EVENT_IONPUTITEM]!=NULL) {
		g_bByPass = false;
		cont->amxevents[EVENT_IONPUTITEM]->Call( cont->getSerial32(), pi->getSerial32(), pc->getSerial32() );
		if (g_bByPass)
		{
                //! \todo the sendpacket stuff here
			item_bounce6(ps,pi);
			return;
		}
	}
	if (cont->layer==0 && cont->getId() == 0x1E5E &&
		cont->getContainer() == pc)
	{
		// Trade window???
		serial=calcserial(cont->moreb1, cont->moreb2, cont->moreb3, cont->moreb4);
		if(serial==-1) return;

		pItem pi_z = pointers::findItemBySerial(serial);

		if (ISVALIDPI(pi_z))
			if ((pi_z->morez || cont->morez))
			{
				pi_z->morez=0;
				cont->morez=0;
                                //! \todo the sendpacket stuff here
				sendtradestatus( pi_z, cont );
			}
	}

	if(SrvParms->usespecialbank)//only if special bank is activated
	{
		if(cont->morey==MOREY_GOLDONLYBANK && cont->morex==MOREX_BANK && cont->type==ITYPE_CONTAINER)
		{
			if ( pi->getId() == ITEMID_GOLD )
			{//if they're gold ok
				pc->playSFX( goldsfx(2) );
			} else
			{//if they're not gold..bounce on ground
				sysmsg(TRANSLATE("You can only put golds in this bank box!"));

				pi->setContainer(0);
				pi->MoveTo( charpos );
				pi->Refresh();
				pc->playSFX( itemsfx(pi->getId()) );
				return;
			}
		}
	}

	// Xanathars's Bank Limit Code
	if (ServerScp::g_nBankLimit != 0) {

		if( ISVALIDPI( contOutMost ) && contOutMost->morex==MOREX_BANK ) {

			int n = contOutMost->CountItems( INVALID, INVALID, false);
			n -= contOutMost->CountItems( ITEMID_GOLD, INVALID, false);
			if( pi->type == ITYPE_CONTAINER )
				n += pi->CountItems( INVALID, INVALID, false);
			else
				++n;
			if( n > ServerScp::g_nBankLimit ) {
				sysmsg(TRANSLATE("You exceeded the number of maximimum items in bank of %d"), ServerScp::g_nBankLimit);
                                //! \todo the sendpacket stuff here
				item_bounce6(ps,pi);
				return;
			}

		}
	}


	//ndEndy this not needed because when is dragging cont serial is INVALID
	//testing UOP Blocking Tauriel 1-12-99
	if (!pi->isInWorld())
	{
        //! \todo the sendpacket stuff here
		item_bounce6(ps,pi);
		return;
	}

	data::seekTile(pi->getId(), tile);
	if ((((pi->magic==2)||((tile.weight==255)&&(pi->magic!=1)))&&!pc->canAllMove) ||
				( (pi->magic==3|| pi->magic==4) && !(pi->getOwnerSerial32()==pc->getSerial32())))
	{
        //! \todo the sendpacket stuff here
		Sndbounce5(s);
		if (isDragging())
		{
			resetDragging();
			item_bounce3(pi);
			if (cont->getId() >= 0x4000)
				senditem(s, cont);
		}
		return;
	}
	// - Trash container
	if( cont->type==ITYPE_TRASH)
	{
		pi->Delete();
		sysmsg(TRANSLATE("As you let go of the item it disappears."));
		return;
	}
	// - Spell Book
	if (cont->type==ITYPE_SPELLBOOK)
	{
		if (!pi->IsSpellScroll72())
		{
			sysmsg(TRANSLATE("You can only place spell scrolls in a spellbook!"));
                        //! \todo the sendpacket stuff here
			Sndbounce5(s);
			if (isDragging())
			{
				resetDragging();
				item_bounce3(pi);
			}
			if (cont->getId() >= 0x4000)
				senditem(s, cont);
			return;
		}
		pack= pc->getBackpack();
		if(ISVALIDPI(pack))
		{
			if ((cont->getContainer() != pc ) && (cont->getContainer()!=pack) && (!pc->CanSnoop()))
			{
				sysmsg(TRANSLATE("You cannot place spells in other peoples spellbooks."));
                                //! \todo the sendpacket stuff here
				item_bounce6(ps,pi);
				return;
			}

			if( strncmp(pi->getCurrentNameC(), "#", 1) )
				pi->getName(temp2);
			else
				strcpy(temp2,pi->getCurrentNameC());

			NxwItemWrapper sii;
			sii.fillItemsInContainer( cont, false );
			for( sii.rewind(); !sii.isEmpty(); sii++ ) {

				P_ITEM pi_ci=sii.getItem();

					if (ISVALIDPI(pi_ci))
					{
						if( strncmp(pi_ci->getCurrentNameC(), "#", 1) )

							pi_ci->getName(temp);
						else
							strcpy(temp,pi_ci->getCurrentNameC());

						if(!(strcmp(temp,temp2)) || !(strcmp(temp,"All-Spell Scroll")))
						{
							sysmsg(TRANSLATE("You already have that spell!"));
                                                        //! \todo the sendpacket stuff here
							item_bounce6(ps,pi);
							return;
						}
					}
				// Juliunus, to prevent ppl from wasting scrolls.
				if (pi->amount > 1)
				{
					sysmsg(TRANSLATE("You can't put more than one scroll at a time in your book."));
                                        //! \todo the sendpacket stuff here
					item_bounce6(ps,pi);
					return;
				}
			}
		}
		cont->AddItem( pi );
		sendSpellBook(cont);
		return;
	}

	if (cont->type == ITYPE_CONTAINER) {

		if ( ISVALIDPC(contOwner) )
		{
			if ( (contOwner->npcaitype==NPCAI_PLAYERVENDOR) && (contOwner->npc) && (contOwner->getOwnerSerial32()==pc->getSerial32()) )
			{
				pc->fx1= DEREF_P_ITEM(pi);
				pc->fx2=17;
				pc->sysmsg(TRANSLATE("Set a price for this item."));
			}
		}

		short xx=loc->x;
		short yy=loc->y;

		cont->AddItem(pi,xx,yy);

		pc->playSFX( itemsfx(pi->getId()) );
                //! \todo the sendpacket stuff here
		statwindow(pc,pc);
	}
	// end of player run vendors

	else
		// - Unlocked item spawner or unlockable item spawner
		if (cont->type==ITYPE_UNLOCKED_CONTAINER || cont->type==ITYPE_NODECAY_ITEM_SPAWNER || cont->type==ITYPE_DECAYING_ITEM_SPAWNER)
		{
			cont->AddItem(pi, loc->x, loc->y); //Luxor
			pc->playSFX( itemsfx(pi->getId()) );

		}
		else  // - Pileable
			if (cont->pileable && pi->pileable)
			{
				if ( !cont->PileItem( pi ) )
				{
                                        //! \todo the sendpacket stuff here
					item_bounce6(ps,pi);
					return;
				}
			}
			else
			{
				if( !pi->getOldContainer() ) //current cont serial is invalid because is dragging
				{
					NxwSocketWrapper sw;
					sw.fillOnline( pi->getPosition() );
                                        //! \todo the sendpacket stuff here
					for( sw.rewind(); !sw.isEmpty(); sw++ )
						SendDeleteObjectPkt(sw.getSocket(), pi->getSerial32() );
					mapRegions->remove(pi);
				}

				pi->setPosition( loc );
				pi->setContainer( cont->getContainer() );

				pi->Refresh();
			}


}


/*!
\brief drop dragged item on the ground or a character
\author Unknown, moved here by Chronodt (4/2/2004)
\param pi item to be dropped (already in dragging mode)
\param loc position to drop item at (eventually in cont)
\param cont container into which *pi has to be dropped (-1 = world)
*/

void cClient::dump_item(pItem pi, Location &loc, pItem cont) // Item is dropped on ground or a character
{

	tile_st tile;

	pChar pc=currChar();
	VALIDATEPC(pc);

	if ( isCharSerial(pi->getContainer()->getSerial()) && pi->getContainer() != pc ) {
		pChar pc_i = (pChar) pi->getContainer();
		if (ISVALIDPC(pc_i))
			pc_i->sysmsg("Warning, backpack disappearing bug located!");

		if (isDragging()) {
                        resetDragging();
                        //! \todo the sendpacket stuff here
                        UpdateStatusWindow(s,pi);
                }
		pi->setContainer( pi->getOldContainer() );
                pi->setPosition( pi->getOldPosition() );
                pi->layer = pi->oldlayer;
                pi->Refresh();
	}

	if (pi->magic == 2) { //Luxor -- not movable objects
		if (isDragging()) {
                        resetDragging();
                        //! \todo the sendpacket stuff here
                        UpdateStatusWindow(s,pi);
                }
		pi->setContainer( pi->getOldContainer() );
		pi->MoveTo( pi->getOldPosition() );
		pi->layer = pi->oldlayer;
		pi->Refresh();
		return;
	}




        if(pi!=NULL)
	{
		weights::NewCalc(pc);
                //! \todo the sendpacket stuff here
		statwindow(pc,pc);
	}


	//Ripper...so order/chaos shields disappear when on ground.
	if( pi->getId()==0x1BC3 || pi->getId()==0x1BC4 )
	{
		pc->playSFX( 0x01FE);
		staticeffect(DEREF_P_CHAR(pc), 0x37, 0x2A, 0x09, 0x06);
		pi->Delete();
		return;
	}


	//test UOP blocking Tauriel 1-12-99
	if (!pi->isInWorld())
	{
        //! \todo the sendpacket stuff here
		item_bounce6(ps,pi);
		return;
	}




	data::seekTile(pi->getId(), tile);
	if (!pc->IsGM() && ((pi->magic==2 || (tile.weight==255 && pi->magic!=1))&&!pc->canAllMove()) ||
		( (pi->magic==3 || pi->magic==4) && !(pi->getOwnerSerial32()==pc->getSerial32())))
	{
        //! \todo the sendpacket stuff here
		item_bounce6(ps,pi);
		return;
	}

	if (loc->x != 0xffff)  // WAS buffer[s][5] != unsigned char '0xff'
        {
               if (pi->amxevents[EVENT_IDROPINLAND]!=NULL) {
	       	        g_bByPass = false;
        	        pi->amxevents[EVENT_IDROPINLAND]->Call( pi->getSerial32(), pc->getSerial32() );
			        if (g_bByPass) {
				        pi->Refresh();
				        return;
			        }
		        }
                NxwSocketWrapper sw;
	        sw.fillOnline( pi );
        	for( sw.rewind(); !sw.isEmpty(); sw++ )
        	{
                        //! \todo the sendpacket stuff here
        		SendDeleteObjectPkt( sw.getSocket(), pi->getSerial32() );
        	}

        	pi->MoveTo(Loc);
                pi->setContainer(0);

                pItem p_boat = Boats->GetBoat(pi->getPosition());

        	if(ISVALIDPI(p_boat))
        	{
        		pi->SetMultiSerial(p_boat->getSerial32());
        	}


        	pi->Refresh();
	}
	else
	{
		if ( !droppedOnChar(pi, loc, cont) ) {
			//<Luxor>: Line of sight check
			//This part avoids the circle of transparency walls bug

			//-----
			if ( !lineOfSight( pc->getPosition(), loc ) ) {
		                ps->sysmsg(TRANSLATE("You cannot place an item there!"));
                                //! \todo the sendpacket stuff here
        	        	Sndbounce5(s);
	                	if (isDragging()) {
	                        	resetDragging();
                        		UpdateStatusWindow(s,pi);
                		}
                		pi->setContainer( pi->getOldContainer() );
                		pi->setPosition( pi->getOldPosition() );
                		pi->layer = pi->oldlayer;
                		pi->Refresh();
                		return;
        		}
        		//</Luxor>

	        	//<Luxor> Items count check
	        	if (!pc->IsGM()) {
				NxwItemWrapper si;
				si.fillItemsAtXY( loc->x, loc->y );
				if (si.size() >= 2) { //Only 2 items permitted
					ps->sysmsg(TRANSLATE("There is not enough space there!"));
                                        //! \todo the sendpacket stuff here
					Sndbounce5(s);
					if (isDragging()) {
						resetDragging();
						UpdateStatusWindow(s,pi);
					}
					if (ISVALIDPI(pc->getBackpack())) {
						pc->getBackpack()->AddItem(pi);
					} else {
						pi->setContainer( pi->getOldContainer() );
						pi->setPosition( pi->getOldPosition() );
					}
					pi->layer = pi->oldlayer;
					pi->Refresh();
					return;
				}
			}
        		//</Luxor>
		}

		weights::NewCalc(pc);  // Ison 2-20-99
                //! \todo the sendpacket stuff here
		statwindow(pc,pc);
		pc->playSFX( itemsfx(pi->getId()) );

		//Boats !
		if (pc->getMultiSerial32() > 0) //How can they put an item in a multi if they aren't in one themselves Cut lag by not checking everytime something is put down
		{
			pItem multi = pointers::findItemBySerial( pc->getMultiSerial32() );
			if (ISVALIDPI(multi))
			{
				multi=findmulti( pi->getPosition() );
				if (ISVALIDPI(multi))
					//setserial(DEREF_P_ITEM(pi),DEREF_P_ITEM(multi),7);
					pi->SetMultiSerial(multi->getSerial32());
			}
		}
		//End Boats
	}
}

/*!
\brief verifies if item has been dropped on a char and if so executes necessary code
\author Unknown, moved here by Chronodt (4/2/2004)
\param pi item to be dropped (already in dragging mode)
\param loc position to drop item at (eventually in cont)
\param cont container into which *pi has to be dropped (-1 = world)
\return bool
*/

bool cClient::droppedOnChar(pItem pi, Location &loc, pItem cont)
{
	VALIDATEPIR(pi, false);


	pChar pTC = pointers::findCharBySerial(cont->getSerial32());	// the targeted character
	VALIDATEPCR(pTC, false);
	pChar pc_currchar = currChar();
	VALIDATEPCR(pc_currchar, false);
	Location charpos = pc_currchar->getPosition();

	if (!pTC) return true;

	if (pc_currchar->getSerial32() != pTC->getSerial32() /*DEREF_P_CHAR(pTC)!=cc*/)
	{
		if (pTC->npc)
		{
			if(!pTC->HasHumanBody())
			{
				droppedOnPet( ps, pp, pi);
			}
			else	// Item dropped on a Human character
			{
				// Item dropped on a Guard (possible bounty quest)
				if( ( pTC->npc == 1 ) && ( pTC->npcaitype == NPCAI_TELEPORTGUARD ) )
				{
					droppedOnGuard(pi, loc, cont);
				}
				if ( pTC->npcaitype == NPCAI_BEGGAR )
				{
					droppedOnBeggar(pi, loc, cont);
				}

				//This crazy training stuff done by Anthracks (fred1117@tiac.net)
				if(pc_currchar->trainer != pTC->getSerial32())

				{
					pTC->talk(this, TRANSLATE("Thank thee kindly, but I have done nothing to warrant a gift."),0);
                                        //! \todo the sendpacket stuff here
					Sndbounce5(s);
					if (isDragging())
					{
						resetDragging();
						item_bounce5(s,pi);
					}
					return true;
				}
				else // The player is training from this NPC
				{
					droppedOnTrainer( pi, loc, cont);
				}
			}//if human or not
		}
		else // dropped on another player
		{
			// By Polygon: Avoid starting the trade if GM drops item on logged on char (crash fix)
			if ((pc_currchar->IsGM()) && !pTC->IsOnline())
			{
				// Drop the item in the players pack instead
				// Get the pack
				pItem pack = pTC->getBackpack();
				if (pack != NULL)	// Valid pack?
				{
					pack->AddItem(pi);	// Add it
					weights::NewCalc(pTC);
				}
				else	// No pack, give it back to the GM
				{
					pack = pc_currchar->getBackpack();
					if (pack != NULL)	// Valid pack?
					{
						pack->AddItem(pi);	// Add it
						weights::NewCalc(pc_currchar);
					}
					else	// Even GM has no pack?
					{
						// Drop it to it's feet
						pi->MoveTo( charpos );
						pi->Refresh();
					}
				}
			}
			else
			{
                                //<Luxor>: secure trade
                //! \todo the sendpacket stuff here
                 pItem tradeCont = tradestart(pc_currchar, pTC);
                 if (ISVALIDPI(tradeCont)) {
                    tradeCont->AddItem( pi, 30, 30 );
                 } else {
                    Sndbounce5(s);
                    if (isDragging()) {
                 		resetDragging();
                 		UpdateStatusWindow(s,pi);
                   	}
                 }
                 //</Luxor>
		        }
	        }
	}
	else // dumping stuff to his own backpack !
	{
		droppedOnSelf( pi, loc, cont);
	}
	return true;

}

/*!
\brief item has been dropped on a pet and verifies if correct item given
\author Unknown, moved here by Chronodt (4/2/2004)
\param pi item to be dropped (already in dragging mode)
\param loc position to drop item at (eventually in cont)
\param cont container into which *pi has to be dropped (-1 = world)
\return bool
*/


bool cClient::droppedOnPet(pItem pi, Location &loc, pItem cont)
{
	pChar pet = pointers::findCharBySerial(cont->getSerial32());
	VALIDATEPCR(pet, false);
	pChar pc = currChar();
	VALIDATEPCR(pc, false);

	if((pet->hunger<6) && (pi->type==ITYPE_FOOD))//AntiChrist new hunger code for npcs
	{
		pc->playSFX( 0x3A+(rand()%3) );	//0x3A - 0x3C three different sounds

		if(pi->poisoned)
		{
			pet->applyPoison(PoisonType(pi->poisoned));
		}

		std::string itmname;
		if( pi->getCurrentName() == "#" )
		{
			char temp2[TEMP_STR_SIZE]; //xan -> this overrides the global temp var
			pi->getName(temp2);
			itmname = temp2;
		}
		else itmname = pi->getCurrentName();

		pet->emotecolor = 0x0026;
		pet->emoteall(TRANSLATE("* You see %s eating %s *"), 1, pet->getCurrentNameC(), itmname.c_str() );
		pet->hunger++;
	} else
	{
		ps->sysmsg(TRANSLATE("It doesn't appear to want the item"));
                //! \todo the sendpacket stuff here
		Sndbounce5(s);
		if (isDragging())
		{
			resetDragging();
			item_bounce5(s,pi);

		}
	}
	return true;
}

/*!
\brief item has been dropped on a guard and verifies if correct item given
\author Unknown, moved here by Chronodt (4/2/2004)
\param pi item to be dropped (already in dragging mode)
\param loc position to drop item at (eventually in cont)
\param cont container into which *pi has to be dropped (-1 = world)
\return bool
*/


bool cClient::droppedOnGuard(pItem pi, Location &loc, pItem cont)
{
	VALIDATEPIR(pi, false);
	char temp[TEMP_STR_SIZE]; //xan -> this overrides the global temp var
	pChar pc = currChar();
	VALIDATEPCR(pc,false);
	pChar pc_t=pointers::findCharBySerial(cont->getSerial32()); //the guard
	VALIDATEPCR(pc_t,false);
	// Search for the key word "the head of"
        //! \todo change check for text to check for id
	if( strstr( pi->getCurrentNameC(), "the head of" ) ) //!!! Wrong! it must check the ItemID, not the name :(
	{
		// This is a head of someone, see if the owner has a bounty on them
		pChar own=pointers::findCharBySerial(pi->getOwnerSerial32());
		VALIDATEPCR(own,false);

		if( own->questBountyReward > 0 )
		{
			// Give the person the bounty assuming that they are not the
			// same person as the reward is for
			if( pc->getSerial32() != own->getSerial32() )
			{
				// give them the gold for bringing the villan to justice
				pc->addGold(own->questBountyReward);
				pc->playSFX( goldsfx( own->questBountyReward ) );

				// Now thank them for their hard work
				sprintf( temp, TRANSLATE("Excellent work! You have brought us the head of %s. Here is your reward of %d gold coins."),
					own->getCurrentNameC(), own->questBountyReward );
				pc_t->talk( this, temp, 0);

				// Delete the Bounty from the bulletin board
				BountyDelete(own );

				// xan : increment fame & karma :)
				pc->modifyFame( ServerScp::g_nBountyFameGain );
				pc->IncreaseKarma(ServerScp::g_nBountyKarmaGain);
			}
			else
				pc_t->talk(this, TRANSLATE("You can not claim that prize scoundrel. You are lucky I don't strike you down where you stand!"),0);

			// Delete the item
			pi->Delete();
		}
	}
	return true;
}

/*!
\brief item has been dropped on a beggar and verifies if correct item given
\author Unknown, moved here by Chronodt (4/2/2004)
\param pi item to be dropped (already in dragging mode)
\param loc position to drop item at (eventually in cont)
\param cont container into which *pi has to be dropped (-1 = world)
\return bool
*/

bool cClient::droppedOnBeggar(pItem pi, Location &loc, pItem cont)
{

	VALIDATEPIR(pi, false);

	char temp[TEMP_STR_SIZE]; //xan -> this overrides the global temp var


	pChar pc=currChar();
	VALIDATEPCR(pc,false);

	pChar pc_t=pointers::findCharBySerial(cont->getSerial32()); //beggar
	VALIDATEPCR(pc_t,false);

	if(pi->getId()!=ITEMID_GOLD)
	{
		sprintf(temp,TRANSLATE("Sorry %s i can only use gold"), pc->getCurrentNameC());
		pc_t->talk(this,temp,0);
                //! \todo the sendpacket stuff here
		Sndbounce5(s);
		if (isDragging())
		{
		        resetDragging();
			item_bounce5(s,pi);
			return true;
		}
	}
	else
	{
		sprintf(temp,TRANSLATE("Thank you %s for the %i gold!"), pc->getCurrentNameC(), pi->amount);
		pc_t->talk(this,temp,0);
		if(pi->amount<=100)
		{
			pc->IncreaseKarma(10);
			ps->sysmsg(TRANSLATE("You have gain a little karma!"));
		}
		else if(pi->amount>100)
		{
			pc->IncreaseKarma(50);
			ps->sysmsg(TRANSLATE("You have gain some karma!"));
		}
		pi->Delete();
		return true;
	}
	return true;
}

/*!
\brief item has been dropped on a trainer and verifies if correct item given
\author Unknown, moved here by Chronodt (4/2/2004)
\param pi item to be dropped (already in dragging mode)
\param loc position to drop item at (eventually in cont)
\param cont container into which *pi has to be dropped (-1 = world)
\return bool
*/


bool cClient::droppedOnTrainer(pItem pi, Location &loc, pItem cont)
{
	VALIDATEPIR(pi, false);

	pChar pc = currChar();
	VALIDATEPCR(pc,false);
	pChar pc_t = pointers::findCharBySerial(cont->getSerial32());
	VALIDATEPCR(pc_t,false);

	if( pi->getId() == ITEMID_GOLD )
	{ // They gave the NPC gold
		UI08 sk=pc_t->trainingplayerin;
		pc_t->talk(this, TRANSLATE("I thank thee for thy payment. That should give thee a good start on thy way. Farewell!"),0);

		int sum = pc->getSkillSum();
		int delta = pc_t->getTeachingDelta(pc, sk, sum);

		if(pi->amount>delta) // Paid too much
		{
			pi->amount-=delta;
                        //! \todo the sendpacket stuff here
			Sndbounce5(s);
			if (isDragging())
			{
			        resetDragging();
				item_bounce5(s,pi);
			}
		}
		else
		{
			if(pi->amount < delta)		// Gave less gold
				delta = pi->amount;		// so adjust skillgain
			pi->Delete();
		}
		pc->baseskill[sk]+=delta;
		Skills::updateSkillLevel(pc, sk);
		pc->updateSkill(sk);

		pc->trainer=-1;
		pc_t->trainingplayerin=0xFF;
		pc->playSFX( itemsfx(pi->getId()) );
	}
	else // Did not give gold
	{
		pc_t->talk(this, TRANSLATE("I am sorry, but I can only accept gold."),0);
                //! \todo the sendpacket stuff here
		Sndbounce5(s);
		if (isDragging())
		{
			resetDragging();
			item_bounce5(s,pi);
		}
	}//if items[i]=gold
	return true;
}

/*!
\brief item has been dropped on self and verifies if it was a correct item
\author Unknown, moved here by Chronodt (4/2/2004)
\param pi item to be dropped (already in dragging mode)
\param loc position to drop item at (eventually in cont)
\param cont container into which *pi has to be dropped (-1 = world)
\return bool
*/


bool cClient::droppedOnSelf(pItem pi, Location &loc, pItem cont)
{

	VALIDATEPIR(pi, false);
	pChar pc = currChar();
	VALIDATEPCR(pc, false);

	Location charpos = pc->getPosition();

	if (pi->getId() >= 0x4000 ) // crashfix , prevents putting multi-objects ni your backback
	{
		sysmsg(TRANSLATE("Hey, putting houses in your pack crashes your back and client !"));
		pi->MoveTo( charpos );
		pi->Refresh();//AntiChrist
		return true;
	}

//	if (pi->glow>0) // glowing items
//	{
//		pc->addHalo(pi);
//		pc->glowHalo(pi);
//	}

	pItem pack = pc->getBackpack();
	if (pack==NULL) // if player has no pack, put it at its feet
	{
		pi->MoveTo( charpos );
		pi->Refresh();
	}
	else
	{
		pack->AddItem(pi); // player has a pack, put it in there

		weights::NewCalc(pc);//AntiChrist bugfixes
		statwindow(pc,pc);
		pc->playSFX( itemsfx(pi->getId()) );
	}
	return true;
}

