//
// Copyright(C) 1993-1996 Id Software, Inc.
// Copyright(C) 2005-2014 Simon Howard
// Copyright(C) 2015-2018 Fabian Greffrath
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
//	[crispy] Crispness menu
//

#ifndef __M_CRISPY__
#define __M_CRISPY__

typedef struct
{
    int value;
    const char *name;
} multiitem_t;

extern multiitem_t multiitem_uncappedframerate[NUM_UNCAPPEDFRAMERATES];
extern multiitem_t multiitem_pixelaspectratio[NUM_PIXELASPECTRATIOS];
extern multiitem_t multiitem_bobfactor[NUM_BOBFACTORS];
extern multiitem_t multiitem_brightmaps[NUM_BRIGHTMAPS];
extern multiitem_t multiitem_coloredblood[NUM_COLOREDBLOOD];
extern multiitem_t multiitem_centerweapon[NUM_CENTERWEAPON];
extern multiitem_t multiitem_coloredhud[NUM_COLOREDHUD];
extern multiitem_t multiitem_crosshair[NUM_CROSSHAIRS];
extern multiitem_t multiitem_crosshairtype[];
extern multiitem_t multiitem_demotimer[NUM_DEMOTIMERS];
extern multiitem_t multiitem_demotimerdir[];
extern multiitem_t multiitem_demotimerpos[];
extern multiitem_t multiitem_difficulties[NUM_SKILLS];
extern multiitem_t multiitem_freeaim[NUM_FREEAIMS];
extern multiitem_t multiitem_freelook[NUM_FREELOOKS];
extern multiitem_t multiitem_hires[NUM_HIRES];
extern multiitem_t multiitem_jump[NUM_JUMPS];
extern multiitem_t multiitem_logo[NUM_LOGOS];
extern multiitem_t multiitem_sndchannels[4];
extern multiitem_t multiitem_mapsecrets[NUM_MAPSECRETS];
extern multiitem_t multiitem_mapviewstats[NUM_MAPVIEWSTATS];
extern multiitem_t multiitem_secretmessage[NUM_SECRETMESSAGE];
extern multiitem_t multiitem_statsformat[NUM_STATSFORMATS];
extern multiitem_t multiitem_translucency[NUM_TRANSLUCENCY];
extern multiitem_t multiitem_widehud[NUM_WIDEHUDS];
extern multiitem_t multiitem_widescreen[NUM_RATIOS];
extern multiitem_t multiitem_widgets[NUM_WIDGETS];
extern void M_CrispyTogglePixelAspectRatio(int choice);
extern void M_CrispyToggleAutomapstats(int choice);
extern void M_CrispyToggleBobfactor(int choice);
extern void M_CrispyToggleBrightmaps(int choice);
extern void M_CrispyToggleCenterweapon(int choice);
extern void M_CrispyToggleColoredblood(int choice);
extern void M_CrispyToggleColoredhud(int choice);
extern void M_CrispyToggleCrosshair(int choice);
extern void M_CrispyToggleCrosshairHealth(int choice);
extern void M_CrispyToggleCrosshairTarget(int choice);
extern void M_CrispyToggleCrosshairtype(int choice);
extern void M_CrispyToggleDemoBar(int choice);
extern void M_CrispyToggleDemoTimer(int choice);
extern void M_CrispyToggleDemoTimerDir(int choice);
extern void M_CrispyToggleDemoTimerPos(int choice);
extern void M_CrispyToggleDemoUseTimer(int choice);
extern void M_CrispyToggleNonDoom1ProjBypassNotTriggerSpecLines(int choice);
extern void M_CrispyToggleEvadingInterOver(int choice);
extern void M_CrispyToggleExtAutomap(int choice);
extern void M_CrispyToggleFlipcorpses(int choice);
extern void M_CrispyToggleFreeaim(int choice);
extern void M_CrispyToggleFreelook(int choice);
extern void M_CrispyToggleFullsounds(int choice);
extern void M_CrispyToggleFpsLimit(int choice);
extern void M_CrispyToggleHires(int choice);
extern void M_CrispyToggleJumping(int choice);
extern void M_CrispyToggleLeveltime(int choice);
extern void M_CrispyToggleLogo(int choice);
extern void M_CrispyToggleMouseLook(int choice);
extern void M_CrispyToggleNeghealth(int choice);
extern void M_CrispyToggleDefaultSkill(int choice);
extern void M_CrispyToggleOverunder(int choice);
extern void M_CrispyTogglePitch(int choice);
extern void M_CrispyTogglePlayerCoords(int choice);
extern void M_CrispyToggleRecoil(int choice);
extern void M_CrispyToggleScreenwipe(int choice);
extern void M_CrispyToggleMapSecrets(int choice);
extern void M_CrispyToggleMapViewStats(int choice);
extern void M_CrispyToggleSecretmessage(int choice);
extern void M_CrispyToggleSmartTotals(int choice);
extern void M_CrispyToggleSmoothLighting(int choice);
extern void M_CrispyToggleSmoothMap(int choice);
extern void M_CrispyToggleSmoothScaling(int choice);
extern void M_CrispyToggleSndChannels(int choice);
extern void M_CrispyToggleSoundfixes(int choice);
extern void M_CrispyToggleSoundMono(int choice);
extern void M_CrispyToggleStatsFormat(int choice);
extern void M_CrispyToggleTranslucency(int choice);
extern void M_CrispyToggleUncapped(int choice);
extern void M_CrispyToggleWeaponSquat(int choice);
extern void M_CrispyToggleWideHUD(int choice);
extern void M_CrispyToggleWidescreen(int choice);

#endif
