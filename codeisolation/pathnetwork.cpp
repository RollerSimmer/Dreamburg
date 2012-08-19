/**********************************************************************
**             Allegro game library project template                 **
**********************************************************************/

#include <iostream>
#include <allegro.h>
#include <types/mytypes.h>
#include <cstdlib>
#include <ctime>
#include <list>
#include <sstream>

using namespace std;

struct PathNetByte
	{
	u8	lonib:4,	hinib:4;
	typedef enum {loNibIdx=0,hiNibIdx}	nibIdx;
	PathNetByte()	{	Set(0,0);	}
	PathNetByte(u16 lo,u16 hi)	{	Set(lo,hi);	}
	void Set(u16 lo,u16 hi)
		{	nibble(PathNetByte::loNibIdx,lo);
		nibble(PathNetByte::hiNibIdx,hi);
		}
	u16 nibble(short idx)
		{
		u16 value=0;
		if(idx==PathNetByte::loNibIdx)		value=lonib;
		else if(idx==PathNetByte::hiNibIdx)	value=hinib;
		else throw "Error: bad nibble index in PathNetByte::nibble()";
		return value;
		}
	void nibble(short idx,u16 value)
		{
		if(idx==PathNetByte::loNibIdx)		lonib=value;
		else if(idx==PathNetByte::hiNibIdx)	hinib=value;
		else throw "Error: bad nibble index in PathNetByte::nibble()";
		}
	};

//constants to use in networks

const short areasz=16;
	/* in the future, the game will use square areas of about 16x16
	   tiles for data like education, healthcare, law enforcement and
		safety, demographic groups, traffic and overall population.

		Road networks will be included in this set of data, but not
		put within the packed structures for each area.
	*/

const short mapmaxsz=(1000+(areasz-1))/areasz;
namespace pathnet
	{
	enum	{	empty=0,indy,first	};
	const u16 maxamt=16;
	const u32 minsz=10;
	}
namespace nbr
	{
	const short xs[4]={-1,0,1,0};
	const short ys[4]={0,-1,0,1};
	}

struct PathNetByteArray
	{	PathNetByte ary[mapmaxsz][mapmaxsz/2];
	PathNetByteArray()
		{	for(int y=0;y<mapmaxsz;y++)
			for(int x=0;x<mapmaxsz;x++)
				tile(x,y,0);
		}
	void tile(int x,int y,int val)
		{	ary[y][x/2].nibble(x%2,val);	}
	u16 tile(int x,int y)
		{	return ary[y][x/2].nibble(x%2);	}
	};

struct PathNetwork
	{
	u16 nnetworks;
	u16 mapw,maph;
	PathNetByteArray*pba;
	u32 viewzoom;
	PathNetwork()
		{	pba=new PathNetByteArray;
		mapw=maph=mapmaxsz;
		cout<<"sizeof(PathNetByte)="<<sizeof(PathNetByte)<<endl;
		cout<<"sizeof()="<<sizeof(PathNetByte)<<endl;
		nnetworks=pathnet::first;
		viewzoom=100;
		}
	~PathNetwork()
		{	delete pba;	}
	void FloodFillNetwork(int x,int y,u16 net)
		{
		if(!inrange(x,y))	return;

		std::list<PathNetwork::NetStackEntry> netstack;
		int cx=x,cy=y;
		PathNetwork::NetStackEntry curtile(cx,cy);

		//a network full of empty tiles is still zero size
		u8 bgnet=tile(cx,cy);
		if(bgnet==pathnet::empty)	return;
		if(bgnet==net)	return;

		//start the stack off with the current tile
		netstack.push_back(curtile);

		u32 netsize=0;

		while(!netstack.empty())
			{
			//pop and fill tile
			curtile=netstack.back();
			netstack.pop_back();
			cx=curtile.x;	cy=curtile.y;
			tile(cx,cy,net);
			++netsize;
			//push neighbors
			for(int i=0;i<4;i++)
				{
				short ox,oy;	//offset coords
				ox=cx+nbr::xs[i];	oy=cy+nbr::ys[i];
				if(tile(ox,oy)==bgnet)
					{
					curtile.Set(ox,oy);
					netstack.push_back(curtile);
					}
				}
			}	//while()
		////cout<<"End of flood fill.  netsize="<<netsize<<endl;
		}
	void DeleteNetwork(u16 delnet)
		{
		//if target is undeletable, get out of here
		if(delnet==pathnet::empty||delnet==pathnet::indy||delnet>=nnetworks)
			return;
		//process map
		for(int y=0;y<maph;y++)
		for(int x=0;x<mapw;x++)
			{
			u16 curnet=tile(x,y);
			//elif the tile matches the target, set to no network
			if(curnet==delnet)
				tile(x,y,pathnet::indy);
			//elif the tile > the target, dec by one
			else if(curnet>delnet)
				tile(x,y,curnet-1);
			}
		//decrease the amount of networks
		--nnetworks;
		}
	void SplitNetworks(int x,int y,u16 net)	{	}
	void MergeNetworks(int x,int y,u16 mergenet)	{	}

	struct NetworkBoolMapByte
		{
		u8	b;
		bool Read(short biti)
			{	u8 bit=0;
			if(biti>7||biti<0)	return false;
			bit=(u8)1<<biti;
			return((b&bit)!=0);
			}
		void Set(short biti,bool state)
			{	if(biti>7||biti<0)	return;
				b=b|((u8)(state&1)<<biti);		}
		void Set(bool state)
			{	for(int i=0;i<7;i++)	Set(i,state);	}
		NetworkBoolMapByte(bool state)	{	Set(state);			}
		NetworkBoolMapByte()	{	Set(0);			}
		};

	struct NetworkBoolMap
		{
		u16 w,h;
		NetworkBoolMapByte data[mapmaxsz][(mapmaxsz+7)/8];
		NetworkBoolMap()	{	Set(0);	}
		NetworkBoolMap(bool state)	{	Set(state);	}
		void Set(bool state)
			{
			w=h=mapmaxsz;
			for(int y=0;y<h;y++)
			for(int x=0;x<w;x+=8)
				data[y][x/8].Set(state);
			}
		void state(short x,short y,bool state)
			{	if(inrange(x,y))
					data[y][x/8].Set(x%8,state);	}
		bool state(short x,short y)
			{	if(!inrange(x,y))	return false;
				return data[y][x/8].Read(x%8);		}
		bool inrange(short x,short y)
			{	return x>=0&&y>=0&&x<w&&y<h;	}
		};

	struct NetStackEntry
		{
		short x,y;
		void Set(short sx,short sy)
			{	x=sx;	y=sy;		}
		NetStackEntry(short x,short y)
			{	Set(x,y);	}
		NetStackEntry()
			{	Set(-1,-1);	}
		};
	u32 GetNetSize(int x,int y,u32 maxsz)
		{
		if(!inrange(x,y))	return 0;

		std::list<PathNetwork::NetStackEntry> netstack;
		int cx=x,cy=y;
		PathNetwork::NetStackEntry curtile(cx,cy);
		NetworkBoolMap*boolmap=new NetworkBoolMap(false);

		//a network full of empty tiles is still zero size
		u8 net=tile(cx,cy);
		if(net==pathnet::empty)	return 0;

		//start the stack off with the current tile
		netstack.push_back(curtile);

		u32 netsize=0;

		while(!netstack.empty()&&netsize<maxsz)
			{
			//pop
			curtile=netstack.back();
			netstack.pop_back();

			cx=curtile.x;	cy=curtile.y;
			++netsize;
			if(!boolmap->state(cx,cy))
				{
				boolmap->state(cx,cy,true);
				for(int i=0;i<4;i++)
					{
					short ox,oy;	//offset coords
					ox=cx+nbr::xs[i];	oy=cy+nbr::ys[i];
					if(  inrange(ox,oy)
						&&!boolmap->state(ox,oy)
					   &&tile(ox,oy)==net    )
					   {
					   curtile.Set(ox,oy);
					   netstack.push_back(curtile);
					   }
					}
				}
			if(netsize>=maxsz)
				////cout<<"GetNetSize(): max size reached @ "<<netsize<<endl
				;
			}	//while()

		delete boolmap;
		return netsize;
		}	//GetNetSize()

	void GetNeighborNetSzs(int x,int y,u32 nbrnetszs[4]
								  ,u16 nbrnets[4])
		{
		for(int i=0;i<4;i++)	{
			short ox=x+nbr::xs[i],	oy=y+nbr::ys[i];
			nbrnetszs[i]=GetNetSize(ox,oy,1);
			nbrnets[i]=tile(ox,oy);	}
		}
	void GetLargestNgbrNet(int x,int y,int&nbrx,int&nbry
	                       ,u32&netsz,u16&net)
		{
		u32 ngbrnetszs[4];u16 ngbrnets[4];
		GetNeighborNetSzs(x,y,ngbrnetszs,ngbrnets);
		u32 maxsz=0;	int maxi=0;	u16 maxnet=pathnet::empty;
		for(int i=0;i<4;i++)
			if(i==0||ngbrnetszs[i]>maxsz)	{
				maxi=i;
				maxsz=ngbrnetszs[i];
				maxnet=ngbrnets[i];	}
		net=maxnet;	netsz=maxsz;
		nbrx=x+nbr::xs[maxi];
		nbry=y+nbr::ys[maxi];
		}
	void FixAllNetworks()
		{
		//Flood different neighbors
		for(int y=0;y<maph;y++)
		for(int x=0;x<mapw;x++)
			{
			for(int i=0;i<4;i++)
				{	short ox=nbr::xs[i]+x,oy=nbr::ys[i]+y;
				u8 ct=tile(x,y),ot=tile(ox,oy);
				if(ct==pathnet::empty||ot==pathnet::empty)
					;	//do nothing
				else if(ct!=ot)
					{	//process the tile if neighbors are different
					//fill the higher network index with the lower
					if(ct==pathnet::indy)		FloodFillNetwork(x,y,ot);
					else if(ot==pathnet::indy)	FloodFillNetwork(x,y,ct);
					else if(ct>ot)					FloodFillNetwork(x,y,ot);
					else //if(ct<ot)
						FloodFillNetwork(ox,oy,ct);
					////cout<<"Found different neighbor - "<<(short)ct<<","<<(short)ot<<endl;
					}
				}
			}

		//adjust network indexes
		u32 netcts[16];
		for(int i=0;i<pathnet::maxamt;i++)			netcts[i]=0;
		//count tiles in each network
		for(int y=0;y<maph;y++)
		for(int x=0;x<mapw;x++)
			++netcts[tile(x,y)];

		for(int i=pathnet::maxamt-1;i>=pathnet::first;i--)
			if(netcts[i]==0)	DeleteNetwork(i);

		//Fill large indy areas with new networks
		for(int y=0;y<maph;y++)
		for(int x=0;x<mapw;x++)
			{
			u8 ct=tile(x,y);
			if(ct==pathnet::indy)
				//this is indy, fill it if net sz is big
				AddNewNetIfOverThreshold(x,y);
			}
		}
	void AddNewNetIfOverThreshold(int x,int y)
		{
		// leave if there are too many networks already
		if(nnetworks>=pathnet::maxamt)	return;
		u16 netsz=GetNetSize(x,y,pathnet::minsz+1);
		// leave if network is too small
		if(netsz<pathnet::minsz)	return;
		////cout<<"GetNetSize("<<x<<","<<y<<") returned "<<netsz<<endl;

		// filtering is finished; proceed with add
		FloodFillNetwork(x,y,nnetworks++);

		////cout<<"Now there are "<<nnetworks<<"networks"<<endl;
		}
	void AddTile(int x,int y)
		{
			//check for empty tiles
			if(tile(x,y)==pathnet::empty)
				#if 0
					{
					int nbrx,nbry;	u32 netsz;	u16 net;
					////GetLargestNgbrNet(x,y,nbrx,nbry,netsz,net);

					//if no networks border this, add to indy
					if(netsz==0)

					//otherwise add it to largest network
					else
						{
						tile(x,y,net);
						for(int i=0;i<4;i++)
							{
							short ox=x+nbr::xs[i],oy=y+nbr::ys[i];
							if(tile(ox,oy)!=net&&tile(ox,oy)!=pathnet::empty)
								FloodFillNetwork(ox,oy,net);
							}
						}

					//afterwards, add new if sz > threshold
					////AddNewNetIfOverThreshold(x,y);
					}
				#else
					tile(x,y,pathnet::indy);
				#endif
		}
	bool inrange(int x,int y)	{	return x>=0&&y>=0&&x<mapw&&y<maph;	}
	u16 tile(int x,int y)
		{	if(inrange(x,y))	return pba->tile(x,y);
			else 					return pathnet::empty;	}
	u16 tile(int x,int y,u16 netval)
		{	if(inrange(x,y))	pba->tile(x,y,netval);	}
	//temp function for drawing the map
	static u32 colorcode(int index)
		{
		const u32
			colorcodes[16]=
				{
				0x000000,	0xffffff
				,	0xff0000,	0x00ff00,	0x0000ff
				,	0x00ffff,	0xffff00,	0xff00ff
				,	0x007fff,	0x7fff00,	0x7f00ff
				,	0x00ff7f,	0xff7f00,	0xff007f
				,	0x007f7f,	0x7f7f00
				};
		return colorcodes[index];
		};
	static const u32 zoommax=5000;
	void inczoom()
		{ 	if(viewzoom<zoommax)
				viewzoom*=2;
		}
	void deczoom()
		{ 	if(viewzoom>=200)		viewzoom/=2;	else viewzoom=100;	}
	void DrawMap(BITMAP*page)
		{
		//setup coordstep
		u32 coordstep;

		for(u32 y=0;y<mapmaxsz;y++)
		for(u32 x=0;x<mapmaxsz;x++)
			{	//draw the tile
			short dcstep=viewzoom/100;
			short dx=x*viewzoom/100,	dy=y*viewzoom/100;
			if(viewzoom<100)
				putpixel( page,dx,dy
				         ,colorcode(tile(x,y))	);
			else if(viewzoom>=100)
				rectfill( page,dx,dy,dx+dcstep-1,dy+dcstep-1
				         ,colorcode(tile(x,y))	);
			}
		}
	};

void InitGraphics()	{	}

int main()
	{
	//initialize Allegro
	if(true)
		{
		cout<<"Initializing Allegro..."<<endl;
		allegro_init();
		cout<<"...Done"<<endl;
		install_keyboard();
		install_timer();
		set_color_depth(24);
		set_gfx_mode(GFX_AUTODETECT_WINDOWED,640,480,0,0);
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

	//
	PathNetwork pn;

	short x,y,xsv,ysv;
	x=rand()%mapmaxsz;	y=rand()%mapmaxsz;	xsv=x;	ysv=y;

	stringstream ss("");

	u32 ticks;
	ticks=0;

	//loop for input
	while(!key[KEY_ESC])
		{
		//rest(10);
		if(keypressed())
			{
			if(key[KEY_MINUS_PAD]||key[KEY_MINUS])
				pn.deczoom();
			else if(key[KEY_PLUS_PAD]||key[KEY_EQUALS])
				pn.inczoom();
			while(  key[KEY_MINUS_PAD]||key[KEY_MINUS]
			      ||key[KEY_PLUS_PAD]||key[KEY_EQUALS])
				;
			}

		//clear the screen w/ white
		clear_to_color(page,0xffffffff);

		acquire_bitmap(page);

		pn.DrawMap(page);

		ss.str("");
		ss<<"viewzoom="<<pn.viewzoom<<endl;
		textout_ex(page,font,ss.str().c_str(),0,470,0x000000,0xffffff);

		release_bitmap(page);

		//cout<<"adding ("<<x<<","<<y<<")"<<endl;
		pn.AddTile(x,y);
		short jump;
		jump=(rand()%100>=75)	?	10	:	1	;

		xsv=x;	ysv=y;
		x=(rand()%(jump+1)+x)%mapmaxsz;	if(x<0)	x+=mapmaxsz;
		if(x==xsv)	{	y=(rand()%(jump*2+1)-jump+y)%mapmaxsz;	if(y<0)	y+=mapmaxsz;	}
		else			y=ysv;

		//page flip
		acquire_screen();
		blit(page,screen,0,0,0,0,SCREEN_W,SCREEN_H);
		release_screen();

		//fix networks every 100 ticks
		if(ticks==50)	{
			////cout<<"Fixing Network..."<<endl;
			pn.FixAllNetworks();
			ticks=0;	}
		else
			{
			++ticks;
			////cout<<"Not fixing network..."<<endl;
			}

		}
	}
