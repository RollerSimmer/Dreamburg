/********************************
**             wardshire.cpp - **
********************************/

/*
To do:
	+ print totals for the whole map somewhere...
*/

/*
***** Update Log *****
	2011:Aug:02:AM:05:20
		+ made this file
	2011:Aug:02:AM:08:12
		+ set up the initial data and methods from my paper
	2011:Aug:02:AM:09:20
		+ now the display scrolls, allowing for bigger maps to be displayed
	2011:Aug:03:AM:05:47
		+ The program now figures commute distances and displays them
	2011:Aug:04:AM:07:46
		+ going to get started displaying map totals
*/

#include <iostream>
#include <allegro.h>

#include <types/mytypes.h>
#include <cstdlib>
#include <ctime>
#include <list>
#include <sstream>
#include <fstream>

#include "intvector2d.h"

using namespace std;


//const u16 hoodDrawDim=96;
const u16 hoodDrawWidth=96;
const u16 hoodDrawLength=96;



class Ward
	{
	private:
		u8 take,borrow,make,importstock,exportstock,roadnet;
		u32 commuteDistTotal,commuteDistAvg,ncommuters;
	public:
		Ward()		{		}
		void DrawHood(BITMAP*page,short hx,short hy,short xo,short yo)
			{
			short x0,y0,x1,y1,x,y;
			x0=hx*hoodDrawWidth+xo;		y0=hy*hoodDrawLength+yo,
			x1=(hx+1)*hoodDrawWidth-1+xo;		y1=(hy+1)*hoodDrawLength-1+yo;

			u32 fgcolor=0xffffff;	u32 bgcolor=0x101010;
			const u16 bordersz=2;

			//draw box
			rectfill(page,x0,y0,x1,y1,bgcolor);
			rectfill(page,x0,y0,x0+bordersz-1,y1,fgcolor); //lft border
			rectfill(page,x0,y0,x1,y0+bordersz-1,fgcolor); //top border
			rectfill(page,x1-bordersz+1,y0,x1,y1,fgcolor); //rgt border
			rectfill(page,x0,y1-bordersz+1,x1,y1,fgcolor); //btm border

			//show text
			x=x0+bordersz+1;	y=y0+bordersz+1;
			stringstream ss;
			ss.str("");
			const u16 rowinc=8;
			ss<<"x="<<(u32)hx<<",y="<<(u32)hy;
			textout_ex(page,font,ss.str().c_str(),x,y,fgcolor,bgcolor);
			y+=rowinc;		ss.str("");
			ss<<"t="<<(u32)take;
			textout_ex(page,font,ss.str().c_str(),x,y,fgcolor,bgcolor);
			y+=rowinc;		ss.str("");
			ss<<"m="<<(u32)make;
			textout_ex(page,font,ss.str().c_str(),x,y,fgcolor,bgcolor);
			y+=rowinc;		ss.str("");
			ss<<"i="<<(u32)importstock;
			textout_ex(page,font,ss.str().c_str(),x,y,fgcolor,bgcolor);
			y+=rowinc;		ss.str("");
			ss<<"e="<<(u32)exportstock;
			textout_ex(page,font,ss.str().c_str(),x,y,fgcolor,bgcolor);
			y+=rowinc;		ss.str("");
			ss<<"b="<<(u32)borrow<<",r="<<(u32)roadnet;
			textout_ex(page,font,ss.str().c_str(),x,y,fgcolor,bgcolor);
			y+=rowinc;		ss.str("");
			ss<<"n=("<<netstock()<<")";
			textout_ex(page,font,ss.str().c_str(),x,y,fgcolor,bgcolor);
			y+=rowinc;		ss.str("");
			ss<<"cdt="<<commuteDistTotal;
			textout_ex(page,font,ss.str().c_str(),x,y,fgcolor,bgcolor);
			y+=rowinc;		ss.str("");
			ss<<"nc="<<ncommuters;
			textout_ex(page,font,ss.str().c_str(),x,y,fgcolor,bgcolor);
			y+=rowinc;		ss.str("");
			ss<<"cda="<<commuteDistAvg;
			textout_ex(page,font,ss.str().c_str(),x,y,fgcolor,bgcolor);
			}
		short netstock()
			{	short net=make+importstock;	net-=(take+exportstock);
				return net;	}
		bool balanced()	{	return (netstock()>=0);	}
		void FigureCommuteAvg()
			{	if(ncommuters>0)	commuteDistAvg=(commuteDistTotal+ncommuters/2)/ncommuters;
				else commuteDistAvg=0;	}
		void AddToCommutes(u32 distance,u32 portamt)
			{
			commuteDistTotal+=distance*portamt;
			ncommuters+=portamt;
			FigureCommuteAvg();
			}
	};

const u16 hooddim=1024/32;
const u16 hoodwidth=hooddim;
const u16 hoodlength=hooddim;

struct Shire
	{

	};


typedef SimpleHood SimpleHoodArray[hoodlength][hoodwidth];

int main()
	{
	//init random generator
	srand(clock());

	cout<<"hoodsz="<<sizeof(SimpleHood)<<" tothoodsz="<<sizeof(SimpleHoodTotal)<<endl;

	//initialize Allegro
	cout<<"Initializing Allegro..."<<endl;
	allegro_init();
	cout<<"...Done"<<endl;
	install_keyboard();
	install_timer();
	set_color_depth(32);
	set_gfx_mode(GFX_AUTODETECT_WINDOWED,640,480,0,0);
	srand(time(0));

	BITMAP*page;
	page=create_bitmap(SCREEN_W,SCREEN_H);

	//init random trade net
	SimpleTradeNet*tn=new SimpleTradeNet;
	SimpleTradeNet*tnb4=new SimpleTradeNet;

	//drawing coords and done flag
	short xo=0,yo=0,rxo=xo+(hoodwidth+1)*hoodDrawWidth;
	bool done=false;	bool randomizeflag=true;

	while(!done)
		{

		//randomize all neighborhoods
		if(randomizeflag)
			{	tn->RandomizeHoods();
				tn->TallyTotals();
				*tnb4=*tn;
			}

		//draw trade net before
		clear_to_color(page,0x00ff00ff);
		tnb4->DrawHoods(page,xo,yo);
		textout_ex(page,font,"BEFORE EXCHANGE",xo,yo-10,0x000000,0xffff00);
		////acquire_screen();
		////blit(page,screen,0,0,0,0,SCREEN_W,SCREEN_H);
		////release_screen();

		//do trade exchange
		if(randomizeflag)
			{
			tn->ExchangeStock();
			tn->TallyTotals();
			}

		//draw trade net after
		//clear_to_color(page,0x00ff00ff);
		tn->DrawHoods(page,rxo,yo);
		textout_ex(page,font,"AFTER EXCHANGE",rxo,yo-10,0x000000,0xffff00);
		acquire_screen();
		blit(page,screen,0,0,0,0,SCREEN_W,SCREEN_H);
		release_screen();

		//clear randomize flag since everything has been drawn for this round
		randomizeflag=false;

		if(keypressed())
			{
			if(key[KEY_ESC])		done=true;
			if(key[KEY_HOME])		xo=yo=0;
			if(key[KEY_LEFT])		xo-=hoodDrawWidth;
			if(key[KEY_RIGHT])	xo+=hoodDrawWidth;
			if(key[KEY_UP])		yo-=hoodDrawLength;
			if(key[KEY_DOWN])		yo+=hoodDrawLength;
			if(key[KEY_SPACE])	randomizeflag=true;
			}

		rxo=xo+(hoodwidth+1)*hoodDrawWidth;

		rest(50);

		}

	//delete trade net
	delete tn;

	//exit
	return 0;
	}
