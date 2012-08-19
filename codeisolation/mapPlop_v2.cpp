/**********************************************************************
**             Allegro game library project template                 **
**********************************************************************/

/*

To do:
	+
In Progress:
	+

********** Update Log *******************
2011:Aug:22:PM:12:21
	+ Started this file
*/

#include <iostream>
#include <allegro.h>
#include <types/mytypes.h>
#include <cstdlib>
#include <ctime>
#include <list>
#include <sstream>

#include "../CityGameClasses/map.h"

using namespace std;

const u16 tilePxSz=32;

const u16 pageWidth=640;
const u16 pageLength=480;

const u16 pageTileWidth=pageWidth/xpixelsPerTile;
const u16 pageTileLength=pageLength/ypixelsPerTile;

typedef std::list<u16> T;

struct BitFieldTest
	{
	union
		{
		u8 byt;
		struct	{	u8 nib1:4;	u8 nib2:4;	};
		};
	BitFieldTest(u16 val)	{	byt=val;	}
	};

int main()
	{
	#if 1
	//initialize Allegro
	if(true)
		{
		cout<<"Initializing Allegro..."<<endl;
		allegro_init();
		cout<<"...Done"<<endl;
		install_keyboard();
		install_timer();
		set_color_depth(24);
		set_gfx_mode(GFX_AUTODETECT_WINDOWED,pageWidth,pageLength,0,0);
		////install_mouse();
		////show_mouse(screen);
		}

	//seed rand
	srand(time(0));

	//setup screen draw page
	BITMAP*page=create_bitmap(SCREEN_W,SCREEN_H);

	//clear the screen w/ white
	clear_to_color(page,0xffffffff);

	//page flip
	acquire_screen();
	blit(page,screen,0,0,0,0,SCREEN_W,SCREEN_H);
	release_screen();

	//initialize map
	Map::Data*map=new Map::Data(mapsz,mapsz);

	//add map patterns
	map->AddRandPattern(4,20,"houses",0xff0000);
	map->AddRandPattern(4,20,"apartments",0x7f0000);
	map->AddRandPattern(4,20,"offices",0x00007f);
	map->AddRandPattern(4,20,"shopcenter",0x00ffff);
	map->AddRandPattern(4,20,"policedept",0x3f3f5f);
	map->AddRandPattern(4,20,"firedept",0x5f3f3f);
	map->AddRandPattern(1,10,"road",0x4f4f4f);

	//add physical zones to the map
	int i;
	for(short ty=0;ty<8;ty++)
	for(short tx=0;tx<8;tx++)
		{	map->AddZone(tx,ty,i=rand()%(map->npatterns()-1)+1,0,0,0);	}

	short tx=0,ty=0;

	//loop for input

	bool update=false;
	bool whileinit=true;

	while(!key[KEY_ESC])
		{
		//rest(10);
		if(keypressed())
			{
			if(key[KEY_LEFT])
				{	if(tx>0)	--tx;	}
			if(key[KEY_RIGHT])
				{	if(tx<5)	++tx;	}
			if(key[KEY_UP])
				{	if(ty>0)	--ty;	}
			if(key[KEY_DOWN])
				{	if(ty<5)	++ty;	}
			rest(20);
			update=true;
			}
		else if(whileinit)
			{
			update=true;
			whileinit=false;
			}
		else
			update=false;

		if(update)
			{
			acquire_bitmap(page);

			//clear the screen w/ white
			clear_to_color(page,0xffffffff);

			//draw here
			map->DrawMap(page,tx,ty);

			release_bitmap(page);

			//page flip
			acquire_screen();
			////scare_mouse();
			blit(page,screen,0,0,0,0,SCREEN_W,SCREEN_H);
			////unscare_mouse();
			release_screen();
			}


		}

	delete map;

	#endif
	#if 0
	cout<<"Tsz="<<sizeof(T)<<endl;

	#endif
	}
