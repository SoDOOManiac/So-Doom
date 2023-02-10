//
// Copyright(C) 1993-1996 Id Software, Inc.
// Copyright(C) 2005-2014 Simon Howard
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
//	Refresh of things, i.e. objects represented by sprites.
//




#include <stdio.h>
#include <stdlib.h>


#include "deh_main.h"
#include "doomdef.h"

#include "i_swap.h"
#include "i_system.h"
#include "z_zone.h"
#include "w_wad.h"

#include "r_local.h"

#include "doomstat.h"

#include "v_trans.h" // [crispy] colored blood sprites
#include "p_local.h" // [crispy] MLOOKUNIT
#include "r_bmaps.h" // [crispy] R_BrightmapForTexName()


#define MINZ				(FRACUNIT*4)
#define BASEYCENTER			(ORIGHEIGHT/2)

//void R_DrawColumn (void);
//void R_DrawFuzzColumn (void);



typedef struct
{
    int		x1;
    int		x2;
	
    int		column;
    int		topclip;
    int		bottomclip;

} maskdraw_t;

static size_t num_vissprite, num_vissprite_alloc, num_vissprite_ptrs; // killough
static vissprite_t **vissprite_ptrs;

// [JN] optimization for drawing huge amount of drawsegs.
// adapted from prboom-plus/src/r_things.c
typedef struct drawseg_xrange_item_s
{
    short x1, x2;
    drawseg_t *user;
} drawseg_xrange_item_t;

typedef struct drawsegs_xrange_s
{
    drawseg_xrange_item_t *items;
    int count;
} drawsegs_xrange_t;

#define DS_RANGES_COUNT 3
static drawsegs_xrange_t drawsegs_xranges[DS_RANGES_COUNT];

static drawseg_xrange_item_t *drawsegs_xrange;
static unsigned int drawsegs_xrange_size = 0;
static int drawsegs_xrange_count = 0;

static degenmobj_t laserspot_m = {{0}};
degenmobj_t *laserspot = &laserspot_m;

// [crispy] extendable, but the last char element must be zero,
// keep in sync with multiitem_t multiitem_crosshairtype[] in m_menu.c
static laserpatch_t laserpatch_m[NUM_CROSSHAIRTYPES + 1] = {
	{'+', "cross1", 0, 0, 0},
	{'^', "cross2", 0, 0, 0},
	{'.', "cross3", 0, 0, 0},
	{0, "", 0, 0, 0},
};
laserpatch_t *laserpatch = laserpatch_m;

//
// Sprite rotation 0 is facing the viewer,
//  rotation 1 is one angle turn CLOCKWISE around the axis.
// This is not the same as the angle,
//  which increases counter clockwise (protractor).
// There was a lot of stuff grabbed wrong, so I changed it...
//
fixed_t		pspritescale;
fixed_t		pspriteiscale;

lighttable_t**	spritelights;

// constant arrays
//  used for psprite clipping and initializing clipping
int		negonearray[MAXWIDTH]; // [crispy] 32-bit integer math
int		screenheightarray[MAXWIDTH]; // [crispy] 32-bit integer math


//
// INITIALIZATION FUNCTIONS
//

// variables used to look up
//  and range check thing_t sprites patches
spritedef_t*	sprites;
int		numsprites;

spriteframe_t	sprtemp[29];
int		maxframe;
const char	*spritename;




//
// R_InstallSpriteLump
// Local function for R_InitSprites.
//
void
R_InstallSpriteLump
( int		lump,
  unsigned	frame,
  char		rot,
  boolean	flipped )
{
    int		r;
    // [crispy] support 16 sprite rotations
    unsigned rotation = (rot >= 'A') ? rot - 'A' + 10 : (rot >= '0') ? rot - '0' : 17;
	
    if (frame >= 29 || rotation > 16) // [crispy] support 16 sprite rotations
	I_Error("R_InstallSpriteLump: "
		"Bad frame characters in lump %i", lump);
	
    if ((int)frame > maxframe)
	maxframe = frame;
		
    if (rotation == 0)
    {
	// the lump should be used for all rotations
	// [crispy] make non-fatal
	if (sprtemp[frame].rotate == false)
	    fprintf (stderr, "R_InitSprites: Sprite %s frame %c has "
		     "multip rot=0 lump\n", spritename, 'A'+frame);

	// [crispy] make non-fatal
	if (sprtemp[frame].rotate == true)
	    fprintf (stderr, "R_InitSprites: Sprite %s frame %c has rotations "
		     "and a rot=0 lump\n", spritename, 'A'+frame);
			
// [crispy] moved ...
//	sprtemp[frame].rotate = false;
	for (r=0 ; r<8 ; r++)
	{
	  // [crispy] only if not yet substituted
	  if (sprtemp[frame].lump[r] == -1)
	  {
	    sprtemp[frame].lump[r] = lump - firstspritelump;
	    sprtemp[frame].flip[r] = (byte)flipped;
	    // [crispy] ... here
	    sprtemp[frame].rotate = false;
	  }
	}
	return;
    }
	
    // the lump is only used for one rotation
    // [crispy] make non-fatal
    if (sprtemp[frame].rotate == false)
	fprintf (stderr, "R_InitSprites: Sprite %s frame %c has rotations "
		 "and a rot=0 lump\n", spritename, 'A'+frame);
		
// [crispy] moved ...
//    sprtemp[frame].rotate = true;

    // make 0 based
    rotation--;		
    if (sprtemp[frame].lump[rotation] != -1)
    {
	// [crispy] make non-fatal
	fprintf (stderr, "R_InitSprites: Sprite %s : %c : %c "
		 "has two lumps mapped to it\n",
		 spritename, 'A'+frame, '1'+rotation);
	return;
    }
		
    sprtemp[frame].lump[rotation] = lump - firstspritelump;
    sprtemp[frame].flip[rotation] = (byte)flipped;
    // [crispy] ... here
    sprtemp[frame].rotate = true;
}




//
// R_InitSpriteDefs
// Pass a null terminated list of sprite names
//  (4 chars exactly) to be used.
// Builds the sprite rotation matrixes to account
//  for horizontally flipped sprites.
// Will report an error if the lumps are inconsistant. 
// Only called at startup.
//
// Sprite lump names are 4 characters for the actor,
//  a letter for the frame, and a number for the rotation.
// A sprite that is flippable will have an additional
//  letter/number appended.
// The rotation character can be 0 to signify no rotations.
//
void R_InitSpriteDefs(const char **namelist)
{ 
    const char **check;
    int		i;
    int		l;
    int		frame;
    int		rotation;
    int		start;
    int		end;
    int		patched;
		
    // count the number of sprite names
    check = namelist;
    while (*check != NULL)
	check++;

    numsprites = check-namelist;
	
    if (!numsprites)
	return;
		
    sprites = Z_Malloc(numsprites *sizeof(*sprites), PU_STATIC, NULL);
	
    start = firstspritelump-1;
    end = lastspritelump+1;
	
    // scan all the lump names for each of the names,
    //  noting the highest frame letter.
    // Just compare 4 characters as ints
    for (i=0 ; i<numsprites ; i++)
    {
	spritename = DEH_String(namelist[i]);
	memset (sprtemp,-1, sizeof(sprtemp));
		
	maxframe = -1;
	
	// scan the lumps,
	//  filling in the frames for whatever is found
	for (l=start+1 ; l<end ; l++)
	{
	    if (!strncasecmp(lumpinfo[l]->name, spritename, 4))
	    {
		frame = lumpinfo[l]->name[4] - 'A';
		rotation = lumpinfo[l]->name[5];

		if (modifiedgame)
		    patched = W_GetNumForName (lumpinfo[l]->name);
		else
		    patched = l;

		R_InstallSpriteLump (patched, frame, rotation, false);

		if (lumpinfo[l]->name[6])
		{
		    frame = lumpinfo[l]->name[6] - 'A';
		    rotation = lumpinfo[l]->name[7];
		    R_InstallSpriteLump (l, frame, rotation, true);
		}
	    }
	}
	
	// check the frames that were found for completeness
	if (maxframe == -1)
	{
	    sprites[i].numframes = 0;
	    continue;
	}
		
	maxframe++;
	
	for (frame = 0 ; frame < maxframe ; frame++)
	{
	    switch ((int)sprtemp[frame].rotate)
	    {
	      case -1:
		// no rotations were found for that frame at all
		// [crispy] make non-fatal
		fprintf (stderr, "R_InitSprites: No patches found "
			 "for %s frame %c\n", spritename, frame+'A');
		break;
		
	      case 0:
		// only the first rotation is needed
		break;
			
	      case 1:
		// must have all 8 frames
		for (rotation=0 ; rotation<8 ; rotation++)
		    if (sprtemp[frame].lump[rotation] == -1)
			I_Error ("R_InitSprites: Sprite %s frame %c "
				 "is missing rotations",
				 spritename, frame+'A');

		// [crispy] support 16 sprite rotations
		sprtemp[frame].rotate = 2;
		for ( ; rotation<16 ; rotation++)
		    if (sprtemp[frame].lump[rotation] == -1)
		    {
			sprtemp[frame].rotate = 1;
			break;
		    }

		break;
	    }
	}
	
	// allocate space for the frames present and copy sprtemp to it
	sprites[i].numframes = maxframe;
	sprites[i].spriteframes = 
	    Z_Malloc (maxframe * sizeof(spriteframe_t), PU_STATIC, NULL);
	memcpy (sprites[i].spriteframes, sprtemp, maxframe*sizeof(spriteframe_t));
    }

}




//
// GAME FUNCTIONS
//
vissprite_t*	vissprites = NULL;

//
// R_InitSprites
// Called at program start.
//
void R_InitSprites(const char **namelist)
{
    int		i;
	
    for (i=0 ; i<SCREENWIDTH ; i++)
    {
	negonearray[i] = -1;
    }
	
    R_InitSpriteDefs (namelist);
}


// -----------------------------------------------------------------------------
// R_ClearSprites
// Called at frame start.
// -----------------------------------------------------------------------------

void R_ClearSprites (void)
{
    num_vissprite = 0;  // [JN] killough
}

// -----------------------------------------------------------------------------
// R_NewVisSprite
// -----------------------------------------------------------------------------

static vissprite_t* R_NewVisSprite (void)
{
    if (num_vissprite >= num_vissprite_alloc)   // [JN] killough
    {
        
        size_t num_vissprite_alloc_prev = num_vissprite_alloc;
        num_vissprite_alloc = num_vissprite_alloc ? num_vissprite_alloc*2 : 128;
        vissprites = I_Realloc(vissprites,num_vissprite_alloc*sizeof(*vissprites));

        // [JN] Andrey Budko: set all fields to zero
        memset(vissprites + num_vissprite_alloc_prev, 0,
        (num_vissprite_alloc - num_vissprite_alloc_prev)*sizeof(*vissprites));
    }

    return vissprites + num_vissprite++;
}


//
// R_DrawMaskedColumn
// Used for sprites and masked mid textures.
// Masked means: partly transparent, i.e. stored
//  in posts/runs of opaque pixels.
//
int*		mfloorclip; // [crispy] 32-bit integer math
int*		mceilingclip; // [crispy] 32-bit integer math

fixed_t		spryscale;
int64_t		sprtopscreen; // [crispy] WiggleFix

void R_DrawMaskedColumn (column_t* column)
{
    int64_t	topscreen; // [crispy] WiggleFix
    int64_t 	bottomscreen; // [crispy] WiggleFix
    fixed_t	basetexturemid;
    int		top = -1;
	
    basetexturemid = dc_texturemid;
    dc_texheight = 0; // [crispy] Tutti-Frutti fix
	
    for ( ; column->topdelta != 0xff ; ) 
    {
	// [crispy] support for DeePsea tall patches
	if (column->topdelta <= top)
	{
		top += column->topdelta;
	}
	else
	{
		top = column->topdelta;
	}
	// calculate unclipped screen coordinates
	//  for post
	topscreen = sprtopscreen + spryscale*top;
	bottomscreen = topscreen + spryscale*column->length;

	dc_yl = (int)((topscreen+FRACUNIT-1)>>FRACBITS); // [crispy] WiggleFix
	dc_yh = (int)((bottomscreen-1)>>FRACBITS); // [crispy] WiggleFix
		
	if (dc_yh >= mfloorclip[dc_x])
	    dc_yh = mfloorclip[dc_x]-1;
	if (dc_yl <= mceilingclip[dc_x])
	    dc_yl = mceilingclip[dc_x]+1;

	if (dc_yl <= dc_yh)
	{
	    dc_source = (byte *)column + 3;
	    dc_texturemid = basetexturemid - (top<<FRACBITS);
	    // dc_source = (byte *)column + 3 - top;

	    // Drawn by either R_DrawColumn
	    //  or (SHADOW) R_DrawFuzzColumn.
	    colfunc ();	
	}
	column = (column_t *)(  (byte *)column + column->length + 4);
    }
	
    dc_texturemid = basetexturemid;
}



//
// R_DrawVisSprite
//  mfloorclip and mceilingclip should also be set.
//
void
R_DrawVisSprite
( vissprite_t*		vis,
  int			x1,
  int			x2 )
{
    column_t*		column;
    int			texturecolumn;
    fixed_t		frac;
    patch_t*		patch;
	
	
    patch = W_CacheLumpNum (vis->patch+firstspritelump, PU_CACHE);

    // [crispy] brightmaps for select sprites
    dc_colormap[0] = vis->colormap[0];
    dc_colormap[1] = vis->colormap[1];
    dc_brightmap = vis->brightmap;
    
    if (!dc_colormap[0])
    {
	// NULL colormap = shadow draw
	colfunc = fuzzcolfunc;
    }
    else if (vis->mobjflags & MF_TRANSLATION)
    {
	colfunc = transcolfunc;
	dc_translation = translationtables - 256 +
	    ( (vis->mobjflags & MF_TRANSLATION) >> (MF_TRANSSHIFT-8) );
    }
    // [crispy] color-translated sprites (i.e. blood)
    else if (vis->translation)
    {
	colfunc = transcolfunc;
	dc_translation = vis->translation;
    }
    // [crispy] translucent sprites
    else if (crispy->translucency && vis->mobjflags & MF_TRANSLUCENT)
    {
	if (!(vis->mobjflags & (MF_NOGRAVITY | MF_COUNTITEM)) ||
	    (vis->mobjflags & MF_NOGRAVITY && crispy->translucency & TRANSLUCENCY_MISSILE) ||
	    (vis->mobjflags & MF_COUNTITEM && crispy->translucency & TRANSLUCENCY_ITEM))
	{
	    colfunc = tlcolfunc;
	}
#ifdef CRISPY_TRUECOLOR
	blendfunc = vis->blendfunc;
#endif
    }
	
    dc_iscale = abs(vis->xiscale)>>detailshift;
    dc_texturemid = vis->texturemid;
    frac = vis->startfrac;
    spryscale = vis->scale;
    sprtopscreen = centeryfrac - FixedMul(dc_texturemid,spryscale);
	
    for (dc_x=vis->x1 ; dc_x<=vis->x2 ; dc_x++, frac += vis->xiscale)
    {
	static boolean error = false;
	texturecolumn = frac>>FRACBITS;
#ifdef RANGECHECK
	if (texturecolumn < 0 || texturecolumn >= SHORT(patch->width))
	{
	    // [crispy] make non-fatal
	    if (!error)
	    {
	    fprintf (stderr, "R_DrawSpriteRange: bad texturecolumn\n");
	    error = true;
	    }
	    continue;
	}
#endif
	column = (column_t *) ((byte *)patch +
			       LONG(patch->columnofs[texturecolumn]));
	R_DrawMaskedColumn (column);
    }

    colfunc = basecolfunc;
#ifdef CRISPY_TRUECOLOR
    blendfunc = I_BlendOver;
#endif
}



//
// R_ProjectSprite
// Generates a vissprite for a thing
//  if it might be visible.
//
void R_ProjectSprite (mobj_t* thing)
{
    fixed_t		tr_x;
    fixed_t		tr_y;
    
    fixed_t		gxt;
    fixed_t		gyt;
    fixed_t		gzt; // [JN] killough 3/27/98
    
    fixed_t		tx;
    fixed_t		tz;

    fixed_t		xscale;
    
    int			x1;
    int			x2;

    spritedef_t*	sprdef;
    spriteframe_t*	sprframe;
    int			lump;
    
    unsigned		rot;
    boolean		flip;
    
    int			index;

    vissprite_t*	vis;
    
    angle_t		ang;
    fixed_t		iscale;
    
    fixed_t             interpx;
    fixed_t             interpy;
    fixed_t             interpz;
    fixed_t             interpangle;

    // [AM] Interpolate between current and last position,
    //      if prudent.
    if (crispy->uncapped &&
        // Don't interpolate if the mobj did something
        // that would necessitate turning it off for a tic.
        thing->interp == true &&
        // Don't interpolate during a paused state.
        leveltime > oldleveltime)
    {
        interpx = thing->oldx + FixedMul(thing->x - thing->oldx, fractionaltic);
        interpy = thing->oldy + FixedMul(thing->y - thing->oldy, fractionaltic);
        interpz = thing->oldz + FixedMul(thing->z - thing->oldz, fractionaltic);
        interpangle = R_InterpolateAngle(thing->oldangle, thing->angle, fractionaltic);
    }
    else
    {
        interpx = thing->x;
        interpy = thing->y;
        interpz = thing->z;
        interpangle = thing->angle;
    }

    // transform the origin point
    tr_x = interpx - viewx;
    tr_y = interpy - viewy;
	
    gxt = FixedMul(tr_x,viewcos); 
    gyt = -FixedMul(tr_y,viewsin);
    
    tz = gxt-gyt; 

    // thing is behind view plane?
    if (tz < MINZ)
	return;
    
    xscale = FixedDiv(projection, tz);
	
    gxt = -FixedMul(tr_x,viewsin); 
    gyt = FixedMul(tr_y,viewcos); 
    tx = -(gyt+gxt); 

    // too far off the side?
    if (abs(tx)>(tz<<2))
	return;
    
    // decide which patch to use for sprite relative to player
#ifdef RANGECHECK
    if ((unsigned int) thing->sprite >= (unsigned int) numsprites)
	I_Error ("R_ProjectSprite: invalid sprite number %i ",
		 thing->sprite);
#endif
    sprdef = &sprites[thing->sprite];
    // [crispy] the TNT1 sprite is not supposed to be rendered anyway
    if (!sprdef->numframes && thing->sprite == SPR_TNT1)
    {
	return;
    }
#ifdef RANGECHECK
    if ( (thing->frame&FF_FRAMEMASK) >= sprdef->numframes )
	I_Error ("R_ProjectSprite: invalid sprite frame %i : %i ",
		 thing->sprite, thing->frame);
#endif
    sprframe = &sprdef->spriteframes[ thing->frame & FF_FRAMEMASK];

    if (sprframe->rotate)
    {
	// choose a different rotation based on player view
	ang = R_PointToAngle (interpx, interpy);
	// [crispy] now made non-fatal
	if (sprframe->rotate == -1)
	{
	    return;
	}
	else
	// [crispy] support 16 sprite rotations
	if (sprframe->rotate == 2)
	{
	    const unsigned rot2 = (ang-interpangle+(unsigned)(ANG45/4)*17);
	    rot = (rot2>>29) + ((rot2>>25)&8);
	}
	else
	{
	rot = (ang-interpangle+(unsigned)(ANG45/2)*9)>>29;
	}
	lump = sprframe->lump[rot];
	flip = (boolean)sprframe->flip[rot];
    }
    else
    {
	// use single rotation for all views
	lump = sprframe->lump[0];
	flip = (boolean)sprframe->flip[0];
    }

    // [crispy] randomly flip corpse, blood and death animation sprites
    if (crispy->flipcorpses &&
        (thing->flags & MF_FLIPPABLE) &&
        !(thing->flags & MF_SHOOTABLE) &&
        (thing->health & 1))
    {
	flip = !flip;
    }
    
    // calculate edges of the shape
    // [crispy] fix sprite offsets for mirrored sprites
    tx -= flip ? spritewidth[lump] - spriteoffset[lump] : spriteoffset[lump];
    x1 = (centerxfrac + FixedMul (tx,xscale) ) >>FRACBITS;

    // off the right side?
    if (x1 > viewwidth)
	return;
    
    tx +=  spritewidth[lump];
    x2 = ((centerxfrac + FixedMul (tx,xscale) ) >>FRACBITS) - 1;

    // off the left side
    if (x2 < 0)
	return;
    
    // [JN] killough 4/9/98: clip things which are out of view due to height
    gzt = interpz + spritetopoffset[lump];
    if (interpz > viewz + FixedDiv(viewheight << FRACBITS, xscale) ||
        gzt < (int64_t)viewz - FixedDiv((viewheight << FRACBITS)-viewheight, xscale))
    {
	return;
    }

    // [JN] quickly reject sprites with bad x ranges
    if (x1 >= x2)
    {
	return;
    }

    // store information in a vissprite
    vis = R_NewVisSprite ();
    vis->translation = NULL; // [crispy] no color translation
    vis->mobjflags = thing->flags;
    vis->scale = xscale<<detailshift;
    vis->gx = interpx;
    vis->gy = interpy;
    vis->gz = interpz;
    vis->gzt = gzt; // [JN] killough 3/27/98
    vis->texturemid = gzt - viewz;
    vis->x1 = x1 < 0 ? 0 : x1;
    vis->x2 = x2 >= viewwidth ? viewwidth-1 : x2;	
    iscale = FixedDiv (FRACUNIT, xscale);

    if (flip)
    {
	vis->startfrac = spritewidth[lump]-1;
	vis->xiscale = -iscale;
    }
    else
    {
	vis->startfrac = 0;
	vis->xiscale = iscale;
    }

    if (vis->x1 > x1)
	vis->startfrac += vis->xiscale*(vis->x1-x1);
    vis->patch = lump;
    
    // get light level
    if (thing->flags & MF_SHADOW)
    {
	// shadow draw
	vis->colormap[0] = vis->colormap[1] = NULL;
    }
    else if (fixedcolormap)
    {
	// fixed map
	vis->colormap[0] = vis->colormap[1] = fixedcolormap;
    }
    else if (thing->frame & FF_FULLBRIGHT)
    {
	// full bright
	vis->colormap[0] = vis->colormap[1] = colormaps;
    }
    
    else
    {
	// diminished light
	index = xscale>>(LIGHTSCALESHIFT-detailshift+crispy->hires);

	if (index >= MAXLIGHTSCALE) 
	    index = MAXLIGHTSCALE-1;

	// [crispy] brightmaps for select sprites
	vis->colormap[0] = spritelights[index];
	vis->colormap[1] = colormaps;
    }	
    vis->brightmap = R_BrightmapForSprite(thing->sprite);

    // [crispy] colored blood
    if ((crispy->coloredblood >= 2) &&
        (thing->type == MT_BLOOD || thing->state - states == S_GIBS) &&
        thing->target)
    {
	// [crispy] Thorn Things in Hacx bleed green blood
	if (gamemission == pack_hacx)
	{
	    if (thing->target->type == MT_BABY)
	    {
		vis->translation = cr[CR_RED2GREEN];
	    }
	}
	else
	{
	    // [crispy] Barons of Hell and Hell Knights bleed green blood
	    if (thing->target->type == MT_BRUISER || thing->target->type == MT_KNIGHT)
	    {
		vis->translation = cr[CR_RED2GREEN];
	    }
	    else
	    // [crispy] Cacodemons bleed blue blood
	    if (thing->target->type == MT_HEAD)
	    {
		vis->translation = cr[CR_RED2BLUE];
	    }
	}
    }

#ifdef CRISPY_TRUECOLOR
    // [crispy] translucent sprites
    if (thing->flags & MF_TRANSLUCENT)
    {
	vis->blendfunc = (thing->frame & FF_FULLBRIGHT) ? I_BlendAdd : I_BlendOver;
    }
#endif
}

extern void P_LineLaser (mobj_t* t1, angle_t angle, fixed_t distance, fixed_t slope);

byte *R_LaserspotColor (void)
{
	if (crispy->crosshairtarget)
	{
		// [crispy] the projected crosshair code calls P_LineLaser() itself
		if (crispy->crosshair == CROSSHAIR_STATIC)
		{
			P_LineLaser(viewplayer->mo, viewangle,
			            16*64*FRACUNIT, PLAYER_SLOPE(viewplayer));
		}
		if (linetarget)
		{
			return cr[CR_GRAY];
		}
	}

	// [crispy] keep in sync with st_stuff.c:ST_WidgetColor(hudcolor_health)
	if (crispy->crosshairhealth)
	{
		const int health = viewplayer->health;

		// [crispy] Invulnerability powerup and God Mode cheat turn Health values gray
		if (viewplayer->cheats & CF_GODMODE ||
		    viewplayer->powers[pw_invulnerability])
			return cr[CR_GRAY];
		else if (health < 25)
			return cr[CR_RED];
		else if (health < 50)
			return cr[CR_GOLD];
		else if (health <= 100)
			return cr[CR_GREEN];
		else
			return cr[CR_BLUE];
	}

	return NULL;
}

// [crispy] generate a vissprite for the laser spot
static void R_DrawLSprite (void)
{
    fixed_t		xscale;
    fixed_t		tx, tz;
    vissprite_t*	vis;

    static int		lump;
    static patch_t*	patch;

    if (weaponinfo[viewplayer->readyweapon].ammo == am_noammo ||
        viewplayer->playerstate != PST_LIVE)
	return;

    if (lump != laserpatch[crispy->crosshairtype].l)
    {
	lump = laserpatch[crispy->crosshairtype].l;
	patch = W_CacheLumpNum(lump, PU_STATIC);
    }

    P_LineLaser(viewplayer->mo, viewangle,
                16*64*FRACUNIT, PLAYER_SLOPE(viewplayer));

    if (!laserspot->thinker.function.acv)
	return;

    tz = FixedMul(laserspot->x - viewx, viewcos) +
         FixedMul(laserspot->y - viewy, viewsin);

    if (tz < MINZ)
	return;

    xscale = FixedDiv(projection, tz);
    // [crispy] the original patch has 5x5 pixels, cap the projection at 20x20
    xscale = (xscale > 4*FRACUNIT) ? 4*FRACUNIT : xscale;

    tx = -(FixedMul(laserspot->y - viewy, viewcos) -
           FixedMul(laserspot->x - viewx, viewsin));

    if (abs(tx) > (tz<<2))
	return;

    vis = R_NewVisSprite();
    memset(vis, 0, sizeof(*vis)); // [crispy] set all fields to NULL, except ...
    vis->patch = lump - firstspritelump; // [crispy] not a sprite patch
    vis->colormap[0] = vis->colormap[1] = fixedcolormap ? fixedcolormap : colormaps; // [crispy] always full brightness
    vis->brightmap = dc_brightmap;
    vis->translation = R_LaserspotColor();
#ifdef CRISPY_TRUECOLOR
    vis->mobjflags |= MF_TRANSLUCENT;
    vis->blendfunc = I_BlendAdd;
#endif
    vis->xiscale = FixedDiv (FRACUNIT, xscale);
    vis->texturemid = laserspot->z - viewz;
    vis->scale = xscale<<detailshift;

    tx -= SHORT(patch->width/2)<<FRACBITS;
    vis->x1 =  (centerxfrac + FixedMul(tx, xscale))>>FRACBITS;
    tx += SHORT(patch->width)<<FRACBITS;
    vis->x2 = ((centerxfrac + FixedMul(tx, xscale))>>FRACBITS) - 1;

    if (vis->x1 < 0 || vis->x1 >= viewwidth ||
        vis->x2 < 0 || vis->x2 >= viewwidth)
	return;

    R_DrawVisSprite (vis, vis->x1, vis->x2);
}



//
// R_AddSprites
// During BSP traversal, this adds sprites by sector.
//
void R_AddSprites (sector_t* sec)
{
    mobj_t*		thing;
    int			lightnum;

    // BSP is traversed by subsector.
    // A sector might have been split into several
    //  subsectors during BSP building.
    // Thus we check whether its already added.
    if (sec->validcount == validcount)
	return;		

    // Well, now it will be done.
    sec->validcount = validcount;
	
    lightnum = (sec->rlightlevel >> LIGHTSEGSHIFT)+(extralight * LIGHTBRIGHT); // [crispy] A11Y

    if (lightnum < 0)		
	spritelights = scalelight[0];
    else if (lightnum >= LIGHTLEVELS)
	spritelights = scalelight[LIGHTLEVELS-1];
    else
	spritelights = scalelight[lightnum];

    // Handle all things in sector.
    for (thing = sec->thinglist ; thing ; thing = thing->snext)
	R_ProjectSprite (thing);
}


//
// R_DrawPSprite
//

boolean pspr_interp = true; // interpolate weapon bobbing

void R_DrawPSprite (pspdef_t* psp, psprnum_t psprnum) // [crispy] differentiate gun from flash sprites
{
    fixed_t		tx;
    int			x1;
    int			x2;
    spritedef_t*	sprdef;
    spriteframe_t*	sprframe;
    int			lump;
    boolean		flip;
    vissprite_t*	vis;
    vissprite_t		avis;
    
    // decide which patch to use
#ifdef RANGECHECK
    if ( (unsigned)psp->state->sprite >= (unsigned int) numsprites)
	I_Error ("R_ProjectSprite: invalid sprite number %i ",
		 psp->state->sprite);
#endif
    sprdef = &sprites[psp->state->sprite];
    // [crispy] the TNT1 sprite is not supposed to be rendered anyway
    if (!sprdef->numframes && psp->state->sprite == SPR_TNT1)
    {
	return;
    }
#ifdef RANGECHECK
    if ( (psp->state->frame & FF_FRAMEMASK)  >= sprdef->numframes)
	I_Error ("R_ProjectSprite: invalid sprite frame %i : %i ",
		 psp->state->sprite, psp->state->frame);
#endif
    sprframe = &sprdef->spriteframes[ psp->state->frame & FF_FRAMEMASK ];

    lump = sprframe->lump[0];
    flip = (boolean)sprframe->flip[0] ^ crispy->flipweapons;
    
    // calculate edges of the shape
    tx = psp->sx2-(ORIGWIDTH/2)*FRACUNIT;
	
    // [crispy] fix sprite offsets for mirrored sprites
    tx -= flip ? 2 * tx - spriteoffset[lump] + spritewidth[lump] : spriteoffset[lump];
    x1 = (centerxfrac + FixedMul (tx,pspritescale) ) >>FRACBITS;

    // off the right side
    if (x1 > viewwidth)
	return;		

    tx +=  spritewidth[lump];
    x2 = ((centerxfrac + FixedMul (tx, pspritescale) ) >>FRACBITS) - 1;

    // off the left side
    if (x2 < 0)
	return;
    
    // store information in a vissprite
    vis = &avis;
    vis->translation = NULL; // [crispy] no color translation
    vis->mobjflags = 0;
    // [crispy] weapons drawn 1 pixel too high when player is idle
    vis->texturemid = (BASEYCENTER<<FRACBITS)+FRACUNIT/4-(psp->sy2+abs(psp->dy)-spritetopoffset[lump]);
    vis->x1 = x1 < 0 ? 0 : x1;
    vis->x2 = x2 >= viewwidth ? viewwidth-1 : x2;	
    vis->scale = pspritescale<<detailshift;
    
    if (flip)
    {
	vis->xiscale = -pspriteiscale;
	vis->startfrac = spritewidth[lump]-1;
    }
    else
    {
	vis->xiscale = pspriteiscale;
	vis->startfrac = 0;
    }
    
    // [crispy] free look
    vis->texturemid += FixedMul(((centery - viewheight / 2) << FRACBITS), pspriteiscale) >> detailshift;

    if (vis->x1 > x1)
	vis->startfrac += vis->xiscale*(vis->x1-x1);

    vis->patch = lump;

    if (viewplayer->powers[pw_invisibility] > 4*32
	|| viewplayer->powers[pw_invisibility] & 8)
    {
	// shadow draw
	vis->colormap[0] = vis->colormap[1] = NULL;
    }
    else if (fixedcolormap)
    {
	// fixed color
	vis->colormap[0] = vis->colormap[1] = fixedcolormap;
    }
    else if (psp->state->frame & FF_FULLBRIGHT)
    {
	// full bright
	vis->colormap[0] = vis->colormap[1] = colormaps;
    }
    else
    {
	// local light
	vis->colormap[0] = spritelights[MAXLIGHTSCALE-1];
	vis->colormap[1] = colormaps;
    }
    vis->brightmap = R_BrightmapForState(psp->state - states);
	
    // [crispy] translucent gun flash sprites
    if (psprnum == ps_flash)
    {
        vis->mobjflags |= MF_TRANSLUCENT;
#ifdef CRISPY_TRUECOLOR
        vis->blendfunc = I_BlendOver; // I_BlendAdd;
#endif
    }

    // interpolate weapon bobbing
    if (crispy->uncapped)
    {
        static int     oldx1, x1_saved;
        static fixed_t oldtexturemid, texturemid_saved;
        static int     oldlump = -1;
        static int     oldgametic = -1;

        if (oldgametic < gametic)
        {
            oldx1 = x1_saved;
            oldtexturemid = texturemid_saved;
            oldgametic = gametic;
        }

        x1_saved = vis->x1;
        texturemid_saved = vis->texturemid;

        if (lump == oldlump && pspr_interp)
        {
            int deltax = vis->x2 - vis->x1;
            vis->x1 = oldx1 + FixedMul(vis->x1 - oldx1, fractionaltic);
            vis->x2 = vis->x1 + deltax;
            vis->x2 = vis->x2 >= viewwidth ? viewwidth - 1 : vis->x2;
            vis->texturemid = oldtexturemid + FixedMul(vis->texturemid - oldtexturemid, fractionaltic);
        }
        else
        {
            oldx1 = vis->x1;
            oldtexturemid = vis->texturemid;
            oldlump = lump;
            pspr_interp = true;
        }
    }

    R_DrawVisSprite (vis, vis->x1, vis->x2);
}


// [crispy] A11Y number of player sprites to draw
int numrpsprites = NUMPSPRITES;

//
// R_DrawPlayerSprites
//
void R_DrawPlayerSprites (void)
{
    int		i;
    int		lightnum;
    pspdef_t*	psp;
    
    // get light level
    lightnum =
	(viewplayer->mo->subsector->sector->rlightlevel >> LIGHTSEGSHIFT) // [crispy] A11Y
	+(extralight * LIGHTBRIGHT);

    if (lightnum < 0)		
	spritelights = scalelight[0];
    else if (lightnum >= LIGHTLEVELS)
	spritelights = scalelight[LIGHTLEVELS-1];
    else
	spritelights = scalelight[lightnum];
    
    // clip to screen bounds
    mfloorclip = screenheightarray;
    mceilingclip = negonearray;
    
    if (crispy->crosshair == CROSSHAIR_PROJECTED)
	R_DrawLSprite();

    // add all active psprites
    for (i=0, psp=viewplayer->psprites;
	 i<numrpsprites; // [crispy] A11Y number of player sprites to draw
	 i++,psp++)
    {
	if (psp->state)
	    R_DrawPSprite (psp, i); // [crispy] pass gun or flash sprite
    }
}

// -----------------------------------------------------------------------------
// R_SortVisSprites
//
// Rewritten by Lee Killough to avoid using unnecessary
// linked lists, and to use faster sorting algorithm.
// -----------------------------------------------------------------------------

#define bcopyp(d, s, n) memcpy(d, s, (n) * sizeof(void *))

// killough 9/2/98: merge sort

static void msort(vissprite_t **s, vissprite_t **t, const int n)
{
    if (n >= 16)
    {
        int n1 = n/2, n2 = n - n1;
        vissprite_t **s1 = s, **s2 = s + n1, **d = t;

        msort(s1, t, n1);
        msort(s2, t, n2);

        while ((*s1)->scale > (*s2)->scale ?
              (*d++ = *s1++, --n1) : (*d++ = *s2++, --n2));

        if (n2)
        bcopyp(d, s2, n2);
        else
        bcopyp(d, s1, n1);

        bcopyp(s, t, n);
    }
    else
    {
        int i;

        for (i = 1; i < n; i++)
        {
            vissprite_t *temp = s[i];

            if (s[i-1]->scale < temp->scale)
            {
                int j = i;

                while ((s[j] = s[j-1])->scale < temp->scale && --j);
                s[j] = temp;
            }
        }
    }
}

// -----------------------------------------------------------------------------
// R_SortVisSprites
// -----------------------------------------------------------------------------

void R_SortVisSprites (void)
{
    if (num_vissprite)
    {
        int i = num_vissprite;

        // If we need to allocate more pointers for the vissprites,
        // allocate as many as were allocated for sprites -- killough
        // killough 9/22/98: allocate twice as many

        if (num_vissprite_ptrs < num_vissprite*2)
        {
            free(vissprite_ptrs);  // better than realloc -- no preserving needed
            vissprite_ptrs = malloc((num_vissprite_ptrs = num_vissprite_alloc*2)
                                    * sizeof *vissprite_ptrs);
        }

        while (--i >= 0)
        {
            vissprite_ptrs[i] = vissprites+i;
        }

        // killough 9/22/98: replace qsort with merge sort, since the keys
        // are roughly in order to begin with, due to BSP rendering.

        msort(vissprite_ptrs, vissprite_ptrs + num_vissprite, num_vissprite);
    }
}

//
// R_DrawSprite
//
void R_DrawSprite (vissprite_t* spr)
{
    drawseg_t*		ds;
    int		clipbot[MAXWIDTH]; // [crispy] 32-bit integer math
    int		cliptop[MAXWIDTH]; // [crispy] 32-bit integer math
    int			x;
    int			r1;
    int			r2;
    fixed_t		scale;
    fixed_t		lowscale;
		
    for (x = spr->x1 ; x<=spr->x2 ; x++)
	clipbot[x] = cliptop[x] = -2;
    
  // [Woof!] e6y: optimization
  if (drawsegs_xrange_size)
  {
    const drawseg_xrange_item_t *last = &drawsegs_xrange[drawsegs_xrange_count - 1];
    drawseg_xrange_item_t *curr = &drawsegs_xrange[-1];
    while (++curr <= last)
    {
      // determine if the drawseg obscures the sprite
      if (curr->x1 > spr->x2 || curr->x2 < spr->x1)
	    continue; // does not cover sprite
			
    ds = curr->user;

	if (ds->scale1 > ds->scale2)
	{
	    lowscale = ds->scale2;
	    scale = ds->scale1;
	}
	else
	{
	    lowscale = ds->scale1;
	    scale = ds->scale2;
	}
		
	if (scale < spr->scale
	    || ( lowscale < spr->scale
		 && !R_PointOnSegSide (spr->gx, spr->gy, ds->curline) ) )
    {
        if (ds->maskedtexturecol)       // masked mid texture?
	    {
          r1 = ds->x1 < spr->x1 ? spr->x1 : ds->x1;
          r2 = ds->x2 > spr->x2 ? spr->x2 : ds->x2;
          R_RenderMaskedSegRange(ds, r1, r2);
	    }
        continue;               // seg is behind sprite
    }

      r1 = ds->x1 < spr->x1 ? spr->x1 : ds->x1;
      r2 = ds->x2 > spr->x2 ? spr->x2 : ds->x2;
	
      // clip this piece of the sprite
      // killough 3/27/98: optimized and made much shorter
      if (ds->silhouette&SIL_BOTTOM && spr->gz < ds->bsilheight) //bottom sil
        for (x=r1 ; x<=r2 ; x++)
          if (clipbot[x] == -2)
            clipbot[x] = ds->sprbottomclip[x];
      if (ds->silhouette&SIL_TOP && spr->gzt > ds->tsilheight)   // top sil
        for (x=r1 ; x<=r2 ; x++)
          if (cliptop[x] == -2)
            cliptop[x] = ds->sprtopclip[x];
    }
    }
    // all clipping has been performed, so draw the sprite

    // check for unclipped columns
    for (x = spr->x1 ; x<=spr->x2 ; x++)
    {
	if (clipbot[x] == -2)		
	    clipbot[x] = viewheight;

	if (cliptop[x] == -2)
	    cliptop[x] = -1;
    }
		
    mfloorclip = clipbot;
    mceilingclip = cliptop;
    R_DrawVisSprite (spr, spr->x1, spr->x2);
}




//
// R_DrawMasked
//
void R_DrawMasked (void)
{
    int				i;
    drawseg_t*		ds;
	
    R_SortVisSprites ();

    // [JN] Andrey Budko
    // Makes sense for scenes with huge amount of drawsegs.
    // ~12% of speed improvement on epic.wad map05
    for (i = 0 ; i < DS_RANGES_COUNT ; i++)
    {
        drawsegs_xranges[i].count = 0;
    }

    if (num_vissprite > 0)
    {
        if (drawsegs_xrange_size < numdrawsegs)
        {
            drawsegs_xrange_size = 2 * numdrawsegs;

            for(i = 0; i < DS_RANGES_COUNT; i++)
            {
                drawsegs_xranges[i].items = I_Realloc(
                drawsegs_xranges[i].items,
                drawsegs_xrange_size * sizeof(drawsegs_xranges[i].items[0]));
            }
        }

        for (ds = ds_p; ds-- > drawsegs;)
        {
            if (ds->silhouette || ds->maskedtexturecol)
            {
                drawsegs_xranges[0].items[drawsegs_xranges[0].count].x1 = ds->x1;
                drawsegs_xranges[0].items[drawsegs_xranges[0].count].x2 = ds->x2;
                drawsegs_xranges[0].items[drawsegs_xranges[0].count].user = ds;

                // [JN] Andrey Budko: ~13% of speed improvement on sunder.wad map10
                if (ds->x1 < centerx)
                {
                    drawsegs_xranges[1].items[drawsegs_xranges[1].count] = 
                    drawsegs_xranges[0].items[drawsegs_xranges[0].count];
                    drawsegs_xranges[1].count++;
                }
                if (ds->x2 >= centerx)
                {
                    drawsegs_xranges[2].items[drawsegs_xranges[2].count] = 
                    drawsegs_xranges[0].items[drawsegs_xranges[0].count];
                    drawsegs_xranges[2].count++;
                }

                drawsegs_xranges[0].count++;
            }
        }
    }

    // draw all vissprites back to front

    for (i = num_vissprite ; --i>=0 ; )
    {
        vissprite_t* spr = vissprite_ptrs[i];

        if (spr->x2 < centerx)
        {
            drawsegs_xrange = drawsegs_xranges[1].items;
            drawsegs_xrange_count = drawsegs_xranges[1].count;
        }
        else if (spr->x1 >= centerx)
        {
            drawsegs_xrange = drawsegs_xranges[2].items;
            drawsegs_xrange_count = drawsegs_xranges[2].count;
        }
        else
        {
            drawsegs_xrange = drawsegs_xranges[0].items;
            drawsegs_xrange_count = drawsegs_xranges[0].count;
        }

        R_DrawSprite(spr);    // [JN] killough
    }
    
    // render any remaining masked mid textures
    for (ds=ds_p-1 ; ds >= drawsegs ; ds--)
	if (ds->maskedtexturecol)
	    R_RenderMaskedSegRange (ds, ds->x1, ds->x2);
    
    if (crispy->cleanscreenshot == 2)
        return;

    // draw the psprites on top of everything
    //  but does not draw on side views
    if (!viewangleoffset)		
	R_DrawPlayerSprites ();
}



