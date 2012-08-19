/**********************************************************************
**             Allegro game library project template                 **
**********************************************************************/

#include <iostream>
#include <allegro.h>
#include "MapDrawer.h"
#include "IsoTerrain.h"
#include "TerrainEroder.h"
#include "RiverEroder.h"
#include <algorithm>
#include <sstream>

using namespace std;

void AllegroInit();

#define test 0

int main_test();

int main()
	{
	#if test
		return main_test();
	#else
		AllegroInit();
		IsoTerrain isoter;
		MapDrawer mapdrawer(&isoter);

		////isoter.GenRandHts();
		isoter.Pyramid(200);
		//isoter.SmoothAll(mapsz/2);

		bool done=false;
		int ox=-10;	int oy=-10;

		int pad=64;
		int cxmin=0-pad;
		int cymin=0-pad;
		int cxmax=mapwidth+pad;
		int cymax=maplength+pad;

		mapdrawer.DrawMap(-10,-10);
		int smoothradius=5;	const int maxsmoothradius=25;
		int ndrops=5;	const int maxndrops=50;
		int radius=5;	const int maxradius=25;
		int digdepth=50;	const int maxdigdepth=200;
		stringstream ss;
		int ytext;

		ytext=0;
		mapdrawer.DrawMap(ox,oy);
		ss.str("");	ss<<"smooth radius: "<<smoothradius<<"       (\"+\"/\"-\") ";
		textout_ex(mapdrawer.GetPage(),font,ss.str().c_str(),0,ytext,0x000000,0xffffff);	ytext+=8;
		ss.str("");	ss<<"radius: "<<radius<<"           (ctrl + left/right)";
		textout_ex(mapdrawer.GetPage(),font,ss.str().c_str(),0,ytext,0x000000,0xffffff);	ytext+=8;
		ss.str("");	ss<<"# of drops: "<<ndrops<<"        (ctrl + \"+\"/\"-\")";
		textout_ex(mapdrawer.GetPage(),font,ss.str().c_str(),0,ytext,0x000000,0xffffff);	ytext+=8;
		ss.str("");	ss<<"dig depth: "<<digdepth<<"         (ctrl + up/down)";
		textout_ex(mapdrawer.GetPage(),font,ss.str().c_str(),0,ytext,0x000000,0xffffff);	ytext+=8;

		acquire_screen();
		blit(mapdrawer.GetPage(),screen,0,0,0,0,800,600);
		release_screen();

		bool update=false;
		int unitInc=1;

		while(!done)
			{
			update=false;
			if(keypressed())
				{
				if(key[KEY_LSHIFT]||key[KEY_LSHIFT])
					unitInc=10;
				else
					unitInc=1;
				if(key[KEY_ESC])	{	done=true;	continue;	}
				if(key[KEY_LEFT])
					{
					if(key[KEY_LCONTROL]||key[KEY_RCONTROL])
						{	for(int n=0;n<unitInc;n++)	if(radius>0)	--radius;	rest(100);	}
					else if(key[KEY_LSHIFT]||key[KEY_RSHIFT])
						{	ox=max(ox-10,cxmin);	oy=min(oy+10,cymax);	}
					else
						{	if(ox>=cxmin)	--ox;	if(oy<=cymax)	++oy;	}
					update=true;
					}
				if(key[KEY_RIGHT])
					{
					if(key[KEY_LCONTROL]||key[KEY_RCONTROL])
						{	for(int n=0;n<unitInc;n++)	if(radius<maxradius)	++radius;	rest(100);	}
					else if(key[KEY_LSHIFT]||key[KEY_RSHIFT])
						{	ox=min(ox+10,cxmax);	oy=max(oy-10,cymin);	}
					else
						{	if(ox<=cxmax)	++ox;	if(oy>=cymin)	--oy;	}
					update=true;
					}
				if(key[KEY_UP])
					{
					if(key[KEY_LCONTROL]||key[KEY_RCONTROL])
						{	for(int n=0;n<unitInc;n++)	if(digdepth>1)	--digdepth;	rest(100);				}
					else if(key[KEY_LSHIFT]||key[KEY_RSHIFT])
						{	ox=max(ox-10,cxmin);	oy=max(oy-10,cymin);	}
					else
						{	if(ox>=cxmin)	--ox;	if(oy>=cymin)	--oy;	}
					update=true;
					}
				if(key[KEY_DOWN])
					{
					if(key[KEY_LCONTROL]||key[KEY_RCONTROL])
						{	for(int n=0;n<unitInc;n++)	if(digdepth<maxdigdepth)	++digdepth;	rest(100);				}
					else if(key[KEY_LSHIFT]||key[KEY_RSHIFT])
						{	ox=min(ox+10,cxmax);	oy=min(oy+10,cymax);	}
					else
						{	if(ox<=cxmax)	++ox;	if(oy<=cymax)	++oy;	}
					update=true;
					}
				if(key[KEY_PLUS_PAD])	//increase smooth radius
					{
					if(key[KEY_LCONTROL]||key[KEY_RCONTROL])
						{	for(int n=0;n<unitInc;n++)	if(ndrops<maxndrops)	++ndrops;	rest(100);				}
					else
						{	if(smoothradius<maxsmoothradius) ++smoothradius;	rest(100);	}
					update=true;
					}
				if(key[KEY_MINUS_PAD])	//decrease smooth radius
					{
					if(key[KEY_LCONTROL]||key[KEY_RCONTROL])
						{	for(int n=0;n<unitInc;n++)	if(ndrops>1)	--ndrops;	rest(100);				}
					else
						{	if(smoothradius>1) --smoothradius;	rest(100);	}
					update=true;
					}
				if(key[KEY_PGDN])
					{	mapdrawer.ZoomIn();	rest(100);	update=true;	}
				if(key[KEY_PGUP])
					{	mapdrawer.ZoomOut();	rest(100);	update=true;	}
				if(key[KEY_O])	//raise and random octagon
					{	isoter.RaiseOctagon(rand()%mapwidth,rand()%maplength
					                      ,rand()%(mapwidth/8+1),rand()%(isoter.GetZCap()/20+1));	rest(100);	}
				if(key[KEY_G])//ru_g_ged terrain
					{	isoter.MakeRugged(radius);	rest(100);	update=true;	}
				if(key[KEY_Y])	//
					{
					if(key[KEY_LCONTROL]||key[KEY_RCONTROL])	//inverted pyramid
						isoter.Invert();
					else if(key[KEY_LSHIFT]||key[KEY_RSHIFT])	//many pyramids
						isoter.ScramblePyramids(200,5,15,5,15);
					else
						isoter.Pyramid(200);
					rest(100);	update=true;
					}
				if(key[KEY_L])
					{
					if(key[KEY_LCONTROL]||key[KEY_RCONTROL])	//lower
						isoter.RaiseAll(-20);
					else
						;
					rest(100);
					update=true;
					}
				if(key[KEY_R])
					{
					if(key[KEY_LCONTROL]||key[KEY_RCONTROL]) //raise
						isoter.RaiseAll(20);
					else
						isoter.GenRandHts(rand()%200+10);
					rest(100);
					update=true;
					}
				if(key[KEY_F])
					{	isoter.FlattenAll(0);	rest(100);	update=true;	}
				if(key[KEY_0]||key[KEY_0_PAD])	//debug function
					{
					RiverEroder*re=new RiverEroder(&isoter);
					int nx,ny,x,y;
					x=rand()%mapwidth/2+mapwidth/4;
					y=rand()%maplength/2+maplength/4;
					re->FloodExitPoint(nx,ny,x,y);
					delete re;
					}
				if(key[KEY_3]||key[KEY_3_PAD])	//triple function (river->smooth->rugged)
					{
					isoter.RiverErode(3000,digdepth,50);
					isoter.SmoothAll(smoothradius);
					isoter.MakeRugged(radius);
					rest(100);	update=true;
					}
				if(key[KEY_2]||key[KEY_2_PAD])	//halve the terrain
					{
					if(key[KEY_LCONTROL]||key[KEY_RCONTROL])	isoter.Halve();
					else													isoter.Double();
					rest(100);	update=true;
					}

				if(key[KEY_T])	//sca_t_ter
					{	isoter.ScatterInc(5);	rest(100);	update=true;	}
				if(key[KEY_S])
					{	isoter.SmoothAll(smoothradius);	rest(100);	update=true;	}
				if(key[KEY_H])
					{
					if(key[KEY_LCONTROL]||key[KEY_RCONTROL])	isoter.SineHill(200);
					else													isoter.SharpHill(200);
					rest(100);	update=true;
					}
				if(key[KEY_E])
					{	isoter.Erode(5,ndrops,radius,digdepth,smoothradius);	rest(100);	update=true;	}
				if(key[KEY_V])
					{
					if(key[KEY_LCONTROL]||key[KEY_RCONTROL])	//voronoi
						{	isoter.Voronoi(30,0,2,4);	rest(100);	update=true;	}
					else
						isoter.PeaksAndValleys();
					rest(100);	update=true;
					}
				if(key[KEY_I]&&(key[KEY_LCONTROL]||key[KEY_RCONTROL]))	//inverted pyramid
					{	isoter.Invert();	rest(100);	update=true;	}
				else if(key[KEY_I])
					////{	isoter.RiverErode(10,digdepth);	rest(100);	update=true;	}
					{	isoter.RiverErode(100,digdepth,100);	update=true;	/*rest(100);	update=true;*/	}
					////           (int ncycles,int dropsPerRound,int radius,int digdepth,int smoothradius);
				if(key[KEY_D])	//debug
					{
					////TerrainEroder te(&isoter,mapwidth,maplength,1,1,5,0);
					////te.SingleDrop(64,100);

					////isoter.DendriteErode(5,20,100,400);

					isoter.DendriteRidges(5,20,100,-50);

					////isoter.RaisePyramid(rand()%mapwidth,rand()%maplength,rand()%20+1,rand()%10+1,rand()%1+1);
					#if 0
						int wcx,wcy;
						////wcx=mapwidth/2;	wcy=maplength/2;
						////isoter.WedgeCut(wcx,wcy,100,wcx+10,wcy+10,0,10,10,100,100);
						int x,y,z;
						x=rand()%mapwidth;	y=rand()%maplength;	z=isoter.ht(x,y);
						isoter.Wedge( x,y,z
						             ,x+rand()%51-25,y+rand()%51-25,z+rand()%21-10
						             ,rand()%20+1,rand()%40+1);
						rest(1);
					#endif
					update=true;
					}
				if(key[KEY_M])	//mound
					{
					if(key[KEY_LCONTROL]||key[KEY_RCONTROL])
						{	isoter.SineHillMesh(200,rand()%20+1,rand()%20+1);	}
					else
						{	isoter.Dome(200);	}
					rest(100);	update=true;
					}

				rest(0);

				if(update)
					{
					ytext=0;
					mapdrawer.DrawMap(ox,oy);
					ss.str("");	ss<<"smooth radius: "<<smoothradius<<"       (\"+\"/\"-\") ";
					textout_ex(mapdrawer.GetPage(),font,ss.str().c_str(),0,ytext,0x000000,0xffffff);	ytext+=8;
					ss.str("");	ss<<"radius: "<<radius<<"           (ctrl + left/right)";
					textout_ex(mapdrawer.GetPage(),font,ss.str().c_str(),0,ytext,0x000000,0xffffff);	ytext+=8;
					ss.str("");	ss<<"# of drops: "<<ndrops<<"        (ctrl + \"+\"/\"-\")";
					textout_ex(mapdrawer.GetPage(),font,ss.str().c_str(),0,ytext,0x000000,0xffffff);	ytext+=8;
					ss.str("");	ss<<"dig depth: "<<digdepth<<"         (ctrl + up/down)";
					textout_ex(mapdrawer.GetPage(),font,ss.str().c_str(),0,ytext,0x000000,0xffffff);	ytext+=8;

					acquire_screen();
					//int cw,ch;
					//cw=min(800,mapdrawer.CalcPageWidth()-ox);
					//ch=min(600,mapdrawer.CalcPageHeight()-oy);
					blit(mapdrawer.GetPage(),screen,0,0,0,0,800,600);
					release_screen();
					}
				}
			}

		while(!keypressed())	{}

		return 0;
	#endif
	}

int main_test()
	{
	AllegroInit();
	int points[8]={0,100,100,0,200,100,100,200};
	acquire_screen();
	polygon(screen,4,points,0x003f7f);
	release_screen();
	while(!keypressed()){}
	return 0;
	}

void AllegroInit()
	{
	//initialize Allegro
	cout<<"Initializing Allegro..."<<endl;
	allegro_init();
	cout<<"...Done"<<endl;
	install_keyboard();
	install_timer();
	set_color_depth(32);
	set_gfx_mode(GFX_AUTODETECT_WINDOWED,800,600,0,0);
	}
