/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief cChar's reputation methods
*/

const bool cChar::IsGrey() const
{
	if ( npc || IsMurderer() || IsCriminal() )
		return false;
	else
		if ( (karma <= -10000) || (nxwflags[0] & flagPermaGrey) || (nxwflags[0] & flagGrey) )
			return true;
		else
			return false;
}

void cChar::setMurderer()
{

	if (amxevents[EVENT_CHR_ONFLAGCHG])
		amxevents[EVENT_CHR_ONFLAGCHG]->Call(getSerial32() );
	//runAmxEvent( EVENT_CHR_ONFLAGCHG, getSerial32(), getSocket() );

	flag=flagKarmaMurderer;
}

void cChar::setInnocent()
{

	if (amxevents[EVENT_CHR_ONFLAGCHG])
		amxevents[EVENT_CHR_ONFLAGCHG]->Call(getSerial32() );
	//runAmxEvent( EVENT_CHR_ONFLAGCHG, getSerial32(), getSocket() );
	flag=flagKarmaInnocent;
}

void cChar::setCriminal()
{

	if (amxevents[EVENT_CHR_ONFLAGCHG])
		amxevents[EVENT_CHR_ONFLAGCHG]->Call(getSerial32() );
	//runAmxEvent( EVENT_CHR_ONFLAGCHG, getSerial32(), getSocket() );
	flag=flagKarmaCriminal;
}

//! Makes someone criminal
void cChar::makeCriminal()
{
	if ((!npc)&&(!IsCriminal() || !IsMurderer()))
	{//Not an npc, not grey, not red
		tempfx::add(this, this, tempfx::CRIMINAL, 0, 0, 0); //Luxor
		if(::region[region].priv&0x01 && SrvParms->guardsactive) { //guarded
			if (ServerScp::g_nInstantGuard == 1)
				npcs::SpawnGuard( this, this, getPosition() ); // LB bugfix
		}
	}
}

/*!
\brief increase or decrease the fame of the char
\author Endymion
\since 0.82a
\param value positive or negative value to add to fame
\note every increase of karma have an event and stuff related
*/
void cChar::modifyFame( SI32 value )
{
	if( GetFame() > 10000 )
		SetFame( 10000 );

	if ( value != 0 )
	{
		SI32	nFame	= value;
		int	nChange	= 0,
			nEffect	= 0;
		int	nCurFame= fame;

		if( nCurFame > nFame ) // if player fame greater abort function
		{
			return;
		}

		if( nCurFame < nFame )
		{
			nChange=(nFame-nCurFame)/75;
			fame=(nCurFame+nChange);
			nEffect=1;
		}

		if( dead )
		{
			if(nCurFame<=0)
			{
				fame=0;
			}
			else
			{
				nChange=(nCurFame-0)/25;
				fame=(nCurFame-nChange);
			}
			nEffect=0;
		}
		if( nChange != 0 )
		{

			if (amxevents[EVENT_CHR_ONREPUTATIONCHG])
			{
				g_bByPass = false;
				int n = nChange;
				if (!nEffect) n = -nChange;
				amxevents[EVENT_CHR_ONREPUTATIONCHG]->Call(getSerial32(), n, REPUTATION_FAME);
			}
			/*
			pc->runAmxEvent( EVENT_CHR_ONREPUTATIONCHG, pc->getSerial32(), (!nEffect ? -nChange : nChange), REPUTATION_FAME);
			*/
			if ( !g_bByPass && !npc )
			{
				if(nChange<=25)
				{
					if(nEffect)
						sysmsg( TRANSLATE("You have gained a little fame."));
					else
						sysmsg( TRANSLATE("You have lost a little fame."));
				}
				else if(nChange<=75)
				{
					if(nEffect)
						sysmsg( TRANSLATE("You have gained some fame."));
					else
						sysmsg( TRANSLATE("You have lost some fame."));
				}
				else if(nChange<=100)
				{
					if(nEffect)
						sysmsg( TRANSLATE("You have gained alot of fame."));
					else
						sysmsg( TRANSLATE("You have lost alot of fame."));
				}
				else if(nChange>100)
				{
					if(nEffect)
						sysmsg( TRANSLATE("You have gained a huge amount of fame."));
					else
						sysmsg( TRANSLATE("You have lost a huge amount of fame."));
				}
			}
		}
	}
}

/*!
\brief increase or decrease the karma of the char
\author Endymion
\since 0.82a
\param value positive or negative value to add to karma
\param killed ptr to killed char
\note every increase of karma has a related event
\note Sparhawk: karma increase now can also be applied to npc's
*/
void cChar::IncreaseKarma( SI32 value, P_CHAR pKilled )
{
	SI32 nCurKarma		= GetKarma();

	if( nCurKarma > 10000 )
		SetKarma( 10000 );
	else
		if( nCurKarma < -10000 )
			SetKarma( -10000 );

	if( value != 0 )
	{
		SI32 	nKarma			= value,
			nChange			= 0;

		bool	positiveKarmaEffect	= false;

		if	( nCurKarma < nKarma && nKarma > 0 )
		{
			nChange=((nKarma-nCurKarma)/75);
			SetKarma( GetKarma() + nChange );
			positiveKarmaEffect = true;
		}
		else if ( nCurKarma > nKarma )
		{
			if ( !ISVALIDPC( pKilled) )
			{
				nChange=((nCurKarma-nKarma)/50);
				SetKarma( GetKarma() + nChange );
			}
			else if( pKilled->GetKarma()>0 )
			{
				nChange=((nCurKarma-nKarma)/50);
				SetKarma( GetKarma() + nChange );
			}
		}

		if( nChange != 0 )
		{
			if ( amxevents[EVENT_CHR_ONREPUTATIONCHG] )
			{
				g_bByPass = false;
				SI32 n = nChange;
				if (!positiveKarmaEffect)
					n = -nChange;
				amxevents[EVENT_CHR_ONREPUTATIONCHG]->Call( getSerial32(), n, REPUTATION_KARMA );
			}
			/*
			pc->runAmxEvent( EVENT_CHR_ONREPUTATIONCHG, pc->getSerial32(), (!nEffect ? -nChange : nChange), REPUTATION_KARMA);
			if (g_bByPass==true)
				return;
			*/
			if( !g_bByPass && !npc )
			{
				if(nChange<=25)
				{
					if(positiveKarmaEffect)
						sysmsg( TRANSLATE("You have gained a little karma."));
					else
						sysmsg( TRANSLATE("You have lost a little karma."));
				}
				else if(nChange<=75)
				{
					if(positiveKarmaEffect)
						sysmsg( TRANSLATE("You have gained some karma."));
					else
						sysmsg( TRANSLATE("You have lost some karma."));
				}
				else if(nChange<=100)
				{
					if(positiveKarmaEffect)
						sysmsg( TRANSLATE("You have gained alot of karma."));
					else
						sysmsg( TRANSLATE("You have lost alot of karma."));
				}
				else if(nChange>100)
				{
					if(positiveKarmaEffect)
						sysmsg( TRANSLATE("You have gained a huge amount of karma."));
					else
						sysmsg( TRANSLATE("You have lost a huge amount of karma."));
				}
			}
		}
	}
}
