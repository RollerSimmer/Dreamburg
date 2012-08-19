/*

To do:
	+	Speed up CutupImage()!
In Progress:
	+

********** Update Log *******************
2011:Aug:13:PM:03:25
	+	added to ZonePattern a method to generate a simple zone image for testing
	+	...method to cut up image into smaller tile images
2011:Aug:14:PM:11:36
	+	added a function to display spaced tile images
2011:Aug:14:PM:12:49
	+	added a class to ZonePattern::CutupImage() called Cutup
		which adds some methods commonly used within CutupImage.
		My first experiment with subfunctions in C++. Used them
		some in Pascal
2011:Aug:14:PM:04:56
	+	CutupImage() works, but it's pretty slow for real time cutting
		I will have to speed it up some.  put/get pixel refs
		need to be modified, and bounds need to be optimized.
2011:Aug:15:AM:10:45
	+	fixed CutUpImage's progress display method
	+	fixed some loops that compared tx against tl instead of tw
		like it should have
2011:Aug:18:AM:10:17
	+	added occupancy() methods, and fixed their initialization
2011:Aug:18:PM:12:00
	+	now cutup looks at occupancy
2011:Aug:18:PM:05:11
	+	now cutup ignores unoccupied tiles on full cases, need to
		on quad cases
	+	a occupancy bitmap generator was created
2011:Aug:19:PM:07:17
	+	most of the cutup process works as far as i can tell
		need to do more testing
	+	Added DrawWholeImage to zonepattern
2011:Aug:20:PM:06:43
	+	now the triangle cut up seems to work and fill the whole image
		(except for bottom tiles since height goes UP)
	+	messy code, so I may need to plan carefully a
		already getting lost.

*/

#pragma once

#include <iostream>

#include <types/mytypes.h>
#include <allegro.h>
#include <vector>
#include <cstdlib>

#include "tilesize.h"

#include <sstream>

using namespace std;

struct ZonePattern
	{
	//data
	u16 tw:8;
	u16 tl:8;
	u8 nfloors;
	bool usesTriQuads: 1;
	bool fill_02_04:	7;
	char name[13];
	std::vector<BITMAP*> sprites;
	std::vector<u8>	occubytes;
	//methods
	ZonePattern();
	ZonePattern(u8 stw,u8 stl,u8 snfloors,const char*pcxfile
	            ,const char*patname,bool loadpcxfile
	            ,bool usesTriQuads=false);
	~ZonePattern();
	inline short xo(short tx,short ty,short quad)
		{	return ((short)xpixelsPerTile-sprite(tx,ty,quad)->w);	}
	inline short yo(short tx,short ty,short quad)
		{	return ((short)ypixelsPerTile-sprite(tx,ty,quad)->h);	}
	inline long quadi(short tx,short ty,short quad)
		{	return (((long)ty*tw+tx)*(long)ImgsPerTile()+quad%4);	}
	inline BITMAP*sprite(short tx,short ty,short quad)
		{	return (sprites[quadi(tx,ty,quad)]);	}
	inline void sprite(short tx,short ty,short quad,BITMAP*spr)
		{	sprites[quadi(tx,ty,quad)]=spr;	}
	inline short ImgsPerTile()	{	return(usesTriQuads?4:1);	}
	inline short nquads()	{	return(ImgsPerTile());	}
	inline short nsprites()	{	return (ImgsPerTile()*tw*tl);	}
	static u32 colorscale(u32 color,u16 mulv,u16 divv);
	BITMAP*GenerateOccupancyImage(u32 fgc,u32 bgc,u16 tilepixsz);
	BITMAP*GenerateBaseImage(u32 fgc,u32 bgc,u32 txtc);
	void CutupImage(BITMAP*baseimg);
	void DrawWholeZone(BITMAP*page,short xo,short yo);
	void DrawEvenlySpacedTiles(BITMAP*page,short xo,short yo
	                           ,u16 pad,bool transparent);
	void SetFullOccupancy();
	void SetRandOccupancy(u16 truewgt,u16 falsewgt);
	void occupied(short tx,short ty,short tq,bool state);
	bool occupied(short tx,short ty,short tq);
	};

////extern ZonePatternList*patterns;
