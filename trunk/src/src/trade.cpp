/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "common_libs.h"
#include "objects/citem.h"
#include "objects/cpc.h"

// this is a q&d fix for 'sell price higher than buy price' bug (Duke, 30.3.2001)
static bool items_match(pItem pi1,pItem pi2)
{
	if ( ! pi1 || ! pi2 ) return false;

	if (pi1->getId()==pi2->getId() &&
		pi1->type==pi2->type &&
		!(pi1->getId()==0x14F0 && (pi1->morex!=pi2->morex)) &&	// house deeds only differ by morex
		!(pi1->isShield() && pi1->getSecondaryName() == pi2->getSecondaryName()) &&	// magic shields only differ by secondary name
		pi1->getColor()==pi2->getColor())		// this enables color checking for armour, but disables dyed clothes :)
		return true;
	return false;

}


/*!
\brief The Trade must go on ... 
\author Luxor
\todo Rewrite to cPacket !!!!
*/
pItem tradestart(pPC pc1, pPC pc2)
{
	if (
		( ! pc1 || ! pc2 ) ||
		( !pc1->getClient() || ! pc2->getClient() ) ||
		( pc1->isDead() || pc2->isDead() ) ||
		( pc1->distFrom( pc2 ) > 5 )
	   )
		return NULL;

        pItem bp1 = pc1->getBody()->getBackpack();
	if ( ! bp1 )
	{
		pc1->getClient()->sysmessage("Time to buy a backpack!");
		return NULL;
	}
        pItem bp2 = pc2->getBody()->getBackpack();
	if ( ! bp2 )
	{
		pc1->getClient()->sysmessage("Time to buy a backpack!");
		return NULL;
	}

//!\todo Need to change this
//	pItem cont1 = item::CreateFromScript( "$item_a_bulletin_board" );
//	pItem cont2 = item::CreateFromScript( "$item_a_bulletin_board" );
	pItem cont1 = NULL, cont2 = NULL;
	if ( ! cont1 || ! cont2 )
		return NULL;

	cont1->setPosition(26, 0, 0);
	cont2->setPosition(26, 0, 0);
	cont1->setContainer(pc1->getBody());
	cont2->setContainer(pc2->getBody());
	cont1->layer = cont2->layer = 0;
	cont1->type = cont2->type = 1;
	cont1->setDyeable(false);
	cont2->setDyeable(false);
        
	//!\todo find client1 & client2
        pc1->getClient()->showItemInContainer(cont1);
        pc2->getClient()->showItemInContainer(cont1);
        pc1->getClient()->showItemInContainer(cont2);
        pc2->getClient()->showItemInContainer(cont2);

        cont2->more2.more = cont1->getSerial();
	cont2->morez=0;
	cont1->more2.more = cont2->getSerial();
	cont1->morez=0;

        uint8_t msg[90];
        msg[0]=0x6F;    //Header Byte
	msg[1]=0;       //Size
	msg[2]=47;      //Size
	msg[3]=0;       //Initiate
        LongToCharPtr(pc2->getSerial(), msg+4);
	LongToCharPtr(cont1->getSerial(), msg+8);
	LongToCharPtr(cont2->getSerial(), msg+12);
	msg[16]=1;
	strcpy((char*)&(msg[17]), pc2->getBody()->getCurrentName().c_str());
	Xsend(s1, msg, 47);

        uint8_t msg2[90];
        msg2[0]=0x6F;   //Header Byte
	msg2[1]=0;      //Size
	msg2[2]=47;     //Size
	msg2[3]=0;      //Initiate
	LongToCharPtr(pc1->getSerial(),   msg2+4);
	LongToCharPtr(cont2->getSerial(), msg2+8);
	LongToCharPtr(cont1->getSerial(), msg2+12);
	msg2[16]=1;
	strcpy((char*)&(msg2[17]), pc1->getBody()->getCurrentName().c_str());
	Xsend(s2, msg2, 47);

        return cont1;
}


void clearalltrades()
{
ConOut("... Sheeezzz ");
/*        pItem pi = NULL;
        pItem pj = NULL;
        pChar pc = NULL;
        pItem pack = NULL;
        uint32_t i = 0;
        for (i = 0; i < itemcount; i++) {
                pi = MAKE_ITEM_REF(i);
                if (!pi) continue;
                if ((pi->type==1) && (pi->getPosition().x==26) && (pi->getPosition().y==0) && (pi->getPosition().z==0) &&
			(pi->id()==0x1E5E))
		{
                        pc = cSerializable::findCharBySerial(pi->getContSerial());
                        if (pc) {
                                pack = pc->getBackpack();
                                if (pack) {
                                    NxwItemWrapper si;
									si.fillItemsInContainer( pi );
									for( si.rewind(); !si.isEmpty(); si++ ) {
										pj = si.getItem();
                                        if( pj) {
                       						pj->setContainer(pack);
                        				}
									}
                                    pi->deleteItem();
        							InfoOut("trade cleared\n");
                                }
                        }
                }
        }*/
}



/*
void restock(bool total)
{
        //Luxor: new cAllObjects system -> this should be changed soon... too slow!!

	cAllObjectsIter objs;
	pItem pi;
	for( objs.rewind(); !objs.IsEmpty(); objs++ )
	{
		if( isCharSerial( objs.getSerial() ) )
			continue;

		pi = (pItem)(objs.getObject());

		if(!pi || pi->layer!=0x1A )
			continue;

		NxwItemWrapper si;
		si.fillItemsInContainer( pi, false ); //ndEndy We don't need subcontainer right?
		for( si.rewind(); !si.isEmpty(); si++ ) {
			pItem pj=si.getItem();
			if( !pj || !pj->restock )
				continue;

			if ( total )
			{
				pj->amount+=pj->restock;
				pj->restock=0;
			}
			else
			{
				if (pj->restock>0)
				{
					int a=qmin(pj->restock, (pj->restock/2)+1);
					pj->amount+=a;
					pj->restock-=a;
				}
			}
		}
		//ndEndy i dont know why is here and what do..
		//if (SrvParms->trade_system==1) StoreItemRandomValue(pi,-1);// Magius(CHE) (2)
	}
}
*/

cRestockMng::cRestockMng()
{
	timer=getclock();
}

void cRestockMng::doRestock()
{
	if( !TIMEOUT( timer ) )
		return;

	if( needrestock.empty() ) {
		rewindList();
	}

	int count= RESTOCK_PER_TIME;

	while( ( --count>0 ) && ( !needrestock.empty() ) ) {

		pItem pi= cSerializable::findItemBySerial( needrestock.front() );
		this->needrestock.pop();
		if( pi && pi->layer==LAYER_TRADE_RESTOCK  ) {

			NxwItemWrapper si;
			si.fillItemsInContainer( pi, false ); //ndEndy We don't need subcontainer right?
			for( si.rewind(); !si.isEmpty(); si++ ) {

				pItem pj=si.getItem();
				if( !pj || ( pj->restock <= 0 ) )
					continue;

				int a=qmin(pj->restock, (pj->restock/2)+1);
				pj->amount+=a;
				pj->restock-=a;

			}

			restocked.push( pi->getSerial() );

		}

	}

	updateTimer();
}

void cRestockMng::doRestockAll()
{

	rewindList();

	while( !needrestock.empty() ) {

		pItem pi= cSerializable::findItemBySerial( needrestock.front() );
		this->needrestock.pop();
		if( pi && pi->layer==LAYER_TRADE_RESTOCK ) {

			NxwItemWrapper si;
			si.fillItemsInContainer( pi, false ); //ndEndy We don't need subcontainer right?
			for( si.rewind(); !si.isEmpty(); si++ ) {

				pItem pj=si.getItem();
				if( !pj || ( pj->restock <= 0 ) )
					continue;

				pj->amount+=pj->restock;
				pj->restock=0;

			}

			restocked.push( pi->getSerial() );

		}

	}

	updateTimer();

}

void cRestockMng::addNewRestock( pItem pi )
{
	if ( ! pi ) return;
	this->needrestock.push( pi->getSerial() );
}

void cRestockMng::rewindList()
{
	while( !restocked.empty() ) {
		needrestock.push( restocked.front() );
		restocked.pop();
	}
}

void cRestockMng::updateTimer()
{
	if( needrestock.empty() ) //end restock.. next after much time
		timer=getclock()+ServerScp::g_nRestockTimeRate*60*MY_CLOCKS_PER_SEC;
	else
		timer=getclock()+CHECK_RESTOCK_EVERY*MY_CLOCKS_PER_SEC;
}

cRestockMng* Restocks = NULL;
