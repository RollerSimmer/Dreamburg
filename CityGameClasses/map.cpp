#include "map.h"

#define map_implementation 1
#if map_implementation

#include <list>

namespace Map
	{

	/*

	To do:
		+	make sure this program works!
	In Progress:
		+	getting the stack to draw in proper order


	********** Update Log *******************
	2011:Aug:23:PM:04:41
		+	Started this file
	2011:Aug:24:PM:01:03
		+	DrawStackEntry struct created
		+	added a draw stack to Map::DrawMap()
	2011:Aug:28:PM:12:45
		+	Made patternindex struct have both index and offset
		+	now draws, but not necessarily in order
		+	need to make sure all tiles are added to stack
	*/

	Map::Data::
		Data(u16 stw,u16 stl)
			{
			zonelist=new ZonePatternList();
			tw=stw;	tl=stl;
			map=new MapPatternIndex::Struct[stw*stl*maxstack];

			for(int y=0;y<tl;y++)
			for(int x=0;x<tw;x++)
			for(int z=0;z<maxstack;z++)
				{
				MapPatternIndex::Struct*mpi=mapat(x,y,z);
				mpi->patidx=MapPatternIndex::empty;
				mpi->stackidx=z&7;
				mpi->patxofs=mpi->patyofs=mpi->patquad=0;
				}

			}

	Map::Data::
		~Data()
			{
			delete[]map;
			delete zonelist;
			}

	void
	Map::Data::
		AddZone(short tx,short ty,u16 patidx,u16 color1,u16 color2,u16 color3)
			{
			//note: for now everything will be

			// first check for proper range
			if(patidx==MapPatternIndex::empty)	return;
			if(patidx>=zonelist->nlistentries)
				{
				cout<<"Map::Data::AddZone() error \n\t";
				cout<<"pattern index ("<<patidx<<") is OOB"<<endl;
				cout<<"\tnpatterns()="<<npatterns()<<endl;

				return;
				}
			ZonePattern*zp=&zonelist->plist[patidx];
			u16 zpw=zp->tw,zpl=zp->tl;
			if(tx<0||ty<0||tx+zpw-1>=tw||ty+zpl-1>=tl)	return;
			if(	mapat(tx,ty,0)->patidx!=MapPatternIndex::empty	)
				return;

			//now add it
			u16 nquads=zp->nquads();
			for(short cty=ty;cty<ty+zpl;cty++)
			for(short ctx=tx;ctx<tx+zpw;ctx++)
				{
				//reset stack index to zero each tx,ty point
				u16 stackidx=0;

				//step through quads
				for(short ctq=0;ctq<nquads;ctq++)
					{
					//init data for quad
					MapPatternIndex::Struct*mpi;
					bool placefree=false;
					bool endofstack=stackidx>=maxstack;

					//look for a free slot in stack to add tile
					while(!placefree&&!endofstack)
						{
						mpi=mapat(ctx,cty,stackidx);
						placefree=mpi->patidx==MapPatternIndex::empty;
						if(!placefree)
							stackidx++;
						endofstack=stackidx>=maxstack;
						};
					if(endofstack)
						cout<<"end of stack reached at ("
						    <<ctx<<","<<cty<<")"<<endl;

					//check for empty tiles
					bool		wasset=false;
					endofstack=stackidx>=maxstack;

					while(!wasset&&!endofstack)
						{
						mpi=mapat(ctx,cty,stackidx);
						////if(	mpi->patidx==MapPatternIndex::empty)
							//this tile is empty, proceed adding index
							{
							//draw all zone pattern references as offsets at first
							mpi->patidx=patidx;
							mpi->patxofs=ctx-tx;
							mpi->patyofs=cty-ty;
							mpi->stackidx=stackidx;
							mpi->patquad=ctq;
							mpi->colorremap1=color1;
							mpi->colorremap2=color2;
							mpi->colorremap3=color3;
							#define addzone_showstatus 1
							#if addzone_showstatus
								cout<<"pat "<<patidx<<" added at tile("<<ctx<<","<<cty<<"); ";
								cout<<"ofs=("<<mpi->patxofs<<","<<mpi->patyofs<<")";
								cout<<" stack="<<mpi->stackidx<<endl;
							#endif
							wasset=true;
							}
						++stackidx;
						endofstack=stackidx>=maxstack;
						}
					}
				}
			}

	ZonePattern*Map::Data::Zone(int zpati)
		{
		return zonelist->Zone(zpati);
		}

	void
	Map::Data::
		DrawMap(BITMAP*page,short tltx,short tlty)
			{
			u16 pageLength=page->h;
			u16 pageWidth=page->w;

			std::list<DrawStackEntry>	drawstack;

			for(short py=0;py<pageLength;py+=ypixelsPerTile)
			for(short px=0;px<pageWidth;px+=xpixelsPerTile)
				{
				short tx=tltx+px/xpixelsPerTile, ty=tlty+py/ypixelsPerTile;
				if(tx>=0&&ty>=0&&tx<tw&&ty<tl)
					{
					//draw a tile

					//but first, build a draw stack of non-empty quads
					drawstack.clear();
					for(short stackidx=0;stackidx<Map::maxstack;stackidx++)
						{
						MapPatternIndex::Struct*mpi=mapat(tx,ty,stackidx);
						if(	mpi->patidx==MapPatternIndex::empty)
							{
							//don't add to stack
							}
						else
							{
							//do add to stack

							DrawStackEntry
								dse(	mpi	);
							drawstack.push_back(dse);
							}
						}
					drawstack.sort(DrawStackEntry::compare_ascend);
					#define drawmap_showtilestacksz 1
					#if drawmap_showtilestacksz
						cout<<"drawmap@("<<tx<<","<<ty<<"): stacksz=";
						cout<<drawstack.size()<<endl;
					#endif


					while(!drawstack.empty())
						{
						//set mpi to back of stack
						DrawStackEntry dse=drawstack.back();
						MapPatternIndex::Struct*mpi;
						mpi=dse.mpi;
						drawstack.pop_back();

						u16 patidx,patxofs,patyofs,patquad;

						patidx=mpi->patidx;
						patxofs=mpi->patxofs;
						patyofs=mpi->patyofs;
						patquad=mpi->patquad;

						if(patidx!=MapPatternIndex::empty)
							//draw the tile
							{
							ZonePattern*zp=&zonelist->plist[patidx];

							BITMAP*sprbmp=zp->sprite(patxofs,patyofs,patquad);

							//place sprite on page with offset figured by sprite dimensions
							short pxo,pyo;
							pxo=(short)xpixelsPerTile-sprbmp->w;
							pyo=(short)ypixelsPerTile-sprbmp->h;
							draw_sprite(page,sprbmp,px+pxo,py+pyo);
							}
						}
					}
				}
			}

	void
	Map::Data::
		AddRandPattern(u16 maxsz,u16 maxfloors,const char*patname,u32 color)
			{
			u8 tw=rand()%maxsz+1;
			u8 tl=rand()%maxsz+1;
			u8 nfloors=rand()%maxfloors+1;
			bool loadspcx=false;
			bool usestq=false;
			ZonePattern zp(tw,tl,nfloors,"",patname,loadspcx,usestq);

			//set to full tile occupancy for now
			zp.SetFullOccupancy();

			//make the zones simple image (stub for mor advanced stuff)
			BITMAP*fullimg;
			fullimg=zp.GenerateBaseImage(color,~color,color);
			zp.CutupImage(fullimg);

			//finally add the pattern to the map's zone list
			zonelist->AddPattern(zp);

			#define addrandpattern_drawwait 0
			#if addrandpattern_drawwait
				acquire_screen();
				clear_to_color(screen,0x8f7f6f);
				zp.DrawWholeZone(screen,100,100);
				release_screen();
				readkey();
			#endif
			}

	u16
	Map::Data::
		npatterns()
			{
			return zonelist->nlistentries;
			}

	MapPatternIndex::Struct*Map::Data::mapat(int x,int y,int si)
		{
		if(ptonmap(x,y,si))
			{
			return &map[(y*tw+x)*maxstack+si];
			}
		}

	bool Map::Data::ptonmap(int x,int y,int si)
		{
		return x>=0&&x<tw&&y>=0&&y<tl&&si>=0&&si<maxstack;
		}
	}	//namespace Map

#endif
