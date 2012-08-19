/**********************************************************************
**             Allegro game library project template                 **
**********************************************************************/

/*

To do:
	+ draw patterns over a map and put in a scroll function
In Progress:
	+ make a generic list of colored sprites for a zone pattern

********** Update Log *******************
2011:Jul:25:PM:08:43
	+	Added some more patterns to the map
	+	Should add some tiles that overlap the base tile (for height dim)
*/

#include <iostream>
#include <allegro.h>
#include <types/mytypes.h>
#include <cstdlib>
#include <ctime>
#include <list>
#include <sstream>

using namespace std;

const u16 tilePxSz=32;

const u16 pageWidth=640;
const u16 pageLength=480;

const u16 pageTileWidth=pageWidth/tilePxSz;
const u16 pageTileLength=pageLength/tilePxSz;

struct TradeBalance
	{
	u8	money	:	4	;
	u8	rawmaterial	:	4	;
	u8	unfinishedgoods	:	4	;
	u8	finishedgoods	:	4	;
	u8	lodging	:	4	;
	u8	dining	:	4	;
	u8	shopping	:	4	;
	u8	entertainment	:	4	;
	u8	transportation	:	4	;
	u8	government	:	4	;
	u8	creativity	:	4	;
	u8	laworder	:	4	;
	u8	safety	:	4	;
	u8	health	:	4	;
	u8	knowledge	:	4	;
	u8	faith	:	4	;
	u8	pollution	:	4	;
	u8	reserved	:	4	;
	};

struct	ZonePattern
	{
	//data
	u8	tlWd	;
	u8	tlLn	;
	u8	nStories	;
	u8	nResidentsPerTile	:	4	;
	u8	ResWS1Val	:	4	;
	u8	pctWS1res	:	4	;
	u8	ResWS2Val	:	4	;
	u8	pctWS2res	:	4	;
	u8	flagIsRented	:	1	;
	u8	flagHasYard	:	1	;
	u8	flagHasGarage	:	1	;
	u8	flagPrivateDrives	:	1	;
	u8	nJobsPerTIle	:	4	;
	u8	jobEduScr1val	:	4	;
	u8	jobEduScr1Salary	:	4	;
	u8	pctEduScr1jobs	:	4	;
	u8	jobEduScr2val	:	4	;
	u8	jobEduScr2Salary	:	4	;
	u8	pctEduScr2jobs	:	4	;
	u8	jobEduScr3val	:	4	;
	u8	jobEduScr3Salary	:	4	;
	u8	pctEduScr3jobs	:	4	;
	u8	flagIsSchool	:	1	;
	u8	SchoolESentryLvl	:	3	;
	u8	schoolESgradLvl	:	4	;
	u8	schoolStudentsPerTile	:	4	;
	u8	schoolTuitionRate	:	3	;
	u8	flagSchoolPrivate	:	1	;
	TradeBalance tradebal;
	u8	flagUseTriTiles	:	1	;
	u8	flagBuildOver	:	1	;
	u8	fill178	:	6	;
	u8	fill184	:	4	;
	u8	fill188	:	4	;
	u8	fill192	:	4	;
	u8	fill196	:	4	;
	u8	fill200	:	4	;
	u8	fill204	:	4	;
	u16 patternIndex;
	BITMAP**	patternSprites	;
	//methods


	void UsesTriTiles(bool ut)	{	flagUseTriTiles=ut;	}


	bool UsesTriTiles()	{	return(bool)flagUseTriTiles;	}


	ZonePattern(string&file)	{			set(file,true);		}
	ZonePattern()	{			string file="";	set(file,true);		}

	void set(string&file,bool init)
		{
		if(!init)	kill();
		//set the dummy zone pattern size to 3
		tlLn=tlWd=3;
		UsesTriTiles(false);
		patternSprites=new BITMAP*[GetSpriteAmt()];
		//make a generic  blocks
		MakeGenericColorBlocks();
		PatternIndex(0);
		}

	~ZonePattern(){	kill();	}

	void kill()	{	delete[]patternSprites;	}


	u16 GetSpriteAmt()
		{	u16 triMul=(UsesTriTiles())?4:1;	return tlLn*tlWd*triMul;	}


	void MakeGenericColorBlocks()
		{
		// make a bunch of randomly colored blocks and put make a tile out of it
		// be sure to label the block with a number
		u16 nsprites=GetSpriteAmt();
		stringstream ss("");
		u32 color=rand()&0x00f0f0f0;

		short htpxsz=tilePxSz+rand()%tilePxSz;

		for(int i=0;i<nsprites;i+=1)
			{
			short tx=i%tlWd,	ty=i/tlLn;
			short pxw,pxl;
			//set top or left edges to dynamic wd or ht, otherwise fixed size
			if(tx==0)	pxw=htpxsz;	else	pxw=tilePxSz;
			if(ty==0)	pxl=htpxsz;	else	pxl=tilePxSz;
			//create the bitmap, fill with color, and write tile code
			patternSprites[i]=create_bitmap(pxw,pxl);
			BITMAP*ps=patternSprites[i];
			bool transparent=false;//=rand()&1;
			if(transparent)	color=0x00ff00ff;
			clear_to_color(ps,color);
			u16 borderSz=3;
			u16 factor=75;
			rectfill(ps,0,0,ps->w-1,borderSz,ZonePattern::ColorDiv(color,factor));	//top line
			rectfill(ps,0,0,borderSz,ps->h-1,ZonePattern::ColorDiv(color,factor));	//left line
			rectfill(ps,0,ps->h-borderSz-1,ps->w-1,ps->h-1,ZonePattern::ColorMul(color,factor));	//bottom line
			rectfill(ps,ps->w-borderSz-1,0,ps->w-1,ps->h-1,ZonePattern::ColorMul(color,factor));	//left line
			ss.str("");	ss<<"t"<<i;
			textout_ex(ps,font,ss.str().c_str(),borderSz,borderSz,color,0xffffff);
			}
		}

	static u32 ColorMul(u32 color,u16 factor)
		{
		factor=std::min(factor,(u16)100);
		union ColorMulUnion {	u32 color;	u8 colorbytes[4];	};
		ColorMulUnion cm;	cm.color=color;
		for(int i=0;i<3;i++)
			cm.colorbytes[i]=(u32)cm.colorbytes[i]*factor/100;
		return cm.color;
		}
	static u32 ColorDiv(u32 color,u16 factor)
		{
		factor=std::min(factor,(u16)100);
		union ColorDivUnion {	u32 color;	u8 colorbytes[4];	};
		ColorDivUnion cd;	cd.color=color;
		for(int i=0;i<3;i++)
			cd.colorbytes[i]=std::min(((u32)cd.colorbytes[i]*100/factor),(u32)255);
		return cd.color;
		}


	void PatternIndex(u16 idx)	{	patternIndex=idx;		};
	u16 PatternIndex()		{	return patternIndex;		};
	};

namespace MapPatternIndex
	{
	enum{index=0,offset=1};
	enum{empty=0,first=1};

	struct Struct
		{
		union
			{
			struct
				{	u16	indexOrOfs:1;	u16 	patternYofs:7;
					u16	patternXofs:7;	u16	stackIndex:1;	};
			struct
				{	u16	iooFill:1;		u16	patternIndex:14;
					u16	siFill:1;	};
			};
		};
	}

const u16 mapsz=16;
const u16 maxpatterns=16;

struct Map
	{
	u16 tw,tl;
	u16 npatterns;

	ZonePattern znpats[maxpatterns];
	MapPatternIndex::Struct map[mapsz][mapsz][2];
	Map(u16 stw,u16 stl,u16 snpatterns)
		{
		tw=stw;	tl=stl;
		npatterns=snpatterns;
		for(int y=0;y<tl;y++)
		for(int x=0;x<tw;x++)
		for(int z=0;z<2;z++)
			{
			map[y][x][z].indexOrOfs=MapPatternIndex::index;
			map[y][x][z].patternIndex=MapPatternIndex::empty;
			map[y][x][z].stackIndex=z&1;
			}
		}

	void AddPattern(short tx,short ty,u16 patidx)
		{
		//note: for now everything will be

		// first check for proper range
		if(patidx==MapPatternIndex::empty)	return;
		if(patidx>=npatterns)	return;
		ZonePattern*zp=&znpats[patidx];
		u16 zpw=zp->tlWd,zpl=zp->tlLn;
		if(tx<0||ty<0||tx+zpw-1>=tw||ty+zpl-1>=tl)	return;
		if(		(map[ty][tx][0].indexOrOfs==MapPatternIndex::index)
				&&	(map[ty][tx][0].patternIndex!=MapPatternIndex::empty)	)
			return;

		//now add it
		for(short cty=ty;cty<ty+zpl;cty++)
		for(short ctx=tx;ctx<tx+zpw;ctx++)
			{
			MapPatternIndex::Struct*mpi=&map[cty][ctx][0];
			//draw all zone pattern references as offsets at first
			mpi->indexOrOfs=MapPatternIndex::offset;
			mpi->patternXofs=ctx-tx;
			mpi->patternYofs=cty-ty;
			mpi->stackIndex=0;
			}
		//now set the origin tile to the pattern index
		MapPatternIndex::Struct*mpi=&map[ty][tx][0];
		mpi->indexOrOfs=MapPatternIndex::index;
		mpi->patternIndex=patidx;
		mpi->stackIndex=0;
		}

	void DrawMap(BITMAP*page,short tltx,short tlty)
		{
		for(short py=0;py<pageLength;py+=tilePxSz)
		for(short px=0;px<pageWidth;px+=tilePxSz)
			{
			short tx=tltx+px/tilePxSz, ty=tlty+py/tilePxSz;
			if(tx>=0&&ty>=0&&tx<tw&&ty<tl)
				{
				//draw a tile
				MapPatternIndex::Struct*mpi=&map[ty][tx][0],*mpio;
				u16 patidx,patxofs,patyofs,patquad;
				if(mpi->indexOrOfs==MapPatternIndex::index)
					{	patidx=mpi->patternIndex;
						patxofs=patyofs=patquad=0;	}
				else	//if(mpi->indexOrOfs==MapPatternIndex::offset)
					{
					patxofs=mpi->patxofs;
					patyofs=mpi->patyofs;
					patquad=mpi->patquad;
					mpio=&map[ty-patyofs][tx-patxofs][0];
					//index? add index: not index? set empty
					if(mpio->indexOrOfs==MapPatternIndex::index)
						patidx=mpio->patternIndex;
					else
						patidx=MapPatternIndex::empty;
					}
				if(patidx!=MapPatternIndex::empty)
					//draw the tile
					{
					ZonePattern*zp=&znpats[patidx];

					u16 spridx;
					spridx=patyofs*zp->tlWd+patxofs;
					if(zp->UsesTriTiles())
						spridx*=4;

					BITMAP*sprbmp=zp->patternSprites[spridx];
					//place sprite on page with offset figured by sprite dimensions
					short pxo,pyo;
					pxo=(short)tilePxSz-sprbmp->w;
					pyo=(short)tilePxSz-sprbmp->h;
					draw_sprite(page,sprbmp,px+pxo,py+pyo);
					}
				}
			}
		}

	};

typedef std::list<u16> T;

struct BitFieldTest
	{
	union
		{
		u8 byt;
		struct	{	u8 nib1:4;	u8 nib2:4;};
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
	Map*map=new Map(mapsz,mapsz,maxpatterns);

	int i=1;
	map->AddPattern(0,0,i++);
	map->AddPattern(4,0,i++);
	map->AddPattern(0,4,i++);
	map->AddPattern(5,6,i++);
	map->AddPattern(8,0,i++);
	map->AddPattern(1,2,i++);
	map->AddPattern(0,10,i++);
	map->AddPattern(3,10,i++);
	map->AddPattern(6,10,i++);
	map->AddPattern(9,10,i++);
	map->AddPattern(12,10,i++);
	map->AddPattern(15,10,i++);

	short tx=0,ty=0;

	//loop for input
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
			}


		acquire_bitmap(page);

		//clear the screen w/ white
		clear_to_color(page,0xffffffff);

		//draw here
		map->DrawMap(page,tx,ty);

		release_bitmap(page);

		//page flip
		acquire_screen();
		blit(page,screen,0,0,0,0,SCREEN_W,SCREEN_H);
		release_screen();


		}

	delete map;

	#endif
	#if 0
	cout<<"Tsz="<<sizeof(T)<<endl;

	#endif
	}
