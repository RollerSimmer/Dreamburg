#pragma once

#include "Shires.h"

class ShireTraffic
	{
	private:
		Shire*shire;
	public:
		inline ShireTraffic(Shire*_shire)	{	shire=_shire;	}		
		void ProcessDay(int amtvehicles);
		inline void FindShortage(int&item,int&shx,int&shy)	{	shire->FindShortage(item,shx,shy);		}	
		int ComputeTotalShortageMag()	{	}	//stub
	};


