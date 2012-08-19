#pragma once

#include "map.h"

class MapDrawer
	{
	private:
		Map::Data*map;		
	public:
		MapDrawer(Map::Data*_map)	{	map=_map;	}		
	};


