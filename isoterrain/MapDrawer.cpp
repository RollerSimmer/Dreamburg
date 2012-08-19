#include "MapDrawer.h"
#include "TerrainColors.h"
#include <iostream>
#include <types/mytypes.h>

using namespace std;

const int terraincolor=0x00ff00;

MapDrawer::MapDrawer(IsoTerrain *_map)
	{
	map=_map;
	page=create_bitmap(CalcPageWidth(),CalcPageHeight());
	tiledim=8;
	}

void MapDrawer::DrawMap(int ox,int oy)
	{
	SetOrigin(ox,oy);
	ClearPage();
	int ntilesAcross=800/tilewidth()+2;
	int ntilesAcrossHalf=ntilesAcross/2;
	int nstripsDown=600/tileheightHalf()+100;
	int xtopleft=ox-ntilesAcrossHalf;	// going W from origin
	int ytopleft=oy-1+ntilesAcrossHalf;	// going S from origin
	int u,v;

	//Draw zone:
		int x=xtopleft	,	y=ytopleft;
		int xleft=x		,	yleft=y;
		for(v=0;v<nstripsDown;v++)
			{
			for(u=0;u<ntilesAcross;u++)
				{
				DrawTile(x++,y--);
				}
			//zig-zag step for each strip
			if(v&1)	//odd
				{	x=xleft;	y=yleft+1;	}
			else	//even
				{	x=xleft+1;	y=yleft;	}
			xleft=x;	yleft=y;
			}
	//Draw mini map
		int skipstep=1;
		int ptsz=2;
		DrawMiniMap(800-mapwidth/skipstep*ptsz,0,skipstep,ptsz);
		DrawSkylines(0,500,100,250);
	}

void MapDrawer::DrawTile(int x,int y)
	{
	if(x+1>mapwidth-1||y+1>maplength-1||x<0||y<0)	return;	//range check

	acquire_bitmap(page);
		{
		//define polygon points
			int x1=projx(x,y);		int y1=projy(x,y);
			int x2=projx(x+1,y);		int y2=projy(x+1,y);
			int x3=projx(x+1,y+1);	int y3=projy(x+1,y+1);
			int x4=projx(x,y+1);		int y4=projy(x,y+1);
			int points[8]={x1,y1,x2,y2,x3,y3,x4,y4};

		//define draw colors
			int color_fill;
			int color_edge=0xffff00;

		//calc color based on slope from corner to corner
			color_fill=CalcSurfaceShade(x,y,255,0,tercolors);

		//draw filled box polygon:
			polygon(page,4,points,color_fill);
			////polygon(screen,4,points,color_fill);

		//draw edges:
			line(page,x1,y1,x2,y2,color_edge);
			line(page,x2,y2,x3,y3,color_edge);
			line(page,x3,y3,x4,y4,color_edge);
			line(page,x4,y4,x1,y1,color_edge);

		}
	release_bitmap(page);
	}

void MapDrawer::ClearPage()	{	clear_to_color(page,0x000000);	}

int heightgradient(int ch,int basecolors[])
	{
	union	{	struct{u8 b,g,r,a;	};	int color;	}	top,btm,ret;
	int btmwgt,topwgt,wgtsum;

	if(ch>=htsnow)
		{
		btmwgt=1;	topwgt=1;
		top.color=btm.color=basecolors[isnow];
		}
	else if(ch>=htrock)
		{
		btmwgt=htsnow-ch;	topwgt=ch-htrock;
		top.color=basecolors[isnow];	btm.color=basecolors[irock];
		}
	else if(ch>=htstep)
		{
		btmwgt=htrock-ch;	topwgt=ch-htstep;
		top.color=basecolors[irock];	btm.color=basecolors[istep];
		}
	else if(ch>=htgrass)
		{
		btmwgt=htstep-ch;	topwgt=ch-htgrass;
		top.color=basecolors[istep];	btm.color=basecolors[igrass];
		}
	else if(ch>=htsand)
		{
		btmwgt=htgrass-ch;	topwgt=ch-htsand;
		top.color=basecolors[igrass];	btm.color=basecolors[isand];
		}
	wgtsum=btmwgt+topwgt;
	ret.r=(top.r*topwgt+btm.r*btmwgt)/wgtsum;
	ret.g=(top.g*topwgt+btm.g*btmwgt)/wgtsum;
	ret.b=(top.b*topwgt+btm.b*btmwgt)/wgtsum;
	return ret.color;
	}

int MapDrawer::CalcSurfaceShade(int x,int y,int gray_lighted,int gray_shadowed,int basecolors[])
	{
	//calc color based on slope from corner to corner
		union	{	struct{u8 b,g,r,a;	};	int color;	}	base;
		int ch=map->ht(x,y);
		base.color=heightgradient(ch,basecolors);
		int gray_fill;
		int gray_span=gray_lighted-gray_shadowed;
		int gray_halfspan=gray_span/2;
		int gray_middle=(gray_lighted+gray_shadowed)/2;
		int slopeval_NESW=map->ht(x+1,y)-map->ht(x,y+1);	// NE corner minus SW corner
		int maxslopeval=75;
		slopeval_NESW=min(slopeval_NESW,maxslopeval);
		slopeval_NESW=max(slopeval_NESW,-maxslopeval);
		int slopescale=slopeval_NESW*gray_span/maxslopeval/2;
		gray_fill=gray_middle+slopescale;
		//get components
			int r,g,b;
			r=gray_fill*base.r/255;
			g=gray_fill*base.g/255;
			b=gray_fill*base.b/255;
		//add some random variation to color components
			#if 0
				r+=rand()%20-10;	r=max(min(r,255),0);
				g+=rand()%20-10;	g=max(min(g,255),0);
				b+=rand()%20-10;	b=max(min(b,255),0);
			#endif
		int color_fill=makecol(r,g,b);
		return color_fill;
	}

void HSV2RGB(int&r,int&g,int&b,int h,int s,int v)
	{
	int f;
	long hi,lo,mu,md;
	//H is 0..360, S is 0..255,and V is 0..255
	h=h%360;	if(h<0)	h+=360;
	s=min(s,255);
	v=min(v,255);

	if(s==0)	{	r=g=b=v;	return;	}

	//f:
		f=((h%60)*255)/60;
	//hue zone:
		int hz=h/6;
	//pqt:
		hi=v;	//high color value
		lo=(v*(256-s))/256;		//low color value
		mu=(v*(256-(s*f)/256))/256;		//	middle color value (going up)
		md=(v*(256-(s*(256-f))/256))/256;	// middle color value (going down)

	switch(hz)
		{
		case 0:	r=hi;	g=mu;	b=lo;	break;
		case 1:	g=hi;	r=md;	b=lo;	break;
		case 2:	g=hi;	b=mu;	r=lo;	break;
		case 3:	b=hi;	g=md;	r=lo;	break;
		case 4:	b=hi;	r=mu;	g=lo;	break;
		default:	r=hi;	b=md;	g=lo;	break;
		}
	}

int MapDrawer::CalcHeightShade(int x,int y)
	{
	const int maxz=1000;
	int huered=0;
	int hueblue=240;
	int h=min(maxz,(int)map->ht(x,y));
	int hue=h*(hueblue-huered)/maxz+huered;
	int r,g,b;
	HSV2RGB(r,g,b,hue,255,255);
	int color=makecol(r,g,b);
	return color;
	}

int MapDrawer::CalcPageWidth()
	{
	/*
	int wunit=xisoUnit.GetX()*2;
	int w=mapwidth*wunit;
	return w;
	*/
	return 800;
	}

int MapDrawer::CalcPageHeight()
	{
	/*
	int hunit=xisoUnit.GetY()*2;
	int h=maplength*hunit;
	h+=maxz*zunit;
	return h;
	*/
	return 600;
	}

intvector2d MapDrawer::proj(int x,int y)
	{
	int xo=x-ox;	//xo = x offset from origin
	int yo=y-oy;	//yo = y offset from origin
	int px=CalcPageWidth()/2;
	int py=0;
	intvector2d projv(px,py);
	int z=map->ht(x,y);

	projv=projv+xisoUnit()*xo;
	projv=projv+yisoUnit()*yo;
	projv=projv+zisoUnit()*z;

	return projv;
	}

int MapDrawer::projx(int x,int y)
	{
	intvector2d projv=proj(x,y);
	return projv.GetX();
	}

int MapDrawer::projy(int x,int y)
	{
	intvector2d projv=proj(x,y);
	return projv.GetY();
	}

void MapDrawer::ZoomIn()
	{
	const int tiledim_max=64;
	if(tiledim<=tiledim_max)	tiledim=min(tiledim*2,tiledim_max);
	}

void MapDrawer::ZoomOut()
	{
	const int tiledim_min=1;
	if(tiledim>=tiledim_min)	tiledim=max(tiledim/2,tiledim_min);
	}

void MapDrawer::DrawMiniMap(int sx,int sy,int ptstep,int dotsz)
	{
	int ix,iy;
	int csx,csy;
	int iw=mapwidth/ptstep;
	int il=maplength/ptstep;

	acquire_bitmap(page);
		for(iy=0;iy<il;iy++)
		for(ix=0;ix<iw;ix++)
			{
			int x,y;
			x=ix*ptstep;
			y=iy*ptstep;

			if(x+1>mapwidth-1||y+1>maplength-1||x<0||y<0)	continue;	//range check

			int color;

			csx=sx+ix*dotsz;

			//draw relief map square:
				csy=sy+iy*dotsz;
				color=CalcSurfaceShade(x,y,256,0,tercolors);
				rectfill(page,csx,csy,csx+dotsz-1,csy+dotsz-1,color);

			//draw height map square
				csy=maplength*dotsz+sy+iy*dotsz;
				color=CalcHeightShade(x,y);
				rectfill(page,csx,csy,csx+dotsz-1,csy+dotsz-1,color);
			}
	release_bitmap(page);
	}

void MapDrawer::DrawSkylines(int sx,int sy,int htmul,int htdiv)
	{
	//set max height to height multiplier
		int htmax=htmul;
	//color for sky background:
		int skycolor=0x7f7fff;
	//color for surface
		int color_fill;

	int wid=mapwidth-1,	len=maplength-1;
	int top,btm,lft,rgt;
	int htary[4];	//quad heights
	int points[8];	//clockwise points

	acquire_bitmap(page);
		//view from the south:
			rectfill(page,sx,sy,sx+mapwidth*2-1,sy+htmul,skycolor);
			for(lft=0;lft<wid;lft++)
				{
				rgt=lft+1;
				for(top=0;top<len;top++)
					{
					btm=top+1;
					htary[0]=sy+htmul-min(map->ht(lft,top)*htmul/htdiv,htmul);
					htary[1]=sy+htmul-min(map->ht(rgt,top)*htmul/htdiv,htmul);
					htary[2]=sy+htmul-min(map->ht(rgt,btm)*htmul/htdiv,htmul);
					htary[3]=sy+htmul-min(map->ht(lft,btm)*htmul/htdiv,htmul);
					int slft=sx+lft*2,srgt=sx+rgt*2;
					//draw a filled poly for quad
						points={slft,htary[0],srgt,htary[1],srgt,htary[2],srgt,htary[3]};
						color_fill=CalcSurfaceShade(lft,top,255,0,tercolors);
						//draw a polygon:
							////polygon(page,4,points,color_fill);
						//(alternative) draw a simple box for height instead of polygon:
							int boxht=max(htary[0],htary[1]);
							rectfill(page,slft,boxht,srgt,sy+htmul,color_fill);
					}
				}
		//view from the east:
			int sx2=sx+mapwidth*2+10;
			rectfill(page,sx2,sy,sx2+mapwidth*2-1,sy+htmul,skycolor);
			for(btm=len;btm>0;btm--)
				{
				top=btm-1;
				for(lft=0;lft<wid;lft++)
					{
					rgt=lft+1;
					htary[0]=sy+htmul-min(map->ht(lft,btm)*htmul/htdiv,htmul);
					htary[1]=sy+htmul-min(map->ht(lft,top)*htmul/htdiv,htmul);
					htary[2]=sy+htmul-min(map->ht(rgt,top)*htmul/htdiv,htmul);
					htary[3]=sy+htmul-min(map->ht(rgt,btm)*htmul/htdiv,htmul);
					int slft=sx2+btm*2
						,srgt=sx2+top*2;
					//draw a filled poly for quad
						points={slft,htary[0],srgt,htary[1],srgt,htary[2],srgt,htary[3]};
						color_fill=CalcSurfaceShade(lft,top,255,0,tercolors);

						//draw a polygon:
							////polygon(page,4,points,color_fill);
						//(alternative) draw a simple box for height instead of polygon:
							int boxht=max(htary[0],htary[1]);
							rectfill(page,slft,boxht,srgt,sy+htmul,color_fill);
					}
				}
	release_bitmap(page);
	}
