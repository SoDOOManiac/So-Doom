# So Doom
[![So Doom Icon](https://github.com/Zodomaniac/So-Doom/blob/master/data/sodoom.png)](https://github.com/zodomaniac/so-doom)

[![Top Language](https://img.shields.io/github/languages/top/fabiangreffrath/crispy-doom.svg?style=flat)](https://github.com/fabiangreffrath/crispy-doom)
[![Code Size](https://img.shields.io/github/languages/code-size/fabiangreffrath/crispy-doom.svg?style=flat)](https://github.com/fabiangreffrath/crispy-doom)
[![License](https://img.shields.io/github/license/fabiangreffrath/crispy-doom.svg?style=flat&logo=gnu)](https://github.com/fabiangreffrath/crispy-doom/blob/master/COPYING.md)
[![Release](https://img.shields.io/github/release/fabiangreffrath/crispy-doom.svg?style=flat)](https://github.com/fabiangreffrath/crispy-doom/releases)
[![Release Date](https://img.shields.io/github/release-date/fabiangreffrath/crispy-doom.svg?style=flat)](https://github.com/fabiangreffrath/crispy-doom/releases)
[![Downloads](https://img.shields.io/github/downloads/fabiangreffrath/crispy-doom/latest/total.svg?style=flat)](https://github.com/fabiangreffrath/crispy-doom/releases)
[![Commits](https://img.shields.io/github/commits-since/fabiangreffrath/crispy-doom/latest.svg?style=flat)](https://github.com/fabiangreffrath/crispy-doom/commits/master)
[![Last Commit](https://img.shields.io/github/last-commit/fabiangreffrath/crispy-doom.svg?style=flat)](https://github.com/fabiangreffrath/crispy-doom/commits/master)
[![Travis Build Status](https://img.shields.io/travis/com/fabiangreffrath/crispy-doom.svg?style=flat&logo=travis)](https://travis-ci.com/fabiangreffrath/crispy-doom/)

So Doom is a limit-removing enhanced-resolution Doom source port based on [Crispy Doom](https://www.chocolate-doom.org/wiki/index.php/Crispy_Doom).

Its name is a tribute to [Sodom, German thrash metal band](http://sodomized.info/).

## Synopsis

So Doom is a friendly fork of [Crispy Doom](https://www.chocolate-doom.org/wiki/index.php/Crispy_Doom) that removes the [static limits](https://doomwiki.org/wiki/Static_limits) of the Doom engine, including a singleplayer-only optional removal of INTERCEPTS limit.

## Objectives and features

So Doom is a source port that aims to provide a faithful Doom gaming experience while also featuring some user-requested improvements and enhancements. It is forked off of Crispy Doom to take advantage of its free and open-source code base, portability, accuracy and compatibility with Vanilla Doom.

Its distinguishing features are:

 * Hot-swapping of Crispy Doom's Flipped Levels and Flipped Weapons features.
 * Option for INTERCEPTS overflow evasion in singleplayer, shadowed and disabled in netgames.
 
So Doom inherits all the core features of its parent Crispy Doom:
 
 * Enhanced 640x400 display resolution, with the original 320x200 resolution still available in the "High Resolution Rendering: Off" mode.
 * Uncapped rendering framerate with interpolation and optional vertical synchronization (VSync) with the screen refresh rate.
 * Intermediate gamma correction levels (0.5, 1.5, 2.5 and 3.5).
 * Removal of all static engine limits, or at least raising of the less crucial ones.
 * Full support for the "Doom Classic" WADs shipped with the "Doom 3: BFG Edition", especially the "No Rest For The Living" episode shipped in the NERVE.WAD file.
 * Support for all versions of John Romero's Episode 5: Sigil for Ultimate Doom.

Furthermore, all the optional Crispy's user-visible and audible features are available:

 * Jumping.
 * Free vertical looking, including mouse look and vertical aiming.
 * Aiming support by a crosshair that may get directly rendered into the game world.
 * A new minimal Crispy HUD, displaying only the status bar numbers.
 * Clean Screenshot feature, enabling to take screenshots without HUD elements and even without status bar numbers and weapon sprites at higher screen sizes.
 * Colorized status bar numbers, HUD texts and blood sprites for certain monsters.
 * Translucency for certain sprites and status bar elements in the Crispy HUD.
 * Randomly mirrored death animations and corpse sprites.
 * Command line options to allow for playing with flipped player weapon sprites and/or entirely flipped level geometry.
 * Players may walk over or under monsters and hanging corpses.
 * Centered Weapons when firing, weapon recoil thrust and pitch.
 * Reports whenever a secret is revealed.
 * Level statistics and extended coloring in the Automap.
 * Playing sounds in full length, and misc. other sound fixes.
 * Demo recording and/or playback timers and progress bar.
 * Demo continue and take-over features, handing controls over to the player when demo playback is finished or interrupted.

Most of these features are disabled by default and need to get enabled either in the in-game "SoDOOMy" menu, in the so-doom-setup tool or as command line parameters. They are implemented in a way that preserves demo-compatibility with Vanilla Doom and network game compatibility with Chocolate Doom. Furthermore, So Doom's savegames and config files are compatible, though not identical (see the [Compatibility section in the Wiki](https://github.com/fabiangreffrath/crispy-doom/wiki/Compatibility)), to Vanilla Doom's.

So Doom strives for maximum compatibility with all "limit-removing Vanilla" maps -- but not Boom or ZDoom maps. More specifically, So Doom, supports some select advanced features such as [ANIMATED](https://doomwiki.org/wiki/ANIMATED) and [SWITCHES](https://doomwiki.org/wiki/SWITCHES) lumps, MBF sky transfers, SMMU swirling flats and [MUSINFO](https://doomwiki.org/wiki/MUSINFO) -- but neither generalized linedef and sector types nor DECORATE and MAPINFO.

Many additional less user-visible features have been implemented, e.g. fixed engine limitations and crashes, fixed rendering bugs, fixed harmless game logic bugs, full support for DEHACKED files and lumps in BEX format, additional and improved cheat codes, an improved Automap, and many more! Due to the extra DEHACKED states added from [MBF](https://doomwiki.org/wiki/MBF), So Doom supports [enhancer](https://www.doomworld.com/forum/topic/84859-black-ops-smooth-weapons-dehacked-mod) [mods](https://www.doomworld.com/forum/topic/85991-smoothed-smooth-monsters-for-doom-retro-and-crispy-doom) that can make the gameplay even more pleasing to the eyes. For a detailed list of features and changes please refer to the release notes below.

### New controls (with default bindings)

 * Move Forward (alt.) <kbd>W</kbd>
 * Move Backward (alt.) <kbd>S</kbd>
 * Strafe Left (alt.) <kbd>A</kbd>
 * Strafe Right (alt.) <kbd>D</kbd>
 * Jump (bindable to joystick and mouse buttons as well) <kbd>/</kbd> (as in Hexen and Strife)
 * Quick Reverse (bindable to mouse buttons as well)
 * Mouse Look (bindable to mouse buttons or permanent)
 * Look up (bindable to joystick axes as well) <kbd>PgDn</kbd> (as in Heretic)
 * Look down (bindable to joystick axes as well) <kbd>Del</kbd> (as in Heretic)
 * Center view <kbd>End</kbd> (as in Heretic)
 * Toggle always run <kbd>&#8682;</kbd>
 * Toggle vertical mouse movement (new in 5.4)
 * Delete savegame <kbd>Del</kbd>
 * Go to next level
 * Reload current level
 * Save a clean screenshot
 * Flip levels
 * Flip weapons
 * Toggle Automap overlay mode <kbd>O</kbd>
 * Toggle Automap rotate mode <kbd>R</kbd>
 * Resurrect from savegame (single player mode only) "Run" + "Use"

### New command line parameters

 * `-dm3` specifies the Deathmatch 3.0 rules (weapons stay, items respawn) for netgames (since 4.1).
 * `-episode 1` launches Hell on Earth and `-episode 2` launches No Rest for the Living episode if the Doom 2 IWAD shipped with the Doom 3: BFG Edition is used.
 * `-warp 1a` warps to the secret level E1M10: Sewers of XBox Doom IWAD (since 2.3).
 * `-mergedump <file>` merges the PWAD file(s) given on the command line with the IWAD file and writes the resulting data into the `<file>` given as argument. May be considered as a replacement for the `DEUSF.EXE` tool (since 2.3).
 * `-blockmap` forces a (re-)building of the BLOCKMAP lumps for loaded maps (since 2.3).
 * `-playdemo demoname -warp N` plays back fast-forward up to the requested map (since 3.0).
 * `-loadgame N -record demoname` and `-loadgame N -playdemo demoname` allow to record and play demos starting from a savegame instead of the level start (since 4.0).
 * `-playdemo demoname1 -record demoname2` plays back fast-forward until the end of demoname1 and continues recording as demoname2 (new in 5.5).
 * `-fliplevels` loads mirrored versions of the maps (this was the default on April 1st up to version 5.0).
 * `-flipweapons` flips the player's weapons (new in 5.3).

### New cheat codes

 * `TNTWEAP` or `TW` followed by a weapon number gives or removes this weapon (8 = Chainsaw, 9 = SSG). `TNTWEAP0` or `TW0` takes away all weapons and ammo except for the pistol and 50 bullets. Try to load Doom 1 with `DOOM2.WAD` as a PWAD and type `TNTWEAP9` to play the SSG in Doom 1.
 * `TA` followed by a number gives the specified type of ammo (1-berserk pack, 2-5 - ammo corresponding to 1-4 lines in HUD, 0 takes away all ammo). 
 * `TNTEM`, `KILLEM` or `FHHALL` kill all monsters on the current map (and disables all cube spitters).
 * `SPECHITS` triggers all [Linedef actions](https://doomwiki.org/wiki/Linedef_type) on a map at once, no matter if they are enabled by pushing, walking over or shooting or whether they require a key or not. It also triggers all boss monster and Commander Keen actions if possible.
 * `NOTARGET` or `FHSHH` toggle deaf and blind monsters that do not act until attacked.
 * `TNTHOM` toggles the flashing [HOM](https://doomwiki.org/wiki/Hall_of_mirrors_effect) indicator (disabled by default).
 * `SHOWFPS` or `IDRATE` toggle printing the FPS in the upper right corner.
 * `NOMOMENTUM` toggles a debug aid for pixel-perfect positioning on a map (not recommended to use in-game).
 * `GOOBERS` triggers an easter egg, i.e. an "homage to an old friend". ;-)
 * `IDBEHOLD0` disables all currently active power-ups (since 2.2).
 * `IDCLEV00` restarts the current level (since 2.0).
 * `IDMUS00` restarts the current music (new in 5.1).
 * `VERSION` shows the engine version, build date and SDL version (new in 5.1).
 * `SKILL` shows the current skill level (new in 5.5.2).

## Download

Binaries for Windows XP / Vista / 7 / 8.1 / 10 (both x86 and x64 editions) are available here: 
https://github.com/Zodomaniac/So-Doom/releases/download/so-doom-5.6.7/so-doom-5.6.7.zip

So Doom can play nearly all variants of Doom. If you don't own any, you may download the [Shareware version of Doom](http://cdn.debian.net/debian/pool/non-free/d/doom-wad-shareware/doom-wad-shareware_1.9.fixed.orig.tar.gz), extract it and copy the DOOM1.WAD file into your So Doom directory. Alternatively, you may want to play So Doom with [Freedoom](https://www.chocolate-doom.org/wiki/index.php/Freedoom) and a MegaWAD.

### Sources

So Doom source code is available at GitHub: https://github.com/zodomaniac/so-doom.
It can be [downloaded in either ZIP or TAR.GZ format](https://github.com/zodomaniac/so-doom/releases) 
or cloned via

```
 git clone https://github.com/zodomaniac/so-doom.git
```

Brief instructions to set up a build system on Windows can be found [in the Crispy Doom Wiki](https://github.com/fabiangreffrath/crispy-doom/wiki/Building-on-Windows). A much more detailed guide is provided [in the Chocolate Doom Wiki](https://www.chocolate-doom.org/wiki/index.php/Building_Chocolate_Doom_on_Windows), but applies to So Doom as well for most parts.

Compilation on Debian systems (Debian 10 "buster" or later) should be as simple as

```
 sudo apt install build-essential automake git
 sudo apt build-dep so-doom
```

to install the prerequisites and then

```
 cd so-doom
 autoreconf -fiv
 ./configure
 make
```

After successful compilation the resulting binaries can be found in the `src/` directory.

## News

### So Doom 5.6.8

So Doom 5.6.8 is to be released on Nov 11, 2019.

**So Doom-specific changes**

 * 'So Doomy HUD' with the status bar face above the ammo widget and translucent background for netgames has been introduced. Thanks a lot to Fabian for all the hints!

**Changes pulled from Crispy Doom**

 * Updating the Archvile fire's `floorz` and `ceilingz` values has been reverted, as it turned out to be the culprit for a demo desync that fraggle discovered. Thanks for that!
 * 'Intermediate Crispy HUD' without the status bar but with the face and its background in place has been introduced.
 * Fuzz effect animation remaining static in one case has been fixed, this happened if the number of pixels to apply the fuzz effect to was an integer multiple of FUZZTABLE. Thanks to JNechaevsky for the suggestion!

### So Doom 5.6.7

So Doom 5.6.7 has been released on Nov 4, 2019.

**So Doom-specific changes**

 * INTERCEPTS overflow option has been implemented in drfrag666's way for better performance.
 * TNTWEAPx (TWx) cheats now display the hint.
 * Changed the hint displayed on typing the TAx cheat.

**Changes pulled from Crispy Doom**

 * Disallowing the vertical mouse movement now disables controlling the menus with the mouse, thanks to bryc for requesting this.
 * TNTWEAP0 now removes the berserk strength, all weapons and ammo except for pistol and 50 bullets.
 * Status bar face expression staying across level changes has been removed, thanks to JNechaevsky for pointing this out.

### So Doom 5.6.6

So Doom 5.6.6 has been released on Oct 28, 2019.

**So Doom-specific changes**

 * New cheat for getting specific ammo (TAx) and shorter one for getting specific weapon (TWx) have been introduced.

**Changes pulled from Crispy Doom**

 * DOOM.WAD v0.99-v1.2 are now supported along with v1.2 demos, merged from Chocolate Doom, contributed by SmileTheory. 
 * Automap panning in flippedlevels mode has been fixed, thanks to JNechaevsky for reporting.
 * Self-repeating states in `P_LatestSafeState()` are now handled.
 * Max-sized background buffer is now allocated for the bezel. This fixes a crash when the game is started with `crispy->hires == 0` and `scaledviewwidth != SCREENWIDTH` and then `crispy->hires` is switched to `1`.
 * Early exit from the tally screen after ExM8 is now forced, which enables demos to progress to the end of game sequence.
 * TNTWEAP2 now removes the pistol.

### So Doom 5.6.5

So Doom 5.6.5 has been released on Oct 15, 2019.

**So Doom-specific changes**

 * The main menu logo has been edited, and its position changed to the left upper corner.
 *  Brightness of the SoDOOMy menu logo has been reduced, thanks to JNechaevsky﻿ for the help with file conversion.

**Changes pulled from Crispy Doom**

 * Switching to the fist after typing a cheat expecting two parameters has been fixed. This affects IDMUSx1 and IDCLEVx1, thanks to maxmanium for pointing this out.

### So Doom 5.6.4

So Doom 5.6.4 has been released on Oct 14, 2019.

**So Doom-specific changes**

 * The values of message-producing settings (detail, messages, always run etc.) are now colorized: low (off) values in red, high (on) values in green. Sodom-style S has been added to the main menu background as port signature.

**Changes pulled from Crispy Doom**

 * Automap marker coordinate for flipped levels has been fixed.


### So Doom 5.6.3 

Here is a piece of good news for authors of limit-removing maps: so-called So Doom 5.6.3.
So Doom 5.6.3 has been released on Oct 11, 2019 and has an option for removing INTERCEPTS limit that me and Fabian Greffrath have different views on,
as well as flipped levels and weapons swappable on the run.

**Features**

 * INTERCEPTS limit evasion (courtesy drfrag666) SoDOOMy menu item applied in single player (including SP demos), disabled by default, shadowed and disabled in netgames﻿. 
 * Flipped levels and weapons features are now hot-swappable and can be toggled by keys, thanks to Fabian Greffrath having completely overhauled them and also made them demo-compatible. 

**Improvements**

 * Some clipping optimizations taken from JNechaevsky's Russian Doom (and there from MBF respectively) have been implemented.
 * Savegame name is automatically overridden on saving if it already starts with a map identifier, proposed by zebzorb.
 * Status bar optimizations, including numbers to be only redrawn if necessary, on JNechaevsky's suggestion.
 * In automap overlay mode the automap is now drawn on top of everything as JNechaevsky suggested, not beneath the bezel for decreased screen sizes.

**Bug Fixes**
 * Loss of grid lines near the automap boundary has been fixed, spotted by JNechaevsky.
 * Overlayed automap blinking one tic on screen borders has been fixed, noticed by JNechaevsky.

**Known Issues**

 * [No music and high-pitched sound effects](https://github.com/fabiangreffrath/crispy-doom/issues/454) occur with SDL2.dll v2.0.10 and SDL2_mixer.dll v2.0.4 on Windows in case of 5.1 speaker configuration, according to investigation by StasBFG. Here is [the DLL pack fixing this and providing fluidsynth soundfont support](https://github.com/fabiangreffrath/crispy-doom/files/3616050/crispy-doom-DLL-fix-pack.zip).

So Doom 5.6.3 is based on Crispy Doom 5.6.3 and has merged all changes to the Crispy Doom master branch up to commit [`af14e55d`](https://github.com/fabiangreffrath/crispy-doom/commit/af14e55dc45a846e28d7b0e99851f1548a9c6b1b).

## Documentation

 * **[Changelog](https://github.com/fabiangreffrath/crispy-doom/wiki/Changelog)**
 * **[Compatibility](https://github.com/fabiangreffrath/crispy-doom/wiki/Compatibility)**
 * **[Crispness](https://github.com/fabiangreffrath/crispy-doom/wiki/Crispness)**
 * **[FAQ](https://github.com/fabiangreffrath/crispy-doom/wiki/FAQ)**

## Versioning

So Doom's version number is increased whenever a new Crispy Doom (pre-)release got merged into its code base.

## Contact

The canonical homepage for So Doom is https://github.com/zodomaniac/so-doom

So Doom is maintained by [Vladislav Melnichuk](mailto:zodomaniac@gmXremovethisXail.com). 

Please report any bugs, glitches or crashes that you encounter to the GitHub [Issue Tracker](https://github.com/fabiangreffrath/crispy-doom/issues).

## Acknowledgement

Thanks a lot to Crispy Doom's developer Fabian Greffrath for sharing his work openly! 

I join all Fabian's acknoledgements I cite here:

"Although I have played the thought of hacking on Chocolate Doom's renderer for quite some time already, it was Brad Harding's [Doom Retro](https://www.chocolate-doom.org/wiki/index.php/Doom_Retro) that provided the incentive to finally do it. However, his fork aims at a different direction and I did not take a single line of code from it. Lee Killough's [MBF](https://doomwiki.org/wiki/WinMBF) was studied and used to debug the code, especially in the form of Team Eternity's [WinMBF](https://doomwiki.org/wiki/WinMBF) source port, which made it easier to compile and run on my machine. And of course there is fraggle's [Chocolate Doom](https://www.chocolate-doom.org/wiki/index.php/Chocolate_Doom) with its exceptionally clean and legible source code. Please let me take this opportunity to appreciate all these authors for their work!

Also, thanks to plums of the [Doomworld forums](https://www.doomworld.com/vb/) for beta testing, "release manager" Zodomaniac and "art director" JNechaevsky for the continuous flow of support and inspiration during the post-3.x development cycle and (last but not the least) [Cacodemon9000](http://www.moddb.com/members/cacodemon9000) for his [Infested Outpost](http://www.moddb.com/games/doom-ii/addons/infested-outpost) map that helped to track down quite a few bugs!

Furthermore, thanks to VGA for his aid with adding support for his two mods: [PerK & NightFright's Black Ops smooth weapons add-on converted to DEHACKED](https://www.doomworld.com/forum/topic/84859-black-ops-smooth-weapons-dehacked-mod) and [Gifty's Smooth Doom smooth monster animations converted to DEHACKED](https://www.doomworld.com/forum/topic/85991-smoothed-smooth-monsters-for-doom-retro-and-crispy-doom) that can make the gameplay even more pleasing to the eyes".

## Legalese

Doom is © 1993-1996 Id Software, Inc.; 
Boom 2.02 is © 1999 id Software, Chi Hoang, Lee Killough, Jim Flynn, Rand Phares, Ty Halderman;
PrBoom+ is © 1999 id Software, Chi Hoang, Lee Killough, Jim Flynn, Rand Phares, Ty Halderman,
© 1999-2000 Jess Haas, Nicolas Kalkhof, Colin Phipps, Florian Schulze,
© 2005-2006 Florian Schulze, Colin Phipps, Neil Stevens, Andrey Budko;
Chocolate Doom is © 1993-1996 Id Software, Inc., © 2005 Simon Howard; 
Chocolate Hexen is © 1993-1996 Id Software, Inc., © 1993-2008 Raven Software, © 2008 Simon Howard;
Strawberry Doom is © 1993-1996 Id Software, Inc., © 2005 Simon Howard, © 2008-2010 GhostlyDeath; 
Crispy Doom is © 2014-2019 Fabian Greffrath;
additionally So Doom is © 2019 Vladislav Melnichuk;
all of the above are released under the [GPL-2+](https://www.gnu.org/licenses/gpl-2.0.html).

SDL 2.0, SDL_mixer 2.0 and SDL_net 2.0 are © 1997-2016 Sam Lantinga and are released under the [zlib license](http://www.gzip.org/zlib/zlib_license.html).

Secret Rabbit Code (libsamplerate) is © 2002-2011 Erik de Castro Lopo and is released under the [GPL-2+](http://www.gnu.org/licenses/gpl-2.0.html).
Libpng is © 1998-2014 Glenn Randers-Pehrson, © 1996-1997 Andreas Dilger, © 1995-1996 Guy Eric Schalnat, Group 42, Inc. and is released under the [libpng license](http://www.libpng.org/pub/png/src/libpng-LICENSE.txt).
Zlib is © 1995-2013 Jean-loup Gailly and Mark Adler and is released under the [zlib license](http://www.zlib.net/zlib_license.html).

The Crispy Doom icon (as shown at the top of this page) is composed of the [Chocolate Doom icon](https://www.chocolate-doom.org/wiki/images/7/77/Chocolate-logo.png) and a [photo](https://en.wikipedia.org/wiki/File:Potato-Chips.jpg) of potato crisps (Utz-brand, grandma's kettle-cooked style) by [Evan-Amos](https://commons.wikimedia.org/wiki/User:Evan-Amos) who kindly released it into the [public domain](https://en.wikipedia.org/wiki/Public_domain). The current high-resolution version of this icon has been contributed by JNechaevsky (formerly by Zodomaniac).
