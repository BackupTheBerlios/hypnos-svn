/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief PyUO natives for Python - Character Management

\note If nothing specified, the function accepts only 1 params and is the
	character to use the function on.
*/

using namespace PythonInterface;

//! get if the character Yell is a server broadcast
PYNATIVE(chr_canBroadcast)
{
	P_CHAR pc = NULL;
	if ( !PyArg_ParseTuple(args, "i", &pc) )
		return NULL;

	if ( ! pc ) return PYNULL;
	return Py_BuildValue("i", pc->CanBroadcast());
}

//! get if the character can see serial number
PYNATIVE(chr_canSeeSerials)
{
	P_CHAR pc = NULL;
	if ( !PyArg_ParseTuple(args, "i", &pc) )
		return NULL;

	if ( ! pc ) return PYNULL;
	return Py_BuildValue("i", pc->CanSeeSerials());
}

//! get if the character can snoop everywhere always
PYNATIVE(chr_canSnoop)
{
	P_CHAR pc = NULL;
	if ( !PyArg_ParseTuple(args, "i", &pc) )
		return NULL;

	if ( ! pc ) return PYNULL;
	return Py_BuildValue("i", pc->CanSnoop());
}

//! get the total gold in character bank box
PYNATIVE(chr_countBankGold)
{
	P_CHAR pc = NULL;
	if ( !PyArg_ParseTuple(args, "i", &pc) )
		return NULL;

	if ( ! pc ) return PYNULL;
	return Py_BuildValue("i", pc->countBankGold());
}

//! get the total gold in character backpack
PYNATIVE(chr_countGold)
{
	P_CHAR pc = NULL;
	if ( !PyArg_ParseTuple(args, "i", &pc) )
		return NULL;

	if ( ! pc ) return PYNULL;
	return Py_BuildValue("i", pc->CountGold());
}

/*
\brief disturb the meditation of the character
\return INVALID if not valid character, 0 else
*/
PYNATIVE(chr_disturbMeditation)
{
	P_CHAR pc = NULL;
	if ( !PyArg_ParseTuple(args, "i", &pc) )
		return NULL;

	if ( ! pc ) return PYNULL;

	pc->disturbMed();
	return Py_BuildValue("i", 0);
}

/*
\brief get the char's bankbox
\param 1: the character
\param 2: bank box type ( BANKBOX_BANK, BANKBOX_WARE )
\return INVALID if not valid character, else the bank
*/
PYNATIVE(chr_getBankBox)
{
	P_CHAR pc = NULL;
	SI32 bank_type = BANK_GOLD;
	if ( !PyArg_ParseTuple(args, "i|i", &pc, &bank_type) )
		return NULL;

	if ( ! pc ) return PYNULL;
        P_ITEM pi = pc->GetBankBox(bank_type);
        if ( ! pi ) return PYNULL;
        return Py_BuildValue("i", pi);
}

//! get character's shield
PYNATIVE(chr_getShield)
{
	P_CHAR pc = NULL;
	if ( !PyArg_ParseTuple(args, "i", &pc) )
		return NULL;

	P_CHAR pc = pointers::findCharBySerial(serial);
	if ( ! pi

	P_ITEM pi = pc->getShield();
	if ( ! pi ) return PYNULL;
	return Py_BuildValue("i", pi);
}

//! get character's shield
PYNATIVE(chr_getWeapon)
{
	P_CHAR pc = NULL;
	if ( !PyArg_ParseTuple(args, "i", &pc) )
		return NULL;

	P_CHAR pc = pointers::findCharBySerial(serial);
	if ( ! pc ) return PYNULL;

	P_ITEM pi = pc->getWeapon();
	if ( ! pi ) return PYNULL;
	return Py_BuildValue("i", pi);
}

/*
\brief get the serial of the item on the specified layer of the character
\param 1: the character
\param 2: layer
\return INVALID on failure, item serial on success
*/
PYNATIVE(chr_getItemOnLayer)
{
	SI32 layer;
	P_CHAR pc = NULL;
	if ( !PyArg_ParseTuple(args, "ii", &pc, &layer) )
		return NULL;

	P_CHAR pc = pointers::findCharBySerial(serial);
	if ( ! pc ) return PYNULL;

	P_ITEM pi = pc->GetItemOnLayer(layer);
	if ( ! pi ) return PYNULL;
	return Py_BuildValue("i", pi);
}

//! check if character is criminal
PYNATIVE(chr_isCriminal)
{
	P_CHAR pc = NULL;
	if ( !PyArg_ParseTuple(args, "i", &pc) )
		return NULL;

	P_CHAR pc = pointers::findCharBySerial(serial);
	VALIDATEPCR( pc, Py_BuildValue("i", 0) );
	return Py_BuildValue("i", pc->isCriminal());
}

//! check if character is Grey for any reason
PYNATIVE(chr_isGrey)
{
	P_CHAR pc = NULL;
	if ( !PyArg_ParseTuple(args, "i", &pc) )
		return NULL;

	P_CHAR pc = pointers::findCharBySerial(serial);
	VALIDATEPCR( pc, Py_BuildValue("i", 0) );
	return Py_BuildValue("i", pc->isGrey());
}

//! check if character is a murderer
PYNATIVE(chr_isMurderer)
{
	P_CHAR pc = NULL;
	if ( !PyArg_ParseTuple(args, "i", &pc) )
		return NULL;

	P_CHAR pc = pointers::findCharBySerial(serial);
	VALIDATEPCR( pc, Py_BuildValue("i", 0) );
	return Py_BuildValue("i", pc->isMurderer());
}

//! check if character is blue flagged
PYNATIVE(chr_isInnocent)
{
	P_CHAR pc = NULL;
	if ( !PyArg_ParseTuple(args, "i", &pc) )
		return NULL;

	P_CHAR pc = pointers::findCharBySerial(serial);
	VALIDATEPCR( pc, Py_BuildValue("i", 0) );
	return Py_BuildValue("i", pc->isInnocent());
}

//! check if character have human body
PYNATIVE(chr_isHuman)
{
	P_CHAR pc = NULL;
	if ( !PyArg_ParseTuple(args, "i", &pc) )
		return NULL;

	P_CHAR pc = pointers::findCharBySerial(serial);
	VALIDATEPCR( pc, Py_BuildValue("i", 0) );
	return Py_BuildValue("i", pc->HasHumanBody());
}

//! check if character is invulnerable
PYNATIVE(chr_isInvulnerable)
{
	P_CHAR pc = NULL;
	if ( !PyArg_ParseTuple(args, "i", &pc) )
		return NULL;

	P_CHAR pc = pointers::findCharBySerial(serial);
	VALIDATEPCR( pc, Py_BuildValue("i", 0) );
	return Py_BuildValue("i", pc->IsInvul());
}

//! check make the character invulnerable
PYNATIVE(chr_makeInvulnerable)
{
	P_CHAR pc = NULL;
	if ( !PyArg_ParseTuple(args, "i", &pc) )
		return NULL;

	P_CHAR pc = pointers::findCharBySerial(serial);
	VALIDATEPCR( pc, Py_BuildValue("i", 0) );
	pc->MakeInvulnerable();
	return Py_BuildValue("i", 0);
}

//! check make the character vulnerable
PYNATIVE(chr_makeVulnerable)
{
	P_CHAR pc = NULL;
	if ( !PyArg_ParseTuple(args, "i", &pc) )
		return NULL;

	P_CHAR pc = pointers::findCharBySerial(serial);
	VALIDATEPCR( pc, Py_BuildValue("i", 0) );
	pc->MakeVulnerable();
	return Py_BuildValue("i", 0);
}

/*
\brief teleport char to given position
\param 1: the character
\param 2: x location
\param 3: y location
\param 4: z location
\return 0 or INVALID if not valid character
*/
PYNATIVE(chr_moveTo)
{
	P_CHAR pc = NULL;
	UI16 x, y;
	SI08 z;

	if ( !PyArg_ParseTuple(args, "iHHb", &pc, &x, &y, &z) )
		return NULL;

	if ( ! pc ) return PYNULL;

	pc->MoveTo(x,y,z);
	pc->teleport();
	return Py_BuildValue("i", 0);
}