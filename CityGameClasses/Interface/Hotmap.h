#pragma once

#include "MapScroller.h"

class Hotmap
	{
	private:
		MapScroller*scroller;		
	public:
		Hotmap(MapScroller*_scroller)	{	scroller=_scroller;	}
	};


