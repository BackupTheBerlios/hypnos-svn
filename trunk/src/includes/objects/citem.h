/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| PyUO Server Emulator                                                     |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in pyuo.cpp file.              |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Header defining cItem class
*/
#ifndef __ITEMS_H
#define __ITEMS_H

class cItem;
typedef cItem* pItem;

#include "object.h"
#include "magic.h"
#include "item.h"
#include "globals.h"

#include "itemid.h"

//! Item class
class cItem : public cObject
{
public:
	static pItem addByID(SI32 id, UI16 nAmount, const char *cName, UI16 color, Location where);


	//! Redefinition of = operator for cItem class
        cItem& operator=(cItem& b);

	static UI32	nextSerial();

        static void	archive();
	static void	safeoldsave();

	cItem(SERIAL serial);
        cItem();
	~cItem();

	static const UI32 flagUseAnimID		= 0x00000001;
		//!< The item uses animID

	//! deletion type
	enum DelType
	{
		DELTYPE_UNKNOWN,
		DELTYPE_DECAY,
		DELTYPE_GMREMOVE
	};

	//! Events for items
	enum Event {
		EVENT_IONSTART = 0,
		EVENT_IONDAMAGE,
		EVENT_IONEQUIP,
		EVENT_IONUNEQUIP,
		EVENT_IONCLICK,
		EVENT_IONDBLCLICK,
		EVENT_IPUTINBACKPACK,
		EVENT_IDROPINLAND,
		EVENT_IONCHECKCANUSE,
		EVENT_IONTRANSFER,
		EVENT_IONSTOLEN,
		EVENT_IONPOISONED,
		EVENT_IONDECAY,
		EVENT_IONREMOVETRAP,
		EVENT_IONLOCKPICK,
		EVENT_IONWALKOVER,
		EVENT_IONPUTITEM,
		EVENT_ITAKEFROMCONTAINER,
		ALLITEMEVENTS
	};

//@{
/*!
\name Weapon Types
*/
	static const UI16 weaponInvalid = 0x0000; //!< Not a weapon
	static const UI16 weaponSword1H = 0x0001; //!< Sword 1 hand
	static const UI16 weaponSword2H = 0x0002; //!< Sword 2 hands
	static const UI16 weaponAxe1H	= 0x0004; //!< Axe 1 hand
	static const UI16 weaponAxe2H	= 0x0008; //!< Axe 2 hands
	static const UI16 weaponMace1H	= 0x0010; //!< Mace 1 hand
	static const UI16 weaponMace2H	= 0x0020; //!< Mace 2 hands
	static const UI16 weaponFenc1H	= 0x0040; //!< Fencing 1 hand
	static const UI16 weaponFenc2H	= 0x0080; //!< Fencing 2 hands
	static const UI16 weaponStave1H = 0x0100; //!< Staff 1 hand
	static const UI16 weaponStave2H = 0x0200; //!< Staff 2 hands
	static const UI16 weaponBow	= 0x0400; //!< Bow
	static const UI16 weaponXBow	= 0x1000; //!< Crossbow
	static const UI16 weaponHXBow	= 0x2000; //!< Heavy Crossbow

	static void loadWeaponsInfo();
	static const bool isWeaponLike(UI16 id, UI16 type);

protected:
	typedef std::map<UI16,UI16> WeaponMap;
	//! Map with types of weapons
	static WeaponMap weaponinfo;

//@}

protected:

//@{
/*!
\name Who is
*/
protected:
	Event			*events[ALLITEMEVENTS];
	int			handleEvent(UI08 code, UI08 nParams, UI32 *params);

	SI32			hp;	//!< Number of hit points an item has.
	SI32			maxhp;	//!< Max number of hit points an item can have.
public:
	void			Refresh();
	const std::string	getName();
	const std::string	getRealItemName();
	void			getPopupHelp(char *str);

	inline const SI32 getHP() const
	{ return hp; }

	inline void setHP(SI32 newhp) const
	{ hp = newhp; }

	inline const SI32 getMaxHP() const
	{ return maxhp; }

	inline void setMaxHP(SI32 newhp) const
	{ maxhp = newhp; }
//@}

//@{
/*!
\name Look
*/
protected:
	UI16		animid;		//!< animation id
	SI08		layer;		//!< Layer if equipped on paperdoll
	SI08		oldlayer;	//!< Old layer - used for bouncing bugfix - AntiChrist
	SI08		scriptlayer;	//!< Luxor, for scripted setted Layer

	SI08		magic;		//!< 0=Default as stored in client, 1=Always movable, 2=Never movable, 3=Owner movable.
	SI08		visible;	//!< 0=Normally Visible, 1=Owner & GM Visible, 2=GM Visible
	SI16		dir;


public:
//@{
/*!
\name itemid
\brief Static funcions to identify items. Most inlines
*/

	inline static const bool isCorpse(UI16 id)
	{ return id==0x2006; }

	inline static const bool isTree(UI16 id)
	{
		return (id==0x0CD0 || id==0x0CD3 || id==0x0CD6 ||
			id==0x0CD8 || id==0x0CDA || id==0x0CDD ||
			id==0x0CE0 || id==0x0CE3 || id==0x0CE6 ||
			(id>=0x0CCA && id<=0x0CCD) ||
			(id>=0x12B8 && id<=0x12BB) ||
			id==0x0D42 || id==0x0D43 || id==0x0D58 ||
			id==0x0D59 || id==0x0D70 || id==0x0D85 ||
			id==0x0D94 || id==0x0D95 || id==0x0D98 ||
			id==0x0Da4 || id==0x0Da8 || id==0x0D58);
	}

	//! this is used in SwordTarget() to give kindling.
	//  Donno why it's different
	inline static const bool isTree2(UI16 id)
	{
		return (id==0x0CD0 || id==0x0CD3 || id==0x0CD6 ||
			id==0x0CD8 || id==0x0CDA || id==0x0CDD ||
			id==0x0CE0 || id==0x0CE3 || id==0x0CE6 ||
			(id>=0x0CCA && id<=0x0CCD) ||
			(id>=0x12B8 && id<=0x12BB) );
	}

	inline static const bool isLog(UI16 id)
	{ return ( id>=0x1BDD && id<=0x1BE2 ); }

	inline static const bool isShaft(UI16 id)
	{ return ( id>=0x1BD4 && id<=0x1BD6 ); }

	inline static const bool isFeather(UI16 id)
	{ return ( id>=0x1BD1 && id<=0x1BD3 ); }

	static const bool isHouse(UI16 id);

// Non-static ItemID functions

	const magic::FieldType isFieldSpellItem() const;

	inline const bool isCorpse() const
	{ return isCorpse( getId() ); }

	//! predefined spells
	inline const bool isSpellScroll() const
	{ return (getId()>0x1F2C && getId()<0x1F6D); }

	//! includes the scrolls with a variable name
	inline const bool isSpellScroll72() const
	{ return (getId() >= 0x1F2D && getId() <= 0x1F72); }

	//! this is used in AxeTarget()
	inline const bool isTree() const
	{ return isTree( getId() ); }

	//! this is used in SwordTarget() to give kindling.
	inline const bool isTree2() const
	{ return isTree2( getId() ); }

	inline const bool isInstrument() const
	{
		return (getId()==0x0E9C || getId()==0x0E9D ||
			getId()==0x0E9E || getId()==0x0EB1 ||
			getId()==0x0EB2 || getId()==0x0EB3 ||
			getId()==0x0EB4);
	}

	inline const bool IsAxe() const
	{ return isWeaponLike( this->getId(), AXE1H); }

	inline const bool IsSword() const
	{ return isWeaponLike( getId(), SWORD1H ); }

	inline const bool IsSwordType() const
	{ return isWeaponLike( getId(), SWORD1H, AXE1H ); }

	inline const bool IsMace1H() const
	{ return isWeaponLike( getId(), MACE1H); }

	inline const bool IsMace2H() const
	{ return isWeaponLike( getId(), MACE2H); }

	inline const bool IsMaceType() const
	{ return isWeaponLike( getId(), MACE1H, MACE2H ); }

	inline const bool IsFencing1H() const
	{ return isWeaponLike( getId(), FENCING1H); }

	inline const bool IsFencing2H() const
	{ return isWeaponLike( getId(), FENCING2H); }

	inline const bool IsFencingType() const
	{ return isWeaponLike( getId(), FENCING1H, FENCING2H ); }

	inline const bool IsBow() const
	{ return isWeaponLike( getId(), BOW ); }

	inline const bool IsCrossbow() const
	{ return isWeaponLike( getId(), CROSSBOW ); }

	inline const bool IsHeavyCrossbow() const
	{ return isWeaponLike( getId(), HEAVYCROSSBOW ); }

	inline const bool IsBowType() const
	{ return isWeaponLike( getId(), BOW, CROSSBOW, HEAVYCROSSBOW ); }

	inline const bool isArrow() const
	{ return (getId()==0x0F3F||getId()==0x0F42); }

	inline const bool isBolt() const
	{ return (getId()==0x1BFB||getId()==0x1BFE); }

	inline const bool IsStave() const
	{ return isWeaponLike( getId(), STAVE1H, STAVE2H ); }

	//! -Fraz- The OSI macing weapons that do stamina and armor damage 2handed only
	inline const bool IsSpecialMace() const
	{ return isWeaponLike( getId(), STAVE1H, STAVE2H, MACE2H ); }

	inline const bool isChaosOrOrderShield() const
	{ return (getId()>=0x1BC3 && getId()<=0x1BC5); }

	inline const bool isShield() const
	{
		return ((getId()>=0x1B72 && getId()<=0x1B7B) ||
			isChaosOrOrderShield());
	}

	inline const bool isLog() const
	{ return isLog(getId()); }

	inline const bool isShaft() const
	{ return isShaft(getId()); }

	inline const bool isBoard() const
	{ return ( getId()>=0x1BD7 && getId()<=0x1BDC ); }

	inline const bool isFeather() const
	{ return isFeather(getId()); }

	inline const bool isCutLeather() const
	{
		return (getId()==0x1067 || getId()==0x1068 ||
			getId()==0x1081 || getId()==0x1082 );
	}

	inline const bool isHide() const
	{ return ( getId()==0x1078 || getId()==0x1079 ); }

	inline const bool isBoltOfCloth() const
	{ return ( getId()>=0x0F95 && getId()<=0x0F9C ); }

	inline const bool isCutCloth() const
	{ return ( getId()>=0x1766 && getId()<=0x1768 ); }

	inline const bool isCloth() const
	{
		return ((getId()>=0x175D && getId()<=0x1765)
			|| isCutCloth() );
	}

	inline const bool isChest() const
	{
		return (( getId()>=0x09A8 && getId()<=0x09AB) ||
			( getId()>=0x0E40 && getId()<=0x0E43) ||
			( getId()==0x0E7C) || (getId()==0x0E7D) ||
			( getId()==0x0E80));
	}

	inline const bool isForge() const
	{
		return  ( getId()==0x0FB1 ||
			( getId()>=0x197A && getId()<=0x19A9 ) );
	}

	inline const bool isAnvil() const
	{ return ( getId()==0x0FAF || getId()==0x0FB0 ); }

	inline const bool isCookingPlace() const
	{
		return ((getId()>=0x0DE3 && getId()<=0x0DE9)||
			(getId()==0x0FAC || getId()==0x0FB1)||
			(getId()>=0x197A && getId()<=0x19B6)||
			(getId()>=0x0461 && getId()<=0x0480)||
			(getId()>=0x0E31 && getId()<=0x0E33)||
			(getId()==0x19BB || getId()==0x1F2B)||
			(getId()>=0x092B && getId()<=0x0934)||
			(getId()>=0x0937 && getId()<=0x0942)||
			(getId()>=0x0945 && getId()<=0x0950)||
			(getId()>=0x0953 && getId()<=0x095E)||
			(getId()>=0x0961 && getId()<=0x096C) );
	}

	inline const bool isDagger() const
	{ return ( getId()==0x0F51 || getId()==0x0F52 ); }

	inline const bool isFish() const
	{ return ( getId()>=0x09CC && getId()<=0x09CF ); }

	inline const bool isFishWater() const
	{
		return ((getId()==0x1798) || (getId()==0x179B) ||
			(getId()==0x179C) || (getId()==0x1799) ||
			(getId()==0x1797) || (getId()==0x179A) ||
			(getId()==0x346E) || (getId()==0x346F) ||
			(getId()>=0x3470) && (getId()<=0x3485) ||
			(getId()>=0x3494) && (getId()<=0x349F) ||
			(getId()>=0x34A0) && (getId()<=0x34AB) ||
			(getId()>=0x34B8) && (getId()<=0x34BB) ||
			(getId()>=0x34BD) && (getId()<=0x34BF) ||
			(getId()>=0x34C0) || (getId()<=0x34C2) ||
			(getId()>=0x34C3) && (getId()<=0x34C5) ||
			(getId()>=0x34C7) && (getId()<=0x34CA) ||
			(getId()>=0x34D1) && (getId()<=0x34D5));
	}

	inline const bool isSign() const
	{
		return ((getId()==0x0B95 || getId()==0x0B96) ||
			(getId()>=0x0BA3 && getId()<=0x0C0E) ||
			(getId()==0x0C43 || getId()==0x0C44));
	}

	inline const bool isBrassSign() const
	{ return ((getId()==0x0BD1 || getId()==0x0BD2)); }

	inline const bool isWoodenSign() const
	{ return ((getId()==0x0BCF || getId()==0x0BD0)); }

	inline const bool isGuildSign() const
	{ return ((getId() >= 0x0BD3 && getId() <= 0x0C0A)); }

	inline const bool isTradeSign() const
	{
		return ((getId()==0x0B95 || getId()==0x0B96) ||
			(getId() >= 0x0BA3 && getId() <= 0x0BCE));
	}

	inline const bool isBankSign() const
	{ return ((getId() >= 0x0C0B && getId() <= 0x0C0C)); }

	inline const bool isTheatreSign() const
	{ return ((getId() >= 0x0C0D && getId() <= 0x0C0E)); }

	inline const bool isHouse() const
	{ return isHouse( getId() ); }

	//! Tells if an item is a dynamic spawner
	inline const bool isSpawner() const
	{ return ( type >= 61 && type <= 65 ) || type == 59 || type == 125; }

//@}

//@{
/*!
\name flags
*/
protected:
	static const UI64 flagPileable		= 0x0000000000000001ull; //!< Can the item be piled?
	static const UI64 flagCanDecay		= 0x0000000000000002ull; //!< Can the item decay?
	static const UI64 flagNewbie		= 0x0000000000000004ull; //!< Is the item newbie?
	static const UI64 flagDispellable	= 0x0000000000000004ull; //!< Can the item be dispelled?

public:
	inline const bool isPileable() const
	{ return flags & flagPileable; }

	inline void setPileable(bool set = true)
	{ setFlag(flagPileable, set); }

	inline const bool canDecay() const
	{ return flags & flagCanDecay; }

	inline void setDecay(bool set = true)
	{ setFlag(flagCanDecay, set); }

	inline const bool isNewbie() const
	{ return flags & flagNewbie; }

	inline void setNewbie(bool set = true)
	{ setFlag(flagNewbie, set); }

	inline const bool isDispellable() const
	{ return flags & flagDispellable; }

	inline void setDispellable(bool set = true)
	{ setFlag(flagDispellable, set); }
//@}

//@{
/*!
\name Container
*/
protected:
	pObject		cont;
	pObject		oldcont;

public:
	inline const pObject getContainer() const
	{ return cont; }

	void setContainer(pObject obj);

	inline const pObject getOldContainer() const
	{ return oldcont; }

	inline void setOldContainer(pObject obj)
	{ oldcont = obj; }

	//! check if item is a container
	inline const bool isContainer() const
	{ return type==1 || type==12 || type==63 || type==8 || type==13 || type==64; }

	inline const bool isSecureContainer() const
	{ return type==8 || type==13 || type==64; }

	SI32		secureIt; // secured chests
	void		putInto( P_ITEM pi );
//@}

/********************************
	Extra info
********************************/
	UI08		more1;		// For various stuff
	UI08		more2;
	UI08		more3;
	UI08		more4;
	SI08		moreb1;
	SI08		moreb2;
	SI08		moreb3;
	SI08		moreb4;
	UI32		morex;
	UI32		morey;
	UI32		morez;

//@{
/*!
\name Amount
*/
protected:
	UI16		amount;		//!< Amount of items in pile
	UI16		amount2;	//!< Used to track things like number of yards left in a roll of cloth

public:
	SI32		ReduceAmount(const SI16 amount);
	SI32		IncreaseAmount(const SI16 amount);

	//! sets the amount of piled items
	inline void setAmount(const UI16 amt)
	{ amount = amt; Refresh(); }

        inline const UI16 getAmount()
	{ return amt; }

	SI32			DeleteAmount(int amount, short id, short color=-1);

	inline const SI32	CountItems(short ID=-1, short col= -1,bool bAddAmounts = true) const
	{ return pointers::containerCountItems(getSerial32(), ID, col, bAddAmounts); }

	inline const SI32	CountItemsByID(unsigned int scriptID, bool bAddAmounts) const
	{ return pointers::containerCountItemsByID(getSerial32(), scriptID, bAddAmounts); }
//@}

//@{
/*!
\name Weight
*/
protected:
	UI32			weight;

public:
	R32			getWeight();
	virtual const R32	getWeightActual();
//@}

//@{
/*!
\name Position
*/
	void MoveTo(Location newloc);

	inline const bool isInWorld() const
	{ return cont; }

	inline void MoveTo(SI32 x, SI32 y, SI08 z)
	{ MoveTo( Location(x, y, z) ); }
//@}

//@{
/*!
\name weapon and armour related
\brief weapon, armour, wear related stuff
\author Xan & Luxor (mostly)
*/
	Skill		getCombatSkill();
	UI32		att;		//!< Item attack
	UI32		def;		//!< Item defense
	Skill		fightskill;	//!< skill used by item
	SI32		reqskill[2];	//!< required skill by item
	DamageType	damagetype;	//!< for different damage types system
	DamageType	auxdamagetype;	//!< Additional damage :]
	SI32		auxdamage;	//!< Additional damage :]
	SI32		lodamage;	//!< Minimum Damage weapon inflicts
	SI32		hidamage;	//!< Maximum damage weapon inflicts
	SI32		wpsk;		//!< The skill needed to use the item
	SI32		spd;		//!< The speed of the weapon
	SI32		itmhand;	//!< ITEMHAND system - AntiChrist
	SI32		resists[MAX_RESISTANCE_INDEX];	//!< for resists system
	SI32		st;		//!< The strength needed to equip the item
	SI32		st2;		//!< The strength the item gives
	SI32		dx;		//!< The dexterity needed to equip the item
	SI32		dx2;		//!< The dexterity the item gives
	SI32		in;		//!< The intelligence needed to equip the item
	SI32		in2;		//!< The intelligence the item gives
	PoisonType	poisoned;	//!< type of poison that poisoned item
	UI32		ammo;		//!< Ammo used (firing weapon)
	UI32		ammoFx;		//!< Flying ammo animation (firing weapon)
//@}

//@{
/*!
\name Magic Related
*/
	UI32		gatetime;
	SI32		gatenumber;
	SI08		offspell;
//@}

//@{
/*!
\name Corpse related
*/
	bool		corpse;		//!< Is item a corpse
	string		murderer;	//!< char's name who kille the char (forensic ev.)
	SI32		murdertime;	//!< when the people has been killed
//@}

//@{
/*!
\name Creation related
\author Magius (CHE)
*/
	SI32		smelt;		//!< for item smelting
	/*!
	\brief for rank system, this value is the LEVEL of the item from 1 to 10.

	Simply multiply the rank*10 and calculate the MALUS this item has from the original.<br>
	for example: RANK 5 ---> 5*10=50% of malus<br>
	this item has same values decreased by 50%..<br>
	RANK 1 ---> 1*10=10% this item has 90% of malus!<br>
	RANK 10 --> 10*10=100% this item has no malus! RANK 10 is automatically setted if you select RANKSYSTEM 0.<br>
	Vars: LODAMAGE,HIDAMAGE,ATT,DEF,HP,MAXHP<br>
	*/
	SI32		rank;

	//char		creator[50];	//!< Store the name of the player made this item
	std::string	creator;	//!< Store the name of the player made this item
	SI32		good;		//!< Store type of GOODs to trade system! (Plz not set as UNSIGNED)
	SI32		rndvaluerate;	//!< Store the value calculated base on RANDOMVALUE in region.scp.

	/*!
	\brief Store the skills used to make this item

	Note by Magius: Value range to -ALLSKILLS-1 to ALLSKILLS+1<br>
	To calculate skill used to made this item:<br>
	if is a positive value, substract 1 it.<br>
		Ex) madewith=34 , 34-1=33 , 33=STEALING<br>
	if is a negative value, add 1 from it and invert value.<br>
		Ex) madewith=-34 , -34+1=-33 , Abs(-33)=33=STEALING.<br>
	0 = NULL<br>

	So... a positive value is used when the item is made by a
	player with 95.0+ at that skill. Infact in this way when
	you click on the item appear its name and the name of the
	creator. A Negative value if the player is not enought
	skilled!
	*/
	SI32		madewith;
	//char		desc[100];	//!< vendor description
	std::string	vendorDescription;
//@}

//@{
/*!
\name Spawn
*/
	SERIAL		spawnserial;
	SERIAL		spawnregion;
	void		SetMultiSerial(SI32 mulser);
//@}

//@{
/*!
\name Buy & Sell
*/
protected:
	SI32		value;		//!< Price shopkeeper sells item at.
	SI32		restock;	//!< Number up to which shopkeeper should restock this item
public:
	const SI32 calcValue(SI32 bvalue);
//@}

//@{
/*!
\name Item Use
*/
protected:
        bool            ToolWearOut(pClient client);            //!< Check for tool consumption. Used in doubleClick

public:
        void            singleClick(pClient client);            //!< Single click on item
        bool            usableWhenLockedDown(pPc pc = NULL);	//!< Item can be used when locked down (e.g. in a house)
        bool            checkItemUsability(pChar pc, int type); //!< If item can be used with "type" method by pc
        void            doubleClick(pClient client);            //!< Use of item by doubleclicking on it

//@{
/*!
\name Trigger
*/
	SI32		trigger;	//!< Trigger number that item activates
	SI32		trigtype;	//!< Type of trigger
	SI32		tuses;		//!< Number of uses for trigger
//@}

//@{
/*!
\name Special Use
*/
	UI32		type;		//!< For things that do special things on doubleclicking
	UI32		type2;
	SI32		carve;		//!< for new carve system
	bool		incognito;	//!< for items under incognito effect
	SI32		wipe;		//!< Should this item be wiped with the /wipe command
	UI32		time_unused;	//!< used for house decay and possibly for more in future, gets saved
	UI32		timeused_last;	//!< helper attribute for time_unused, doesnt get saved
//@}

/********************************
	Effect related
********************************/
//	SI32		glow; // LB identifies glowing objects
//	SI08		glow_c1; // for backup of old color
//	SI08		glow_c2;
//	SI08		glow_effect;
	SI08		doordir; // Reserved for doors
	bool		dooropen;
	void		explode(NXWSOCKET  s);


	bool		dye;		//!< Reserved: Can item be dyed by dye kit

protected:
	TIMERVAL	decaytime;

public:
	bool		doDecay();

	inline const void setDecayTime( const TIMERVAL delay = uiCurrentTime+(SrvParms->decaytimer*MY_CLOCKS_PER_SEC) )
	{ decaytime = delay; }

	inline const TIMERVAL getDecayTime() const
	{ return decaytime; }

	pItem		getOutMostCont( UI16 rec=50 );
	pBody		getPackOwner();

	UI32		distFrom( P_CHAR pc );
	UI32		distFrom( P_ITEM pi );

	inline void setAnimid(UI16 id)
	{ animid = id; }

	inline const UI16 getAnimid() const
	{ return animid ? animid : getId(); }

public:
	virtual void	Delete();
} PACK_NEEDED;

class cWeapon : public cItem
{
public:
        cWeapon(SERIAL serial);
} PACK_NEEDED;

extern bool LoadItemEventsFromScript (P_ITEM pi, char *script1, char *script2);


#define MAKE_ITEM_REF(i) pointers::findItemBySerial(i)

#endif
