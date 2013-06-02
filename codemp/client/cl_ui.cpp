//Anything above this #include will be ignored by the compiler
#include "qcommon/exe_headers.h"

#include "client.h"

#include "botlib/botlib.h"
#include "qcommon/stringed_ingame.h"

/*
Ghoul2 Insert Start
*/

#if !defined(G2_H_INC)
	#include "ghoul2/G2.h"
#endif

/*
Ghoul2 Insert End
*/

#ifdef VV_LIGHTING
#include "renderer/tr_lightmanager.h"
#endif

#if !defined(GENERICPARSER2_H_INC)
#include "qcommon/GenericParser2.h"
#endif

extern	botlib_export_t	*botlib_export;
void SP_Register(const char *Package);

vm_t *uivm;

#define MAX_POOL_SIZE	2048000

//I am using this for all the stuff like NPC client structures on server/client and
//non-humanoid animations as well until/if I can get dynamic memory working properly
//with casted datatypes, which is why it is so large.


static char		bg_pool[MAX_POOL_SIZE];
static int		bg_poolSize = 0;
static int		bg_poolTail = MAX_POOL_SIZE;

void *BG_Alloc ( int size )
{
	bg_poolSize = ((bg_poolSize + 0x00000003) & 0xfffffffc);

	if (bg_poolSize + size > bg_poolTail)
	{
		Com_Error( ERR_DROP, "BG_Alloc: buffer exceeded tail (%d > %d)", bg_poolSize + size, bg_poolTail);
		return 0;
	}

	bg_poolSize += size;

	return &bg_pool[bg_poolSize-size];
}

void *BG_AllocUnaligned ( int size )
{
	if (bg_poolSize + size > bg_poolTail)
	{
		Com_Error( ERR_DROP, "BG_AllocUnaligned: buffer exceeded tail (%d > %d)", bg_poolSize + size, bg_poolTail);
		return 0;
	}

	bg_poolSize += size;

	return &bg_pool[bg_poolSize-size];
}

void *BG_TempAlloc( int size )
{
	size = ((size + 0x00000003) & 0xfffffffc);

	if (bg_poolTail - size < bg_poolSize)
	{
		Com_Error( ERR_DROP, "BG_TempAlloc: buffer exceeded head (%d > %d)", bg_poolTail - size, bg_poolSize);
		return 0;
	}

	bg_poolTail -= size;

	return &bg_pool[bg_poolTail];
}

void BG_TempFree( int size )
{
	size = ((size + 0x00000003) & 0xfffffffc);

	if (bg_poolTail+size > MAX_POOL_SIZE)
	{
		Com_Error( ERR_DROP, "BG_TempFree: tail greater than size (%d > %d)", bg_poolTail+size, MAX_POOL_SIZE );
	}

	bg_poolTail += size;
}

char *BG_StringAlloc ( const char *source )
{
	char *dest = (char*)BG_Alloc( strlen ( source ) + 1 );
	strcpy( dest, source );
	return dest;
}

qboolean BG_OutOfMemory ( void )
{
	return (qboolean) (bg_poolSize >= MAX_POOL_SIZE);
}


/*
====================
GetClientState
====================
*/
static void GetClientState( uiClientState_t *state ) {
	state->connectPacketCount = clc.connectPacketCount;
	state->connState = cls.state;
	Q_strncpyz( state->servername, cls.servername, sizeof( state->servername ) );
	Q_strncpyz( state->updateInfoString, cls.updateInfoString, sizeof( state->updateInfoString ) );
	Q_strncpyz( state->messageString, clc.serverMessage, sizeof( state->messageString ) );
	state->clientNum = cl.snap.ps.clientNum;
}

/*
====================
LAN_LoadCachedServers
====================
*/
void LAN_LoadCachedServers( ) {
	int size;
	fileHandle_t fileIn;
	cls.numglobalservers = cls.nummplayerservers = cls.numfavoriteservers = 0;
	cls.numGlobalServerAddresses = 0;
	if (FS_SV_FOpenFileRead("servercache.dat", &fileIn)) {
		FS_Read(&cls.numglobalservers, sizeof(int), fileIn);
		FS_Read(&cls.nummplayerservers, sizeof(int), fileIn);
		FS_Read(&cls.numfavoriteservers, sizeof(int), fileIn);
		FS_Read(&size, sizeof(int), fileIn);
		if (size == sizeof(cls.globalServers) + sizeof(cls.favoriteServers) + sizeof(cls.mplayerServers)) {
			FS_Read(&cls.globalServers, sizeof(cls.globalServers), fileIn);
			FS_Read(&cls.mplayerServers, sizeof(cls.mplayerServers), fileIn);
			FS_Read(&cls.favoriteServers, sizeof(cls.favoriteServers), fileIn);
		} else {
			cls.numglobalservers = cls.nummplayerservers = cls.numfavoriteservers = 0;
			cls.numGlobalServerAddresses = 0;
		}
		FS_FCloseFile(fileIn);
	}
}

/*
====================
LAN_SaveServersToCache
====================
*/
void LAN_SaveServersToCache( ) {
	int size;
	fileHandle_t fileOut = FS_SV_FOpenFileWrite("servercache.dat");
	FS_Write(&cls.numglobalservers, sizeof(int), fileOut);
	FS_Write(&cls.nummplayerservers, sizeof(int), fileOut);
	FS_Write(&cls.numfavoriteservers, sizeof(int), fileOut);
	size = sizeof(cls.globalServers) + sizeof(cls.favoriteServers) + sizeof(cls.mplayerServers);
	FS_Write(&size, sizeof(int), fileOut);
	FS_Write(&cls.globalServers, sizeof(cls.globalServers), fileOut);
	FS_Write(&cls.mplayerServers, sizeof(cls.mplayerServers), fileOut);
	FS_Write(&cls.favoriteServers, sizeof(cls.favoriteServers), fileOut);
	FS_FCloseFile(fileOut);
}


/*
====================
LAN_ResetPings
====================
*/
static void LAN_ResetPings(int source) {
	int count,i;
	serverInfo_t *servers = NULL;
	count = 0;

	switch (source) {
		case AS_LOCAL :
			servers = &cls.localServers[0];
			count = MAX_OTHER_SERVERS;
			break;
		case AS_MPLAYER :
			servers = &cls.mplayerServers[0];
			count = MAX_OTHER_SERVERS;
			break;
		case AS_GLOBAL :
			servers = &cls.globalServers[0];
			count = MAX_GLOBAL_SERVERS;
			break;
		case AS_FAVORITES :
			servers = &cls.favoriteServers[0];
			count = MAX_OTHER_SERVERS;
			break;
	}
	if (servers) {
		for (i = 0; i < count; i++) {
			servers[i].ping = -1;
		}
	}
}

/*
====================
LAN_AddServer
====================
*/
static int LAN_AddServer(int source, const char *name, const char *address) {
	int max, *count, i;
	netadr_t adr;
	serverInfo_t *servers = NULL;
	max = MAX_OTHER_SERVERS;
	count = 0;

	switch (source) {
		case AS_LOCAL :
			count = &cls.numlocalservers;
			servers = &cls.localServers[0];
			break;
		case AS_MPLAYER :
			count = &cls.nummplayerservers;
			servers = &cls.mplayerServers[0];
			break;
		case AS_GLOBAL :
			max = MAX_GLOBAL_SERVERS;
			count = &cls.numglobalservers;
			servers = &cls.globalServers[0];
			break;
		case AS_FAVORITES :
			count = &cls.numfavoriteservers;
			servers = &cls.favoriteServers[0];
/*			if (!name || !*name)
			{
				name = "?";
			}
*/
			break;
	}
	if (servers && *count < max) {
		NET_StringToAdr( address, &adr );
		if (adr.type == NA_BAD)
		{
			return -1;
		}
		for ( i = 0; i < *count; i++ ) {
			if (NET_CompareAdr(servers[i].adr, adr)) {
				break;
			}
		}
		if (i >= *count) {
			servers[*count].adr = adr;
			Q_strncpyz(servers[*count].hostName, name, sizeof(servers[*count].hostName));
			servers[*count].visible = qtrue;
			(*count)++;
			return 1;
		}
		return 0;
	}
	return -1;
}

/*
====================
LAN_RemoveServer
====================
*/
static void LAN_RemoveServer(int source, const char *addr) {
	int *count, i;
	serverInfo_t *servers = NULL;
	count = 0;
	switch (source) {
		case AS_LOCAL :
			count = &cls.numlocalservers;
			servers = &cls.localServers[0];
			break;
		case AS_MPLAYER :
			count = &cls.nummplayerservers;
			servers = &cls.mplayerServers[0];
			break;
		case AS_GLOBAL :
			count = &cls.numglobalservers;
			servers = &cls.globalServers[0];
			break;
		case AS_FAVORITES :
			count = &cls.numfavoriteservers;
			servers = &cls.favoriteServers[0];
			break;
	}
	if (servers) {
		netadr_t comp;
		NET_StringToAdr( addr, &comp );
		for (i = 0; i < *count; i++) {
			if (servers[i].adr.type==NA_BAD || NET_CompareAdr( comp, servers[i].adr)) {
				int j = i;
				while (j < *count - 1) {
					Com_Memcpy(&servers[j], &servers[j+1], sizeof(servers[j]));
					j++;
				}
				(*count)--;
				break;
			}
		}
	}
}


/*
====================
LAN_GetServerCount
====================
*/
static int LAN_GetServerCount( int source ) {
	switch (source) {
		case AS_LOCAL :
			return cls.numlocalservers;
			break;
		case AS_MPLAYER :
			return cls.nummplayerservers;
			break;
		case AS_GLOBAL :
			return cls.numglobalservers;
			break;
		case AS_FAVORITES :
			return cls.numfavoriteservers;
			break;
	}
	return 0;
}

/*
====================
LAN_GetLocalServerAddressString
====================
*/
static void LAN_GetServerAddressString( int source, int n, char *buf, int buflen ) {
	switch (source) {
		case AS_LOCAL :
			if (n >= 0 && n < MAX_OTHER_SERVERS) {
				Q_strncpyz(buf, NET_AdrToString( cls.localServers[n].adr) , buflen );
				return;
			}
			break;
		case AS_MPLAYER :
			if (n >= 0 && n < MAX_OTHER_SERVERS) {
				Q_strncpyz(buf, NET_AdrToString( cls.mplayerServers[n].adr) , buflen );
				return;
			}
			break;
		case AS_GLOBAL :
			if (n >= 0 && n < MAX_GLOBAL_SERVERS) {
				Q_strncpyz(buf, NET_AdrToString( cls.globalServers[n].adr) , buflen );
				return;
			}
			break;
		case AS_FAVORITES :
			if (n >= 0 && n < MAX_OTHER_SERVERS) {
				Q_strncpyz(buf, NET_AdrToString( cls.favoriteServers[n].adr) , buflen );
				return;
			}
			break;
	}
	buf[0] = '\0';
}

/*
====================
LAN_GetServerInfo
====================
*/
static void LAN_GetServerInfo( int source, int n, char *buf, int buflen ) {
	char info[MAX_STRING_CHARS];
	serverInfo_t *server = NULL;
	info[0] = '\0';
	switch (source) {
		case AS_LOCAL :
			if (n >= 0 && n < MAX_OTHER_SERVERS) {
				server = &cls.localServers[n];
			}
			break;
		case AS_MPLAYER :
			if (n >= 0 && n < MAX_OTHER_SERVERS) {
				server = &cls.mplayerServers[n];
			}
			break;
		case AS_GLOBAL :
			if (n >= 0 && n < MAX_GLOBAL_SERVERS) {
				server = &cls.globalServers[n];
			}
			break;
		case AS_FAVORITES :
			if (n >= 0 && n < MAX_OTHER_SERVERS) {
				server = &cls.favoriteServers[n];
			}
			break;
	}
	if (server && buf) {
		buf[0] = '\0';
		Info_SetValueForKey( info, "hostname", server->hostName);
		Info_SetValueForKey( info, "mapname", server->mapName);
		Info_SetValueForKey( info, "clients", va("%i",server->clients));
		Info_SetValueForKey( info, "sv_maxclients", va("%i",server->maxClients));
		Info_SetValueForKey( info, "ping", va("%i",server->ping));
		Info_SetValueForKey( info, "minping", va("%i",server->minPing));
		Info_SetValueForKey( info, "maxping", va("%i",server->maxPing));
		Info_SetValueForKey( info, "nettype", va("%i",server->netType));
		Info_SetValueForKey( info, "needpass", va("%i", server->needPassword ) );
		Info_SetValueForKey( info, "game", server->game);
		Info_SetValueForKey( info, "gametype", server->gameType);
		Info_SetValueForKey( info, "addr", NET_AdrToString(server->adr));
		Info_SetValueForKey( info, "sv_allowAnonymous", va("%i", server->allowAnonymous));
//		Info_SetValueForKey( info, "pure", va("%i", server->pure ) );
		Q_strncpyz(buf, info, buflen);
	} else {
		if (buf) {
			buf[0] = '\0';
		}
	}
}

/*
====================
LAN_GetServerPing
====================
*/
static int LAN_GetServerPing( int source, int n ) {
	serverInfo_t *server = NULL;
	switch (source) {
		case AS_LOCAL :
			if (n >= 0 && n < MAX_OTHER_SERVERS) {
				server = &cls.localServers[n];
			}
			break;
		case AS_MPLAYER :
			if (n >= 0 && n < MAX_OTHER_SERVERS) {
				server = &cls.mplayerServers[n];
			}
			break;
		case AS_GLOBAL :
			if (n >= 0 && n < MAX_GLOBAL_SERVERS) {
				server = &cls.globalServers[n];
			}
			break;
		case AS_FAVORITES :
			if (n >= 0 && n < MAX_OTHER_SERVERS) {
				server = &cls.favoriteServers[n];
			}
			break;
	}
	if (server) {
		return server->ping;
	}
	return -1;
}

/*
====================
LAN_GetServerPtr
====================
*/
static serverInfo_t *LAN_GetServerPtr( int source, int n ) {
	switch (source) {
		case AS_LOCAL :
			if (n >= 0 && n < MAX_OTHER_SERVERS) {
				return &cls.localServers[n];
			}
			break;
		case AS_MPLAYER :
			if (n >= 0 && n < MAX_OTHER_SERVERS) {
				return &cls.mplayerServers[n];
			}
			break;
		case AS_GLOBAL :
			if (n >= 0 && n < MAX_GLOBAL_SERVERS) {
				return &cls.globalServers[n];
			}
			break;
		case AS_FAVORITES :
			if (n >= 0 && n < MAX_OTHER_SERVERS) {
				return &cls.favoriteServers[n];
			}
			break;
	}
	return NULL;
}

/*
====================
LAN_CompareServers
====================
*/
static int LAN_CompareServers( int source, int sortKey, int sortDir, int s1, int s2 ) {
	int res;
	serverInfo_t *server1, *server2;

	server1 = LAN_GetServerPtr(source, s1);
	server2 = LAN_GetServerPtr(source, s2);
	if (!server1 || !server2) {
		return 0;
	}

	res = 0;
	switch( sortKey ) {
		case SORT_HOST:
			res = Q_stricmp( server1->hostName, server2->hostName );
			break;

		case SORT_MAP:
			res = Q_stricmp( server1->mapName, server2->mapName );
			break;
		case SORT_CLIENTS:
			if (server1->clients < server2->clients) {
				res = -1;
			}
			else if (server1->clients > server2->clients) {
				res = 1;
			}
			else {
				res = 0;
			}
			break;
		case SORT_GAME:
			if (server1->gameType < server2->gameType) {
				res = -1;
			}
			else if (server1->gameType > server2->gameType) {
				res = 1;
			}
			else {
				res = 0;
			}
			break;
		case SORT_PING:
			if (server1->ping < server2->ping) {
				res = -1;
			}
			else if (server1->ping > server2->ping) {
				res = 1;
			}
			else {
				res = 0;
			}
			break;
	}

	if (sortDir) {
		if (res < 0)
			return 1;
		if (res > 0)
			return -1;
		return 0;
	}
	return res;
}

/*
====================
LAN_GetPingQueueCount
====================
*/
static int LAN_GetPingQueueCount( void ) {
	return (CL_GetPingQueueCount());
}

/*
====================
LAN_ClearPing
====================
*/
static void LAN_ClearPing( int n ) {
	CL_ClearPing( n );
}

/*
====================
LAN_GetPing
====================
*/
static void LAN_GetPing( int n, char *buf, int buflen, int *pingtime ) {
	CL_GetPing( n, buf, buflen, pingtime );
}

/*
====================
LAN_GetPingInfo
====================
*/
static void LAN_GetPingInfo( int n, char *buf, int buflen ) {
	CL_GetPingInfo( n, buf, buflen );
}

/*
====================
LAN_MarkServerVisible
====================
*/
static void LAN_MarkServerVisible(int source, int n, qboolean visible ) {
	if (n == -1) {
		int count = MAX_OTHER_SERVERS;
		serverInfo_t *server = NULL;
		switch (source) {
			case AS_LOCAL :
				server = &cls.localServers[0];
				break;
			case AS_MPLAYER :
				server = &cls.mplayerServers[0];
				break;
			case AS_GLOBAL :
				server = &cls.globalServers[0];
				count = MAX_GLOBAL_SERVERS;
				break;
			case AS_FAVORITES :
				server = &cls.favoriteServers[0];
				break;
		}
		if (server) {
			for (n = 0; n < count; n++) {
				server[n].visible = visible;
			}
		}

	} else {
		switch (source) {
			case AS_LOCAL :
				if (n >= 0 && n < MAX_OTHER_SERVERS) {
					cls.localServers[n].visible = visible;
				}
				break;
			case AS_MPLAYER :
				if (n >= 0 && n < MAX_OTHER_SERVERS) {
					cls.mplayerServers[n].visible = visible;
				}
				break;
			case AS_GLOBAL :
				if (n >= 0 && n < MAX_GLOBAL_SERVERS) {
					cls.globalServers[n].visible = visible;
				}
				break;
			case AS_FAVORITES :
				if (n >= 0 && n < MAX_OTHER_SERVERS) {
					cls.favoriteServers[n].visible = visible;
				}
				break;
		}
	}
}


/*
=======================
LAN_ServerIsVisible
=======================
*/
static int LAN_ServerIsVisible(int source, int n ) {
	switch (source) {
		case AS_LOCAL :
			if (n >= 0 && n < MAX_OTHER_SERVERS) {
				return cls.localServers[n].visible;
			}
			break;
		case AS_MPLAYER :
			if (n >= 0 && n < MAX_OTHER_SERVERS) {
				return cls.mplayerServers[n].visible;
			}
			break;
		case AS_GLOBAL :
			if (n >= 0 && n < MAX_GLOBAL_SERVERS) {
				return cls.globalServers[n].visible;
			}
			break;
		case AS_FAVORITES :
			if (n >= 0 && n < MAX_OTHER_SERVERS) {
				return cls.favoriteServers[n].visible;
			}
			break;
	}
	return qfalse;
}

/*
=======================
LAN_UpdateVisiblePings
=======================
*/
qboolean LAN_UpdateVisiblePings(int source ) {
	return CL_UpdateVisiblePings_f(source);
}

/*
====================
LAN_GetServerStatus
====================
*/
int LAN_GetServerStatus( char *serverAddress, char *serverStatus, int maxLen ) {
	return CL_ServerStatus( serverAddress, serverStatus, maxLen );
}

/*
====================
CL_GetGlConfig
====================
*/
static void CL_GetGlconfig( glconfig_t *config ) {
	*config = cls.glconfig;
}

/*
====================
GetClipboardData
====================
*/
static void GetClipboardData( char *buf, int buflen ) {
	char	*cbd;

	cbd = Sys_GetClipboardData();

	if ( !cbd ) {
		*buf = 0;
		return;
	}

	Q_strncpyz( buf, cbd, buflen );

	Z_Free( cbd );
}

/*
====================
Key_KeynumToStringBuf
====================
*/
// only ever called by binding-display code, therefore returns non-technical "friendly" names 
//	in any language that don't necessarily match those in the config file...
//
void Key_KeynumToStringBuf( int keynum, char *buf, int buflen ) 
{
	const char *psKeyName = Key_KeynumToString( keynum/*, qtrue */);

	// see if there's a more friendly (or localised) name...
	//
	const char *psKeyNameFriendly = SE_GetString( va("KEYNAMES_KEYNAME_%s",psKeyName) );

	Q_strncpyz( buf, (psKeyNameFriendly && psKeyNameFriendly[0]) ? psKeyNameFriendly : psKeyName, buflen );
}


/*
====================
Key_GetBindingBuf
====================
*/
static void Key_GetBindingBuf( int keynum, char *buf, int buflen ) {
	char	*value;

	value = Key_GetBinding( keynum );
	if ( value ) {
		Q_strncpyz( buf, value, buflen );
	}
	else {
		*buf = 0;
	}
}

/*
====================
Key_GetCatcher
====================
*/
int Key_GetCatcher( void ) {
	return cls.keyCatchers;
}

/*
====================
Ket_SetCatcher
====================
*/
void Key_SetCatcher( int catcher ) {
	cls.keyCatchers = catcher;
}


/*
====================
GetConfigString
====================
*/
static int GetConfigString(int index, char *buf, int size)
{
	int		offset;

	if (index < 0 || index >= MAX_CONFIGSTRINGS)
		return qfalse;

	offset = cl.gameState.stringOffsets[index];
	if (!offset) {
		if( size ) {
			buf[0] = 0;
		}
		return qfalse;
	}

	Q_strncpyz( buf, cl.gameState.stringData+offset, size);
 
	return qtrue;
}

/*
====================
FloatAsInt
====================
*/
static int FloatAsInt( float f ) {
	int		temp;

	*(float *)&temp = f;

	return temp;
}

void *VM_ArgPtr( int intValue );
#define	VMA(x) VM_ArgPtr(args[x])
#define	VMF(x)	((float *)args)[x]

/*
====================
CL_UISystemCalls

The ui module is making a system call
====================
*/
int CL_UISystemCalls( int *args ) {
	switch( args[0] ) {
	//rww - alright, DO NOT EVER add a GAME/CGAME/UI generic call without adding a trap to match, and
	//all of these traps must be shared and have cases in sv_game, cl_cgame, and cl_ui. They must also
	//all be in the same order, and start at 100.
	case TRAP_MEMSET:
		Com_Memset( VMA(1), args[2], args[3] );
		return 0;
	case TRAP_MEMCPY:
		Com_Memcpy( VMA(1), VMA(2), args[3] );
		return 0;
	case TRAP_STRNCPY:
		return (int)strncpy( (char *)VMA(1), (const char *)VMA(2), args[3] );
	case TRAP_SIN:
		return FloatAsInt( sin( VMF(1) ) );
	case TRAP_COS:
		return FloatAsInt( cos( VMF(1) ) );
	case TRAP_ATAN2:
		return FloatAsInt( atan2( VMF(1), VMF(2) ) );
	case TRAP_SQRT:
		return FloatAsInt( sqrt( VMF(1) ) );
	case TRAP_MATRIXMULTIPLY:
		MatrixMultiply( (vec3_t *)VMA(1), (vec3_t *)VMA(2), (vec3_t *)VMA(3) );
		return 0;
	case TRAP_ANGLEVECTORS:
		AngleVectors( (const float *)VMA(1), (float *)VMA(2), (float *)VMA(3), (float *)VMA(4) );
		return 0;
	case TRAP_PERPENDICULARVECTOR:
		PerpendicularVector( (float *)VMA(1), (const float *)VMA(2) );
		return 0;
	case TRAP_FLOOR:
		return FloatAsInt( floor( VMF(1) ) );
	case TRAP_CEIL:
		return FloatAsInt( ceil( VMF(1) ) );
	case TRAP_TESTPRINTINT:
		return 0;
	case TRAP_TESTPRINTFLOAT:
		return 0;
	case TRAP_ACOS:
		return FloatAsInt( Q_acos( VMF(1) ) );
	case TRAP_ASIN:
		return FloatAsInt( Q_asin( VMF(1) ) );


	case UI_ERROR:
		Com_Error( ERR_DROP, "%s", VMA(1) );
		return 0;

	case UI_PRINT:
		Com_Printf( "%s", VMA(1) );
		return 0;

	case UI_MILLISECONDS:
		return Sys_Milliseconds();

	case UI_CVAR_REGISTER:
		Cvar_Register( (vmCvar_t *)VMA(1), (const char *)VMA(2), (const char *)VMA(3), args[4] ); 
		return 0;

	case UI_CVAR_UPDATE:
		Cvar_Update( (vmCvar_t *)VMA(1) );
		return 0;

	case UI_CVAR_SET:
		Cvar_Set( (const char *)VMA(1), (const char *)VMA(2) );
		return 0;

	case UI_CVAR_VARIABLEVALUE:
		return FloatAsInt( Cvar_VariableValue( (const char *)VMA(1) ) );

	case UI_CVAR_VARIABLESTRINGBUFFER:
		Cvar_VariableStringBuffer( (const char *)VMA(1), (char *)VMA(2), args[3] );
		return 0;

	case UI_CVAR_SETVALUE:
		Cvar_SetValue( (const char *)VMA(1), VMF(2) );
		return 0;

	case UI_CVAR_RESET:
		Cvar_Reset( (const char *)VMA(1) );
		return 0;

	case UI_CVAR_CREATE:
		Cvar_Get( (const char *)VMA(1), (const char *)VMA(2), args[3] );
		return 0;

	case UI_CVAR_INFOSTRINGBUFFER:
		Cvar_InfoStringBuffer( args[1], (char *)VMA(2), args[3] );
		return 0;

	case UI_ARGC:
		return Cmd_Argc();

	case UI_ARGV:
		Cmd_ArgvBuffer( args[1], (char *)VMA(2), args[3] );
		return 0;

	case UI_CMD_EXECUTETEXT:
		Cbuf_ExecuteText( args[1], (const char *)VMA(2) );
		return 0;

	case UI_FS_FOPENFILE:
		return FS_FOpenFileByMode( (const char *)VMA(1), (int *)VMA(2), (fsMode_t)args[3] );

	case UI_FS_READ:
		FS_Read2( VMA(1), args[2], args[3] );
		return 0;

	case UI_FS_WRITE:
		FS_Write( VMA(1), args[2], args[3] );
		return 0;

	case UI_FS_FCLOSEFILE:
		FS_FCloseFile( args[1] );
		return 0;

	case UI_FS_GETFILELIST:
		return FS_GetFileList( (const char *)VMA(1), (const char *)VMA(2), (char *)VMA(3), args[4] );

	case UI_R_REGISTERMODEL:
		return re.RegisterModel( (const char *)VMA(1) );

	case UI_R_REGISTERSKIN:
		//KLAAS TODO
		return re.RegisterSkin( (const char *)VMA(1), 0, NULL );

	case UI_R_REGISTERSHADERNOMIP:
		return re.RegisterShaderNoMip( (const char *)VMA(1) );

	case UI_R_CLEARSCENE:
		re.ClearScene();
		return 0;

	case UI_R_ADDREFENTITYTOSCENE:
		re.AddRefEntityToScene( (const refEntity_t *)VMA(1) );
		return 0;

	case UI_R_ADDPOLYTOSCENE:
		//KLAAS TODO
		re.AddPolyToScene( args[1], args[2], (const polyVert_t *)VMA(3), 1 );
		return 0;

	case UI_R_ADDLIGHTTOSCENE:
#ifdef VV_LIGHTING
		VVLightMan.RE_AddLightToScene( (const float *)VMA(1), VMF(2), VMF(3), VMF(4), VMF(5) );
#else
		re.AddLightToScene( (const float *)VMA(1), VMF(2), VMF(3), VMF(4), VMF(5) );
#endif
		return 0;

	case UI_R_RENDERSCENE:
		re.RenderScene( (const refdef_t *)VMA(1) );
		return 0;

	case UI_R_SETCOLOR:
		re.SetColor( (const float *)VMA(1) );
		return 0;

	case UI_R_DRAWSTRETCHPIC:
		re.DrawStretchPic( VMF(1), VMF(2), VMF(3), VMF(4), VMF(5), VMF(6), VMF(7), VMF(8), args[10] );
		return 0;

  case UI_R_MODELBOUNDS:
		re.ModelBounds( args[1], (float *)VMA(2), (float *)VMA(3) );
		return 0;

	case UI_UPDATESCREEN:
		if ( args[1] )
		{
			// draw loading screen
			VM_Call( uivm, UI_REFRESH, cls.realtime );
			VM_Call( uivm, UI_DRAW_LOADING_SCREEN );
			re.BeginFrame( STEREO_CENTER );
			re.EndFrame( NULL, NULL );
		}
		else
		{
			SCR_UpdateScreen();
		}
		return 0;

	case UI_CM_LERPTAG:
		re.LerpTag( (orientation_t *)VMA(1), args[2], args[3], args[4], VMF(5), (const char *)VMA(6) );
		return 0;

	case UI_S_REGISTERSOUND:
		return S_RegisterSound( (const char *)VMA(1) );

	case UI_S_STARTLOCALSOUND:
		S_StartLocalSound( args[1], args[2] );
		return 0;

	case UI_KEY_KEYNUMTOSTRINGBUF:
		Key_KeynumToStringBuf( args[1], (char *)VMA(2), args[3] );
		return 0;

	case UI_KEY_GETBINDINGBUF:
		Key_GetBindingBuf( args[1], (char *)VMA(2), args[3] );
		return 0;

	case UI_KEY_SETBINDING:
		Key_SetBinding( args[1], (const char *)VMA(2) );
		return 0;

	case UI_KEY_ISDOWN:
		return Key_IsDown( args[1] );

	case UI_KEY_GETOVERSTRIKEMODE:
		return Key_GetOverstrikeMode();

	case UI_KEY_SETOVERSTRIKEMODE:
		Key_SetOverstrikeMode( (qboolean)args[1] );
		return 0;

	case UI_KEY_CLEARSTATES:
		Key_ClearStates();
		return 0;

	case UI_KEY_GETCATCHER:
		return Key_GetCatcher();

	case UI_KEY_SETCATCHER:
		Key_SetCatcher( args[1] );
		return 0;

	case UI_GETCLIPBOARDDATA:
		GetClipboardData( (char *)VMA(1), args[2] );
		return 0;

	case UI_GETCLIENTSTATE:
		GetClientState( (uiClientState_t *)VMA(1) );
		return 0;		

	case UI_GETGLCONFIG:
		CL_GetGlconfig( (glconfig_t *)VMA(1) );
		return 0;

	case UI_GETCONFIGSTRING:
		return GetConfigString( args[1], (char *)VMA(2), args[3] );

	case UI_NET_AVAILABLE:
		return qtrue;

	case UI_LAN_LOADCACHEDSERVERS:
		LAN_LoadCachedServers();
		return 0;

	case UI_LAN_SAVECACHEDSERVERS:
		LAN_SaveServersToCache();
		return 0;

	case UI_LAN_ADDSERVER:
		return LAN_AddServer(args[1], (const char *)VMA(2), (const char *)VMA(3));

	case UI_LAN_REMOVESERVER:
		LAN_RemoveServer(args[1], (const char *)VMA(2));
		return 0;

	case UI_LAN_GETPINGQUEUECOUNT:
		return LAN_GetPingQueueCount();

	case UI_LAN_CLEARPING:
		LAN_ClearPing( args[1] );
		return 0;

	case UI_LAN_GETPING:
		LAN_GetPing( args[1], (char *)VMA(2), args[3], (int *)VMA(4) );
		return 0;

	case UI_LAN_GETPINGINFO:
		LAN_GetPingInfo( args[1], (char *)VMA(2), args[3] );
		return 0;

	case UI_LAN_GETSERVERCOUNT:
		return LAN_GetServerCount(args[1]);

	case UI_LAN_GETSERVERADDRESSSTRING:
		LAN_GetServerAddressString( args[1], args[2], (char *)VMA(3), args[4] );
		return 0;

	case UI_LAN_GETSERVERINFO:
		LAN_GetServerInfo( args[1], args[2], (char *)VMA(3), args[4] );
		return 0;

	case UI_LAN_GETSERVERPING:
		return LAN_GetServerPing( args[1], args[2] );

	case UI_LAN_MARKSERVERVISIBLE:
		LAN_MarkServerVisible( args[1], args[2], (qboolean)args[3] );
		return 0;

	case UI_LAN_SERVERISVISIBLE:
		return LAN_ServerIsVisible( args[1], args[2] );

	case UI_LAN_UPDATEVISIBLEPINGS:
		return LAN_UpdateVisiblePings( args[1] );

	case UI_LAN_RESETPINGS:
		LAN_ResetPings( args[1] );
		return 0;

	case UI_LAN_SERVERSTATUS:
		return LAN_GetServerStatus( (char *)VMA(1), (char *)VMA(2), args[3] );

	case UI_LAN_COMPARESERVERS:
		return LAN_CompareServers( args[1], args[2], args[3], args[4], args[5] );

	case UI_MEMORY_REMAINING:
		return Hunk_MemoryRemaining();

	case UI_R_REGISTERFONT:
		return re.RegisterFont( (const char *)VMA(1) );

	case UI_R_GETTEXTWIDTH:
		return re.Font_StrLenPixels((const char *)VMA(1), args[2], VMF(3));

	case UI_R_GETTEXTHEIGHT:
		return re.Font_HeightPixels(args[2], VMF(3));
		
	case UI_R_DRAWTEXTWITHCURSOR:
		re.Font_DrawString(args[1], args[2], args[3], VMF(4), (vec_t *)VMA(5), (const char *)VMA(6), args[7], args[8], args[9], args[10]);
		return 0;

	case UI_R_DRAWTEXT:
		re.Font_DrawString(args[1], args[2], args[3], VMF(4), (vec_t *)VMA(5), (const char *)VMA(6), args[7], args[8], 0, 0);
		return 0;

	case UI_PC_ADD_GLOBAL_DEFINE:
		return botlib_export->PC_AddGlobalDefine( (char *)VMA(1) );
	case UI_PC_LOAD_SOURCE:
		return botlib_export->PC_LoadSourceHandle( (const char *)VMA(1) );
	case UI_PC_FREE_SOURCE:
		return botlib_export->PC_FreeSourceHandle( args[1] );
	case UI_PC_READ_TOKEN:
		return botlib_export->PC_ReadTokenHandle( args[1], (struct pc_token_s *)VMA(2) );
	case UI_PC_SOURCE_FILE_AND_LINE:
		return botlib_export->PC_SourceFileAndLine( args[1], (char *)VMA(2), (int *)VMA(3) );
	case UI_PC_LOAD_GLOBAL_DEFINES:
		return botlib_export->PC_LoadGlobalDefines ( (char *)VMA(1) );
	case UI_PC_REMOVE_ALL_GLOBAL_DEFINES:
		botlib_export->PC_RemoveAllGlobalDefines ( );
		return 0;

	case UI_S_STOPBACKGROUNDTRACK:
		S_StopBackgroundTrack();
		return 0;
	case UI_S_STARTBACKGROUNDTRACK:
		S_StartBackgroundTrack( (const char *)VMA(1), (const char *)VMA(2), qfalse);
		return 0;

	case UI_REAL_TIME:
		return Com_RealTime( (struct qtime_s *)VMA(1) );

	case UI_CIN_PLAYCINEMATIC:
	  Com_DPrintf("UI_CIN_PlayCinematic\n");
	  return CIN_PlayCinematic((const char *)VMA(1), args[2], args[3], args[4], args[5], args[6]);

	case UI_CIN_STOPCINEMATIC:
	  return CIN_StopCinematic(args[1]);

	case UI_CIN_RUNCINEMATIC:
	  return CIN_RunCinematic(args[1]);

	case UI_CIN_DRAWCINEMATIC:
	  CIN_DrawCinematic(args[1]);
	  return 0;

	case UI_CIN_SETEXTENTS:
	  CIN_SetExtents(args[1], args[2], args[3], args[4], args[5]);
	  return 0;

	case UI_R_REMAP_SHADER:
		re.RemapShader( (const char *)VMA(1), (const char *)VMA(2), (const char *)VMA(3) );
		return 0;

/*
Ghoul2 Insert Start
*/
		
	case UI_G2_LISTSURFACES:
		re.G2API_ListSurfaces( (CGhoul2Info *) args[1] );
		return 0;

	case UI_G2_LISTBONES:
		re.G2API_ListBones( (CGhoul2Info *) args[1], args[2]);
		return 0;

	case UI_G2_HAVEWEGHOULMODELS:
		return re.G2API_HaveWeGhoul2Models( *((CGhoul2Info_v *)args[1]) );

	case UI_G2_SETMODELS:
		re.G2API_SetGhoul2ModelIndexes( *((CGhoul2Info_v *)args[1]),(qhandle_t *)VMA(2),(qhandle_t *)VMA(3));
		return 0;

	case UI_G2_GETBOLT:
		return re.G2API_GetBoltMatrix(*((CGhoul2Info_v *)args[1]), args[2], args[3], (mdxaBone_t *)VMA(4), (const float *)VMA(5),(const float *)VMA(6), args[7], (qhandle_t *)VMA(8), (float *)VMA(9));

	/*case UI_G2_GETBOLT_NOREC:
		re.G2API_BoltMatrixReconstruction( qfalse );//gG2_GBMNoReconstruct = qtrue;
		return re.G2API_GetBoltMatrix(*((CGhoul2Info_v *)args[1]), args[2], args[3], (mdxaBone_t *)VMA(4), (const float *)VMA(5),(const float *)VMA(6), args[7], (qhandle_t *)VMA(8), (float *)VMA(9));

	case UI_G2_GETBOLT_NOREC_NOROT:
		//RAZFIXME: cgame reconstructs bolt matrix, why is this different?
		re.G2API_BoltMatrixReconstruction( qfalse );//gG2_GBMNoReconstruct = qtrue;
		re.G2API_BoltMatrixSPMethod( qtrue );//gG2_GBMUseSPMethod = qtrue;
		return re.G2API_GetBoltMatrix(*((CGhoul2Info_v *)args[1]), args[2], args[3], (mdxaBone_t *)VMA(4), (const float *)VMA(5),(const float *)VMA(6), args[7], (qhandle_t *)VMA(8), (float *)VMA(9));*/

	case UI_G2_INITGHOUL2MODEL:
#ifdef _FULL_G2_LEAK_CHECKING
		g_G2AllocServer = 0;
#endif
		return	re.G2API_InitGhoul2Model((CGhoul2Info_v **)VMA(1), (const char *)VMA(2), args[3], (qhandle_t) args[4],
									  (qhandle_t) args[5], args[6], args[7]);


	/*case UI_G2_COLLISIONDETECT:
	case UI_G2_COLLISIONDETECTCACHE:
		return 0; //not supported for ui*/

	case UI_G2_ANGLEOVERRIDE:
		return re.G2API_SetBoneAngles(*((CGhoul2Info_v *)args[1]), args[2], (const char *)VMA(3), (float *)VMA(4), args[5],
							 (const Eorientations) args[6], (const Eorientations) args[7], (const Eorientations) args[8],
							 (qhandle_t *)VMA(9), args[10], args[11] );
	
	case UI_G2_CLEANMODELS:
#ifdef _FULL_G2_LEAK_CHECKING
		g_G2AllocServer = 0;
#endif
		re.G2API_CleanGhoul2Models((CGhoul2Info_v **)VMA(1));
		return 0;

	case UI_G2_PLAYANIM:
		return re.G2API_SetBoneAnim(*((CGhoul2Info_v *)args[1]), args[2], (const char *)VMA(3), args[4], args[5],
								args[6], VMF(7), args[8], VMF(9), args[10]);

	/*case UI_G2_GETBONEANIM:
		{
			CGhoul2Info_v &g2 = *((CGhoul2Info_v *)args[1]);
			int modelIndex = args[10];

			return re.G2API_GetBoneAnim(&g2[modelIndex], (const char*)VMA(2), args[3], (float *)VMA(4), (int *)VMA(5),
								(int *)VMA(6), (int *)VMA(7), (float *)VMA(8), (int *)VMA(9));
		}

	case UI_G2_GETBONEFRAME:
		{ //rwwFIXMEFIXME: Just make a G2API_GetBoneFrame func too. This is dirty.
			CGhoul2Info_v &g2 = *((CGhoul2Info_v *)args[1]);
			int modelIndex = args[6];
			int iDontCare1 = 0, iDontCare2 = 0, iDontCare3 = 0;
			float fDontCare1 = 0;

			return re.G2API_GetBoneAnim(&g2[modelIndex], (const char*)VMA(2), args[3], (float *)VMA(4), &iDontCare1,
								&iDontCare2, &iDontCare3, &fDontCare1, (int *)VMA(5));
		}*/

	case UI_G2_GETGLANAME:
		return (int)re.G2API_GetGLAName(*((CGhoul2Info_v *)args[1]), args[2]);
		
	case UI_G2_COPYGHOUL2INSTANCE:
		return (int)re.G2API_CopyGhoul2Instance(*((CGhoul2Info_v *)args[1]), *((CGhoul2Info_v *)args[2]), args[3]);

	case UI_G2_COPYSPECIFICGHOUL2MODEL:
		re.G2API_CopySpecificG2Model(*((CGhoul2Info_v *)args[1]), args[2], *((CGhoul2Info_v *)args[3]), args[4]);
		return 0;

	case UI_G2_DUPLICATEGHOUL2INSTANCE:
#ifdef _FULL_G2_LEAK_CHECKING
		g_G2AllocServer = 0;
#endif
		re.G2API_DuplicateGhoul2Instance(*((CGhoul2Info_v *)args[1]), (CGhoul2Info_v **)VMA(2));
		return 0;

	case UI_G2_REMOVEGHOUL2MODEL:
#ifdef _FULL_G2_LEAK_CHECKING
		g_G2AllocServer = 0;
#endif
		return (int)re.G2API_RemoveGhoul2Model((CGhoul2Info_v **)VMA(1), args[2]);

	case UI_G2_ADDBOLT:
		return re.G2API_AddBolt(*((CGhoul2Info_v *)args[1]), args[2], (const char *)VMA(3));

	case UI_G2_REMOVEBOLT:
		{
			CGhoul2Info_v &g2 = *((CGhoul2Info_v *)args[1]);
			int modelIndex = args[2];
			return re.G2API_RemoveBolt(&g2[modelIndex], args[3]);
		}

	/*case UI_G2_SETBOLTON:
		re.G2API_SetBoltInfo(*((CGhoul2Info_v *)args[1]), args[2], args[3]);
		return 0;*/

#ifdef _SOF2	
	case UI_G2_ADDSKINGORE:
		re.G2API_AddSkinGore(*((CGhoul2Info_v *)args[1]),*(SSkinGoreData *)VMA(2));
		return 0;
#endif // _SOF2
	/*case UI_G2_SETROOTSURFACE:
		return re.G2API_SetRootSurface(*((CGhoul2Info_v *)args[1]), args[2], (const char *)VMA(3));*/

	case UI_G2_SETSURFACEONOFF:
		{
			CGhoul2Info_v &g2 = *((CGhoul2Info_v *)args[1]);
			//KLAAS TODO
			//int modelIndex = args[2];
			return re.G2API_SetSurfaceOnOff(g2, (const char *)VMA(3), args[4]);
		}

		return 0;
	case UI_G2_SETSKIN:
		{
			CGhoul2Info_v &g2 = *((CGhoul2Info_v *)args[1]);
			int modelIndex = args[2];
			return re.G2API_SetSkin(&g2[modelIndex], args[3], 0);
		}

	case UI_G2_ATTACHG2MODEL:
		{
			CGhoul2Info_v *g2From = ((CGhoul2Info_v *)args[1]);
			CGhoul2Info_v *g2To = ((CGhoul2Info_v *)args[3]);
			
			return re.G2API_AttachG2Model(*g2From, args[2], *g2To, args[4], args[5]);
		}
	case UI_G2_GETANIMFILENAMEINDEX:
		{
			CGhoul2Info_v &ghoul2 = *((CGhoul2Info_v *)args[1]);
			qhandle_t modelIndex = (qhandle_t) args[2];
			char * srcFilename;
			qboolean retval = re.G2API_GetAnimFileName(&ghoul2[modelIndex], &srcFilename);
			strncpy((char *) VMA(3), srcFilename, MAX_QPATH);
			return (int) retval;
		}

	case UI_G2_REGISTERSKIN:
		return re.RegisterSkin((const char *)VMA(1), args[2], (char *)VMA(3) );
/*
Ghoul2 Insert End
*/

	case UI_GP_PARSE:
		return (int)GP_Parse((char **) VMA(1), (bool) args[2], (bool) args[3]);
	case UI_GP_PARSE_FILE:
		{
			char * data;
			FS_ReadFile((char *) args[1], (void **) &data);
			return (int)GP_Parse(&data, (bool) args[2], (bool) args[3]);
		}
	case UI_GP_CLEAN:
		GP_Clean((TGenericParser2) args[1]);
		return 0;
	case UI_GP_DELETE:
		GP_Delete((TGenericParser2 *) args[1]);
		return 0;
	case UI_GP_GET_BASE_PARSE_GROUP:
		return (int)GP_GetBaseParseGroup((TGenericParser2) args[1]);

	case UI_VM_LOCALALLOC:
		return (int)BG_Alloc((int) args[1]);
	case UI_VM_LOCALALLOCUNALIGNED:
		return (int)BG_AllocUnaligned((int) args[1]);
	case UI_VM_LOCALTEMPALLOC:
		return (int)BG_TempAlloc((int) args[1]);
	case UI_VM_LOCALTEMPFREE:
		BG_TempFree((int) args[1]);
		return 0;
	case UI_VM_LOCALSTRINGALLOC:
		return (int)BG_StringAlloc((char *) VMA(1));

	case UI_GET_CDKEY:
		return 0;
	case UI_SET_CDKEY:
		return 0;
	case UI_VERIFY_CDKEY:
		return 1;

	case UI_GPG_GET_NAME:
		return (int)GPG_GetName((TGPGroup) args[1], (char *) VMA(2));
	case UI_GPG_GET_NEXT:
		return (int)GPG_GetNext((TGPGroup) args[1]);
	case UI_GPG_GET_INORDER_NEXT:
		return (int)GPG_GetInOrderNext((TGPGroup) args[1]);
	case UI_GPG_GET_INORDER_PREVIOUS:
		return (int)GPG_GetInOrderPrevious((TGPGroup) args[1]);
	case UI_GPG_GET_PAIRS:
		return (int)GPG_GetPairs((TGPGroup) args[1]);
	case UI_GPG_GET_INORDER_PAIRS:
		return (int)GPG_GetInOrderPairs((TGPGroup) args[1]);
	case UI_GPG_GET_SUBGROUPS:
		return (int)GPG_GetSubGroups((TGPGroup) args[1]);
	case UI_GPG_GET_INORDER_SUBGROUPS:
		return (int)GPG_GetInOrderSubGroups((TGPGroup) args[1]);
	case UI_GPG_FIND_SUBGROUP:
		return (int)GPG_FindSubGroup((TGPGroup) args[1], (char *) VMA(2));
	case UI_GPG_FIND_PAIR:
		return (int)GPG_FindPair((TGPGroup) args[1], (const char *) VMA(2));
	case UI_GPG_FIND_PAIRVALUE:
		return (int)GPG_FindPairValue((TGPGroup) args[1], (const char *) VMA(2), (const char *) VMA(3), (char *) VMA(4));
		
	case UI_GPV_GET_NAME:
		return (int)GPV_GetName((TGPValue) args[1], (char *) VMA(2));
	case UI_GPV_GET_NEXT:
		return (int)GPV_GetNext((TGPValue) args[1]);
	case UI_GPV_GET_INORDER_NEXT:
		return (int)GPV_GetInOrderNext((TGPValue) args[1]);
	case UI_GPV_GET_INORDER_PREVIOUS:
		return (int)GPV_GetInOrderPrevious((TGPValue) args[1]);

	case UI_GPV_IS_LIST:
		return (int)GPV_IsList((TGPValue) args[1]);
	case UI_GPV_GET_TOP_VALUE:
		{
			const char * topValue = GPV_GetTopValue((TGPValue) args[1]);
			if (topValue)
			{
				strcpy((char *) VMA(2), topValue);
			}
			return 0;
		}
	case UI_GPV_GET_LIST:
		return (int)GPV_GetList((TGPValue) args[1]);

	case UI_PB_ISENABLED:
	case UI_PB_ENABLE:
	case UI_PB_DISABLE:
		return qfalse;

	default:
		Com_Printf("Bad UI system trap: %i", args[0] );
		//Com_Error( ERR_DROP, "Bad UI system trap: %i", args[0] );

	}

	return 0;
}

/*
====================
CL_ShutdownUI
====================
*/
void CL_ShutdownUI( void ) {
	cls.keyCatchers &= ~KEYCATCH_UI;
	cls.uiStarted = qfalse;
	if ( !uivm ) {
		return;
	}
	VM_Call( uivm, UI_SHUTDOWN );
	//VM_Call( uivm, UI_MENU_RESET );
	VM_Free( uivm );
	uivm = NULL;
}

/*
====================
CL_InitUI
====================
*/

void CL_InitUI( void ) {
	int		v;
	vmInterpret_t		interpret;

	// load the dll or bytecode
	if ( cl_connectedToPureServer != 0 ) {
#if 0
		// if sv_pure is set we only allow qvms to be loaded
		interpret = VMI_COMPILED;
#else //load the module type based on what the server is doing -rww
		interpret = (vmInterpret_t)cl_connectedUI;
#endif
	}
	else {
		interpret = (vmInterpret_t)(int)Cvar_VariableValue( "vm_ui" );
	}
	uivm = VM_Create( "sof2mp_ui", CL_UISystemCalls, interpret );
	if ( !uivm ) {
		Com_Error( ERR_FATAL, "VM_Create on UI failed" );
	}

	// sanity check
	v = VM_Call( uivm, UI_GETAPIVERSION );
	if (v != UI_API_VERSION) {
		Com_Error( ERR_DROP, "User Interface is version %d, expected %d", v, UI_API_VERSION );
		cls.uiStarted = qfalse;
	}
	else {
		// init for this gamestate
		//rww - changed to <= CA_ACTIVE, because that is the state when we did a vid_restart
		//ingame (was just < CA_ACTIVE before, resulting in ingame menus getting wiped and
		//not reloaded on vid restart from ingame menu)
		VM_Call( uivm, UI_INIT, (cls.state >= CA_AUTHORIZING && cls.state <= CA_ACTIVE) );
	}
}

/*
====================
UI_GameCommand

See if the current console command is claimed by the ui
====================
*/
qboolean UI_GameCommand( void ) {
	if ( !uivm ) {
		return qfalse;
	}

	return (qboolean)VM_Call( uivm, UI_CONSOLE_COMMAND, cls.realtime );
}
