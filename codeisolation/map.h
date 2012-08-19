#pragma once

#include <types/mytypes.h>

#include "zonepatternlist.h"
#include "mappatternindex.h"

////const u16 mapsz=1024;
const u16 maxpatterns=16;

namespace Map
	{
	const u16 maxstack=8;

	struct Data
		{
		u16 tw,tl;
		u16 npatterns();
		ZonePatternList*zonelist;
		MapPatternIndex::Struct map[mapsz][mapsz][maxstack];
		Data(u16 stw,u16 stl);
		~Data();
		void AddZone(short tx,short ty,u16 patidx,u16 color1
		             ,u16 color2,u16 color3);
		void DrawMap(BITMAP*page,short tltx,short tlty);
		void AddRandPattern(u16 maxsz,u16 maxfloors
								  ,const char*patname,u32 color);
		};
	}

