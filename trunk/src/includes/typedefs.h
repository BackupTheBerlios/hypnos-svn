/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file typedefs.h
\brief contains typedefs for integer values and common structures.
\todo complete the documentation of types
*/

#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__

typedef uint32_t			TIMERVAL;
typedef int32_t				ACCOUNT;
typedef int32_t				FUNCIDX;

typedef std::set<uint32_t> uint32_set;		//!< A set of uint32_t
typedef std::vector<uint32_t> uint32_vector;	//!< A vector of uint32_t

typedef class cObject *pObject;			//!< Pointer to a cObject
typedef class cTarget* pTarget;			//!< Pointer to a cTarget
typedef class cGMPage *pGMPage;			//!< Pointer to a GM Page
typedef class cMenu *pMenu;			//!< Pointer to a cMenu

typedef class cAccount *pAccount;		//!< Pointer to a cAccount
typedef class cClient *pClient;			//!< Pointer to a cClient
typedef class cSocket *pSocket;			//!< Pointer to a cSocket
typedef class cPacketSend *pPacketSend;		//!< Pointer to a cPacketSend

typedef class cItem *pItem;			//!< Pointer to a cItem
typedef class cBook *pBook;			//!< Pointer to a cBook
typedef class cWeapon *pWeapon;			//!< Pointer to a cWeapon
typedef class cContainer *pContainer;		//!< Pointer to a cContainer
typedef class cMsgBoard *pMsgBoard;		//!< Pointer to a cMsgBoard
typedef class cMap *pMap;			//!< Pointer to a cMap

typedef class cBody* pBody;			//!< Pointer to a cBody

typedef class cChar *pChar;			//!< Pointer to a cChar (character)
typedef class cPC *pPC;				//!< Pointer to a pPC (playing character)
typedef class cNPC *pNPC;			//!< Pointer to a pNPC (non playing character)

typedef std::list<pGMPage> GMPageList;		//!< List of GM Pages

typedef std::list<pItem> ItemList;		//!< List of items
typedef std::list<pChar> CharList;		//!< List of characters
typedef std::list<pClient> ClientList;		//!< List of connected clients

#endif //__TYPEDEFS_H__
