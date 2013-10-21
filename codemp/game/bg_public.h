// Copyright (C) 1999-2000 Id Software, Inc.
//
// bg_public.h -- definitions shared by both the server game and client game modules

// because games can change separately from the main system version, we need a
// second version that must match between game and cgame

#ifndef __BG_PUBLIC_H__
#define __BG_PUBLIC_H__

#include "bg_weapons.h"
#include "anims.h"
#include "bg_vehicles.h"

//these two defs are shared now because we do clientside ent parsing
#define	MAX_SPAWN_VARS			64
#define	MAX_SPAWN_VARS_CHARS	4096


#define	GAME_VERSION		"sof2mp-1.02"

#define	STEPSIZE		18

#define	DEFAULT_GRAVITY		800
#define ARMOR_PROTECTION		0.55

#define	JUMP_VELOCITY		270

#define	MAX_ITEMS			256

#define	RANK_TIED_FLAG		0x4000

#define	ITEM_RADIUS			15		// item sizes are needed for client side pickup detection

#define	SCORE_NOT_PRESENT	-9999	// for the CS_SCORES[12] when only one player is present

#define DEFAULT_MINS_2		-24
#define DEFAULT_MAXS_2		40
#define CROUCH_MAXS_2		16
#define	STANDARD_VIEWHEIGHT_OFFSET -4

#define	MINS_Z				-46
#define	DEFAULT_VIEWHEIGHT	(DEFAULT_MAXS_2+STANDARD_VIEWHEIGHT_OFFSET)//26
#define CROUCH_VIEWHEIGHT	(CROUCH_MAXS_2+STANDARD_VIEWHEIGHT_OFFSET)//12
#define PRONE_VIEWHEIGHT	-22
#define	DEAD_VIEWHEIGHT		-38

#define MAX_CLIENT_SCORE_SEND 20

//
// config strings are a general means of communicating variable length strings
// from the server to all connected clients.
//

// CS_SERVERINFO and CS_SYSTEMINFO are defined in q_shared.h
#define	CS_MUSIC				2
#define	CS_MESSAGE				3		// from the map worldspawn's message field
#define	CS_MOTD					4		// g_motd string for server message of the day
#define	CS_WARMUP				5		// server time when the match will be restarted
#define	CS_SCORES1				6
#define	CS_SCORES2				7
#define CS_VOTE_TIME			8
#define CS_VOTE_STRING			9
#define	CS_VOTE_YES				10
#define	CS_VOTE_NO				11

#define CS_TEAMVOTE_TIME		12
#define CS_TEAMVOTE_STRING		14
#define	CS_TEAMVOTE_YES			16
#define	CS_TEAMVOTE_NO			18

#define	CS_GAME_VERSION			20
#define	CS_LEVEL_START_TIME		21		// so the timer only shows the current level
#define	CS_INTERMISSION			22		// when 1, fraglimit/timelimit has been hit and intermission will start in a second or two
#define CS_FLAGSTATUS			23		// string indicating flag status in CTF
#define CS_SHADERSTATE			24
#define CS_BOTINFO				25

#define	CS_ITEMS				27		// string of 0's and 1's that tell which items are present

#define CS_CLIENT_JEDIMASTER	28		// current jedi master
#define CS_CLIENT_DUELWINNER	29		// current duel round winner - needed for printing at top of scoreboard
#define CS_CLIENT_DUELISTS		30		// client numbers for both current duelists. Needed for a number of client-side things.
#define CS_CLIENT_DUELHEALTHS	31		// nmckenzie: DUEL_HEALTH.  Hopefully adding this cs is safe and good?
#define CS_GLOBAL_AMBIENT_SET	32

#define CS_AMBIENT_SET			37

#define CS_SIEGE_STATE			(CS_AMBIENT_SET+MAX_AMBIENT_SETS)
#define CS_SIEGE_OBJECTIVES		(CS_SIEGE_STATE+1)
#define CS_SIEGE_TIMEOVERRIDE	(CS_SIEGE_OBJECTIVES+1)
#define CS_SIEGE_WINTEAM		(CS_SIEGE_TIMEOVERRIDE+1)
#define CS_SIEGE_ICONS			(CS_SIEGE_WINTEAM+1)

#define	CS_MODELS				(CS_SIEGE_ICONS+1)
#define	CS_SKYBOXORG			(CS_MODELS+MAX_MODELS)		//rww - skybox info
#define	CS_SOUNDS				(CS_SKYBOXORG+1)
#define CS_ICONS				(CS_SOUNDS+MAX_SOUNDS)
#define	CS_PLAYERS				(CS_ICONS+MAX_ICONS)
/*
Ghoul2 Insert Start
*/
#define CS_G2BONES				(CS_PLAYERS+MAX_CLIENTS)
//rww - used to be CS_CHARSKINS, but I have eliminated the need for that.
/*
Ghoul2 Insert End
*/
#define CS_LOCATIONS			(CS_G2BONES+MAX_G2BONES)
#define CS_PARTICLES			(CS_LOCATIONS+MAX_LOCATIONS) 
#define CS_EFFECTS				(CS_PARTICLES+MAX_LOCATIONS)
#define	CS_LIGHT_STYLES			(CS_EFFECTS + MAX_FX)

//rwwRMG - added:
#define CS_TERRAINS				(CS_LIGHT_STYLES + (MAX_LIGHT_STYLES*3))
#define CS_BSP_MODELS			(CS_TERRAINS + MAX_TERRAINS)

#define CS_MAX					(CS_BSP_MODELS + MAX_SUB_BSP)

//SOF2 TODO
//#if (CS_MAX) > MAX_CONFIGSTRINGS
//#error overflow: (CS_MAX) > MAX_CONFIGSTRINGS
//#endif

typedef enum {
	G2_MODELPART_HEAD = 10,
	G2_MODELPART_WAIST,
	G2_MODELPART_LARM,
	G2_MODELPART_RARM,
	G2_MODELPART_RHAND,
	G2_MODELPART_LLEG,
	G2_MODELPART_RLEG
} g2ModelParts_t;

#define G2_MODEL_PART	50

#define BG_NUM_TOGGLEABLE_SURFACES 31

#define MAX_CUSTOM_SIEGE_SOUNDS 30

extern const char *bg_customSiegeSoundNames[MAX_CUSTOM_SIEGE_SOUNDS];

extern const char *bgToggleableSurfaces[BG_NUM_TOGGLEABLE_SURFACES];
extern const int bgToggleableSurfaceDebris[BG_NUM_TOGGLEABLE_SURFACES];

typedef enum
{
	BROKENLIMB_NONE = 0,
	BROKENLIMB_LARM,
	BROKENLIMB_RARM,
	NUM_BROKENLIMBS
} brokenLimb_t;

//for supplier class items
#define TOSS_DEBOUNCE_TIME				5000

typedef enum { GENDER_MALE, GENDER_FEMALE, GENDER_NEUTER } gender_t;

extern vec3_t WP_MuzzlePoint[WP_NUM_WEAPONS];

typedef enum direction_e
{
	DIR_RIGHT,
	DIR_LEFT,
	DIR_FRONT,
	DIR_BACK
} direction_t;

/*
===================================================================================

PMOVE MODULE

The pmove code takes a player_state_t and a usercmd_t and generates a new player_state_t
and some other output data.  Used for local prediction on the client game and true
movement on the server game.
===================================================================================
*/


#pragma pack(push, 1)
typedef struct animation_s {
	int		firstFrame;
	int		numFrames;
	int		loopFrames;			// 0 to numFrames
	int		frameLerp;			// msec between frames
	int		initialLerp;		// msec to get to first frame
	int		reversed;			// true if animation is reversed
	int		flipflop;			// true if animation should flipflop back to base
} animation_t;
#pragma pack(pop)

typedef struct ladder_s 
{
	vec3_t	origin;
	vec3_t	fwd;

} ladder_t;

// flip the togglebit every time an animation
// changes so a restart of the same anim can be detected
#define	ANIM_TOGGLEBIT				2048		// Note that there are 12 bits (max 4095) for animations.
#define ITEM_AUTOSWITCHBIT			(1<<31)	
#define ITEM_QUIETPICKUP			(1<<30)

typedef enum {
	PM_NORMAL,		// can accelerate and turn
	PM_NOCLIP,		// noclip movement
	PM_SPECTATOR,	// still run into walls
	PM_DEAD,		// no acceleration or turning, but free falling
	PM_FREEZE,		// stuck in place with no control
	PM_INTERMISSION,	// no movement or status bar
} pmtype_t;

typedef enum {
	WEAPON_READY, 
	WEAPON_SPAWNING,
	WEAPON_RAISING,
	WEAPON_DROPPING,
	WEAPON_RELOADING,
	WEAPON_RELOADING_ALT,
	WEAPON_FIRING,
	WEAPON_FIRING_ALT,
	WEAPON_CHARGING,
	WEAPON_CHARGING_ALT,
	WEAPON_ZOOMIN,
	WEAPON_ZOOMOUT,
} weaponstate_t;

// pmove->pm_flags
#define	PMF_DUCKED			1
#define	PMF_JUMP_HELD		2
#define PMF_ROLLING			4
#define	PMF_BACKWARDS_JUMP	8		// go into backwards land
#define	PMF_BACKWARDS_RUN	16		// coast down to backwards run
#define	PMF_TIME_LAND		32		// pm_time is time before rejump
#define	PMF_TIME_KNOCKBACK	64		// pm_time is an air-accelerate only time
#define	PMF_FIX_MINS		128		// mins have been brought up, keep tracing down to fix them
#define	PMF_TIME_WATERJUMP	256		// pm_time is waterjump
#define	PMF_RESPAWNED		512		// clear after attack and jump buttons come up
#define	PMF_USE_ITEM_HELD	1024
#define PMF_UPDATE_ANIM		2048	// The server updated the animation, the pmove should set the ghoul2 anim to match.
#define PMF_FOLLOW			4096	// spectate following another player
#define PMF_SCOREBOARD		8192	// spectate as a scoreboard
#define PMF_STUCK_TO_WALL	16384	// grabbing a wall

#define	PMF_ALL_TIMES	(PMF_TIME_WATERJUMP|PMF_TIME_LAND|PMF_TIME_KNOCKBACK)

#define	MAXTOUCH	32

typedef struct bgEntity_s
{
	entityState_t	s;
	playerState_t	*playerState;
	Vehicle_t		*m_pVehicle; //vehicle data
	void			*ghoul2; //g2 instance
	int				localAnimIndex; //index locally (game/cgame) to anim data for this skel
	vec3_t			modelScale; //needed for g2 collision

	//Data type(s) must directly correspond to the head of the gentity and centity structures
#if defined(__GNUC__) || defined(__GCC__) || defined(MINGW32) || defined(MACOS_X)
	} _bgEntity_t;
#else
	} bgEntity_t;
#endif

typedef struct {
	// state (in / out)
	playerState_t	*ps;

	// command (in)
	usercmd_t	cmd;
	int			tracemask;			// collide against these types of surfaces
	int			debugLevel;			// if set, diagnostic output will be printed
	qboolean	noFootsteps;		// if the game is setup for no footsteps by the server
	qboolean	gauntletHit;		// true if a gauntlet attack would actually hit something

	int			framecount;

	// results (out)
	int			numtouch;
	int			touchents[MAXTOUCH];

	int			useEvent;

	vec3_t		mins, maxs;			// bounding box size

	int			watertype;
	int			waterlevel;

	animation_t	*animations;

	float		xyspeed;

	// for fixed msec Pmove
	int			pmove_fixed;
	int			pmove_msec;

	// callbacks to test the world
	// these will be different functions during game and cgame
	void		(*trace)( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentMask );
	int			(*pointcontents)( const vec3_t point, int passEntityNum );

	int			weaponAnimIdx;
	char		weaponAnim[MAX_QPATH];
	char		weaponEndAnim[MAX_QPATH];
} pmove_t;


extern	pmove_t		*pm;

#define SETANIM_TORSO 1
#define SETANIM_LEGS  2
#define SETANIM_BOTH  SETANIM_TORSO|SETANIM_LEGS//3

#define SETANIM_FLAG_NORMAL		0//Only set if timer is 0
#define SETANIM_FLAG_OVERRIDE	1//Override previous
#define SETANIM_FLAG_HOLD		2//Set the new timer
#define SETANIM_FLAG_RESTART	4//Allow restarting the anim if playing the same one (weapon fires)
#define SETANIM_FLAG_HOLDLESS	8//Set the new timer


// if a full pmove isn't done on the client, you can just update the angles
void PM_UpdateViewAngles( playerState_t *ps, const usercmd_t *cmd );
void Pmove (pmove_t *pmove);


//===================================================================================


// player_state->stats[] indexes
// NOTE: may not have more than 16
typedef enum {
	STAT_HEALTH,
	STAT_WEAPONS,					// 16 bit fields
	STAT_ARMOR,				
	STAT_DEAD_YAW,					// look this direction when dead (FIXME: get rid of?)
	STAT_CLIENTS_READY,				// bit mask of clients wishing to exit the intermission (FIXME: configstring?)
	STAT_FROZEN,
	STAT_GOGGLES,					// Which visual enhancing device they have
	STAT_GAMETYPE_ITEMS,			// Which gametype items they have	
	STAT_SEED,						// seed used to keep weapon firing in sync
	STAT_OUTFIT_GRENADE,			// indicates which greande is chosen in the outfitting
	STAT_USEICON,					// icon to display when able to use a trigger or item
	STAT_USETIME,					// elased time for using 
	STAT_USETIME_MAX,				// total time required to use
	STAT_USEWEAPONDROP,				// value to drop weapon out of view when using
} statIndex_t;


// player_state->persistant[] indexes
// these fields are the only part of player_state that isn't
// cleared on respawn
// NOTE: may not have more than 16
typedef enum {
	PERS_SCORE,						// !!! MUST NOT CHANGE, SERVER AND GAME BOTH REFERENCE !!!
	PERS_RANK,						// player rank or team rank
	PERS_TEAM,						// player team
	PERS_SPAWN_COUNT,				// incremented every respawn
	PERS_PLAYEREVENTS,				// 16 bits that can be flipped for events
	PERS_ATTACKER,					// clientnum of last damage inflicter

	PERS_RED_SCORE,					// Blue team score
	PERS_BLUE_SCORE,				// red team score

	PERS_RED_ALIVE_COUNT,			// number of alive people on the red team
	PERS_BLUE_ALIVE_COUNT,			// number of alive people on the blue team
} persEnum_t;

typedef enum
{
	GOGGLES_NONE,
	GOGGLES_NIGHTVISION,
	GOGGLES_INFRARED,
	GOGGLES_MAX

} goggleType_t;

// entityState_t->eFlags
#define	EF_G2ANIMATING			(1<<0)		//perform g2 bone anims based on torsoAnim and legsAnim, works for ET_GENERAL -rww
#define	EF_DEAD					(1<<1)		// don't draw a foe marker over players with EF_DEAD
//#define	EF_BOUNCE_SHRAPNEL		(1<<2)		// special shrapnel flag
//do not use eflags for server-only things, it wastes bandwidth -rww
#define EF_RADAROBJECT			(1<<2)		// display on team radar

#define	EF_TELEPORT_BIT			(1<<3)		// toggled every time the origin abruptly changes

#define	EF_SHADER_ANIM			(1<<4)		// Animating shader (by s.frame)

#define EF_PLAYER_EVENT			(1<<5)
//#define	EF_BOUNCE				(1<<5)		// for missiles
//#define	EF_BOUNCE_HALF			(1<<6)		// for missiles
//these aren't even referenced in bg or client code and do not need to be eFlags, so I
//am using these flags for rag stuff -rww

#define EF_RAG					(1<<6)		//ragdoll him even if he's alive


#define EF_PERMANENT			(1<<7)		// rww - I am claiming this. (for permanent entities)

#define	EF_NODRAW				(1<<8)		// may have an event, but no model (unspawned items)
#define	EF_FIRING				(1<<9)		// for lightning gun
#define EF_ALT_FIRING			(1<<10)		// for alt-fires, mostly for lightning guns though
#define	EF_JETPACK_ACTIVE		(1<<11)		//jetpack is activated

#define EF_NOT_USED_1			(1<<12)		// not used

#define	EF_TALK					(1<<13)		// draw a talk balloon
#define	EF_CONNECTION			(1<<14)		// draw a connection trouble sprite
#define	EF_NOT_USED_6			(1<<15)		// not used

#define	EF_NOT_USED_2			(1<<16)		// not used
#define	EF_NOT_USED_3			(1<<17)		// not used
#define	EF_NOT_USED_4			(1<<18)		// not used

#define	EF_BODYPUSH				(1<<19)		//rww - claiming this for fullbody push effect

#define	EF_DOUBLE_AMMO			(1<<20)		// Hacky way to get around ammo max
#define EF_SEEKERDRONE			(1<<21)		// show seeker drone floating around head
#define EF_MISSILE_STICK		(1<<22)		// missiles that stick to the wall.
#define EF_ITEMPLACEHOLDER		(1<<23)		// item effect
#define EF_SOUNDTRACKER			(1<<24)		// sound position needs to be updated in relation to another entity
#define EF_DROPPEDWEAPON		(1<<25)		// it's a dropped weapon
#define EF_DISINTEGRATION		(1<<26)		// being disintegrated by the disruptor
#define EF_INVULNERABLE			(1<<27)		// just spawned in or whatever, so is protected

#define EF_CLIENTSMOOTH			(1<<28)		// standard lerporigin smooth override on client

#define EF_JETPACK				(1<<29)		//rww - wearing a jetpack
#define EF_JETPACK_FLAMING		(1<<30)		//rww - jetpack fire effect

#define	EF_NOT_USED_5			(1<<31)		// not used

//These new EF2_??? flags were added for NPCs, they really should not be used often.
//NOTE: we only allow 10 of these!
#define	EF2_HELD_BY_MONSTER		(1<<0)		// Being held by something, like a Rancor or a Wampa
#define	EF2_USE_ALT_ANIM		(1<<1)		// For certain special runs/stands for creatures like the Rancor and Wampa whose runs/stands are conditional
#define	EF2_ALERTED				(1<<2)		// For certain special anims, for Rancor: means you've had an enemy, so use the more alert stand
#define	EF2_GENERIC_NPC_FLAG	(1<<3)		// So far, used for Rancor...
#define	EF2_FLYING				(1<<4)		// Flying FIXME: only used on NPCs doesn't *really* have to be passed over, does it?
#define	EF2_BRACKET_ENTITY		(1<<6)		// Draw as bracketed
#define	EF2_SHIP_DEATH			(1<<7)		// "died in ship" mode
#define	EF2_NOT_USED_1			(1<<8)		// not used


typedef enum {
	EFFECT_NONE = 0,
	EFFECT_SMOKE,
	EFFECT_EXPLOSION,
	EFFECT_EXPLOSION_PAS,
	EFFECT_SPARK_EXPLOSION,
	EFFECT_EXPLOSION_TRIPMINE,
	EFFECT_EXPLOSION_DETPACK,
	EFFECT_EXPLOSION_FLECHETTE,
	EFFECT_STUNHIT,
	EFFECT_EXPLOSION_DEMP2ALT,
	EFFECT_EXPLOSION_TURRET,
	EFFECT_SPARKS,
	EFFECT_WATER_SPLASH,
	EFFECT_ACID_SPLASH,
	EFFECT_LAVA_SPLASH,
	EFFECT_LANDING_MUD,
	EFFECT_LANDING_SAND,
	EFFECT_LANDING_DIRT,
	EFFECT_LANDING_SNOW,
	EFFECT_LANDING_GRAVEL,
	EFFECT_MAX
} effectTypes_t;

// NOTE: may not have more than 16
typedef enum {
	PW_NONE,

	#ifdef BASE_COMPAT
		PW_QUAD,
		PW_BATTLESUIT,
	#endif // BASE_COMPAT

	PW_PULL,

	PW_REDFLAG,
	PW_BLUEFLAG,
	PW_NEUTRALFLAG,

	PW_SHIELDHIT,

	PW_SPEEDBURST,
	PW_DISINT_4,
	PW_SPEED,
	PW_CLOAKED,
	PW_FORCE_ENLIGHTENED_LIGHT,
	PW_FORCE_ENLIGHTENED_DARK,
	PW_FORCE_BOON,
	PW_YSALAMIRI,

	PW_NUM_POWERUPS

} powerup_t;

typedef enum {
	HI_NONE,

	HI_SEEKER,
	HI_SHIELD,
	HI_MEDPAC,
	HI_MEDPAC_BIG,
	HI_BINOCULARS,
	HI_SENTRY_GUN,
	HI_JETPACK,

	HI_HEALTHDISP,
	HI_AMMODISP,
	HI_EWEB,
	HI_CLOAK,

	HI_NUM_HOLDABLE
} holdable_t;


typedef enum {
	CTFMESSAGE_FRAGGED_FLAG_CARRIER,
	CTFMESSAGE_FLAG_RETURNED,
	CTFMESSAGE_PLAYER_RETURNED_FLAG,
	CTFMESSAGE_PLAYER_CAPTURED_FLAG,
	CTFMESSAGE_PLAYER_GOT_FLAG
} ctfMsg_t;

// reward sounds (stored in ps->persistant[PERS_PLAYEREVENTS])
#define	PLAYEREVENT_DENIEDREWARD		0x0001
#define	PLAYEREVENT_GAUNTLETREWARD		0x0002

// entityState_t->event values
// entity events are for effects that take place reletive
// to an existing entities origin.  Very network efficient.

// two bits at the top of the entityState->event field
// will be incremented with each change in the event so
// that an identical event started twice in a row can
// be distinguished.  And off the value with ~EV_EVENT_BITS
// to retrieve the actual event number
#define	EV_EVENT_BIT1		0x00000100
#define	EV_EVENT_BIT2		0x00000200
#define	EV_EVENT_BITS		(EV_EVENT_BIT1|EV_EVENT_BIT2)

#define	EVENT_VALID_MSEC	300

typedef enum
{
	PDSOUND_NONE,
	PDSOUND_PROTECTHIT,
	PDSOUND_PROTECT,
	PDSOUND_ABSORBHIT,
	PDSOUND_ABSORB,
	PDSOUND_FORCEJUMP,
	PDSOUND_FORCEGRIP
} pdSounds_t;

typedef enum {
	EV_NONE,

	EV_FOOTSTEP,
	EV_FOOTWADE,
	EV_SWIM,

	EV_STEP_4,
	EV_STEP_8,
	EV_STEP_12,
	EV_STEP_16,

	EV_FALL_SHORT,
	EV_FALL_MEDIUM,
	EV_FALL_FAR,

	EV_JUMP,
	EV_WATER_FOOTSTEP,
	EV_WATER_TOUCH,	// foot touches
	EV_WATER_LAND,  // landed in water
	EV_WATER_CLEAR,

	EV_ITEM_PICKUP,			// normal item pickups are predictable
	EV_ITEM_PICKUP_QUIET,	// quiet pickup

	EV_NOAMMO,
	EV_CHANGE_WEAPON,
	EV_CHANGE_WEAPON_CANCELLED,
	EV_READY_WEAPON,
	EV_FIRE_WEAPON,
	EV_ALT_FIRE,

	EV_USE,			// +Use key

	EV_ITEM_RESPAWN,
	EV_ITEM_POP,
	EV_PLAYER_TELEPORT_IN,
	EV_PLAYER_TELEPORT_OUT,

	EV_GRENADE_BOUNCE,		// eventParm will be the soundindex

	EV_PLAY_EFFECT,

	EV_GENERAL_SOUND,
	EV_GLOBAL_SOUND,		// no attenuation
	EV_ENTITY_SOUND,

	EV_GLASS_SHATTER,

	EV_MISSILE_HIT,
	EV_MISSILE_MISS,

	EV_BULLET_HIT_WALL,
	EV_BULLET_HIT_FLESH,
	EV_BULLET,				// otherEntity is the shooter

	EV_EXPLOSION_HIT_FLESH,

	EV_PAIN,
	EV_PAIN_WATER,
	EV_OBITUARY,

	EV_DESTROY_GHOUL2_INSTANCE,

	EV_WEAPON_CHARGE,
	EV_WEAPON_CHARGE_ALT,

	EV_DEBUG_LINE,
	EV_TESTLINE,
	EV_STOPLOOPINGSOUND,

	EV_BODY_QUEUE_COPY,
	EV_BOTWAYPOINT,

	// Procedural gore event.
	EV_PROC_GORE,
	
	EV_GAMETYPE_RESTART,			// gametype restarting
	EV_GAME_OVER,					// game is over

	EV_GOGGLES,						// goggles turning on/off

	EV_WEAPON_CALLBACK,
	
} entity_event_t;			// There is a maximum of 256 events (8 bits transmission, 2 high bits for uniqueness)


typedef enum {
	GTS_RED_CAPTURE,
	GTS_BLUE_CAPTURE,
	GTS_RED_RETURN,
	GTS_BLUE_RETURN,
	GTS_RED_TAKEN,
	GTS_BLUE_TAKEN,
	GTS_REDTEAM_SCORED,
	GTS_BLUETEAM_SCORED,
	GTS_REDTEAM_TOOK_LEAD,
	GTS_BLUETEAM_TOOK_LEAD,
	GTS_TEAMS_ARE_TIED
} global_team_sound_t;



typedef enum {
	TEAM_FREE,
	TEAM_RED,
	TEAM_BLUE,
	TEAM_SPECTATOR,

	TEAM_NUM_TEAMS
} team_t;


// Time between location updates
#define TEAM_LOCATION_UPDATE_TIME		1000

// How many players on the overlay
#define TEAM_MAXOVERLAY		32


//---------------------------------------------------------

// gitem_t->type
typedef enum {
	IT_BAD,
	IT_WEAPON,		// Weapon item
	IT_AMMO,		// Ammo item
	IT_ARMOR,		// Armor item
	IT_HEALTH,		// Healh item
	IT_GAMETYPE,	// Custom gametype related item
	IT_BACKPACK,	// replenish backpack item
	IT_PASSIVE,		// Passive items
} itemType_t;

#define MAX_ITEM_MODELS 4

typedef struct gitem_s {
	char		*classname;	// spawning name
	char		*pickup_sound;
	char		*world_model[MAX_ITEM_MODELS];

	char		*icon;
	char		*render;
	char		*pickup_prefix;					// an, some, a, the, etc..
	char		*pickup_name;					// for printing on pickup

	int			quantity;						// for ammo how much
	itemType_t  giType;							// IT_* flags

	int			giTag;

	char		*precaches;						// string of all models and images this item will use
	char		*sounds;						// string of all sounds this item will use

	int			outfittingGroup;
} gitem_t;

// included in both the game dll and the client

extern	gitem_t	bg_itemlist[];
extern	int		bg_numItems;

float vectoyaw( const vec3_t vec );

gitem_t	*BG_FindItem( const char *classname );
gitem_t	*BG_FindItemForWeapon( weapon_t weapon );
gitem_t	*BG_FindItemForPowerup( powerup_t pw );
gitem_t	*BG_FindItemForHoldable( holdable_t pw );
#define	ITEM_INDEX(x) ((x)-bg_itemlist)

qboolean	BG_CanItemBeGrabbed( int gametype, const entityState_t *ent, const playerState_t *ps );



// g_dmflags->integer flags
#define	DF_NO_FALLING			8
#define DF_FIXED_FOV			16
#define	DF_NO_FOOTSTEPS			32

//rwwRMG - added in CONTENTS_TERRAIN
// content masks
#define	MASK_ALL				(-1)
#define	MASK_SOLID				(CONTENTS_SOLID|CONTENTS_TERRAIN)
#define	MASK_PLAYERSOLID		(CONTENTS_SOLID|CONTENTS_TERRAIN|CONTENTS_PLAYERCLIP|CONTENTS_BODY|CONTENTS_SHOTCLIP)
#define	MASK_DEADSOLID			(CONTENTS_SOLID|CONTENTS_TERRAIN|CONTENTS_PLAYERCLIP)
#define	MASK_WATER				(CONTENTS_WATER)
#define	MASK_OPAQUE				(CONTENTS_SOLID|CONTENTS_TERRAIN|CONTENTS_SLIME|CONTENTS_LAVA)
#define	MASK_SHOT				(CONTENTS_SOLID|CONTENTS_TERRAIN|CONTENTS_BODY|CONTENTS_SHOTCLIP)


// ET_FX States (stored in modelindex2)

#define	FX_STATE_OFF			0
#define FX_STATE_ONE_SHOT		1
#define FX_STATE_ONE_SHOT_LIMIT	10
#define FX_STATE_CONTINUOUS		20

//
// entityState_t->eType
//
typedef enum {
	ET_GENERAL,
	ET_PLAYER,
	ET_ITEM,
	ET_MISSILE,
	ET_MOVER,
	ET_BEAM,
	ET_PORTAL,
	ET_SPEAKER,
	ET_PUSH_TRIGGER,
	ET_TELEPORT_TRIGGER,
	ET_INVISIBLE,
	ET_GRAPPLE,				// grapple hooked on wall
	ET_BODY,
	ET_DAMAGEAREA,
	ET_TERRAIN,

	ET_DEBUG_CYLINDER,

	ET_GAMETYPE_TRIGGER,

	ET_WALL,

	ET_EVENTS				// any of the EV_* events can be added freestanding
							// by setting eType to ET_EVENTS + eventNum
							// this avoids having to set eFlags and eventNum
} entityType_t;

// Okay, here lies the much-dreaded Pat-created FSM movement chart...  Heretic II strikes again!
// Why am I inflicting this on you?  Well, it's better than hardcoded states.
// Ideally this will be replaced with an external file or more sophisticated move-picker
// once the game gets out of prototype stage.

// rww - Moved all this to bg_public so that we can access the saberMoveData stuff on the cgame
// which is currently used for determining if a saber trail should be rendered in a given frame
#ifdef LS_NONE
#undef LS_NONE
#endif
bgEntity_t *PM_BGEntForNum( int num );
qboolean BG_KnockDownable(playerState_t *ps);


#ifdef __LCC__ //can't inline it then, it is declared over in bg_misc in this case
void BG_GiveMeVectorFromMatrix(mdxaBone_t *boltMatrix, int flags, vec3_t vec);
#else
// given a boltmatrix, return in vec a normalised vector for the axis requested in flags
static ID_INLINE void BG_GiveMeVectorFromMatrix(mdxaBone_t *boltMatrix, int flags, vec3_t vec)
{
	switch (flags)
	{
	case ORIGIN:
		vec[0] = boltMatrix->matrix[0][3];
		vec[1] = boltMatrix->matrix[1][3];
		vec[2] = boltMatrix->matrix[2][3];
		break;
	case POSITIVE_Y:
		vec[0] = boltMatrix->matrix[0][1];
		vec[1] = boltMatrix->matrix[1][1];
		vec[2] = boltMatrix->matrix[2][1];
 		break;
	case POSITIVE_X:
		vec[0] = boltMatrix->matrix[0][0];
		vec[1] = boltMatrix->matrix[1][0];
		vec[2] = boltMatrix->matrix[2][0];
		break;
	case POSITIVE_Z:
		vec[0] = boltMatrix->matrix[0][2];
		vec[1] = boltMatrix->matrix[1][2];
		vec[2] = boltMatrix->matrix[2][2];
		break;
	case NEGATIVE_Y:
		vec[0] = -boltMatrix->matrix[0][1];
		vec[1] = -boltMatrix->matrix[1][1];
		vec[2] = -boltMatrix->matrix[2][1];
		break;
	case NEGATIVE_X:
		vec[0] = -boltMatrix->matrix[0][0];
		vec[1] = -boltMatrix->matrix[1][0];
		vec[2] = -boltMatrix->matrix[2][0];
		break;
	case NEGATIVE_Z:
		vec[0] = -boltMatrix->matrix[0][2];
		vec[1] = -boltMatrix->matrix[1][2];
		vec[2] = -boltMatrix->matrix[2][2];
		break;
	}
}
#endif


void BG_IK_MoveArm(void *ghoul2, int lHandBolt, int time, entityState_t *ent, int basePose, vec3_t desiredPos, qboolean *ikInProgress,
					 vec3_t origin, vec3_t angles, vec3_t scale, int blendTime, qboolean forceHalt);

void BG_G2PlayerAngles(void *ghoul2, int motionBolt, entityState_t *cent, int time, vec3_t cent_lerpOrigin,
					   vec3_t cent_lerpAngles, vec3_t legs[3], vec3_t legsAngles, qboolean *tYawing,
					   qboolean *tPitching, qboolean *lYawing, float *tYawAngle, float *tPitchAngle,
					   float *lYawAngle, int frametime, vec3_t turAngles, vec3_t modelScale, int ciLegs,
					   int ciTorso, int *corrTime, vec3_t lookAngles, vec3_t lastHeadAngles, int lookTime,
					   entityState_t *emplaced, int *crazySmoothFactor);
void BG_G2ATSTAngles(void *ghoul2, int time, vec3_t cent_lerpAngles );

//BG anim utility functions:

int BG_AnimLength( int index, animNumber_t anim );

qboolean BG_InSpecialJump( int anim );
qboolean BG_InReboundJump( int anim );
qboolean BG_InReboundHold( int anim );
qboolean BG_InReboundRelease( int anim );
qboolean BG_InBackFlip( int anim );
qboolean BG_DirectFlippingAnim( int anim );
qboolean BG_KickMove( int move );
qboolean BG_FlippingAnim( int anim );
qboolean BG_KickingAnim( int anim );
int BG_InGrappleMove(int anim);
int BG_BrokenParryForAttack( int move );
int BG_BrokenParryForParry( int move );
int BG_KnockawayForParry( int move );
qboolean BG_InRoll( playerState_t *ps, int anim );
qboolean BG_InDeathAnim( int anim );

void	BG_EvaluateTrajectory( const trajectory_t *tr, int atTime, vec3_t result );
void	BG_EvaluateTrajectoryDelta( const trajectory_t *tr, int atTime, vec3_t result );

void	BG_AddPredictableEventToPlayerstate( int newEvent, int eventParm, playerState_t *ps );

void	BG_TouchJumpPad( playerState_t *ps, entityState_t *jumppad );

void	BG_PlayerStateToEntityState( playerState_t *ps, entityState_t *s, qboolean snap );
void	BG_PlayerStateToEntityStateExtraPolate( playerState_t *ps, entityState_t *s, int time, qboolean snap );

qboolean	BG_PlayerTouchesItem( playerState_t *ps, entityState_t *item, int atTime );

void	BG_InitAnimsets(void);
void	BG_ClearAnimsets(void);
int		BG_ParseAnimationFile(const char *filename, animation_t *animSet, qboolean isHumanoid);
#ifndef QAGAME
int		BG_ParseAnimationEvtFile( const char *as_filename, int animFileIndex, int eventFileIndex );
#endif

qboolean BG_HasAnimation(int animIndex, int animation);
int		BG_PickAnim( int animIndex, int minAnim, int maxAnim );

int BG_GetItemIndexByTag(int tag, int type);

qboolean BG_IsItemSelectable(playerState_t *ps, int item);

qboolean BG_HasYsalamiri(int gametype, playerState_t *ps);
qboolean BG_CanUseFPNow(int gametype, playerState_t *ps, int time);

void *BG_Alloc ( int size );
void *BG_AllocUnaligned ( int size );
void *BG_TempAlloc( int size );
void BG_TempFree( int size );
char *BG_StringAlloc ( const char *source );
qboolean BG_OutOfMemory ( void );

extern void BG_AttachToRancor( void *ghoul2,float rancYaw,vec3_t rancOrigin,int time,qhandle_t *modelList,vec3_t modelScale,qboolean inMouth,vec3_t out_origin,vec3_t out_angles,vec3_t out_axis[3] );
void BG_ClearRocketLock( playerState_t *ps );

extern int WeaponReadyAnim[WP_NUM_WEAPONS];
extern int WeaponAttackAnim[WP_NUM_WEAPONS];


#define ARENAS_PER_TIER		4
#define MAX_ARENAS			1024
#define	MAX_ARENAS_TEXT		8192

#define MAX_BOTS			1024
#define MAX_BOTS_TEXT		8192


#endif //__BG_PUBLIC_H__
