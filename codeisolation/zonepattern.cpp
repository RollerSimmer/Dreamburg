#include "zonepattern.h"
#include "tilesize.h"

#include <sstream>
#include <iostream>

using namespace std;

////ZonePatternList*patterns;

///*** ZonePatternList - implementation ***
#define ZonePattern_implementation 1
#if ZonePattern_implementation

ZonePattern::
	ZonePattern()
			:	sprites(0*0*0,(BITMAP*)0)
			,	occubytes((0*0*4+7)/8,0)
		{
		tw=tl=1;	nfloors=0;
		usesTriQuads=false;
		strncpy(name,"empty",12);
		}

ZonePattern::
	ZonePattern(u8 stw,u8 stl,u8 snfloors,const char*pcxfile
	            ,const char*patname,bool loadpcxfile
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
		u32 abiti=(ty*tw+tx)*4+tq;
		u32 bytei=abiti/8;
		u16 biti=abiti%8;
		u16 bitmask=1<<biti;
		u16 statemask=(state&1)<<biti;
		occubytes[bytei]&=~bitmask;
		occubytes[bytei]|=statemask;
		}

bool
ZonePattern::
	occupied(short tx,short ty,short tq)
		{
		u32 abiti=(ty*tw+tx)*4+tq;
		u32 bytei=abiti/8;
		u16 biti=abiti%8;
		u16 bitmask=1<<biti;
		bool state=(occubytes[bytei]&bitmask)>>biti;
		}

BITMAP*
ZonePattern::
	GenerateOccupancyImage(u32 fgc,u32 bgc,u16 tilepixsz)
		{
		tilepixsz=std::max((u16)(tilepixsz*2/2),(u16)4);
		BITMAP*oimg=create_bitmap(tw*tilepixsz,tl*tilepixsz);
		if(!oimg)
			{
			cout<<"Occupancy image not created"<<endl;
			return oimg;
			}
		clear_to_color(oimg,bgc);
		const short ndims=2;
		const short ncorners=3;
		const short ntris=4;

		short triofss[ntris][ncorners*ndims]=
			{
			//top
				{0,0,tilepixsz/2,tilepixsz/2,tilepixsz-1,0}
			//left
			,	{0,0,tilepixsz/2,tilepixsz/2,0,tilepixsz-1}
			//right
			,	{tilepixsz-1,0,tilepixsz/2,tilepixsz/2,tilepixsz-1,tilepixsz-1}
			//bottom
			,	{0,tilepixsz-1,tilepixsz/2,tilepixsz/2,tilepixsz-1,tilepixsz-1}
			};

		short ix,iy;

		for(int ty=0;ty<tl;ty++)
		{
		for(int tx=0;tx<tw;tx++)
		{
		for(int tq=0;tq<4;tq++)
			{
			if(occupied(tx,ty,tq))
				{
				ix=tx*tilepixsz;	iy=ty*tilepixsz;
				triangle(oimg,	ix+triofss[tq][0],iy+triofss[tq][1]
							,ix+triofss[tq][2],iy+triofss[tq][3]
							,ix+triofss[tq][4],iy+triofss[tq][5]
							,fgc);
				#if 0
					acquire_screen();
					clear_to_color(screen,0);
					blit(oimg,screen,0,0,0,0,oimg->w,oimg->h);
					release_screen();
					cout<<"oimg: ix="<<ix<<" iy="<<iy<<" fgc="<<fgc;
					cout<<" oimg->w="<<oimg->w<<" oimg->l="<<oimg->h;
					cout<<" tw="<<tw<<" tl="<<tl<<endl;
					////readkey();
				#endif
				}
			}//for tq
		}//for tx
		}//for ty
		return oimg;
		}


void
ZonePattern::
	CutupImage(BITMAP*baseimg)
		{

		class Cutup
			{
			public:
			static void DisplayRawImages(BITMAP*bimg,BITMAP*dimg)
				{
				#define dodisplayrawimage 0
				#if dodisplayrawimage
					//display the base img copy and precrop draw img
					acquire_screen();
					clear_to_color(screen,0);
					blit(bimg,screen,0,0,0,0,bimg->w,bimg->h);
					blit(dimg,screen,0,0,bimg->w,0,dimg->w,dimg->h);
					release_screen();
					readkey();
				#endif
				}
			static void ClearBaseImgDots(BITMAP*bimg,BITMAP*dimg
			                             ,short lft,short top
			                             ,short rgt,short btm
			                             ,bool usetriquads)
				{
				#define doclearbaseimg 0
				#if doclearbaseimg
					//clear drawn parts on read img
					const u32 bgcolor=0xff00ff;
					short btma1=btm+1,	rgta1=rgt+1;
					if(usetriquads)
						{
						for(int ciy=top;ciy<btma1;ciy++)
						for(int cix=lft;cix<rgta1;cix++)
							{
							u32 dot=getpixel(dimg,cix,ciy);
							if(dot!=bgcolor)
								putpixel(bimg,cix,ciy,bgcolor);
							}	//end of for(cix and ciy)
						}
					else
						{
						rectfill(bimg,lft,top,rgt,btm,bgcolor);
						}

					DisplayRawImages(bimg,dimg);
					////cout<<"ClearBaseImgDots(): npixels="<<((btm-top+1)*(rgt-lft+1))<<endl;
				#endif
				}
			static void FindPixelTop(short ix,short&iy,short&top
			                    ,BITMAP*bimg,BITMAP*dimg)
				{
				/*
				move up from top of tile position until blank
				or top of image is reached
				*/
				u16 il=0;
				u32 cdot;
				const u32 bgcolor=0xff00ff;
				while((cdot=getpixel(bimg,ix,iy))!=bgcolor)
					{
					////putpixel(dimg,ix,iy,cdot);
					////putpixel(bimg,ix,iy,bgcolor);
					top=std::min(top,iy);
					--iy;
					++il;
					if(iy<0)	break;
					}
				//just blit the whole strip
				blit(bimg,dimg,ix,top,ix,top,1,il);
				}

			static void FindOccupancyTop(short tx,short ty
				                          ,short lft,short&top
			                             ,short rgt,short btm
												  ,bool lside,bool rside
												  ,ZonePattern&zone
												  ,BITMAP*bimg,BITMAP*dimg)
				{
				short tty=ty;	short svtop=top;
				bool occuflag=false;	bool loopinit=true;
				bool exitflag=false;
				u16 occucount;
				while(tty>0&&!exitflag)
					{
					if(loopinit)
						//unset loopinit if set
						{
						loopinit=false;
						exitflag=false;
						}
					else
						{
						//init values
						exitflag=false;
						--tty;
						occucount=0;

						//tally occupied quads
						for(int tq=0;tq<4;tq++)
							if(zone.occupied(tx,tty,tq))
								{
								/*	add to occupancy count based on quad
									and half flags	*/
								if(	tq==0||tq==3			//	top and btm cover both sides
										||(tq==1&&lside)		//	left side flag and quad=lft
										||(tq==2&&rside)	)	//	right side flag and quad=rgt
									occucount++;
								}

						//if occucount<4 and tty==0, Find pixel top
						if(occucount<4&&tty==0)
							{
							#define pixelfind_findexacttop 0
							#if pixelfind_findexacttop
								short ixfirst=tx*xpixelsPerTile;
								short ixlast=(tx+1)*xpixelsPerTile-1;
								top-=ypixelsPerTile;
								short iy=top-1;
								for(short ix=ixfirst;ix<=ixlast;ix++)
									FindPixelTop(ix,iy,top,bimg,dimg);
							#else
								top=0;
							#endif
							exitflag=true;
							}
						//if the tile is fully empty, you need to keep going,
						else if(occucount==0)
							{
							top-=ypixelsPerTile;
							exitflag=false;
							}
						//if partially occupied tile, exit but move tile y and top up
						else if(occucount<4)
							{
							top-=ypixelsPerTile;
							exitflag=true;
							}
						//if full tile, only move tty up
						else if(occucount>=4)
							{
							exitflag=true;
							}
						}
					}
				blit(bimg,dimg,lft,top,lft,top,(rgt-lft)+1,(svtop-top)+1);
				}
			static void DisplayProgress(u32 tileimgidx,u32 ntileimgs)
				{
				static short svprogpct=0;
				short progpct=tileimgidx*100/ntileimgs;
				short absprodiff=std::abs(progpct-svprogpct);
				if(absprodiff>=10||tileimgidx==1||tileimgidx==ntileimgs)
					{
					stringstream ss;
					ss.str("");
					ss<<"Cutup progress: "<<tileimgidx<<"/"<<ntileimgs;
					ss<<" ("<<progpct<<"%) "<<endl;
					cout<<ss.str();
					#if 1	//text out to allegro screen
						acquire_screen();
						textout_ex(screen,font,ss.str().c_str()
						           ,0,0,0xffffff,0);
						release_screen();
					#endif
					svprogpct=progpct;
					}
				}
			static void memputpixel(BITMAP*bmp,int x,int y,long color)
				{
				if(get_color_depth()==32)
					((long*)bmp->line[y])[x]=color;
				else
					putpixel(bmp,x,y,color);
				}
			static long memgetpixel(BITMAP*bmp,int x,int y)
				{
				if(get_color_depth()==32)
					return ((long*)bmp->line[y])[x];
				else
					return getpixel(bmp,x,y);
				}

			};


		u32 ntileimgs=nsprites();
		u32 tileimgidx=0;

		//make a copy of the base image to copy
		BITMAP*bimg=create_bitmap(baseimg->w,baseimg->h);
		blit(baseimg,bimg,0,0,0,0,bimg->w,bimg->h);
		//make an image of same size to place uncropped tile img
		BITMAP*dimg=create_bitmap(baseimg->w,baseimg->h);

		acquire_bitmap(bimg);
		acquire_bitmap(dimg);

		for(short ty=tl-1;ty>=0;ty--)
		for(short tx=tw-1;tx>=0;tx--)
			{

			//do quads
			u16 nquads=ImgsPerTile();

			short bx,by;	//base x and y of tile(0,0)

			bx=0;
			by=(short)bimg->h-tl*ypixelsPerTile;

			if(nquads==1)	//whole tiles
				{
				//initialize draw image to background color
				const u32 bgcolor=0xff00ff;
				clear_to_color(dimg,bgcolor);

				short tbx,tby;	//tile base x and y to draw
				tbx=bx+tx*xpixelsPerTile;
				tby=by+ty*ypixelsPerTile;
				short ix,iy;	//image x and y to draw


				iy=tby;	ix=tbx;
				//init bounds
				short lft,rgt,top,btm;	//crop coords;
				lft=ix;	rgt=ix+xpixelsPerTile-1;
				top=iy;	btm=iy+ypixelsPerTile-1;

				if(!occupied(tx,ty,0))
					{
					//not occupied - do nothing

					}
				else
					{
					//occupied - do the following:

					//blit the base of the tile
					blit(bimg,dimg,ix,iy,ix,iy,
						  xpixelsPerTile,ypixelsPerTile);

					//clear the tile base from read/base image
					////rectfill(bimg,lft,top,rgt,btm,bgcolor);

					//if at tile top, use find top function
					if(ty==0)
						{
						//step through x
						for(ix=lft;ix<=rgt;ix++)
							{
							//init iy to dot just above base
							iy=tby-1;

							Cutup::FindPixelTop(ix,iy,top,bimg,dimg);
							}
						}

					else
						{
						bool lside=true,rside=true;
						Cutup::FindOccupancyTop(tx,ty,lft,top,rgt,btm
														,lside,rside
														,*this,bimg,dimg);
						}

					//occupied - do the following:
					}

				//set tile sprite to cropped version of draw image
				short sw,sl;	//sprite width and length;
				sw=rgt-lft+1;	sl=btm-top+1;
				if(sprite(tx,ty,0))	destroy_bitmap(sprite(tx,ty,0));
				sprite(tx,ty,0,create_bitmap(sw,sl));
				blit(dimg,sprite(tx,ty,0),lft,top,0,0,sw,sl);

				Cutup::ClearBaseImgDots(bimg,dimg,lft,top,rgt,btm,usesTriQuads);
				Cutup::DisplayProgress(++tileimgidx,ntileimgs);
				}
			else	//quadrants of tiles
				{
				for(short tq=0;tq<nquads;tq++)
					{
					//initialize draw image to background color
					const u32 bgcolor=0xff00ff;
					clear_to_color(dimg,bgcolor);

					short tbx,tby;	//tile base x and y to draw
					tbx=bx+tx*xpixelsPerTile;
					tby=by+ty*ypixelsPerTile;
					short ix,iy;	//image x and y to draw

					//set up coords and bounds
					iy=tby;	ix=tbx;
					short lft,rgt,top,btm;	//crop coords;

					if(!occupied(tx,ty,tq))
						{
						/*	not occupied - set bounds to base bounds
								so to size an empty image (bgcolor) 	*/
						lft=tbx;	rgt=lft+xpixelsPerTile-1;
						top=tby;	btm=top+ypixelsPerTile-1;
						}
					else
						{
						switch(tq)	{
						/*
							\0/
							1X2
							/3\
						*/
						case 0: //top
							{
							lft=ix;	rgt=ix+xpixelsPerTile-1;
							top=iy;	btm=iy+ypixelsPerTile/2-1;

							//copy base tile
							blit(bimg,dimg,lft,top,lft,top,rgt-lft+1,btm-top+1);
							//clear corners on draw img
							triangle(dimg,lft,top+1,lft,btm,(lft+rgt)/2-1,btm,bgcolor);
							triangle(dimg,rgt,top+1,rgt,btm,(lft+rgt)/2+1,btm,bgcolor);


							if(ty==0)
								{
								for(ix=lft;ix<=rgt;ix++)
									{
									//init iy to dot just above base
									iy=tby-1;

									Cutup::FindPixelTop(ix,iy,top,bimg,dimg);
									}
								}
							else
								{
								bool lside=true,rside=true;
								Cutup::FindOccupancyTop(tx,ty,lft,top,rgt,btm
																,lside,rside
																,*this,bimg,dimg);
								}

							break;

							}
						case 1:	//left
							{
							lft=ix;	top=iy;
							rgt=ix+xpixelsPerTile/2-1;
							btm=iy+ypixelsPerTile-1;
							blit(bimg,dimg,lft,top,lft,top,rgt-lft+1,btm-top+1);
							//clear corners on draw img
							////triangle(dimg,lft+1,top,rgt,top,rgt,(top+btm)/2-1,bgcolor);
							triangle(dimg,lft+1,btm,rgt,btm,rgt,(top+btm)/2+1,bgcolor);


							if(ty==0)
								{
								short slopey=tby-1;

								for(ix=lft;ix<=rgt;ix++)
									{
									//init iy to dot just above base
									iy=slopey++;

									Cutup::FindPixelTop(ix,iy,top,bimg,dimg);
									}
								}
							else
								{
								bool lside=true,rside=false;
								Cutup::FindOccupancyTop(tx,ty,lft,top,rgt,btm
																,lside,rside
																,*this,bimg,dimg);
								}

							break;
							}
						case 2:	//right
							{
							lft=ix+xpixelsPerTile/2;

							top=iy;
							rgt=ix+xpixelsPerTile-1;
							btm=iy+ypixelsPerTile-1;
							blit(bimg,dimg,lft,top,lft,top,rgt-lft+1,btm-top+1);

							//clear corners on draw img
							triangle(dimg,lft,btm,rgt,btm,lft,(top+btm)/2+1,bgcolor);

							if(ty==0)
								{
								short slopey=tby-1;

								for(ix=rgt;ix>=lft;ix--)
									{
									//init iy to dot just above base
									iy=slopey++;
									Cutup::FindPixelTop(ix,iy,top,bimg,dimg);
									}
								}
							else
								{
								bool lside=false,rside=true;
								Cutup::FindOccupancyTop(tx,ty,lft,top,rgt,btm
																,lside,rside
																,*this,bimg,dimg);
								}

							break;
							}
						case 3: //bottom
							{
							lft=ix;
							top=iy;
							rgt=ix+xpixelsPerTile-1;
							btm=iy+ypixelsPerTile-1;
							blit(bimg,dimg,lft,top,lft,top,rgt-lft+1,btm-top+1);

							/* don't clear corners on draw img - this is
								bottom tile quadrant. */
							if(ty==0)
								{
								short slopey=btm-1;
								short slopeyinc=-1;
								for(ix=rgt;ix>=lft;ix--)
									{
									//init iy to dot just above base
									if(ix==(rgt+lft+1)/2)
										{
										slopeyinc=1;
										////slopey+=slopeyinc;
										}
									iy=slopey;
									slopey+=slopeyinc;

									Cutup::FindPixelTop(ix,iy,top,bimg,dimg);
									}
								}
							else
								{
								bool lside=true,rside=true;
								Cutup::FindOccupancyTop(tx,ty,lft,top,rgt,btm
																,lside,rside
																,*this,bimg,dimg);
								}
							}
							break;
						default: break;	}	//switch tq
						}

					#if 1
						Cutup::DisplayRawImages(bimg,dimg);
						////exit(0);
					#endif


					/*	for tile quads: fix bounds to be like
						those of a full tile	for cropping	*/
					lft=tbx;		rgt=tbx+xpixelsPerTile-1;
					top=std::min(tby,top);
					btm=tby+ypixelsPerTile-1;

					//set tile sprite to cropped version of draw image
					short sw,sl;	//sprite width and length;
					sw=rgt-lft+1;	sl=btm-top+1;
					if(sprite(tx,ty,tq))	destroy_bitmap(sprite(tx,ty,tq));
					sprite(tx,ty,tq,create_bitmap(sw,sl));

					//blit cropped part
					blit(dimg,sprite(tx,ty,tq),lft,top,0,0,sw,sl);

					Cutup::ClearBaseImgDots(bimg,dimg,lft,top,rgt,btm,usesTriQuads);
					Cutup::DisplayProgress(++tileimgidx,ntileimgs);
					}//for tq

				}	//end of if/else (quad==1)

			}	//end for tx and ty

		release_bitmap(bimg);
		release_bitmap(dimg);

		}	//end of ZonePattern::CutupImages()

void
ZonePattern::
	DrawEvenlySpacedTiles(BITMAP*page,short xo,short yo
	                           ,u16 pad,bool transparent)
		{
		short		ix=xo,	iy=yo;
		for(short ty=0;ty<tl;ty++)
			{
			//init ix to base xo
			ix=xo;
			//find the max len for the tiles (max wid is fixed to xPixelsPerTile)
			u32 maxw=xpixelsPerTile;
			u32 maxl=ypixelsPerTile;
			for(short tx=0;tx<tw;tx++)
			for(short tq=0;tq<ImgsPerTile();tq++)
				maxl=std::max(maxl,(u32)sprite(tx,ty,tq)->h);
			//draw evenly spaced row
			for(short tx=0;tx<tw;tx++)
			for(short tq=0;tq<ImgsPerTile();tq++)
				{
				BITMAP*spr=sprite(tx,ty,tq);
				if(spr)
					{
					if(transparent)
						draw_sprite(page,spr,ix,iy);
					else
						blit(spr,page,0,0,ix,iy,spr->w,spr->h);
					}
				ix+=maxw+pad;
				}
			//inc iy for next row
			iy+=maxl+pad;
			}
		}

void
ZonePattern::
	DrawWholeZone(BITMAP*page,short xo,short yo)
		{
		short ix,iy;
		for(short ty=0;ty<tl;ty++)
		for(short tx=0;tx<tw;tx++)
			{
			for(short tq=0;tq<(usesTriQuads?4:1);tq++)
				{
				BITMAP*spr=sprite(tx,ty,tq);
				short syo=ypixelsPerTile-spr->h;	//sprite y offset
				ix=xo+tx*xpixelsPerTile;
				iy=yo+syo+ty*ypixelsPerTile;
				draw_sprite(page,spr,ix,iy);
				}
			}
		}

u32
ZonePattern::
	colorscale(u32 color,u16 mulv,u16 divv)
		{
		union Color	{	struct	{u8 b,g,r,a;};	u32	c;	Color(u32 sc)	{c=sc;}	};
		Color cd(color);
		cd.r=std::min(cd.r*mulv/divv,255);
		cd.g=std::min(cd.g*mulv/divv,255);
		cd.b=std::min(cd.b*mulv/divv,255);
		return cd.c;
		}

BITMAP*
ZonePattern::
	GenerateBaseImage(u32 fgc,u32 bgc,u32 txtc)
		{
		//initialize values
		u16 iw=tw*xpixelsPerTile;
		u16 il=tl*ypixelsPerTile+nfloors*zpixelsPerFloor;
		BITMAP*gbi=create_bitmap(iw,il);
		short btop,bbtm,blft,brgt;	//base box bounds
		blft=0;	brgt=iw-1;	bbtm=il-1;
		btop=il-tl*ypixelsPerTile;
		u32 sprTransColor=0xff00ff;
		u16 padsz=4;	//building pad sz

		//clear to sprite transparency
		clear_to_color(gbi,sprTransColor);
		//draw base box
		rectfill(gbi,blft,btop,brgt,bbtm,bgc);
		rect(gbi,blft,btop,brgt,bbtm,fgc);
		//draw vertical wall
		short walltop=bbtm-padsz-nfloors*zpixelsPerFloor;
		rectfill(gbi,blft+padsz,walltop,brgt-padsz,bbtm-padsz,colorscale(bgc,1,2));
		rect(gbi,blft+padsz,walltop,brgt-padsz,bbtm-padsz,colorscale(fgc,1,2));
		//draw roof
		short rooftop=walltop-(bbtm-btop-padsz*2);
		rectfill(gbi,blft+padsz,rooftop,brgt-padsz,walltop-1,colorscale(bgc,3,2));
		rect(gbi,blft+padsz,rooftop,brgt-padsz,walltop-1,colorscale(fgc,3,2));
		//show name
		std::stringstream ss;
		ss.str("");
		ss<<name<<" ("<<tw<<"x"<<tl<<")";
		textout_ex(gbi,font,ss.str().c_str(),blft+2,(btop+bbtm)/2-4,txtc,colorscale(bgc,4,2));
		//return the generated bitmap
		return gbi;
		}


#endif
//!*** ZonePatternList - implementation ***

