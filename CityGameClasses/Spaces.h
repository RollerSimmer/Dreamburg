#pragma once

#include "TradeItems.h"
#include "zonepatternlist.h"

class Space
	{
	private:
		u16 x,y;	//top-left x and y, in tiles
		u16 w,l;	//width and length, in tiles
		TradeItem_SpaceBalance_Array tradebal;
	public:
		//accessors:
			inline u16&getx()	{	return x;	}
			inline u16&gety()	{	return y;	}
			inline u16&getw()	{	return w;	}
			inline u16&getl()	{	return l;	}
			TradeItem_SpaceBalance*gettradebal(int item);
		//constructor
			Space(u16 x,u16 y,u16 w,u16 l);
		//methods:
			bool InSpace(int tx,int ty)	{	return x>=0&&y>=0&&x<=w&&y<=l;	}		
	};
