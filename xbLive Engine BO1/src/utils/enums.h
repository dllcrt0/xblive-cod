#pragma once

enum ScrollOptionType {
	SCROLL,
	SELECT,
	TOGGLE,
	SCROLLSELECT
};

enum TraceHitType {
	TRACE_HITTYPE_NONE = 0x0,
	TRACE_HITTYPE_ENTITY = 0x1,
	TRACE_HITTYPE_DYNENT_MODEL = 0x2,
	TRACE_HITTYPE_DYNENT_BRUSH = 0x3,
};

enum weapType_t {
	WEAPTYPE_BULLET = 0x0,
	WEAPTYPE_GRENADE = 0x1,
	WEAPTYPE_PROJECTILE = 0x2,
	WEAPTYPE_BINOCULARS = 0x3,
	WEAPTYPE_GAS = 0x4,
	WEAPTYPE_BOMB = 0x5,
	WEAPTYPE_MINE = 0x6,
	WEAPTYPE_MELEE = 0x7,
	WEAPTYPE_RIOTSHIELD = 0x8,
	WEAPTYPE_NUM = 0x9,
};

enum weapClass_t {
	WEAPCLASS_RIFLE = 0x0,
	WEAPCLASS_MG = 0x1,
	WEAPCLASS_SMG = 0x2,
	WEAPCLASS_SPREAD = 0x3,
	WEAPCLASS_PISTOL = 0x4,
	WEAPCLASS_GRENADE = 0x5,
	WEAPCLASS_ROCKETLAUNCHER = 0x6,
	WEAPCLASS_TURRET = 0x7,
	WEAPCLASS_NON_PLAYER = 0x8,
	WEAPCLASS_GAS = 0x9,
	WEAPCLASS_ITEM = 0xA,
	WEAPCLASS_MELEE = 0xB,
	WEAPCLASS_KILLSTREAK_ALT_STORED_WEAPON = 0xC,
	WEAPCLASS_PISTOL_SPREAD = 0xD,
	WEAPCLASS_NUM = 0xE,
};

enum PenetrateType {
	PENETRATE_TYPE_NONE = 0x0,
	PENETRATE_TYPE_SMALL = 0x1,
	PENETRATE_TYPE_MEDIUM = 0x2,
	PENETRATE_TYPE_LARGE = 0x3,
	PENETRATE_TYPE_COUNT = 0x4,
};

enum weapInventoryType_t {
	WEAPINVENTORY_PRIMARY = 0x0,
	WEAPINVENTORY_OFFHAND = 0x1,
	WEAPINVENTORY_ITEM = 0x2,
	WEAPINVENTORY_ALTMODE = 0x3,
	WEAPINVENTORY_MELEE = 0x4,
	WEAPINVENTORY_DWLEFTHAND = 0x5,
	WEAPINVENTORYCOUNT = 0x6,
};

enum weapFireType_t {
	WEAPON_FIRETYPE_FULLAUTO = 0x0,
	WEAPON_FIRETYPE_SINGLESHOT = 0x1,
	WEAPON_FIRETYPE_BURSTFIRE2 = 0x2,
	WEAPON_FIRETYPE_BURSTFIRE3 = 0x3,
	WEAPON_FIRETYPE_BURSTFIRE4 = 0x4,
	WEAPON_FIRETYPE_BURSTFIRE5 = 0x5,
	WEAPON_FIRETYPE_STACKED = 0x6,
	WEAPON_FIRETYPE_MINIGUN = 0x7,
	WEAPON_FIRETYPE_CHARGESHOT = 0x8,
	WEAPON_FIRETYPE_JETGUN = 0x9,
	WEAPON_FIRETYPECOUNT = 0xA,
};

enum weapClipType_t {
	WEAPON_CLIPTYPE_BOTTOM = 0x0,
	WEAPON_CLIPTYPE_TOP = 0x1,
	WEAPON_CLIPTYPE_LEFT = 0x2,
	WEAPON_CLIPTYPE_DP28 = 0x3,
	WEAPON_CLIPTYPE_PTRS = 0x4,
	WEAPON_CLIPTYPE_LMG = 0x5,
	WEAPON_CLIPTYPECOUNT = 0x6,
};

enum barrelType_t {
	BARREL_TYPE_SINGLE = 0x0,
	BARREL_TYPE_DUAL = 0x1,
	BARREL_TYPE_DUAL_ALTERNATE = 0x2,
	BARREL_TYPE_QUAD = 0x3,
	BARREL_TYPE_QUAD_ALTERNATE = 0x4,
	BARREL_TYPE_QUAD_DOUBLE_ALTERNATE = 0x5,
	BARREL_TYPE_COUNT = 0x6,
};

enum OffhandClass {
	OFFHAND_CLASS_NONE = 0x0,
	OFFHAND_CLASS_FRAG_GRENADE = 0x1,
	OFFHAND_CLASS_SMOKE_GRENADE = 0x2,
	OFFHAND_CLASS_FLASH_GRENADE = 0x3,
	OFFHAND_CLASS_GEAR = 0x4,
	OFFHAND_CLASS_SUPPLYDROP_MARKER = 0x5,
	OFFHAND_CLASS_COUNT = 0x6,
};

enum OffhandSlot {
	OFFHAND_SLOT_NONE = 0x0,
	OFFHAND_SLOT_LETHAL_GRENADE = 0x1,
	OFFHAND_SLOT_TACTICAL_GRENADE = 0x2,
	OFFHAND_SLOT_EQUIPMENT = 0x3,
	OFFHAND_SLOT_SPECIFIC_USE = 0x4,
	OFFHAND_SLOT_COUNT = 0x5,
};

enum weapStance_t {
	WEAPSTANCE_STAND = 0x0,
	WEAPSTANCE_DUCK = 0x1,
	WEAPSTANCE_PRONE = 0x2,
	WEAPSTANCE_NUM = 0x3,
};

enum activeReticleType_t {
	VEH_ACTIVE_RETICLE_NONE = 0x0,
	VEH_ACTIVE_RETICLE_PIP_ON_A_STICK = 0x1,
	VEH_ACTIVE_RETICLE_BOUNCING_DIAMOND = 0x2,
	VEH_ACTIVE_RETICLE_MISSILE_LOCK = 0x3,
	VEH_ACTIVE_RETICLE_COUNT = 0x4,
};

enum weaponIconRatioType_t {
	WEAPON_ICON_RATIO_1TO1 = 0x0,
	WEAPON_ICON_RATIO_2TO1 = 0x1,
	WEAPON_ICON_RATIO_4TO1 = 0x2,
	WEAPON_ICON_RATIO_COUNT = 0x3,
};

enum ImpactType {
	IMPACT_TYPE_NONE = 0x0,
	IMPACT_TYPE_BULLET_SMALL = 0x1,
	IMPACT_TYPE_BULLET_LARGE = 0x2,
	IMPACT_TYPE_BULLET_AP = 0x3,
	IMPACT_TYPE_BULLET_XTREME = 0x4,
	IMPACT_TYPE_SHOTGUN = 0x5,
	IMPACT_TYPE_GRENADE_BOUNCE = 0x6,
	IMPACT_TYPE_GRENADE_EXPLODE = 0x7,
	IMPACT_TYPE_RIFLE_GRENADE = 0x8,
	IMPACT_TYPE_ROCKET_EXPLODE = 0x9,
	IMPACT_TYPE_ROCKET_EXPLODE_XTREME = 0xA,
	IMPACT_TYPE_PROJECTILE_DUD = 0xB,
	IMPACT_TYPE_MORTAR_SHELL = 0xC,
	IMPACT_TYPE_TANK_SHELL = 0xD,
	IMPACT_TYPE_BOLT = 0xE,
	IMPACT_TYPE_BLADE = 0xF,
	IMPACT_TYPE_COUNT = 0x10,
};

enum GamePadButton : int {
	DPAD_UP,
	DPAD_DOWN,
	DPAD_LEFT,
	DPAD_RIGHT,
	START,
	BACK,
	LEFT_THUMB,
	RIGHT_THUMB,
	LEFT_SHOULDER,
	RIGHT_SHOULDER,
	A,
	B,
	X,
	Y,
	LEFT_TRIGGER,
	RIGHT_TRIGGER,
};

enum HitLocationIndexs : DWORD {
	HITLOC_NONE = 0,
	HITLOC_HELMET = 1,
	HITLOC_HEAD = 2,
	HITLOC_NECK = 3,
	HITLOC_TORSO_UPR = 4,
	HITLOC_TORSO_MID = 5,
	HITLOC_TORSO_LWR = 6,
	HITLOC_R_ARM_UPR = 7,
	HITLOC_L_ARM_UPR = 8,
	HITLOC_R_ARM_LWR = 9,
	HITLOC_L_ARM_LWR = 0x0A,
	HITLOC_R_HAND = 0x0B,
	HITLOC_L_HAND = 0x0C,
	HITLOC_R_LEG_UPR = 0x0D,
	HITLOC_L_LEG_UPR = 0x0E,
	HITLOC_R_LEG_LWR = 0x0F,
	HITLOC_L_LEG_LWR = 0x10,
	HITLOC_R_FOOT = 0x11,
	HITLOC_L_FOOT = 0x12,
	HITLOC_GUN = 0x13,
	HITLOC_SHIELD = 0x14,
	HITLOC_NUM = 0x15
};

enum handleCall_t {
	CALL_START = 0x1,
	CALL_EXECUTE = 0x2,
	CALL_TEMP = 0x3,
	CALL_STOP = 0x4,
	CALL_FREE = 0x5,
};

enum eWeaponCheck {
	WEAPON_STUN,
	WEAPON_FRAG,
	WEAPON_SMOKE,
	WEAPON_STICKY,
	WEAPON_SENSOR,
	WEAPON_AXE,
	WEAPON_EMP,
	WEAPON_BOUNCING_BETTY,
	WEAPON_PROXIMITY_MINE,
	WEAPON_SATCHEL_CHARGE,
	WEAPON_CLAYMORE,
	WEAPON_FLASH,
	WEAPON_TROPHY,
	WEAPON_TACTICAL_INSERTION,
	WEAPON_SCAV,
	WEAPON_SHIELD
};

enum EntityType {
	ET_GENERAL,
	ET_PLAYER,
	ET_CORPSE,
	ET_ITEM,
	ET_MISSLE,
	ET_INVISIBLE_ENTITY,
	ET_SCRIPTMOVER,
	ET_SOUND_BLEND,
	ET_FX,
	ET_LOOP_FX,
	ET_PRIMARY_LIGHT,
	ET_TURRET,
	ET_HELICOPTER,
	ET_PLANE,
	ET_VEHICLE,
	ET_VEHICLE_CORPSE,
	ET_ACTOR,
	ET_ACTOR_SPAWNER,
	ET_ACTOR_CORPSE,
	ET_STREAMER_HINT,
	ET_ZBARRIER,
	ET_EVENTS
};