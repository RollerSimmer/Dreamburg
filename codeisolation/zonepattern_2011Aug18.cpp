#include "zonepattern.h"
#include "tilesize.h"

#include <sstream>
#include <iostream>

using namespace std;

ZonePatternList*patterns;

///*** ZonePatternList - implementation ***
#define ZonePattern_implementation 1
#if ZonePattern_implementation

ZonePattern::
	ZonePattern(u8 stw,u8 stl,u8 snfloors,char*pcxfile
	            ,char*patname,bool loadpcxfile
	            ,bool susesTriQuads)
			:	sprites(stw*stl*(susesTriQuads?4:1),(BITMAP*)0)
			,	occubytes((stw*stl*4+7)/8,0)
		{
		tw=stw;	tl=stl;	nfloors=snfloors;
		usesTriQuads=susesTriQuads;
		strncpy(name,patname,12);
		//try to load a pcx file
		BITMAP*wholeimg;
		if(loadpcxfile)
			{
			#if 0
			wholeimg=load_pcx(pcxfile,0);
			#endif
			}
		else
			{
			// if no pcx, create a box with the name in it
			u32 bgc,fgc;
			u32 bw=tw*xpixelsPerTile;
			u32 bl=tl*ypixelsPerTile;
			#if 0
			sprite=create_bitmap(bw,bl);
			if(sprite&&bw>0&&bl>0)
				{
				fgc=0x000000;
				bgc=0xffffff;
				clear_to_color(sprite,fgc);
				rectfill(sprite,1,1,sprite->w-2,sprite->h-2,bgc);
				short xo=2;
				short yo=(sprite->h+1)/2-4;
				textout_ex(sprite,font,name,xo,yo,fgc,bgc);
				std::stringstream ss;
				ss.str("");	yo+=8;
				ss<<"("<<tw<<","<<tl<<","<<(short)nfloors<<")";
				textout_ex(sprite,font,ss.str().c_str(),xo,yo,fgc,bgc);
				////draw_trans_sprite(sprite,screen,0,0);
				#if 0
					//test out the image (delete this later)
					static short i=0;
					cout<<"drawing sprite "<<(i++)<<"..."<<endl;
					acquire_screen();
					clear_to_color(screen,0);
					blit(sprite,screen,0,0,0,0,sprite->w,sprite->h);
					release_screen();
					readkey();
				#endif
				}
			#endif
			}
		//initialize sprites to NULLs
		int nsprites=tw*tl*ImgsPerTile();
		sprites.empty();
		for(int i=0;i<nsprites;i++)
			sprites.push_back((BITMAP*)0);
		#if 1
			if(sprites.size()==nsprites)
				cout<<"ZonePattern: there are exactly enough sprite ptrs"<<endl;
			else if(sprites.size()>nsprites)
				cout<<"ZonePattern: there are enough sprite ptrs"<<endl;
			else if(sprites.size()<nsprites)
				cout<<"ZonePattern: there aren't enough sprite ptrs"<<endl;
		#endif
		int nquads=tw*tl*4;
		for(int i=0;i<(nquads+7)/8;i++)
			occubytes.push_back(0);
		SetFullOccupancy();
		}


ZonePattern::
	~ZonePattern()
		{
		#if 0
		if(sprite)	release_bitmap(sprite);
		#endif
		}

void
ZonePattern::
	SetFullOccupancy()
		{
		for(short ty=0;ty<tl;ty++)
		for(short tx=0;tx<tw;tx++)
		for(short tq=0;tq<4;tq++)
			occupied(tx,ty,tq,true);
		}

void
ZonePattern::
	SetRandOccupancy(u16 truewgt,u16 falsewgt)
		{
		u16 totwgt=truewgt+falsewgt+1;
		u16 rv;
		bool state;
		for(short ty=0;ty<tl;ty++)
		for(short tx=0;tx<tw;tx++)
			if(usesTriQuads)
				{
				for(short tq=0;tq<4;tq++)
					{
					rv=rand()%totwgt;
					state=rv<=truewgt;
					occupied(tx,ty,tq,state);
					}
				}
			else
				{
				rv=rand()%totwgt;
				state=rv<=truewgt;
				for(short tq=0;tq<4;tq++)
					occupied(tx,ty,tq,state);
				}
		}

void
ZonePattern::
	occupied(short tx,short ty,short tq,bool state)
		{
		u32 abiti=((ty*tw+tx)*4+tq);
		u32 bytei=abiti/8;
		u16 biti=abiti%8;
		u16 bitmask=1<<abiti;
		u16 statemask=(state&1)<<abiti;
		occubytes[bytei]&=~bitmask;
		occubytes[bytei]|=statemask;
		}

bool
ZonePattern::
	occupied(short tx,short ty,short tq)
		{
		u32 abiti=((ty*tw+tx)*4+tq);
		u32 bytei=abiti/8;
		u16 biti=abiti%8;
		u16 bitmask=1<<abiti;
		bool state=(occubytes[bytei]&bitmask)>>abiti;
		}

#endif
//!*** ZonePatternList - implementation ***

///*** ZonePatternList - implementation ***
#define ZonePatternList_implementation 1
#if ZonePatternList_implementation

ZonePatternList::
	ZonePatternList()
		:	plist(maxzonepatterns,ZonePattern(0,0,0,"","empty",false))
		{
		plist.clear();
		nlistentries=0;
		ZonePattern nullpattern(0,0,0,"","nullpat",false);
		plist.push_back(nullpattern);
		}

bool
ZonePatternList::
	AddPattern(	ZonePattern&pat	)
		{
		if(amount()<=maxzonepatterns)			{
			++nlistentries;
			plist.push_back(pat);
			////cout<<"plist.size()="<<plist.size()<<endl;
			////cout<<"plist.capacity()="<<plist.capacity()<<endl;
			return true;		}
		else
			return false;
		}

short
ZonePatternList::
	amount()
		{
		////return list.size();
		return nlistentries;
		}


void
ZonePatternList::
	GenerateRandom(short npats)
		{
		char names[][13]=
			{
			"houses","apts","offices","stadium","arena","hospital"
			,"policedept","firedept","cityhall","stcapitol","mill"
			,"manuplant","mall"
			};
		const short nnames=sizeof(names)/sizeof(names[0]);
		for(int i=0;i<npats;i++)
			{
			u16 rtw=rand()%8+12,	rtl=rand()%8+12,	rnfloors=rand()%128;
			short ni=rand()%nnames;
			std::cout<<"Generating random zone #"<<i<<"..."<<std::endl;
			ZonePattern*zp=new ZonePattern(rtw,rtl,rnfloors,"",names[ni],false);
			AddPattern(*zp);
			delete zp;
			}
		}

ZonePattern&
ZonePatternList::
	operator[](int i)
		{
		if(i<amount())	return plist[i];
		else return plist[nullpatternID];
		}


#endif
//!*** ZonePatternList - implementation ***

