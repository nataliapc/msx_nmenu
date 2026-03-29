/*
	Copyright (c) 2025 Natalia Pujol Cremades
	info@abitwitches.com

	See LICENSE file.
*/
#pragma opt_code_size
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "msx_const.h"
#include "lvgm_player.h"
#include "dos.h"
#include "heap.h"
#include "utils.h"
#include "interrupt.h"
#include "vdp.h"
#include "msx2ansi.h"
#include "ini.h"
#include "nmenu.h"


// ========================================================
static uint8_t msxVersionROM;
static uint8_t kanjiMode;
static uint8_t originalLINL40;
static uint8_t originalSCRMOD;
static uint8_t originalFORCLR;
static uint8_t originalBAKCLR;
static uint8_t originalBDRCLR;

const char *SECTION_BACKGROUND = "Background";
const char *SECTION_SETTINGS   = "Settings";
const char *SECTION_OPTIONS    = "Options";

const char initScreen[] = ANSI_COLOR(%s) ANSI_CLRSCR ANSI_CURSORHOME;
const char entryPattern[] = ANSI_RESET ANSI_CURSORPOS(%u,%u)ANSI_COLOR(%s)ANSI_COLOR(%s)"%s";

MENU_t *menu;
uint8_t selected = 0;
uint8_t lastSel = 0xff;

char *buff;
void *heapBackup;


// ========================================================
// Function declarations

bool SCC_Initialize();
void MSXMusic_Initialize();
void MSXAudio_Initialize();

void restoreScreen();


// ========================================================
static void abortRoutine()
{
	restoreScreen();
	dos2_exit(1);
}

static void checkPlatformSystem()
{
	// Check MSX2 ROM or higher
	msxVersionROM = getRomByte(MSXVER);
	if (!msxVersionROM) {
		die("MSX1 not supported!");
	}

	// Check MSX-DOS 2 or higher
	if (dosVersion() < VER_MSXDOS2x) {
		die("MSX-DOS 2.x or higher required!");
	}

	// Set abort exit routine
	dos2_setAbortRoutine((void*)abortRoutine);

	// Initialize interrupt hook & Music
	interrupt_init();
	SCC_Initialize();
	MSXMusic_Initialize();
	MSXAudio_Initialize();

	// Backup original values
	originalLINL40 = varLINL40;
	originalSCRMOD = varSCRMOD;
	originalFORCLR = varFORCLR;
	originalBAKCLR = varBAKCLR;
	originalBDRCLR = varBDRCLR;
	kanjiMode = (detectKanjiDriver() ? getKanjiMode() : MODE_ANK);
}

void interrupt_hook(void)
{
		//Play music
		LVGM_Decode();
}

void stopMusic()
{
	// Stop music
	LVGM_Stop();
	ASM_EI; ASM_HALT;
	LVGM_Stop();
}

// ========================================================
uint8_t last_posx = 0;
uint8_t last_posy = 0;
uint8_t index = 0;

static int handler(const char* section, const char* name, const char* value)
{
	#define MATCH_SECTION(s) strcmp(section, s) == 0
	#define MATCH_NAME(n) strcmp(name, n) == 0
	#define MATCH(s, n) MATCH_SECTION(s) && MATCH_NAME(n)

	if (MATCH(SECTION_BACKGROUND, "bg.color")) {
		strncpy(menu->bgColor, value, MAX_COLOR-1);
	} else
	if (MATCH(SECTION_BACKGROUND, "bg.file.ansi")) {
		strncpy(menu->bgFileANSI, value, MAX_FILE_PATH-1);
	} else
	if (MATCH(SECTION_BACKGROUND, "bg.file.sc7")) {
		strncpy(menu->bgFileSC7, value, MAX_FILE_PATH-1);
	} else
	if (MATCH(SECTION_SETTINGS, "music.lvgm")) {
		strncpy(menu->musicLVGM, value, MAX_FILE_PATH-1);
	} else
	if (MATCH(SECTION_SETTINGS, "option.bg.color")) {
		strncpy(menu->menuBgColor, value, MAX_COLOR-1);
	} else
	if (MATCH(SECTION_SETTINGS, "option.fr.color")) {
		strncpy(menu->menuFgColor, value, MAX_COLOR-1);
	} else
	if (MATCH(SECTION_SETTINGS, "selected.bg.color")) {
		strncpy(menu->selectedBgColor, value, MAX_COLOR-1);
	} else
	if (MATCH(SECTION_SETTINGS, "selected.fr.color")) {
		strncpy(menu->selectedFgColor, value, MAX_COLOR-1);
	} else
	if (MATCH_SECTION(SECTION_OPTIONS)) {
		if (index >= 0 && index < MAX_MENU_ENTRIES) {
			MENU_ENTRY_t *entry = &menu->entries[index];
			if (!entry->enabled) {
				entry->posx = last_posx;
				entry->posy = ++last_posy;
			}
			if (MATCH_NAME("posx")) {
				entry->enabled = true;
				last_posx = entry->posx = atoi(value);
			} else
			if (MATCH_NAME("posy")) {
				entry->enabled = true;
				last_posy = entry->posy = atoi(value);
			} else
			if (MATCH_NAME("text")) {
				entry->enabled = true;
				strcpy(entry->text, value);
			} else
			if (MATCH_NAME("exec")) {
				strncpy(entry->exec, dos2_strupr(value), MAX_FILE_PATH-1);
			} else
			if (MATCH_NAME("next")) {
				index++;
			}
		}
	}
/*
sprintf(buff, "Section: %s, Name: %s, Value: %s", section, name, value);
cputs(buff);
*/
	return 1;
}


// ========================================================
void entry_print(MENU_ENTRY_t *entry, bool selected)
{
	if (selected) {
		csprintf(buff, entryPattern,
			entry->posy, entry->posx, menu->selectedBgColor, menu->selectedFgColor, entry->text);
	} else {
		csprintf(buff, entryPattern,
			entry->posy, entry->posx, menu->menuBgColor, menu->menuFgColor, entry->text);
	}
	ASM_EI; ASM_HALT;
	AnsiPrint(buff);
}

void menu_show()
{
	void *palette = NULL;

	// Ensure VDP is ready before operations
	waitVDPready();
	ASM_EI; ASM_HALT;
	
	copyToPage1();
	waitVDPready();
	ASM_EI; ASM_HALT;
	
	setVPage(1);
	waitVDPready();
	ASM_EI; ASM_HALT;
	
	clearSC7();
	waitVDPready();
	ASM_EI; ASM_HALT;

	// Set backgrounds
	csprintf(buff, initScreen, menu->bgColor);
	AnsiPrint(buff);
	if (menu->bgFileSC7[0] && dos2_fileexists(menu->bgFileSC7)) {
		// Load background SC7 file
		palette = bloads(menu->bgFileSC7);
		waitVDPready();
		ASM_EI; ASM_HALT;
	}
	if (menu->bgFileANSI[0] && dos2_fileexists(menu->bgFileANSI)) {
		// Load background ANSI file
		AnsiPrint(ANSI_RESET ANSI_CURSORHOME);
		uint16_t size = dos2_filesize(menu->bgFileANSI);
		char *ansiBuffer = malloc(size + 1);
		loadFile(menu->bgFileANSI, ansiBuffer, size);
		AnsiPrint(ansiBuffer);
		free(size);
	}

	// Print full menu entries
	MENU_ENTRY_t *entry = menu->entries;

	for (uint8_t i = 0; i < MAX_MENU_ENTRIES; i++) {
		if (entry->text[0]) {
			entry_print(entry, false);
		}
		entry++;
	}

	// Check if background music file exists
	if (menu->musicLVGM[0]) {
		if (dos2_fileexists(menu->musicLVGM)) {
			// Load background music
			uint16_t lvgmSize = dos2_filesize(menu->musicLVGM);
			void *lvgm = malloc(lvgmSize);
			loadFile(menu->musicLVGM, lvgm, lvgmSize);
			LVGM_Play(lvgm, true);
/*
if (LVGM_IncludePSG()) AnsiPrint(ANSI_CURSORPOS(0,22)ANSI_COLOR(1;37;40)" LVGM_IncludePSG... "ANSI_RESET);
if (LVGM_IncludeOPLL()) AnsiPrint(ANSI_CURSORPOS(0,23)ANSI_COLOR(1;37;40)" LVGM_IncludeOPLL... "ANSI_RESET);
if (LVGM_IncludeOPL()) AnsiPrint(ANSI_CURSORPOS(0,24)ANSI_COLOR(1;37;40)" LVGM_IncludeOPL1... "ANSI_RESET);
if (LVGM_IncludeSCC()) AnsiPrint(ANSI_CURSORPOS(0,25)ANSI_COLOR(1;37;40)" LVGM_IncludeSCC... "ANSI_RESET);
*/
		} else {
			AnsiPrint(ANSI_CURSORPOS(0,26)ANSI_COLOR(1;33;41)" No music file found! "ANSI_RESET);
		}
	}

	waitVDPready();
	ASM_EI; ASM_HALT;
	
	setVPage(0);
	waitVDPready();
	ASM_EI; ASM_HALT;
	
	if (palette) {
		setPalette(palette);
		waitVDPready();
		ASM_EI; ASM_HALT;
	}
}

bool menu_init(char *iniFilename)
{
	// Check if INI file exists
	strcpy(buff, iniFilename);
	if (!dos2_fileexists(buff)) {
		return false;
	}

	//Initialize music
	stopMusic();

	// Initialize heap to start value
	heap_top = heapBackup;

	// Allocate memory for menu structure
	menu = (MENU_t*)malloc(sizeof(MENU_t));
	memset(menu, 0, sizeof(MENU_t));
	menu->entries = (MENU_ENTRY_t*)malloc(sizeof(MENU_ENTRY_t) * MAX_MENU_ENTRIES);
	memset(menu->entries, 0, sizeof(MENU_ENTRY_t) * MAX_MENU_ENTRIES);

	// Set working directory
	char *file = strrchr(buff, '\\');
	if (file) {
		*file++ = '\0';
		dos2_setCurrentDirectory(buff);
	} else {
		file = buff;
	}

	// Initialize menu data from INI file
	index = last_posx = last_posy = 0;
	ini_parse(file, handler);

	// Start menu display
	menu_show();

	// Clear pressed keys
	varPUTPNT = varGETPNT;
	
	return true;
}

void launch_exec(MENU_ENTRY_t *entry)
{
	// Launch executable
	if (entry->exec[0]) {
		// Check if entry is a .INI file
		char *dot = strrchr(entry->exec, '.');
		if (dot && !strcmp(dot, ".INI")) {
			if (dos2_fileexists(entry->exec)) {
				// Launch INI file
				menu_init(entry->exec);
				selected = 0;
				lastSel = 0xff;
			}
		} else {
			// Restore screen properly before executing
			AnsiPrint(ANSI_CURSORON);
			AnsiEndBuffer();
			AnsiFinish();
			
			// Wait for VDP to complete all operations
			ASM_EI; ASM_HALT;
			waitVDPready();
			ASM_EI; ASM_HALT;
			
			// Disable screen before major changes
			__asm
				ld   ix, #DISSCR
				BIOSCALL
			__endasm;
			
			restoreScreen();
			resetPalette();
			
			// Enable screen again
			__asm
				ld   ix, #ENASCR
				BIOSCALL
			__endasm;
			
			// Execute command
			execv(entry->exec);
		}
	}
}

bool menu_loop()
{
	// Menu loop
	bool end = false;
	uint8_t key;
	MENU_ENTRY_t *entry;

	selected = 0;
	lastSel = 0xff;

	while (!end) {
		if (selected != lastSel) {
			// Clear last selected entry
			if (lastSel < MAX_MENU_ENTRIES) {
				entry_print(&menu->entries[lastSel], false);
			}
			// Highlight selected entry
			entry = &menu->entries[selected];
			lastSel = selected;
			entry_print(entry, true);
		}
		// Wait for a pressed key
		if (kbhit()) {
			key = getchar();
			switch (key) {
				case KEY_ESC:
					end = true;
					break;
				case KEY_UP:
					if (selected > 0) {
						selected--;
						while (selected > 0 && !menu->entries[selected].enabled) {
							selected--;
						}
						if (!menu->entries[selected].enabled) {
							selected = lastSel;
						}
					}
					break;
				case KEY_DOWN:
					if (selected < MAX_MENU_ENTRIES - 1) {
						selected++;
						while (selected < MAX_MENU_ENTRIES - 1 && !menu->entries[selected].enabled) {
							selected++;
						}
						if (!menu->entries[selected].enabled) {
							selected = lastSel;
						}
					}
					break;
				case KEY_RETURN:
				case KEY_SELECT:
				case KEY_SPACE:
					stopMusic();
					launch_exec(entry);
					break;
			}
		}
	}

	return true;
}


// ========================================================
void restoreOriginalScreenMode() __naked
{
	// Restore original screen mode
	__asm
		ld   a, (_originalSCRMOD)
		or   a
		jr   nz, .screen1
		ld   ix, #INITXT				; Restore SCREEN 0
		jr   .bioscall
	.screen1:
		ld   ix, #INIT32				; Restore SCREEN 1
	.bioscall:
		JP_BIOSCALL
	__endasm;
}

void restoreScreen()
{
	// Restore interrupt hook
	stopMusic();
	interrupt_end();

	// Clear & restore original screen parameters & colors
	__asm
		ld   ix, #DISSCR				; Disable screen
		BIOSCALL
	__endasm;

	// Add small delay for VDP stability
	ASM_EI; ASM_HALT;
	
	varLINL40 = originalLINL40;
	varFORCLR = originalFORCLR;
	varBAKCLR = originalBAKCLR;
	varBDRCLR = originalBDRCLR;

	if (kanjiMode && kanjiMode != MODE_ANK) {
		// Restore kanji mode if needed
		// First ensure we're in ANK mode before switching
		if (detectKanjiDriver()) {
			setKanjiMode(MODE_ANK);
			ASM_EI; ASM_HALT;
			setKanjiMode(kanjiMode);
		}
	} else {
		// Restore original screen mode
		restoreOriginalScreenMode();
	}

	// Add delay before enabling screen
	ASM_EI; ASM_HALT;
	
	__asm
		ld   ix, #ENASCR
		BIOSCALL
	__endasm;

	// Restore abort routine
	dos2_setAbortRoutine((void*)0x0000);
}

void printHelp()
{
	// Print help message
	cputs("## NMENU v"VERSIONAPP"\n"
		  "## by NataliaPC 2025\n\n"
		  "Usage:\n"
		  "  nmenu <INI file>\n\n"
		  "See NMENU.HLP file for more information.\n");
	dos2_exit(1);
}

char *checkArguments(char **argv, int argc)
{
	// Check if argument is a INI file
	if (argc && argv[0]) {
		dos2_strupr(argv[0]);
		char *dot = strrchr(argv[0], '.');
		if (!dot || strcmp(dot, ".INI")) {
			printHelp();
		}
	}
	// Check if INI file exists
	strcpy(buff, argc ? argv[0] : "NMENU.INI");
	if (!dos2_fileexists(buff)) {
		restoreScreen();
		csprintf(buff, "%s file not found!!\n"VT_BEEP, buff);
		cputs(buff);
		printHelp();
	}
	return buff;
}

// ========================================================
int main(char **argv, int argc) __sdcccall(0)
{
	argv, argc;

	// A way to avoid using low memory when using BIOS calls from DOS
	if (heap_top < (void*)0x8000)
		heap_top = (void*)0x8000;

	// Initialize generic string buffer
	buff = malloc(200);
	heapBackup = heap_top;

	// Check arguments
	buff = checkArguments(argv, argc);

	//Platform system checks
	checkPlatformSystem();

	// Initialize ANSI screen
	if (kanjiMode != MODE_ANK) {
		setKanjiMode(MODE_ANK);
	}
	AnsiInit();
	AnsiStartBuffer();
	AnsiPrint(ANSI_CURSOROFF);

	// Initialize menu structure
	menu_init(buff);

	menu_loop();

	AnsiEndBuffer();
	AnsiFinish();

	restoreScreen();
	return 0;
}
