//Anything above this #include will be ignored by the compiler
#include "qcommon/exe_headers.h"

#include "client.h"
/*

key up events are sent even if in console mode

*/

field_t		chatField;
qboolean	chat_team;

int			chat_playerNum;

keyGlobals_t	kg;										

// names not in this list can either be lowercase ascii, or '0xnn' hex sequences
keyname_t keynames[K_LAST_KEY] =							
{			
	{"TAB", K_TAB},
	{"ENTER", K_ENTER},
	{"ESCAPE", K_ESCAPE},
	{"SPACE", K_SPACE},

	{"BACKSPACE", K_BACKSPACE},

	{"COMMAND", K_COMMAND},
	{"CAPSLOCK", K_CAPSLOCK},
	{"POWER", K_POWER},
	{"PAUSE", K_PAUSE},

	{"UPARROW", K_UPARROW},
	{"DOWNARROW", K_DOWNARROW},
	{"LEFTARROW", K_LEFTARROW},
	{"RIGHTARROW", K_RIGHTARROW},

	{"ALT", K_ALT},
	{"CTRL", K_CTRL},
	{"SHIFT", K_SHIFT},
	{"INS", K_INS},
	{"DEL", K_DEL},
	{"PGDN", K_PGDN},
	{"PGUP", K_PGUP},
	{"HOME", K_HOME},
	{"END", K_END},
	
	{"F1", K_F1},
	{"F2", K_F2},
	{"F3", K_F3},
	{"F4", K_F4},
	{"F5", K_F5},
	{"F6", K_F6},
	{"F7", K_F7},
	{"F8", K_F8},
	{"F9", K_F9},
	{"F10", K_F10},
	{"F11", K_F11},
	{"F12", K_F12},
	{"F13", K_F13},
	{"F14", K_F14},
	{"F15", K_F15},

	{"SCROLL", K_SCROLL},

	{"KP_HOME", K_KP_HOME },
	{"KP_UPARROW", K_KP_UPARROW },
	{"KP_PGUP", K_KP_PGUP },
	{"KP_LEFTARROW", K_KP_LEFTARROW },
	{"KP_5", K_KP_5 },
	{"KP_RIGHTARROW", K_KP_RIGHTARROW },
	{"KP_END", K_KP_END },
	{"KP_DOWNARROW", K_KP_DOWNARROW },
	{"KP_PGDN", K_KP_PGDN },
	{"KP_ENTER",  K_KP_ENTER },
	{"KP_INS", K_KP_INS },
	{"KP_DEL", K_KP_DEL },
	{"KP_SLASH", K_KP_SLASH },
	{"KP_MINUS", K_KP_MINUS },
	{"KP_PLUS", K_KP_PLUS },
	{"KP_NUMLOCK", K_KP_NUMLOCK },
	{"KP_STAR", K_KP_STAR },
	{"KP_EQUALS", K_KP_EQUALS },
	
	{"MOUSE1", K_MOUSE1},
	{"MOUSE2", K_MOUSE2},
	{"MOUSE3", K_MOUSE3},
	{"MOUSE4", K_MOUSE4},
	{"MOUSE5", K_MOUSE5},

	{"MWHEELUP",    K_MWHEELUP },
	{"MWHEELDOWN",  K_MWHEELDOWN },

	{"JOY1", K_JOY1},
	{"JOY2", K_JOY2},
	{"JOY3", K_JOY3},
	{"JOY4", K_JOY4},
	{"JOY5", K_JOY5},
	{"JOY6", K_JOY6},
	{"JOY7", K_JOY7},
	{"JOY8", K_JOY8},
	{"JOY9", K_JOY9},
	{"JOY10", K_JOY10},
	{"JOY11", K_JOY11},
	{"JOY12", K_JOY12},
	{"JOY13", K_JOY13},
	{"JOY14", K_JOY14},
	{"JOY15", K_JOY15},
	{"JOY16", K_JOY16},
	{"JOY17", K_JOY17},
	{"JOY18", K_JOY18},
	{"JOY19", K_JOY19},
	{"JOY20", K_JOY20},
	{"JOY21", K_JOY21},
	{"JOY22", K_JOY22},
	{"JOY23", K_JOY23},
	{"JOY24", K_JOY24},
	{"JOY25", K_JOY25},
	{"JOY26", K_JOY26},
	{"JOY27", K_JOY27},
	{"JOY28", K_JOY28},
	{"JOY29", K_JOY29},
	{"JOY30", K_JOY30},
	{"JOY31", K_JOY31},
	{"JOY32", K_JOY32},

	{"AUX1", K_AUX1},
	{"AUX2", K_AUX2},
	{"AUX3", K_AUX3},
	{"AUX4", K_AUX4},
	{"AUX5", K_AUX5},
	{"AUX6", K_AUX6},
	{"AUX7", K_AUX7},
	{"AUX8", K_AUX8},
	{"AUX9", K_AUX9},
	{"AUX10", K_AUX10},
	{"AUX11", K_AUX11},
	{"AUX12", K_AUX12},
	{"AUX13", K_AUX13},
	{"AUX14", K_AUX14},
	{"AUX15", K_AUX15},
	{"AUX16", K_AUX16},

	{"SEMICOLON", ';'},     // because a raw semicolon seperates commands

	{NULL, 0}
};



/*
=============================================================================

EDIT FIELDS

=============================================================================
*/


/*
===================
Field_Draw

Handles horizontal scrolling and cursor blinking
x, y, amd width are in pixels
===================
*/
void Field_VariableSizeDraw( field_t *edit, int x, int y, int width, int size, qboolean showCursor ) {
	int		len;
	int		drawLen;
	int		prestep;
	int		cursorChar;
	char	str[MAX_STRING_CHARS];
	int		i;

	drawLen = edit->widthInChars;
	len = strlen( edit->buffer ) + 1;

	// guarantee that cursor will be visible
	if ( len <= drawLen ) {
		prestep = 0;
	} else {
		if ( edit->scroll + drawLen > len ) {
			edit->scroll = len - drawLen;
			if ( edit->scroll < 0 ) {
				edit->scroll = 0;
			}
		}
		prestep = edit->scroll;

/*
		if ( edit->cursor < len - drawLen ) {
			prestep = edit->cursor;	// cursor at start
		} else {
			prestep = len - drawLen;
		}
*/
	}

	if ( prestep + drawLen > len ) {
		drawLen = len - prestep;
	}

	// extract <drawLen> characters from the field at <prestep>
	if ( drawLen >= MAX_STRING_CHARS ) {
		Com_Error( ERR_DROP, "drawLen >= MAX_STRING_CHARS" );
	}

	Com_Memcpy( str, edit->buffer + prestep, drawLen );
	str[ drawLen ] = 0;

	// draw it
	if ( size == SMALLCHAR_WIDTH ) {
		float	color[4];

		color[0] = color[1] = color[2] = color[3] = 1.0;
		SCR_DrawSmallStringExt( x, y, str, color, qfalse );
	} else {
		// draw big string with drop shadow
		SCR_DrawBigString( x, y, str, 1.0 );
	}

	// draw the cursor
	if ( !showCursor ) {
		return;
	}

	if ( (int)( cls.realtime >> 8 ) & 1 ) {
		return;		// off blink
	}

	if ( kg.key_overstrikeMode ) {
		cursorChar = 11;
	} else {
		cursorChar = 10;
	}

	i = drawLen - ( Q_PrintStrlen( str ) + 1 );

	if ( size == SMALLCHAR_WIDTH ) {
		SCR_DrawSmallChar( x + ( edit->cursor - prestep - i ) * size, y, cursorChar );
	} else {
		str[0] = cursorChar;
		str[1] = 0;
		SCR_DrawBigString( x + ( edit->cursor - prestep - i ) * size, y, str, 1.0 );

	}
}

void Field_Draw( field_t *edit, int x, int y, int width, qboolean showCursor ) 
{
	Field_VariableSizeDraw( edit, x, y, width, SMALLCHAR_WIDTH, showCursor );
}

void Field_BigDraw( field_t *edit, int x, int y, int width, qboolean showCursor ) 
{
	Field_VariableSizeDraw( edit, x, y, width, GIANTCHAR_HEIGHT/2, showCursor );
}

/*
================
Field_Paste
================
*/
void Field_Paste( field_t *edit ) {
	char	*cbd;
	int		pasteLen, i;

	cbd = Sys_GetClipboardData();

	if ( !cbd ) {
		return;
	}

	// send as if typed, so insert / overstrike works properly
	pasteLen = strlen( cbd );
	for ( i = 0 ; i < pasteLen ; i++ ) {
		Field_CharEvent( edit, cbd[i] );
	}

	Z_Free( cbd );
}

/*
=================
Field_KeyDownEvent

Performs the basic line editing functions for the console,
in-game talk, and menu fields

Key events are used for non-printable characters, others are gotten from char events.
=================
*/
void Field_KeyDownEvent( field_t *edit, int key ) {
	int		len;

	// shift-insert is paste
	if ( ( ( key == K_INS ) || ( key == K_KP_INS ) ) && kg.keys[K_SHIFT].down ) {
		Field_Paste( edit );
		return;
	}

	len = strlen( edit->buffer );

	if ( key == K_DEL ) {
		if ( edit->cursor < len ) {
			memmove( edit->buffer + edit->cursor, 
				edit->buffer + edit->cursor + 1, len - edit->cursor );
		}
		return;
	}

	if ( key == K_RIGHTARROW ) 
	{
		if ( edit->cursor < len ) {
			edit->cursor++;
		}

		if ( edit->cursor >= edit->scroll + edit->widthInChars && edit->cursor <= len )
		{
			edit->scroll++;
		}
		return;
	}

	if ( key == K_LEFTARROW ) 
	{
		if ( edit->cursor > 0 ) {
			edit->cursor--;
		}
		if ( edit->cursor < edit->scroll )
		{
			edit->scroll--;
		}
		return;
	}

	if ( key == K_HOME || ( tolower(key) == 'a' && kg.keys[K_CTRL].down ) ) 
	{
		edit->cursor = 0;
		return;
	}

	if ( key == K_END || ( tolower(key) == 'e' && kg.keys[K_CTRL].down ) ) 
	{
		edit->cursor = len;
		return;
	}

	if ( key == K_INS ) {
		kg.key_overstrikeMode = (qboolean)!kg.key_overstrikeMode;
		return;
	}
}

/*
==================
Field_CharEvent
==================
*/
void Field_CharEvent( field_t *edit, int ch ) {
	int		len;

	if ( ch == 'v' - 'a' + 1 ) {	// ctrl-v is paste
		Field_Paste( edit );
		return;
	}

	if ( ch == 'c' - 'a' + 1 ) {	// ctrl-c clears the field
		Field_Clear( edit );
		return;
	}

	len = strlen( edit->buffer );

	if ( ch == 'h' - 'a' + 1 )	{	// ctrl-h is backspace
		if ( edit->cursor > 0 ) {
			memmove( edit->buffer + edit->cursor - 1, 
				edit->buffer + edit->cursor, len + 1 - edit->cursor );
			edit->cursor--;
			if ( edit->cursor < edit->scroll )
			{
				edit->scroll--;
			}
		}
		return;
	}

	if ( ch == 'a' - 'a' + 1 ) {	// ctrl-a is home
		edit->cursor = 0;
		edit->scroll = 0;
		return;
	}

	if ( ch == 'e' - 'a' + 1 ) {	// ctrl-e is end
		edit->cursor = len;
		edit->scroll = edit->cursor - edit->widthInChars;
		return;
	}

	//
	// ignore any other non printable chars
	//
	if ( ch < 32 ) {
		return;
	}

	if ( kg.key_overstrikeMode ) {	
		if ( edit->cursor == MAX_EDIT_LINE - 1 )
			return;
		edit->buffer[edit->cursor] = ch;
		edit->cursor++;
	} else {	// insert mode
		if ( len == MAX_EDIT_LINE - 1 ) {
			return; // all full
		}
		memmove( edit->buffer + edit->cursor + 1, 
			edit->buffer + edit->cursor, len + 1 - edit->cursor );
		edit->buffer[edit->cursor] = ch;
		edit->cursor++;
	}


	if ( edit->cursor >= edit->widthInChars ) {
		edit->scroll++;
	}

	if ( edit->cursor == len + 1) {
		edit->buffer[edit->cursor] = 0;
	}
}

/*
==================
Field_Clear
==================
*/
void Field_Clear( field_t *edit ) {
	edit->buffer[0] = 0;
	edit->cursor = 0;
	edit->scroll = 0;
}

/*
=============================================================================

CONSOLE LINE EDITING

==============================================================================
*/

static const char *completionString;
static char shortestMatch[MAX_TOKEN_CHARS];
static int	matchCount;

/*
===============
FindMatches

===============
*/
static void FindMatches( const char *s ) {
	int		i;

	if ( Q_stricmpn( s, completionString, strlen( completionString ) ) ) {
		return;
	}
	matchCount++;
	if ( matchCount == 1 ) {
		Q_strncpyz( shortestMatch, s, sizeof( shortestMatch ) );
		return;
	}

	// cut shortestMatch to the amount common with s
	for ( i = 0 ; s[i] ; i++ ) {
		if ( tolower(shortestMatch[i]) != tolower(s[i]) ) {
			shortestMatch[i] = 0;
			break;
		}
	}
	if (!s[i])
	{
		shortestMatch[i] = 0;
	}
}

/*
===============
PrintMatches

===============
*/
static void PrintMatches( const char *s ) {
	if ( !Q_stricmpn( s, shortestMatch, strlen( shortestMatch ) ) ) {
		Com_Printf( "    %s\n", s );
	}
}

static void keyConcatArgs( void ) {
	int		i;
	char	*arg;

	for ( i = 1 ; i < Cmd_Argc() ; i++ ) {
		Q_strcat( kg.g_consoleField.buffer, sizeof( kg.g_consoleField.buffer ), " " );
		arg = Cmd_Argv( i );
		while (*arg) {
			if (*arg == ' ') {
				Q_strcat( kg.g_consoleField.buffer, sizeof( kg.g_consoleField.buffer ),  "\"");
				break;
			}
			arg++;
		}
		Q_strcat( kg.g_consoleField.buffer, sizeof( kg.g_consoleField.buffer ),  Cmd_Argv( i ) );
		if (*arg == ' ') {
			Q_strcat( kg.g_consoleField.buffer, sizeof( kg.g_consoleField.buffer ),  "\"");
		}
	}
}

static void ConcatRemaining( const char *src, const char *start ) {
	const char *str;

	str = strstr(src, start);
	if (!str) {
		keyConcatArgs();
		return;
	}

	str += strlen(start);
	Q_strcat( kg.g_consoleField.buffer, sizeof( kg.g_consoleField.buffer ), str);
}


/*
===============
CompleteCommand

Tab expansion
===============
*/
void CompleteCommand( void ) 
{
	field_t		*edit;
	field_t		temp;

	edit = &kg.g_consoleField;

	// only look at the first token for completion purposes
	Cmd_TokenizeString( edit->buffer );

	completionString = Cmd_Argv(0);
	if ( completionString[0] == '\\' || completionString[0] == '/' ) {
		completionString++;
	}
	matchCount = 0;
	shortestMatch[0] = 0;

	if ( strlen( completionString ) == 0 ) {
		return;
	}

	Cmd_CommandCompletion( FindMatches );
	Cvar_CommandCompletion( FindMatches );

	if ( matchCount == 0 ) {
		return;	// no matches
	}

	Com_Memcpy(&temp, edit, sizeof(field_t));

	if ( matchCount == 1 ) {
		Com_sprintf( edit->buffer, sizeof( edit->buffer ), "\\%s", shortestMatch );
		if ( Cmd_Argc() == 1 ) {
			Q_strcat( kg.g_consoleField.buffer, sizeof( kg.g_consoleField.buffer ), " " );
		} else {
			ConcatRemaining( temp.buffer, completionString );
		}
		edit->cursor = strlen( edit->buffer );
		return;
	}

	// multiple matches, complete to shortest
	Com_sprintf( edit->buffer, sizeof( edit->buffer ), "\\%s", shortestMatch );
	edit->cursor = strlen( edit->buffer );
	ConcatRemaining( temp.buffer, completionString );

	Com_Printf( "]%s\n", edit->buffer );

	// run through again, printing matches
	Cmd_CommandCompletion( PrintMatches );
	Cvar_CommandCompletion( PrintMatches );
}


/*
====================
Console_Key

Handles history and console scrollback
====================
*/
void Console_Key (int key) {
	// ctrl-L clears screen
	if ( tolower(key) == 'l' && kg.keys[K_CTRL].down ) {
		Cbuf_AddText ("clear\n");
		return;
	}

	// enter finishes the line
	if ( key == K_ENTER || key == K_KP_ENTER ) {
		// if not in the game explicitly prepent a slash if needed
		if ( cls.state != CA_ACTIVE && kg.g_consoleField.buffer[0] != '\\' 
			&& kg.g_consoleField.buffer[0] != '/' ) {
			char	temp[MAX_STRING_CHARS];

			Q_strncpyz( temp, kg.g_consoleField.buffer, sizeof( temp ) );
			Com_sprintf( kg.g_consoleField.buffer, sizeof( kg.g_consoleField.buffer ), "\\%s", temp );
			kg.g_consoleField.cursor++;
		}
		else
		{	// Added this to automatically make explicit commands not need slashes.
			CompleteCommand();
		}

		Com_Printf ( "]%s\n", kg.g_consoleField.buffer );

		// leading slash is an explicit command
		if ( kg.g_consoleField.buffer[0] == '\\' || kg.g_consoleField.buffer[0] == '/' ) {
			Cbuf_AddText( kg.g_consoleField.buffer+1 );	// valid command
			Cbuf_AddText ("\n");
		} else {
			// other text will be chat messages
			if ( !kg.g_consoleField.buffer[0] ) {
				return;	// empty lines just scroll the console without adding to history
			} else {
				Cbuf_AddText ("cmd say ");
				Cbuf_AddText( kg.g_consoleField.buffer );
				Cbuf_AddText ("\n");
			}
		}

		// copy line to history buffer
		kg.historyEditLines[kg.nextHistoryLine % COMMAND_HISTORY] = kg.g_consoleField;
		kg.nextHistoryLine++;
		kg.historyLine = kg.nextHistoryLine;

		Field_Clear( &kg.g_consoleField );

		kg.g_consoleField.widthInChars = g_console_field_width;

		if ( cls.state == CA_DISCONNECTED ) {
			SCR_UpdateScreen ();	// force an update, because the command
		}							// may take some time
		return;
	}

	// command completion

	if (key == K_TAB) {
		CompleteCommand();
		return;
	}

	// command history (ctrl-p ctrl-n for unix style)

	if ( ( key == K_MWHEELUP && kg.keys[K_SHIFT].down ) || ( key == K_UPARROW ) || ( key == K_KP_UPARROW ) || ( ( tolower(key) == 'p' ) && kg.keys[K_CTRL].down ) ) 
	{
		if ( kg.nextHistoryLine - kg.historyLine < COMMAND_HISTORY && kg.historyLine > 0 ) 
		{
			kg.historyLine--;
		}
		kg.g_consoleField = kg.historyEditLines[ kg.historyLine % COMMAND_HISTORY ];
		return;
	}

	if ( ( key == K_MWHEELDOWN && kg.keys[K_SHIFT].down ) || ( key == K_DOWNARROW ) || ( key == K_KP_DOWNARROW ) || ( ( tolower(key) == 'n' ) && kg.keys[K_CTRL].down ) ) 
	{
		if (kg.historyLine == kg.nextHistoryLine)
			return;
		kg.historyLine++;
		kg.g_consoleField = kg.historyEditLines[ kg.historyLine % COMMAND_HISTORY ];
		return;
	}

	// console scrolling
	if ( key == K_PGUP ) {
		Con_PageUp();
		return;
	}

	if ( key == K_PGDN ) {
		Con_PageDown();
		return;
	}

	if ( key == K_MWHEELUP ) {	//----(SA)	added some mousewheel functionality to the console
		Con_PageUp();
		if(kg.keys[K_CTRL].down) {	// hold <ctrl> to accelerate scrolling
			Con_PageUp();
			Con_PageUp();
		}
		return;
	}

	if ( key == K_MWHEELDOWN ) {	//----(SA)	added some mousewheel functionality to the console
		Con_PageDown();
		if(kg.keys[K_CTRL].down) {	// hold <ctrl> to accelerate scrolling
			Con_PageDown();
			Con_PageDown();
		}
		return;
	}

	// ctrl-home = top of console
	if ( key == K_HOME && kg.keys[K_CTRL].down ) {
		Con_Top();
		return;
	}

	// ctrl-end = bottom of console
	if ( key == K_END && kg.keys[K_CTRL].down ) {
		Con_Bottom();
		return;
	}

	// pass to the normal editline routine
	Field_KeyDownEvent( &kg.g_consoleField, key );
}

//============================================================================


/*
================
Message_Key

In game talk message
================
*/
void Message_Key( int key ) {
	char	buffer[MAX_STRING_CHARS];

	if (key == K_ESCAPE) {
		cls.keyCatchers &= ~KEYCATCH_MESSAGE;
		Field_Clear( &chatField );
		return;
	}

	if ( key == K_ENTER || key == K_KP_ENTER )
	{
		if ( chatField.buffer[0] && cls.state == CA_ACTIVE ) {
			if (chat_playerNum != -1 )
				Com_sprintf( buffer, sizeof( buffer ), "tell %i \"%s\"\n", chat_playerNum, chatField.buffer );
			else if (chat_team)
				Com_sprintf( buffer, sizeof( buffer ), "say_team \"%s\"\n", chatField.buffer );
			else
				Com_sprintf( buffer, sizeof( buffer ), "say \"%s\"\n", chatField.buffer );

			CL_AddReliableCommand( buffer );
		}
		cls.keyCatchers &= ~KEYCATCH_MESSAGE;
		Field_Clear( &chatField );
		return;
	}

	Field_KeyDownEvent( &chatField, key );
}

//============================================================================


qboolean Key_GetOverstrikeMode( void ) {
	return kg.key_overstrikeMode;
}


void Key_SetOverstrikeMode( qboolean state ) {
	kg.key_overstrikeMode = state;
}


/*
===================
Key_IsDown
===================
*/
qboolean Key_IsDown( int keynum ) {
	if ( keynum == -1 ) {
		return qfalse;
	}

	return kg.keys[keynum].down;
}


/*
===================
Key_StringToKeynum

Returns a key number to be used to index keys[] by looking at
the given string.  Single ascii characters return themselves, while
the K_* names are matched up.

0x11 will be interpreted as raw hex, which will allow new controlers
to be configured even if they don't have defined names.
===================
*/
int Key_StringToKeynum( char *str ) {
	keyname_t	*kn;
	
	if ( !str || !str[0] ) 
	{
		return -1;
	}
	// If single char bind, presume ascii char bind
	if ( !str[1] ) 
	{
		return str[0];
	}

	// scan for a text match
	for ( kn=keynames ; kn->name ; kn++ ) 
	{
		if ( !stricmp( str, kn->name ) )
		{
			return kn->keynum;
		}
	}

	// check for hex code
	if ( str[0] == '0' && str[1] == 'x' && strlen( str ) == 4) 
	{
		int		n1, n2;
		
		n1 = str[2];
		if ( n1 >= '0' && n1 <= '9' ) 
		{
			n1 -= '0';
		}
		else if ( n1 >= 'A' && n1 <= 'F' ) 
		{
			n1 = n1 - 'A' + 10;
		}
		else 
		{
			n1 = 0;
		}

		n2 = str[3];
		if ( n2 >= '0' && n2 <= '9' ) 
		{
			n2 -= '0';
		}
		else if ( n2 >= 'A' && n2 <= 'F' ) 
		{
			n2 = n2 - 'A' + 10;
		}
		else 
		{
			n2 = 0;
		}
		return n1 * 16 + n2;
	}

	return -1;
}


static char tinyString[16];
static const char *Key_KeynumValid( int keynum )
{
	if ( keynum == -1 ) 
	{
		return "<KEY NOT FOUND>";
	}
	if ( keynum < 0 || keynum >= K_LAST_KEY ) 
	{
		return "<OUT OF RANGE>";
	}
	return NULL;
}

static const char *Key_KeyToHex( int keynum )
{
	int		i, j;

	i = keynum >> 4;
	j = keynum & 15;

	tinyString[0] = '0';
	tinyString[1] = 'x';
	tinyString[2] = i > 9 ? i - 10 + 'A' : i + '0';
	tinyString[3] = j > 9 ? j - 10 + 'A' : j + '0';
	tinyString[4] = 0;

	return tinyString;
}

/*
===================
Key_KeynumToString

Returns a string (either a single ascii char, a K_* name, or a 0x11 hex string) for the
given keynum.
===================
*/
// Returns a console/config file friendly name for the key
const char *Key_KeynumToString( int keynum ) 
{
	keyname_t   *kn;
	const char	*name;

	name = Key_KeynumValid(keynum);

	// check for printable ascii
	if ( !name && keynum > 32 && keynum < 127) 
	{
		tinyString[0] = keynum;
		tinyString[1] = 0;
		return tinyString;
	}
	// Check for friendly name
	if ( !name )
	{
		for ( kn=keynames ; kn->name ; kn++ ) {
			if (keynum == kn->keynum) {
				name = kn->name;
				break;
			}
		}
	}
	// Fallback to hex number
	if ( !name )
	{
		name = Key_KeyToHex(keynum);
	}
	return name;
}



/*
===================
Key_SetBinding
===================
*/
void Key_SetBinding( int keynum, const char *binding ) {
	if ( keynum == -1 ) {
		return;
	}

	// free old bindings
	if ( kg.keys[keynum].binding ) {
		Z_Free( kg.keys[keynum].binding );
		kg.keys[keynum].binding = NULL;
	}
			
	// allocate memory for new binding
	if (binding)
	{
		kg.keys[keynum].binding = CopyString( binding );
	}

	// consider this like modifying an archived cvar, so the
	// file write will be triggered at the next oportunity
	cvar_modifiedFlags |= CVAR_ARCHIVE;
}


/*
===================
Key_GetBinding
===================
*/
char *Key_GetBinding( int keynum ) {
	if ( keynum == -1 ) {
		return "";
	}

	return kg.keys[ keynum ].binding;
}

/* 
===================
Key_GetKey
===================
*/

int Key_GetKey(const char *binding) {
  int i;

  if (binding) {
  	for (i=0 ; i<256 ; i++) {
      if (kg.keys[i].binding && Q_stricmp(binding, kg.keys[i].binding) == 0) {
        return i;
      }
    }
  }
  return -1;
}

/*
===================
Key_Unbind_f
===================
*/
void Key_Unbind_f (void)
{
	int		b;

	if (Cmd_Argc() != 2)
	{
		Com_Printf ("unbind <key> : remove commands from a key\n");
		return;
	}
	
	b = Key_StringToKeynum (Cmd_Argv(1));
	if (b==-1)
	{
		Com_Printf ("\"%s\" isn't a valid key\n", Cmd_Argv(1));
		return;
	}

	Key_SetBinding (b, "");
}

/*
===================
Key_Unbindall_f
===================
*/
void Key_Unbindall_f (void)
{
	int		i;
	
	for (i = 0; i < K_LAST_KEY ; i++)
	{
		if (kg.keys[i].binding)
		{
			Key_SetBinding (i, "");
		}
	}
}



/*
===================
Key_Bind_f
===================
*/
void Key_Bind_f (void)
{
	int			i, c, b;
	char		cmd[1024];
	
	c = Cmd_Argc();

	if (c < 2)
	{
		Com_Printf ("bind <key> [command] : attach a command to a key\n");
		return;
	}
	b = Key_StringToKeynum (Cmd_Argv(1));
	if (b==-1)
	{
		Com_Printf ("\"%s\" isn't a valid key\n", Cmd_Argv(1));
		return;
	}

	if (c == 2)
	{
		if (kg.keys[b].binding)
			Com_Printf ("\"%s\" = \"%s\"\n", Cmd_Argv(1), kg.keys[b].binding );
		else
			Com_Printf ("\"%s\" is not bound\n", Cmd_Argv(1) );
		return;
	}
	
// copy the rest of the command line
	cmd[0] = 0;		// start out with a null string
	for (i=2 ; i< c ; i++)
	{
		strcat (cmd, Cmd_Argv(i));
		if (i != (c-1))
			strcat (cmd, " ");
	}

	Key_SetBinding (b, cmd);
}

/*
============
Key_WriteBindings

Writes lines containing "bind key value"
============
*/
void Key_WriteBindings( fileHandle_t f ) {
	int		i;

	FS_Printf (f, "unbindall\n" );
	for (i=0 ; i<K_LAST_KEY ; i++) {
		if (kg.keys[i].binding && kg.keys[i].binding[0] ) {
			const char *name = Key_KeynumToString(i);

			// handle the escape character nicely
			if (!strcmp(name, "\\")) {
				FS_Printf (f, "bind \"\\\" \"%s\"\n", kg.keys[i].binding);
			}
			else {
				FS_Printf (f, "bind \"%s\" \"%s\"\n", name, kg.keys[i].binding);
			}
		}
	}
}

/*
============
Key_Bindlist_f

============
*/
void Key_Bindlist_f( void ) {
	int		i;

	for ( i = 0 ; i < K_LAST_KEY ; i++ ) {
		if ( kg.keys[i].binding && kg.keys[i].binding[0] ) {
			Com_Printf( "Key : %s \"%s\"\n", Key_KeynumToString(i), kg.keys[i].binding );
		}
	}
}

/*
===================
CL_InitKeyCommands
===================
*/
void CL_InitKeyCommands( void ) {
	// register our functions
	Cmd_AddCommand ("bind",Key_Bind_f);
	Cmd_AddCommand ("unbind",Key_Unbind_f);
	Cmd_AddCommand ("unbindall",Key_Unbindall_f);
	Cmd_AddCommand ("bindlist",Key_Bindlist_f);
}

/*
===================
CL_AddKeyUpCommands
===================
*/
void CL_AddKeyUpCommands( int key, char *kb ) {
	int i;
	char button[1024], *buttonPtr;
	char	cmd[1024];
	qboolean keyevent;

	if ( !kb ) {
		return;
	}
	keyevent = qfalse;
	buttonPtr = button;
	for ( i = 0; ; i++ ) {
		if ( kb[i] == ';' || !kb[i] ) {
			*buttonPtr = '\0';
			if ( button[0] == '+') {
				// button commands add keynum and time as parms so that multiple
				// sources can be discriminated and subframe corrected
				Com_sprintf (cmd, sizeof(cmd), "-%s %i %i\n", button+1, key, time);
				Cbuf_AddText (cmd);
				keyevent = qtrue;
			} else {
				if (keyevent) {
					// down-only command
					Cbuf_AddText (button);
					Cbuf_AddText ("\n");
				}
			}
			buttonPtr = button;
			while ( (kb[i] <= ' ' || kb[i] == ';') && kb[i] != 0 ) {
				i++;
			}
		}
		*buttonPtr++ = kb[i];
		if ( !kb[i] ) {
			break;
		}
	}
}

/*
===================
CL_KeyEvent

Called by the system for both key up and key down events
===================
*/
void CL_KeyEvent (int key, qboolean down, unsigned time) {
	char	*kb;
	char	cmd[1024];

	// update auto-repeat status and BUTTON_ANY status
	kg.keys[key].down = down;
	if (down)
	{
		kg.keys[key].repeats++;
		if ( kg.keys[key].repeats == 1 && key != K_KP_NUMLOCK && key != K_CAPSLOCK )
		{
			kg.anykeydown = qtrue;
			kg.keyDownCount++;
		}
	}
	else
	{
		kg.keys[key].repeats = 0;
		if( key != K_KP_NUMLOCK && key != K_CAPSLOCK )
			kg.keyDownCount--;
		if(kg.keyDownCount <= 0)
		{
			kg.anykeydown = qfalse;
			kg.keyDownCount = 0;
		}
	}

	// console key is hardcoded, so the user can never unbind it
	if (key == K_CONSOLE || ( kg.keys[K_SHIFT].down && key == K_ESCAPE ) ) {
		if (!down) {
			return;
		}
	    Con_ToggleConsole_f ();
		return;
	}

	// kg.keys can still be used for bound actions
	if ( down && ( key < 128 || key == K_MOUSE1 ) && 
		( cls.state == CA_CINEMATIC ) &&
		!cls.keyCatchers) {

		if (Cvar_VariableValue ("com_cameraMode") == 0) {
			Cvar_Set ("nextdemo","");
			key = K_ESCAPE;
		}
	}

	// escape is always handled special
	if ( key == K_ESCAPE && down ) {
		if ( cls.keyCatchers & KEYCATCH_MESSAGE ) {
			// clear message mode
			Message_Key( key );
			return;
		}

		// escape always gets out of CGAME stuff
		if (cls.keyCatchers & KEYCATCH_CGAME) {
			cls.keyCatchers &= ~KEYCATCH_CGAME;
			VM_Call (cgvm, CG_EVENT_HANDLING, CGAME_EVENT_NONE);
			return;
		}

		if ( !( cls.keyCatchers & KEYCATCH_UI ) ) {
			if ( cls.state == CA_ACTIVE && !clc.demoplaying ) {
				VM_Call( uivm, UI_SET_ACTIVE_MENU, UIMENU_INGAME );
			}
			else {
				CL_Disconnect_f();
				S_StopAllSounds();
				VM_Call( uivm, UI_SET_ACTIVE_MENU, UIMENU_MAIN );
			}
			return;
		}

		VM_Call( uivm, UI_KEY_EVENT, key, down );
		return;
	}

	//
	// key up events only perform actions if the game key binding is
	// a button command (leading + sign).  These will be processed even in
	// console mode and menu mode, to keep the character from continuing 
	// an action started before a mode switch.
	//
	if (!down) {
		kb = kg.keys[key].binding;

		CL_AddKeyUpCommands( key, kb );

		if ( cls.keyCatchers & KEYCATCH_UI && uivm ) {
			VM_Call( uivm, UI_KEY_EVENT, key, down );
		} else if ( cls.keyCatchers & KEYCATCH_CGAME && cgvm ) {
			VM_Call( cgvm, CG_KEY_EVENT, key, down );
		} 

		return;
	}


	// distribute the key down event to the apropriate handler
	if ( cls.keyCatchers & KEYCATCH_CONSOLE ) {
		Console_Key( key );
	} else if ( cls.keyCatchers & KEYCATCH_UI ) {
		if ( uivm ) {
			VM_Call( uivm, UI_KEY_EVENT, key, down );
		} 
	} else if ( cls.keyCatchers & KEYCATCH_CGAME ) {
		if ( cgvm ) {
			VM_Call( cgvm, CG_KEY_EVENT, key, down );
		} 
	} else if ( cls.keyCatchers & KEYCATCH_MESSAGE ) {
		Message_Key( key );
	} else if ( cls.state == CA_DISCONNECTED ) {
		Console_Key( key );
	} else {
		// send the bound action
		kb = kg.keys[key].binding;
		if (kb)
		{
			if (kb[0] == '+') {	
				int i;
				char button[1024], *buttonPtr;
				buttonPtr = button;
				for ( i = 0; ; i++ ) {
					if ( kb[i] == ';' || !kb[i] ) {
						*buttonPtr = '\0';
						if ( button[0] == '+') {
							// button commands add keynum and time as parms so that multiple
							// sources can be discriminated and subframe corrected
							Com_sprintf (cmd, sizeof(cmd), "%s %i %i\n", button, key, time);
							Cbuf_AddText (cmd);
						} else {
							// down-only command
							Cbuf_AddText (button);
							Cbuf_AddText ("\n");
						}
						buttonPtr = button;
						while ( (kb[i] <= ' ' || kb[i] == ';') && kb[i] != 0 ) {
							i++;
						}
					}
					*buttonPtr++ = kb[i];
					if ( !kb[i] ) {
						break;
					}
				}
			} else {
				// down-only command
				Cbuf_AddText (kb);
				Cbuf_AddText ("\n");
			}
		}
	}
}


/*
===================
CL_CharEvent

Normal keyboard characters, already shifted / capslocked / etc
===================
*/
void CL_CharEvent( int key ) {
	// the console key should never be used as a char
	if ( key == '`' || key == '~' ) {
		return;
	}

	// delete is not a printable character and is
	// otherwise handled by Field_KeyDownEvent
	if ( key == 127 ) {
		return;
	}

	// distribute the key down event to the apropriate handler
	if ( cls.keyCatchers & KEYCATCH_CONSOLE )
	{
		Field_CharEvent( &kg.g_consoleField, key );
	}
	else if ( cls.keyCatchers & KEYCATCH_UI )
	{
		VM_Call( uivm, UI_KEY_EVENT, key | K_CHAR_FLAG, qtrue );
	}
	else if ( cls.keyCatchers & KEYCATCH_MESSAGE ) 
	{
		Field_CharEvent( &chatField, key );
	}
	else if ( cls.state == CA_DISCONNECTED )
	{
		Field_CharEvent( &kg.g_consoleField, key );
	}
}


/*
===================
Key_ClearStates
===================
*/
void Key_ClearStates (void)
{
	int		i;

	kg.anykeydown = qfalse;

	for ( i=0 ; i < K_LAST_KEY ; i++ ) {
		if (i == K_KP_NUMLOCK || i == K_CAPSLOCK)
			continue;

		if ( kg.keys[i].down ) {
			CL_KeyEvent( i, qfalse, 0 );
		}
		kg.keys[i].down = qfalse;
		kg.keys[i].repeats = 0;
	}
}

