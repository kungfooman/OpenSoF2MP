// Copyright (C) 2000-2002 Raven Software, Inc.
//

#define _STR(x) #x
#define STR(x) _STR(x)

// Current version of the multi player game
#define VERSION_MAJOR_RELEASE		1
#define VERSION_MINOR_RELEASE		0
#define VERSION_EXTERNAL_BUILD		3
#define VERSION_INTERNAL_BUILD		0

#define VERSION_STRING				STR(VERSION_MAJOR_RELEASE) ", " STR(VERSION_MINOR_RELEASE) ", " STR(VERSION_EXTERNAL_BUILD) ", " STR(VERSION_INTERNAL_BUILD) // "1, 0, 1, 0"
#define VERSION_STRING_DOTTED		STR(VERSION_MAJOR_RELEASE) "." STR(VERSION_MINOR_RELEASE) STR(VERSION_EXTERNAL_BUILD) // "1.03"

#if defined(_DEBUG)
	#define	JK_VERSION		"(debug)SOF2MP GOLD V"VERSION_STRING_DOTTED
#elif defined(FINAL_BUILD)
	#define	JK_VERSION		"SOF2MP GOLD V"VERSION_STRING_DOTTED
#else
	#define	JK_VERSION		"(internal)SOF2MP GOLD V"VERSION_STRING_DOTTED
#endif
