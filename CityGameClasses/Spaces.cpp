#include "Spaces.h"

Space::Space(u16 _x,u16 _y,u16 _w,u16 _l)
	{
	x=_x;	y=_y;	w=_w;	l=_l;
	for(int i=0;i<tradeitem::amount;i++)
		{
		#if(useShortageAndSurplus)
			tradebal[i].shortage=0;
			tradebal[i].surplus=0;
		#else
			tradebal[i]=0;
		#endif
		}
	}

TradeItem_SpaceBalance*Space::gettradebal(int item)
	{	
	if(item<tradeitem::amount)
		return &tradebal[item];	
	else
		return 0;
	}
