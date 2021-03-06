/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
#include "common_libs.h"
#include "sndpkg.h"
#include "inlines.h"
#include "logsystem.h"
#include "backend/scheduler.h"
#include "networking/network.h"
#include "networking/remadmin.h"

#ifdef ECHO
	#undef ECHO
#endif

#include <stdio.h>

//@{
/*!
\name Telnet Protocol
\brief Telnet protocol flags for define telnet works.
*/
static const uint8_t IAC 	= 0xFF; //!< all command start by this

static const uint8_t SE 	= 0xF0;
static const uint8_t SB		= 0xFA;
static const uint8_t GA		= 0xF9;
static const uint8_t WILL	= 0xFB;
static const uint8_t DO		= 0xFC;
static const uint8_t WONT	= 0xFD;
static const uint8_t DONT	= 0xFE;

static const uint8_t ECHO	= 0x01;
static const uint8_t TIMINGMARK	= 0x06;
static const uint8_t LINEMODE	= 0x22;
static const uint8_t SLC	= 0x03;
static const uint8_t SLC_FLUSHOUT = 0x20;
static const uint8_t SLC_FLUSHIN = 0x40;
//@}

RemoteAdmin::RemoteAdmin()
{
	debugSocket= -1;
	racSocket= -1;
}

RemoteAdmin::~RemoteAdmin()
{
	if (racSocket < 0 ) return;

	closesocket(racSocket);
	for (int i=0; i<racnow; i++)
		closesocket(sockets[i]);

}

/*!
\brief Initialize remote adm. console socket(s)
\author Xanatar
*/
void RemoteAdmin::Init()
{
	int bcode;
	outPlain( "Initializing remote administration server...");
	
	rac_port = ServerScp::g_nRacTCPPort;
	
	if (rac_port==0) 
	{
		outPlain("[DISABLED]\n");
		return;
	}
		
	racSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (racSocket < 0 )
	{
		outPlain("[FAIL]\nERROR: Unable to create RAC socket\n");
		return;
	}


	raclen_connection_addr=sizeof (struct sockaddr_in);
	racconnection.sin_family=AF_INET;
	racconnection.sin_addr.s_addr=INADDR_ANY;	
	racconnection.sin_port=(((rac_port%256)<<8))+(rac_port>>8); //Port

	bcode = bind(racSocket, (struct sockaddr *)&racconnection, raclen_connection_addr);

	if (bcode!=0)
	{
		#ifndef __unix__
			bcode = WSAGetLastError ();
		#endif
		outPlainf("[FAIL]\nERROR: Unable to bind RAC socket - Error code: %i\n",bcode);
		return;
	}
    
	bcode = listen(racSocket, MAXRACLIENT);

	if (bcode!=0)
	{
		outPlainf("[FAIL]\nERROR: Unable to set RAC socket in listen mode  - Error code: %i\n",bcode);
		return;
	}

	unsigned long nonzero = 1;

	ioctlsocket(racSocket,FIONBIO,&nonzero) ;

	outPlain("[ OK ]\n");
}


/*!
\brief Check periodically for new connections on rac socket
\author Xanatar
*/
void RemoteAdmin::CheckConn ()
{
	int s;
	socklen_t len;
	
	if ((rac_port==0) || (racnow>=MAXRACLIENT)) return;

	FD_ZERO(&conn);
	FD_SET(racSocket, &conn);
	nfds=racSocket+1;

	s=select(nfds, &conn, NULL, NULL, &nettimeout);	

	if (s<=0) return;
		
	outPlain("Connecting sockets...");
	len=sizeof (struct sockaddr_in);
	sockets[racnow] = accept(racSocket, (struct sockaddr *)&rac_sockets_addr, &len); 

	if ((sockets[racnow]<0)) {
		outPlain("[FAIL]\n");
		return;
	}
	
	if (Network->CheckForBlockedIP(client_addr))
	{
		outPlainf("[BLOCKED!] IP Address: %s\n",
		#ifdef HAVE_INET_NTOA
			inet_ntoa(rac_sockets_addr.sin_addr)
		#else
			long2ip(rac_sockets_addr.sin_addr).c_str()
		#endif
		);
		closesocket(sockets[racnow]);
		return;
	}

	outPlain("[ OK ]\n");

	status[racnow]=RACST_CHECK_USR;
	inputptrs[racnow] = 0;

	// disable local echo for client
	Printf("%c%c%c", IAC, WILL, ECHO);

	Printf("Hypnos %s [%s]\r\nRemote Administration Console\r\nProgrammed by: %s", strVersion, OS, strDevelopers);
	Printf("\r\nBased on NoX-Wizard 20031228");
	Printf("\r\nWeb-site : http://hypnos.berlios.de/\r\n");
	Printf(racnow, "\r\n");
	Printf(racnow, "INFO: character typed for login and password\r\n");
	Printf(racnow, "are not echoed, this is not a bug.\r\n");

	Printf(racnow, "\r\nLogin : ");
	racnow++;

}

/*!
\brief Check sockets for input
\author Xanatar
*/
void RemoteAdmin::CheckInp ()
{
	int s, i, oldnow;
	int lp, loops;     //Xan : rewritten to support more than 64 concurrent socketss

	if (rac_port==0) return;

	oldnow = racnow;
	loops = racnow / 64; //xan : we should do loops of 64 players
	
	for (lp = 0; lp <= loops; lp++) { 


		FD_ZERO(&all);
		FD_ZERO(&errsock);
		nfds=0;

		for (i=0+(64*lp);i<((lp<loops) ? 64 : oldnow);i++)
		{
			FD_SET(sockets[i],&all);
			FD_SET(sockets[i],&errsock);
			if (sockets[i]+1>nfds) nfds=sockets[i]+1;
		}
		
		s=select(nfds, &all, NULL, &errsock, &nettimeout);

		if (s>0)
		{
			for (i=0+(64*lp);i<((lp<loops) ? 64 : oldnow);i++)
			{
				if (FD_ISSET(sockets[i],&errsock))
					Disconnect(i);

				if ((FD_ISSET(sockets[i],&all))&&(oldnow==racnow))
					Rcv(i);
			}
		}
	}

}


/*!
\brief Disconnect rac client
\author Xanatar
\param s client Client to be disconnected
*/
void RemoteAdmin::Disconnect(int s)
{
	if (rac_port==0) return;

	closesocket(sockets[s]);

	//outPlain("RAC : disconnecting socket...[ OK ]\n");
	//if it was the last socket opened, just throw it away :)
	if (racnow==s+1) {
		racnow--;
		return;
	}

	//nope, it isn't the last.. so make it be the last!!

	int t = racnow-1;

	for (int i=0; i<MAXLENGHT; i++)
		inputbufs[s][i] = inputbufs[t][i];
	
	inputptrs[s] = inputptrs[t];
	sockets[s] = sockets[t];
	status[s] = status[t];
	
	racnow--;
}


/*!
\brief Receives data from remote client
\author Xanatar
\param s client Client to recieve data from
*/
void RemoteAdmin::Rcv (int s)
{
	unsigned char buff[1024];
	int n,i=0;

	if (rac_port==0) return;

	n = recv(sockets[s], (char *)buff,1000,0);

	if (n<=0) 
	{
		Disconnect(s);
		return;
	}

	// ignore commands sent by client
	if( buff[0]==IAC )
		return;
	
	i = 0;
	while (i<n)
	{
		if (status[s]==RACST_ACCESS_DENIED) { Disconnect(s); return; }

		if ((buff[i]=='\n')||(buff[i]=='\r')) { 
			if (ServerScp::g_nUseCharByCharMode) Printf(s,"\r\n");

			if (inputptrs[s]>0) {
				if (status[s]==RACST_STDIN) Printf(s,"\r\n");
				ProcessInput(s); 
				if (status[s]==RACST_STDIN) Printf(s,"\r\n");
			}

			if (status[s]==RACST_STDIN) Printf(s,"[%s]",serv[0][0]);

			break;
		}
		if (inputptrs[s]>MAXLENGHT-3) {
			i++;
			continue;
		}

		inputbufs[s][inputptrs[s]++]=buff[i];

		if (status[s]==RACST_ACCESS_DENIED) { Disconnect(s); return; }

		if (ServerScp::g_nUseCharByCharMode) {
			if (status[s]!=RACST_CHECK_PWD)
				Printf(s,"%c", inputbufs[s][inputptrs[s]-1]);
			else 
			{
				Printf(s,"*");
			}
		}
		i++;
	}

}

/*!
\brief Sends data as printf format
\author Xanatar
\param s socket
\param txt format string of text
\param ... printf parameters
*/
void RemoteAdmin::Printf(int s, char *txt, ...) // System message (In lower left corner)
{
	if (rac_port==0) return;
	if(s==-1) return;
	va_list argptr;
	char *msg;

	va_start( argptr, txt );
	vasprintf( &msg, txt, argptr );
	va_end( argptr );

	if (s==DBG) 
		send(debugSocket, msg, strlen(msg), 0);
	else 
		send(sockets[s], msg, strlen(msg), 0);
	free(msg);
}

/*!
\brief Processes input from socket s
\author Xanatar, Anthalir
\param s socket to process input
*/
void RemoteAdmin::ProcessInput(int s)
{
	char inp2[1024]= {'\0'};
	char *inp= inp2;
	uint32_t i;


	if (rac_port==0) return;

	if( !strncmp(&inputbufs[s][0], "!!", 2) )
	{
		strcpy(inp2, oldinp[s]);
	}
	else 
	{
		int j=0;
		for (int i=0; i<inputptrs[s]; i++)
		{
			if (inputbufs[s][i]!='\b')		// \b = bell
				inp2[j++] = inputbufs[s][i];
		}

		strncpy(oldinp[s], inp2, sizeof(oldinp[s])-1);
	}

	// remove spaces and tabulations at string start
	while( isspace(inp[0]) )
		inp++;

	if (status[s]==RACST_STDIN)
	{
		for (i=0; i< strlen(inp); i++)
		{
			if ((inp[i]>='a')&&(inp[i]<='z')) 
				inp[i] -= 'a'-'A';

			if (inp[i]==' ') break;
		}
	}
		
	inputptrs[s] = 0;

	if (status[s]==RACST_CHECK_USR)
	{
		strcpy(loginname[s], inp);
		Printf(s, "password : ");
		status[s]=RACST_CHECK_PWD;
		return;
	}

	if (status[s]==RACST_CHECK_PWD)
	{
		if( !Accounts->AuthenticateRAS(loginname[s], inp) ) 
		{
			Printf(s, "\r\nAccess Denied.\r\nPress any key to get disconnected...");
			outPlainf("Access Denied on Remote Console for user '%s' with pass '%s'\n", loginname[s], inp);
			status[s]=RACST_ACCESS_DENIED;
			return;
		} else {
			status[s]=RACST_STDIN;
			Printf(s,"\r\n\r\n");
			Printf(s,"------------------------------------------------------------------------\r\n");
			Printf(s,"Welcome to the administration console\r\n");
			Printf(s,"\r\nType HELP to receive help on commands.\r\n");
			Printf(s,"If you are not authorized to access this system\r\n");
			Printf(s,"please exit immediately by typing the EXIT command\r\n");
			Printf(s,"or by closing your client. Any other unauthorized\r\n");
			Printf(s,"action can be persecuted by law.\r\n");
			Printf(s,"\r\n");

			// re-enable local echo for client
			Printf(s, "%c%c%c", IAC, DO, ECHO);
			Printf(s, "%c%c%c", IAC, WONT, ECHO);

			outPlain("Authorised acces on Remote Console by user %s\n", loginname[s]);
			return;
		}
	}

	if (status[s]!=RACST_STDIN) return;

	if (!strcmp(inp,"WHO")) 
	{
			if(now==0) 
			{
				Printf(s,"There are no users connected.\r\n");
				return;
			}

			int i, j=0;
			Printf(s,"Current Users in the World:\r\n");
			for (i=0;i<now;i++)
			{
				pChar pc_i=cSerializable::findCharBySerial(currchar[i]);				
				if( pc_i && clientInfo[i]->ingame ) //Keeps NPC's from appearing on the list
				{
					j++;
					Printf(s, "    %i) %s [%08x]\r\n", j, pc_i->getCurrentName().c_str(), pc_i->getSerial());
				}
			}
			Printf(s, "Total Users Online: %d\r\n", j);
			Printf(s, "End of userlist\r\n");
			return;
	}

	if (!strcmp(inp,"QUIT") || !strcmp(inp, "EXIT")) 
	{ 
		Printf(s, "Bye %s ! :)\r\n\r\n", loginname[s]); 
		Disconnect(s); 
		return; 
	}

	if (!strcmp(inp,"HELP")) {
		Printf(s,"!! : repeats the last command\r\n");
		Printf(s,"WHO : displays logon players\r\n");
		Printf(s,"QUIT : close the connection\r\n");
		Printf(s,"EXIT : close the connection\r\n");
		Printf(s,"PDUMP : displays performance dump\r\n");
		Printf(s,"SAVE : saves the world\r\n");
		Printf(s,"SHUTDOWN : shuts the server\r\n");
		Printf(s,"ABORT : shuts the server WITHOUT saving\r\n");
		Printf(s,"BROADCAST, BC or '!' <msg> : broadcasts a message to everyone\r\n");
		Printf(s,"CFG <section>.<property>=<value> : sets a server.cfg setting dynamically\r\n");
		Printf(s,"ADDACCT <name>,<pwd> : creates a new account\r\n");
		Printf(s,"REMACCT <name> : remove an account\r\n");
		Printf(s,"CHANGEACCTPWD <name>,<pwd> : change an account password\r\n");
		Printf(s,"RELOADSCHED : reloads the schedules.xml file\r\n");
		Printf(s,"RELOADBLOCKS : reload hosts_deny.scp\r\n");

		return;
	}

	char tkn[1024];
	char *cmd = NULL, *par2 = NULL;
	

	for (i=0; i< strlen(inp); i++)
	{
		if (inp[i]==' ') 
		{
			tkn[i] = '\0';
			cmd = inp+i+1;
			break;
		} 
		else 
		{
			tkn[i]=inp[i];
		}
	}

	// before the splitting of arguments

	if (!strcmp(tkn,"CFG")) 
	{
		if ((cmd==NULL))
		{
			Printf(s,"Syntax is : CFG <section>.<property>=<value>\r\nExample : CFG SERVER.STATCAP=200\r\nSections and properties are the ones contained in server.scp\r\n");
			return;
		}
		int n=cfg_command(cmd);
		if (n!=0) 
			Printf(s, "Error number : %d\r\n", n);
		else 
			Printf(s, "OK.\r\n");

		return;
	}

	if (cmd!=NULL) {
		for (i=0; i< strlen(cmd); i++)
		{
			if ((cmd[i]==',')||(cmd[i]==' ')) 
			{
				par2 = cmd+i+1;
				cmd[i] = '\0';
				break;
			}
		}
	}

	if (!strcmp(inp,"SAVE")) 
	{
			if ( !cwmWorldState->Saving() )
			{
				Printf(s, "Saving worldfile...");
				cwmWorldState->saveNewWorld();
				saveserverscript();
				Printf(s, "[DONE]\r\n");
				Printf(s, "OK.\r\n");
			} 
			return;
	}

	if (!strcmp(inp,"SHUTDOWN")) {
			outPlain("!!!: Immediate Shutdown initialized from Remote Console!\n");
			Printf(s, "Bye! :)\r\n\r\n");
			keeprun=0;
			return;
	}

	if (!strcmp(inp,"ABORT")) 
	{
		exit(3);
		return;
	}

	if (!strcmp(inp,"RELOADSCHEDULER")) 
	{
		cScheduler::restart();
		return;
	}


	if (!strcmp(inp,"RELOADBLOCKS")) 
	{
		Network->LoadHosts_deny();
		Printf(s,"hosts_deny.scp reloaded.");
		return;
	}


	if ((!strcmp(tkn,"BROADCAST"))||(!strcmp(tkn,"BC"))||(!strcmp(tkn,"!"))) 
	{
		if ((cmd==NULL)) 
		{
			Printf(s,"Syntax is : BROADCAST <message>\r\nExample : BROADCAST Warning restarting server!\r\n");
			return;
		}

		if (par2!=NULL) 
			sysbroadcast("%s %s", cmd, par2);
		else 
			sysbroadcast("%s", cmd);

		Printf(s, "OK.\r\n");
		return;
	}

	if (!strcmp(tkn,"ADDACCT")) 
	{
		if ((cmd==NULL)||(par2==NULL)) 
		{
			Printf(s,"Syntax is : ADDACCT <name>,<password>\r\nExample : ADDACCT administrator,password\r\n");
			return;
		}
		int acc = Accounts->CreateAccount(cmd, par2);
		if ( acc != INVALID )
		{
			Printf(s, "Account %d created\r\n  Name : %s\r\n  Pass : %s\r\n", acc,cmd,par2);
			Printf(s, "[  OK  ] \r\n");
		}
		else
		{
			Printf(s, "Account %s not created. Username existant.\r\n", cmd);
			Printf(s, "[ERRORS]\r\n");
		}
		return;
	}
	
	if ( !strcmp(tkn,"REMACCT") )
	{
		if ((cmd==NULL))
		{
			Printf(s,"Syntax is : REMACCT <name>\r\nExample : REMACCT administrator\r\n");
			return;
		}
		if ( Accounts->RemoveAccount(cmd) )
		{
			Printf(s, "Account %s deleted\r\n", cmd);
			Printf(s, "[  OK  ]\r\n");
		}
		else
		{
			Printf(s, "Account %s not deleted\r\n", cmd);
			Printf(s, "[ERRORS]\r\n");
		}
		return;
	}
	
	if ( !strcmp(tkn,"CHANGEACCTPWD") )
	{
		if ((cmd==NULL)||(par2==NULL))
		{
			Printf(s,"Syntax is: CHANGEACCTPWD <name>,<password>\r\nExample : CHANGEACCTPWD administrator,newpassword\r\n");
			return;
		}
		ACCOUNT acc = Accounts->GetAccountByName(cmd);
		if ( acc != INVALID )
		{
			Accounts->ChangePassword(acc, par2);
			Printf(s, "Account %d has new password\r\n  Password : %s\r\n", acc, par2);
			Printf(s, "[  OK  ] \r\n");
		}
		else
		{
			Printf(s, "Password of account %s not changed. Username not existant.\r\n", cmd);
			Printf(s, "[ERRORS]\r\n");
		}
		return;
	}

	Printf(s,"Unknown command entered.\r\n");
}
