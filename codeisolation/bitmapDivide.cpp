/**********************************************************************
**             Allegro game library project template                 **
**********************************************************************/

/*

To do:
	+ cut up tile quadrant images and display them evenly spaced
	+ display size of sprites (std::vector<BITMAP*>)
In Progress:
	+

********** Update Log *******************
2011:Aug:13:PM:12:38
	+	created this file
2011:Aug:13:PM:03:58
	+	now draws a generated building
*/

#include <iostream>
#include <allegro.h>
#include <types/mytypes.h>
#include <cstdlib>
#include <ctime>
#include <list>
#include <sstream>

#include "zonepattern.h"

using namespace std;

const short pageWidth=640;
const short pageLength=480;

int main()
	{
	u16 ztw,ztl;
	cout<<"Enter zone width: "<<endl;
	cin>>ztw;
	cout<<"Enter zone length: "<<endl;
	cin>>ztl;
	bool usetq;
	string usetqstr;
	cout<<"Does this zone use tri-quads (y/n)? "<<endl;
	cin>>usetqstr;
	usetq=usetqstr=="y"||usetqstr=="Y";
	char zname[13];
	cout<<"Enter name: "<<endl;
	cin>>zname;

	#if 1
	//initialize Allegro
	if(true)
		{
		cout<<"Initializing Allegro..."<<endl;
		allegro_init();
		cout<<"...Done"<<endl;
		install_keyboard();
		install_timer();
		set_color_depth(32);
		set_gfx_mode(GFX_AUTODETECT_WINDOWED,pageWidth,pageLength,0,0);
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

	short tx=0,ty=0;
	bool flagloadpcx=false;

	ZonePattern zone(ztw,ztl,100,"",zname,flagloadpcx,usetq);
	u32 zifgc=rand()%0x1000000;
	BITMAP*wholeimg=zone.GenerateBaseImage(0xff0000,zifgc,0x0000ff);
	cout<<"whole image :"<<endl;
	cout<<"\t# pixels = "<<(wholeimg->w*wholeimg->h)<<endl;
	cout<<"\tw = "<<wholeimg->w<<endl;
	cout<<"\tl = "<<wholeimg->h<<endl;

	//set tile occupancy map to random values
	zone.SetRandOccupancy(75,25);

	//generate tile occupancy image
	u16 oitilesz=24;
	BITMAP*oimg;
	u32 oifgc=0x7f7fff;
	u32 oibgc=0x3f3f7f;
	oimg=zone.GenerateOccupancyImage(oifgc,oibgc,oitilesz);

	//cut up tile quadrant images
	zone.CutupImage(wholeimg);
	cout<<"total # of zone sprites: "<<zone.nsprites()<<endl;

	//output
	#if 1
		cout<<"std::vector sz = "<<sizeof(std::vector<short>)<<endl;
		cout<<"ZonePattern::sprites sz = "<<sizeof(ZonePattern::sprites)<<endl;
		cout<<"std::list sz = "<<sizeof(std::list<short>)<<endl;
		cout<<"ZonePattern sz = "<<sizeof(ZonePattern)<<endl;
	#endif

	//loop for input
	while(!key[KEY_ESC])
		{
		//rest(10);
		if(keypressed())
			{
			if(key[KEY_LEFT])
				{	if(tx>-ztw*20)	--tx;	}
			if(key[KEY_RIGHT])
				{	if(tx<ztw*20)	++tx;	}
			if(key[KEY_UP])
				{	if(ty>-ztl*20)	--ty;	}
			if(key[KEY_DOWN])
				{	if(ty<ztl*20)	++ty;	}
			if(key[KEY_HOME])
				{	tx=ty=0;	}
			rest(20);
			}

		//draw the map
		#if 1
			acquire_bitmap(page);

			//clear the screen w/ white
			clear_to_color(page,0xffffffff);

			//draw here
			////map->DrawMap(page,tx,ty);
			short xo,yo;
			xo=tx*xpixelsPerTile;
			yo=ty*ypixelsPerTile;
			draw_sprite(page,wholeimg,xo,yo);
			u16 pad=2;	bool transparent=false;
			zone.DrawEvenlySpacedTiles(page,xo+wholeimg->w,yo,pad
			                           ,transparent);
			blit(oimg,page,0,0,xo,yo+wholeimg->h+pad,oimg->w,oimg->h);
			//draw the zone as one
			short wxo=xo+wholeimg->w+(zone.tw*zone.nquads()+pad)*xpixelsPerTile;
			short wyo=yo;
			zone.DrawWholeZone(page,wxo,wyo);

			release_bitmap(page);

			//page flip
			acquire_screen();
			blit(page,screen,0,0,0,0,SCREEN_W,SCREEN_H);
			release_screen();
		#endif
		}

	////delete map;

	#endif
	#if 0
	cout<<"Tsz="<<sizeof(T)<<endl;

	#endif
	}
