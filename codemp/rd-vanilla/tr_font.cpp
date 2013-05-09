// leave this as first line for PCH reasons...
//
#include "server/exe_headers.h"

#include "qcommon/sstring.h"	// stl string class won't compile in here (MS shite), so use Gil's.
#include "tr_local.h"
#include "tr_font.h"

#include "qcommon/stringed_ingame.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// This file is shared in the single and multiplayer codebases, so be CAREFUL WHAT YOU ADD/CHANGE!!!!!
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef enum
{
	eWestern,	// ( I only care about asian languages in here at the moment )
	eRussian,	//  .. but now I need to care about this, since it uses a different TP
	ePolish,	// ditto
} Language_e;

// this is to cut down on all the stupid string compares I've been doing, and convert asian stuff to switch-case
//
Language_e GetLanguageEnum()
{
	static int			iSE_Language_ModificationCount = -1234;	// any old silly value that won't match the cvar mod count
	static Language_e	eLanguage = eWestern;

	// only re-strcmp() when language string has changed from what we knew it as...
	//
	if (iSE_Language_ModificationCount != se_language->modificationCount )
	{
		iSE_Language_ModificationCount  = se_language->modificationCount;

				if ( Language_IsRussian()	)	eLanguage = eRussian;
		else	if ( Language_IsPolish()	)	eLanguage = ePolish;
		else	eLanguage = eWestern;
	}

	return eLanguage;
}

struct SBCSOverrideLanguages_t
{
	LPCSTR		m_psName;
	Language_e	m_eLanguage;
};

// so I can do some stuff with for-next loops when I add polish etc...
//
SBCSOverrideLanguages_t g_SBCSOverrideLanguages[]=
{
	{"russian",	eRussian},
	{"polish",	ePolish},
	{NULL,		eWestern}
};



//================================================
//

class CFontInfo
{
private:
	// From the fontdat file
	glyphInfo_t		mGlyphs[GLYPH_COUNT];

	// end of fontdat data

	int				mShader;   				// handle to the shader with the glyph
	
	int				m_iLanguageModificationCount;	// doesn't matter what this is, so long as it's comparable as being changed

public:
	char			m_sFontName[MAX_QPATH];	// eg "fonts/lcd"	// needed for korean font-hint if we need >1 hangul set
	int				mPointSize;
	int				mHeight;
	int				mAscender;
	int				mDescender;

	bool			mbRoundCalcs;	// trying to make this !@#$%^ thing work with scaling
	int				m_iThisFont;	// handle to itself
	int				m_iAltSBCSFont;	// -1 == NULL // alternative single-byte font for languages like russian/polish etc that need to override high characters ?
	int				m_iOriginalFontWhenSBCSOverriden;
	float			m_fAltSBCSFontScaleFactor;	// -1, else amount to adjust returned values by to make them fit the master western font they're substituting for
	bool			m_bIsFakeAlienLanguage;	// ... if true, don't process as MBCS or override as SBCS etc

	CFontInfo(const char *fontName);
//	CFontInfo(int fill) { memset(this, fill, sizeof(*this)); }	// wtf?
	~CFontInfo(void) {}	

	const int GetPointSize(void) const { return(mPointSize); }
	const int GetHeight(void) const { return(mHeight); }
	const int GetAscender(void) const { return(mAscender); }
	const int GetDescender(void) const { return(mDescender); }

	const glyphInfo_t *GetLetter(const unsigned int uiLetter, int *piShader = NULL);

	const int GetLetterWidth(const unsigned int uiLetter);
	const int GetLetterHorizAdvance(const unsigned int uiLetter);
	const int GetShader(void) const { return(mShader); }
};

//================================================




// round float to one decimal place...
//
float RoundTenth( float fValue )
{
	return ( floorf( (fValue*10.0f) + 0.5f) ) / 10.0f;
}


int							g_iCurrentFontIndex;	// entry 0 is reserved index for missing/invalid, else ++ with each new font registered
vector<CFontInfo *>			g_vFontArray;
typedef map<sstring_t, int>	FontIndexMap_t;
							FontIndexMap_t g_mapFontIndexes;
int g_iNonScaledCharRange;	// this is used with auto-scaling of asian fonts, anything below this number is preserved in scale, anything above is scaled down by 0.75f

//paletteRGBA_c				lastcolour;

// ============================================================================

// takes char *, returns integer char at that point, and advances char * on by enough bytes to move
//	past the letter (either western 1 byte or Asian multi-byte)...
//
// looks messy, but the actual execution route is quite short, so it's fast...
//
// Note that I have to have this 3-param form instead of advancing a passed-in "const char **psText" because of VM-crap where you can only change ptr-contents, not ptrs themselves. Bleurgh. Ditto the qtrue:qfalse crap instead of just returning stuff straight through.
//
unsigned int AnyLanguage_ReadCharFromString( const char *psText, int *piAdvanceCount, qboolean *pbIsTrailingPunctuation /* = NULL */)
{	
	const byte *psString = (const byte *) psText;	// avoid sign-promote bug
	unsigned int uiLetter;

	// ... must not have been an MBCS code...
	//
	uiLetter = psString[0];
	*piAdvanceCount = 1;

	if (pbIsTrailingPunctuation)
	{
		*pbIsTrailingPunctuation = (uiLetter == '!' || 
									uiLetter == '?' || 
									uiLetter == ',' || 
									uiLetter == '.' || 
									uiLetter == ';' || 
									uiLetter == ':'
									) ? qtrue : qfalse;
	}

	return uiLetter;
}


// ======================================================================
// name is (eg) "ergo" or "lcd", no extension.
//
//  If path present, it's a special language hack for SBCS override languages, eg: "lcd/russian", which means
//	  just treat the file as "russian", but with the "lcd" part ensuring we don't find a different registered russian font
//
CFontInfo::CFontInfo(const char *_fontName)
{
	int			len, i;
	void		*buff;
	dfontdat_t	*fontdat;

	// remove any special hack name insertions...
	//
	char fontName[MAX_QPATH];
	sprintf(fontName,"fonts/%s.fontdat",COM_SkipPath(const_cast<char*>(_fontName)));	// COM_SkipPath should take a const char *, but it's just possible people use it as a char * I guess, so I have to hack around like this <groan>
	
	// clear some general things...
	//
	m_iAltSBCSFont = -1;
	m_iThisFont = -1;
	m_iOriginalFontWhenSBCSOverriden = -1;
	m_fAltSBCSFontScaleFactor = -1;
	m_bIsFakeAlienLanguage = !strcmp(_fontName,"aurabesh");	// dont try and make SBCS or asian overrides for this

	len = ri.FS_ReadFile(fontName, NULL);
	if (len == sizeof(dfontdat_t))
	{
		ri.FS_ReadFile(fontName, &buff);
		fontdat = (dfontdat_t *)buff;

		for(i = 0; i < GLYPH_COUNT; i++)
		{
			mGlyphs[i] = fontdat->mGlyphs[i];
		}
		mPointSize = fontdat->mPointSize;
		mHeight = fontdat->mHeight;
		mAscender = fontdat->mAscender;
		mDescender = fontdat->mDescender;
		mbRoundCalcs = !!strstr(fontName,"ergo");

		// cope with bad fontdat headers...
		//
		if (mHeight == 0)
		{
			mHeight = mPointSize;
            mAscender = mPointSize - Round( ((float)mPointSize/10.0f)+2 );	// have to completely guess at the baseline... sigh.
            mDescender = mHeight - mAscender;
		}

		ri.FS_FreeFile(buff);
	}
	else
	{
		mHeight = 0;
		mShader = 0;
	}

	Q_strncpyz(m_sFontName, fontName, sizeof(m_sFontName));
	COM_StripExtension( m_sFontName, m_sFontName, sizeof( m_sFontName ) );	// so we get better error printing if failed to load shader (ie lose ".fontdat")
	mShader = RE_RegisterShaderNoMip(m_sFontName);

	// finished...
	g_vFontArray.resize(g_iCurrentFontIndex + 1);
	g_vFontArray[g_iCurrentFontIndex++] = this;


	if ( ri.Cvar_VariableIntegerValue( "com_buildScript" ) == 2)
	{
		Com_Printf( "com_buildScript(2): Registering foreign fonts...\n" );
		static qboolean bDone = qfalse;	// Do this once only (for speed)...
		if (!bDone)
		{
			bDone = qtrue;

			int iGlyphTPs = 0;
			const char *psLang = NULL;

			// SBCS override languages...
			//
			fileHandle_t f;	
			for (int i=0; g_SBCSOverrideLanguages[i].m_psName ;i++)
			{
				char sTemp[MAX_QPATH];

				sprintf(sTemp,"fonts/%s.tga", g_SBCSOverrideLanguages[i].m_psName );
				ri.FS_FOpenFileRead( sTemp, &f, qfalse );
				if (f) ri.FS_FCloseFile( f );

				sprintf(sTemp,"fonts/%s.fontdat", g_SBCSOverrideLanguages[i].m_psName );
				ri.FS_FOpenFileRead( sTemp, &f, qfalse );
				if (f) ri.FS_FCloseFile( f );
			}
		}
	}
}

static CFontInfo *GetFont_Actual(int index)
{
	index &= SET_MASK;
	if((index >= 1) && (index < g_iCurrentFontIndex))
	{
		CFontInfo *pFont = g_vFontArray[index];
		return pFont;
	}
	return(NULL);
}


// needed to add *piShader param because of multiple TPs, 
//	if not passed in, then I also skip S,T calculations for re-usable static asian glyphinfo struct...
//
const glyphInfo_t *CFontInfo::GetLetter(const unsigned int uiLetter, int *piShader /* = NULL */)
{ 	
	if (piShader)
	{
		*piShader = GetShader();
	}

	const glyphInfo_t *pGlyph = &mGlyphs[ uiLetter & 0xff ];
	return pGlyph;
}

const int CFontInfo::GetLetterWidth(unsigned int uiLetter)
{
	const glyphInfo_t *pGlyph = GetLetter( uiLetter );
	return pGlyph->width ? pGlyph->width : mGlyphs['.'].width;
}

const int CFontInfo::GetLetterHorizAdvance(unsigned int uiLetter)
{	
	const glyphInfo_t *pGlyph = GetLetter( uiLetter );
	return pGlyph->horizAdvance ? pGlyph->horizAdvance : mGlyphs['.'].horizAdvance;
}

// ensure any GetFont calls that need SBCS overriding (such as when playing in Russian) have the appropriate stuff done...
//
static CFontInfo *GetFont_SBCSOverride(CFontInfo *pFont, Language_e eLanguageSBCS, LPCSTR psLanguageNameSBCS )
{
	if ( !pFont->m_bIsFakeAlienLanguage )
	{
		if ( GetLanguageEnum() == eLanguageSBCS )
		{
			if ( pFont->m_iAltSBCSFont == -1 ) 	// no reg attempted yet?
			{
				// need to register this alternative SBCS font...
				//
				int iAltFontIndex = RE_RegisterFont( va("%s/%s",COM_SkipPath(pFont->m_sFontName),psLanguageNameSBCS) );	// ensure unique name (eg: "lcd/russian")
				CFontInfo *pAltFont = GetFont_Actual( iAltFontIndex );
				if ( pAltFont )
				{
					// work out the scaling factor for this font's glyphs...( round it to 1 decimal place to cut down on silly scale factors like 0.53125 )
					//
					pAltFont->m_fAltSBCSFontScaleFactor = RoundTenth((float)pFont->GetPointSize() / (float)pAltFont->GetPointSize());
					//
					// then override with the main properties of the original font...
					//
					pAltFont->mPointSize = pFont->GetPointSize();//(float) pAltFont->GetPointSize() * pAltFont->m_fAltSBCSFontScaleFactor;
					pAltFont->mHeight	 = pFont->GetHeight();//(float) pAltFont->GetHeight()	* pAltFont->m_fAltSBCSFontScaleFactor;
					pAltFont->mAscender	 = pFont->GetAscender();//(float) pAltFont->GetAscender()	* pAltFont->m_fAltSBCSFontScaleFactor;
					pAltFont->mDescender = pFont->GetDescender();//(float) pAltFont->GetDescender()	* pAltFont->m_fAltSBCSFontScaleFactor;

//					pAltFont->mPointSize = (float) pAltFont->GetPointSize() * pAltFont->m_fAltSBCSFontScaleFactor;
//					pAltFont->mHeight	 = (float) pAltFont->GetHeight()	* pAltFont->m_fAltSBCSFontScaleFactor;
//					pAltFont->mAscender	 = (float) pAltFont->GetAscender()	* pAltFont->m_fAltSBCSFontScaleFactor;
//					pAltFont->mDescender = (float) pAltFont->GetDescender()	* pAltFont->m_fAltSBCSFontScaleFactor;

					pAltFont->mbRoundCalcs = true;
					pAltFont->m_iOriginalFontWhenSBCSOverriden = pFont->m_iThisFont;
				}
				pFont->m_iAltSBCSFont = iAltFontIndex;
			}

			if ( pFont->m_iAltSBCSFont > 0)
			{
				return GetFont_Actual( pFont->m_iAltSBCSFont );
			}
		}
	}

	return NULL;
}



CFontInfo *GetFont(int index)
{
	CFontInfo *pFont = GetFont_Actual( index );

	if (pFont)
	{
		// any SBCS overrides? (this has to be pretty quick, and is (sort of))...
		//
		for (int i=0; g_SBCSOverrideLanguages[i].m_psName; i++)
		{
			CFontInfo *pAltFont = GetFont_SBCSOverride( pFont, g_SBCSOverrideLanguages[i].m_eLanguage, g_SBCSOverrideLanguages[i].m_psName );
			if (pAltFont) 
			{
				return pAltFont;
			}
		}
	}

	return pFont;
}


int RE_Font_StrLenPixels(const char *psText, const int iFontHandle, const float fScale)
{			
	int			iMaxWidth = 0;
	int			iThisWidth= 0;
	CFontInfo	*curfont;

	curfont = GetFont(iFontHandle);
	if(!curfont)
	{
		return(0);
	}

	float fScaleA = fScale;
	while(*psText)
	{
		int iAdvanceCount;
		unsigned int uiLetter = AnyLanguage_ReadCharFromString( psText, &iAdvanceCount, NULL );
		psText += iAdvanceCount;

		if (uiLetter == '^' )
		{
			if (*psText >= '0' &&
				*psText <= '9')
			{
				uiLetter = AnyLanguage_ReadCharFromString( psText, &iAdvanceCount, NULL );
				psText += iAdvanceCount;
				continue;
			}
		}

		if (uiLetter == 0x0A)
		{
			iThisWidth = 0;
		}
		else
		{
			int iPixelAdvance = curfont->GetLetterHorizAdvance( uiLetter );
	
			float fValue = iPixelAdvance * ((uiLetter > g_iNonScaledCharRange) ? fScaleA : fScale);
			iThisWidth += curfont->mbRoundCalcs ? Round( fValue ) : fValue;
			if (iThisWidth > iMaxWidth)
			{
				iMaxWidth = iThisWidth;
			}
		}
	}

	return iMaxWidth;
}

// not really a font function, but keeps naming consistant...
//
int RE_Font_StrLenChars(const char *psText)
{			
	// logic for this function's letter counting must be kept same in this function and RE_Font_DrawString()
	//
	int iCharCount = 0;

	while ( *psText )
	{
		// in other words, colour codes and CR/LF don't count as chars, all else does...
		//
		int iAdvanceCount;
		unsigned int uiLetter = AnyLanguage_ReadCharFromString( psText, &iAdvanceCount, NULL );
		psText += iAdvanceCount;

		switch (uiLetter)
		{
			case '^':
				if (*psText >= '0' &&
					*psText <= '9')
				{
					psText++;
				}
				else
				{
					iCharCount++;
				}
				break;	// colour code (note next-char skip)
			case 10:								break;	// linefeed
			case 13:								break;	// return 
			case '_':	iCharCount += 1; break;	// special word-break hack
			default:	iCharCount++;				break;
		}
	}
	
	return iCharCount;
}

int RE_Font_HeightPixels(const int iFontHandle, const float fScale)
{			
	CFontInfo	*curfont;

	curfont = GetFont(iFontHandle);
	if(curfont)
	{
		float fValue = curfont->GetPointSize() * fScale;
		return curfont->mbRoundCalcs ? Round(fValue) : fValue;
	}
	return(0);
}

// limit is 0 for "all of string", else display count...
//
void RE_Font_DrawString(int x, int y, qhandle_t font, float scale, vec4_t color, const char* text, int limit, int flags, int cursorPos, char cursor)
{
	int					curX, curY, colour;
	const glyphInfo_t	*letter;
	qhandle_t			shader;

	assert (text);

	if(font & STYLE_BLINK)
	{
		if((ri.Milliseconds() >> 7) & 1)
		{
			return;
		}
	}

	CFontInfo *curfont = GetFont(font);
	if(!curfont || !text)
	{
		return;
	}

	RE_SetColor( color );

	curX = x;
	y += Round((curfont->GetHeight() - (curfont->GetDescender() >> 1)) * scale);

	qboolean nextTextWouldOverflow = qfalse;
	const char *textStart = text;
	while (*text && !nextTextWouldOverflow)
	{
		int advanceCount;
		unsigned int uiLetter = AnyLanguage_ReadCharFromString( text, &advanceCount, NULL );
		text += advanceCount;

		switch( uiLetter )
		{
		case 10:						//linefeed
			curX = x;
			y += Round(curfont->GetPointSize() * scale);
			break;
		case 13:						// Return
			break;
		case 32:						// Space
			letter = curfont->GetLetter(' ');			
			curX += Round(letter->horizAdvance * scale);
			nextTextWouldOverflow = (limit && (text - textStart) > limit) ? qtrue : qfalse;	// yeuch
			break;
		case '^':
			if (*text >= '0' &&
				*text <= '9')
			{
				colour = ColorIndex(*text++);
				RE_SetColor( g_color_table[colour] );
				break;
			}
			//purposely falls thrugh
		default:
			letter = curfont->GetLetter( uiLetter, &shader );			// Description of letter
			if(!letter->width)
			{
				letter = curfont->GetLetter('.');
			}

			int advancePixels = Round(letter->horizAdvance * scale);
			nextTextWouldOverflow = (limit && (text - textStart) > limit) ? qtrue : qfalse;	// yeuch
			if (!nextTextWouldOverflow)
			{
				// this 'mbRoundCalcs' stuff is crap, but the only way to make the font code work. Sigh...
				//				
				curY = y - (curfont->mbRoundCalcs ? Round(letter->baseline * scale) : letter->baseline * scale);
				if (curfont->m_fAltSBCSFontScaleFactor != -1)
				{
					curY += 3;	// I'm sick and tired of going round in circles trying to do this legally, so bollocks to it
				}

				RE_StretchPic ( curX + Round(letter->horizOffset * scale), // float x
								curY,	// float y
								curfont->mbRoundCalcs ? Round(letter->width * scale) : letter->width * scale,	// float w
								curfont->mbRoundCalcs ? Round(letter->height * scale) : letter->height * scale, // float h
								letter->s,						// float s1
								letter->t,						// float t1
								letter->s2,					// float s2
								letter->t2,					// float t2
								//lastcolour.c, 
								shader							// qhandle_t hShader
								);

				curX += advancePixels;
			}

			break;
		}
	}

	if (cursor && (ri.Milliseconds() >> 7) & 1) {
		// draw blinking cursor
		curX = x;
		for ( int i = 0 ; i < cursorPos ; ++i ) {
			letter = curfont->GetLetter( textStart[i] );			// Description of letter
			curX += Round(letter->horizAdvance * scale);
		}
		letter = curfont->GetLetter( cursor, &shader );			// Description of letter
		if(!letter->width)
		{
			letter = curfont->GetLetter('.');
		}
		curY = y - (curfont->mbRoundCalcs ? Round(letter->baseline * scale) : letter->baseline * scale);
		RE_StretchPic ( curX + Round(letter->horizOffset * scale), // float x
						curY,	// float y
						curfont->mbRoundCalcs ? Round(letter->width * scale) : letter->width * scale,	// float w
						curfont->mbRoundCalcs ? Round(letter->height * scale) : letter->height * scale, // float h
						letter->s,						// float s1
						letter->t,						// float t1
						letter->s2,					// float s2
						letter->t2,					// float t2
						//lastcolour.c, 
						shader							// qhandle_t hShader
						);
	}
	//let it remember the old color //RE_SetColor(NULL);;
}

int RE_RegisterFont(const char *psName) 
{
	FontIndexMap_t::iterator it = g_mapFontIndexes.find(psName);
	if (it != g_mapFontIndexes.end() )
	{
		int iFontIndex = (*it).second;
		return iFontIndex;
	}

	// not registered, so...
	//
	{
		CFontInfo *pFont = new CFontInfo(psName);
		if (pFont->GetPointSize() > 0)
		{
			int iFontIndex = g_iCurrentFontIndex - 1;
			g_mapFontIndexes[psName] = iFontIndex;
			pFont->m_iThisFont = iFontIndex;
			return iFontIndex;
		}
		else
		{
			g_mapFontIndexes[psName] = 0;	// missing/invalid
		}
	}

	return 0;
}

void R_InitFonts(void)
{
	g_iCurrentFontIndex = 1;			// entry 0 is reserved for "missing/invalid"
	g_iNonScaledCharRange = INT_MAX;	// default all chars to have no special scaling (other than user supplied)
}

/*
===============
R_FontList_f
===============
*/
void R_FontList_f( void ) {
	Com_Printf ("------------------------------------\n");

	FontIndexMap_t::iterator it;
	for (it = g_mapFontIndexes.begin(); it != g_mapFontIndexes.end(); ++it)
	{
		CFontInfo *font = GetFont((*it).second);
		if( font )
		{
			Com_Printf("%3i:%s  ps:%hi h:%hi a:%hi d:%hi\n", (*it).second, font->m_sFontName,
				font->mPointSize, font->mHeight, font->mAscender, font->mDescender);
		}
	}
	Com_Printf ("------------------------------------\n");
}

void R_ShutdownFonts(void)
{
	for(int i = 1; i < g_iCurrentFontIndex; i++)	// entry 0 is reserved for "missing/invalid"
	{
		delete g_vFontArray[i];
	}
	g_mapFontIndexes.clear();
	g_vFontArray.clear();
	g_iCurrentFontIndex = 1;	// entry 0 is reserved for "missing/invalid"
}

// this is only really for debugging while tinkering with fonts, but harmless to leave in...
//
void R_ReloadFonts_f(void)
{
	// first, grab all the currently-registered fonts IN THE ORDER THEY WERE REGISTERED...
	//
	vector <sstring_t> vstrFonts;

	int iFontToFind;
	for (iFontToFind = 1; iFontToFind < g_iCurrentFontIndex; iFontToFind++)
	{	
		FontIndexMap_t::iterator it;
		for (it = g_mapFontIndexes.begin(); it != g_mapFontIndexes.end(); ++it)
		{
			if (iFontToFind == (*it).second)
			{
				vstrFonts.push_back( (*it).first );
				break;
			}
		}
		if (it == g_mapFontIndexes.end() )
		{
			break;	// couldn't find this font
		}
	}
	if ( iFontToFind == g_iCurrentFontIndex ) // found all of them?
	{
		// now restart the font system...
		//
		R_ShutdownFonts();
		R_InitFonts();
		//
		// and re-register our fonts in the same order as before (note that some menu items etc cache the string lengths so really a vid_restart is better, but this is just for my testing)
		//
		for (int iFont = 0; iFont < vstrFonts.size(); iFont++)
		{
#ifdef _DEBUG
			int iNewFontHandle = RE_RegisterFont( vstrFonts[iFont].c_str() );
			assert( iNewFontHandle == iFont+1 );
#else
			RE_RegisterFont( vstrFonts[iFont].c_str() );
#endif
		}
		Com_Printf( "Done.\n" );
	}
	else
	{
		Com_Printf( "Problem encountered finding current fonts, ignoring.\n" );	// poo. Oh well, forget it.
	}
}


// end
