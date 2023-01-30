
//
// Copyright(C) 1993-1996 Id Software, Inc.
// Copyright(C) 2005-2014 Simon Howard
// Copyright(C) 2016-2023 Julian Nechaevsky
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
//	Here is a core component: drawing the floors and ceilings,
//	 while maintaining a per column clipping list only.
//	Moreover, the sky areas have to be determined.
//


#include <stdio.h>
#include <stdlib.h>

#include "i_system.h"
#include "z_zone.h"
#include "w_wad.h"

#include "doomdef.h"
#include "doomstat.h"

#include "r_local.h"
#include "r_sky.h"
#include "r_bmaps.h" // [crispy] R_BrightmapForTexName()
#include "r_swirl.h" // [crispy] R_DistortedFlat()



//planefunction_t		floorfunc; //unused
//planefunction_t		ceilingfunc; //unused

//
// opening
//

// -----------------------------------------------------------------------------
// MAXVISPLANES is no longer a limit on the number of visplanes,
// but a limit on the number of hash slots; larger numbers mean
// better performance usually but after a point they are wasted,
// and memory and time overheads creep in.
//
// Lee Killough
// -----------------------------------------------------------------------------

#define MAXVISPLANES	128                  // must be a power of 2

static visplane_t *visplanes[MAXVISPLANES];  // [JN] killough
static visplane_t *freetail;                 // [JN] killough
static visplane_t **freehead = &freetail;    // [JN] killough
visplane_t *floorplane, *ceilingplane;

// [JN] killough -- hash function for visplanes
// Empirically verified to be fairly uniform:

#define visplane_hash(picnum, lightlevel, height) \
    ((unsigned)((picnum) * 3 + (lightlevel) + (height) * 7) & (MAXVISPLANES - 1))

// [JN] killough 8/1/98: set static number of openings to be large enough
// (a static limit is okay in this case and avoids difficulties in r_segs.c)

size_t maxopenings;
int *openings, *lastopening; // [crispy] 32-bit integer math

//
// Clip values are the solid pixel bounding the range.
//  floorclip starts out SCREENHEIGHT
//  ceilingclip starts out -1
//

// [JN] Andrey Budko: resolution limitation is removed
int *floorclip = NULL;    // dropoff overflow
int *ceilingclip = NULL;  // dropoff overflow

//
// spanstart holds the start of a plane span
// initialized to 0 at start
//

// [JN] Andrey Budko: resolution limitation is removed
static int *spanstart = NULL;  // killough 2/8/98


//
// texture mapping
//

static lighttable_t **planezlight;
static fixed_t planeheight;
static fixed_t cachedheight[MAXHEIGHT];
static fixed_t cacheddistance[MAXHEIGHT];
static fixed_t cachedxstep[MAXHEIGHT];
static fixed_t cachedystep[MAXHEIGHT];

// [JN] Andrey Budko: resolution limitation is removed
fixed_t *yslope = NULL;
fixed_t *distscale = NULL;
fixed_t  yslopes[LOOKDIRS][MAXHEIGHT];


// -----------------------------------------------------------------------------
// R_InitPlanesRes
// -----------------------------------------------------------------------------

void R_InitPlanesRes (void)
{
    if (floorclip)
    {
        free(floorclip);
    }
    if (ceilingclip)
    {
        free(ceilingclip);
    }
    if (spanstart)
    {
        free(spanstart);
    }
    if (yslope)
    {
        free(yslope);
    }
    if (distscale)
    {
        free(distscale);
    }

    floorclip = calloc(1, MAXWIDTH * sizeof(*floorclip));
    ceilingclip = calloc(1, MAXWIDTH * sizeof(*ceilingclip));
    spanstart = calloc(1, MAXWIDTH * sizeof(*spanstart));
    yslope = calloc(1, MAXWIDTH * sizeof(*yslope));
    distscale = calloc(1, MAXWIDTH * sizeof(*distscale));
}

// -----------------------------------------------------------------------------
// R_InitPlanesRes
// -----------------------------------------------------------------------------

void R_InitVisplanesRes (void)
{
    freetail = NULL;
    freehead = &freetail;

    for (int i = 0; i < MAXVISPLANES; i++)
    {
        visplanes[i] = 0;
    }
}

// -----------------------------------------------------------------------------
// R_MapPlane
//
// Uses global vars:
//  - planeheight
//  - ds_source
//  - viewx
//  - viewy
//
// BASIC PRIMITIVE
// -----------------------------------------------------------------------------

void R_MapPlane (int y, int x1, int x2)
{
    unsigned index;
    int      dx, dy;
    fixed_t	 distance;

#ifdef RANGECHECK
    if (x2 < x1 || x1 < 0 || x2 >= viewwidth || y > viewheight)
    {
        I_Error ("R_MapPlane: %i, %i at %i",x1,x2,y);
    }
#endif

    // [crispy] visplanes with the same flats now match up far better than before
    // adapted from prboom-plus/src/r_plane.c:191-239, translated to fixed-point math
    if (!(dy = abs(centery - y)))
    {
        return;
    }

    if (planeheight != cachedheight[y])
    {
        cachedheight[y] = planeheight;
        distance = cacheddistance[y] = FixedMul (planeheight, yslope[y]);
        ds_xstep = cachedxstep[y] = FixedMul (viewsin, planeheight) / dy;
        ds_ystep = cachedystep[y] = FixedMul (viewcos, planeheight) / dy;
    }
    else
    {
        distance = cacheddistance[y];
        ds_xstep = cachedxstep[y];
        ds_ystep = cachedystep[y];
    }

    dx = x1 - centerx;

    ds_xfrac = viewx + FixedMul(viewcos, distance) + dx * ds_xstep;
    ds_yfrac = -viewy - FixedMul(viewsin, distance) + dx * ds_ystep;

    if (fixedcolormap)
    {
        ds_colormap[0] = ds_colormap[1] = fixedcolormap;
    }
    else
    {
        // [JN] Note: no smoother diminished lighting in -vanilla mode
        index = distance >> LIGHTZSHIFT;

        if (index >= MAXLIGHTZ)
            index = MAXLIGHTZ-1;

        ds_colormap[0] = planezlight[index];
        ds_colormap[1] = colormaps;
    }

    ds_y = y;
    ds_x1 = x1;
    ds_x2 = x2;

    // high or low detail
    spanfunc ();	
}

// -----------------------------------------------------------------------------
// R_ClearPlanes
// At begining of frame.
// -----------------------------------------------------------------------------

void R_ClearPlanes (void)
{
    int i;

    // opening / clipping determination
    for (i = 0 ; i < viewwidth ; i++)
    {
        floorclip[i] = viewheight;
        ceilingclip[i] = -1;
    }

    for (i = 0; i < MAXVISPLANES; i++)  // [JN] new code -- killough
        for (*freehead = visplanes[i], visplanes[i] = NULL ; *freehead ; )
            freehead = &(*freehead)->next;

    lastopening = openings;

    // texture calculation
    memset (cachedheight, 0, sizeof(cachedheight));
}

// -----------------------------------------------------------------------------
// [crispy] remove MAXVISPLANES Vanilla limit
// New function, by Lee Killough
// -----------------------------------------------------------------------------

static visplane_t *new_visplane (unsigned const int hash)
{
    visplane_t *check = freetail;

    if (!check)
    {
        check = calloc(1, sizeof(*check));
    }
    else if (!(freetail = freetail->next))
    {
        freehead = &freetail;
    }

    check->next = visplanes[hash];
    visplanes[hash] = check;

    return check;
}

// -----------------------------------------------------------------------------
// R_FindPlane
// -----------------------------------------------------------------------------

visplane_t *R_FindPlane (fixed_t height, int picnum, int lightlevel)
{
    visplane_t *check;
    unsigned int hash;

    if (picnum == skyflatnum)
    {
        height = 0; // all skys map together
        // [JN] Don't modify lightlevel parameter of visplane with sky texture.
        // Otherwise hash function will consider it as unique one, forcing 
        // creation of new visplane.
        // lightlevel = 0;
    }

    // New visplane algorithm uses hash table -- killough
    hash = visplane_hash(picnum, lightlevel, height);

    for (check = visplanes[hash]; check; check = check->next)
        if (height == check->height && picnum == check->picnum 
        && lightlevel == check->lightlevel)
            return check;

    check = new_visplane(hash);

    check->height = height;
    check->picnum = picnum;
    check->lightlevel = lightlevel;
    check->minx = SCREENWIDTH;
    check->maxx = -1;

    memset(check->top, UINT_MAX, sizeof(check->top));

    return check;
}

// -----------------------------------------------------------------------------
// R_DupPlane
// -----------------------------------------------------------------------------

visplane_t *R_DupPlane(visplane_t *pl, int start, int stop)
{
    visplane_t  *new_pl = new_visplane(visplane_hash(pl->picnum, pl->lightlevel, pl->height));

    new_pl->height = pl->height;
    new_pl->picnum = pl->picnum;
    new_pl->lightlevel = pl->lightlevel;
    new_pl->minx = start;
    new_pl->maxx = stop;

    memset(new_pl->top, UINT_MAX, sizeof(new_pl->top));

    return new_pl;
}

// -----------------------------------------------------------------------------
// R_CheckPlane
// -----------------------------------------------------------------------------

visplane_t *R_CheckPlane (visplane_t *pl, int start, int stop)
{
    int intrl, intrh, unionl, unionh, x;

    if (start < pl->minx)
    {
        intrl = pl->minx, unionl = start;
    }
    else
    {
        unionl = pl->minx, intrl = start;
    }

    if (stop  > pl->maxx)
    {
        intrh = pl->maxx, unionh = stop;
    }
    else
    {
        unionh = pl->maxx, intrh  = stop;
    }

    for (x=intrl ; x <= intrh && pl->top[x] == UINT_MAX; x++); // [crispy] hires / 32-bit integer math
    if (x > intrh)
    {
        // Can use existing plane; extend range
        pl->minx = unionl, pl->maxx = unionh;
        return pl;
    }
    else
    {
        // Cannot use existing plane; create a new one
        return R_DupPlane(pl, start, stop);
    }
}

// -----------------------------------------------------------------------------
// R_MakeSpans
// -----------------------------------------------------------------------------

void R_MakeSpans (const int x, unsigned int t1, unsigned int b1, // [crispy] 32-bit integer math
                          unsigned int t2, unsigned int b2) // [crispy] 32-bit integer math
{
    for ( ; t1 < t2 && t1 <= b1 ; t1++)
    {
        R_MapPlane(t1, spanstart[t1], x-1);
    }
    for ( ; b1 > b2 && b1 >= t1 ; b1--)
    {
        R_MapPlane(b1, spanstart[b1], x-1);
    }
    while (t2 < t1 && t2 <= b2)
    {
        spanstart[t2++] = x;
    }
    while (b2 > b1 && b2 >= t2)
    {
        spanstart[b2--] = x;
    }
}

// -----------------------------------------------------------------------------
// R_DrawPlanes
// At the end of each frame.
// -----------------------------------------------------------------------------

void R_DrawPlanes (void) 
{
    int			light;
    int			x;
    int			stop;
    int			angle;
    int         lumpnum;
    boolean swirling;

    for (int i = 0 ; i < MAXVISPLANES ; i++)
    for (visplane_t *pl = visplanes[i] ; pl ; pl = pl->next)
    if (pl->minx <= pl->maxx)
    {
        // sky flat
        // [crispy] add support for MBF sky tranfers
	if (pl->picnum == skyflatnum || pl->picnum & PL_SKYFLAT)
	{
	    int texture;
	    angle_t an = viewangle, flip;
	    if (pl->picnum & PL_SKYFLAT)
	    {
		const line_t *l = &lines[pl->picnum & ~PL_SKYFLAT];
		const side_t *s = *l->sidenum + sides;
		texture = texturetranslation[s->toptexture];
		dc_texturemid = s->rowoffset - 28*FRACUNIT;
		// [crispy] stretch sky
		if (crispy->stretchsky)
		{
		    dc_texturemid = dc_texturemid * (textureheight[texture]>>FRACBITS) / SKYSTRETCH_HEIGHT;
		}
		flip = (l->special == 272) ? 0u : ~0u;
		an += s->textureoffset;
	    }
        else
	    {
		texture = skytexture;
		dc_texturemid = skytexturemid;
		flip = 0;
	    }
	    dc_iscale = pspriteiscale>>detailshift;
	    
	    // Sky is allways drawn full bright,
	    //  i.e. colormaps[0] is used.
	    // Because of this hack, sky is not affected
	    //  by INVUL inverse mapping.
	    // [crispy] no brightmaps for sky
	    dc_colormap[0] = dc_colormap[1] = colormaps;
//	    dc_texturemid = skytexturemid;
	    dc_texheight = textureheight[texture]>>FRACBITS; // [crispy] Tutti-Frutti fix
	    // [crispy] stretch sky
	    if (crispy->stretchsky)
	        dc_iscale = dc_iscale * dc_texheight / SKYSTRETCH_HEIGHT;
	    for (x=pl->minx ; x <= pl->maxx ; x++)
	    {
		dc_yl = pl->top[x];
		dc_yh = pl->bottom[x];

		if ((unsigned) dc_yl <= dc_yh) // [crispy] 32-bit integer math
		{
		    angle = ((an + xtoviewangle[x])^flip)>>ANGLETOSKYSHIFT;
		    dc_x = x;
		    dc_source = R_GetColumn(texture, angle);
		    colfunc ();
		}
	    }
	    continue;
	}
	swirling = (flattranslation[pl->picnum] == -1);
	// regular flat
        lumpnum = firstflat + (swirling ? pl->picnum : flattranslation[pl->picnum]);
	// [crispy] add support for SMMU swirling flats
	ds_source = swirling ? R_DistortedFlat(lumpnum) : W_CacheLumpNum(lumpnum, PU_STATIC);
	ds_brightmap = R_BrightmapForFlatNum(lumpnum-firstflat);
	
	planeheight = abs(pl->height-viewz);
	light = (pl->lightlevel >> LIGHTSEGSHIFT)+(extralight * LIGHTBRIGHT);

	if (light >= LIGHTLEVELS)
	    light = LIGHTLEVELS-1;

	if (light < 0)
	    light = 0;

	planezlight = zlight[light];

	pl->top[pl->maxx+1] = 0xffffffffu; // [crispy] hires / 32-bit integer math
	pl->top[pl->minx-1] = 0xffffffffu; // [crispy] hires / 32-bit integer math
		
	stop = pl->maxx + 1;

	for (x=pl->minx ; x<= stop ; x++)
	{
	    R_MakeSpans(x,pl->top[x-1],
			pl->bottom[x-1],
			pl->top[x],
			pl->bottom[x]);
	}
	
    W_ReleaseLumpNum(lumpnum);

    }
}

