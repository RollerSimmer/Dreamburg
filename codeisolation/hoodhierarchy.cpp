/**********************************************
**             region hierarchy					**
**********************************************/

/*
To do:
	+	Need to fix bounded drawing.  Some zones disappear before leaving the viewport
	+	clean up debug couts
	+	figure out how to get code::blocks to jump to my implementation functions in sep
		files
*/

/*
***** Update Log *****
	2011:Aug:07:AM:03:42
		+	Started this file to see how hood trees work.
	2011:Aug:07:AM:11:49
		+	Added the code, but need to debug so that no segfaults occur
	2011:Aug:07:PM:01:36
		+	Finally got it to draw.
*/

#include <iostream>
#include <allegro.h>

#include <types/mytypes.h>
#include <cstdlib>
#include <ctime>
#include <list>
#include <sstream>
#include <fstream>
#include <stack>

#include "zonepattern.h"
#include "tilesize.h"

using namespace std;

//struct ZoneRef;
//struct TradeData;
struct ZoneRef
	{
	u16 tx,ty;	u8 tw,tl;	u16 patternID,zval;
	ZoneRef(u16 stx,u16 sty,u16 spatternID=0)
		{
		tx=stx;	ty=sty;
		patternID=spatternID;
		zval=tx+ty;
		tw=pattern().tw;	tl=pattern().tl;
		}
	u16 rightedge()	{	return tx+pattern().tw-1;	}
	u16 bottomedge()	{	return ty+pattern().tl-1;	}
	ZoneRef(u16 stx,u16 sty,u8 stw,u8 stl,u16 spatternID=0)
		{
		//construct the zone reference with directly assigned w & l
		tx=stx;	ty=sty;
		patternID=spatternID;
		zval=tx+ty;
		tw=stw;	tl=stl;
		}
	ZonePattern&pattern()
		{
		if(patterns->amount()<maxzonepatterns)
			return patterns->plist[patternID];
		else
			return patterns->plist[nullpatternID];
		}
	static bool comparezvals_descending(ZoneRef a,ZoneRef b)
		{	return (a.zval>b.zval);		}
	static bool comparezvals_ascending(ZoneRef a,ZoneRef b)
		{	return (a.zval<b.zval);		}
	bool PointInZone(short ptx,short pty)
		{	return ptx>=tx&&ptx<=rightedge()&&pty>=ty&&pty<=bottomedge();	}
	void DrawZone(BITMAP*page,short txo,short tyo)
		{
		short dtx=tx+txo;	short dty=ty+tyo;
		short dtw;	short dtl;
		dtx*=xpixelsPerTile;
		dty*=ypixelsPerTile;

		BITMAP*patsprite=(*patterns)[patternID].sprite;
		dtw=patsprite->w;	dtl=patsprite->h;
		blit(patsprite,page,0,0,dtx,dty,dtw,dtl);
		}
	};

struct TradeData
	{
	u8 dummy;
	};

struct CollisionMap;

struct QuadRegion
	{
	//data
	u16 tx,ty,tw,tl;	//x,y,width, and length
	QuadRegion*children[4];
	std::list<ZoneRef> zonelist;
	TradeData trade;
	QuadRegion*parent;
	CollisionMap*collision;
	//constuctor/destructor
	void SetDim(u16 sx,u16 sy,u16 sw,u16 sl)	{	tx=sx;	ty=sy;	tw=sw;	tl=sl;	}
	QuadRegion(u16 x,u16 y,u16 w,u16 l,QuadRegion*parent,CollisionMap*collision)
		//	:	trade(0);
		{
		SetDim(x,y,w,l);
		for(int i=0;i<4;i++)	children[i]=0;
		if(!zonelist.empty())	zonelist.clear();
		this->parent=parent;
		this->collision=collision;
		}
	~QuadRegion()
		{
		for(int i=0;i<4;i++)	if(children[i])	delete children[i];
		this->parent=0;
		this->collision=0;
		}
	//methods
	void GetBounds(short&l,short&r,short&t,short&b
						,short&ql,short&qr,short&qt,short&qb
			         ,short&zl,short&zr,short&zt,short&zb,ZoneRef&z)
		{
		ql=tx;	qr=tx+tw-1;		qt=ty;	qb=ty+tl-1;
		zl=z.tx;	zr=z.tx+z.tw-1;	zt=z.ty;	zb=z.ty+z.tl-1;
		l=std::max(ql,zl);	r=std::min(qr,zr);
		t=std::max(qt,zt);	b=std::max(qb,zb);
		}
	void GetBounds(short&l,short&r,short&t,short&b
						,short&ql,short&qr,short&qt,short&qb
			         ,short zl,short zr,short zt,short zb)
		{
		ql=tx;	qr=tx+tw-1;		qt=ty;	qb=ty+tl-1;
		l=std::max(ql,zl);	r=std::min(qr,zr);
		t=std::max(qt,zt);	b=std::min(qb,zb);
		}
	bool ZoneContained(ZoneRef*z)
		{
		bool zc=false;
		short ql,qr,qt,qb;	//quad region bounds
		short zl,zr,zt,zb;	//zone bounds
		short l,r,t,b;	//zone bounds
		GetBounds(l,r,t,b,ql,qr,qt,qb,zl,zr,zt,zb);
		zc=l<r&&t<b;
		return zc;
		}
	bool ZoneContainedCompletely(ZoneRef*z)
		{
		bool zcc=false;
		short ql,qr,qt,qb;	//quad region bounds
		short zl,zr,zt,zb;	//zone bounds
		short l,r,t,b;	//zone bounds
		GetBounds(l,r,t,b,ql,qr,qt,qb,zl,zr,zt,zb,*z);
		zcc=l==zl&&r==zr&&t==zt&&b==zb;
		return zcc;
		}
	bool ZoneContained(short zl,short zt,short zr,short zb)
		{
		bool zc=false;
		short ql,qr,qt,qb;	//quad region bounds
		short l,r,t,b;	//zone bounds
		GetBounds(l,r,t,b,ql,qr,qt,qb,zl,zr,zt,zb);
		zc=l<r&&t<b;
		return zc;
		}
	void PlaceZone(short tx,short ty,short zpatID)
		{	ZoneRef z(tx,ty,zpatID);	PlaceZone(&z);		}
	void PlaceZone(ZoneRef*z)
		{
		QuadRegion*qr=this;
		bool inchildren=false;
		for(int i=0;i<4;i++)
			if(children[i])	{
				if(children[i]->ZoneContainedCompletely(z))	{
					children[i]->PlaceZone(z);
					inchildren=true;	}	}
		if(inchildren)	return;
		else if(ZoneContainedCompletely(z))
			this->AddZone(z);
		}
	void AddZone(ZoneRef*z)
		{	zonelist.push_back(*z);	}
	void RemoveZonesAt(short rzx,short rzy)
		{
		std::list<ZoneRef>::iterator zli;
		//erase all zones in list
		for(zli=zonelist.begin();zli!=zonelist.end();zli++)
			if(zli->PointInZone(rzx,rzy))
				zonelist.erase(zli);
		//erase all zones in children's lists
		for(int i=0;i<4;i++)
			if(children[i])
				children[i]->RemoveZonesAt(rzx,rzy);
		}
	void DrawZones(BITMAP*page,short xo,short yo)
		{
		short
			dl=xo,	dt=yo
			,	dr=xo+page->w/xpixelsPerTile,	db=yo+page->h/ypixelsPerTile;
		std::list<ZoneRef> drawlist;
		std::stack<QuadRegion*> rgnstack;
		rgnstack.push(this);
		QuadRegion*rgn;

		while(!rgnstack.empty())
			{
			rgn=rgnstack.top();	rgnstack.pop();
			//step through children, and push if they are not null
			if(tw>16&&tl>16)
				{
				for(short i=0;i<4;i++)
					if(rgn->children[i])
						{
						if(rgn->children[i]->ZoneContained(dl,dt,dr,db))
							rgnstack.push(rgn->children[i]);
						}
				}
			//add zones in zone list to draw list if they lie in bounds
			std::list<ZoneRef>::iterator zli;
			cout<<"rgn->tw="<<rgn->tw<<endl;
			for(zli=rgn->zonelist.begin();zli!=rgn->zonelist.end();zli++)
				drawlist.push_back(*zli);
			}
		//sort the draw list
		drawlist.sort(ZoneRef::comparezvals_ascending);
		//draw sorted list
		std::list<ZoneRef>::iterator dli;
		for(dli=drawlist.begin();dli!=drawlist.end();dli++)
			dli->DrawZone(page,xo,yo);
		}

	void SubdivUntilSize(u16 ttw,u16 ttl)
		{
		std::stack<QuadRegion*> rgnstack;
		QuadRegion*rgn=this;
		rgnstack.push(rgn);
		while(!rgnstack.empty())
			{
			rgn=rgnstack.top();	rgnstack.pop();
			if(rgn->tw>ttw||rgn->tl>ttl)
				{
				//create new children and push
				short ctw=(rgn->tw+1)/2,	ctl=(rgn->tl+1)/2;
				rgn->children[0]
					=	new QuadRegion(tx,ty,ctw,ctl
											,rgn,collision);
				rgn->children[1]
					=	new QuadRegion(tx+tw/2,ty,ctw,ctl
											,rgn,collision);
				rgn->children[2]
					=	new QuadRegion(tx,ty+tl/2,ctw,ctl
											,rgn,collision);
				rgn->children[3]
					=	new QuadRegion(tx+tl/2,ty+tl/2,ctw,ctl
											,rgn,collision);
				cout<<"SubdivUntilSize(): rgn->tw="<<rgn->tw<<endl;
				for(short i=0;i<4;i++)
					if(rgn->children[i])
						rgnstack.push(rgn->children[i]);
				}
			}

		}
	};




int main()
	{
	//init random generator
	srand(clock());

	cout<<"zonerefsz="<<sizeof(ZoneRef)<<" quadrgnsz="<<sizeof(QuadRegion)<<endl;
	cout<<"zonepatsz="<<sizeof(ZonePattern)<<" tradedatasz="<<sizeof(TradeData)<<endl;

	//initialize Allegro
	cout<<"Initializing Allegro..."<<endl;
	allegro_init();
	cout<<"...Done"<<endl;
	install_keyboard();
	install_timer();
	set_color_depth(24);
	set_gfx_mode(GFX_AUTODETECT_WINDOWED,640,480,0,0);
	srand(time(0));

	BITMAP*page;
	page=create_bitmap(SCREEN_W,SCREEN_H);

	//init random patterns
	patterns=new ZonePatternList;
	patterns->GenerateRandom(100);


	const u16 mapTileDim=32;
	QuadRegion rootrgn(0,0,mapTileDim,mapTileDim,(QuadRegion*)0,(CollisionMap*)0);

	rootrgn.SubdivUntilSize(16,16);

	const u32 nzones=100;
	for(int i=0;i<nzones;i++)
		{
		//generate a random zone reference and add it
		u16 ztx,zty,ztw,ztl,zpatternID;
		ztx=rand()%mapTileDim;
		zty=rand()%mapTileDim;
		ztw=ztw=std::min((u16)(rand()%128),(u16)(mapTileDim-ztx));
		ztl=ztw=std::min((u16)(rand()%128),(u16)(mapTileDim-zty));
		zpatternID=rand()%patterns->amount();
		ZoneRef zr(ztx,zty,ztw,ztl,zpatternID);
		rootrgn.PlaceZone(&zr);
		}

	//variables for while loop
	bool done=false,changed=true;
	short txo=0,tyo=0;

	while(!done)
		{

		if(keypressed())
			{
			//key input
			if(key[KEY_ESC])		done=true;
			if(key[KEY_UP])		--tyo;
			if(key[KEY_DOWN])		++tyo;
			if(key[KEY_LEFT])		--txo;
			if(key[KEY_RIGHT])	++txo;
			if(key[KEY_HOME])		txo=tyo=0;
			changed=true;
			rest(25);
			}

		if(changed)
			{
			//draw the zones
			clear_to_color(page,0xff00ff);
			rootrgn.DrawZones(page,txo,tyo);

			acquire_screen();
			blit(page,screen,0,0,0,0,page->w,page->h);
			release_screen();
			changed=false;
			}
		}

	//exit
	return 0;
	}
