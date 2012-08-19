#include "../CityGameClasses/Shires.h"
#include "../CityGameClasses/map.h"
#include "../CityGameClasses/OOAllegro/OOAllegro.h"
#include "../CityGameClasses/Testing/ColorCodedTradeBal.h"

#include <algorithm>
#include <ctime>

using namespace std;

int main()
	{
	//variables:
		OOAllegro*ooallegro;
		ColorCodedTradeBal*colorbal;

	//initialize:
		srand(time(0));
		ooallegro=new OOAllegro(800,600,32);
		patterns=new ZonePatternList;
		Shire*shire=new Shire;
		colorbal=new ColorCodedTradeBal(shire);
		const int shire_amttilesacross=shire_amtWardsAcross*amtTilesPerWard;
		Map::Data*map=new Map::Data(shire_amttilesacross,shire_amttilesacross);

	//alloc all wards, and randomize the connections and trade balances first:
		shire->AllocAllWards(map);
		shire->RandomizeWardConnections(75);
		shire->RandomizeWardTradeBalances();
		shire->UpdateWardPathNets();
		////cout<<"Number of shortages after randomizer: "<<shire->CountShortages()<<endl;
	//trade between wards until escape is pressed:
		bool done=false;
		colorbal->DrawTradeBalances();
		while(!done)
			{
			if(keypressed())
				{
				if(key[KEY_ESC])
					done=true;
				else if(key[KEY_SPACE])
					{
					shire->TradeBetweenWards(20000);
					colorbal->DrawTradeBalances();
					}
				rest(1);
				}
			}

	delete colorbal;
	delete ooallegro;
	delete map;
	delete shire;
	}

int main_testboxrings()
	{
	//variables:
		OOAllegro*ooallegro;

	ooallegro=new OOAllegro(800,600,32);

	int x,y,r,shx,shy,color;
	shx=256;	shy=256;
	clear_to_color(screen,0);
	acquire_screen();
	for(r=0;r<255;r++)
		{
		x=shx-r;	y=shy-r;
		color=makecol(r,r,r);
		while(x<shx+r)	{	++x;	putpixel(screen,x,y,color);	}
		while(y<shy+r)	{	++y;	putpixel(screen,x,y,color);	}
		while(x>shx-r)	{	--x;	putpixel(screen,x,y,color);	}
		while(y>shy-r)	{	--y;	putpixel(screen,x,y,color);	}
		}
	release_screen();

	bool done=false;
	while(!done)
		{
		while(!keypressed())	{	rest(100);	}
		if(keypressed())
			{
			if(key[KEY_ESC])
				done=true;
			else if(key[KEY_SPACE])
				{
				}
			rest(100);
			}
		}
	delete ooallegro;
	}

