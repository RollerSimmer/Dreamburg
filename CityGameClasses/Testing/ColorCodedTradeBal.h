#pragma once

#include "../Shires.h"
#include "../OOAllegro/OOAllegroPage.h"

class ColorCodedTradeBal
	{
	private:
		Shire*shire;
		OOAllegroPage*oopage;
	public:
		//initialization:
			ColorCodedTradeBal(Shire*shire);
			~ColorCodedTradeBal();
		//display:
			void DrawTradeBalances();
			void DrawTradeBalanceAt(int wx,int wy,int item);
			int getbalcolor(int wx,int wy,int item);
			int getbalcolor(long balance);
	};


