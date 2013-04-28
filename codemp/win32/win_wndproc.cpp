//Anything above this #include will be ignored by the compiler
#include "qcommon/exe_headers.h"

#include "client/client.h"
#include "win_local.h"

WinVars_t	g_wv;

// The only directly referenced keycode - the console key (which gives different ascii codes depending on locale)
#define CONSOLE_SCAN_CODE	0x29

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL (WM_MOUSELAST+1)  // message that will be supported by the OS 
#endif

static UINT MSH_MOUSEWHEEL;

// Console variables that we need to access from this module
cvar_t			*vid_xpos;			// X coordinate of window position
cvar_t			*vid_ypos;			// Y coordinate of window position
static cvar_t	*r_fullscreen;

#define VID_NUM_MODES ( sizeof( vid_modes ) / sizeof( vid_modes[0] ) )

LONG WINAPI MainWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

static qboolean s_alttab_disabled;

static void WIN_DisableAltTab( void )
{
	if ( s_alttab_disabled )
		return;

	if ( !Q_stricmp( Cvar_VariableString( "arch" ), "winnt" ) )
	{
		RegisterHotKey( 0, 0, MOD_ALT, VK_TAB );
	}
	else
	{
		BOOL old;

		SystemParametersInfo( SPI_SCREENSAVERRUNNING, 1, &old, 0 );
	}
	s_alttab_disabled = qtrue;
}

static void WIN_EnableAltTab( void )
{
	if ( s_alttab_disabled )
	{
		if ( !Q_stricmp( Cvar_VariableString( "arch" ), "winnt" ) )
		{
			UnregisterHotKey( 0, 0 );
		}
		else
		{
			BOOL old;

			SystemParametersInfo( SPI_SCREENSAVERRUNNING, 0, &old, 0 );
		}

		s_alttab_disabled = qfalse;
	}
}

/*
==================
VID_AppActivate
==================
*/
static void VID_AppActivate(BOOL fActive, BOOL minimize)
{
	g_wv.isMinimized = (qboolean)minimize;

	Com_DPrintf("VID_AppActivate: %i\n", fActive );

	Key_ClearStates();	// FIXME!!!

	// we don't want to act like we're active if we're minimized
	if (fActive && !g_wv.isMinimized )
	{
		g_wv.activeApp = qtrue;
	}
	else
	{
		g_wv.activeApp = qfalse;
	}

	// minimize/restore mouse-capture on demand
	if (!g_wv.activeApp )
	{
		IN_Activate (qfalse);
	}
	else
	{
		IN_Activate (qtrue);
	}
}

//==========================================================================


static byte virtualKeyConvert[0x92][2] =
{
	{ 0,				0				},     
	{ K_MOUSE1,			K_MOUSE1		}, // VK_LBUTTON 01 Left mouse button  
	{ K_MOUSE2,			K_MOUSE2		}, // VK_RBUTTON 02 Right mouse button  
	{ 0,				0				}, // VK_CANCEL 03 Control-break processing  
	{ K_MOUSE3,			K_MOUSE3		}, // VK_MBUTTON 04 Middle mouse button (three-button mouse)  
	{ K_MOUSE4,			K_MOUSE4		}, // VK_XBUTTON1 05 Windows 2000/XP: X1 mouse button 
	{ K_MOUSE5,			K_MOUSE5		}, // VK_XBUTTON2 06 Windows 2000/XP: X2 mouse button 
	{ 0,				0				}, // 07 Undefined  
	{ K_BACKSPACE,		K_BACKSPACE		}, // VK_BACK 08 BACKSPACE key  
	{ K_TAB,			K_TAB			}, // VK_TAB 09 TAB key  
	{ 0,				0				}, // 0A Reserved  
	{ 0,				0				}, // 0B Reserved  
	{ K_KP_5,			0				}, // VK_CLEAR 0C CLEAR key  
	{ K_ENTER,			K_KP_ENTER 		}, // VK_RETURN 0D ENTER key  
	{ 0,				0				}, // 0E Undefined  
	{ 0,				0				}, // 0F Undefined  
	{ K_SHIFT,			K_SHIFT			}, // VK_SHIFT 10 SHIFT key  
	{ K_CTRL,			K_CTRL			}, // VK_CONTROL 11 CTRL key  
	{ K_ALT,			K_ALT			}, // VK_MENU 12 ALT key  
	{ K_PAUSE,			K_PAUSE			}, // VK_PAUSE 13 PAUSE key  
	{ K_CAPSLOCK,		K_CAPSLOCK		}, // VK_CAPITAL 14 CAPS LOCK key  
	{ 0,				0				}, // VK_KANA 15 IME Kana mode 
	{ 0,				0				}, // 16 Undefined  
	{ 0,				0				}, // VK_JUNJA 17 IME Junja mode 
	{ 0,				0				}, // VK_FINAL 18 IME final mode 
	{ 0,				0				}, // VK_KANJI 19 IME Kanji mode 
	{ 0,				0				}, // 1A Undefined  
	{ K_ESCAPE,			K_ESCAPE		}, // VK_ESCAPE 1B ESC key  
	{ 0,				0				}, // VK_CONVERT 1C IME convert 
	{ 0,				0				}, // VK_NONCONVERT 1D IME nonconvert 
	{ 0,				0				}, // VK_ACCEPT 1E IME accept 
	{ 0,				0				}, // VK_MODECHANGE 1F IME mode change request 
	{ K_SPACE,			K_SPACE			}, // VK_SPACE 20 SPACEBAR  
	{ K_KP_PGUP,		K_PGUP			}, // VK_PRIOR 21 PAGE UP key  
	{ K_KP_PGDN,		K_PGDN			}, // VK_NEXT 22 PAGE DOWN key  
	{ K_KP_END,			K_END			}, // VK_END 23 END key  
	{ K_KP_HOME,		K_HOME			}, // VK_HOME 24 HOME key  
	{ K_KP_LEFTARROW,	K_LEFTARROW		}, // VK_LEFT 25 LEFT ARROW key  
	{ K_KP_UPARROW,		K_UPARROW   	}, // VK_UP 26 UP ARROW key  
	{ K_KP_RIGHTARROW,	K_RIGHTARROW	}, // VK_RIGHT 27 RIGHT ARROW key  
	{ K_KP_DOWNARROW,	K_DOWNARROW		}, // VK_DOWN 28 DOWN ARROW key  
	{ 0,				0				}, // VK_SELECT 29 SELECT key  
	{ 0,				0				}, // VK_PRINT 2A PRINT key 
	{ 0,				0				}, // VK_EXECUTE 2B EXECUTE key  
	{ 0,				0				}, // VK_SNAPSHOT 2C PRINT SCREEN key  
	{ K_KP_INS,			K_INS			}, // VK_INSERT 2D INS key  
	{ K_KP_DEL,			K_DEL			}, // VK_DELETE 2E DEL key  
	{ 0,				0				}, // VK_HELP 2F HELP key  
	{ K_0,				K_0				}, // 30 0 key  
	{ K_1,				K_1				}, // 31 1 key  
	{ K_2,				K_2				}, // 32 2 key  
	{ K_3,				K_3				}, // 33 3 key  
	{ K_4,				K_4				}, // 34 4 key  
	{ K_5,				K_5				}, // 35 5 key  
	{ K_6,				K_6				}, // 36 6 key  
	{ K_7,				K_7				}, // 37 7 key  
	{ K_8,				K_8				}, // 38 8 key  
	{ K_9,				K_9				}, // 39 9 key  
	{ 0,				0				}, // 3A Undefined  
	{ 0,				0				}, // 3B Undefined  
	{ 0,				0				}, // 3C Undefined  
	{ 0,				0				}, // 3D Undefined  
	{ 0,				0				}, // 3E Undefined  
	{ 0,				0				}, // 3F Undefined  
	{ 0,				0				}, // 40 Undefined  
	{ K_CAP_A,			K_CAP_A			}, // 41 A key  
	{ K_CAP_B,			K_CAP_B			}, // 42 B key  
	{ K_CAP_C,			K_CAP_C			}, // 43 C key  
	{ K_CAP_D,			K_CAP_D			}, // 44 D key  
	{ K_CAP_E,			K_CAP_E			}, // 45 E key  
	{ K_CAP_F,			K_CAP_F			}, // 46 F key  
	{ K_CAP_G,			K_CAP_G			}, // 47 G key  
	{ K_CAP_H,			K_CAP_H			}, // 48 H key  
	{ K_CAP_I,			K_CAP_I			}, // 49 I key  
	{ K_CAP_J,			K_CAP_J			}, // 4A J key  
	{ K_CAP_K,			K_CAP_K			}, // 4B K key  
	{ K_CAP_L,			K_CAP_L			}, // 4C L key  
	{ K_CAP_M,			K_CAP_M			}, // 4D M key  
	{ K_CAP_N,			K_CAP_N			}, // 4E N key  
	{ K_CAP_O,			K_CAP_O			}, // 4F O key  
	{ K_CAP_P,			K_CAP_P			}, // 50 P key  
	{ K_CAP_Q,			K_CAP_Q			}, // 51 Q key  
	{ K_CAP_R,			K_CAP_R			}, // 52 R key  
	{ K_CAP_S,			K_CAP_S			}, // 53 S key  
	{ K_CAP_T,			K_CAP_T			}, // 54 T key  
	{ K_CAP_U,			K_CAP_U			}, // 55 U key  
	{ K_CAP_V,			K_CAP_V			}, // 56 V key  
	{ K_CAP_W,			K_CAP_W			}, // 57 W key  
	{ K_CAP_X,			K_CAP_X			}, // 58 X key  
	{ K_CAP_Y,			K_CAP_Y			}, // 59 Y key  
	{ K_CAP_Z,			K_CAP_Z			}, // 5A Z key  
	{ 0,				0				}, // VK_LWIN 5B Left Windows key (Microsoft® Natural® keyboard)  
	{ 0,				0				}, // VK_RWIN 5C Right Windows key (Natural keyboard)  
	{ 0,				0				}, // VK_APPS 5D Applications key (Natural keyboard)  
	{ 0,				0				}, // 5E Reserved  
	{ 0,				0				}, // VK_SLEEP 5F Computer Sleep key 
	{ K_0,				K_0				}, // VK_NUMPAD0 60 Numeric keypad 0 key  
	{ K_1,				K_1				}, // VK_NUMPAD1 61 Numeric keypad 1 key  
	{ K_2,				K_2				}, // VK_NUMPAD2 62 Numeric keypad 2 key  
	{ K_3,				K_3				}, // VK_NUMPAD3 63 Numeric keypad 3 key  
	{ K_4,				K_4				}, // VK_NUMPAD4 64 Numeric keypad 4 key  
	{ K_5,				K_5				}, // VK_NUMPAD5 65 Numeric keypad 5 key  
	{ K_6,				K_6				}, // VK_NUMPAD6 66 Numeric keypad 6 key  
	{ K_7,				K_7				}, // VK_NUMPAD7 67 Numeric keypad 7 key  
	{ K_8,				K_8				}, // VK_NUMPAD8 68 Numeric keypad 8 key  
	{ K_9,				K_9				}, // VK_NUMPAD9 69 Numeric keypad 9 key  
	{ K_KP_STAR,		K_KP_STAR		}, // VK_MULTIPLY 6A Multiply key  
	{ K_KP_PLUS, 		K_KP_PLUS 		}, // VK_ADD 6B Add key  
	{ 0,				0				}, // VK_SEPARATOR 6C Separator key  
	{ K_KP_MINUS,		K_KP_MINUS		}, // VK_SUBTRACT 6D Subtract key  
	{ K_KP_DEL,			K_KP_DEL		}, // VK_DECIMAL 6E Decimal key  
	{ K_KP_SLASH,		K_KP_SLASH		}, // VK_DIVIDE 6F Divide key  
	{ K_F1,				K_F1			}, // VK_F1 70 F1 key  
	{ K_F2,				K_F2			}, // VK_F2 71 F2 key  
	{ K_F3,				K_F3			}, // VK_F3 72 F3 key  
	{ K_F4,				K_F4			}, // VK_F4 73 F4 key  
	{ K_F5,				K_F5			}, // VK_F5 74 F5 key  
	{ K_F6,				K_F6			}, // VK_F6 75 F6 key  
	{ K_F7,				K_F7			}, // VK_F7 76 F7 key  
	{ K_F8,				K_F8			}, // VK_F8 77 F8 key  
	{ K_F9,				K_F9			}, // VK_F9 78 F9 key  
	{ K_F10,			K_F10			}, // VK_F10 79 F10 key  
	{ K_F11,			K_F11			}, // VK_F11 7A F11 key  
	{ K_F12,			K_F12			}, // VK_F12 7B F12 key  
	{ 0,				0				}, // VK_F13 7C F13 key  
	{ 0,				0				}, // VK_F14 7D F14 key  
	{ 0,				0				}, // VK_F15 7E F15 key  
	{ 0,				0				}, // VK_F16 7F F16 key  
	{ 0,				0				}, // VK_F17 80H F17 key  
	{ 0,				0				}, // VK_F18 81H F18 key  
	{ 0,				0				}, // VK_F19 82H F19 key  
	{ 0,				0				}, // VK_F20 83H F20 key  
	{ 0,				0				}, // VK_F21 84H F21 key  
	{ 0,				0				}, // VK_F22 85H F22 key  
	{ 0,				0				}, // VK_F23 86H F23 key  
	{ 0,				0				}, // VK_F24 87H F24 key  
	{ 0,				0				}, // 88 Unassigned
	{ 0,				0				}, // 89 Unassigned
	{ 0,				0				}, // 8A Unassigned
	{ 0,				0				}, // 8B Unassigned
	{ 0,				0				}, // 8C Unassigned
	{ 0,				0				}, // 8D Unassigned
	{ 0,				0				}, // 8E Unassigned
	{ 0,				0				}, // 8F Unassigned
	{ K_KP_NUMLOCK,		K_KP_NUMLOCK	}, // VK_NUMLOCK 90 NUM LOCK key  
	{ 0,				0	}  // VK_SCROLL 91 
};

/*
=======
MapKey

Map from windows to quake keynums
=======
*/
static int MapKey (ulong key, word wParam)
{
	ulong	result, scan, extended;

	// Check for the console key (hard code to the key you would expect)
	scan = ( key >> 16 ) & 0xff;
	if(scan == CONSOLE_SCAN_CODE)
	{
		return(K_CONSOLE);
	}

	// Try to convert the virtual key directly
	result = 0;
	extended = (key >> 24) & 1;
	if(wParam > 0 && wParam <= VK_SCROLL)
	{
		// yeuch, but oh well...
		//
		if ( wParam >= VK_NUMPAD0 && wParam <= VK_NUMPAD9 )
		{
			bool bNumlockOn = !!(GetKeyState( VK_NUMLOCK ) & 1);
			if ( bNumlockOn )
			{
				wParam = 0x30 + (wParam - VK_NUMPAD0);	// convert to standard 0..9
			}
		}
		result = virtualKeyConvert[wParam][extended];
	}
	// Get the unshifted ascii code (if any)
	if(!result)
	{
		result = MapVirtualKey(wParam, 2) & 0xff;
	}
	// Output any debug prints
//	if(in_debug && in_debug->integer & 1)
//	{
//		Com_Printf("WM_KEY: %x : %x : %x\n", key, wParam, result);
//	}
	return(result);
}


/*
====================
MainWndProc

main window procedure
====================
*/

#define WM_BUTTON4DOWN	(WM_MOUSELAST+2)
#define WM_BUTTON4UP	(WM_MOUSELAST+3)
#define MK_BUTTON4L		0x0020
#define MK_BUTTON4R		0x0040

LONG WINAPI MainWndProc (
    HWND    hWnd,
    UINT    uMsg,
    WPARAM  wParam,
    LPARAM  lParam)
{
	byte code;

	if ( uMsg == MSH_MOUSEWHEEL )
	{
		if ( ( ( int ) wParam ) > 0 )
		{
			Sys_QueEvent( g_wv.sysMsgTime, SE_KEY, K_MWHEELUP, qtrue, 0, NULL );
			Sys_QueEvent( g_wv.sysMsgTime, SE_KEY, K_MWHEELUP, qfalse, 0, NULL );
		}
		else
		{
			Sys_QueEvent( g_wv.sysMsgTime, SE_KEY, K_MWHEELDOWN, qtrue, 0, NULL );
			Sys_QueEvent( g_wv.sysMsgTime, SE_KEY, K_MWHEELDOWN, qfalse, 0, NULL );
		}
        return DefWindowProc (hWnd, uMsg, wParam, lParam);
	}

	switch (uMsg)
	{
	case WM_MOUSEWHEEL:
		//
		//
		// this chunk of code theoretically only works under NT4 and Win98
		// since this message doesn't exist under Win95
		//
		if ( ( short ) HIWORD( wParam ) > 0 )
		{
			Sys_QueEvent( g_wv.sysMsgTime, SE_KEY, K_MWHEELUP, qtrue, 0, NULL );
			Sys_QueEvent( g_wv.sysMsgTime, SE_KEY, K_MWHEELUP, qfalse, 0, NULL );
		}
		else
		{
			Sys_QueEvent( g_wv.sysMsgTime, SE_KEY, K_MWHEELDOWN, qtrue, 0, NULL );
			Sys_QueEvent( g_wv.sysMsgTime, SE_KEY, K_MWHEELDOWN, qfalse, 0, NULL );
		}
		break;

	case WM_CREATE:

		g_wv.hWnd = hWnd;

		vid_xpos = Cvar_Get ("vid_xpos", "3", CVAR_ARCHIVE);
		vid_ypos = Cvar_Get ("vid_ypos", "22", CVAR_ARCHIVE);
		r_fullscreen = Cvar_Get ("r_fullscreen", "1", CVAR_ARCHIVE | CVAR_LATCH );

		MSH_MOUSEWHEEL = RegisterWindowMessage("MSWHEEL_ROLLMSG"); 
		if ( r_fullscreen->integer )
		{
			WIN_DisableAltTab();
		}
		else
		{
			WIN_EnableAltTab();
		}

		break;
#if 0
	case WM_DISPLAYCHANGE:
		Com_DPrintf( "WM_DISPLAYCHANGE\n" );
		// we need to force a vid_restart if the user has changed
		// their desktop resolution while the game is running,
		// but don't do anything if the message is a result of
		// our own calling of ChangeDisplaySettings
		if ( com_insideVidInit ) {
			break;		// we did this on purpose
		}
		// something else forced a mode change, so restart all our gl stuff
		Cbuf_AddText( "vid_restart\n" );
		break;
#endif
	case WM_DESTROY:
		// let sound and input know about this?
		g_wv.hWnd = NULL;
		if ( r_fullscreen->integer )
		{
			WIN_EnableAltTab();
		}
		break;

	case WM_CLOSE:
		Cbuf_ExecuteText( EXEC_APPEND, "quit" );
		break;

	case WM_ACTIVATE:
		{
			int	fActive, fMinimized;

			fActive = LOWORD(wParam);
			fMinimized = (BOOL) HIWORD(wParam);

			VID_AppActivate( fActive != WA_INACTIVE, fMinimized);
			SNDDMA_Activate( (qboolean)(fActive != WA_INACTIVE && !fMinimized) );
		}
		break;

	case WM_MOVE:
		{
			int		xPos, yPos;
			RECT r;
			int		style;

			if (!r_fullscreen->integer )
			{
				xPos = (short) LOWORD(lParam);    // horizontal position 
				yPos = (short) HIWORD(lParam);    // vertical position 

				r.left   = 0;
				r.top    = 0;
				r.right  = 1;
				r.bottom = 1;

				style = GetWindowLong( hWnd, GWL_STYLE );
				AdjustWindowRect( &r, style, FALSE );

				Cvar_SetValue( "vid_xpos", xPos + r.left);
				Cvar_SetValue( "vid_ypos", yPos + r.top);
				vid_xpos->modified = qfalse;
				vid_ypos->modified = qfalse;
				if ( g_wv.activeApp )
				{
					IN_Activate (qtrue);
				}
			}
		}
		break;

// this is complicated because Win32 seems to pack multiple mouse events into
// one update sometimes, so we always check all states and look for events
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_BUTTON4DOWN:
	case WM_BUTTON4UP:
		{
			int	temp;

			temp = 0;

			if (wParam & MK_LBUTTON)
				temp |= 1;

			if (wParam & MK_RBUTTON)
				temp |= 2;

			if (wParam & MK_MBUTTON)
				temp |= 4;

		 	if (wParam & MK_BUTTON4L)
				temp |= 8;

			if (wParam & MK_BUTTON4R)
				temp |= 16;

			IN_MouseEvent (temp);
		}
		break;

	case WM_SYSCOMMAND:
		if ( (wParam&0xFFF0) == SC_SCREENSAVE || (wParam&0xFFF0) == SC_MONITORPOWER)
		{
			return 0;
		}
		break;

	case WM_SYSKEYDOWN:
		if ( wParam == VK_RETURN )
		{
			if ( r_fullscreen && cl_allowAltEnter &&
				(cls.state==CA_DISCONNECTED ||  cls.state==CA_CONNECTED)
				)
			{
				if (cl_allowAltEnter->integer)
				{
					Cvar_SetValue( "r_fullscreen", !r_fullscreen->integer );
					Cbuf_AddText( "vid_restart\n" );
				}
			}
			return 0;
		}
		// fall through
	case WM_KEYDOWN:
		code = MapKey( lParam, wParam );
		if(code)
		{
			Sys_QueEvent( g_wv.sysMsgTime, SE_KEY, code, qtrue, 0, NULL );
		}
		break;

	case WM_SYSKEYUP:
	case WM_KEYUP:
		code = MapKey( lParam, wParam );
		if(code)
		{
			Sys_QueEvent( g_wv.sysMsgTime, SE_KEY, code, qfalse, 0, NULL );
		}
		break;

	case WM_CHAR:
		if(((lParam >> 16) & 0xff) != CONSOLE_SCAN_CODE)
		{
			Sys_QueEvent( g_wv.sysMsgTime, SE_CHAR, wParam, 0, 0, NULL );
		}
		// Output any debug prints
//		if(in_debug && in_debug->integer & 2)
//		{
//			Com_Printf("WM_CHAR: %x\n", wParam);
//		}
		break;

	case WM_POWERBROADCAST:
		if (wParam == PBT_APMQUERYSUSPEND)
		{
#ifndef FINAL_BUILD
			Com_Printf("Cannot go into hibernate / standby mode while game is running!\n");
#endif
			return BROADCAST_QUERY_DENY;
		}
		break;
   }

    return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

