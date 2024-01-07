//
// Copyright(C) 1993-1996 Id Software, Inc.
// Copyright(C) 2005-2014 Simon Howard
// Copyright(C) 2014-2017 Fabian Greffrath
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// DESCRIPTION:
//	Crispy Doom specific variables.
//


#ifndef __CRISPY_H__
#define __CRISPY_H__

#include "doomtype.h"

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif
#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif
#ifndef BETWEEN
#define BETWEEN(l,u,x) (((l)>(x))?(l):((x)>(u))?(u):(x))
#endif

#define CRISPY_FPSLIMIT_MAX 500

typedef struct
{
	// [crispy] "crispness" config variables
	int automapoverlay;
	int automaprotate;
	int automapstats;
	int bobfactor;
	int brightmaps;
	int btusetimer;
	int centerweapon;
	int coloredblood;
	int coloredhud;
	int crosshair;
	int crosshairhealth;
	int crosshairtarget;
	int crosshairtype;
	int defaultskill;
	int demotimer;
	int demotimerdir;
	int demotimerpos;
	int demobar;
	int doom2projbypass;
	int evadinginterover;
	int extautomap;
	int flipcorpses;
	int fliplevels;
	int flipweapons;
	int fpslimit;
	int freeaim;
	int freelook;
	int freelook_hh;
	int gamma;
	int hires;
	int jump;
	int leveltime;
	int logo;
	int mouselook;
	int neghealth;
	int overunder;
	int pitch;
	int playercoords;
	int recoil;
	int screenwipe;
	int mapsecrets;
	int mapviewstats;
	int secretmessage;
	int smarttotals;
	int smoothlight;
	int smoothmap;
	int smoothscaling;
	int soundfix;
	int soundfull;
	int soundmono;
	int statsformat;
	int translucency;
#ifdef CRISPY_TRUECOLOR
	int truecolor;
#endif
	int uncapped;
	int vsync;
	int weaponsquat;
	int widehud;
	int widescreen;

	// [crispy] in-game switches and variables
	int screenshotmsg;
	int snowflakes;
	int cleanscreenshot;
	int demowarp;
	int fps;
	int rendered_segs;
	int rendered_visplanes;
	int rendered_sprites;
	int rendered_openings;

	boolean flashinghom;
//	boolean fliplevels;
//	boolean flipweapons;
	boolean haved1e5;
	boolean haved1e6;
	boolean havee1m10;
	boolean havemap33;
	boolean havessg;
	boolean singleplayer;
	boolean stretchsky;

	// [crispy] custom difficulty parameters
	boolean autohealth;
	boolean fast;
	boolean keysloc;
	boolean moreammo;
	boolean pistolstart;

	char *havenerve;
	char *havemaster;
	char *havesigil;
	char *havesigil2;

	const char *sdlversion;
	const char *platform;

	void (*post_rendering_hook) (void);
} crispy_t;

extern crispy_t *const crispy;
extern const crispy_t *critical;

extern void CheckCrispySingleplayer (boolean singleplayer);

enum
{
	REINIT_FRAMEBUFFERS = 1,
	REINIT_RENDERER = 2,
	REINIT_TEXTURES = 4,
	REINIT_ASPECTRATIO = 8,
};

enum
{
    UNCAPPEDFRAMERATE_OFF,
    UNCAPPEDFRAMERATE_FULL,
    UNCAPPEDFRAMERATE_CAMERAMOVEMENT,
    UNCAPPEDFRAMERATE_FULL_VSYNC,
    UNCAPPEDFRAMERATE_CAMERAMOVEMENT_VSYNC,
    NUM_UNCAPPEDFRAMERATES,
};

enum
{
    VSYNC_OFF,
    VSYNC_ON,
    VSYNC_CAPPED,
    NUM_VSYNC,
};

enum
{
    PIXELASPECTRATIO_OFF,
    PIXELASPECTRATIO_1_2,
    PIXELASPECTRATIO_1,
    NUM_PIXELASPECTRATIOS,
};

enum
{
    BOBFACTOR_FULL,
    BOBFACTOR_75,
    BOBFACTOR_50,
    BOBFACTOR_OFF,
    NUM_BOBFACTORS,
};

enum
{
    BRIGHTMAPS_OFF,
    BRIGHTMAPS_TEXTURES,
    BRIGHTMAPS_SPRITES,
    BRIGHTMAPS_BOTH,
    NUM_BRIGHTMAPS,
};

enum
{
	COLOREDBLOOD_OFF,
	COLOREDBLOOD_SKULLPUFFS,
	COLOREDBLOOD_RGB,
	COLOREDBLOOD_SKULLPUFFS_RGB,
	COLOREDBLOOD_ALL,
	COLOREDBLOOD_PLUSCORPSES,
	NUM_COLOREDBLOOD,
};

enum
{
    CENTERWEAPON_OFF,
    CENTERWEAPON_HOR,
    CENTERWEAPON_CENTER,
    CENTERWEAPON_BOB,
    NUM_CENTERWEAPON,
};

enum
{
    COLOREDHUD_OFF,
    COLOREDHUD_BAR,
    COLOREDHUD_TEXT,
    COLOREDHUD_BOTH,
    NUM_COLOREDHUD
};

enum
{
    CROSSHAIR_OFF,
    CROSSHAIR_STATIC,
    CROSSHAIR_PROJECTED,
    NUM_CROSSHAIRS,
    CROSSHAIR_INTERCEPT = 0x10
};

enum
{
    DEMOTIMER_OFF,
    DEMOTIMER_RECORD,
    DEMOTIMER_PLAYBACK,
    DEMOTIMER_BOTH,
    NUM_DEMOTIMERS
};

enum
{
    FREEAIM_AUTO,
    FREEAIM_DIRECT,
    FREEAIM_BOTH,
    NUM_FREEAIMS
};

enum
{
    FREELOOK_OFF,
    FREELOOK_SPRING,
    FREELOOK_LOCK,
    NUM_FREELOOKS
};

enum
{
    FREELOOK_HH_LOCK,
    FREELOOK_HH_SPRING,
    NUM_FREELOOKS_HH
};

enum
{
    HIRES_OFF,
    HIRES_DOUBLE,
    HIRES_QUAD,
    NUM_HIRES
};

enum
{
    JUMP_OFF,
    JUMP_LOW,
    JUMP_HIGH,
    NUM_JUMPS
};

enum
{
    LOGO_OFF,
    LOGO_MAINMENU,
    LOGO_OPTIONS,
    LOGO_BOTH,
    NUM_LOGOS
};

enum
{
    RATIO_NON_WIDE,
    RATIO_MATCH_SCREEN,
    RATIO_16_10,
    RATIO_17_10,
    RATIO_16_9,
    RATIO_17_9,
    RATIO_18_9,
    RATIO_21_9,
    NUM_RATIOS
};

enum
{
    TRANSLUCENCY_OFF,
    TRANSLUCENCY_MISSILE,
    TRANSLUCENCY_ITEM,
    TRANSLUCENCY_BOTH,
    NUM_TRANSLUCENCY
};

enum
{
    SECRETMESSAGE_OFF,
    SECRETMESSAGE_ON,
    SECRETMESSAGE_COUNT,
    NUM_SECRETMESSAGE
};

enum
{
    MAPSECRETS_DEFAULT,
    MAPSECRETS_DEFAULT_AND_COLOR,
    MAPSECRETS_FORCE,
    MAPSECRETS_FORCE_AND_COLOR,
    NUM_MAPSECRETS
};

enum
{
    MAPVIEWSTATS_OFF,
    MAPVIEWSTATS_NO_FPS,
    MAPVIEWSTATS_WITH_FPS,
    NUM_MAPVIEWSTATS
};

enum
{
    WIDEHUD_OFF,
    WIDEHUD_WIDE,
    WIDEHUD_COMPACT,
    NUM_WIDEHUDS
};

enum
{
    WIDGETS_OFF,
    WIDGETS_AUTOMAP,
    WIDGETS_ALWAYS,
    WIDGETS_STBAR,
    NUM_WIDGETS
};

enum
{
    STATSFORMAT_RATIO,
    STATSFORMAT_REMAINING,
    STATSFORMAT_PERCENT,
    STATSFORMAT_BOOLEAN,
    NUM_STATSFORMATS
};

enum
{
    SKILL_ITYTD,
    SKILL_HNTR,
    SKILL_HMP,
    SKILL_UV,
    SKILL_NIGHTMARE,
    NUM_SKILLS
};

#endif
