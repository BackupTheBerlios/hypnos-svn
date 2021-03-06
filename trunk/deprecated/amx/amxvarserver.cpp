  /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    || NoX-Wizard UO Server Emulator (NXW) [http://noxwizard.sourceforge.net]  ||
    ||                                                                         ||
    || This software is free software released under GPL2 license.             ||
    || You can find detailed license information in nox-wizard.cpp file.       ||
    ||                                                                         ||
    || For any question post to NoX-Wizard forums.                             ||
    -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#include "amxvarserver.h"
#include "scp_parser.h"

AMXVARSRV_DATATYPE amxVariable::getType()
{
	return AMXVARSRV_UNDEFINED;
}

int32_t amxVariable::getSize( const int32_t index )
{
	return 0;
}
//
// amxIntegerVariable
//
amxIntegerVariable::amxIntegerVariable( const int32_t initialValue )
{
	value = initialValue;
}

amxIntegerVariable::~amxIntegerVariable()
{
}

AMXVARSRV_DATATYPE	amxIntegerVariable::getType()
{
	return AMXVARSRV_INTEGER;
}

int32_t	amxIntegerVariable::getValue()
{
	return value;
}

void	amxIntegerVariable::setValue( const int32_t newValue )
{
	value = newValue;
}

int32_t amxIntegerVariable::getSize()
{
	return sizeof( value );
}

//
// amxIntegerVector
//
amxIntegerVector::amxIntegerVector( const int32_t size, const int32_t initialValue )
{
	value.resize( size, initialValue );
}

amxIntegerVector::~amxIntegerVector()
{
	//
	//	Sparhawk: Dumb gcc 3.3 bug, bogus statement else internal compiler error
	//
	value.clear();
}

AMXVARSRV_DATATYPE amxIntegerVector::getType()
{
	return AMXVARSRV_INTEGERVECTOR;
}

int32_t amxIntegerVector::getValue( const uint32_t index )
{
	if( (uint32_t)index >= value.size() )
		return -1;
	return value[ index ];
}

void amxIntegerVector::setValue( const uint32_t index, const int32_t newValue )
{
	if( (uint32_t)index >= value.size() )
		return ;
	value[ index ] = newValue;
}

int32_t amxIntegerVector::getSize( const int32_t index )
{
	if( index == INVALID )
		return value.size();
	else
		return sizeof( int32_t );
}

//
// amxStringVariable
//
amxStringVariable::amxStringVariable( const std::string& initialValue )
{
	value = initialValue;
}

amxStringVariable::~amxStringVariable()
{
}

AMXVARSRV_DATATYPE	amxStringVariable::getType()
{
	return AMXVARSRV_STRING;
}

std::string	amxStringVariable::getValue()
{
	return value;
}

void	amxStringVariable::setValue( const std::string& newValue )
{
	value = newValue;
}

int32_t amxStringVariable::getSize()
{
	return value.size();
}

//@{
/*!
\name AmxScriptId
\brief AmxScriptId variable
*/


/*
\brief
\author Endymion
\todo write it
*/
amxScriptIdVariable::amxScriptIdVariable( char* initialValue )
{
// Akron: this line doesn't compile under serious c++ compilers...
//	amxScriptIdVariable( xss::getIntFromDefine( initialValue ) );
}

/*
\brief
\author Endymion
*/
amxScriptIdVariable::amxScriptIdVariable( uint32_t initialValue )
{
	value=initialValue;
}

/*
\brief
\author Endymion
*/
amxScriptIdVariable::~amxScriptIdVariable()
{
}

/*
\brief
\author Endymion
*/
AMXVARSRV_DATATYPE amxScriptIdVariable::getType()
{
	return AMXVARSRV_SCRIPTID;
}

/*
\brief
\author Endymion
*/
uint32_t amxScriptIdVariable::getValue()
{
	return value;
}

/*
\brief
\author Endymion
*/
void amxScriptIdVariable::setValue( char* newValue )
{
	setValue( xss::getIntFromDefine( newValue ) );
}

/*
\brief
\author Endymion
*/
void amxScriptIdVariable::setValue( uint32_t newValue )
{
	value=newValue;
}

/*
\brief
\author Endymion
*/
int32_t amxScriptIdVariable::getSize()
{
	return sizeof( value );
}

//@}


//
//	amxVariableServer
//
amxVariableServer::amxVariableServer()
{
	setUserMode();
	error = AMXVARSRV_OK;
}

amxVariableServer::~amxVariableServer()
{
}

int32_t amxVariableServer::getError()
{
	return error;
}

bool amxVariableServer::inUserMode()
{
	return mode;
}

bool amxVariableServer::inServerMode()
{
	return !mode;
}

void	amxVariableServer::setUserMode()
{
	mode = true;
}

void amxVariableServer::setServerMode()
{
	mode = false;
}

int32_t amxVariableServer::firstVariable( const uint32_t serial )
{
	
	amxObjectVariableMapIterator object = varMap.find( serial );

	if( object != varMap.end() )
		if( !object->second.empty() )
			return object->second.begin()->first;
	return -1;
}

int32_t amxVariableServer::nextVariable( const uint32_t serial, const int32_t previous )
{
	amxObjectVariableMapIterator object = varMap.find( serial );

	if( object != varMap.end() )
	{
		amxVariableMapIterator variable = object->second.find( previous );
		if( ++variable != object->second.end() )
			return variable->first;
	}
	return -1;
}

AMXVARSRV_DATATYPE amxVariableServer::typeOfVariable( const uint32_t serial, const int32_t variable )
{
	amxObjectVariableMapIterator object = varMap.find( serial );

	if( object != varMap.end() )
	{
		amxVariableMapIterator subject = object->second.find( variable );
		return subject->second->getType();
	}
	return AMXVARSRV_UNDEFINED;
}

bool amxVariableServer::insertVariable( const uint32_t serial, const int32_t variable, const int32_t value )
{
	if ( variable < 1000 && inUserMode() )
	{
			error = AMXVARSRV_ACCESS_DENIED;
			return false;
	}

	if( !existsVariable( serial, variable, 0 ) )
	{
		varMap[ serial ][ variable ] = new amxIntegerVariable( value );
		error = AMXVARSRV_OK;
		return true;
	}
	error = AMXVARSRV_DUPLICATE_VAR;
	return false;
}

bool amxVariableServer::insertVariable( const uint32_t serial, const int32_t variable, const std::string& value )
{
	if ( variable < 1000 && inUserMode() )
	{
			error = AMXVARSRV_ACCESS_DENIED;
			return false;
	}

	if( !existsVariable( serial, variable, 0 ) && variable >= 1000 )
	{
		varMap[ serial ][ variable ] = new amxStringVariable( value );
		error = AMXVARSRV_OK;
		return true;
	}
	error = AMXVARSRV_DUPLICATE_VAR;
	return false;
}

bool amxVariableServer::insertVariable( const uint32_t serial, const int32_t variable, const int32_t size, const int32_t value )
{
	if ( variable < 1000 && inUserMode() )
	{
			error = AMXVARSRV_ACCESS_DENIED;
			return false;
	}

	if( !existsVariable( serial, variable, 0 ) )
	{
		varMap[ serial ][ variable ] = new amxIntegerVector( size, value );
		error = AMXVARSRV_OK;
		return true;
	}
	error = AMXVARSRV_DUPLICATE_VAR;
	return false;
}

bool amxVariableServer::deleteVariable( const uint32_t serial, const int32_t variable )
{
	if ( variable < 1000 && inUserMode() )
	{
			error = AMXVARSRV_ACCESS_DENIED;
			return false;
	}

	if( existsVariable( serial, variable, 0 ) && variable >= 1000 )
	{
		varMap[ serial ].erase( variable );
		error = AMXVARSRV_OK;
		return true;
	}
	error = AMXVARSRV_UNKNOWN_VAR;
	return false;
}

bool amxVariableServer::deleteVariable( const uint32_t serial )
{
	error = error = AMXVARSRV_OK;
	return varMap.erase( serial );
}

bool amxVariableServer::updateVariable( const uint32_t serial, const int32_t variable, const int32_t value )
{
	if( existsVariable( serial, variable, AMXVARSRV_INTEGER ) )
	{
		static_cast<amxIntegerVariable*>(varMap[serial][variable])->setValue( value );
		error = AMXVARSRV_OK;
		return true;
	}
	else
		if ( variable <= 16 )
		{
			setServerMode();
			insertVariable( serial, variable, value );
			setUserMode();
			error = AMXVARSRV_OK;
			return true;
		}
	error = AMXVARSRV_UNKNOWN_VAR;
	return false;
}

bool amxVariableServer::updateVariable( const uint32_t serial, const int32_t variable, const std::string& value )
{
	if( existsVariable( serial, variable, AMXVARSRV_STRING ) )
	{
		static_cast<amxStringVariable*>(varMap[serial][variable])->setValue( value );
		error = AMXVARSRV_OK;
		return true;
	}
	error = AMXVARSRV_UNKNOWN_VAR;
	return false;
}

bool	amxVariableServer::updateVariable( const uint32_t serial, const int32_t variable, const int32_t index, const int32_t value )
{
	if( existsVariable( serial, variable, AMXVARSRV_INTEGERVECTOR ) )
	{
		static_cast<amxIntegerVector*>(varMap[serial][variable])->setValue( index, value );
		error = AMXVARSRV_OK;
		return true;
	}
	error = AMXVARSRV_UNKNOWN_VAR;
	return false;
}

bool	amxVariableServer::selectVariable( const uint32_t serial, const int32_t variable, const int32_t index, int32_t& value )
{
	if( existsVariable( serial, variable, AMXVARSRV_INTEGERVECTOR ) )
	{
		value = static_cast<amxIntegerVector*>(varMap[serial][variable])->getValue( index );
		error = AMXVARSRV_OK;
		return true;
	}
	error = AMXVARSRV_UNKNOWN_VAR;
	return false;
}

bool amxVariableServer::selectVariable( const uint32_t serial, const int32_t variable, int32_t& value )
{
	if( existsVariable( serial, variable, AMXVARSRV_INTEGER ) )
	{
		value = static_cast<amxIntegerVariable*>(varMap[serial][variable])->getValue();
		error = AMXVARSRV_OK;
		return true;
	}
	else
		if( variable < 16 )
		{
			value = 0;
			error = AMXVARSRV_OK;
			return true;
		}
	error = AMXVARSRV_UNKNOWN_VAR;
	return false;
}

bool amxVariableServer::selectVariable( const uint32_t serial, const int32_t variable, std::string& value )
{
	if( existsVariable( serial, variable, AMXVARSRV_STRING ) )
	{
		value = static_cast<amxStringVariable*>(varMap[serial][variable])->getValue();
		error = AMXVARSRV_OK;
		return true;
	}

	error = AMXVARSRV_UNKNOWN_VAR;
	return false;
}

bool amxVariableServer::existsVariable( const uint32_t serial, const int32_t variable, const int32_t type )
{
	amxObjectVariableMapIterator ovmIt( varMap.find( serial ) );
	if( ovmIt == varMap.end() )
	{
/*		if ( variable < 16 )
			if( type == AMXVARSRV_UNDEFINED || type == AMXVARSRV_INTEGER )
			{
				error = AMXVARSRV_OK;
				return true;
			}
			else
				error = AMXVARSRV_WRONG_TYPE;
		else
			error = AMXVARSRV_UNKNOWN_VAR;
*/
		error = AMXVARSRV_UNKNOWN_VAR;
		return false;
	}
	amxVariableMapIterator vmIt( ovmIt->second.find( variable ) );
	if( vmIt == ovmIt->second.end() )
	{
		error = AMXVARSRV_UNKNOWN_VAR;
		return false;
	}
	if( vmIt->second->getType() == type || type == 0 )
	{
		error = AMXVARSRV_OK;
		return true;
	}
	error = AMXVARSRV_WRONG_TYPE;
	return false;
}

int32_t amxVariableServer::countVariable()
{
	amxObjectVariableMapIterator ovmItBegin( varMap.begin() ), ovmItEnd( varMap.end() );
	int32_t count = 0;
	while( ovmItBegin != ovmItEnd )
	{
		count += ovmItBegin->second.size();
		++ovmItBegin;
	}
	return count;
}

int32_t amxVariableServer::countVariable( const uint32_t serial )
{
	amxObjectVariableMapIterator ovmItBegin( varMap.find( serial ) ), ovmItEnd( varMap.end() );
	if( ovmItBegin != ovmItEnd )
		return ovmItBegin->second.size();
	return 0;
}

int32_t amxVariableServer::countVariable( const uint32_t serial, const SERIAL type )
{
	int32_t count = 0;
	amxObjectVariableMapIterator ovmItBegin( varMap.begin() ), ovmItEnd( varMap.end() );
	if( ovmItBegin != ovmItEnd )
	{
		amxVariableMapIterator vmItBegin( ovmItBegin->second.begin() ), vmItEnd( ovmItBegin->second.end() );
		while( vmItBegin != vmItEnd )
		{
			if( type == 0 || vmItBegin->second->getType() == type )
				++count;
			++vmItBegin;
		}
	}
	return count;
}

bool amxVariableServer::moveVariable( const uint32_t fromSerial, const SERIAL toSerial )
{
	if( copyVariable( fromSerial, toSerial ) )
	{
		deleteVariable( fromSerial );
		return true;
	}
	return false;
}

bool amxVariableServer::copyVariable( const uint32_t fromSerial, const SERIAL toSerial )
{
	amxObjectVariableMapIterator ovmIt( varMap.find( fromSerial ) );
	if( ovmIt == varMap.end() )
		return false;
	deleteVariable( toSerial );
	varMap[ toSerial ] = varMap[ fromSerial ];
	return true;
}

int32_t	amxVariableServer::size( const uint32_t serial, const int32_t variable, const int32_t index )
{
	if( existsVariable( serial, variable, AMXVARSRV_UNDEFINED ) )
	{
		//value = static_cast<amxStringVariable*>(varMap[serial][variable])->getValue();
		error = AMXVARSRV_OK;
		return varMap[serial][variable]->getSize( index );
	}

	error = AMXVARSRV_UNKNOWN_VAR;
	return INVALID;
}

void amxVariableServer::saveVariable( uint32_t serial, FILE * stream )
{
	amxObjectVariableMapIterator ovmItBegin( varMap.find( serial ) ), ovmItEnd( varMap.end() );
	if( ovmItBegin != ovmItEnd )
	{
		amxVariableMapIterator vmItBegin( ovmItBegin->second.begin() ), vmItEnd( ovmItBegin->second.end() );
		while( vmItBegin != vmItEnd )
		{
			switch( vmItBegin->second->getType() )
			{
				case AMXVARSRV_UNDEFINED:
					break;
				case AMXVARSRV_INTEGER	:
					if( !(vmItBegin->first < 16 && static_cast<amxIntegerVariable*>(vmItBegin->second)->getValue() == 0) )
						fprintf( stream, "AMXINT %d %d\n", vmItBegin->first, static_cast<amxIntegerVariable*>(vmItBegin->second)->getValue() );
					break;
				case AMXVARSRV_bool	:
					break;
				case AMXVARSRV_STRING		:
					fprintf( stream, "AMXSTR %d %s\n", vmItBegin->first, static_cast<amxStringVariable*>(vmItBegin->second)->getValue().c_str() );
					break;
				case AMXVARSRV_INTEGERVECTOR	:
					{
					int32_t vectorSize = static_cast<amxIntegerVector*>(vmItBegin->second)->getSize();
					fprintf( stream, "AMXINTVEC %d %d\n{\n", vmItBegin->first, vectorSize );
					for( int32_t vectorIndex = 0; vectorIndex < vectorSize; ++vectorIndex )
						fprintf( stream, "%d\n", static_cast<amxIntegerVector*>(vmItBegin->second)->getValue( vectorIndex ) );
					fprintf( stream, "}\n" );
					}
					break;
			}
			++vmItBegin;
		}
	}
}
