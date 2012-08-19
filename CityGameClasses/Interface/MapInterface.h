#pragma once

#include "MapPlopper.h"
#include "MapZoomer.h"
#include "MapScroller.h"

class MapInterface
	{
	private:
		MapPlopper*plopper;
		MapZoomer*zoomer;
		MapScroller*scroller;
	public:
		MapInterface(MapPlopper*_plopper,MapZoomer*_zoomer,MapScroller*_scroller);
	};


