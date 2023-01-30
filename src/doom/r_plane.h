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
//	Refresh, visplane stuff (floor, ceilings).
//


#ifndef __R_PLANE__
#define __R_PLANE__


#include "r_data.h"


#define PL_SKYFLAT (0x80000000)

// Visplane related.
extern  int*		lastopening; // [crispy] 32-bit integer math


// typedef void (*planefunction_t) (int top, int bottom); //unused

// extern planefunction_t	floorfunc; //unused
// extern planefunction_t	ceilingfunc_t; //unused

extern int*		floorclip;
extern int*		ceilingclip;

extern fixed_t*	yslope;
extern fixed_t		yslopes[LOOKDIRS][MAXHEIGHT];
extern fixed_t*	distscale;

void R_InitPlanesRes (void);
void R_InitVisplanesRes (void);
void R_DrawPlanes (void);
void R_ClearPlanes (void);

void R_MapPlane
( int		y,
  int		x1,
  int		x2 );

void R_MakeSpans
( const int		x,
  unsigned int		t1, // [crispy] 32-bit integer math
  unsigned int		b1, // [crispy] 32-bit integer math
  unsigned int		t2, // [crispy] 32-bit integer math
  unsigned int		b2 ); // [crispy] 32-bit integer math

visplane_t*
R_FindPlane
( fixed_t	height,
  const int		picnum,
  const int		lightlevel );

visplane_t*
R_DupPlane
( visplane_t*	pl,
  int		start,
  int		stop );

visplane_t*
R_CheckPlane
( visplane_t*	pl,
  int		start,
  int		stop );



#endif
