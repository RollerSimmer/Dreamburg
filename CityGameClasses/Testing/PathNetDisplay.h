#pragma once

#include "../Shires.h"
#include "../OOAllegro/OOAllegroPage.h"

class PathNetDisplay
	{
	private:
		Shire*shire;
		OOAllegroPage*oopage;
	public:
		//initialization:
			PathNetDisplay(Shire*_shire);
			~PathNetDisplay();
		//display:
			void DrawPathNets();
			int GetPathNetColor(int wx,int wy);
			int GetPathNetColor(int pathnet);
	};


