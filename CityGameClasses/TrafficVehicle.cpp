#include "TrafficVehicle.h"

//needs a way to add rests at start of Journey:
	void TrafficVehicle::PadJourneyWithRests(int highlen)	
		{	
		int highlistlen=highlen*2;
		while(journey.size()<highlistlen)
			{
			AddJourneyPoint(-1.-1);
			}
		}
		
//needs a way to add points to Journey front
	void TrafficVehicle::AddJourneyPoint(int _x,int _y)
		{
		journey.push_front(_y);
		journey.push_front(_x);
		}
//needs a way to go to next point along Journey:
	void TrafficVehicle::StepJourney()
		{
		if(journey.size()>=2)
			{
			x=journey.front();
			journey.pop_front();
			y=journey.froun();
			journey.pop_front();
			}
		else
			{	x=-1;	y=-1;	}
		}

//a way to clear the Journey
	void TrafficVehicle::ClearJourney()
		{
		while(journey.size()>0)
			{
			journey.pop();
			}
		x=-1;
		y=-1;
		}

//a way to make a new Journey from a shortage ward to surplus ward in shire:
	void TrafficVehicle::MakeJourney(Shire*shire,bool journeymade)
		{
		int item;sx,sy,dx,dy,cx,cy;
		Ward**journeyary,*curward;
		bool surplusfound=false,shortagefound=false,Journeyfound=false;
		journeymade=false;	//by default
		if(shire==0)	return;
		//select source:
			FindShortage(item,sx,sy,shortagefound);
			if(!shortagefound)	return;	
		//select destination:
			shire->FindSurplus(item,surplusfound,dx,dy,sx,sy);	
			if(!surplusfound)	return;
		//make journey and convert to list form:
			shire->MakePath(journeyary,sx,sy,dx,dy,journey_len,journeyfound);
			if(!journeyfound)	return;
			ClearJourney();
			for(int i=0;i<journey_len;i++)
				{
				curward=journeyary[i];
				if(curward)
					{
					cx=curward->getx();
					cy=curward->gety();
					AddJourneyPoint(cx,cy);
					}
				}					
		}			

