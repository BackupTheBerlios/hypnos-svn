/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#ifndef __PACKETS_SENT_H__
#define __PACKETS_SENT_H__

#include "common_libs.h"
#include "enums.h"
#include "structs.h"
#include "speech.h"

/*!
\author Flameeyes
\brief Packet to be sent
*/
class cPacketSend
{
protected:
	uint8_t *buffer;	//!< Pointer to the buffer
	uint16_t length;	//!< Length of the buffer

	cPacketSend(uint8_t *aBuffer, uint16_t aLenght)
	{ buffer = aBuffer; length = aLenght; }

public:
	inline virtual ~cPacketSend()
	{ delete[] buffer; }

	//! Prepare the buffer to be sent
	virtual void prepare() = 0;

	//! Prepare the packet if not prepared and return the buffer
	inline const uint8_t *getBuffer()
	{
		if ( ! length )
			prepare();

		return buffer;
	}

	//! Prepare the packet if not prepared and return the length of the buffer
	inline const uint16_t getLength()
	{
		if ( ! length )
			prepare();

		return length;
	}

	virtual void fixForClient(ClientType ct)
	{ }
};

namespace nPackets {
	namespace Sent {	
		/*!
		\brief Status window
		\author Flameeyes
		\note packet 0x11
		*/
		class Status : public cPacketSend
		{
		protected:
			pChar pc;	//!< Character
			uint8_t type;	//!< Type
			bool canrename;	//!< Can the characeter be renamed?
		public:
			/*!
			\param p Character
			\param t type of window
			\param r client who receives this packet can rename char p
			*/
			inline Status(pChar p, uint8_t t, bool r) :
				cPacketSend(NULL, 0), pc(p), type(t), canrename(r)
			{ }
		
			void prepare();
		};
		
		
		/*!
		\brief Sends to client info on item on the ground
		\author Chronodt
		\note packet 0x1a
		*/
		
		
		class ObjectInformation : public cPacketSend
		{
		protected:
			pItem pi;	//!< Item to send
			pPC pc;		//!< Player who sees the item
		public:
			/*!
			\param i item
			\param p pc who sees the item
			*/
			inline ObjectInformation(pItem i, pPC p) :
				cPacketSend(NULL, 0), pi(i), pc(p)
			{ }
			void prepare();
		};
		
		
		/*!
		\brief lsd - alters item already shown to client. Real item position & color are not altered, just the shown ones
		\author Chronodt
		\note packet 0x1a
		*/
		class LSDObject : public cPacketSend
		{
		protected:
			pItem pi;		//!< Item
			pPC pc;			//!< Player who sees the item
			uint16_t color;		//!< new color of item
			sLocation position;	//!< new position of item
		public:
			/*!
			\param i item
			\param p pc who sees the item
			\param c new color of item
			\param pos new position of item
			*/
			inline LSDObject(pItem i, pPC p, uint16_t c, sLocation pos) :
				cPacketSend(NULL, 0), pi(i), pc(p), color(c), position(pos)
			{ }
			void prepare();
		};
		
		/*!
		\brief Char location and body type (Login confirmation)
		\author Chronodt
		\note packet 0x1b
		*/
		
		
		class LoginConfirmation : public cPacketSend
		{
		protected:
			pPC pc;			//!< Player who is logging in
		public:
			/*!
			\param p pc who is logging in
			*/
			inline LoginConfirmation(pPC p) :
				cPacketSend(NULL, 0), pc(p)
			{ }
			void prepare();
		};
		
		/*!
		\brief Send character speech (not unicode) to listener
		\author Chronodt
		\note packet 0x1c
		*/
		
		class Speech : public cPacketSend
		{
		protected:
			cSpeech speech;				//!< what ps is saying
			bool ghost;				//!< "ghostize" message (OOOoOOOoOOO :) )
		public:
			/*!
			\param s what is being told
			\param p talker. NOTE: if p is NULL, it is considered a system message
			\param ghostize speech has to be mutated to ghost speech
			*/
			inline Speech(cSpeech &s, bool ghostize = false) :
				cPacketSend(NULL, 0), speech(s), ghost(ghostize)
			{ }
			void prepare();
		};
		
		
		/*!
		\brief Delete object
		\author Flameeyes & Chronodt
		\note packet 0x1d
		*/
		class DeleteObj : public cPacketSend
		{
		protected:
			pSerializable pser;
		public:
			/*!
			\param s serializable object to remove
			*/
			inline DeleteObj(pSerializable s) :
				cPacketSend(NULL, 0), pser(s)
			{ }
		
			void prepare();
		};
		
		/*!
		\brief Draws game player (used only for client-played char)
		\author Chronodt
		\note packet 0x20
		*/
		
		class DrawGamePlayer : public cPacketSend
		{
		protected:
			pPC pc;		//! Current player
		public:
			/*!
			\param player current player
			*/
			inline DrawGamePlayer(pPC player) :
				cPacketSend(NULL, 0), pc(player)
			{ }
		
			void prepare();
		};
		
		/*!
		\brief Move reject
		\author Chronodt
		\note packet 0x21
		*/
		
		class MoveReject : public cPacketSend
		{
		protected:
			pPC pc;			//! Current player
			uint8_t sequence;       //! Sequence number of move rejected
		public:
			/*!
			\param player current player
			\param seq Sequence number to reject
			*/
			inline MoveReject(pPC player, uint8_t seq) :
				cPacketSend(NULL, 0), pc(player), sequence(seq)
			{ }
		
			void prepare();
		};
		
		/*!
		\brief Move accepted
		\author Chronodt
		\note packet 0x22
		*/
		
		class MoveAcknowdledge : public cPacketSend
		{
		protected:
			uint8_t sequence;	//! walk sequence number
			uint8_t notoriety;	//! could be how the player has to be seen (blue innocent, grey criminal, etc) not sure about it
		public:
			/*!
			\param s sequence #
			\param n notoriety (????)
			*/
		
			inline MoveAcknowdledge(uint8_t s, uint8_t n) :
				cPacketSend(NULL, 0), sequence(s), notoriety(n)
			{ }
		
			void prepare();
		};
		
		/*!
		\brief Item dragging
		\author Chronodt
		\note packet 0x23
		*/
		
		class DragItem : public cPacketSend
		{
		protected:
			pItem item; 		//!< dragged item
			sLocation destination; 	//!< where the item is dragged to
			uint16_t amount;	//!< how many items in stack
		public:
			inline DragItem(pItem aItem, sLocation aDestination, uint16_t aAmount) :
				cPacketSend(NULL, 0), item(aItem), destination(aDestination), amount(aAmount)
			{ }

			void prepare();
		};

                /*!
		\brief Open gump
		\author Chronodt
		\note packet 0x24
		*/
		class OpenGump : public cPacketSend
		{
		protected:
			uint32_t serial; 		//!< serial of gump or gump-related item/char
			uint16_t gump;			//!< gump id
		public:
			inline OpenGump(uint32_t aSerial, uint16_t aGump) :
				cPacketSend(NULL, 0), serial(aSerial), gump(aGump)
			{ }

			void prepare();
		};

		/*!
		\brief tells to the client the serial and all other data about an item in the given container. usually preceded by another package
		\author Flameeyes & Chronodt
		\note packet 0x25

		\note packet has to be sent AFTER the real moving of the item. This only tells the client where it has gone to
		*/

		class ShowItemInContainer : public cPacketSend
		{
		protected:
			pItem item;	//!< Item to add
		public:
			/*!
			\param aItem item to add
			*/
			inline ShowItemInContainer(pItem aItem) :
				cPacketSend(NULL, 0), item(aItem)
			{ }

			void prepare();
		};

		/*!
		\brief Kick client
		\author Chronodt
		\note packet 0x26

		\todo use this packet where it is needed
		*/

		class Kick : public cPacketSend
		{
		protected:
			pPC kicker;	//!< gm who performs the kick (not sure if really needed)
		public:
			/*!
			\param aKicker gm who kick this client'pg
			*/
			inline Kick(pPC aKicker) :
				cPacketSend(NULL, 0), kicker(aKicker)
			{ }

			void prepare();
		};


		/*!
		\brief Bounce item
		\author Chronodt
		\note packet 0x27
		*/

		class BounceItem : public cPacketSend
		{
		protected:
			uint8_t mode; 		//!< I dont' really know. Sometimes you send a 5, sometimes a 0....
		public:
			inline BounceItem(uint8_t aMode = 0) :
				cPacketSend(NULL, 0), mode(aMode)
			{ }

			void prepare();
		};



		/*!
		\brief Clear Square
		\author Chronodt
		\note packet 0x28
		*/
		class ClearSquare : public cPacketSend
		{
		protected:
		uint16_t x;	//!< x coordinate
		uint16_t y;	//!< y coordinate
		public:
			inline ClearSquare(uint16_t aX, uint16_t aY) :
				cPacketSend(NULL, 0), x(aX), y(aY)
			{ }

			void prepare();
		};

		/*!
		\brief Packet to warn client that something has been added to pc's clothing
		\author Chronodt
		\note packet 0x29
		*/

		class PaperdollClothingUpdated : public cPacketSend
		{
		public:
			inline PaperdollClothingUpdated() :
				cPacketSend(NULL, 0)
			{ }

			void prepare();
		};

		/*!
		\brief Mobile attributes
		\author Chronodt
		\note packet 0x2d
		*/
		class MobileAttributes : public cPacketSend
		{
		protected:
		pChar chr;		//!< character
		public:
			inline MobileAttributes(pChar aChr) :
				cPacketSend(NULL, 0), chr(aChr)
			{ }

			void prepare();
		};

		/*!
		\brief Adds item to client's paperdoll
		\author Chronodt
		\note packet 0x2e
		*/

		class WornItem : public cPacketSend
		{
		protected:
			pEquippable item;
		public:
			inline WornItem(pEquippable aItem) :
				cPacketSend(NULL, 0), item(aItem)
			{ }

			void prepare();
		};

		/*!
		\brief There is a fight going on somewhere on screen
		\author Chronodt
		\note 0x2F
		*/
		class FightOnScreen : public cPacketSend
		{
		protected:
			pChar attacker;		//!< attacking character
			pChar defender;		//!< attacked character

		public:
			inline FightOnScreen(pChar aAttacker, pChar aDefender) :
				cPacketSend(NULL, 0), attacker(aAttacker), defender(aDefender)
			{ }

			void prepare();
		};


		/*!
		\brief Pause/resume client [Packet 0x33]
		\author Chronodt
		
		This packet is sent by the functions cClient::pause() and
		cClient::resume() which contains two static copies of this.
		
		Don't use this directly, but call the above methods
		*/
		class PauseClient : public cPacketSend
		{
		protected:
			uint8_t pausestatus;	//!< 0 = pause, 1 = resume

		public:
			/*!
			\brief Constructor
			\param aPauseStatus value to set pausestatus to. 0 for pause, 1 for resume
			\note The constructor also calls prepare() because the packet doesn't
				depends on other things other than that.
			*/
			inline PauseClient(uint8_t aPauseStatus) :
				cPacketSend(NULL, 0), pausestatus(aPauseStatus)
			{ prepare(); }

			void prepare();
		};

		/*!
		\brief pathfinding
		\author Chronodt
		\note packet 0x38

		\note this packet should be sent at least 19 times consecutively to have any effect (unverified)
		*/

		class Pathfinding : public cPacketSend
		{
		protected:
			sLocation loc;		//!< destination
		public:
			inline Pathfinding(sLocation aLoc) :
				cPacketSend(NULL, 0), loc(aLoc)
			{ }

			void prepare();
		};


		/*!
		\brief Send skill list or updates a single skill
		\author Chronodt
		\note packet 0x3a
		*/

		class SendSkills : public cPacketSend
		{
		protected:
			pChar pc;
			uint8_t skill;
		public:
			/*!
			\param aPc Character to send the skill of
			\param aSkill skill to send. if INVALID (or omitted) sends all the skills
			*/
			inline SendSkills(pChar aPc, uint8_t aSkill = UINVALID16) :
				cPacketSend(NULL, 0), pc(aPc), skill(aSkill)
			{ }

			void prepare();
		};

		/*!
		\brief empties buy window (i.e. just like pressing the clear button on client, but this is from server side)
		\author Chronodt
		\note packet 0x3b
		*/
		class ClearBuyWindow : public cPacketSend
		{
		protected:
			const pNPC npc;	        //!< Vendor

		public:
			/*!
			\param n npc vendor
			*/
			inline ClearBuyWindow(pNPC n) :
				cPacketSend(NULL, 0), npc(n)
			{ }
			void prepare();
		};


		/*!
		\brief tells client which items are in the given container
		\author Flameeyes
		\note packet 0x3c
		*/
		class ContainerItem : public cPacketSend
		{
		protected:
			std::list<sContainerItem> items;
		public:
			inline ContainerItem() :
				cPacketSend(NULL, 0)
			{ }

			/*!
			\brief add an item to the list of items in the container
			\param item item to add to the container
			*/
			inline void addItem(pItem item)
			{ items.push_back(sContainerItem(item)); }

			void prepare();
		};

		/*!
		\brief tells client which items are in the given container (msgboard version)
		\author Flameeyes & Chronodt
		\note packet 0x3c

		\note This functions holds and releases the cMsgBoard::globalMutex and cMsgBloard::boardMutex mutexes
		*/
		class MsgBoardItemsinContainer : public cPacketSend
		{
		protected:
		
			const pMsgBoard msgboard;
		
		public:
			/*!
			\note constructor for msgboards
			\param aMsgboard msgboard used. This is called only on msgboard first opening
			*/
			inline MsgBoardItemsinContainer(pMsgBoard aMsgboard) :
				cPacketSend(NULL, 0), msgboard (aMsgboard)
			{ }
		
			void prepare();
		};


		/*!
		\brief Personal Light Level
		\author Chronodt
		\note packet 0x4e
		*/
		class PersonalLight : public cPacketSend
		{
		protected:
			pChar pc;	//!< Character
			uint8_t light;	//!< light level
		public:
			/*!
			\param aPc Character to send the light level of
			\param aLight light level ( 0x00=day, 0x09=OSI night, 0x1F=Black )
			*/
			inline PersonalLight(pChar aPc, uint8_t aLight) :
				cPacketSend(NULL, 0), pc(aPc), light(aLight)
			{ }

			void prepare();
		};

		/*!
		\brief Overall Light Level
		\author Flameeyes
		\note packet 0x4f
		*/
		class OverallLight : public cPacketSend
		{
		protected:
			uint8_t level;	//!< Light level to send
		public:
			/*!
			\param l Light level
			*/
			inline OverallLight(uint8_t l) :
				cPacketSend(NULL, 0), level(l)
			{ }
		
			void prepare();
		};

		/*!
		\brief Idle warning
		\author Chronodt
		\note packet 0x53
		*/

		class IdleWarning : public cPacketSend
		{
		protected:
		/* type is:

			1=no character
			2=char exists
			3=can't connect
			4=can't connect
			5=character already in world
			6=login problem
			7=idle
			8=can't connect
		*/
			uint8_t type;
		public:
			/*!
			\param aType Warning type
			*/
			inline IdleWarning(uint8_t aType) :
				cPacketSend(NULL, 0), type(aType)
			{ }

			void prepare();
		};

		/*!
		\brief Sound effect
		\author Flameeyes
		\note packet 0x54
		*/

		class SoundFX : public cPacketSend
		{
		protected:
			uint16_t model;	//!< Sound model
			sLocation loc;	//!< sLocation where the sound is played
			bool rep;	//!< If true, the sound will be repeated
		public:
			/*!
			\param m sound model
			\param l where the sound will be played
			\param r should the sound be repeated?
			*/
			inline SoundFX(uint16_t m, sLocation l, bool r) :
				cPacketSend(NULL, 0), model(m), loc(l), rep(r)
			{ }

			void prepare();
		};

		/*!
		\brief StartGame
		\author Chronodt
		\note packet 0x55
		*/

		class StartGame : public cPacketSend
		{
		protected:
		public:
			inline StartGame() :
				cPacketSend(NULL, 0)
			{ }

			void prepare();
		};

		/*!
		\brief Sets/resets pins in a map
		\author Chronodt
		\note packet 0x56
		*/

		class MapPlotCourse : public cPacketSend
		{
		protected:
			const pMap map;
			const PlotCourseCommands command;
			
			/*!
			if command is pccWriteableStatus, it is plotting state (1=on, 0=off)
			if command is pccInsertPin, it is the pin number that the new pin must have (all others must be "pushed onward")
			
			Apparently you can have no more than 50 pins in a map, and it appears a client side limitation (and it appears really messy if you put them all :D)
			*/
			const uint8_t pin;
			const uint16_t x;
			const uint16_t y;
		public:
			/*!
			\param aMap map used
			\param aCommand command to be sent to client
			\param aPin pin modified (or writeability status if command == 6)
			\param xx x position of pin (map relative, pixels from topleft corner)
			\param yy y position of pin (map relative, pixels from topleft corner)
			*/
			inline MapPlotCourse(pMap aMap, PlotCourseCommands aCommand, uint8_t aPin = 0, uint16_t xx = 0, uint16_t yy = 0) :
				cPacketSend(NULL, 0), map (aMap), command (aCommand), pin (aPin),  x (xx), y (yy)
			{ }

			void prepare();
		};
		/*!
		\brief Local time of server (or game time?)
		\author Chronodt
		\note packet 0x5b
		*/

		class GameTime : public cPacketSend
		{
		protected:
		public:
			inline GameTime() :
				cPacketSend(NULL, 0)
			{ }
			void prepare();
		};

		/*!
		\brief Send Weather
		\author Chronodt
		\note packet 0x65

		\remarks maximum number of weather effects on screen is 70
		\remarks If it is raining, you can add snow by setting the num
		to the num of rain currently going, plus the number of snow you want
		\attention Weather messages are only displayed when weather starts
		\note Weather will end automatically after 6 minutes without any weather change packets
		\remarks You can totally end weather (to display a new message) by teleporting

		weather type:
		0x00="It starts to rain"
		0x01="A fierce storm approaches."
		0x02="It begins to snow"
		0x03="A storm is brewing.",
		0xFF=None (turns off sound effects),
		0xFE=(no effect?? Set temperature?)
		0x78,0x20,0x4F,0x4E = reset, never tested
		*/
		class Weather : public cPacketSend
		{
		protected:
			uint8_t weather;	//!< weather effect (0x00 - rain, 0x02- snow, 0xff - nothing)
			uint8_t intensity;	//!< number of simultaneous weather effects on screen
		public:
			inline Weather(uint8_t aWeather, uint8_t aIntensity) :
				cPacketSend(NULL, 0), weather(aWeather), intensity(aIntensity)
			{ }
			void prepare();
		};

		/*!
		\brief Send Book Page (Read-Write book version)
		\author Flameeyes
		\note packet 0x66

		\note Sends the pages of a writable book
		*/
		class BookPagesReadWrite : public cPacketSend
		{
		protected:
			pBook book;
			
			void preparePagesReadOnly();
			void preparePagesReadWrite();
		public:
			inline BookPagesReadWrite(pBook abook) :
				cPacketSend(NULL, 0), book(abook)
			{ }
		
			void prepare();
		};

		/*!
		\brief Send Book Page (Read-only book version)
		\author Flameeyes
		\note packet 0x66

		Sends the requested page of a readonly book
		*/
		class BookPageReadOnly : public cPacketSend
		{
		protected:
			pBook book;
			uint16_t p;	//!< Index of the page to send
			
			void preparePagesReadOnly();
			void preparePagesReadWrite();
		public:
			inline BookPageReadOnly(pBook abook, uint16_t page) :
				cPacketSend(NULL, 0), book(abook), p(page)
			{ }
		
			void prepare();
		};


		/*!
		\brief Send targeting cursor to client
		\author Chronodt
		\note packet 0x6C

		\todo targeting has to be remade almost completely -_-
		*/
		class TargetingCursor : public cPacketSend
		{
		protected:
			uint8_t type;		//!< 0 = creature/item target, 1 = xyz(location) target
			uint32_t cursorid;	//!< I SUPPOSE it is something related to the animation id of the targeting cursor
		public:
			inline TargetingCursor(uint8_t aType, uint32_t aCursorId) :
				cPacketSend(NULL, 0), type(aType), cursorid(aCursorId)
			{ }
			void prepare();
		};

		/*!
		\brief Send music code to client
		\author Flameeyes
		\note packet 0x6D
		*/
		class PlayMidi : public cPacketSend
		{
		protected:
			uint16_t id;	//!< ID of the MIDI to play
		public:
			/*!
			\param midi Midi file id
			*/
			inline PlayMidi(uint16_t midi) :
				cPacketSend(NULL, 0), id(midi)
			{ }
			void prepare();
		};

		/*!
		\brief Send action code to client (character performs the requested action)
		\author Flameeyes
		\note packet 0x6E
		\note cChar::action() packet
		*/
		class Action : public cPacketSend
		{
		protected:
			const pChar pc;	//!< char who performs the action
			const uint16_t action;	//!< id of the action to execute

		public:
			/*!
			\param aChr char who performs the action
			\param a id of the action to execute
			*/
			inline Action(pChar aChr, uint16_t a) :
				cPacketSend(NULL, 0), pc(aChr), action(a)
			{ }
			void prepare();
		};


		/*!
		\brief Secure trade commands
		\author Chronodt
		\note packet 0x6f
		*/
		class SecureTrading : public cPacketSend
		{
		protected:
		
			const uint8_t action;		//!< 0 = initiate, 1 = close window (end trade), 2 = change status (the button)
			const pPC tradePartner;		//!< trade partner of receiving client
			const uint32_t id1;		//!< if trade opening, must contains serial of trade container for this client, else its tradestatus (the button status: 1 checked 0 unchecked)
			const uint32_t id2;		//!< if trade opening, must contains serial of trade container for partner client, else its tradestatus (the button status: 1 checked 0 unchecked)

		public:
			inline SecureTrading(uint8_t aAction, pPC aTradePartner, uint32_t aId1, uint32_t aId2) :
				cPacketSend(NULL, 0), action(aAction), tradePartner(aTradePartner), id1(aId1), id2(aId2)
			{ }
		
			void prepare();
		};

		/*!
		\brief Sends a visual effect
		\author Chronodt
		\note packet 0x70
		*/
		class GraphicalEffect : public cPacketSend
		{
		protected:

			EffectType type;		//!< movement type
			pSerializable src, dst;		//!< source and destination of effect
			uint16_t effect;		//!< visual id of effect
			sLocation src_pos, dst_pos;	//!< souce and destination position of effect
			uint8_t speed;			//!< speed of effect (SPPED OF ANIMATION)
			uint8_t duration;		//!< travel speed of effect (update: maybe number of loops of the effects: should investigate)
			bool fixeddir;  		//!< if true animation direction does not change during effect
			bool explode;			//!< explosion once effect reaches target


		public:
			//! object to object constructor
			inline GraphicalEffect(EffectType aType, pSerializable aSrc, pSerializable aDst, uint16_t aEffect, uint8_t aSpeed, uint8_t aDuration, bool aFixedDir, bool aExplode) :
				cPacketSend(NULL, 0), type(aType), src(aSrc), dst(aDst), effect(aEffect), speed(aSpeed), duration(aDuration), fixeddir(aFixedDir), explode(aExplode)
			{
				src_pos = src->getWorldPosition();
				dst_pos = (dst) ? dst->getWorldPosition() : sLocation(0,0,0);
			}
			//! object to position constructor
			inline GraphicalEffect(EffectType aType, pSerializable aSrc, sLocation aDst_pos, uint16_t aEffect, uint8_t aSpeed, uint8_t aDuration, bool aFixedDir, bool aExplode) :
				cPacketSend(NULL, 0), type(aType), src(aSrc), dst(NULL), dst_pos(aDst_pos), effect(aEffect), speed(aSpeed), duration(aDuration), fixeddir(aFixedDir), explode(aExplode)
			{
				src_pos = src->getWorldPosition();
			}
			//! position to object constructor
			inline GraphicalEffect(EffectType aType, sLocation aSrc_pos, pSerializable aDst, uint16_t aEffect, uint8_t aSpeed, uint8_t aDuration, bool aFixedDir, bool aExplode) :
				cPacketSend(NULL, 0), type(aType), src(NULL), dst(aDst), src_pos(aSrc_pos), effect(aEffect), speed(aSpeed), duration(aDuration), fixeddir(aFixedDir), explode(aExplode)
			{
				dst_pos = dst->getWorldPosition();
			}
			//! position to position constructor
			inline GraphicalEffect(EffectType aType, sLocation aSrc_pos, sLocation aDst_pos, uint16_t aEffect, uint8_t aSpeed, uint8_t aDuration, bool aFixedDir, bool aExplode) :
				cPacketSend(NULL, 0), type(aType), src(NULL), dst(NULL), src_pos(aSrc_pos), dst_pos(aDst_pos), effect(aEffect), speed(aSpeed), duration(aDuration), fixeddir(aFixedDir), explode(aExplode)
			{ }
		
			void prepare();
		};


		/*!
		\brief Sends bulletin boards commands [packet 0x71]
		\author Chronodt

		Server-side Bulletin board commands
		*/

		enum BBoardCommands {bbcDisplayBBoard, bbcSendMessageSummary, bbcSendMessageBody};
		class BBoardCommand : public cPacketSend
		{
		protected:
		
			const pMsgBoard msgboard;
			const BBoardCommands command;
			const pMessage message;
		
		public:
			/*!
			\param m msgboard used
			\param com command for the msgboard
			\param mess message to be sent. May be omitted if command is DisplayBBoard
			*/
			inline BBoardCommand(pMsgBoard m, BBoardCommands com, pMessage mess = NULL) :
				cPacketSend(NULL, 0), msgboard (m), command(com), message(mess)
			{ }
		
			void prepare();
		};

		/*!
		\brief Sends war mode actual status to client [packet 0x72]
		\author Chronodt
		*/

		class WarModeStatus : public cPacketSend
		{
		protected:
		
			uint8_t buf[5];
		
		public:
			inline WarModeStatus(uint8_t* buffer) :
				cPacketSend(NULL, 0)
			{ memcpy(buf, buffer, 5);}

			void prepare();
		};

		/*!
		\brief Ping reply [packet 0x73]
		\author Chronodt

		Reply to client ping
		*/
		class PingReply : public cPacketSend
		{
		protected:
			uint8_t buf[2];
		public:
			inline PingReply(uint8_t* buffer) :
				cPacketSend(NULL, 0)
			{ memcpy(buf, buffer, 2);}

			void prepare();
		};

		/*!
		\brief Open Buy Window [packet 0x74]
		\author Chronodt
		*/
		class BuyWindow : public cPacketSend
		{
		protected:
			pEquippableContainer container;
		public:
			inline BuyWindow(uint8_t* buffer) :
				cPacketSend(NULL, 0)
			{ }
			void prepare();
		};


		/*!
		\brief Updates player [packet 0x77]
		\author Chronodt

		Sent whenever player is modified
		*/

		class UpdatePlayer : public cPacketSend
		{
		protected:
			pChar chr;
			uint8_t dir, flag, hi_color;
			uint32_t id1,id2,id3;

		public:
			inline UpdatePlayer(pChar pc, uint8_t newdir, uint8_t newflag, uint8_t newhi_color) :
				cPacketSend(NULL, 0), chr(pc), dir(newdir), flag(newflag), hi_color(newhi_color)
			{ }

			void prepare();
		};

		/*!
		\brief Draws a player [packet 0x78]
		\author Chronodt
		\note this packet must be personalized for each client who receives it, since it calculates guild status between receiver and drawn char
		*/

		class DrawObject : public cPacketSend
		{
		protected:
			pClient client;		//!< receiver of packet
			pChar pc;		//!< player to be drawn to client
			bool usedispz;		//!< use dispz instead of z as z-coordinate
		public:
			inline DrawObject(pClient aClient, pChar aPc, bool aUseDispZ) :
				cPacketSend(NULL, 0), client(aClient), pc(aPc), usedispz(aUseDispZ)
			{ }
			void prepare();
		};

		/*!
		\brief Opens dialog box [packet 0x7c]
		\author Chronodt
		\todo this function

		This packet sends a dialog to the client. No checkbox, id or anything except buttons and text and their respective return code is sent
		Useful for menus and (obiously) dialog box where multiple choices (or even a single ok button) have to be selected

		This is the first of the "open gump" packets (the other ones are the 0xab and the 0xb0 packets)
		*/
		class OpenDialogBox : public cPacketSend
		{
		protected:
		public:
			inline OpenDialogBox() :		//!< \todo finish this function
				cPacketSend(NULL, 0)
			{ }
			void prepare();
		};


		/*!
		\brief Login Denied [packet 0x82]
		\author Kheru

		0x00 = unknown user
		0x01 = account already in use
		0x02 = account disabled
		0x03 = password bad
		0x04 and higher = communications failed 
		*/

		class LoginDenied : public cPacketSend
		{
		protected:
			uint8_t reason;

		public:
			inline LoginDenied(uint8_t aReason) :
				cPacketSend(NULL, 0), reason(aReason)
			{ }

			void prepare();
		};


		/*!
		\brief Char deletion error [packet 0x85]
		\author Chronodt

		reason:

		0x00 => That character password is invalid.
		0x01 => That character doesn't exist.
		0x02 => That character is being played right now.
		0x03 => That charater is not old enough to delete.
			The character must be 7 days old before it can be deleted.
		0x04 => That character is currently queued for backup and cannot be
			deleted.
		0x05 => Couldn't carry out your request.
		*/
		class CharDeleteError : public cPacketSend
		{
		protected:
			uint8_t reason;
		public:
			inline CharDeleteError(uint8_t r) :
				cPacketSend(NULL, 0), reason(r)
			{ }
			void prepare();
		};


		/*!
		\brief Resends character list to client after a succesful deletion [packet 0x86]
		\author Chronodt
		*/
		class CharAfterDelete : public cPacketSend
		{
		protected:
			pAccount account;
		public:
			inline CharAfterDelete(pAccount aAccount) :
				cPacketSend(NULL, 0), account(aAccount)
			{ }
			void prepare();
		};

		/*!
		\brief Opens Paperdoll [packet 0x88]
		\author Chronodt
		*/
		class OpenPaperdoll : public cPacketSend
		{
		protected:
			pChar pc;
		public:
			inline CharAfterDelete(pChar aPc) :
				cPacketSend(NULL, 0), pc(aPc)
			{ }
			void prepare();
		};

		/*!
		\brief Corpse Clothing [packet 0x89]
		\author Chronodt

		dressing of corpse with ex-equipped item
		*/
		class CorpseClothing : public cPacketSend
		{
		protected:
			std::slist<pEquippable> items;
			pContainer corpse;
		public:
			inline CorpseClothing(pContainer aCorpse) :
				cPacketSend(NULL, 0), corpse(aCorpse)
			{ }
			inline void addItem(pEquippable item)
			{ items.push_back(item); }

			void prepare();
		};

		/*!
		\brief Connect to game server [packet 0x8c]
		\author Chronodt
		*/
		class ConnectToGameServer : public cPacketSend
		{
		protected:
			uint32_t ip;		//!< ip of game server
			uint16_t port;		//!< port of game server
			uint32_t newkey;	//!< new crypt key to use
		public:
			inline ConnectToGameServer(uint32_t aIp, uint16_t sPort, uint32_t aNewkey) :
				cPacketSend(NULL, 0), ip(aIp), port(aPort), newkey(aNewKey)
			{ }
			void prepare();
		};

		/*!
		\brief Opens map gump on client [packet 0x90]
		\author Chronodt
		\todo use something better than the mores for map placement data
		*/
		class OpenMapGump : public cPacketSend
		{
		protected:
			const pMap map;		//!< Map's gump
		public:
			/*!
			\param m map
			*/
			inline OpenMapGump(pMap m) :
				cPacketSend(NULL, 0), map (m)
			{ }
			void prepare();
		};

		/*!
		\brief Sends book header data [packet 0x93]
		\author Flameeyes
		*/
		class BookHeader : public cPacketSend
		{
		protected:
			pBook book;	//!< Book to send header of
			bool readonly;	//!< Is the book read only?
		public:
			inline BookHeader(pBook abook, bool ro) :
				cPacketSend(NULL, 0), book(abook), readonly(ro)
			{ }

			void prepare();
		};

		/*!
		\brief Sends Dye window [packet 0x95]
		\author Chronodt
		*/
		class DyeWindow : public cPacketSend
		{
		protected:
			pSerializable object;	//!< Item/char to dye
		public:
			inline DyeWindow(pSerializable aObject) :
				cPacketSend(NULL, 0), object(aObject)
			{ }
			void prepare();
		};

		/*!
		\brief Sends Move player [packet 0x97]
		\author Chronodt

		\note this packet seems to be unused by old nox O_o
		*/
		class MovePlayer : public cPacketSend
		{
		protected:
			uint8_t direction;	//!< direction of movement
		public:
			inline MovePlayer(uint8_t aDirection) :
				cPacketSend(NULL, 0), direction(aDirection)
			{ }
			void prepare();
		};

		/*!
		\todo packet 0x98: all names 3d, if needed
		*/

		/*!
		\brief This is sent to bring up a house-placing target [packet 0x99]
		\author Kheru
		\param multi_serial The serial of the multi deed
		\param multi_model The house's multi number (item model - 0x4000)
		\param radius The object's tile radius. [Default: 0 ??]
		*/
		class TargetMulti : public cPacketSend
		{
		protected:
			uint32_t multi_serial;
			uint16_t multi_model;
			uint32_t radius;
		public:
			inline TargetMulti(uint32_t aSerial, uint16_t aModelID, uint32_t aRadius = 0x00000000) :
				cPacketSend(NULL, 0), multi_serial(aSerial), multi_model(aModelID), radius(aRadius)
			{ }
			prepare();
		};

		/*!
		\todo packet 0x9a: console entry prompt, if needed
		*/

		/*!
		\brief Sends Sell list [packet 0x9e]
		\author Chronodt

		sends list of selling items list
		*/
		class SellList : public cPacketSend
		{
		protected:
			std::slist<pItem> items;
			pNpc vendor;
		public:
			inline SellList(pNpc aVendor) :
				cPacketSend(NULL, 0), vendor(aVendor)
			{ }
			inline void addItem(pItem item)
			{ items.push_back(item); }
			void prepare();
		};

		/*!
		\brief Sends new Hp value of a char to client [packet 0xa1]
		\author Chronodt
		*/
		class UpdateHp : public cPacketSend
		{
		protected:
			pChar pc;
		public:
			inline UpdateHp(pChar aPc) :
				cPacketSend(NULL, 0), pc(aPc)
			{ }
			void prepare();
		};

		/*!
		\brief Sends new Mana value of a char to client [packet 0xa2]
		\author Chronodt
		*/
		class UpdateMana : public cPacketSend
		{
		protected:
			pChar pc;
		public:
			inline UpdateMana(pChar aPc) :
				cPacketSend(NULL, 0), pc(aPc)
			{ }
			void prepare();
		};

		/*!
		\brief Sends new Stamina value of a char to client [packet 0xa3]
		\author Chronodt
		*/
		class UpdateStamina : public cPacketSend
		{
		protected:
			pChar pc;
		public:
			inline UpdateStamina(pChar aPc) :
				cPacketSend(NULL, 0), pc(aPc)
			{ }
			void prepare();
		};


		/*!
		\brief Opens web browser [packet 0xa5]
		\author Flameeyes
		*/
		class OpenBrowser : public cPacketSend
		{
		protected:
			std::string url;	//!< Url to open the browser to
		public:
			/*!
			\param str Url to open the browser to
			*/
			inline OpenBrowser(std::string str) :
				cPacketSend(NULL, 0), url(str)
			{ }
			void prepare();
		};

		/*!
		\brief Send Tips or MOTD/Server notice window [packet 0xa6]
		\author Kheru
		\note Packet 0xA6
		*/
		class TipsWindow : public cPacketSend
		{
		protected:
			uint8_t	type;		//!< Type: 0x00 tip, 0x02 MOTD
			uint16_t tip_num;	//!< #Tip number (no use if MOTD)
			std::string message;	//!< Tip (or MOTD) to send

		public:
			inline TipsWindow(std::string aMessage, uint8_t TipType = 0x02, uint16_t TipNum = 0x0000) :
				cPacketSend(NULL, 0), type(TipType), tip_num(TipNum), message(aMessage)
			{ }

			void prepare();
		};

		/*!
		\todo packet 0xa8: Game Server List... login packet (strictly linked with Login2 in network.cpp and a global variable)
		*/

		/*!
		\todo packet 0xa9: Characters / Starting Locations... login packet (strictly linked with GoodAuth in network.cpp and a global variable)
		*/


		/*!
		\brief Sends the OK/Not OK for an attack
		\author Flameeyes
		\note Packet 0xAA
		*/
		class AttackAck : public cPacketSend
		{
		protected:
			pChar victim;	//!< Character attacked
		public:
			/*!
			\param v Character attacked (should be NULL to undo an attack)
			*/
			AttackAck(pChar v) :
				cPacketSend(NULL, 0), victim(v)
			{ }

			void prepare();
		};

		/*!
		\brief Opens Gump Text Entry Dialog [packet 0xab]
		\author Chronodt

		This packet sends a text entry dialog to the client. No checkbox, id or anything except a single button, a description and a text entry to be filled
		Useful for name request popups, and similar dialogs

		This is the second "open gump" packet (the other ones are the 0x7c and the 0xb0 packets)
		\todo awaiting gump remake
		*/
		class OpenTextEntryDialog : public cPacketSend
		{
		protected:
		public:
			inline OpenTextEntryDialog() :		//!< \todo finish this function
				cPacketSend(NULL, 0)
			{ }
			void prepare();
		};

		/*!
		\brief Send character unicode speech to listener [packet 0xae]
		\author Chronodt
		*/

		class UnicodeSpeech : public cPacketSend
		{
		protected:
			cSpeech speech;				//!< what ps is saying
		public:
			/*!
			\param s what is being told
			*/
			inline UnicodeSpeech(cSpeech &s) :
				cPacketSend(NULL, 0), speech(s)
			{ }
			void prepare();
		};

		/*!
		\brief Display death action [packet 0xaf]
		\author Chronodt
		*/

		class DeathAction : public cPacketSend
		{
		protected:
			pChar pc;		//!< the newly dead :D
			pContainer corpse;	//!< the new corpse for player
		public:
			inline DeathAction(pChar aPc, pContainer aCorpse) :
				cPacketSend(NULL, 0), pc(aPc), corpse(aCorpse)
			{ }
			void prepare();
		};

		/*!
		\brief Opens Gump Menu Dialog [packet 0xb0]
		\author Chronodt

		This packet sends a gump to the client. This is the most complete dialog since you cann add icons, buttons, checkboxes
		and everything else to the gump. Obiously is the gump packet that sends the most data to the client (and receives the most
		data for reply)

		This is the third and last "open gump" packet (the other ones are the 0x7c and the 0xab packets)
		\todo awaiting gump remake
		*/
		class OpenGumpMenuDialog : public cPacketSend
		{
		protected:
		public:
			inline OpenGumpMenuDialog() :		//!< \todo finish this function
				cPacketSend(NULL, 0)
			{ }
			void prepare();
		};

		/*!
		\brief Chat Message [packet 0xb2]
		\author Chronodt

		\todo awaiting chat implementation
		*/
		class ChatMessage : public cPacketSend
		{
		protected:
		public:
			inline ChatMessage() :		//!< \todo finish this function
				cPacketSend(NULL, 0)
			{ }
			void prepare();
		};


		/*!
		\brief Chat Message [packet 0xb5]
		\author Chronodt

		\todo awaiting chat implementation
		*/
		class OpenChatWindow : public cPacketSend
		{
		protected:
		public:
			inline OpenChatWindow() :		//!< \todo finish this function
				cPacketSend(NULL, 0)
			{ }
			void prepare();
		};


		/*!
		\brief Sends popup help [packet 0xb7]
		\author Chronodt
		*/
		class PopupHelp : public cPacketSend
		{
		protected:
			std::string text;	// text to be sent (will be converted in unicode)
			pSerializable object;	// since this packet is only sent immediately after the request
		public:
			inline PopupHelp(std::string &aText, pSerializable aObject) :
				cPacketSend(NULL, 0), text(aText), object(aObject)
			{ }
			void prepare();
		};

		/*!
		\brief sends character profile [packet 0xb8]
		\author Chronodt
		*/
		class CharProfile : public cPacketSend
		{
		protected:
			uint32_t serial;
			pChar who;
		public:
			inline CharProfile(uint32_t s, pChar w) :
				cPacketSend(NULL, 0), serial(s), who(w)
			{ }
			void prepare();
		};

		/*!
		\brief Enable locked client features [Packet 0xB9]
		\param features to set.

		if (MSB not set)
			Bit#1 T2A upgrade, enables chatbutton,
			Bit#2 enables LBR update. (of course LBR installation is required)
				(plays MP3 instead of midis, 2D LBR client shows new LBR monsters,)
		if (MSB set)
			Bit#3 T2A upgrade, enables chatbutton,
			Bit#4 enables LBR update.
			Bit#5 enables AOS update (Aos monsters/map (AOS installation required for that) ,
				AOS skills/necro/paladin/fight book stuff - works for ALL clients 4.0 and above)
		                           
		\note This packet enables various client-side features, and should be sent before the  \ref nPackets::Sent::CharList packet.
 		\note Note that in order to create a Paladin or Necromancer or use the 6th character slot, the corresponding flags in the \ref nPackets::Sent::CharList packet must be set.
		
		*/	
		class Features : public cPacketSend
		{
		protected:
			uint16_t features;

		public:
			inline Features(uint16_t aFeatures) :
				cPacketSend(NULL, 0), features(aFeatures)
			{ }

			void prepare();
		};
		
		class LogoutStatus : public cPacketSend
		{
		protected:
		
		public:
			inline LogoutStatus() :
				cPacketSend(NULL, 0)
			{ }
		
			void prepare();
		};
		
		class ClientViewRange : public cPacketSend
		{
		protected:
			uint8_t range;
		public:
			inline ClientViewRange(uint8_t r) :
				cPacketSend(NULL, 0), range(r)
			{ }
		
			void prepare();
		};
		

	} // Sent
} // nPackets

#endif

