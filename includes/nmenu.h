/*
	Copyright (c) 2025 Natalia Pujol Cremades
	info@abitwitches.com

	See LICENSE file.
*/
#pragma once

#include <stdint.h>
#include <stdbool.h>

#define VERSIONAPP			"1.0.3"

#define MAX_FILE_PATH		64
#define MAX_MENU_ENTRIES	50
#define MAX_MENU_TEXT		81
#define MAX_COLOR			8

typedef struct {
	bool     enabled;						// Entry enabled
	uint8_t  posx;							// X position
	uint8_t  posy;							// Y position
	char     text[MAX_MENU_TEXT];			// Text
	char     exec[MAX_FILE_PATH];			// Executable instruction
} MENU_ENTRY_t;

typedef struct {
	// Background
	char bgColor[MAX_COLOR];				// Background color
	char bgFileANSI[MAX_FILE_PATH];			// Background ANSI file
	char bgFileSC7[MAX_FILE_PATH];			// Background SC7 file

	// Menu settings
	char musicLVGM[MAX_FILE_PATH];			// Music file LVGM
	char menuBgColor[MAX_COLOR];			// Menu background color
	char menuFgColor[MAX_COLOR];			// Menu foreground color
	char selectedBgColor[MAX_COLOR];		// Selected background color
	char selectedFgColor[MAX_COLOR];		// Selected foreground color

	// Menu entries
	MENU_ENTRY_t *entries;	// Menu entries
} MENU_t;




// General ASCII codes

#define ASCII_BELL				"\x07"				// Terminal bell
#define ASCII_BS				"\x08"				// Backspace
#define ASCII_HT				"\x09"				// Horizontal TAB
#define ASCII_LF				"\x0a"				// Linefeed (newline)
#define ASCII_FF				"\x0c"				// Formfeed (also: New page)
#define ASCII_CR				"\x0d"				// Carriage return
#define ASCII_ESC				"\x1b"				// ESCape character
#define ASCII_DEL				"\x7f"				// Delete character


// VT52 escape codes
// From: https://espterm.github.io/docs/VT100%20escape%20codes.html

#define ANSI_CURSORUP			"\033A"				// Move cursor up one line
#define ANSI_CURSORDN			"\033B"				// Move cursor down one line
#define ANSI_CURSORRT			"\033C"				// Move cursor right one char
#define ANSI_CURSORLF			"\033D"				// Move cursor left one char
//#define ANSI_CURSORHOME		"\033H"				// Move cursor to upper left corner
#define ANSI_CLREOL				"\033K"				// Erase to end of current line


// ANSI escape codes
// From: https://espterm.github.io/docs/VT100%20escape%20codes.html
// From: https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797

// Cursor control
#define ANSI_CURSORHOME			"\033[H"			// Move cursor to upper left corner
#define ANSI_CURSORHOME2		"\033[;H"			// Move cursor to upper left corner
#define ANSI_HVHOME				"\033[f"			// Move cursor to upper left corner
#define ANSI_HVHOME2			"\033[;f"			// Move cursor to upper left corner

#define ANSI_CURSORPOS(x,y)		"\033["#y";"#x"H"	// CUP: Move cursor to screen location v,h
#define ANSI_HVPOS(x,y)			"\033["#y";"#x"f"	// CUP: Move cursor to screen location v,h

#define ANSI_CURSORNUP(n)		"\033["#n"A"		// CUU: Move cursor up n lines
#define ANSI_CURSORNDN(n)		"\033["#n"B"		// CUD: Move cursor down n lines
#define ANSI_CURSORNRT(n)		"\033["#n"C"		// CUF: Move cursor right n lines
#define ANSI_CURSORNLF(n)		"\033["#n"D"		// CUB: Move cursor left n lines

#define ANSI_CURSORCOL(n)		"\033["#n"G"		// Moves cursor to column n

#define ANSI_SAVECURSOR			"\0337"				// DECSC: Save cursor position and attributes
#define ANSI_RESTCURSOR			"\0338"				// DECSC: Restore cursor position and attributes
#define ANSI_SAVECURSOR2		"\033[s"			// SCO: Save cursor position
#define ANSI_RESTCURSOR2		"\033[u"			// SCO: Restores the cursor to the last saved position

#define ANSI_CURSOROFF			"\033x5"			// Make cursor invisible
#define ANSI_CURSORON			"\033y5"			// Make cursor visible


// Erase functions
#define ANSI_CLREOS				"\033[J"			// ED0: Clear screen from cursor down
#define ANSI_CLREOS2			"\033[0J"			// ED0: Clear screen from cursor down
#define ANSI_CLRBOS				"\033[1J"			// ED1: Clear screen from cursor up
#define ANSI_CLRSCR				"\033[2J"			// ED2: Clear entire screen and move cursor to home

#define ANSI_CLREOL2			"\033[K"			// EL0: Clear line from cursor to right
#define ANSI_CLREOL3			"\033[0K"			// EL0: Clear line from cursor to right
#define ANSI_CLRBOL				"\033[1K"			// EL1: Clear line from cursor to left
#define ANSI_CLRLINE			"\033[2K"			// EL2: Clear entire line

// Text styles
#define ANSI_RESET				"\033[0m"			// SGR0: Reset all modes (styles and colors)
#define ANSI_BOLD				"\033[1m"			// SGR1: Turn bold mode on
//#define ANSI_LOWINT			"\033[2m"			// SGR2: Turn low intensity mode on
//#define ANSI_ITALIC			"\033[3m"			// SGR3: Turn Italic mode on
//#define ANSI_UNDERLINE		"\033[4m"			// SGR4: Turn underline mode on
//#define ANSI_BLINK			"\033[5m"			// SGR5: Turn blinking mode on
#define ANSI_INVERSE			"\033[7m"			// SGR7: Turn inverse video on
#define ANSI_INVERSEOFF			"\033[27m"			// SGR7: Turn inverse video off
#define ANSI_HIDDEN				"\033[8m"			// SGR8: Turn invisible text mode on
#define ANSI_HIDDENOFF			"\033[28m"			// SGR8: Turn invisible text mode off
//#define ANSI_STRIKETHROUGH	"\033[9m"			// SGR9: Turn strikethrough text mode on

// Color codes
#define ANSI_COLOR(c)			"\033["#c"m"		// Set color
#define ANSI_FRCOL(c)			"\033[3"#c"m"		// Set foreground color
#define ANSI_BGCOL(c)			"\033[4"#c"m"		// Set background color
#define ANSI_FRCOL1(c)			"\033[1;3"#c"m"		// Set foreground color
#define ANSI_BGCOL1(c)			"\033[1;4"#c"m"		// Set background color


// Not supported in MSX2ANSI

//#define ANSI_INDEX			"\033D"				// IND: Move/scroll window up one line
//#define ANSI_REVINDEX			"\033M"				// RI:  Move/scroll window down one line
//#define ANSI_NEXTLINE			"\033E"				// NEL: Move to next line

//#define ANSI_TABSET			"\033H"				// HTS: Set a tab at the current column
//#define ANSI_TABCLR			"\033[g"			// TBC: Clear a tab at the current column
//#define ANSI_TABCLR2			"\033[0g"			// TBC: Clear a tab at the current column
//#define ANSI_TABCLRALL		"\033[3g"			// TBC: Clear all tabs

//#define ANSI_DHTOP			"\033#3"			// DECDHL: Double-height letters, top half    
//#define ANSI_DHBOT			"\033#4"			// DECDHL: Double-height letters, bottom half 
//#define ANSI_SWSH				"\033#5"			// DECSWL: Single width, single height letters
//#define ANSI_DWSH				"\033#6"			// DECDWL: Double width, single height letters

//#define ANSI_DEVSTAT			"\0335n"			// DSR: Device status report
//#define ANSI_TERMOK			"\0330n"			// DSR: Response: terminal is OK
//#define ANSI_TERMNOK			"\0333n"			// DSR: Response: terminal is not OK

//#define ANSI_GETCURSOR		"\0336n"			// DSR: Get cursor position
//#define ANSI_CURSORPOS		"\033"#v";"#h"R"	// CPR: Response: cursor is at v,h

//#define ANSI_IDENT			"\033[c"			// DA: Identify what terminal type
//#define ANSI_IDENT2			"\033[0c"			// DA: Identify what terminal type (another)
//#define ANSI_GETTYPE			"\033[?1;"#n"0c"	// DA: Response: terminal type code n

//#define ANSI_RESET			"\033c"				// RIS: Reset terminal to initial state

//#define ANSI_ALIGN			"\033#8"			// DECALN: Screen alignment display
//#define ANSI_TESTPU			"\033[2;1y"			// DECTST: Confidence power up test
//#define ANSI_TESTLB			"\033[2;2y"			// DECTST: Confidence loopback test
//#define ANSI_TESTPUREP		"\033[2;9y"			// DECTST: Repeat power up test
//#define ANSI_TESTLBREP		"\033[2;10y"		// DECTST: Repeat loopback test

//#define ANSI_LEDSOFF			"\033[0q"			// DECLL0: Turn off all four leds
//#define ANSI_LED1				"\033[1q"			// DECLL1: Turn on LED #1
//#define ANSI_LED2				"\033[2q"			// DECLL2: Turn on LED #2
//#define ANSI_LED3				"\033[3q"			// DECLL3: Turn on LED #3
//#define ANSI_LED4				"\033[4q"			// DECLL4: Turn on LED #4

// Screen modes
//#define ANSI_MODE40x25M		"\033=0h"			// 40 x 25 monochrome (text)
//#define ANSI_MODE40x25C		"\033=1h"			// 40 x 25 color (text)
//#define ANSI_MODE80x25M		"\033=2h"			// 80 x 25 monochrome (text)
//#define ANSI_MODE80x25C		"\033=3h"			// 80 x 25 color (text)

// Common private modes
//#define ANSI_CURSORON			"\033?25l"			// Make cursor invisible
//#define ANSI_CURSOROFF		"\033?25h"			// Make cursor visible
//#define ANSI_RESTSCREEN		"\033?47l"			// Restore screen
//#define ANSI_SAVESCREEN		"\033?47h"			// Save screen
