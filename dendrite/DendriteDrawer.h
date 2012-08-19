#pragma once

#include "dendrite.h"
#include <allegro.h>

class DendriteDrawer
	{
	private:
		BITMAP*page;
		Dendrite*d;
		int scalediv;
	public:
		DendriteDrawer(Dendrite*_d,int _scalediv);
		~DendriteDrawer();

		void Draw(int x,int y);
		void DrawLattice(int x,int y,int color);
	};
