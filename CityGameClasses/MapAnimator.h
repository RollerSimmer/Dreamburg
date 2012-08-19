#pragma once

#include "map.h"

class MapAnimator
	{
	private:
		Map::Data*map;		
	public:
		MapAnimator(Map::Data*_map)	{	map=_map;	}		
	};


