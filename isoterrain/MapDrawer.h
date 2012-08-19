#pragma once

#include "../intvector2d/intvector2d.h"
#include "IsoTerrain.h"
#include <types/mytypes.h>
#include <allegro.h>
#include <algorithm>

using namespace std;

const int maxz=100;

class MapDrawer
	{
	protected:
		IsoTerrain*map;
		BITMAP*page;
		int ox,oy;
		int tiledim;
	public:
		MapDrawer(IsoTerrain*_map)	;
		~MapDrawer()	{}
		inline void SetOrigin(int _ox,int _oy)	{	ox=_ox;	oy=_oy;	}
		void SetPage(BITMAP*_page)	{	page=_page;	}
		BITMAP*GetPage()	{	return page;	}
		void DrawMap(int ox,int oy);
			void ClearPage();
			void DrawTile(int x,int y);
			void DrawMiniMap(int sx,int sy,int ptstep,int dotsz);
			void DrawSkylines(int sx,int sy,int htmul,int htdiv);
			int CalcSurfaceShade(int x,int y,int gray_lighted,int gray_shadowed,int basecolors[]);
			int CalcHeightShade(int x,int y);
		int CalcPageWidth();
		int CalcPageHeight();
		int projx(int x,int y);
		int projy(int x,int y);
		intvector2d proj(int x,int y);
		//zoom
			void ZoomIn();
			void ZoomOut();
		//one-liners
			int zunit()	{	return max(tiledim/4,1);	}
			intvector2d xisoUnit()	{	return intvector2d(2*tiledim,tiledim);	}
			intvector2d yisoUnit()	{	return intvector2d(-2*tiledim,tiledim);	}
			intvector2d zisoUnit()	{	return intvector2d(0,-zunit());	}
			int tilewidth()	{	return xisoUnit().GetX()*2;	};
			int tileheightHalf()	{	return xisoUnit().GetY();	};
	};

