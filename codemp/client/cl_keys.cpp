//Anything above this #include will be ignored by the compiler
#include "qcommon/exe_headers.h"

#include "client.h"
#include "qcommon/stringed_ingame.h"
/*

key up events are sent even if in console mode

*/

field_t		chatField;
qboolean	chat_team;

int			chat_playerNum;

keyGlobals_t	kg;										

// do NOT blithely change any of the key names (3rd field) here, since they have to match the key binds
//	in the CFG files, they're also prepended with "KEYNAME_" when looking up StringEd references
//
keyname_t keynames[K_LAST_KEY] =							
{			
	{ 0x00, 0x00, NULL, A_NULL, false									}											
	{ 0x01, 0x01, "SHIFT", K_SHIFT, false 								},					       
	{ 0x02, 0x02, "CTRL", K_CTRL, false   								},					       
	{ 0x03, 0x03, "ALT", K_ALT, false									},					       
	{ 0x04, 0x04, "CAPSLOCK", K_CAPSLOCK, false							},					       
	{ 0x05, 0x05, "KP_NUMLOCK", K_KP_NUMLOCK, false						},					       
	{ 0x06, 0x06, "SCROLLLOCK", NULL, false								},					       
	{ 0x07, 0x07, "PAUSE", K_PAUSE, false								},					       
	{ 0x08, 0x08, "BACKSPACE", K_BACKSPACE, false						},					       
	{ 0x09, 0x09, "TAB", K_TAB, false									},					       
	{ 0x0a, 0x0a, "ENTER", K_ENTER, false								},					       
	{ 0x0b, 0x0b, "KP_PLUS", K_KP_PLUS, false							},					       
	{ 0x0c, 0x0c, "KP_MINUS", K_KP_MINUS, false							},					       
	{ 0x0d, 0x0d, "KP_ENTER", K_KP_ENTER, false							},					       
	{ 0x0e, 0x0e, "KP_DEL", K_KP_DEL, false								},					       
	{ 0x0f, 0x0f, NULL, NULL, false										},
	{ 0x10, 0x10, "KP_INS", K_KP_INS, false								},					       
	{ 0x11, 0x11, "KP_END", K_KP_END, false								},					       
	{ 0x12, 0x12, "KP_DOWNARROW", K_KP_DOWNARROW, false					},					       
	{ 0x13, 0x13, "KP_PGDN", K_KP_PGDN, false							},					       
	{ 0x14, 0x14, "KP_LEFTARROW", K_KP_LEFTARROW, false					},					       
	{ 0x15, 0x15, "KP_5", K_KP_5, false									},					       
	{ 0x16, 0x16, "KP_RIGHTARROW", K_KP_RIGHTARROW, false				},					       
	{ 0x17, 0x17, "KP_HOME", K_KP_HOME, false							},					       
	{ 0x18, 0x18, "KP_UPARROW", K_KP_UPARROW, false						},					       
	{ 0x19, 0x19, "KP_PGUP", K_KP_PGUP, false							},					       
	{ 0x1a, 0x1a, "CONSOLE", K_CONSOLE, false 							},					       
	{ 0x1b, 0x1b, "ESCAPE", K_ESCAPE, false								},					       
	{ 0x1c, 0x1c, "F1", K_F1, true										},					       
	{ 0x1d, 0x1d, "F2", K_F2, true										},					       
	{ 0x1e, 0x1e, "F3", K_F3, true										},					       
	{ 0x1f, 0x1f, "F4", K_F4, true										},					       
			    													        
	{ 0x20, 0x20, "SPACE", K_SPACE, false								},
	{ (word)'!', (word)'!', NULL, K_PLING, false		  				},
	{ (word)'"', (word)'"', NULL, K_DOUBLE_QUOTE, false  				},
	{ (word)'#', (word)'#', NULL, K_HASH, false		  					},
	{ (word)'$', (word)'$', NULL, K_STRING, false						},
	{ (word)'%', (word)'%', NULL, K_PERCENT, false						},
	{ (word)'&', (word)'&', NULL, K_AND, false							},
	{ 0x27, 0x27, NULL, K_SINGLE_QUOTE, false							},
	{ (word)'(', (word)'(', NULL, K_OPEN_BRACKET, false					},
	{ (word)')', (word)')', NULL, K_CLOSE_BRACKET, false				},
	{ (word)'*', (word)'*', NULL, K_STAR, false							},
	{ (word)'+', (word)'+', NULL, K_PLUS, false							},
	{ (word)',', (word)',', NULL, K_COMMA, false						},
	{ (word)'-', (word)'-', NULL, K_MINUS, false						},
	{ (word)'.', (word)'.', NULL, K_PERIOD, false						},
	{ (word)'/', (word)'/', NULL, K_FORWARD_SLASH, false				},
	{ (word)'0', (word)'0', NULL, K_0, false							},
	{ (word)'1', (word)'1', NULL, K_1, false							},
	{ (word)'2', (word)'2', NULL, K_2, false							},
	{ (word)'3', (word)'3', NULL, K_3, false							},
	{ (word)'4', (word)'4', NULL, K_4, false							},
	{ (word)'5', (word)'5', NULL, K_5, false							},
	{ (word)'6', (word)'6', NULL, K_6, false							},
	{ (word)'7', (word)'7', NULL, K_7, false							},
	{ (word)'8', (word)'8', NULL, K_8, false							},
	{ (word)'9', (word)'9', NULL, K_9, false							},
	{ (word)':', (word)':', NULL, K_COLON, false						},
	{ (word)';', (word)';', "SEMICOLON", K_SEMICOLON, false				},
	{ (word)'<', (word)'<', NULL, K_LESSTHAN, false						},
	{ (word)'=', (word)'=', NULL, K_EQUALS, false						},
	{ (word)'>', (word)'>', NULL, K_GREATERTHAN, false					},
	{ (word)'?', (word)'?', NULL, K_QUESTION, false						},
			    													
	{ (word)'@', (word)'@', NULL, K_AT, false							},	   	            
	{ (word)'A', (word)'a', NULL, K_CAP_A, false						},		            
	{ (word)'B', (word)'b', NULL, K_CAP_B, false						},		            
	{ (word)'C', (word)'c', NULL, K_CAP_C, false						},		            
	{ (word)'D', (word)'d', NULL, K_CAP_D, false						},		            
	{ (word)'E', (word)'e', NULL, K_CAP_E, false						},		            
	{ (word)'F', (word)'f', NULL, K_CAP_F, false						},		            
	{ (word)'G', (word)'g', NULL, K_CAP_G, false						},		            
	{ (word)'H', (word)'h', NULL, K_CAP_H, false						},		            
	{ (word)'I', (word)'i', NULL, K_CAP_I, false						},		            
	{ (word)'J', (word)'j', NULL, K_CAP_J, false						},		            
	{ (word)'K', (word)'k', NULL, K_CAP_K, false						},		            
	{ (word)'L', (word)'l', NULL, K_CAP_L, false						},		            
	{ (word)'M', (word)'m', NULL, K_CAP_M, false						},		            
	{ (word)'N', (word)'n', NULL, K_CAP_N, false						},		            
	{ (word)'O', (word)'o', NULL, K_CAP_O, false						},		            
	{ (word)'P', (word)'p', NULL, K_CAP_P, false						},		            
	{ (word)'Q', (word)'q', NULL, K_CAP_Q, false						},		            
	{ (word)'R', (word)'r', NULL, K_CAP_R, false						},		            
	{ (word)'S', (word)'s', NULL, K_CAP_S, false						},		            
	{ (word)'T', (word)'t', NULL, K_CAP_T, false						},		            
	{ (word)'U', (word)'u', NULL, K_CAP_U, false						},		            
	{ (word)'V', (word)'v', NULL, K_CAP_V, false						},		            
	{ (word)'W', (word)'w', NULL, K_CAP_W, false						},		            
	{ (word)'X', (word)'x', NULL, K_CAP_X, false						},		            
	{ (word)'Y', (word)'y', NULL, K_CAP_Y, false						},		            
	{ (word)'Z', (word)'z', NULL, K_CAP_Z, false						},		            
	{ (word)'[', (word)'[', NULL, K_OPEN_SQUARE, false					},					    
	{ 0x5c, 0x5c, NULL, K_BACKSLASH, false								},					    
	{ (word)']', (word)']', NULL, K_CLOSE_SQUARE, false 				},					    
	{ (word)'^', (word)'^', NULL, K_CARET, false		 				},					    
	{ (word)'_', (word)'_', NULL, K_UNDERSCORE, false					},					    
			    													   
	{ 0x60, 0x60, NULL, K_LEFT_SINGLE_QUOTE, false						},
	{ (word)'A', (word)'a', NULL, K_LOW_A, false						},           
	{ (word)'B', (word)'b', NULL, K_LOW_B, false						},           
	{ (word)'C', (word)'c', NULL, K_LOW_C, false						},           
	{ (word)'D', (word)'d', NULL, K_LOW_D, false						},           
	{ (word)'E', (word)'e', NULL, K_LOW_E, false						},           
	{ (word)'F', (word)'f', NULL, K_LOW_F, false						},           
	{ (word)'G', (word)'g', NULL, K_LOW_G, false						},           
	{ (word)'H', (word)'h', NULL, K_LOW_H, false						},           
	{ (word)'I', (word)'i', NULL, K_LOW_I, false						},           
	{ (word)'J', (word)'j', NULL, K_LOW_J, false						},           
	{ (word)'K', (word)'k', NULL, K_LOW_K, false						},           
	{ (word)'L', (word)'l', NULL, K_LOW_L, false						},           
	{ (word)'M', (word)'m', NULL, K_LOW_M, false						},           
	{ (word)'N', (word)'n', NULL, K_LOW_N, false						},           
	{ (word)'O', (word)'o', NULL, K_LOW_O, false						},           
	{ (word)'P', (word)'p', NULL, K_LOW_P, false						},           
	{ (word)'Q', (word)'q', NULL, K_LOW_Q, false						},           
	{ (word)'R', (word)'r', NULL, K_LOW_R, false						},           
	{ (word)'S', (word)'s', NULL, K_LOW_S, false						},           
	{ (word)'T', (word)'t', NULL, K_LOW_T, false						},           
	{ (word)'U', (word)'u', NULL, K_LOW_U, false						},           
	{ (word)'V', (word)'v', NULL, K_LOW_V, false						},           
	{ (word)'W', (word)'w', NULL, K_LOW_W, false						},           
	{ (word)'X', (word)'x', NULL, K_LOW_X, false						},           
	{ (word)'Y', (word)'y', NULL, K_LOW_Y, false						},           
	{ (word)'Z', (word)'z', NULL, K_LOW_Z, false						},           
	{ (word)'{', (word)'{', NULL, K_OPEN_BRACE, false					},
	{ (word)'|', (word)'|', NULL, K_BAR, false							},
	{ (word)'}', (word)'}', NULL, K_CLOSE_BRACE, false					},
	{ (word)'~', (word)'~', NULL, K_TILDE, false						},
	{ 0x7f, 0x7f, "DEL", K_DEL, false									},
			    													               
	{ 0x80, 0x80, "EURO", NULL, false  									},
	{ 0x81, 0x81, "SHIFT", K_SHIFT, false								},
	{ 0x82, 0x82, "CTRL", K_CTRL, false									},
	{ 0x83, 0x83, "ALT", K_ALT, false									},
	{ 0x84, 0x84, "F5", K_F5, true										},
	{ 0x85, 0x85, "F6", K_F6, true										},
	{ 0x86, 0x86, "F7", K_F7, true										},
	{ 0x87, 0x87, "F8", K_F8, true										},
	{ 0x88, 0x88, "CIRCUMFLEX", NULL, false								},
	{ 0x89, 0x89, "MWHEELUP", K_MWHEELUP, false							},
	{ 0x8a, 0x9a, NULL, NULL, false										},	// ******
	{ 0x8b, 0x8b, "MWHEELDOWN", K_MWHEELDOWN, false						},
	{ 0x8c, 0x9c, NULL, NULL, false										},	// ******
	{ 0x8d, 0x8d, "MOUSE1", K_MOUSE1, false								},
	{ 0x8e, 0x8e, "MOUSE2", K_MOUSE2, false								},
	{ 0x8f, 0x8f, "INS", K_INS, false									},
	{ 0x90, 0x90, "HOME", K_HOME, false									},
	{ 0x91, 0x91, "PGUP", K_PGUP, false									},
	{ 0x92, 0x92, NULL, NULL, false										},
	{ 0x93, 0x93, NULL, NULL, false										},
	{ 0x94, 0x94, NULL, NULL, false										},
	{ 0x95, 0x95, "F9", K_F9, true										},
	{ 0x96, 0x96, "F10", K_F10, true									},
	{ 0x97, 0x97, "F11", K_F11, true									},
	{ 0x98, 0x98, "F12", K_F12, true									},
	{ 0x99, 0x99, NULL, NULL, false										},
	{ 0x8a, 0x9a, NULL, NULL, false										},	// ******
	{ 0x9b, 0x9b, "SHIFT_ENTER", K_ENTER, false							},
	{ 0x8c, 0x9c, NULL, NULL, false										},	// ******
	{ 0x9d, 0x9d, "END", K_END, false									},
	{ 0x9e, 0x9e, "PGDN", K_PGDN, false									},
	{ 0x9f, 0xff, NULL, NULL, false										},	// ******
			    													
	//{ 0xa0, 0,	  "SHIFT_SPACE", K_SPACE, false							},
	//{ 0xa1, 0xa1, NULL, K_EXCLAMDOWN, false								},	// upside down '!' - undisplayable
	//{ (word)(byte)'¢', (word)(byte)'¢', NULL, K_CENT, false	  			},
	//{ (word)(byte)'£', (word)(byte)'£', NULL, K_POUND, false	  		},
	//{ 0xa4, 0,    "SHIFT_KP_ENTER", K_KP_ENTER, false					},
	//{ (word)(byte)'¥', (word)(byte)'¥', NULL, K_YEN, false		  		},
	{ 0xa6, 0xa6, "MOUSE3", K_MOUSE3, false								},
	{ 0xa7, 0xa7, "MOUSE4", K_MOUSE4, false								},
	{ 0xa8, 0xa8, "MOUSE5", K_MOUSE5, false								},
	//{ (word)(byte)'©', (word)(byte)'©', NULL, K_COPYRIGHT, false 		},
	{ 0xaa, 0xaa, "UPARROW", K_UPARROW, false							},
	{ 0xab, 0xab, "DOWNARROW", K_DOWNARROW, false						},
	{ 0xac, 0xac, "LEFTARROW", K_LEFTARROW, false						},
	{ 0xad, 0xad, "RIGHTARROW", K_RIGHTARROW, false						},
	/*{ (word)(byte)'®', (word)(byte)'®', NULL, K_REGISTERED, false		},
	{ 0xaf, 0,	  NULL, K_UNDEFINED_7, false							},
	{ 0xb0, 0,	  NULL, K_UNDEFINED_8, false							},
	{ 0xb1, 0,	  NULL, K_UNDEFINED_9, false							},
	{ 0xb2, 0,	  NULL, K_UNDEFINED_10, false							},
	{ 0xb3, 0,	  NULL, K_UNDEFINED_11, false							},
	{ 0xb4, 0,	  NULL, K_UNDEFINED_12, false							},
	{ 0xb5, 0,	  NULL, K_UNDEFINED_13, false							},
	{ 0xb6, 0,	  NULL, K_UNDEFINED_14, false							},
	{ 0xb7, 0,	  NULL, K_UNDEFINED_15, false							},
	{ 0xb8, 0,	  NULL, K_UNDEFINED_16, false							},
	{ 0xb9, 0,	  NULL, K_UNDEFINED_17, false							},
	{ 0xba, 0,	  NULL, K_UNDEFINED_18, false							},
	{ 0xbb, 0,	  NULL, K_UNDEFINED_19, false							},
	{ 0xbc, 0,	  NULL, K_UNDEFINED_20, false							},
	{ 0xbd, 0,	  NULL, K_UNDEFINED_21, false							},
	{ 0xbe, 0,	  NULL, K_UNDEFINED_22, false							},
	{ (word)(byte)'¿', (word)(byte)'¿', NULL, K_QUESTION_DOWN, false	},
			    		                       
	{ (word)(byte)'À', (word)(byte)'à', NULL, K_CAP_AGRAVE, false		},
	{ (word)(byte)'Á', (word)(byte)'á', NULL, K_CAP_AACUTE, false		},
	{ (word)(byte)'Â', (word)(byte)'â', NULL, K_CAP_ACIRCUMFLEX, false	},
	{ (word)(byte)'Ã', (word)(byte)'ã', NULL, K_CAP_ATILDE, false		},
	{ (word)(byte)'Ä', (word)(byte)'ä', NULL, K_CAP_ADIERESIS, false	},
	{ (word)(byte)'Å', (word)(byte)'å', NULL, K_CAP_ARING, false		},
	{ (word)(byte)'Æ', (word)(byte)'æ', NULL, K_CAP_AE, false			},
	{ (word)(byte)'Ç', (word)(byte)'ç', NULL, K_CAP_CCEDILLA, false		},
	{ (word)(byte)'È', (word)(byte)'è', NULL, K_CAP_EGRAVE, false		},
	{ (word)(byte)'É', (word)(byte)'é', NULL, K_CAP_EACUTE, false		},
	{ (word)(byte)'Ê', (word)(byte)'ê', NULL, K_CAP_ECIRCUMFLEX, false	},
	{ (word)(byte)'Ë', (word)(byte)'ë', NULL, K_CAP_EDIERESIS, false	},
	{ (word)(byte)'Ì', (word)(byte)'ì', NULL, K_CAP_IGRAVE, false		},
	{ (word)(byte)'Í', (word)(byte)'í', NULL, K_CAP_IACUTE, false		},
	{ (word)(byte)'Î', (word)(byte)'î', NULL, K_CAP_ICIRCUMFLEX, false	},
	{ (word)(byte)'Ï', (word)(byte)'ï', NULL, K_CAP_IDIERESIS, false	},
	{ (word)(byte)'Ð', (word)(byte)'ð', NULL, K_CAP_ETH, false			},
	{ (word)(byte)'Ñ', (word)(byte)'ñ', NULL, K_CAP_NTILDE, false		},
	{ (word)(byte)'Ò', (word)(byte)'ò', NULL, K_CAP_OGRAVE, false		},
	{ (word)(byte)'Ó', (word)(byte)'ó', NULL, K_CAP_OACUTE, false		},
	{ (word)(byte)'Ô', (word)(byte)'ô', NULL, K_CAP_OCIRCUMFLEX, false	},
	{ (word)(byte)'Õ', (word)(byte)'õ', NULL, K_CAP_OTILDE, false		},
	{ (word)(byte)'Ö', (word)(byte)'ö', NULL, K_CAP_ODIERESIS, false	},
	{ (word)(byte)'×', (word)(byte)'×', "KP_STAR", K_MULTIPLY, false 	},
	{ (word)(byte)'Ø', (word)(byte)'ø', NULL, K_CAP_OSLASH, false		},  
	{ (word)(byte)'Ù', (word)(byte)'ù', NULL, K_CAP_UGRAVE, false		},  
	{ (word)(byte)'Ú', (word)(byte)'ú', NULL, K_CAP_UACUTE, false		},  
	{ (word)(byte)'Û', (word)(byte)'û', NULL, K_CAP_UCIRCUMFLEX, false	},  
	{ (word)(byte)'Ü', (word)(byte)'ü', NULL, K_CAP_UDIERESIS, false	},  
	{ (word)(byte)'Ý', (word)(byte)'ý', NULL, K_CAP_YACUTE, false		},  
	{ (word)(byte)'Þ', (word)(byte)'þ', NULL, K_CAP_THORN, false		},  
	{ (word)(byte)'ß', (word)(byte)'ß', NULL, K_GERMANDBLS, false 		},
			    		                       
	{ (word)(byte)'À', (word)(byte)'à', NULL, K_LOW_AGRAVE, false		},		 
	{ (word)(byte)'Á', (word)(byte)'á', NULL, K_LOW_AACUTE, false		},		 
	{ (word)(byte)'Â', (word)(byte)'â', NULL, K_LOW_ACIRCUMFLEX, false	},		 
	{ (word)(byte)'Ã', (word)(byte)'ã', NULL, K_LOW_ATILDE, false		},		 
	{ (word)(byte)'Ä', (word)(byte)'ä', NULL, K_LOW_ADIERESIS, false	},		 
	{ (word)(byte)'Å', (word)(byte)'å', NULL, K_LOW_ARING, false		},		 
	{ (word)(byte)'Æ', (word)(byte)'æ', NULL, K_LOW_AE, false			},		 
	{ (word)(byte)'Ç', (word)(byte)'ç', NULL, K_LOW_CCEDILLA, false		},		 
	{ (word)(byte)'È', (word)(byte)'è', NULL, K_LOW_EGRAVE, false		},		 
	{ (word)(byte)'É', (word)(byte)'é', NULL, K_LOW_EACUTE, false		},		 
	{ (word)(byte)'Ê', (word)(byte)'ê', NULL, K_LOW_ECIRCUMFLEX, false	},		 
	{ (word)(byte)'Ë', (word)(byte)'ë', NULL, K_LOW_EDIERESIS, false	},		 
	{ (word)(byte)'Ì', (word)(byte)'ì', NULL, K_LOW_IGRAVE, false		},		 
	{ (word)(byte)'Í', (word)(byte)'í', NULL, K_LOW_IACUTE, false		},		 
	{ (word)(byte)'Î', (word)(byte)'î', NULL, K_LOW_ICIRCUMFLEX, false	},		 
	{ (word)(byte)'Ï', (word)(byte)'ï', NULL, K_LOW_IDIERESIS, false	},		 
	{ (word)(byte)'Ð', (word)(byte)'ð', NULL, K_LOW_ETH, false			},		 
	{ (word)(byte)'Ñ', (word)(byte)'ñ', NULL, K_LOW_NTILDE, false		},		 
	{ (word)(byte)'Ò', (word)(byte)'ò', NULL, K_LOW_OGRAVE, false		},		 
	{ (word)(byte)'Ó', (word)(byte)'ó', NULL, K_LOW_OACUTE, false		},		 
	{ (word)(byte)'Ô', (word)(byte)'ô', NULL, K_LOW_OCIRCUMFLEX, false	},		 
	{ (word)(byte)'Õ', (word)(byte)'õ', NULL, K_LOW_OTILDE, false		},		 
	{ (word)(byte)'Ö', (word)(byte)'ö', NULL, K_LOW_ODIERESIS, false	},		 
	{ (word)(byte)'÷', (word)(byte)'÷', "KP_SLASH", K_DIVIDE, false 	},		 
	{ (word)(byte)'Ø', (word)(byte)'ø', NULL, K_LOW_OSLASH, false		},		 
	{ (word)(byte)'Ù', (word)(byte)'ù', NULL, K_LOW_UGRAVE, false		},		 
	{ (word)(byte)'Ú', (word)(byte)'ú', NULL, K_LOW_UACUTE, false		},		 
	{ (word)(byte)'Û', (word)(byte)'û', NULL, K_LOW_UCIRCUMFLEX, false	},		 
	{ (word)(byte)'Ü', (word)(byte)'ü', NULL, K_LOW_UDIERESIS, false	},		 
	{ (word)(byte)'Ý', (word)(byte)'ý', NULL, K_LOW_YACUTE, false		},		 
	{ (word)(byte)'Þ', (word)(byte)'þ', NULL, K_LOW_THORN, false		},		 
	{ 0x9f, 0xff, NULL, K_LOW_YDIERESIS, false							},*/	// *******
																		
	{ 0x100, 0x100, "JOY0", NULL, false									},
	{ 0x101, 0x101, "JOY1", K_JOY1, false								},
	{ 0x102, 0x102, "JOY2", K_JOY2, false								},
	{ 0x103, 0x103, "JOY3", K_JOY3, false								},
	{ 0x104, 0x104, "JOY4", K_JOY4, false								},
	{ 0x105, 0x105, "JOY5", K_JOY5, false								},
	{ 0x106, 0x106, "JOY6", K_JOY6, false								},
	{ 0x107, 0x107, "JOY7", K_JOY7, false								},
	{ 0x108, 0x108, "JOY8", K_JOY8, false								},
	{ 0x109, 0x109, "JOY9", K_JOY9, false								},
	{ 0x10a, 0x10a, "JOY10", K_JOY10, false								},
	{ 0x10b, 0x10b, "JOY11", K_JOY11, false								},
	{ 0x10c, 0x10c, "JOY12", K_JOY12, false								},
	{ 0x10d, 0x10d, "JOY13", K_JOY13, false								},
	{ 0x10e, 0x10e, "JOY14", K_JOY14, false								},
	{ 0x10f, 0x10f, "JOY15", K_JOY15, false								},
	{ 0x110, 0x110, "JOY16", K_JOY16, false								},
	{ 0x111, 0x111, "JOY17", K_JOY17, false								},
	{ 0x112, 0x112, "JOY18", K_JOY18, false								},
	{ 0x113, 0x113, "JOY19", K_JOY19, false								},
	{ 0x114, 0x114, "JOY20", K_JOY20, false								},
	{ 0x115, 0x115, "JOY21", K_JOY21, false								},
	{ 0x116, 0x116, "JOY22", K_JOY22, false								},
	{ 0x117, 0x117, "JOY23", K_JOY23, false								},
	{ 0x118, 0x118, "JOY24", K_JOY24, false								},
	{ 0x119, 0x119, "JOY25", K_JOY25, false								},
	{ 0x11a, 0x11a, "JOY26", K_JOY26, false								},
	{ 0x11b, 0x11b, "JOY27", K_JOY27, false								},
	{ 0x11c, 0x11c, "JOY28", K_JOY28, false								},
	{ 0x11d, 0x11d, "JOY29", K_JOY29, false								},
	{ 0x11e, 0x11e, "JOY30", K_JOY30, false								},
	{ 0x11f, 0x11f, "JOY31", K_JOY31, false								},
		    	    												
	{ 0x120, 0x120, "AUX0", NULL, false									},
	{ 0x121, 0x121, "AUX1", K_AUX1, false								},
	{ 0x122, 0x122, "AUX2", K_AUX2, false								},
	{ 0x123, 0x123, "AUX3", K_AUX3, false								},
	{ 0x124, 0x124, "AUX4", K_AUX4, false								},
	{ 0x125, 0x125, "AUX5", K_AUX5, false								},
	{ 0x126, 0x126, "AUX6", K_AUX6, false								},
	{ 0x127, 0x127, "AUX7", K_AUX7, false								},
	{ 0x128, 0x128, "AUX8", K_AUX8, false								},
	{ 0x129, 0x129, "AUX9", K_AUX9, false								},
	{ 0x12a, 0x12a, "AUX10", K_AUX10, false								},
	{ 0x12b, 0x12b, "AUX11", K_AUX11, false								},
	{ 0x12c, 0x12c, "AUX12", K_AUX12, false								},
	{ 0x12d, 0x12d, "AUX13", K_AUX13, false								},
	{ 0x12e, 0x12e, "AUX14", K_AUX14, false								},
	{ 0x12f, 0x12f, "AUX15", K_AUX15, false								},
	{ 0x130, 0x130, "AUX16", K_AUX16, false								},
	/*{ 0x131, 0x131, "AUX17", K_AUX17, false								},
	{ 0x132, 0x132, "AUX18", K_AUX18, false								},
	{ 0x133, 0x133, "AUX19", K_AUX19, false								},
	{ 0x134, 0x134, "AUX20", K_AUX20, false								},
	{ 0x135, 0x135, "AUX21", K_AUX21, false								},
	{ 0x136, 0x136, "AUX22", K_AUX22, false								},
	{ 0x137, 0x137, "AUX23", K_AUX23, false								},
	{ 0x138, 0x138, "AUX24", K_AUX24, false								},
	{ 0x139, 0x139, "AUX25", K_AUX25, false								},
	{ 0x13a, 0x13a, "AUX26", K_AUX26, false								},
	{ 0x13b, 0x13b, "AUX27", K_AUX27, false								},
	{ 0x13c, 0x13c, "AUX28", K_AUX28, false								},
	{ 0x13d, 0x13d, "AUX29", K_AUX29, false								},
	{ 0x13e, 0x13e, "AUX30", K_AUX30, false								},
	{ 0x13f, 0x13f, "AUX31", K_AUX31, false								}*/
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
	Field_VariableSizeDraw( edit, x, y, width, BIGCHAR_WIDTH, showCursor );
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

	if ( key == K_HOME || ( keynames[key].lower == 'a' && kg.keys[K_CTRL].down ) ) 
	{
		edit->cursor = 0;
		return;
	}

	if ( key == K_END || ( keynames[key].lower == 'e' && kg.keys[K_CTRL].down ) ) 
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
	if ( keynames[ key ].lower == 'l' && kg.keys[K_CTRL].down ) {
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
			if (cgvm && cl.mSharedMemory)
			{ //don't do this unless cgame is inited and shared memory is valid
				const char *buf = (kg.g_consoleField.buffer+1);
				TCGIncomingConsoleCommand *icc = (TCGIncomingConsoleCommand *)cl.mSharedMemory;

				strcpy(icc->conCommand, buf);
				
				if (VM_Call(cgvm, CG_INCOMING_CONSOLE_COMMAND))
				{ //rww - let mod authors filter client console messages so they can cut them off if they want.
					Cbuf_AddText( kg.g_consoleField.buffer+1 );	// valid command
					Cbuf_AddText ("\n");
				}
				else if (icc->conCommand[0])
				{ //the vm call says to execute this command in place
					Cbuf_AddText( icc->conCommand );
					Cbuf_AddText ("\n");
				}
			}
			else
			{ //just exec it then
				Cbuf_AddText( kg.g_consoleField.buffer+1 );	// valid command
				Cbuf_AddText ("\n");
			}
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

	if ( ( key == K_MWHEELUP && kg.keys[K_SHIFT].down ) || ( key == K_UPARROW ) || ( key == K_KP_UPARROW ) || ( ( keynames[ key ].lower == 'p' ) && kg.keys[K_CTRL].down ) ) 
	{
		if ( kg.nextHistoryLine - kg.historyLine < COMMAND_HISTORY && kg.historyLine > 0 ) 
		{
			kg.historyLine--;
		}
		kg.g_consoleField = kg.historyEditLines[ kg.historyLine % COMMAND_HISTORY ];
		return;
	}

	if ( ( key == K_MWHEELDOWN && kg.keys[K_SHIFT].down ) || ( key == K_DOWNARROW ) || ( key == K_KP_DOWNARROW ) || ( ( keynames[ key ].lower == 'n' ) && kg.keys[K_CTRL].down ) ) 
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

	return kg.keys[ keynames[keynum].upper ].down;
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
	int			i;
	
	if ( !str || !str[0] ) 
	{
		return -1;
	}
	// If single char bind, presume ascii char bind
	if ( !str[1] ) 
	{
		return keynames[ (unsigned char)str[0] ].upper;
	}

	// scan for a text match
	for ( i = 0 ; i < K_LAST_KEY ; i++ ) 
	{
		if ( keynames[i].name && !stricmp( str, keynames[i].name ) )
		{
			return keynames[i].keynum;
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

static const char *Key_KeyToName( int keynum )
{
	return keynames[keynum].name;
}


static const char *Key_KeyToAscii( int keynum )
{
	if(!keynames[keynum].lower)
	{
		return(NULL);
	}
	/*if(keynum == K_SPACE)
	{
		tinyString[0] = (char)K_SHIFT_SPACE;
	}
	else if(keynum == K_ENTER)
	{
		tinyString[0] = (char)K_SHIFT_ENTER;
	}
	else if(keynum == K_KP_ENTER)
	{
		tinyString[0] = (char)K_SHIFT_KP_ENTER;
	}*/
	else
	{
		tinyString[0] = keynames[keynum].upper;
	}
	tinyString[1] = 0;
	return tinyString;
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

// Returns the ascii code of the keynum
const char *Key_KeynumToAscii( int keynum ) 
{
	const char	*name;

	name = Key_KeynumValid(keynum);

	// check for printable ascii
	if ( !name && keynum > 0 && keynum < 256 ) 
	{
		name = Key_KeyToAscii(keynum);
	}
	// Check for name (for JOYx and AUXx buttons)
	if ( !name )
	{
		name = Key_KeyToName(keynum);
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
Key_KeynumToString

Returns a string (either a single ascii char, a K_* name, or a 0x11 hex string) for the
given keynum.
===================
*/
// Returns a console/config file friendly name for the key
const char *Key_KeynumToString( int keynum ) 
{
	const char	*name;

	name = Key_KeynumValid(keynum);

	// Check for friendly name
	if ( !name )
	{
		name = Key_KeyToName(keynum);
	}
	// check for printable ascii
	if ( !name && keynum > 0 && keynum < 256) 
	{
		name = Key_KeyToAscii(keynum);
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
	if ( kg.keys[ keynames[keynum].upper ].binding ) {
		Z_Free( kg.keys[ keynames[keynum].upper ].binding );
		kg.keys[ keynames[keynum].upper ].binding = NULL;
	}
			
	// allocate memory for new binding
	if (binding)
	{
		kg.keys[ keynames[keynum].upper ].binding = CopyString( binding );
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
			Com_Printf( "Key : %s (%s) \"%s\"\n", Key_KeynumToAscii(i), Key_KeynumToString(i), kg.keys[i].binding );
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
	kg.keys[ keynames[key].upper ].down = down;
	if (down)
	{
		kg.keys[ keynames[key].upper ].repeats++;
		if ( kg.keys[ keynames[key].upper ].repeats == 1 && key != K_KP_NUMLOCK && key != K_CAPSLOCK )
		{
			kg.anykeydown = qtrue;
			kg.keyDownCount++;
		}
	}
	else
	{
		kg.keys[ keynames[key].upper ].repeats = 0;
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
	if ( down && /*( key < 128 || key == K_MOUSE1 ) && */
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
		kb = kg.keys[ keynames[key].upper ].binding;

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
		kb = kg.keys[ keynames[key].upper ].binding;
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
				if (cgvm && cl.mSharedMemory)
				{ //don't do this unless cgame is inited and shared memory is valid
					TCGIncomingConsoleCommand *icc = (TCGIncomingConsoleCommand *)cl.mSharedMemory;

					strcpy(icc->conCommand, kb);
					
					if (VM_Call(cgvm, CG_INCOMING_CONSOLE_COMMAND))
					{ //rww - let mod authors filter client console messages so they can cut them off if they want.
						Cbuf_AddText (kb);
						Cbuf_AddText ("\n");
					}
					else if (icc->conCommand[0])
					{ //the vm call says to execute this command in place
						Cbuf_AddText( icc->conCommand );
						Cbuf_AddText ("\n");
					}
				}
				else
				{ //otherwise just add it
					Cbuf_AddText (kb);
					Cbuf_AddText ("\n");
				}
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

