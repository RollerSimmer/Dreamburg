#pragma once

////#include "Ward.h"
#include "Shires.h"

class TrafficVehicle
	{
	private:
		//needs a list of points:
			list<int>journey;
		//needs a current point:
			int x,y;
	public:
		//accessors:
			inline void setx(int _x)	{	x=_x;	}
			inline void sety(int _y)	{	y=_y;	}
			inline int getx()	{	return x;	}
			inline int gety()	{	return y;	}
			//a way to find journey length:
				inline int GetJourneyLen()	{	return journey.size()/2;	}
		//constructors:	
			TrafficVehicle()	{	}	//stub
		//methods:
			//needs a way to add rests at start of path:
				void PadJourneyWithRests(int highlen);
			//needs a way to add points to path front
				void AddJourneyPoint(int _x,int _y);
			//needs a way to go to next point along path:
				void StepJourney();	
			//a way to clear the path
				void ClearJourney();			
			//a way to make a new path from a shortage ward to surplus ward in shire:
				void MakeJourney(Shire*shire);			
			//a way to find out if vehicle is idle:
				inline bool IsIdleThisStep()	{	return x==-1&&y==-1;	}
	};


