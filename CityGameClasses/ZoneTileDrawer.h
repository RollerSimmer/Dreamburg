#pragma once

#include "zonepatternlist.h"
#include "ZoneTileGhostDrawer.h"

class ZoneTileDrawer
	{
	private:
		ZonePatternList*zonelist;
		ZoneTileGhostDrawer*ghostdraw;
	public:
		ZoneTileDrawer(ZonePatternList*_zonelist,ZoneTileGhostDrawer*_ghostdraw)	;
	};	


