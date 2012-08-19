
#pragma once

#include "TerrainEroder.h"

class FlowFieldDrawer
	{
	protected:
		TerrainEroder*eroder;
	public:
		FlowFieldDrawer(TerrainEroder*_eroder)	:	eroder(_eroder)	{}
		void Draw();
			void DrawTile(int x,int y);
			void KeyPause();
	};
