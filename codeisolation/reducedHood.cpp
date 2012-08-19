/******************************************************************
**             reducedHood - {Wave,Clarity,Light}                **
******************************************************************/

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

struct SimpleHood
	{
	u8 take,borrow,make,importstock,exportstock,roadnet;
	u32 commuteDistTotal,commuteDistAvg,ncommuters;
	SimpleHood()		{		}
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

const u16 hooddim=512/16;
const u16 hoodwidth=hooddim;
const u16 hoodlength=hooddim;

typedef SimpleHood SimpleHoodArray[hoodlength][hoodwidth];

struct SimpleHoodTotal:public SimpleHood
	{
	u32 take,borrow,make,importstock,exportstock;
	SimpleHoodArray*hoods;
	////u32 commuteDistTotal,commuteDistAvg,ncommuters;
	SimpleHoodTotal(SimpleHoodArray*shoods)	{	hoods=shoods;	}
	void TallyTotals()
		{
		take=borrow=make=importstock=exportstock=0;
		commuteDistTotal=commuteDistAvg=ncommuters=0;
		for(short hy=0;hy<hoodlength;hy++)
		for(short hx=0;hx<hoodwidth;hx++)
			{
			SimpleHood*h=&(*hoods)[hy][hx];
			//inc all the fields
			take+=h->take;	borrow+=h->borrow;	make+=h->make;
			importstock+=h->importstock;	exportstock+=h->exportstock;
			commuteDistTotal+=h->commuteDistTotal;
			ncommuters+=h->ncommuters;
			}
		FigureCommuteAvg();
		}
	short netstock()
		{	short net=make+importstock;	net-=take+exportstock;
			return net;	}
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
		ss<<"b="<<(u32)borrow;
		textout_ex(page,font,ss.str().c_str(),x,y,fgcolor,bgcolor);
		y+=rowinc;		ss.str("");
		ss<<"r="<<(u32)roadnet;
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
	};

struct SimpleTradeNet
	{
	u16 w,l;
	SimpleHoodArray hoods;
	SimpleHoodTotal hoodtotals;
	SimpleTradeNet()
			:	hoodtotals(&hoods)
		{	w=hoodwidth;	l=hoodlength;	RandomizeHoods();		}
	void RandomizeHoods()
		{
		SimpleHood*h;
		for(int hy=0;hy<l;hy++)
		for(int hx=0;hx<w;hx++)
			{
			h=&hoods[hy][hx];
			h->take=rand()%256;
			h->borrow=rand()%256;
			h->make=rand()%256;
			h->importstock=0;
			h->exportstock=0;
			h->roadnet=1;
			h->commuteDistTotal=0;
			h->commuteDistAvg=0;
			h->ncommuters=0;
			}
		}
	void DrawHoods(BITMAP*page,short xo,short yo)
		{
		for(int hy=0;hy<hoodlength;hy++)
		for(int hx=0;hx<hoodwidth;hx++)
			{	hoods[hy][hx].DrawHood(page,hx,hy,xo,yo);	}
		//draw totals
		short tmpyo=yo+(hoodlength+1)*hoodDrawLength-10;
		textout_ex(page,font,"TOTAL",xo,tmpyo,0x000000,0x7f7fff);
		hoodtotals.DrawHood(page,0,hoodlength+1,xo,yo);
		}
	bool HoodInBounds(short shx,short shy)
		{	return (shx>=0&&shy>=0&&shx<l&&shy<w);	}
	template <class T>
		static T RandIntDist(T fx,T fy,T tx,T ty)
			{
			T linedist=intvector2d<T,T>::intmag(tx-fx,ty-fy);
			T arcdist=linedist*157/100;
			T dist=std::min(linedist,arcdist);
			dist=dist+rand()%std::abs(arcdist-linedist);
			return dist;
			}
	template <class T>
		static T AvgIntDist(T fx,T fy,T tx,T ty)
			{
			T linedist=intvector2d<T,T>::intmag((T)tx-fx,(T)ty-fy);
			T arcdist=linedist*157/100;
			T dist=(linedist+arcdist+1)/2;
			return dist;
			}
	bool FindNearestSurplus(short&shx,short&shy,short hx,short hy)
		{
		//Note: this method steps through the hoods in a box path, so sometimes
		//the dist can be 1.4*radius.  Even so, this way is faster...
		short r=1;
		shx=hx-r;	shy=hy-r;
		short xinc=1;		short yinc=0;
		short xt=hx+r;	short yt=hy-r;	//targets
		short dir=0;
		bool found=false,done=false;
		//set radius max
		short wmax=std::max(shx,(short)(w-shx));
		short lmax=std::max(shy,(short)(l-shy));
		short rmax=std::max(wmax,lmax);
		while(!done)
			{
			//check for surplus
			if(HoodInBounds(shx,shy))
				found=hoods[shy][shx].netstock()>0;
			else
				found=false;
			if(found)	return true;
			shx+=xinc;shy+=yinc;
			if(shx==xt&&shy==yt)
				switch(dir=(dir+1)%4)	{
				case 1:	//down
					yinc=1;	xinc=0;	xt=hx+r;	yt=hy+r;
					break;
				case 2:	//left
					yinc=0;	xinc=-1;	xt=hx-r;	yt=hy+r;
					break;
				case 3:	//up
					yinc=-1;	xinc=0;	xt=hx-r;	yt=hy-r;
					break;
				case 0:	//left
					r++;
					yinc=0;	xinc=1;	xt=hx+r;	yt=hy-r;
					shx--;	shy--;
					done=r>rmax;
					break;
				default:	break;	}
			}

		return false;
		}
	void ExchangeStock()
		{
		for(int hy=0;hy<hoodlength;hy++)
		for(int hx=0;hx<hoodwidth;hx++)
			{
			bool hooddone=false;
			short shx,shy;//surplus hood x and y
			SimpleHood*h=&hoods[hy][hx];
			SimpleHood*sh;
			if(!h->balanced()) while(!hooddone&&!h->balanced())
				{
				if(FindNearestSurplus(shx,shy,hx,hy))	//if surplus was found
					{
					sh=&hoods[shy][shx];
					short surplusnet=std::abs(sh->netstock());
					short deficitnet=std::abs(h->netstock());
					u16 portamt=std::min(surplusnet,deficitnet);
					sh->exportstock+=portamt;
					h->importstock+=portamt;
					u32 distance=AvgIntDist<u32>(shx,shy,hx,hy);
					h->AddToCommutes(distance,portamt);
					sh->AddToCommutes(distance,portamt);
					}
				else
					hooddone=true;
				}
			}
		}
	void TallyTotals()		{	hoodtotals.TallyTotals();	}
	};

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
