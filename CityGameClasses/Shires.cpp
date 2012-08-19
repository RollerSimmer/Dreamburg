#include "Shires.h"
#include "neighbors.h"
#include "map.h"
#include "zonepatternlist.h"
#include "../intvector2d/intvector2d.h"
#include <stack>
#include <queue>
#include <list>


using namespace std;

Shire::Shire()
		:Space(0,0,1024,1024)
	{
	for(int y=0;y<shire_amtWardsAcross;y++)
	for(int x=0;x<shire_amtWardsAcross;x++)
		{
		wardary[y][x]=0;
		}
	}

void Shire::AllocAllWards(Map::Data*map)
	{
	Ward**curward;
	for(int y=0;y<shire_amtWardsAcross;y++)
	for(int x=0;x<shire_amtWardsAcross;x++)
		{
		curward=&wardary[y][x];
		if(*curward==0)
			*curward=new Ward(amtTilesPerWard,amtTilesPerWard,map);
		}
	}

inline Ward*Shire::ward(int xw,int yw)
	{
	if(InSpace(xw,yw))	return wardary[yw][xw];
	else 					return 0;
	}

void Shire::ClearWardVisitedFlags()
	{
	for(unsigned int y=0;y<shire_amtWardsAcross;y++)
	for(unsigned int x=0;x<shire_amtWardsAcross;x++)
		{
		if(ward(x,y))	ward(x,y)->ClearVisited();
		}
	}

void Shire::ClearWardPathLinks()
	{
	for(unsigned int y=0;y<shire_amtWardsAcross;y++)
	for(unsigned int x=0;x<shire_amtWardsAcross;x++)
		{
		if(ward(x,y))
			{
			////ward(x,y)->SetPathNext(0);
			ward(x,y)->SetPathPrev(0);
			ward(x,y)->SetPathEnterEdge(edge::mid);
			}
		}
	}

struct makepath_stackstruct
	{
	short x,y,distto;
	};

bool makepath_shorterdist(makepath_stackstruct p1,makepath_stackstruct p2)
	{
	return p1.distto<p2.distto;
	}

void Shire::MakePath(Ward**&wardpath,int sx,int sy,int dx,int dy,int&wardpath_len,bool&pathfound)
	{
	makepath_stackstruct cur,nbr;
	stack<makepath_stackstruct> xystack;
		int xystacksz;
	list<makepath_stackstruct> pol;	//push order list
	//clear the visited flags and links on each ward before starting:
		ClearWardVisitedFlags();
		ClearWardPathLinks();
	cur.x=sx;	cur.y=sy;
	cur.distto=intvector2d::dist(sx,sy,dx,dy);

	//set pathfound to false to start:
		pathfound=false;

	if(inshire(cur.x,cur.y))
		{
		xystack.push(cur);
		makepath_stackstruct entry;
		while (!xystack.empty())
			{
			xystacksz=xystack.size();
			cur=xystack.top();	xystack.pop();
			if(inshire(cur.x,cur.y)	&&	!(ward(cur.x,cur.y)->GetVisitedState()))
				{
				ward(cur.x,cur.y)->SetVisited();
				if(cur.x==dx&&cur.y==dy)	//we found our destination:
					{
					//set pathfound to true since connected destination was found from source:
						pathfound=true;
					//backtrace to source:
						stack<Ward*> pathstack;
						Ward*curward;
						curward=ward(cur.x,cur.y);
						while(curward!=ward(sx,sy))
							{
							if(curward==0)
								{
								//clear stack and return:
									while(!pathstack.empty())	pathstack.pop();
									wardpath=0;
									goto CleanUpAndReturn;
								}
							else
								{
								pathstack.push(curward);
								curward=curward->GetPathPrev();
								}
							}
						//pop stack and add to list in order:
							wardpath_len=pathstack.size();
							wardpath=new Ward*[wardpath_len];
							for(unsigned int i=0;i<wardpath_len;i++)
								{
								if(!pathstack.empty())
									{
									wardpath[i]=pathstack.top();
									pathstack.pop();
									}
								}
							goto CleanUpAndReturn;
					}
				else
					{
					//push unvisited neighbors in order of distance:
						short nx,ny;
						short exitedge,enteredge;
						for(unsigned int i=0;i<4;i++)
							{
							nx=cur.x+xo4[i];	ny=cur.y+yo4[i];
							Ward*nward,*curward;
							nward=ward(nx,ny);	curward=ward(cur.x,cur.y);
							if(    inshire(nx,ny)&&nward!=0&&curward!=0
							     &&(!nward->GetVisitedState())
							     &&curward->IsPathConnectedTo(edgeary[i])  )
								{
								entry.x=nx;
								entry.y=ny;
								entry.distto=intvector2d::dist(dx,dy,nx,ny);
								pol.push_front(entry);
								nward->SetPathPrev(ward(cur.x,cur.y));
								nward->SetPathEnterEdge(enteredgeary[i]);
								}
							}
						pol.sort(makepath_shorterdist);
						if(pol.empty()&&!xystack.empty())
							xystack.pop();
						else while(!pol.empty())
							{
							entry=pol.front();	pol.pop_front();
							xystack.push(entry);
							}
					}
				}
			}
		}
	CleanUpAndReturn:
		while(!pol.empty())	pol.pop_front();
		while(!xystack.empty())	xystack.pop();
		return;
	}

void Shire::FindSurplus(int tradeitem,bool&found,int&sux,int&suy,int shx,int shy)
	{
	int r,x,y,trgtpathnet;
	Ward*cw;
	bool done,oob;
	found=done=oob=false;
	r=1;
	while(!done)
		{
		trgtpathnet=0;
		oob=!inshire(shx,shy);
		if(!oob)
			{
			cw=ward(shx,shy);
			if(cw)
				trgtpathnet=cw->getpathnet();
			}
		oob=r>=32;
		if(!oob)
			{
			x=shx-r;	y=shy-r;
			while(!found&&x<shx+r)	{	++x;	CheckForSurplusAt(tradeitem,found,sux,suy,x,y,trgtpathnet);	}
			while(!found&&y<shy+r)	{	++y;	CheckForSurplusAt(tradeitem,found,sux,suy,x,y,trgtpathnet);	}
			while(!found&&x>shx-r)	{	--x;	CheckForSurplusAt(tradeitem,found,sux,suy,x,y,trgtpathnet);	}
			while(!found&&y>shy-r)	{	--y;	CheckForSurplusAt(tradeitem,found,sux,suy,x,y,trgtpathnet);	}
			}
		done=found||oob;
		++r;
		}
	}

void Shire::CheckForSurplusAt(int tradeitem,bool&found,int&sux,int&suy,int x,int y,int trgtpathnet)
	{
	Ward*w;
	TradeItem_SpaceBalance*tb;
	//only proceed if on map and not found yet:
		if(inshire(x,y)&&!found)
			{
			w=ward(x,y);
			tb=w->gettradebal(tradeitem);
			if(tb&&*tb>0&&w->getpathnet()!=0&&w->getpathnet()==trgtpathnet)
				//set true return values:
					{	sux=x;	suy=y;	found=true;		}
			}
	}

void Shire::StepWardTrade()
	{
	//airing (broadcasting) made trade values:
		for(int y=0;y<shire_amtWardsAcross;y++)
		for(int x=0;x<shire_amtWardsAcross;x++)
			{
			StepWardTrade_Air(x,y);
			}
	//make and take values:
		for(int y=0;y<shire_amtWardsAcross;y++)
		for(int x=0;x<shire_amtWardsAcross;x++)
			{
			StepWardTrade_MakeAndTake(x,y);
			}
	}

void Shire::StepWardTrade_Air(int x,int y)
	{
	Ward*cward=ward(x,y);
	if(cward!=0)
		{
		int tx,ty,tz,txstart,tystart,txmax,tymax;
		txstart=cward->getx();			tystart=cward->gety();
		txmax=txstart+cward->getw();	tymax=tystart+cward->getl();
		Map::Data*map;
		map=cward->getmap();
		if(map)
			{
			for(int ty=tystart;ty<tymax;ty++)
			for(int tx=txstart;tx<txmax;tx++)
			for(int tz=0;tz<Map::maxstack;tz++)
				{
				StepWardTrade_Air_FromTile(x,y,tx,ty,tz);
				}
			}
		}
	}

void Shire::StepWardTrade_Air_FromTile(int tx,int ty,int tz)
	{
	int wx,wy;
	wx=tx/amtTilesPerWard;	wy=ty/amtTilesPerWard;
	StepWardTrade_Air_FromTile(wx,wy,tx,ty,tz);
	}

void Shire::StepWardTrade_Air_FromTile(int wx,int wy,int tx,int ty,int tz)
	{
	//note: ward(x,y) and ward(x,y)->getmap() are already valid, since caller checks them
	Ward*cward=ward(wx,wy);
	Map::Data*map=cward->getmap();
	MapPatternIndex::Struct*mpi=map->mapat(tx,ty,tz);
	if(mpi)
		{
		ZonePattern*zp=patterns->Zone(mpi->patidx);
		if(zp)
			{
			for(int item=0;item<tradeitem::amount;item++)
				{
				TradeItem_SpaceBalance*tbal;
				tbal=cward->gettradebal(item);
				TradeItem_UnitDef*tradePerUnit=&zp->tradePerUnit[item];
				int airradius=tradePerUnit->airradius;
				int airAmtPerWard=tradePerUnit->air*zp->amtfloors;
				AirFromWard(wx,wy,item,airAmtPerWard,airradius);
				}
			}
		}
	}

void Shire::StepWardTrade_MakeAndTake(int wx,int wy)
	{
	Ward*cward=ward(wx,wy);
	if(cward!=0)
		{
		cward->StepTrade_MakeAndTake();
		}
	}

void Shire::AirFromWard(int cwx,int cwy,int item,int airAmtPerWard,int airradius)
	{
	intvector2d o,oinc[3];
	//go over an octagonal area and cover with ward air routine
		enum {	mid=0,cap		};
		o.SetXY(airradius,0);
		for(int pass=mid;pass<=cap;pass++)
			{
			if(pass==mid)
				{
				//incs will be set to go down and left 45 degrees:
					oinc[0].SetXY(0,1);	oinc[1]=oinc[0];	oinc[2].SetXY(-1,0);
				}
			else if(pass==cap)
				{
				//incs will be set to go down and left 45 degrees:
					oinc[0].SetXY(-1,0);	oinc[1]=oinc[0];	oinc[2].SetXY(0,1);
				}
			else
				continue;

			//air the current strip:
				bool done=false;
				while(!done)
					{
					int wyup,wydn,wxr,wxl,wx;
					wydn=cwy+o.GetY();	wyup=cwy-o.GetY();
					wxr=cwx+o.GetX();	wxl=cwx-o.GetX();
					for(wx=wxl;wx<=wxr;wx++)
						{
						Ward*cward;
						cward=ward(wx,wyup);
						if(cward)
							cward->Air_Receive(item,airAmtPerWard);
						if(wydn!=wyup)
							{
							cward=ward(wx,wyup);
							if(cward)
								cward->Air_Receive(item,airAmtPerWard);
							}
						}

				if(pass==mid)
					done=o.GetX()<=o.GetY();
				else if(pass==cap)
					done=o.GetX()<=0;
				}
			}
	}

void Shire::TradeBetweenWards(int amtseeds)
	{
	int wx,wy,sux,suy,item;
	long giveamt;
	bool found;
	TradeItem_SpaceBalance*bal,*subal;
	for(int s=0;s<amtseeds;s++)
		{
		wx=rand()%shire_amtWardsAcross;
		wy=rand()%shire_amtWardsAcross;
		item=rand()%tradeitem::amount;
		bal=GetBalanceAt(wx,wy,item);
		if(bal)
			{
			cout<<"seed went to ("<<wx<<","<<wy<<","<<item<<")"<<endl;
			////cout<<"Balance for seed #"<<s<<" is "<<*bal<<" ("<<wx<<","<<wy<<","<<item<<")"<<endl;
			#if(useShortageAndSurplus)
				if(bal->shortage>bal->surplus)
			#else
				if(*bal<0)
			#endif
					{
					found=false;
					FindSurplus(item,found,sux,suy,wx,wy);
					if(found/*&&IsConnectedByPath(wx,wy,sux,suy)*/)	//IsConnected() replaced by path net finder
						{
						subal=ward(sux,suy)->gettradebal(item);
						#if(useShortageAndSurplus)
							giveamt=min(bal->shortage,subal->surplus);
							bal->shortage-=giveamt;
							subal->surplus-=giveamt;
						#else
							giveamt=min((long)-(*bal),(long)*subal);
							*bal+=giveamt;
							*subal-=giveamt;
						#endif

						//possibly do something with sister wards later:
						}
					}
			}
		}
	}

TradeItem_SpaceBalance*Shire::GetBalanceAt(int wx,int wy,int item)
	{
	Ward*w=ward(wx,wy);
	if(inshire(wx,wy)&&w)
		return w->gettradebal(item);
	else
		return 0;
	}

bool Shire::IsConnectedByPath(int sx,int sy,int dx,int dy)
	{
	Ward**wardpath=0;
	int wardpath_len;
	bool pathfound;
	MakePath(wardpath,sx,sy,dx,dy,wardpath_len,pathfound);
	if(wardpath)	delete[]wardpath;
	return pathfound;
	}

//testing functions:
#define shires_dotesting true
#if shires_dotesting
	void Shire::RandomizeWardTradeBalances()
		{
		for(int y=0;y<shire_amtWardsAcross;y++)
		for(int x=0;x<shire_amtWardsAcross;x++)
			{
			if(ward(x,y)!=0)
				ward(x,y)->RandomizeTradeBalances();
			}
		}

	void Shire::RandomizeWardConnections(int pctconnected)
		{
		for(int y=0;y<shire_amtWardsAcross;y++)
		for(int x=0;x<shire_amtWardsAcross;x++)
			{
			if(ward(x,y)!=0)
				ward(x,y)->RandomizeConnections(pctconnected);
			}
		}

	int Shire::CountShortages()
		{
		Ward*w;
		int amtshortages=0;
		for(int wy=0;wy<shire_amtWardsAcross;wy++)
		for(int wx=0;wx<shire_amtWardsAcross;wx++)
			{
			w=ward(wx,wy);
			if(w)
				for(int item=0;item<tradeitem::amount;item++)
					{
					TradeItem_SpaceBalance*itembal=w->gettradebal(item);
					#if(useShortageAndSurplus)
						if(itembal->shortage>itembal->surplus)
							++amtshortages;
					#else
						if(*itembal<0)
							++amtshortages;
					#endif
					}
			}
		return amtshortages;
		}

#endif

//coding in progress:
void Shire::UpdateWardPathNets()
	{
	ClearWardPathNets();
	ClearWardVisitedFlags();
	typedef enum {	unconnected=0	}	pathnet_type;
	for(int y=0;y<shire_amtWardsAcross;y++)
	for(int x=0;x<shire_amtWardsAcross;x++)
		{
		UpdateWardPathNetFrom(x,y);
		}
	#if 1
		RenumberWardPathNets();
	#endif
	}

void Shire::ClearWardPathNets()
	{
	Ward*cward;
	amtpathnets=1;
	for(int y=0;y<shire_amtWardsAcross;y++)
	for(int x=0;x<shire_amtWardsAcross;x++)
		{
		cward=ward(x,y);
		if(cward)
			{
			cward->setpathnet(0);
			}
		}
	}

struct netstack_entry
	{
	short wx,wy,fromedge;
	};

void Shire::UpdateWardPathNetFrom(int wx,int wy)
	{
	stack<netstack_entry> netstack;
	netstack_entry cur,nbr;
	Ward*cward;
	cur.wx=wx;	cur.wy=wy;	cur.fromedge=edge::mid;
	bool processtile=false;
	cward=ward(cur.wx,cur.wy);
	processtile=cward;
	if(processtile)	processtile=cward->getpathnet()==0;
	if(processtile)
		netstack.push(cur);
	bool incAmtPathNets=false;
	while(!netstack.empty())
		{
		cur=netstack.top();	netstack.pop();
		cward=ward(cur.wx,cur.wy);
		processtile=false;
		processtile=cward;
		if(processtile)	processtile=cward->getpathnet()==0;
		if(processtile)
			{
			cward->setpathnet(amtpathnets);
			incAmtPathNets=true;
			bool pushedges[4]={false,false,false,false};
			int edge1,edge2;
			edge1=cur.fromedge;
			//consider from edge and find paths from there
				if(cward->IsFullyConnected())
					{
					pushedges={true,true,true,true};
					if(edge1!=edge::mid)
						pushedges[edge1]=false;
					}
				else
					{
					for(edge2=edge::rgt;edge2<=edge::btm;edge2++)
						{
						if(edge1!=edge2&&cward->IsConnected(edge1,edge2))
							{
							pushedges[edge2]=true;
							}
						}
					}
				int curpathnet=cward->getpathnet();
				int nbrpathnet;
				Ward*nward;
				for(int edgei=edge::rgt;edgei<=edge::btm;edgei++)
					{
					if(pushedges[edgei])
						{
						nbr.wx=cur.wx+xo4[edgei];
						nbr.wy=cur.wy+yo4[edgei];
						switch(edgei)
							{
							case edge::rgt:	nbr.fromedge=edge::lft;	break;
							case edge::top:	nbr.fromedge=edge::btm;	break;
							case edge::lft:	nbr.fromedge=edge::rgt;	break;
							case edge::btm:	nbr.fromedge=edge::top;	break;
							default:				nbr.fromedge=edge::mid;	break;
							}
						nward=ward(nbr.wx,nbr.wy);
						if(nward&&inshire(nbr.wx,nbr.wy))
							{
							nbrpathnet=nward->getpathnet();
							if(nbrpathnet!=curpathnet)
								{
								////nward->setpathnet(curpathnet);
								netstack.push(nbr);
								}
							}
						}
					}
			}
		}
	if(incAmtPathNets)
		++amtpathnets;
	}

void Shire::RenumberWardPathNets()
	{
	const int remaparysz=shire_amtWardsAcross*shire_amtWardsAcross+10;
	u16*pathnetremap=new u16[remaparysz];
	//init remap array entries to zeroes:
		for(int si=0;si<remaparysz;si++)
			pathnetremap[si]=0;
	pathnetremap[0]=0;
	u16 i=1;
	Ward*cward;
	//first, go through the wards and map each path net id in order of encounter:
		for(int y=0;y<shire_amtWardsAcross;y++)
		for(int x=0;x<shire_amtWardsAcross;x++)
			{
			cward=ward(x,y);
			if(cward)
				{
				bool netinary=false;
				u16 curpathnet;
				curpathnet=cward->getpathnet();
				//remap the current path network to the next i only if previously blank:
					if(curpathnet!=0&&pathnetremap[curpathnet]==0)
						pathnetremap[curpathnet]=i++;
				}
			}
	//now remap each ward's path network using the array just built:
		for(int y=0;y<shire_amtWardsAcross;y++)
		for(int x=0;x<shire_amtWardsAcross;x++)
			{
			Ward*cward;
			cward=ward(x,y);
			u16 curpathnet;
			if(cward)
				{
				curpathnet=cward->getpathnet();
				if(curpathnet!=0)
					cward->setpathnet(pathnetremap[curpathnet]);
				}
			}
	delete[]pathnetremap;
	}


void Shire::FindShortage(int&tradeitem,int&shx,int&shy,bool&found)
	{
	found=false;
	const int maxseed=shire_amtWardsAcross*32;
	int seed=0;

	while(!found&&seed<maxseed)
		{
		item=rand()%tradeitem::amount;
		shx=rand()%shire_amtWardsAcross;
		shy=rand()%shire_amtWardsAcross;
		if(ward(shx,shy))
			{
			found=found||ward(shx,shy)->HasShortage();
			}
		++seed;
		}
	if(found)	//go through sequentially
		{
		return;
		}
	else
		{
		for(shy=0;shy<shire_amtWardsAcross;shy++)
		for(shx=0;shx<shire_amtWardsAcross;shx++)
			{
			Ward*cw=ward(shx,shy);
			if(cw)
				{
				for(item=0;item<tradeitem::amount;item++)
					{
					if(cw->HasShortage(item))
						{
						found=true;
						return;
						}
					}
				}
			}
		}
	return;
	}

void Shire::ResetTrafficVolumes()
	{
	for(int y=0;y<shire_amtWardsAcross;y++)
	for(int x=0;x<shire_amtWardsAcross;x++)
		{
		if(ward(x,y))
			{
			ward(x,y)->SetPeakTrafficVol(0);
			ward(x,y)->SetCurrentTrafficVol(0);
			}		
		}
	}

void Shire::ResetTrafficVolumes_Current()
	{
	for(int y=0;y<shire_amtWardsAcross;y++)
	for(int x=0;x<shire_amtWardsAcross;x++)
		{
		if(ward(x,y))
			ward(x,y)->SetCurrentTrafficVol(0);
		}
	}

void Shire::ResetTrafficVolumes_Peak()
	{
	for(int y=0;y<shire_amtWardsAcross;y++)
	for(int x=0;x<shire_amtWardsAcross;x++)
		{
		if(ward(x,y))
			ward(x,y)->SetPeakTrafficVol(0);
		}
	}

void Shire::IncCurTrafficVol(int x,int y,int volinc)
	{
	if(ward(x,y))
		ward(x,y)->IncCurTrafficVol(volinc);
	}

void Shire::UpdatePeakTrafficVol(int x,int y)
	{
	if(ward(x,y))
		ward(x,y)->UpdatePeakTrafficVol();
	}

