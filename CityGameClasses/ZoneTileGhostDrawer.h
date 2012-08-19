#pragma once

#include "zonepatternlist.h"
#include "SpriteGhoster.h"

class ZoneTileGhostDrawer
	{
	private:
		ZonePatternList*zonelist;
		SpriteGhoster*sprghoster;
	public:
		ZoneTileGhostDrawer(ZonePatternList*_zonelist,SpriteGhoster*_sprghoster);
	};	


