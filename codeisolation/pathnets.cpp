#include "../CityGameClasses/Shires.h"
#include "../CityGameClasses/map.h"
#include "../CityGameClasses/OOAllegro/OOAllegro.h"
#include "../CityGameClasses/Testing/PathNetDisplay.h"

#include <algorithm>
#include <ctime>
#include <sstream>

using namespace std;

int main(int parc,char*parv[])
	{
	//variables:
		OOAllegro*ooallegro;
		PathNetDisplay*pathnets;
		stringstream ss;

	//initialize:
		srand(time(0));
		ooallegro=new OOAllegro(800,600,32);
		patterns=new ZonePatternList;
		Shire*shire=new Shire;
		pathnets=new PathNetDisplay(shire);
		const int shire_amttilesacross=shire_amtWardsAcross*amtTilesPerWard;
		Map::Data*map=new Map::Data(shire_amttilesacross,shire_amttilesacross);

	//alloc all wards, and randomize the connections and trade balances first:
		shire->AllocAllWards(map);
		int connectpct;
		if(parc>=1)
			{	ss.str(parv[1]);
				ss>>connectpct;	}
		else
			connectpct=35;
		shire->RandomizeWardConnections(connectpct);
		shire->RandomizeWardTradeBalances();

	//update path nets for each ward in shire:
		shire->UpdateWardPathNets();

	//display until escape is pressed:
		bool done=false;
		pathnets->DrawPathNets();
		while(!done)
			{
			if(keypressed())
				{
				if(key[KEY_ESC])
					done=true;
				else if(key[KEY_SPACE])
					{
					textout_ex(screen,font,"Updating path nets...",0,0,0xffffff,0);
					shire->RandomizeWardConnections(connectpct);
					shire->UpdateWardPathNets();
					pathnets->DrawPathNets();
					}
				rest(100);
				}
			else
				rest(0);
			}
	delete pathnets;
	delete ooallegro;
	delete map;
	delete shire;
	}
