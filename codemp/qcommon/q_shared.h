#pragma once

// Copyright (C) 1999-2000 Id Software, Inc.
//


// q_shared.h -- included first by ALL program modules.
// A user mod should never modify this file

#define PRODUCT_NAME			"sof2mp"

#define CLIENT_WINDOW_TITLE "SoF2 MP"
#define CLIENT_CONSOLE_TITLE "Soldier of Fortune 2 Console"

//NOTENOTE: Only change this to re-point ICARUS to a new script directory
#define Q3_SCRIPT_DIR	"scripts"

#define MAX_TEAMNAME 32

#define BASE_COMPAT // some unused and leftover code has been stripped out, but this breaks compatibility
					//	between base<->modbase clients and servers (mismatching events, powerups, etc)
					// leave this defined to ensure compatibility

//#define USE_WIDESCREEN // Adjust fov for widescreen aspect ratio

#include "qcommon/disablewarnings.h"

#include "game/teams.h" //npc team stuff

#define MAX_WORLD_COORD		( 64 * 1024 )
#define MIN_WORLD_COORD		( -64 * 1024 )
#define WORLD_SIZE			( MAX_WORLD_COORD - MIN_WORLD_COORD )

//Pointer safety utilities
#define VALID( a )		( a != NULL )
#define	VALIDATE( a )	( assert( a ) )

#define	VALIDATEV( a )	if ( a == NULL ) {	assert(0);	return;			}
#define	VALIDATEB( a )	if ( a == NULL ) {	assert(0);	return qfalse;	}
#define VALIDATEP( a )	if ( a == NULL ) {	assert(0);	return NULL;	}

#define VALIDSTRING( a )	( ( a != 0 ) && ( a[0] != 0 ) )
#define VALIDENT( e )		( ( e != 0 ) && ( (e)->inuse ) )

//JAC: Added
#define ARRAY_LEN( x ) ( sizeof( x ) / sizeof( *(x) ) )
#define STRING( a ) #a
#define XSTRING( a ) STRING( a )

/*
#define G2_EHNANCEMENTS

#ifdef G2_EHNANCEMENTS
//these two will probably explode if they're defined independant of one another.
//rww - RAGDOLL_BEGIN
#define JK2_RAGDOLL
//rww - RAGDOLL_END
//rww - Bone cache for multiplayer base.
#define MP_BONECACHE
#endif
*/

#ifndef FINAL_BUILD
	//RAZFIXME: may want to enable timing and leak checking again. requires G2API changes.
//	#define G2_PERFORMANCE_ANALYSIS
//	#define _FULL_G2_LEAK_CHECKING
//	extern int g_Ghoul2Allocations;
//	extern int g_G2ServerAlloc;
//	extern int g_G2ClientAlloc;
//	extern int g_G2AllocServer;
#endif

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <limits.h>

//Ignore __attribute__ on non-gcc platforms
#if !defined(__GNUC__) && !defined(__attribute__)
	#define __attribute__(x)
#endif

#if defined(__GNUC__)
	#define UNUSED_VAR __attribute__((unused))
#else
	#define UNUSED_VAR
#endif

#if (defined _MSC_VER)
	#define Q_EXPORT __declspec(dllexport)
#elif (defined __SUNPRO_C)
	#define Q_EXPORT __global
#elif ((__GNUC__ >= 3) && (!__EMX__) && (!sun))
	#define Q_EXPORT __attribute__((visibility("default")))
#else
	#define Q_EXPORT
#endif

#if defined(__linux__) && !defined(__GCC__)
#define Q_EXPORT_C extern "C"
#else
#define Q_EXPORT_C
#endif

// this is the define for determining if we have an asm version of a C function
#if (defined(_M_IX86) || defined(__i386__)) && !defined(__sun__) && !defined(__LCC__)
	#define id386	1
#else
	#define id386	0
#endif

#if (defined(powerc) || defined(powerpc) || defined(ppc) || defined(__ppc) || defined(__ppc__)) && !defined(C_ONLY)
	#define idppc	1
#else
	#define idppc	0
#endif

// the mac compiler can't handle >32k of locals, so we
// just waste space and make big arrays static...
#define MAC_STATIC //RAZFIXME

short ShortSwap( short l );
int LongSwap( int l );
float FloatSwap( const float *f );


// for windows fastcall option
#define QDECL
#define QCALL

// Win64
#if defined(_WIN64) || defined(__WIN64__)

	#define idx64

	#undef QDECL
	#define QDECL __cdecl

	#undef QCALL
	#define QCALL __stdcall

	#if defined(_MSC_VER)
		#define OS_STRING "win_msvc64"
	#elif defined(__MINGW64__)
		#define OS_STRING "win_mingw64"
	#endif

	#define QINLINE __inline
	#define PATH_SEP '\\'

	#if defined(__WIN64__)
		#define ARCH_STRING "x64"
	#elif defined(_M_ALPHA)
		#define ARCH_STRING "AXP"
	#endif

	#define Q3_LITTLE_ENDIAN

	#define DLL_EXT ".dll"
#endif

// Win32
#ifdef WIN32

	#undef QDECL
	#define	QDECL __cdecl

	#undef QCALL
	#define QCALL __stdcall

	#if defined(_MSC_VER)
		#define OS_STRING "win_msvc"
	#elif defined(__MINGW32__)
		#define OS_STRING "win_mingw"
	#endif

	#define ID_INLINE __inline
	#define PATH_SEP '\\'

	#if defined(_M_IX86) || defined(__i386__)
		#define ARCH_STRING "x86"
	#elif defined _M_ALPHA
		#define ARCH_STRING "AXP"
	#endif

	#define Q3_LITTLE_ENDIAN

	#define DLL_EXT ".dll"

#endif


// ================================================================
//
// MAC OS X DEFINES
//
// ================================================================

#ifdef MACOS_X

	#include <sys/mman.h>

	#define __cdecl
	#define __declspec(x)
	#define stricmp strcasecmp
	#define ID_INLINE /*inline*/ 

	#ifdef __ppc__
		#define CPUSTRING "MacOSX-ppc"
	#elif defined __i386__
		#define CPUSTRING "MacOSX-i386"
	#else
		#define CPUSTRING "MacOSX-other"
	#endif

	#define	PATH_SEP	'/'

	#define __rlwimi(out, in, shift, maskBegin, maskEnd) asm("rlwimi %0,%1,%2,%3,%4" : "=r" (out) : "r" (in), "i" (shift), "i" (maskBegin), "i" (maskEnd))
	#define __dcbt(addr, offset) asm("dcbt %0,%1" : : "b" (addr), "r" (offset))

	static inline unsigned int __lwbrx(register void *addr, register int offset) {
		register unsigned int word;

		asm("lwbrx %0,%2,%1" : "=r" (word) : "r" (addr), "b" (offset));
		return word;
	}

	static inline unsigned short __lhbrx(register void *addr, register int offset) {
		register unsigned short halfword;

		asm("lhbrx %0,%2,%1" : "=r" (halfword) : "r" (addr), "b" (offset));
		return halfword;
	}

	static inline float __fctiw(register float f) {
		register float fi;

		asm("fctiw %0,%1" : "=f" (fi) : "f" (f));
		return fi;
	}

    #ifdef __ppc__
        #define LittleShort(x) ShortSwap(x)
        #define LittleLong(x) LongSwap(x)
        #define LittleFloat(x) FloatSwap(&x)
        #define BigShort
        #define BigLong
        #define BigFloat
    #elif defined __i386__
        #define LittleShort
        #define LittleLong
        #define LittleFloat
        #define BigShort(x) ShortSwap(x)
        #define BigLong(x) LongSwap(x)
        #define BigFloat(x) FloatSwap(&x)
    #endif

#endif // MACOS_X


// ================================================================
//
// MAC DEFINES
//
// ================================================================

#ifdef __MACOS__

	#include <MacTypes.h>
	#define ID_INLINE inline 

	#define	CPUSTRING "MacOS-PPC"

	#define	PATH_SEP ':'

	void Sys_PumpEvents( void );

	#define BigShort
	static inline short LittleShort( short l ) { return ShortSwap( l ); }
	#define BigLong
	static inline int LittleLong( int l ) { return LongSwap( l ); }
	#define BigFloat
	static inline float LittleFloat( const float l ) { return FloatSwap( &l ); }

#endif // __MACOS__


// ================================================================
//
// LINUX DEFINES
//
// ================================================================

#ifdef __linux__

	#include <sys/mman.h>
	#include <unistd.h>

	// bk001205 - from Makefile
	#define stricmp strcasecmp

	#define ID_INLINE /*inline*/

	#define	PATH_SEP '/'
	#define RAND_MAX 2147483647

	#if defined(__linux__)
		#define OS_STRING "linux"
	#else
		#define OS_STRING "kFreeBSD"
	#endif

	#ifdef __clang__
		#define QINLINE static inline
	#else
		#define QINLINE inline
	#endif

	#define PATH_SEP '/'

	#if defined(__i386__)
		#define ARCH_STRING "i386"
	#elif defined(__x86_64__)
		#define idx64
		#define ARCH_STRING "x86_64"
	#elif defined(__powerpc64__)
		#define ARCH_STRING "ppc64"
	#elif defined(__powerpc__)
		#define ARCH_STRING "ppc"
	#elif defined(__s390__)
		#define ARCH_STRING "s390"
	#elif defined(__s390x__)
		#define ARCH_STRING "s390x"
	#elif defined(__ia64__)
		#define ARCH_STRING "ia64"
	#elif defined(__alpha__)
		#define ARCH_STRING "alpha"
	#elif defined(__sparc__)
		#define ARCH_STRING "sparc"
	#elif defined(__arm__)
		#define ARCH_STRING "arm"
	#elif defined(__cris__)
		#define ARCH_STRING "cris"
	#elif defined(__hppa__)
		#define ARCH_STRING "hppa"
	#elif defined(__mips__)
		#define ARCH_STRING "mips"
	#elif defined(__sh__)
		#define ARCH_STRING "sh"
	#endif

	#if __FLOAT_WORD_ORDER == __BIG_ENDIAN
		#define Q3_BIG_ENDIAN
	#else
		#define Q3_LITTLE_ENDIAN
	#endif

	#define DLL_EXT ".so"

#endif

	// BSD
#if defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)

	#include <sys/types.h>
	#include <machine/endian.h>
	
	#ifndef __BSD__
		#define __BSD__
	#endif
	
	#if defined(__FreeBSD__)
		#define OS_STRING "freebsd"
	#elif defined(__OpenBSD__)
		#define OS_STRING "openbsd"
	#elif defined(__NetBSD__)
		#define OS_STRING "netbsd"
	#endif
	
	#define QINLINE inline
	#define PATH_SEP '/'
	
	#if defined(__i386__)
		#define ARCH_STRING "i386"
	#elif defined(__amd64__)
		#define idx64
		#define ARCH_STRING "amd64"
	#elif defined(__axp__)
		#define ARCH_STRING "alpha"
	#endif
	
	#if BYTE_ORDER == BIG_ENDIAN
		#define Q3_BIG_ENDIAN
	#else
		#define Q3_LITTLE_ENDIAN
	#endif
	
	#define DLL_EXT ".so"

#endif
// catch missing defines in above blocks
#if !defined(OS_STRING)
	#error "Operating system not supported"
#endif
#if !defined(ARCH_STRING)
	#error "Architecture not supported"
#endif
#if !defined(ID_INLINE)
	#error "ID_INLINE not defined"
#endif
#if !defined(PATH_SEP)
	#error "PATH_SEP not defined"
#endif
#if !defined(DLL_EXT)
	#error "DLL_EXT not defined"
#endif


// endianness
void CopyShortSwap( void *dest, void *src );
void CopyLongSwap( void *dest, void *src );
short ShortSwap( short l );
int LongSwap( int l );
float FloatSwap( const float *f );

#if defined(Q3_BIG_ENDIAN) && defined(Q3_LITTLE_ENDIAN)
	#error "Endianness defined as both big and little"
#elif defined(Q3_BIG_ENDIAN)
	#define CopyLittleShort( dest, src )	CopyShortSwap( dest, src )
	#define CopyLittleLong( dest, src )		CopyLongSwap( dest, src )
	#define LittleShort( x )				ShortSwap( x )
	#define LittleLong( x )					LongSwap( x )
	#define LittleFloat( x )				FloatSwap( &x )
	#define BigShort
	#define BigLong
	#define BigFloat
#elif defined( Q3_LITTLE_ENDIAN )
	#define CopyLittleShort( dest, src )	Com_Memcpy(dest, src, 2)
	#define CopyLittleLong( dest, src )		Com_Memcpy(dest, src, 4)
	#define LittleShort
	#define LittleLong
	#define LittleFloat
	#define BigShort( x )					ShortSwap( x )
	#define BigLong( x )					LongSwap( x )
	#define BigFloat( x )					FloatSwap( &x )
#else
	#error "Endianness not defined"
#endif


// platform string
#if defined(NDEBUG)
	#define PLATFORM_STRING OS_STRING "-" ARCH_STRING
#else
	#define PLATFORM_STRING OS_STRING "-" ARCH_STRING "-debug"
#endif

// ================================================================
// TYPE DEFINITIONS
// ================================================================

typedef unsigned char 		byte;
typedef unsigned short		word;
typedef unsigned long		ulong;

typedef enum qboolean_e { qfalse=0, qtrue } qboolean;

#ifndef min
	#define min(x,y) ((x)<(y)?(x):(y))
#endif
#ifndef max
	#define max(x,y) ((x)>(y)?(x):(y))
#endif

typedef int		qhandle_t;
typedef int		thandle_t; //rwwRMG - inserted
typedef int		fxHandle_t;
typedef int		sfxHandle_t;
typedef int		fileHandle_t;
typedef int		clipHandle_t;

//Raz: can't think of a better place to put this atm,
//		should probably be in the platform specific definitions
#if defined (_MSC_VER) && (_MSC_VER >= 1600)

	#include <stdint.h>

	// vsnprintf is ISO/IEC 9899:1999
	// abstracting this to make it portable
	int Q_vsnprintf( char *str, size_t size, const char *format, va_list args );

#elif defined (_MSC_VER)

	#include <io.h>

	typedef signed __int64 int64_t;
	typedef signed __int32 int32_t;
	typedef signed __int16 int16_t;
	typedef signed __int8  int8_t;
	typedef unsigned __int64 uint64_t;
	typedef unsigned __int32 uint32_t;
	typedef unsigned __int16 uint16_t;
	typedef unsigned __int8  uint8_t;

	// vsnprintf is ISO/IEC 9899:1999
	// abstracting this to make it portable
	int Q_vsnprintf( char *str, size_t size, const char *format, va_list args );
#else // not using MSVC

	#include <stdint.h>

	#define Q_vsnprintf vsnprintf

#endif

#define PAD(base, alignment)	(((base)+(alignment)-1) & ~((alignment)-1))
#define PADLEN(base, alignment)	(PAD((base), (alignment)) - (base))

#define PADP(base, alignment)	((void *) PAD((intptr_t) (base), (alignment)))

#ifndef NULL
#define NULL ((void *)0)
#endif

#define	MAX_QINT			0x7fffffff
#define	MIN_QINT			(-MAX_QINT-1)


// angle indexes
#define	PITCH				0		// up / down
#define	YAW					1		// left / right
#define	ROLL				2		// fall over

// the game guarantees that no string from the network will ever
// exceed MAX_STRING_CHARS
#define	MAX_STRING_CHARS	1024	// max length of a string passed to Cmd_TokenizeString
#define	MAX_STRING_TOKENS	1024	// max tokens resulting from Cmd_TokenizeString
#define	MAX_TOKEN_CHARS		1024	// max length of an individual token

#define	MAX_INFO_STRING		1024
#define	MAX_INFO_KEY		1024
#define	MAX_INFO_VALUE		1024

#define	BIG_INFO_STRING		8192  // used for system info key only
#define	BIG_INFO_KEY		  8192
#define	BIG_INFO_VALUE		8192

#define NET_ADDRSTRMAXLEN 48 // maximum length of an IPv6 address string including trailing '\0'

//Raz: moved these from ui_local.h so we can access them everywhere
#define MAX_ADDRESSLENGTH		256//64
#define MAX_HOSTNAMELENGTH		256//22
#define MAX_MAPNAMELENGTH		256//16
#define MAX_STATUSLENGTH		256//64

#define	MAX_QPATH			64		// max length of a quake game pathname
#ifdef PATH_MAX
#define MAX_OSPATH			PATH_MAX
#else
#define	MAX_OSPATH			256		// max length of a filesystem pathname
#endif

#define	MAX_NAME_LENGTH		32		// max length of a client name
#define MAX_NETNAME			36

#define	MAX_SAY_TEXT	150

// paramters for command buffer stuffing
typedef enum {
	EXEC_NOW,			// don't return until completed, a VM should NEVER use this,
						// because some commands might cause the VM to be unloaded...
	EXEC_INSERT,		// insert at current position, but don't run yet
	EXEC_APPEND			// add to end of the command buffer (normal case)
} cbufExec_t;


//
// these aren't needed by any of the VMs.  put in another header?
//
#define	MAX_MAP_AREA_BYTES		32		// bit vector of area visibility


#define LS_STYLES_START			0
#define LS_NUM_STYLES			32
#define	LS_SWITCH_START			(LS_STYLES_START+LS_NUM_STYLES)
#define LS_NUM_SWITCH			32
#if !defined MAX_LIGHT_STYLES
#define MAX_LIGHT_STYLES		64
#endif

//For system-wide prints
enum WL_e {
	WL_ERROR=1,
	WL_WARNING,
	WL_VERBOSE,
	WL_DEBUG
};

extern float forceSpeedLevels[4];

// print levels from renderer (FIXME: set up for game / cgame?)
typedef enum {
	PRINT_ALL,
	PRINT_DEVELOPER,		// only print when "developer 1"
	PRINT_WARNING,
	PRINT_ERROR
} printParm_t;


#ifdef ERR_FATAL
#undef ERR_FATAL			// this is be defined in malloc.h
#endif

// parameters to the main Error routine
typedef enum {
	ERR_FATAL,					// exit the entire game with a popup window
	ERR_DROP,					// print to console and disconnect from game
	ERR_SERVERDISCONNECT,		// don't kill server
	ERR_DISCONNECT,				// client disconnected from the server
	ERR_NEED_CD					// pop up the need-cd dialog
} errorParm_t;

#if defined(_DEBUG) && !defined(BSPC)
	#define HUNK_DEBUG
#endif

typedef enum {
	h_high,
	h_low,
	h_dontcare
} ha_pref;

void *Hunk_Alloc( int size, ha_pref preference );

#define Com_Memset memset
#define Com_Memcpy memcpy

#define CIN_system	1
#define CIN_loop	2
#define	CIN_hold	4
#define CIN_silent	8
#define CIN_shader	16

/*
==============================================================

MATHLIB

==============================================================
*/


typedef float vec_t;
typedef vec_t vec2_t[2];
typedef vec_t vec3_t[3];
typedef vec_t vec4_t[4];
typedef vec_t vec5_t[5];

//rwwRMG - new vec types
typedef vec3_t	vec3pair_t[2];

typedef int ivec3_t[3];
typedef int ivec4_t[4];
typedef int ivec5_t[5];

typedef	int	fixed4_t;
typedef	int	fixed8_t;
typedef	int	fixed16_t;

#ifndef M_PI
#define M_PI		3.14159265358979323846f	// matches value in gcc v2 math.h
#endif

//rww - a C-ified structure version of the class which fires off callbacks and gives arguments to update ragdoll status.
enum sharedERagPhase
{
	RP_START_DEATH_ANIM,
	RP_END_DEATH_ANIM,
	RP_DEATH_COLLISION,
	RP_CORPSE_SHOT,
	RP_GET_PELVIS_OFFSET,  // this actually does nothing but set the pelvisAnglesOffset, and pelvisPositionOffset
	RP_SET_PELVIS_OFFSET,  // this actually does nothing but set the pelvisAnglesOffset, and pelvisPositionOffset
	RP_DISABLE_EFFECTORS  // this removes effectors given by the effectorsToTurnOff member
};

enum sharedERagEffector
{
	RE_MODEL_ROOT=			0x00000001, //"model_root"
	RE_PELVIS=				0x00000002, //"pelvis"
	RE_LOWER_LUMBAR=		0x00000004, //"lower_lumbar"
	RE_UPPER_LUMBAR=		0x00000008, //"upper_lumbar"
	RE_THORACIC=			0x00000010, //"thoracic"
	RE_CRANIUM=				0x00000020, //"cranium"
	RE_RHUMEROUS=			0x00000040, //"rhumerus"
	RE_LHUMEROUS=			0x00000080, //"lhumerus"
	RE_RRADIUS=				0x00000100, //"rradius"
	RE_LRADIUS=				0x00000200, //"lradius"
	RE_RFEMURYZ=			0x00000400, //"rfemurYZ"
	RE_LFEMURYZ=			0x00000800, //"lfemurYZ"
	RE_RTIBIA=				0x00001000, //"rtibia"
	RE_LTIBIA=				0x00002000, //"ltibia"
	RE_RHAND=				0x00004000, //"rhand"
	RE_LHAND=				0x00008000, //"lhand"
	RE_RTARSAL=				0x00010000, //"rtarsal"
	RE_LTARSAL=				0x00020000, //"ltarsal"
	RE_RTALUS=				0x00040000, //"rtalus"
	RE_LTALUS=				0x00080000, //"ltalus"
	RE_RRADIUSX=			0x00100000, //"rradiusX"
	RE_LRADIUSX=			0x00200000, //"lradiusX"
	RE_RFEMURX=				0x00400000, //"rfemurX"
	RE_LFEMURX=				0x00800000, //"lfemurX"
	RE_CEYEBROW=			0x01000000 //"ceyebrow"
};

typedef struct
{
	vec3_t angles;
	vec3_t position;
	vec3_t scale;
	vec3_t pelvisAnglesOffset;    // always set on return, an argument for RP_SET_PELVIS_OFFSET
	vec3_t pelvisPositionOffset; // always set on return, an argument for RP_SET_PELVIS_OFFSET

	float fImpactStrength; //should be applicable when RagPhase is RP_DEATH_COLLISION
	float fShotStrength; //should be applicable for setting velocity of corpse on shot (probably only on RP_CORPSE_SHOT)
	int me; //index of entity giving this update

	//rww - we have convenient animation/frame access in the game, so just send this info over from there.
	int startFrame;
	int endFrame;

	int collisionType; // 1 = from a fall, 0 from effectors, this will be going away soon, hence no enum 

	qboolean CallRagDollBegin; // a return value, means that we are now begininng ragdoll and the NPC stuff needs to happen

	int RagPhase;

// effector control, used for RP_DISABLE_EFFECTORS call

	int effectorsToTurnOff;  // set this to an | of the above flags for a RP_DISABLE_EFFECTORS

} sharedRagDollParams_t;

//And one for updating during model animation.
typedef struct
{
	vec3_t angles;
	vec3_t position;
	vec3_t scale;
	vec3_t velocity;
	int	me;
	int settleFrame;
} sharedRagDollUpdateParams_t;

//rww - update parms for ik bone stuff
typedef struct
{
	char boneName[512]; //name of bone
	vec3_t desiredOrigin; //world coordinate that this bone should be attempting to reach
	vec3_t origin; //world coordinate of the entity who owns the g2 instance that owns the bone
	float movementSpeed; //how fast the bone should move toward the destination
} sharedIKMoveParams_t;


typedef struct
{
	vec3_t pcjMins; //ik joint limit
	vec3_t pcjMaxs; //ik joint limit
	vec3_t origin; //origin of caller
	vec3_t angles; //angles of caller
	vec3_t scale; //scale of caller
	float radius; //bone rad
	int blendTime; //bone blend time
	int pcjOverrides; //override ik bone flags
	int startFrame; //base pose start
	int endFrame; //base pose end
	qboolean forceAnimOnBone; //normally if the bone has specified start/end frames already it will leave it alone.. if this is true, then the animation will be restarted on the bone with the specified frames anyway.
} sharedSetBoneIKStateParams_t;

enum sharedEIKMoveState
{
	IKS_NONE = 0,
	IKS_DYNAMIC
};

//material stuff needs to be shared
typedef enum //# material_e
{
	MAT_METAL = 0,	// scorched blue-grey metal
	MAT_GLASS,		// not a real chunk type, just plays an effect with glass sprites
	MAT_ELECTRICAL,	// sparks only
	MAT_ELEC_METAL,	// sparks/electrical type metal
	MAT_DRK_STONE,	// brown
	MAT_LT_STONE,	// tan
	MAT_GLASS_METAL,// glass sprites and METAl chunk
	MAT_METAL2,		// electrical metal type
	MAT_NONE,		// no chunks
	MAT_GREY_STONE,	// grey
	MAT_METAL3,		// METAL and METAL2 chunks
	MAT_CRATE1,		// yellow multi-colored crate chunks
	MAT_GRATE1,		// grate chunks
	MAT_ROPE,		// for yavin trial...no chunks, just wispy bits
	MAT_CRATE2,		// read multi-colored crate chunks
	MAT_WHITE_METAL,// white angular chunks
	MAT_SNOWY_ROCK,	// gray & brown chunks

	NUM_MATERIALS
} material_t;

//rww - bot stuff that needs to be shared
#define MAX_WPARRAY_SIZE 4096
#define MAX_NEIGHBOR_SIZE 32

#define MAX_NEIGHBOR_LINK_DISTANCE 128
#define MAX_NEIGHBOR_FORCEJUMP_LINK_DISTANCE 400

#define DEFAULT_GRID_SPACING 400

typedef struct wpneighbor_s
{
	int num;
	int forceJumpTo;
} wpneighbor_t;

typedef struct wpobject_s
{
	vec3_t origin;
	int inuse;
	int index;
	float weight;
	float disttonext;
	int flags;
	int associated_entity;

	int forceJumpTo;

	int neighbornum;
	//int neighbors[MAX_NEIGHBOR_SIZE];
	wpneighbor_t neighbors[MAX_NEIGHBOR_SIZE];
} wpobject_t;


#define NUMVERTEXNORMALS	162
extern	vec3_t	bytedirs[NUMVERTEXNORMALS];

// all drawing is done to a 640*480 virtual screen size
// and will be automatically scaled to the real resolution
#define	SCREEN_WIDTH		640
#define	SCREEN_HEIGHT		480

#define TINYCHAR_WIDTH		(SMALLCHAR_WIDTH)
#define TINYCHAR_HEIGHT		(SMALLCHAR_HEIGHT/2)

#define SMALLCHAR_WIDTH		8
#define SMALLCHAR_HEIGHT	16

#define BIGCHAR_WIDTH		12	// 16
#define BIGCHAR_HEIGHT		13	// 16

#define	GIANTCHAR_WIDTH		32
#define	GIANTCHAR_HEIGHT	48

typedef enum
{
CT_NONE,
CT_BLACK,
CT_RED,
CT_GREEN,
CT_BLUE,
CT_YELLOW,
CT_MAGENTA,
CT_CYAN,
CT_WHITE,
CT_LTGREY,
CT_MDGREY,
CT_DKGREY,
CT_DKGREY2,

CT_VLTORANGE,
CT_LTORANGE,
CT_DKORANGE,
CT_VDKORANGE,

CT_VLTBLUE1,
CT_LTBLUE1,
CT_DKBLUE1,
CT_VDKBLUE1,

CT_VLTBLUE2,
CT_LTBLUE2,
CT_DKBLUE2,
CT_VDKBLUE2,

CT_VLTBROWN1,
CT_LTBROWN1,
CT_DKBROWN1,
CT_VDKBROWN1,

CT_VLTGOLD1,
CT_LTGOLD1,
CT_DKGOLD1,
CT_VDKGOLD1,

CT_VLTPURPLE1,
CT_LTPURPLE1,
CT_DKPURPLE1,
CT_VDKPURPLE1,

CT_VLTPURPLE2,
CT_LTPURPLE2,
CT_DKPURPLE2,
CT_VDKPURPLE2,

CT_VLTPURPLE3,
CT_LTPURPLE3,
CT_DKPURPLE3,
CT_VDKPURPLE3,

CT_VLTRED1,
CT_LTRED1,
CT_DKRED1,
CT_VDKRED1,
CT_VDKRED,
CT_DKRED,

CT_VLTAQUA,
CT_LTAQUA,
CT_DKAQUA,
CT_VDKAQUA,

CT_LTPINK,
CT_DKPINK,
CT_LTCYAN,
CT_DKCYAN,
CT_LTBLUE3,
CT_BLUE3,
CT_DKBLUE3,

CT_MAX
} ct_table_t;

extern vec4_t colorTable[CT_MAX];

extern	vec4_t		colorBlack;
extern	vec4_t		colorRed;
extern	vec4_t		colorGreen;
extern	vec4_t		colorBlue;
extern	vec4_t		colorYellow;
extern	vec4_t		colorMagenta;
extern	vec4_t		colorCyan;
extern	vec4_t		colorWhite;
extern	vec4_t		colorLtGrey;
extern	vec4_t		colorMdGrey;
extern	vec4_t		colorDkGrey;
extern	vec4_t		colorLtBlue;
extern	vec4_t		colorDkBlue;

#define Q_COLOR_ESCAPE	'^'
#define Q_IsColorString(p)	( p && *(p) == Q_COLOR_ESCAPE && *((p)+1) && *((p)+1) != Q_COLOR_ESCAPE )


#define COLOR_BLACK		'0'
#define COLOR_RED		'1'
#define COLOR_GREEN		'2'
#define COLOR_YELLOW	'3'
#define COLOR_BLUE		'4'
#define COLOR_CYAN		'5'
#define COLOR_MAGENTA	'6'
#define COLOR_WHITE		'7'
#define ColorIndex(c)	( ( (c) - '0' ) & 7 )

#define S_COLOR_BLACK	"^0"
#define S_COLOR_RED		"^1"
#define S_COLOR_GREEN	"^2"
#define S_COLOR_YELLOW	"^3"
#define S_COLOR_BLUE	"^4"
#define S_COLOR_CYAN	"^5"
#define S_COLOR_MAGENTA	"^6"
#define S_COLOR_WHITE	"^7"

extern vec4_t	g_color_table[8];

#define	MAKERGB( v, r, g, b ) v[0]=r;v[1]=g;v[2]=b
#define	MAKERGBA( v, r, g, b, a ) v[0]=r;v[1]=g;v[2]=b;v[3]=a

struct cplane_s;

extern	vec3_t	vec3_origin;
extern	vec3_t	axisDefault[3];

#define	nanmask (255<<23)
#define	IS_NAN(x) (((*(int *)&x)&nanmask)==nanmask)


#if idppc

static inline float Q_rsqrt( float number ) {
		float x = 0.5f * number;
                float y;
#ifdef __GNUC__            
                asm("frsqrte %0,%1" : "=f" (y) : "f" (number));
#else
		y = __frsqrte( number );
#endif
		return y * (1.5f - (x * y * y));
	}

#ifdef __GNUC__            
static inline float Q_fabs(float x) {
    float abs_x;
    
    asm("fabs %0,%1" : "=f" (abs_x) : "f" (x));
    return abs_x;
}
#else
#define Q_fabs __fabsf
#endif

#else

float Q_fabs( float f );
float Q_rsqrt( float f );		// reciprocal square root

#endif // idppc


#define SQRTFAST( x ) ( (x) * Q_rsqrt( x ) )

signed char ClampChar( int i );
signed short ClampShort( int i );

float Q_powf ( float x, int y );

// this isn't a real cheap function to call!
int DirToByte( vec3_t dir );
void ByteToDir( int b, vec3_t dir );

//rwwRMG - added math defines
#define minimum( x, y ) ((x) < (y) ? (x) : (y))
#define maximum( x, y ) ((x) > (y) ? (x) : (y))

//JAC: Moved to q_math.c
#define DEG2RAD( deg ) ( ((deg)*M_PI) / 180.0f )
#define RAD2DEG( rad ) ( ((rad)*180.0f) / M_PI )

extern ID_INLINE void		VectorAdd( const vec3_t vec1, const vec3_t vec2, vec3_t vecOut );
extern ID_INLINE void		VectorSubtract( const vec3_t vec1, const vec3_t vec2, vec3_t vecOut );
extern ID_INLINE void		VectorScale( const vec3_t vecIn, vec_t scale, vec3_t vecOut );
extern ID_INLINE void		VectorScale4( const vec4_t vecIn, vec_t scale, vec4_t vecOut );
extern ID_INLINE void		VectorMA( const vec3_t vec1, float scale, const vec3_t vec2, vec3_t vecOut );
extern ID_INLINE vec_t		VectorLength( const vec3_t vec );
extern ID_INLINE vec_t		VectorLengthSquared( const vec3_t vec );
extern ID_INLINE vec_t		Distance( const vec3_t p1, const vec3_t p2 );
extern ID_INLINE vec_t		DistanceSquared( const vec3_t p1, const vec3_t p2 );
extern ID_INLINE void		VectorNormalizeFast( vec3_t vec );
extern ID_INLINE vec_t		VectorNormalize( vec3_t vec );
extern ID_INLINE vec_t		VectorNormalize2( const vec3_t vec, vec3_t vecOut );
extern ID_INLINE void		VectorCopy( const vec3_t vecIn, vec3_t vecOut );
extern ID_INLINE void		VectorCopy4( const vec4_t vecIn, vec4_t vecOut );
extern ID_INLINE void		VectorSet( vec3_t vec, vec_t x, vec_t y, vec_t z );
extern ID_INLINE void		VectorSet4( vec4_t vec, vec_t x, vec_t y, vec_t z, vec_t w );
extern ID_INLINE void		VectorSet5( vec5_t vec, vec_t x, vec_t y, vec_t z, vec_t w, vec_t u );
extern ID_INLINE void		VectorClear( vec3_t vec );
extern ID_INLINE void		VectorClear4( vec4_t vec );
extern ID_INLINE void		VectorInc( vec3_t vec );
extern ID_INLINE void		VectorDec( vec3_t vec );
extern ID_INLINE void		VectorInverse( vec3_t vec );
extern ID_INLINE void		CrossProduct( const vec3_t vec1, const vec3_t vec2, vec3_t vecOut );
extern ID_INLINE vec_t		DotProduct( const vec3_t vec1, const vec3_t vec2 );
extern ID_INLINE qboolean	VectorCompare( const vec3_t vec1, const vec3_t vec2 );

#define				VectorAddM( vec1, vec2, vecOut )		((vecOut)[0]=(vec1)[0]+(vec2)[0], (vecOut)[1]=(vec1)[1]+(vec2)[1], (vecOut)[2]=(vec1)[2]+(vec2)[2])
#define				VectorSubtractM( vec1, vec2, vecOut )	((vecOut)[0]=(vec1)[0]-(vec2)[0], (vecOut)[1]=(vec1)[1]-(vec2)[1], (vecOut)[2]=(vec1)[2]-(vec2)[2])
#define				VectorScaleM( vecIn, scale, vecOut )	((vecOut)[0]=(vecIn)[0]*(scale), (vecOut)[1]=(vecIn)[1]*(scale), (vecOut)[2]=(vecIn)[2]*(scale))
#define				VectorScale4M( vecIn, scale, vecOut )	((vecOut)[0]=(vecIn)[0]*(scale), (vecOut)[1]=(vecIn)[1]*(scale), (vecOut)[2]=(vecIn)[2]*(scale), (vecOut)[3]=(vecIn)[3]*(scale))
#define				VectorMAM( vec1, scale, vec2, vecOut )	((vecOut)[0]=(vec1)[0]+(vec2)[0]*(scale), (vecOut)[1]=(vec1)[1]+(vec2)[1]*(scale), (vecOut)[2]=(vec1)[2]+(vec2)[2]*(scale))
#define				VectorLengthM( vec )					VectorLength( vec )
#define				VectorLengthSquaredM( vec )				VectorLengthSquared( vec )
#define				DistanceM( vec )						Distance( vec )
#define				DistanceSquaredM( p1, p2 )				DistanceSquared( p1, p2 )
#define				VectorNormalizeFastM( vec )				VectorNormalizeFast( vec )
#define				VectorNormalizeM( vec )					VectorNormalize( vec )
#define				VectorNormalize2M( vec, vecOut )		VectorNormalize2( vec, vecOut )
#define				VectorCopyM( vecIn, vecOut )			((vecOut)[0]=(vecIn)[0], (vecOut)[1]=(vecIn)[1], (vecOut)[2]=(vecIn)[2])
#define				VectorCopy4M( vecIn, vecOut )			((vecOut)[0]=(vecIn)[0], (vecOut)[1]=(vecIn)[1], (vecOut)[2]=(vecIn)[2], (vecOut)[3]=(vecIn)[3])
#define				VectorSetM( vec, x, y, z )				((vec)[0]=(x), (vec)[1]=(y), (vec)[2]=(z))
#define				VectorSet4M( vec, x, y, z, w )			((vec)[0]=(x), (vec)[1]=(y), (vec)[2]=(z), (vec)[3]=(w))
#define				VectorSet5M( vec, x, y, z, w, u )		((vec)[0]=(x), (vec)[1]=(y), (vec)[2]=(z), (vec)[3]=(w), (vec)[4]=(u))
#define				VectorClearM( vec )						((vec)[0]=(vec)[1]=(vec)[2]=0)
#define				VectorClear4M( vec )					((vec)[0]=(vec)[1]=(vec)[2]=(vec)[3]=0)
#define				VectorIncM( vec )						((vec)[0]+=1.0f, (vec)[1]+=1.0f, (vec)[2]+=1.0f)
#define				VectorDecM( vec )						((vec)[0]-=1.0f, (vec)[1]-=1.0f, (vec)[2]-=1.0f)
#define				VectorInverseM( vec )					((vec)[0]=-(vec)[0], (vec)[1]=-(vec)[1], (vec)[2]=-(vec)[2])
#define				CrossProductM( vec1, vec2, vecOut )		((vecOut)[0]=((vec1)[1]*(vec2)[2])-((vec1)[2]*(v2)[1]), (vecOut)[1]=((vec1)[2]*(vec2)[0])-((vec1)[0]*(vec2)[2]), (vecOut)[2]=((vec1)[0]*(vec2)[1])-((vec1)[1]*(vec2)[0]))
#define				DotProductM( x, y )						((x)[0]*(y)[0]+(x)[1]*(y)[1]+(x)[2]*(y)[2])
#define				VectorCompareM( vec1, vec2 )			(!!((vec1)[0]==(vec2)[0] && (vec1)[1]==(vec2)[1] && (vec1)[2]==(vec2)[2]))

// TODO
#define VectorScaleVector(a,b,c)		(((c)[0]=(a)[0]*(b)[0]),((c)[1]=(a)[1]*(b)[1]),((c)[2]=(a)[2]*(b)[2]))
#define VectorInverseScaleVector(a,b,c)	((c)[0]=(a)[0]/(b)[0],(c)[1]=(a)[1]/(b)[1],(c)[2]=(a)[2]/(b)[2])
#define VectorScaleVectorAdd(c,a,b,o)	((o)[0]=(c)[0]+((a)[0]*(b)[0]),(o)[1]=(c)[1]+((a)[1]*(b)[1]),(o)[2]=(c)[2]+((a)[2]*(b)[2]))
#define VectorAdvance(a,s,b,c)			(((c)[0]=(a)[0] + s * ((b)[0] - (a)[0])),((c)[1]=(a)[1] + s * ((b)[1] - (a)[1])),((c)[2]=(a)[2] + s * ((b)[2] - (a)[2])))
#define VectorAverage(a,b,c)			(((c)[0]=((a)[0]+(b)[0])*0.5f),((c)[1]=((a)[1]+(b)[1])*0.5f),((c)[2]=((a)[2]+(b)[2])*0.5f))
#define VectorNegate(a,b)				((b)[0]=-(a)[0],(b)[1]=-(a)[1],(b)[2]=-(a)[2])

#ifdef __LCC__
#ifdef VectorCopy
#undef VectorCopy
// this is a little hack to get more efficient copies in our interpreter
typedef struct {
	float	v[3];
} vec3struct_t;
#define VectorCopy(a,b)	*(vec3struct_t *)b=*(vec3struct_t *)a;
#define ID_INLINE static
#endif
#endif

#if defined(MACOS_X) || defined(__linux__)
	#define	SnapVector(v) {v[0]=((int)(v[0]));v[1]=((int)(v[1]));v[2]=((int)(v[2]));}
#else 
	#if !defined(__LCC__) && !defined(MINGW32)
		//pitiful attempt to reduce _ftol2 calls -rww
		static ID_INLINE void SnapVector( float *v )
		{
			//RAZTODO: q_math.c plz
			static int i;
			static float f;

			f = *v;
			__asm fld f
			__asm fistp	i
			*v = i;
			v++;
			f = *v;
			__asm fld f
			__asm fistp i
			*v = i;
			v++;
			f = *v;
			__asm fld f
			__asm fistp i
			*v = i;
		}
	#else
		#define	SnapVector(v) {v[0]=((int)(v[0]));v[1]=((int)(v[1]));v[2]=((int)(v[2]));}
	#endif // __LCC__ || MINGW32
#endif // MACOS_X || __linux__

unsigned ColorBytes3 (float r, float g, float b);
unsigned ColorBytes4 (float r, float g, float b, float a);

float NormalizeColor( const vec3_t in, vec3_t out );

float RadiusFromBounds( const vec3_t mins, const vec3_t maxs );
void ClearBounds( vec3_t mins, vec3_t maxs );
vec_t DistanceHorizontal( const vec3_t p1, const vec3_t p2 );
vec_t DistanceHorizontalSquared( const vec3_t p1, const vec3_t p2 );
void AddPointToBounds( const vec3_t v, vec3_t mins, vec3_t maxs );
void VectorRotate( vec3_t in, vec3_t matrix[3], vec3_t out );
int Q_log2(int val);

float Q_acos(float c);
float Q_asin(float c);

int		Q_rand( int *seed );
float	Q_random( int *seed );
float	Q_crandom( int *seed );

#define random()	((rand () & 0x7fff) / ((float)0x7fff))
#define crandom()	(2.0 * (random() - 0.5))

void vectoangles( const vec3_t value1, vec3_t angles);
void AnglesToAxis( const vec3_t angles, vec3_t axis[3] );

void AxisClear( vec3_t axis[3] );
void AxisCopy( vec3_t in[3], vec3_t out[3] );

void SetPlaneSignbits( struct cplane_s *out );
int BoxOnPlaneSide (vec3_t emins, vec3_t emaxs, struct cplane_s *plane);

double	fmod( double x, double y );
float	AngleMod(float a);
float	LerpAngle (float from, float to, float frac);
float	AngleSubtract( float a1, float a2 );
void	AnglesSubtract( vec3_t v1, vec3_t v2, vec3_t v3 );

float AngleNormalize360 ( float angle );
float AngleNormalize180 ( float angle );
float AngleDelta ( float angle1, float angle2 );

qboolean PlaneFromPoints( vec4_t plane, const vec3_t a, const vec3_t b, const vec3_t c );
void ProjectPointOnPlane( vec3_t dst, const vec3_t p, const vec3_t normal );
void RotatePointAroundVector( vec3_t dst, const vec3_t dir, const vec3_t point, float degrees );
void RotateAroundDirection( vec3_t axis[3], float yaw );
void MakeNormalVectors( const vec3_t forward, vec3_t right, vec3_t up );
// perpendicular vector could be replaced by this

//int	PlaneTypeForNormal (vec3_t normal);

void MatrixMultiply(float in1[3][3], float in2[3][3], float out[3][3]);
void AngleVectors( const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up);
void PerpendicularVector( vec3_t dst, const vec3_t src );
void NormalToLatLong( const vec3_t normal, byte bytes[2] ); //rwwRMG - added

//=============================================

int Com_Clampi( int min, int max, int value ); //rwwRMG - added
float Com_Clamp( float min, float max, float value );

char	*COM_SkipPath( char *pathname );
void	COM_StripExtension( const char *in, char *out, int destsize );
void	COM_DefaultExtension( char *path, int maxSize, const char *extension );

void	COM_BeginParseSession( const char *name );
int		COM_GetCurrentParseLine( void );
const char	*SkipWhitespace( const char *data, qboolean *hasNewLines );
char	*COM_Parse( const char **data_p );
char	*COM_ParseExt( const char **data_p, qboolean allowLineBreak );
int		COM_Compress( char *data_p );
void	COM_ParseError( char *format, ... );
void	COM_ParseWarning( char *format, ... );
qboolean COM_ParseString( const char **data, const char **s );
qboolean COM_ParseInt( const char **data, int *i );
qboolean COM_ParseFloat( const char **data, float *f );
qboolean COM_ParseVec4( const char **buffer, vec4_t *c);
//int		COM_ParseInfos( char *buf, int max, char infos[][MAX_INFO_STRING] );

#define MAX_TOKENLENGTH		1024

#ifndef TT_STRING
//token types
#define TT_STRING					1			// string
#define TT_LITERAL					2			// literal
#define TT_NUMBER					3			// number
#define TT_NAME						4			// name
#define TT_PUNCTUATION				5			// punctuation
#endif

typedef struct pc_token_s
{
	int type;
	int subtype;
	int intvalue;
	float floatvalue;
	char string[MAX_TOKENLENGTH];
} pc_token_t;

// data is an in/out parm, returns a parsed out token

void	COM_MatchToken( const char**buf_p, char *match );

void SkipBracedSection (const char **program);
void SkipRestOfLine ( const char **data );

void Parse1DMatrix (const char **buf_p, int x, float *m);
void Parse2DMatrix (const char **buf_p, int y, int x, float *m);
void Parse3DMatrix (const char **buf_p, int z, int y, int x, float *m);

int	QDECL Com_sprintf (char *dest, int size, const char *fmt, ...);


// mode parm for FS_FOpenFile
typedef enum {
	FS_READ,
	FS_WRITE,
	FS_APPEND,
	FS_APPEND_SYNC,
	FS_READ_TEXT,
	FS_WRITE_TEXT,
	FS_APPEND_TEXT,
	FS_APPEND_SYNC_TEXT
} fsMode_t;

typedef enum {
	FS_SEEK_CUR,
	FS_SEEK_END,
	FS_SEEK_SET
} fsOrigin_t;

//=============================================

int Q_isprint( int c );
int Q_islower( int c );
int Q_isupper( int c );
int Q_isalpha( int c );
qboolean Q_isanumber( const char *s );
qboolean Q_isintegral( float f );

// portable case insensitive compare
int		Q_stricmp (const char *s1, const char *s2);
int		Q_strncmp (const char *s1, const char *s2, int n);
int		Q_stricmpn (const char *s1, const char *s2, int n);
char	*Q_strlwr( char *s1 );
char	*Q_strupr( char *s1 );
char	*Q_strrchr( const char* string, int c );

// buffer size safe library replacements
void	Q_strncpyz( char *dest, const char *src, int destsize );
void	Q_strcat( char *dest, int size, const char *src );

// strlen that discounts Quake color sequences
int Q_PrintStrlen( const char *string );
// removes color sequences from string
char *Q_CleanStr( char *string );
void Q_strstrip( char *string, const char *strip, const char *repl );
const char *Q_strchrs( const char *string, const char *search );

//=============================================

// 64-bit integers for global rankings interface
// implemented as a struct for qvm compatibility
typedef struct
{
	byte	b0;
	byte	b1;
	byte	b2;
	byte	b3;
	byte	b4;
	byte	b5;
	byte	b6;
	byte	b7;
} qint64;

//=============================================
/*
short	BigShort(short l);
short	LittleShort(short l);
int		BigLong (int l);
int		LittleLong (int l);
qint64  BigLong64 (qint64 l);
qint64  LittleLong64 (qint64 l);
float	BigFloat (const float *l);
float	LittleFloat (const float *l);

void	Swap_Init (void);
*/
char	* QDECL va(const char *format, ...);

//=============================================

//
// key / value info strings
//
char *Info_ValueForKey( const char *s, const char *key );
void Info_RemoveKey( char *s, const char *key );
void Info_RemoveKey_Big( char *s, const char *key );
void Info_SetValueForKey( char *s, const char *key, const char *value );
void Info_SetValueForKey_Big( char *s, const char *key, const char *value );
qboolean Info_Validate( const char *s );
void Info_NextPair( const char **s, char *key, char *value );

// this is only here so the functions in q_shared.c and bg_*.c can link
void	QDECL Com_Error( int level, const char *error, ... );
void	QDECL Com_Printf( const char *msg, ... );


/*
==========================================================

CVARS (console variables)

Many variables can be used for cheating purposes, so when
cheats is zero, force all unspecified variables to their
default values.
==========================================================
*/

#define	CVAR_NONE			0x00000000
#define	CVAR_ARCHIVE		0x00000001		// set to cause it to be saved to vars.rc
											// used for system variables, not for player
											// specific configurations
#define	CVAR_USERINFO		0x00000002		// sent to server on connect or change
#define	CVAR_SERVERINFO		0x00000004		// sent in response to front end requests
#define	CVAR_SYSTEMINFO		0x00000008		// these cvars will be duplicated on all clients
#define	CVAR_INIT			0x00000010		// don't allow change from console at all,
											// but can be set from the command line
#define	CVAR_LATCH			0x00000020		// will only change when C code next does
											// a Cvar_Get(), so it can't be changed
											// without proper initialization.  modified
											// will be set, even though the value hasn't
											// changed yet
#define	CVAR_ROM			0x00000040		// display only, cannot be set by user at all (can be set by code)
#define	CVAR_USER_CREATED	0x00000080		// created by a set command
#define	CVAR_TEMP			0x00000100		// can be set even when cheats are disabled, but is not archived
#define CVAR_CHEAT			0x00000200		// can not be changed if cheats are disabled
#define CVAR_NORESTART		0x00000400		// do not clear when a cvar_restart is issued
#define CVAR_INTERNAL		0x00000800		// cvar won't be displayed, ever (for passwords and such)
#define	CVAR_PARENTAL		0x00001000		// lets cvar system know that parental stuff needs to be updated
#define CVAR_SERVER_CREATED	0x2000			// cvar was created by a server the client connected to.
#define CVAR_VM_CREATED		0x4000			// cvar was created exclusively in one of the VMs.
#define CVAR_PROTECTED		0x8000			// prevent modifying this var from VMs or the server
// These flags are only returned by the Cvar_Flags() function
#define CVAR_MODIFIED		0x40000000		// Cvar was modified
#define CVAR_NONEXISTENT	0x80000000		// Cvar doesn't exist.
#define CVAR_LOCK_RANGE		0x00002000		// enforces the mins / maxs

// nothing outside the Cvar_*() functions should modify these fields!
typedef struct cvar_s {
	char		*name;
	char		*string;
	char		*resetString;		// cvar_restart will reset to this value
	char		*latchedString;		// for CVAR_LATCH vars
	float		mMinValue, mMaxValue;
	int			flags;
	qboolean	modified;			// set each time the cvar is changed
	int			modificationCount;	// incremented each time the cvar is changed
	float		value;				// atof( string )
	int			integer;			// atoi( string )
	struct cvar_s *next;
	struct cvar_s *hashNext;
} cvar_t;

#define	MAX_CVAR_VALUE_STRING	256

typedef int	cvarHandle_t;

// the modules that run in the virtual machine can't access the cvar_t directly,
// so they must ask for structured updates
typedef struct {
	cvarHandle_t	handle;
	int			modificationCount;
	float		value;
	int			integer;
	char		string[MAX_CVAR_VALUE_STRING];
} vmCvar_t;

/*
==============================================================

COLLISION DETECTION

==============================================================
*/

#include "game/surfaceflags.h"			// shared with the q3map utility

// plane types are used to speed some tests
// 0-2 are axial planes
#define	PLANE_X			0
#define	PLANE_Y			1
#define	PLANE_Z			2
#define	PLANE_NON_AXIAL	3


/*
=================
PlaneTypeForNormal
=================
*/

#define PlaneTypeForNormal(x) (x[0] == 1.0 ? PLANE_X : (x[1] == 1.0 ? PLANE_Y : (x[2] == 1.0 ? PLANE_Z : PLANE_NON_AXIAL) ) )

// plane_t structure
// !!! if this is changed, it must be changed in asm code too !!!
typedef struct cplane_s {
	vec3_t	normal;
	float	dist;
	byte	type;			// for fast side tests: 0,1,2 = axial, 3 = nonaxial
	byte	signbits;		// signx + (signy<<1) + (signz<<2), used as lookup during collision
	byte	pad[2];
} cplane_t;
/*
Ghoul2 Insert Start
*/
typedef struct
{
	float		mDistance;
	int			mEntityNum;
	int			mModelIndex;
	int			mPolyIndex;
	int			mSurfaceIndex;
	vec3_t		mCollisionPosition;
	vec3_t		mCollisionNormal;
	int			mFlags;
	int			mMaterial;
	int			mLocation;
	float		mBarycentricI; // two barycentic coodinates for the hit point
	float		mBarycentricJ; // K = 1-I-J
}CollisionRecord_t;

#define MAX_G2_COLLISIONS 16

typedef CollisionRecord_t G2Trace_t[MAX_G2_COLLISIONS];	// map that describes all of the parts of ghoul2 models that got hit

/*
Ghoul2 Insert End
*/
// a trace is returned when a box is swept through the world
typedef struct {
	qboolean	allsolid;	// if true, plane is not valid
	qboolean	startsolid;	// if true, the initial point was in a solid area

	float		fraction;	// time completed, 1.0 = didn't hit anything
	vec3_t		endpos;		// final position
	cplane_t	plane;		// surface normal at impact, transformed to world space
	int			surfaceFlags;	// surface hit
	int			contents;	// contents on other side of surface hit
	int			entityNum;	// entity the contacted sirface is a part of
/*
Ghoul2 Insert Start
*/
	//rww - removed this for now, it's just wasting space in the trace structure.
//	CollisionRecord_t G2CollisionMap[MAX_G2_COLLISIONS];	// map that describes all of the parts of ghoul2 models that got hit
/*
Ghoul2 Insert End
*/
} trace_t;

// trace->entityNum can also be 0 to (MAX_GENTITIES-1)
// or ENTITYNUM_NONE, ENTITYNUM_WORLD


// markfragments are returned by CM_MarkFragments()
typedef struct {
	int		firstPoint;
	int		numPoints;
} markFragment_t;



typedef struct {
	vec3_t		origin;
	vec3_t		axis[3];
} orientation_t;

//=====================================================================


// in order from highest priority to lowest
// if none of the catchers are active, bound key strings will be executed
#define KEYCATCH_CONSOLE		0x0001
#define	KEYCATCH_UI					0x0002
#define	KEYCATCH_MESSAGE		0x0004
#define	KEYCATCH_CGAME			0x0008
#define KEYCATCH_NUMBERSONLY	0x0010


// sound channels
// channel 0 never willingly overrides
// other channels will allways override a playing sound on that channel
typedef enum {
	CHAN_AUTO,	//## %s !!"W:\game\base\!!sound\*.wav;*.mp3" # Auto-picks an empty channel to play sound on
	CHAN_LOCAL,	//## %s !!"W:\game\base\!!sound\*.wav;*.mp3" # menu sounds, etc
	CHAN_WEAPON,//## %s !!"W:\game\base\!!sound\*.wav;*.mp3" 
	CHAN_VOICE, //## %s !!"W:\game\base\!!sound\voice\*.wav;*.mp3" # Voice sounds cause mouth animation
	//CHAN_VOICE_ATTEN, //## %s !!"W:\game\base\!!sound\voice\*.wav;*.mp3" # Causes mouth animation but still use normal sound falloff 
	CHAN_ITEM,  //## %s !!"W:\game\base\!!sound\*.wav;*.mp3"
	CHAN_BODY,	//## %s !!"W:\game\base\!!sound\*.wav;*.mp3"
	CHAN_LOCAL_SOUND,	//## %s !!"W:\game\base\!!sound\*.wav;*.mp3" #chat messages, etc
	CHAN_ANNOUNCER,		//## %s !!"W:\game\base\!!sound\*.wav;*.mp3" #announcer voices, etc
	CHAN_AMBIENT,//## %s !!"W:\game\base\!!sound\*.wav;*.mp3" # added for ambient sounds
	//CHAN_LESS_ATTEN,	//## %s !!"W:\game\base\!!sound\*.wav;*.mp3" #attenuates similar to chan_voice, but uses empty channel auto-pick behaviour
	//CHAN_MENU1,		//## %s !!"W:\game\base\!!sound\*.wav;*.mp3" #menu stuff, etc
	//CHAN_VOICE_GLOBAL,  //## %s !!"W:\game\base\!!sound\voice\*.wav;*.mp3" # Causes mouth animation and is broadcast, like announcer
	//CHAN_MUSIC,	//## %s !!"W:\game\base\!!sound\*.wav;*.mp3" #music played as a looping sound - added by BTO (VV)
} soundChannel_t;


/*
========================================================================

  ELEMENTS COMMUNICATED ACROSS THE NET

========================================================================
*/

#define	ANGLE2SHORT(x)	((int)((x)*65536/360) & 65535)
#define	SHORT2ANGLE(x)	((x)*(360.0/65536))

#define	SNAPFLAG_RATE_DELAYED	1
#define	SNAPFLAG_NOT_ACTIVE		2	// snapshot used during connection and for zombies
#define SNAPFLAG_SERVERCOUNT	4	// toggled every map_restart so transitions can be detected

//
// per-level limits
//
#define	MAX_CLIENTS			64		// absolute limit
//#define MAX_RADAR_ENTITIES	MAX_GENTITIES
#define MAX_TERRAINS		32 //rwwRMG: inserted
#define MAX_LOCATIONS		64
#define MAX_LADDERS			64

#define MAX_INSTANCE_TYPES		16

#define	GENTITYNUM_BITS	10		// don't need to send any more
#define	MAX_GENTITIES	(1<<GENTITYNUM_BITS)

// entitynums are communicated with GENTITY_BITS, so any reserved
// values thatare going to be communcated over the net need to
// also be in this range
#define	ENTITYNUM_NONE		(MAX_GENTITIES-1)
#define	ENTITYNUM_WORLD		(MAX_GENTITIES-2)
#define	ENTITYNUM_MAX_NORMAL	(MAX_GENTITIES-2)


// these are also in be_aas_def.h - argh (rjr)
#define	MAX_MODELS				256		// these are sent over the net as 8 bits
#define	MAX_SOUNDS				256		// so they cannot be blindly increased
#define MAX_AMBIENT_SOUNDSETS	64
#define MAX_FX					64		// max effects strings, I'm hoping that 64 will be plenty
#define MAX_SUB_BSP				32
#define MAX_ICONS				32
#define	MAX_CHARSKINS			64		// character skins
#define	MAX_HUDICONS			16		// icons on hud

#define	MAX_CONFIGSTRINGS	1400 //this is getting pretty high. Try not to raise it anymore than it already is.

// these are the only configstrings that the system reserves, all the
// other ones are strictly for servergame to clientgame communication
#define	CS_SERVERINFO		0		// an info string with all the serverinfo cvars
#define	CS_SYSTEMINFO		1		// an info string for server system to client system configuration (timescale, etc)
#define CS_PLAYERS			2		// info string for player user info
#define CS_CUSTOM			(CS_PLAYERS + MAX_CLIENTS )

#define	RESERVED_CONFIGSTRINGS	2	// game can't modify below this, only the system can

#define	MAX_GAMESTATE_CHARS	16000
typedef struct {
	int			stringOffsets[MAX_CONFIGSTRINGS];
	char		stringData[MAX_GAMESTATE_CHARS];
	int			dataCount;
} gameState_t;

//=========================================================

// bit field limits
#define	MAX_STATS				16
#define	MAX_PERSISTANT			16
#define	MAX_AMMO				16
#define	MAX_WEAPONS				32		
#define MAX_GAMETYPE_ITEMS		5

#define	MAX_PS_EVENTS			4

#define PS_PMOVEFRAMECOUNTBITS	6

typedef enum
{
	ATTACK_NORMAL,
	ATTACK_ALTERNATE,
	ATTACK_MAX

} attackType_t;

// playerState_t is the information needed by both the client and server
// to predict player motion and actions
// nothing outside of pmove should modify these, or some degree of prediction error
// will occur

// you can't add anything to this without modifying the code in msg.c

// playerState_t is a full superset of entityState_t as it is used by players,
// so if a playerState_t is transmitted, the entityState_t can be fully derived
// from it.

typedef struct playerState_s {
	int			commandTime;	// cmd->serverTime of last executed command
	int			pm_type;
	int			bobCycle;		// for view bobbing and footstep generation
	int			pm_flags;		// ducked, etc
	int			pm_debounce;	// debounce buttons
	int			pm_time;

	vec3_t		origin;
	vec3_t		velocity;

	int			weaponTime;
	int			weaponFireBurstCount;
	int			weaponAnimId;
	int			weaponAnimIdChoice;
	int			weaponAnimTime;
	int			weaponCallbackTime;
	int			weaponCallbackStep;

	int			gravity;
	int			speed;
	int			delta_angles[3];				// add to command angles to get view direction
												// changed by spawns, rotating objects, and teleporters
	int			groundEntityNum;				// ENTITYNUM_NONE = in air
												
	int			legsAnim;						// mask off ANIM_TOGGLEBIT
												
	int			torsoTimer;						// don't change low priority animations until this runs out
	int			torsoAnim;						// mask off ANIM_TOGGLEBIT
												
	int			movementDir;					// a number 0 to 7 that represents the reletive angle
												// of movement to the view angle (axial and diagonals)
												// when at rest, the value will remain unchanged
												// used to twist the legs during strafing
												
	int			eFlags;							// copied to entityState_t->eFlags
												
	int			eventSequence;					// pmove generated events
	int			events[MAX_PS_EVENTS];			
	int			eventParms[MAX_PS_EVENTS];		
												
	int			externalEvent;					// events set on player from another source
	int			externalEventParm;				
	int			externalEventTime;				
												
	int			clientNum;						// ranges from 0 to MAX_CLIENTS-1
	int			weapon;							// copied to entityState_t->weapon
	int			weaponstate;					
												
	vec3_t		viewangles;						// for fixed views
	int			viewheight;						
												
	// damage feedback							
	int			damageEvent;					// when it changes, latch the other parms
	int			damageYaw;						
	int			damagePitch;					
	int			damageCount;					
												
	int			painTime;						// used for both game and client side to process the pain twitch - NOT sent across the network
	int			painDirection;					// NOT sent across the network
										
	int			stats[MAX_STATS];				
	int			persistant[MAX_PERSISTANT];		// stats that aren't cleared on death
	int			ammo[MAX_AMMO];
	int			clip[ATTACK_MAX][MAX_WEAPONS];
	int			firemode[MAX_WEAPONS];

	int			generic1;
	int			loopSound;

	// Incaccuracy values for firing
	int			inaccuracy;
	int			inaccuracyTime;
	int			kickPitch;

	// not communicated over the net at all
	int			ping;							// server to game info for scoreboard
	int			pmove_framecount;				// FIXME: don't transmit over the network
	int			jumppad_frame;
	int			entityEventSequence;
	vec3_t		pvsOrigin;						// view origin used to calculate PVS (also the lean origin)
												// THIS VARIABLE MUST AT LEAST BE THE PLAYERS ORIGIN ALL OF THE 
												// TIME OR THE PVS CALCULATIONS WILL NOT WORK.

	// Zooming
	int			zoomTime;
	int			zoomFov;

	// LAdders
	int			ladder;
	int			leanTime;

	// Timers 
	int			grenadeTimer;
	int			respawnTimer;
} playerState_t;


typedef enum 
{
	TEAM_FREE,
	TEAM_RED,
	TEAM_BLUE,
	TEAM_SPECTATOR,

	TEAM_NUM_TEAMS

} team_t;

//====================================================================


//
// usercmd_t->button bits, many of which are generated by the client system,
// so they aren't game/cgame only definitions
//
#define	BUTTON_ATTACK		(1<<0)
#define	BUTTON_TALK			(1<<1)			// displays talk balloon and disables actions	
#define BUTTON_GOGGLES		(1<<2)			// turns nv or therm goggles on/off
#define BUTTON_LEAN			(1<<3)			// lean modifier, when held strafe left and right will lean
#define	BUTTON_WALKING		(1<<4)			// walking can't just be infered from MOVE_RUN
											// because a key pressed late in the frame will
											// only generate a small move value for that frame
											// walking will use different animations and
											// won't generate footsteps
#define	BUTTON_USE			(1<<5)			// the ol' use key returns!
#define	BUTTON_RELOAD		(1<<6)			// reloads current weapon
#define BUTTON_ALT_ATTACK	(1<<7)
#define	BUTTON_ANY			(1<<8)			// any key whatsoever
#define BUTTON_ZOOMIN		(1<<9)
#define BUTTON_ZOOMOUT		(1<<10)
#define BUTTON_FIREMODE		(1<<11)

#define BUTTON_LEAN_RIGHT	(1<<12)
#define BUTTON_LEAN_LEFT	(1<<13)

// usercmd_t is sent to the server each client frame
typedef struct usercmd_s {
	int				serverTime;
	int				angles[3];
	int 			buttons;
	byte			weapon;
	signed char		forwardmove;
	signed char		rightmove;
	signed char		upmove;
} usercmd_t;

//===================================================================

//rww - unsightly hack to allow us to make an FX call that takes a horrible amount of args
typedef struct addpolyArgStruct_s {
	vec3_t				p[4];
	vec2_t				ev[4];
	int					numVerts;
	vec3_t				vel;
	vec3_t				accel;
	float				alpha1;
	float				alpha2;
	float				alphaParm;
	vec3_t				rgb1;
	vec3_t				rgb2;
	float				rgbParm;
	vec3_t				rotationDelta;
	float				bounce;
	int					motionDelay;
	int					killTime;
	qhandle_t			shader;
	int					flags;
} addpolyArgStruct_t;

typedef struct addbezierArgStruct_s {
	vec3_t start;
	vec3_t end;
	vec3_t control1;
	vec3_t control1Vel;
	vec3_t control2;
	vec3_t control2Vel;
	float size1;
	float size2;
	float sizeParm;
	float alpha1;
	float alpha2;
	float alphaParm;
	vec3_t sRGB;
	vec3_t eRGB;
	float rgbParm;
	int killTime;
	qhandle_t shader;
	int flags;
} addbezierArgStruct_t;

typedef struct addspriteArgStruct_s
{
	vec3_t origin;
	vec3_t vel;
	vec3_t accel;
	float scale;
	float dscale;
	float sAlpha;
	float eAlpha;
	float rotation;
	float bounce;
	int life;
	qhandle_t shader;
	int flags;
} addspriteArgStruct_t;

typedef struct
{
	vec3_t	origin;

	// very specifc case, we can modulate the color and the alpha
	vec3_t	rgb;
	vec3_t	destrgb;
	vec3_t	curRGB;

	float	alpha;
	float	destAlpha;
	float	curAlpha;

	// this is a very specific case thing...allow interpolating the st coords so we can map the texture
	//	properly as this segement progresses through it's life
	float	ST[2];
	float	destST[2];
	float	curST[2];
} effectTrailVertStruct_t;

typedef struct effectTrailArgStruct_s {
	effectTrailVertStruct_t		mVerts[4];
	qhandle_t					mShader;
	int							mSetFlags;
	int							mKillTime;
} effectTrailArgStruct_t;

typedef struct
{
	vec3_t start;
	vec3_t end;
	float size1;
	float size2;
	float sizeParm;
	float alpha1;
	float alpha2;
	float alphaParm;
	vec3_t sRGB;
	vec3_t eRGB;
	float rgbParm;
	float chaos;
	int killTime;
	qhandle_t shader;
	int flags;
} addElectricityArgStruct_t;

// if entityState->solid == SOLID_BMODEL, modelindex is an inline model number
#define	SOLID_BMODEL	0xffffff

typedef enum {
	TR_STATIONARY,
	TR_INTERPOLATE,				// non-parametric, but interpolate between snapshots
	TR_LINEAR,
	TR_LINEAR_STOP,
	TR_SINE,					// value = base + sin( time / duration ) * delta
	TR_GRAVITY,
	TR_HEAVYGRAVITY,
	TR_LIGHTGRAVITY
} trType_t;

typedef struct {
	trType_t	trType;
	int		trTime;
	int		trDuration;			// if non 0, trTime + trDuration = stop time
	vec3_t	trBase;
	vec3_t	trDelta;			// velocity, etc
} trajectory_t;

// entityState_t is the information conveyed from the server
// in an update message about entities that the client will
// need to render in some way
// Different eTypes may use the information in different ways
// The messages are delta compressed, so it doesn't really matter if
// the structure size is fairly large

typedef struct entityState_s 
{
	int				number;			// entity index
	int				eType;			// entityType_t
	int				eFlags;

	trajectory_t	pos;			// for calculating position
	trajectory_t	apos;			// for calculating angles

	int				time;
	int				time2;
					
	vec3_t			origin;
	vec3_t			origin2;
					
	vec3_t			angles;
	vec3_t			angles2;
					
	int				otherEntityNum;	// shotgun sources, etc
	int				otherEntityNum2;
					
	int				groundEntityNum;	// -1 = in air
					
	int				loopSound;		// constantly loop this sound
	int				mSoundSet;
										
	int				modelindex;
	int				modelindex2;
	int				clientNum;		// 0 to (MAX_CLIENTS - 1), for players and corpses
	int				frame;
					
	int				solid;			// for client side prediction, trap_linkentity sets this properly
					
	int				event;			// impulse events -- muzzle flashes, footsteps, etc
	int				eventParm;

	int				generic1;

	// for players
	// these fields are only transmitted for client entities!!!!!
	int				gametypeitems;	// bit flags indicating which items are carried
	int				weapon;			// determines weapon and flash model, etc
	int				legsAnim;		// mask off ANIM_TOGGLEBIT
	int				torsoAnim;		// mask off ANIM_TOGGLEBIT
	int				torsoTimer;		// time the animation will play for
	int				leanOffset;		// Lean direction
} entityState_t;

typedef enum {
	CA_UNINITIALIZED,
	CA_DISCONNECTED, 	// not talking to a server
	CA_AUTHORIZING,		// not used any more, was checking cd key 
	CA_CONNECTING,		// sending request packets to the server
	CA_CHALLENGING,		// sending challenge packets to the server
	CA_CONNECTED,		// netchan_t established, getting gamestate
	CA_LOADING,			// only during cgame initialization, never during main loop
	CA_PRIMED,			// got gamestate, waiting for first frame
	CA_ACTIVE,			// game views should be displayed
	CA_CINEMATIC		// playing a cinematic or a static pic, not connected to a server
} connstate_t;


#define Square(x) ((x)*(x))

// real time
//=============================================


typedef struct qtime_s {
	int tm_sec;     /* seconds after the minute - [0,59] */
	int tm_min;     /* minutes after the hour - [0,59] */
	int tm_hour;    /* hours since midnight - [0,23] */
	int tm_mday;    /* day of the month - [1,31] */
	int tm_mon;     /* months since January - [0,11] */
	int tm_year;    /* years since 1900 */
	int tm_wday;    /* days since Sunday - [0,6] */
	int tm_yday;    /* days since January 1 - [0,365] */
	int tm_isdst;   /* daylight savings time flag */
} qtime_t;


// server browser sources
#define AS_LOCAL			0
#define AS_GLOBAL			1
#define AS_FAVORITES		2


// cinematic states
typedef enum {
	FMV_IDLE,
	FMV_PLAY,		// play
	FMV_EOF,		// all other conditions, i.e. stop/EOF/abort
	FMV_ID_BLT,
	FMV_ID_IDLE,
	FMV_LOOPED,
	FMV_ID_WAIT
} e_status;

#define	MAX_GLOBAL_SERVERS			2048
#define	MAX_OTHER_SERVERS			128
#define MAX_PINGREQUESTS			32
#define MAX_SERVERSTATUSREQUESTS	16

#define SAY_ALL		0
#define SAY_TEAM	1
#define SAY_TELL	2

#define QRAND_MAX 32768

void Rand_Init(int seed);
float flrand(float min, float max);
int irand(int min, int max);
int Q_irand(int value1, int value2);

/*
Ghoul2 Insert Start
*/

typedef struct {
	float		matrix[3][4];
} mdxaBone_t;

// For ghoul2 axis use

typedef enum Eorientations
{
	ORIGIN = 0, 
	POSITIVE_X,
	POSITIVE_Z,
	POSITIVE_Y,
	NEGATIVE_X,
	NEGATIVE_Z,
	NEGATIVE_Y
} orientations_t;
/*
Ghoul2 Insert End
*/


// define the new memory tags for the zone, used by all modules now
//
#define TAGDEF(blah) TAG_ ## blah
typedef enum {
	#include "qcommon/tags.h"
} memtag;
typedef char memtag_t;

typedef struct 
{
	int		isValid;	
	void	*ghoul2;
	int		modelNum;
	int		boltNum;
	vec3_t	angles;
	vec3_t	origin;
	vec3_t	scale;
	vec3_t	dir;
	vec3_t	forward;

} CFxBoltInterface;

//rww - conveniently toggle "gore" code, for model decals and stuff.
#define _G2_GORE

// these are the actual shaders
typedef enum {
	PGORE_NONE,
	PGORE_ARMOR,
	PGORE_BULLETBIG,
	PGORE_KNIFESLASH,
	PGORE_PUNCTURE,
	PGORE_SHOTGUN,
	PGORE_SHOTGUNBIG,
	PGORE_IMMOLATE,
	PGORE_BURN,
	PGORE_SPURT,
	PGORE_SPLATTER,
	PGORE_BLOODY_GLASS,
	PGORE_BLOODY_GLASS_B,
	PGORE_BLOODY_ICK,
	PGORE_BLOODY_DROOP,
	PGORE_BLOODY_MAUL,
	PGORE_BLOODY_DROPS,
	PGORE_BULLET_E,
	PGORE_BULLET_F,
	PGORE_BULLET_G,
	PGORE_BULLET_H,
	PGORE_BULLET_I,
	PGORE_BULLET_J,
	PGORE_BULLET_K,
	PGORE_BLOODY_HAND,
	PGORE_POWDER_BURN_DENSE,
	PGORE_POWDER_BURN_CHUNKY,
	PGORE_KNIFESLASH2,
	PGORE_KNIFESLASH3,
	PGORE_CHUNKY_SPLAT,
	PGORE_BIG_SPLATTER,
	PGORE_BLOODY_SPLOTCH,
	PGORE_BLEEDER,
	PGORE_PELLETS,
	PGORE_KNIFE_SOAK,
	PGORE_BLEEDER_DENSE,
	PGORE_BLOODY_SPLOTCH2,
	PGORE_BLOODY_DRIPS,
	PGORE_DRIPPING_DOWN,
	PGORE_GUTSHOT,
	PGORE_SHRAPNEL,
	PGORE_COUNT
} goreEnum_t;

struct goreEnumShader_t
{
	int				maxLODBias;   //if r_lodBias (and the other 3 convars) =x then shaders with this larger than x will not be used
	goreEnum_t		shaderEnum;  // why is this even in here?
	char			shaderName[MAX_QPATH];
};

typedef struct SSkinGoreData_s
{
	vec3_t			angles;
	vec3_t			position;
	int				currentTime;
	int				entNum;
	vec3_t			rayDirection;	// in world space
	vec3_t			hitLocation;	// in world space
	vec3_t			scale;
	float			SSize;			// size of splotch in the S texture direction in world units
	float			TSize;			// size of splotch in the T texture direction in world units
	float			theta;			// angle to rotate the splotch

	// growing stuff
	int				growDuration;			// time over which we want this to scale up, set to -1 for no scaling
	float			goreScaleStartFraction; // fraction of the final size at which we want the gore to initially appear

	qboolean		frontFaces;
	qboolean		backFaces;
	qboolean		baseModelOnly;
	int				lifeTime;				// effect expires after this amount of time
	int				fadeOutTime;			//specify the duration of fading, from the lifeTime (e.g. 3000 will start fading 3 seconds before removal and be faded entirely by removal)
	int				shrinkOutTime;			// unimplemented
	float			alphaModulate;			// unimplemented
	vec3_t			tint;					// unimplemented
	float			impactStrength;			// unimplemented

	goreEnum_t		shaderEnum; // enum that'll get switched over to the shader's actual handle 

	int				myIndex; // used internally
} SSkinGoreData;

/*
========================================================================

String ID Tables

========================================================================
*/
#define ENUM2STRING(arg)   { #arg, arg }
typedef struct stringID_table_s
{
	char	*name;
	int		id;
} stringID_table_t;

int GetIDForString ( stringID_table_t *table, const char *string );
const char *GetStringForID( stringID_table_t *table, int id );


// stuff to help out during development process, force reloading/uncacheing of certain filetypes...
//
typedef enum
{
	eForceReload_NOTHING,
//	eForceReload_BSP,	// not used in MP codebase
	eForceReload_MODELS,
	eForceReload_ALL

} ForceReload_e;


void NET_AddrToString( char *out, size_t size, void *addr );
