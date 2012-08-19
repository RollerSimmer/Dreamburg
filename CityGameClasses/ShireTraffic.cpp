#include "ShireTraffic.h"
#include "TrafficVehicle.h"

#include "intvector2d.h"

////ShireTraffic::
void ShireTraffic::CommuteSamples(int amtvehicles)
	{
	int i;
	bool shortagefound,surplusfound;
	list<TrafficVehicle> vehlist;
	list<TrafficVehicle>::iterator tvi;	
	TrafficVehicle tv;
	int magsum,journeylen,volperveh;
	//decide upon number of vehicles
		//...already decided with argument
	//trade should not already be balanced
	for(i=0;i<amtvehicles;i++)
		{
		//for each vehicle, create a journey randomly selecting source and destination \
		  among available shortages and surpluses:
			tv.MakeJourney(shire);
		vehlist.push(tv):
		}
	//compute the total shortage magnitude over the whole shire:
		magsum=ComputeTotalShortageMag();
		//can be randomly sampled:
		
	//set the traffic volume of each vehicle to total shortage magnitude\
	  divided by number of vehicles	(magsum/amtvehicles):
		volperveh=magsum/amtvehicles;
	  
	//find maximum journey length among all vehicles and \
	  add the suitable number of rest steps to each journey (prevlen-maxlen):
		for(tvi=vehlist.begin();tvi!=vehlist.end();tvi++)
			{
			journeylen=max((int)tv.GetJourneyLen(),(int)journeylen);
		for(tvi=vehlist.begin();tvi!=vehlist.end();tvi++)
			{
			tvi->PadJourneyWithRests(journeylen);
			}
	
	//set peak traffic volume to zero for each ward:
		shire->ResetPeakTrafficVolumes_Peak();
	
	//process (maxlen) frames:
		for(int frame=0;frame<journeylen;frame++)
			{
			//set the current traffic volume to zero for each ward:
				shire->ResetPeakTrafficVolumes_Current();
	
			//update each vehicle to where it is supposed to be in the journey:
				for(tvi=vehlist.begin();tvi!=vehlist.end();vehlist++)
					tvi->StepJourney();
		
			//if a vehicle is idle this step, do nothing with it:
				if(tvi->IsIdleThisStep())
					{}
			//otherwise, add the vehicle's volume to the total current volume for \
			  the ward that the vehicle is on, this step:
				else
					{
					shire->IncreaseCurTrafficVol(tvi->getx(),tvi->gety());
					
					//if that ward's peak is less than the current volume, set peak to current:
						shire->UpdatePeakTrafficVol(tvi->getx(),tvi->gety());
					}

			}

	//after this loop is done, peaks should now reflect traffic peak volumes, \
	  so return:
		return;	
	}


