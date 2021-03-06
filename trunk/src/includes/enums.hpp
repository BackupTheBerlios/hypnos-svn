/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
/*!
\file
\brief Enum types
\author Flameeyes

Enumerations are often used by the code to allow the developers to use mnemonic
string constants instead of rough and raw hexadecimal constants. And quite
always we need to access these constants also in header files to define default
parameters of functions.

And as staded in the coding standards, we shouldn't include the header of the
class which the enums are related to have them declared (because we create
a chain of dependencies in header files which is unsafe), so we need a way
to declare all the enums without their own class. The solution is this file.

In this file there are different enums, documented when needed, and with a
refernece to the class which primary uses them (with a \\see link).

\todo Complete the documentation of types and enumerated constantas
*/

#ifndef __ENUMS_H__
#define __ENUMS_H__

/*!
\brief Message Board's Post's Type

\see cMsgBoard
*/
enum PostType { ptLocalPost = 0, ptRegionalPost, ptGlobalPost };

/*!
\brief Message Board's Quest's Type

\see cMsgBoard
*/
enum QuestType { qtEscort = 0xFF, qtBounty = 0xFE, qtItem = 0xFD, qtInvalid = 0x0 };

/*!
\brief Layer index as used by client

\see cEquippable
\see cBody
*/
enum Layer {
	layNone,		//!< Not equipped
	layWeapon1H = 1,	//!< One handed weapon layer
	layWeapon2H,		//!< Two handed weapon, shield, or misc layer
	layShoes,		//!< Shoes layer
	layPants,		//!< Pants layer
	layShirt,		//!< Shirt layer
	layHat,			//!< Helm/Hat layer
	layGloves,		//!< Gloves layer
	layRing,		//!< Ring layer
	layUnused,		//!< Unused layer
	layNeck,		//!< Neck layer
	layHair,		//!< Hair layer
	layWaist,		//!< Waist (half apron) layer
	layTorsoInner,		//!< Torso (inner) (chest armor) layer
	layBracelet,		//!< Bracelet layer
	layUnused2,		//!< Unused (backpack?) layer
	layBeard,		//!< Facial Hair (Beard) layer
	layTorsoMiddle,		//!< Torso (middle) (sircoat, tunic, full apron, sash) layer
	layEarrings,		//!< Earrings layer
	layArms,		//!< Arms layer
	layBack,		//!< Back (cloack) layer
	layBackpack,		//!< Backpack layer
	layTorsoOuter,		//!< Torso (outer) (robe) layer
	layLegsOuter,		//!< Legs (outer) (skirt/kilt) layer
	layLegsInner,		//!< Legs (inner) (leg armor) layer
	layMount,		//!< Mount (horse, ostarard, etc) layer
	layNPCBuyR,		//!< NPC Buy Restock container layer (see cNPC::restock)
	layNPCBuyN,		//!< NPC Buy no-restock container layer (see cNPC::bought)
	laySell,		//!< NPC Buy Sell container layer (see cNPC::sell)
	layBank			//!< PC Bank Box (see cPC::bankBox)
};

/*!
\brief Type of crypto-algorithms

\see cAccount
*/
enum CryptoType {
	cryptoPlain,	//! Plain text
	cryptoMD5,	//! MD5 hash
	cryptoSHA1	//! SHA1 hash
};

/*!
\brief Type of clients

\see cPacketSend
*/
enum ClientType {
	ctNormal	//!< Normal client
};

//! Type of poison
enum PoisonType {
	poisonNone,	//!< No poison
	poisonWeak,	//!< Weak poison
	poisonNormal,	//!< Normal poison
	poisonGreater,	//!< Greater poison
	poisonDeadly	//!< Deadly poison
};

/*!
\brief Directions

Directions are represented in the UO protocol as a byte, where are used only
the three LSBs, and the MSB states the 'running' action.

This enumeration type can be used both for get only the three LSBs of the
direction (but is faster use var&0x07), and to access the direction constasnts
without remember the right index of them.

\see cBody
\see cItem
\see cBoat
*/
enum Direction {
	dirNorth, dirNorthEast, dirEast, dirSouthEast,
	dirSouth, dirSouthWest, dirWest, dirNorthWest
};

/*!
\brief Types of damage and resistance [UO4]

This type defines the five kinds of attacks and defences possible on the new
Ultima OnLine: Age of Shadows extension.
The previous implementation (the NoX-Wizard's one) consisted of many more
damage types and so resistances, but they are now simplified to the base ones.

It will be quite simple to implement at scripting level new types of damage, so
they won't be handled by the core system.
*/
enum DamageType {
	damPhysical, damFire, damCold, damPoison, damEnergy
};

//! Type of hiding
enum HideType {
	htUnhidden,	//!< Not hidden
	htBySkill,	//!< Hidden by hiding skill
	htBySpell	//!< Hidden by invisible spell
};

//! deletion type
enum DelType {
	deleteUnknown, deleteDecay, deleteGMRemove
};

//! Type of dispel
enum DispelType {
	dispelUnknwon, dispelDispel, dispelTimeout, dispelGMRemove
};

//! Reputation's karma
enum Karma {
	karmaInnocent, karmaMurderer, karmaCriminal
};

//! cChar::canEquip result
enum NotEquippableReason {
	nerUnknown = -1,		//!< unknown reason
	nerEquipOk = 0,			//!< item can be equipped
	nerNotEquippableItem,		//!< item is not convertible to a cEquippable
	nerNotHumanBody,		//!< char cannot equip item because it is not human
	nerInsufficientStrength,	//!< Insufficient strength
	nerInsufficientDexterity,	//!< Insufficient dexterity
	nerInsufficientIntelligence,	//!< Insufficient intelligence
	nerInsufficientSkil1,		//!< Insufficient skill 1
	nerInsufficientSkil2,		//!< Insufficient skill 2
	nerInsufficientSkil3,		//!< Insufficient skill 3
	nerCharDead,			//!< Char is dead. Dead people cannot wear anything :D
	nerMaleEquippingFemaleArmor	//!< It wouldn't fit anyway :D

};

//! Skills
enum Skill {
	skInvalid = -1, skAlchemy = 0, skAnatomy, skAnimalLore, skItemID, skArmsLore, skParrying,
	skBegging, skBlacksmithing, skBowcraft, skPeacemaking, skCamping, skCarpentry, skCartography,
	skCooking, skDetectingHidden, skEnticement, skEvaluatingIntelligence, skHealing, skFishing,
	skForensics, skHerding, skHiding, skProvocation, skInscription, skLockPicking, skMagery,
	skMagicResistance, skTactics, skSnooping, skMusicianship, skPoisoning, skArchery, skSpiritSpeak,
	skStealing, skTailoring, skTaming, skTasteID, skTinkering, skTracking, skVeterinary, skSwordsmanship,
	skMacefighting, skFencing, skWrestling, skLumberjacking, skMining, skMeditation, skStealth,
	skRemoveTraps, skTrueSkills
};

/*!
\brief Types of variaibles
\author Endymion
*/
enum VarType { VAR_EFF =0, VAR_DEC, VAR_REAL, VAR_ACT };

//! Commands for PlotCourse packet
enum PlotCourseCommands {
	pccAddPin = 1,		//!< Add map poin
	pccInsertPin,		//!< Add new pin with pin number (insertion. other pins after the number are pushed back.)
	pccChangePin,		//!< Change pin
	pccRemovePin,		//!< Remove pin
	pccClearAllPins,	//!< Remove all pins on the map
	pccToggleWritable,	//!< Toggle the 'editable' state of the map
	pccWriteableStatus	//!< Return message from the server to request 6 of the client
};

//! climate types
enum Climates {
	clNormal = 1,		//!< Temperate
	clArtic,		//!< frigid and should have high snow probability
	clTropical,		//!< hot and humid areas. Should have a high rain probability
	clDry,			//!< deserts (or generally warm low-raining areas)
	clNone			//!< dungeons, underground and other no-weather areas
};

//! types of weather
enum WeatherType {
	wtSun,		//!< Sunny, no clouds in the sky
	wtCloud,	//!< moderately cloudy
	wtStormCloud,	//!< heavy clouds, storm-type (but no raining yet)
	wtLightRain,	//!< light rain
	wtMediumRain,	//!< medium sized storm
	wtHeavyRain,	//!< waterfall-level storm :D
	wtLightSnow,	//!< light snow
	wtMediumSnow,	//!< medium snowstorm
	wtHeavySnow	//!< blizzard
};

//! Graphical effect type
enum EffectType {
	etBolt = 0,	//!< effect travels from source to target
	etLightning,	//!< lighning effect at source
	etStayInPlace,	//!< effect stays at source xyz
	etFollowObject	//!< effect follows source
};

/*!
\brief Mode to set a suspect

This enum is used for cChar::setCrimGrey() function which depends on various 
settings in the nSettings::Reputation namespace.
*/
enum SuspectAction {
	saNormal,	//!< Doesn't change the suspect's state
	saCriminal,	//!< Set the suspect criminal
	saGrey		//!< Set the suspect grey
};

/*!
\brief Place to add the newly created items
\see sNewbieItem
*/
enum ItemPlace {
	ipBackpack,	//!< The item will be added in the backpack
	ipBankbox,	//!< The item will be added in the bank box
	ipEquip		//!< The item will be equipped to the character
};

/*!
\brief Reputation change type
\see cChar

Used only to call the scripts callbacks
*/
enum ReputationChange {
	repFame = 1,	//!< Fame changed
	repKarma	//!< Karma changed
};

//! target type
enum TargetType {
	ttAll = 0,		//!< any target is valid
	ttObject,		//!< anything with a serial is valid
	ttChar,			//!< chars are valid targets
	ttItem,			//!< items are valid targets
	ttLocation		//!< target location rather than an object
};

/*!
\brief Default spells' IDs

This enums contains all the spell's IDs from the default provided by Hypnos.
They are magery, necromantic and chivalry spells, and everyone uses the spells'
framework provided by Hypnos if not otherwise specified int the spells datafile.

New spells which shares the underlying framework with the base's one can be
added always using the spells datafile.

\todo Add values for necromantic spells.
\todo Add documentation about the spells' datafile.
\todo Add support for the spells' datafile
*/
enum SpellId {
	spellInvalid = -1,
	// Magery - Level 1
	spellClumsy, spellCreateFood, spellFeebleMind, spellHeal, 
	spellMagicArrow, spellNightSight, spellReactiveArmour, spellWeaken,
	// Magery - Level 2
	spellAgility, spellCunning, spellCure, spellHarm,
	spellTrap, spellUntrap, spellProtection, spellStrength,
	// Magery - Level 3
	spellBless, spellFireball, spellLock, spellPoison, 
	spellTelekinesys, spellTeleport, spellUnlock, spellWallStone,
	// Magery - Level 4
	spellArchCure, spellArchProtection, spellCurse, spellFireField,
	spellGreatHeal, spellLightning, spellManaDrain, spellRecall,
	// Magery - Level 5
	spellBladeSpirit, spellDispelField, spellIncognito, spellReflection,
	spellMindBlast, spellParalyze, spellPoisonField, spellSummon,
	// Magery - Level 6
	spellDispel, spellEnergyBolt, spellExplosion, spellInvisibility,
	spellMark, spellMassCurse, spellParalyzeField, spellReveal,
	// Magery - Level 7
	spellChainLighting, spellEnergyField, spellFlameStrike, spellGate,
	spellManaVampire, spellMassDispel, spellMeteorSwarm, spellPolymorph,
	// Magery - Level 8
	spellEarthquake, spellEnergyVortex, spellResurrection, spellSummonElemAir,
	spellSummonDaemon, spellSummonElemEarth, spellSummonElemFire, spellSummonElemWater,
	// Chivalry
	spellCleanseByFire, spellCloseWounds, spellConsecrateWeapon,
	spellDispelEvil, spellDivineFury, spellEnemyOfOne, spellHolyLight,
	spellNobleSacrifice, spellRemoveCurse, spellSacredJourney
};

#endif
