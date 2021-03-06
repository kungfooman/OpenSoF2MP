// Copyright (C) 2001-2002 Raven Software
//
#ifndef __UI_PUBLIC_H__
#define __UI_PUBLIC_H__

#define UI_API_VERSION	6

typedef struct {
	connstate_t		connState;
	int				connectPacketCount;
	int				clientNum;
	char			servername[MAX_STRING_CHARS];
	char			updateInfoString[MAX_STRING_CHARS];
	char			messageString[MAX_STRING_CHARS];
} uiClientState_t;

typedef enum 
{
	UI_ERROR,							
	UI_PRINT,
	UI_MILLISECONDS,
	UI_CVAR_SET,
	UI_CVAR_VARIABLEVALUE,			

	UI_CVAR_VARIABLESTRINGBUFFER,	
	UI_CVAR_SETVALUE,
	UI_CVAR_RESET,
	UI_CVAR_CREATE,
	UI_CVAR_INFOSTRINGBUFFER,

	UI_ARGC,							
	UI_ARGV,
	UI_CMD_EXECUTETEXT,
	UI_FS_FOPENFILE,
	UI_FS_READ,

	UI_FS_WRITE,						
	UI_FS_FCLOSEFILE,
	UI_FS_GETFILELIST,
	UI_R_REGISTERMODEL,
	UI_R_REGISTERSKIN,

	UI_R_REGISTERSHADERNOMIP,			
	UI_R_CLEARSCENE,
	UI_R_ADDREFENTITYTOSCENE,
	UI_R_ADDPOLYTOSCENE,
	UI_R_ADDLIGHTTOSCENE,

	UI_R_RENDERSCENE,					
	UI_R_SETCOLOR,
	UI_R_DRAWSTRETCHPIC,
	UI_UPDATESCREEN,
	UI_CM_LERPTAG,

	UI_CM_LOADMODEL,					
	UI_S_REGISTERSOUND,
	UI_S_STARTLOCALSOUND,
	UI_KEY_KEYNUMTOSTRINGBUF,
	UI_KEY_GETBINDINGBUF,

	UI_KEY_SETBINDING,					
	UI_KEY_ISDOWN,
	UI_KEY_GETOVERSTRIKEMODE,
	UI_KEY_SETOVERSTRIKEMODE,
	UI_KEY_CLEARSTATES,

	UI_KEY_GETCATCHER,					
	UI_KEY_SETCATCHER,
	UI_GETCLIPBOARDDATA,
	UI_GETGLCONFIG,
	UI_GETCLIENTSTATE,

	UI_GETCONFIGSTRING,					
	UI_LAN_GETPINGQUEUECOUNT,
	UI_LAN_CLEARPING,
	UI_LAN_GETPING,
	UI_LAN_GETPINGINFO,

	UI_CVAR_REGISTER,					
	UI_CVAR_UPDATE,
	UI_MEMORY_REMAINING,
	UI_GET_CDKEY,
	UI_SET_CDKEY,

	UI_R_REGISTERFONT,					
	UI_R_GETTEXTWIDTH,
	UI_R_GETTEXTHEIGHT,
	UI_R_DRAWTEXT,
	UI_R_DRAWTEXTWITHCURSOR,

	UI_R_MODELBOUNDS,					
	UI_PC_ADD_GLOBAL_DEFINE,
	UI_PC_LOAD_SOURCE,
	UI_PC_FREE_SOURCE,
	UI_PC_READ_TOKEN,

	UI_PC_SOURCE_FILE_AND_LINE,			
	UI_PC_LOAD_GLOBAL_DEFINES,
	UI_PC_REMOVE_ALL_GLOBAL_DEFINES,

	UI_S_STOPBACKGROUNDTRACK,
	UI_S_STARTBACKGROUNDTRACK,
	UI_REAL_TIME,
	UI_LAN_GETSERVERCOUNT,

	UI_LAN_GETSERVERADDRESSSTRING,		
	UI_LAN_GETSERVERINFO,
	UI_LAN_MARKSERVERVISIBLE,
	UI_LAN_UPDATEVISIBLEPINGS,
	UI_LAN_RESETPINGS,

	UI_LAN_LOADCACHEDSERVERS,			
	UI_LAN_SAVECACHEDSERVERS,
	UI_LAN_ADDSERVER,
	UI_LAN_REMOVESERVER,
	UI_CIN_PLAYCINEMATIC,

	UI_CIN_STOPCINEMATIC,				
	UI_CIN_RUNCINEMATIC,
	UI_CIN_DRAWCINEMATIC,
	UI_CIN_SETEXTENTS,
	UI_R_REMAP_SHADER,

	UI_VERIFY_CDKEY,					
	UI_LAN_SERVERSTATUS,
	UI_LAN_GETSERVERPING,
	UI_LAN_SERVERISVISIBLE,
	UI_LAN_COMPARESERVERS,

	UI_MEMSET = 100,					
	UI_MEMCPY,
	UI_STRNCPY,
	UI_SIN,
	UI_COS,

	UI_ATAN2,							
	UI_SQRT,
	UI_FLOOR,
	UI_ANGLEVECTORS,
	UI_PERPENDICULARVECTOR,
	UI_CEIL,

	UI_TESTPRINTINT,
	UI_TESTPRINTFLOAT,

	UI_ACOS,
	UI_ASIN,
	UI_MATRIXMULTIPLY,

	UI_G2_LISTBONES,
	UI_G2_LISTSURFACES,
	UI_G2_ADDBOLT,
	UI_G2_REMOVEBOLT,

	UI_G2_ATTACHG2MODEL,				
	UI_G2_HAVEWEGHOULMODELS,
	UI_G2_SETMODELS,
	UI_G2_GETBOLT,
	UI_G2_INITGHOUL2MODEL,

	UI_G2_CLEANMODELS,					
	UI_G2_ANGLEOVERRIDE,
	UI_G2_PLAYANIM,
	UI_G2_GETGLANAME,
	UI_G2_COPYGHOUL2INSTANCE,

	UI_G2_COPYSPECIFICGHOUL2MODEL,		
	UI_G2_DUPLICATEGHOUL2INSTANCE,
	UI_G2_REMOVEGHOUL2MODEL,

	// CGenericParser2 (void *) routines
	UI_GP_PARSE,						
	UI_GP_PARSE_FILE,
	UI_GP_CLEAN,
	UI_GP_DELETE,
	UI_GP_GET_BASE_PARSE_GROUP,

	UI_G2_SETSURFACEONOFF,
	UI_G2_REGISTERSKIN,
	UI_G2_SETSKIN,
	UI_G2_GETANIMFILENAMEINDEX,

	// CGPGroup (void *) routines
	UI_GPG_GET_NAME,					
	UI_GPG_GET_NEXT,
	UI_GPG_GET_INORDER_NEXT,
	UI_GPG_GET_INORDER_PREVIOUS,
	UI_GPG_GET_PAIRS,

	UI_GPG_GET_INORDER_PAIRS,			
	UI_GPG_GET_SUBGROUPS,
	UI_GPG_GET_INORDER_SUBGROUPS,
	UI_GPG_FIND_SUBGROUP,
	UI_GPG_FIND_PAIR,

	UI_GPG_FIND_PAIRVALUE,				

	// CGPValue (void *) routines
	UI_GPV_GET_NAME,
	UI_GPV_GET_NEXT,
	UI_GPV_GET_INORDER_NEXT,
	UI_GPV_GET_INORDER_PREVIOUS,

	UI_GPV_IS_LIST,						
	UI_GPV_GET_TOP_VALUE,
	UI_GPV_GET_LIST,
	
	UI_CL_READ_G2SKIN,					
	UI_CL_READ_NPCFILES,
	UI_CL_FREE_NPCFILES,
	UI_CL_GET_MODEL_LIST,

	UI_PARENTAL_UPDATE,					
	UI_PARENTAL_SET_PASSWORD,
	UI_PARENTAL_GET_PASSWORD,

	UI_VM_LOCALALLOC,
	UI_VM_LOCALALLOCUNALIGNED,
	UI_VM_LOCALTEMPALLOC,
	UI_VM_LOCALTEMPFREE,
	UI_VM_LOCALSTRINGALLOC,

	UI_NET_AVAILABLE,

	UI_VERSION_GET_DESCRIPTION,
	UI_VERSION_GET_NUM_SITES,
	UI_VERSION_GET_SITE,
	UI_VERSION_DOWNLOAD,

	UI_PB_ENABLE,
	UI_PB_DISABLE,
	UI_PB_ISENABLED,

	UI_GET_TEAM_COUNT,
	UI_GET_TEAM_SCORE,

} uiImport_t;

typedef enum 
{
	UIMENU_NONE,
	UIMENU_MAIN,
	UIMENU_INGAME,
	UIMENU_BAD_CD_KEY,
	UIMENU_TEAM,
	UIMENU_OBJECTIVES,
	UIMENU_OUTFITTING,
	UIMENU_RADIO,
	UIMENU_VERSION,

} uiMenuCommand_t;

#define	SORT_NEEDPASS		0
#define SORT_HOST			1
#define SORT_MAP			2
#define SORT_CLIENTS		3
#define SORT_GAME			4
#define SORT_PING			5
#define SORT_PUNKBUSTER		6

typedef enum 
{
	UI_GETAPIVERSION = 0,	// system reserved

	UI_INIT,
//	void	UI_Init( void );

	UI_SHUTDOWN,
//	void	UI_Shutdown( void );

	UI_KEY_EVENT,
//	void	UI_KeyEvent( int key );

	UI_MOUSE_EVENT,
//	void	UI_MouseEvent( int dx, int dy );

	UI_REFRESH,
//	void	UI_Refresh( int time );

	UI_IS_FULLSCREEN,
//	qboolean UI_IsFullscreen( void );

	UI_SET_ACTIVE_MENU,
//	void	UI_SetActiveMenu( uiMenuCommand_t menu );

	UI_CONSOLE_COMMAND,
//	qboolean UI_ConsoleCommand( int realTime );

	UI_DRAW_CONNECT_SCREEN,
//	void	UI_DrawConnectScreen( qboolean overlay );
	UI_HASUNIQUECDKEY,

	UI_CLOSEALL,
//	void	Menus_CloseAll ( void );

// if !overlay, the background will be drawn, otherwise it will be
// overlayed over whatever the cgame has drawn.
// a GetClientState syscall will be made to get the current strings

	UI_DRAW_LOADING_SCREEN,

} uiExport_t;

#endif
