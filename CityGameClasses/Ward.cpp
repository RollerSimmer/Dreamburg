#include "Ward.h"
#include "neighbors.h"
#include "../intvector2d/intvector2d.h"
#include <stack>
#include <algorithm>

using namespace std;

Ward::Ward(int xward,int yward,Map::Data*_map)
		:	Space(xward,yward,32,32)
	{
	pathnet=idUnusedPathNetwork;
	map=_map;
	}

Ward*Ward::getsister(int item,int isister)
	{
	if(item>=0&&item<tradeitem::amount&&isister>=0&&isister<amtSistersPerItem)
		{
		return sisterwards[item][isister];
		}
	else
		return 0;
	}

void Ward::SetTileVisitedState(int tx,int ty,bool state)
	{
	int wtx,wty;
	u32 bit;
	if(IsTileInWard(tx,ty))
		{
		wtx=tx-getx();	wty=ty-gety();
		bit=state?1:0;
		#if amtTilesPerWard==32
			tilevisits.row[wty]|=(bit<<wtx);
		#endif
		}
	}

bool Ward::WasTileVisited(int tx,int ty)
	{
	int wtx,wty;
	u32 bit,bitmask;
	if(IsTileInWard(tx,ty))
		{
		wtx=tx-getx();	wty=ty-gety();
		#if amtTilesPerWard==32
			bitmask=1<<wtx;
			bit=tilevisits.row[wty]&bitmask;
		#endif
		return bit!=0;
		}
	else
		return false;
	}

bool Ward::IsEdgeTile(int tx,int ty,int edgev)
	{
	if(!IsTileInWard(tx,ty))	return false;
	else if(edgev==edge::top)
		return ty==gety();
	else if(edgev==edge::lft)
		return tx==getx();
	else if(edgev==edge::btm)
		return ty==gety()+amtTilesPerWard-1;
	else if(edgev==edge::rgt)
		return ty==gety()+amtTilesPerWard-1;
	else if(edgev==edge::mid)
		return (  !IsEdgeTile(tx,ty,edge::top)
		        &&!IsEdgeTile(tx,ty,edge::btm)
		        &&!IsEdgeTile(tx,ty,edge::lft)
		        &&!IsEdgeTile(tx,ty,edge::rgt)  );
	else
		return false;
	}

bool Ward::IsTileInWard(int tx,int ty)
	{
	int lx,rx,uy,by;
	lx=getx();	rx=lx+amtTilesPerWard-1;
	uy=gety();	by=uy+amtTilesPerWard-1;
	return tx>=lx&&ty>=uy&&tx<=rx&&ty<=by;
	}

bool Ward::IsConnected(int routev)
	{
	switch(routev)
		{
		case route::EW:		case route::NS:		case route::NE:
		case route::NW:		case route::SW:		case route::SE:
			return isconnectedAry[routev];
		default:	return false;
		}
	}

bool Ward::IsConnected(int edge1,int edge2)
	{
	//check for valid edges first:
		if(edge1<0||edge1>=edge::amt)	return false;
		if(edge2<0||edge2>=edge::amt)	return false;
	//you will need to convert to a route ID and return the entry in the isconnectedAry array:
		if(edge1==edge2)	return true;
		else if(edge1==edge::n||edge2==edge::n)
			{
			if(edge1==edge::s||edge2==edge::s)
				return isconnectedAry[route::NS];
			else if(edge1==edge::e||edge2==edge::e)
				return isconnectedAry[route::NE];
			else if(edge1==edge::w||edge2==edge::w)
				return isconnectedAry[route::NW];
			}
		else if(edge1==edge::s||edge2==edge::s)
			{
			if(edge1==edge::e||edge2==edge::e)
				return isconnectedAry[route::SE];
			else if(edge1==edge::w||edge2==edge::w)
				return isconnectedAry[route::SW];
			}
		else if(edge1==edge::e||edge2==edge::e)
			{
			if(edge1==edge::w||edge2==edge::w)
				return isconnectedAry[route::SE];
			}
		else
			return false;
	}

bool Ward::IsFullyConnected()
	{
	return IsConnected(route::NS&&route::EW);	//an intersection must occur and all possible routes are present
	}

bool Ward::IsPathConnectedTo(int edge1)
	{
	if(pathEnterEdge>=edge::lft&&pathEnterEdge<=edge::mid)
		return(IsConnected(edge1,pathEnterEdge));
	else
		return false;
	}

bool Ward::DoesEdgeHavePath(int edgev)
	{
	intvector2d pinc,p;
	switch(edgev)
		{
		case edge::mid:
			//check internal space
		case edge::top: p.SetXY(getx(),gety());	goto horz;
		case edge::btm: p.SetXY(getx(),gety()+amtTilesPerWard-1);
			horz:	pinc.SetXY(1,0);	break;
		case edge::lft: p.SetXY(getx(),gety());	goto vert;
		case edge::rgt: p.SetXY(getx()+amtTilesPerWard-1,gety());
			vert:	pinc.SetXY(0,1);	break;
		default: return false;
		}
	for(int i=0;i<amtTilesPerWard;i++)
		{
		if(DoesTileHavePath(p.GetX(),p.GetY()))
			return true;
		p+=pinc;
		}
	}

bool Ward::DoesTileHavePath(int tx,int ty)
	{
	if(IsTileInWard(tx,ty))	//note: since this is called within, remove calls to IsTileInWard paired with DoesTileHavePath calls
		{
		#if(map_paths_implemented_yet)
			bool haspath=false;
			MapPatternIndex::Struct*mappat;
			int zpati;
			ZonePattern*zonepat;

			for(int si=0;si<Map::maxstack;si++)
				{
				mappat=map->mapat(tx,ty,si);
				zpati=mappat->patidx;
				zonepat=map->Zone(zpati);
				haspath=haspath||zonepat->IsPath();
				}
		#else
			bool haspath=true;
		#endif
		return haspath;
		}
	else
		return false;
	}

void Ward::FindConnections()
	{
	for(int routev=0;routev<route::amt;routev++)
		isconnectedAry[routev]=FindConnection(routev);
	}

bool Ward::FindConnection(int routev)
	{
	switch(routev)
		{
		case route::EW:	return FindConnection(edge::e,edge::w);
		case route::NS:	return FindConnection(edge::n,edge::s);
		case route::NE:	return FindConnection(edge::n,edge::e);
		case route::NW:	return FindConnection(edge::n,edge::w);
		case route::SW:	return FindConnection(edge::s,edge::w);
		case route::SE:	return FindConnection(edge::s,edge::e);
		case route::EC:	return DoesEdgeHavePath(edge::e);
		case route::NC:	return DoesEdgeHavePath(edge::n);
		case route::WC:	return DoesEdgeHavePath(edge::w);
		case route::SC:	return DoesEdgeHavePath(edge::s);
		default:	return false;
		}
	}

bool Ward::FindConnection(int edge1,int edge2)
	{
	if(edge1==edge2)	return true;
	else if(edge1==edge::mid)
		return DoesEdgeHavePath(edge2);
	else if(edge2==edge::mid)
		return DoesEdgeHavePath(edge1);
	else
		{
		//trace path from edge to edge here:

			//clear ward visited flags
				ClearTileVisits();
			intvector2d p,pinc;
			//create a stack for searching for path:
				stack<intvector2d> pstack;
			p.SetXY(getx(),gety());
			if(edge1==edge::lft||edge1==edge::rgt)		pinc.SetXY(0,1);
			else if(edge1==edge::top||edge1==edge::btm)		pinc.SetXY(1,0);
			if(edge1==edge::rgt)	pinc.SetX(amtTilesPerWard-1);
			if(edge1==edge::btm)	pinc.SetY(amtTilesPerWard-1);
			//push any paths on initial edge onto stack:
				for(int i=0;i<amtTilesPerWard;i++)
					{
					int tx=p.GetX();	int	ty=p.GetY();
					if(  DoesTileHavePath(tx,ty)&&IsTileInWard(tx,ty)
						&&WasTileVisited(tx,ty)  )
						{
						//push the tile onto stack and set visited since it is valid:
							SetTileVisitedState(tx,ty,true);
							pstack.push(p);
						}
					p+=pinc;
					}
			//while stack is not empty, repeat the process of adding valid neighbors until a point on dest edge is found:
				while(!pstack.empty())
					{
					p=pstack.top();	pstack.pop();
					//if point is on dest edge, return true:
						if(IsEdgeTile(p.GetX(),p.GetY(),edge2))
							return true;
					//otherwise push valid neighbors:
						else
							{
							int tx,ty,nx,ny;
							intvector2d np;
							tx=p.GetX();	ty=p.GetY();
							for(int i=0;i<4;i++)
								{
								nx=tx+xo4[i];	ny=ty+yo4[i];
								np.SetXY(nx,ny);
								if(    IsTileInWard(nx,ny)&&DoesTileHavePath(nx,ny)
									  &&!WasTileVisited(nx,ny))
									{
									SetTileVisitedState(nx,ny,true);
									pstack.push(np);
									}
								}
							}
					}
			// making it this far means no valid connections were found, so return false:
				return false;
			}
	}

void Ward::ClearTileVisits()
	{
	////bool state=false;
	int tx,ty,ex,ey;
	ex=getx()+amtTilesPerWard;	ey=gety()+amtTilesPerWard;
	for(ty=gety();ty<ey;ty++)
	for(tx=getx();tx<ex;tx++)
		{
		SetTileVisitedState(tx,ty,false);
		}
	}

void Ward::StepTrade_MakeAndTake()
	{
	//step through each tile and update ward's balances with value
	int tx,ty,tz,txend,tyend;
	txend=getx()+getw();	tyend=gety()+getl();
	for(ty=gety();ty<tyend;ty++)
	for(tx=getx();tx<txend;tx++)
	for(tz=0;tz<Map::maxstack;tz++)
		{
		if(map)
			{
			MapPatternIndex::Struct*mpi=map->mapat(tx,ty,tz);
			if(mpi&&patterns)
				{
				ZonePattern*zp=patterns->Zone(mpi->patidx);
				if(zp)
					{
					for(int item=0;item<tradeitem::amount;item++)
						{
						IncSurplus(item,zp->tradePerUnit[item].make);
						IncShortage(item,zp->tradePerUnit[item].take);
						}
					}
				}
			}


		}
	}

void Ward::Air_Receive(int item,int airamt)
	{
	IncSurplus(item,airamt);
	}

void Ward::IncSurplus(int item,int amt)
	{
	TradeItem_SpaceBalance*itembal;
	itembal=gettradebal(item);
	if(itembal)
		#if(useShortageAndSurplus)
			{
			itembal->surplus+=amt;
			FixShortageAndSurplus(item);
			}
		#else
			*itembal+=amt;
		#endif
	}

void Ward::IncShortage(int item,int amt)
	{
	TradeItem_SpaceBalance*itembal;
	itembal=gettradebal(item);
	if(itembal)
		#if(useShortageAndSurplus)
			{
			itembal->shortage+=amt;
			FixShortageAndSurplus(item);
			}
		#else
			*itembal-=amt;
		#endif
	}

void Ward::FixShortageAndSurplus(int item)
	{
	TradeItem_SpaceBalance*itembal;
	itembal=gettradebal(item);
	if(itembal)
		#if(useShortageAndSurplus)
			{
			int m=min(itembal->shortage,itembal->surplus)
			itembal->shortage-=m;
			itembal->surplus-=m;
			}
		#else
			;	//do nothing since it's all one value in this case;
		#endif
	}

void Ward::RandomizeTradeBalances()
	{
	const long maxbalance=1000000;
	const long minbalance=(-maxbalance);
	long balance;

	TradeItem_SpaceBalance*wardbal;
	for(int item=0;item<tradeitem::amount;item++)
		{
		balance=rand()%(maxbalance-minbalance)+minbalance;
		wardbal=gettradebal(item);
		#if(useShortageAndSurplus)
			if(balance>0)
				{	wardbal->shortage=-balance;	wardbal->surplus=0;	}
			else
				{	wardbal->surplus=balance;	wardbal->shortage=0;	}
		#else
			if(balance>0)
				*wardbal=balance;
			else
				*wardbal=balance;
		#endif
		}
	}

void Ward::RandomizeConnections(int pctconnected)
	{
	for(int pair=0;pair<route::amt;pair++)
		{
		//set the edge2edge connection of ward to true a certain percentage of times
			isconnectedAry[pair]=rand()%100+1<=pctconnected;
		}
	}

//coding in progress:

bool Ward::HasShortage();
	{
	#if(useShortageAndSurplus)
		return balance[item].shortage>0;
	#else
		return balance[item]<0;
	#endif
	}

bool Ward::HasSurplus()
	{
	#if(useShortageAndSurplus)
		return balance[item].surplus>0;
	#else
		return balance[item]>0;
	#endif
	}

void Ward::UpdatePeakTrafficVol()
	{
	peakTrafficVol=max(curTrafficVol,curTrafficVol);
	}

