/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
#include "common_libs.h"
#include "network.h"
#include "cmds.h"
#include "sregions.h"
#include "bounty.h"
#include "sndpkg.h"
#include "debug.h"
#include "calendar.h"
#include "race.h"
#include "commands.h"
#include "telport.h"
#include "worldmain.h"
#include "data.h"
#include "spawn.h"
#include "trade.h"
#include "basics.h"
#include "inlines.h"
#include "map.h"
#include "jail.h"
#include "skills.h"

/*///////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
//Implementations of classes declared in cmds.h
////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////*/


//Implementation of cCommand Class

cCommand::cCommand(std::string& name, int8_t number ,AmxFunction* callback) {

	cmd_name=name;
	cmd_level=number; 
	cmd_callback=callback;
}


int8_t cCommand::getCommandLevel(pCommand cmd) {
	return cmd->cmd_level;
}


//Implementation of cCallCommand Class

uint32_t cCallCommand::current_serial = 0;



cCallCommand::cCallCommand(std::string params){

	all_params=params;
//	single_param=param;
}


cCallCommand::~cCallCommand() {
}




//Implementation of cCommandMap Class

cCommandMap::cCommandMap() {

	// all addGmCommand(...); goes here.

}


pCommand cCommandMap::addGmCommand(std::string name, int8_t priv, AmxFunction* callback) {

	pCommand cmd= new cCommand(name, priv, callback);
    command_map[name]= cmd;
 	return cmd;
}




bool cCommandMap::Check( string& text ){

	std::map< std::string, pCommand >::iterator iter( command_map.find( text ) );

	if ( iter == command_map.end() )	//command not exists
		return false;

	//other checks..

	return true;
}




pCommand cCommandMap::findCommand(std::string name){

		
	std::map< std::string, pCommand >::iterator iter( command_map.find( "name" ) );

	if ( iter != command_map.end() )	//command exists
		return iter->second;
	else
		return NULL;					//command doesnt exist
}





//Implementation of cCallCommandMap Class


cCallCommand* cCallCommand::findCallCommand(uint32_t cmd){

	std::map< uint32_t, cCallCommand* >::iterator iter( callcommand_map.find( cmd ) );

	if ( iter != callcommand_map.end() )	//command exists
		return iter->second;
	else
		return NULL;					//command doesnt exist
}




uint32_t cCallCommand::addCallCommand(cCallCommand* called){

	callcommand_map[++current_serial]=called;
	return current_serial;

}



void cCallCommand::delCommand(uint32_t cmd){

	callcommand_map.erase(cmd);
}



cCommandMap* commands = new cCommandMap();





/*///////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
//Todo's
////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////*/




//The function that is called after the control done in speech.cpp

/*!
\todo The new splitline function doesn't works with old global vars, so all this
is to be changed
*/
void Command(NXWSOCKET  s, char* speech) // Client entred a command like 'ADD
{
	unsigned char *comm;
	unsigned char nonuni[512];
	//cCallCommand* command; 

	//cmd_offset = 1;

	//cCallCommandMap* callcommands = new cCallCommandMap()
	
	
	pChar pc_currchar =  cSerializable::findCharBySerial(currchar[s]);
	if ( ! pc_currchar )
		return;

	strcpy((char*)nonuni, speech);
	strcpy((char*)tbuffer, (char*)nonuni);

	strupr((char*)nonuni);
	cline = (char*)&nonuni[0];
	
	stringVector tokens = splitline(nonuni);

	if ( tokens.size() < 1 )
		return;
	// Let's ignore the command prefix;
	comm = nonuni + 1;

	pCommand p_cmd= commands->findCommand((char*)comm);
	
	
	NXWCLIENT client= getClientFromSocket(s);

	if(p_cmd==NULL) {
		return;
	}
	
	
	//Control between cCommand privilege and cChar privilege.

	if(p_cmd->getCommandLevel(p_cmd)==pc_currchar->commandLevel){
	client->sysmsg("You can't use this command!");
		return;
	}

	
	cCallCommand* called= new cCallCommand(speech);


	uint32_t cmd_serial=called->addCallCommand(called);

	
	
	// Frodo:
	// NOW CALL AMX FUNCTION specified in cCommand.cmd_callback giving pc_currchar and 
	// cmd_serial
	
	
	
	//Let's delete the temp object		
			
	called->delCommand(cmd_serial);
	

}

/*******
Must complete a native function for AMX to get command property
for use it in small scripting.

*******/
const int CP_PARAM=0;
const int CP_ALLPARAMS=1;
const int CP_N_PARAMS=2;


//Frodo:	must add the following function in AMX_NATIVE_INFO nxw_API[] 
//			{ "getCmdProperty", _getCmdProperty } 
	
	// params[1] = cCallCommand Serial
	// params[2] = property
	// params[3] = number of the param 



NATIVE2(_getCmdProperty) {		

//	cCallCommand*

	if (cCallCommand->findCallCommand(param[1])==NULL)
		return NULL;
	  
		
/*	if ( !params[2] )
	{
		switch( params[2] ) {
			
			case CP_PARAM: {
				}
			case CP_ALLPARAMS: {
				}
			case CP_N_PARAMS: {
				}
		}

*/ 	//}
  	
	ErrOut("itm_getProperty called without a valid property !\n");
	return '\0';
	
}

	

