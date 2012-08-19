#include "TerrainEroder.h"
#include "SlopeFaller.h"
////#include "FlowFieldDrawer.h"
#include "../intvector2d/intvector2d.h"
#include <algorithm>
#include <stack>
#include <iostream>


using namespace std;

TerrainEroder::TerrainEroder(IsoTerrain*_ter,int _w,int _l,int _dropsPerRound
		                       ,int _radius,int _digdepth,int _smoothradius)
	:	ter(_ter),	w(_w)	,	l(_l)
	,	flows(_w*_l,intvector3d(0,0,0))
	,	dropsPerRound(_dropsPerRound)
	,	radius(_radius)
	,	safetyflow(0,0,0)
	,	visitedflags(w*l,false)
	,	digdepth(_digdepth)
	,	smoothradius(_smoothradius)
	{
	radius=max(radius,1);
	smoothradius=max(smoothradius,0);
	safetyvisited=true;
	ter=_ter;
	for(int i=0;i<w*l;i++)
		{
		flows.push_back(intvector3d(0,0,0));
		visitedflags.push_back(false);
		slidetallies.push_back((NeighborTally){0,0,0,0,0,0,0,0,0});
		htrecalls.push_back(0);
		}
	ResetSlideTallies();
	SaveHeights();
	}

int TerrainEroder::edgex(int x)	{	return min(max(x,0),w-1);	}
int TerrainEroder::edgey(int y)	{	return min(max(y,0),l-1);	}

bool TerrainEroder::OnMap(int x,int y)
	{	return x>=0||y>=0||x<w||y<l;	}

bool&TerrainEroder::visited(int x,int y)
	{
	if(!OnMap(x,y))
		return safetyvisited;
	else
		return *(bool*)(&visitedflags[y*w+x]);
	}

int&TerrainEroder::htrecall(int x,int y)
	{
	if(!OnMap(x,y))
		return safetyhtrecall;
	else
		return htrecalls[y*w+x];
	}

bool TerrainEroder::StillUnvisitedNeighborsLeft(int x,int y)
	{
	bool allvisited=true;
	for(int j=-1;j<=1;j++)
	for(int i=-1;i<=1;i++)
		{
		if(i!=0||j!=0)
			allvisited=allvisited||visited(x+i,y+j);
		}
	}

NeighborTally&TerrainEroder::slidetally(int x,int y)
	{
	if(!OnMap(x,y))
		return slidetally(edgex(x),edgey(y));
	else
		return slidetallies[y*w+x];
	}

void TerrainEroder::Erode(int ncycles)
	{
	for(int cycle=ncycles;cycle>0;cycle--)
		{
		RoundSetup();
		Rainfall();
		////SmoothAllTerrain();
		if(!ShrinkDrops())	return;
		}
	}

void TerrainEroder::SmoothAllTerrain()
	{
	ter->SmoothAll(smoothradius);
	}

void TerrainEroder::RoundSetup()
	{
	////ResetFlowField();
	////ResetVisitedFlags();
	}

void TerrainEroder::ResetVisitedFlags()
	{
	for(int y=0;y<l;y++)
	for(int x=0;x<w;x++)
		{
		visited(x,y)=false;
		////cout<<"visited("<<x<<","<<y<<")="<<visited(x,y)<<endl;
		}
	}

void TerrainEroder::ResetSlideTallies()
	{
	for(int y=0;y<l;y++)
	for(int x=0;x<w;x++)
		{
		slidetally(x,y)=(NeighborTally){0,0,0,0,0,0,0,0,0};
		}
	}

void TerrainEroder::SaveHeights()
	{
	for(int y=0;y<l;y++)
	for(int x=0;x<w;x++)
		{
		htrecall(x,y)=ter->ht(x,y);
		}
	}

bool TerrainEroder::ShrinkDrops()
	{
	dropsPerRound*=2;		//goes up
	digdepth=digdepth*7/10;	//goes down
	smoothradius=smoothradius*7/10;		//goes down
	smoothradius=max(smoothradius,1);
	radius=radius*7/10;		//goes down
	radius=max(radius,1);

	if(digdepth==0)	return false;
	return true;
	}

void TerrainEroder::Rainfall()
	{
	int ndrops=rand()%dropsPerRound;
	for(int d=0;d<ndrops;d++)
		{
		int x=rand()%w;
		int y=rand()%l;
		SingleDrop(x,y);
		}
	}

void TerrainEroder::PickFallNeighbor(int&nx,int&ny,int x,int y)
	{
	int neighbordist=max(radius,1);
	const int tallythreshold=1000;
	NeighborTally*stally=&slidetally(x,y);
	int tallytotal=stally->total();
	if(tallytotal>=tallythreshold)
		{
		//use previous tallies as weights in a random selection:
			stally->select(nx,ny,x,y,neighbordist);
		}
	else
		{
		//use height offsets from maxmimum as weights in a random selection:
			NeighborTally htally;
			safetyhtrecall=htrecall(x,y);
			int topht=htrecall(x,y)+1;
			htally.c=4;	////htally.c=max(topht-ter->ht(x,y),1);
			htally.n=max((topht-htrecall(x,y-neighbordist))*3,4);
			htally.e=max((topht-htrecall(x-neighbordist,y))*3,4);
			htally.w=max((topht-htrecall(x+neighbordist,y))*3,4);
			htally.s=max((topht-htrecall(x,y+neighbordist))*3,4);
			htally.nw=max((topht-htrecall(x-neighbordist,y-neighbordist))*3,4);
			htally.ne=max((topht-htrecall(x+neighbordist,y-neighbordist))*3,4);
			htally.se=max((topht-htrecall(x+neighbordist,y+neighbordist))*3,4);
			htally.sw=max((topht-htrecall(x-neighbordist,y+neighbordist))*3,4);
			htally.c*=htally.c;
			htally.n*=htally.n;
			htally.s*=htally.s;
			htally.e*=htally.e;
			htally.w*=htally.w;
			htally.nw*=htally.nw;
			htally.ne*=htally.ne;
			htally.se*=htally.se;
			htally.sw*=htally.sw;
			htally.select(nx,ny,x,y,neighbordist);
		}
	//add to starting point's slide tally based on which direction fell
		bool wentN=ny>y,	wentS=ny<y,	wentE=nx<x,	wentW=nx>x;
		bool wentNE=wentN&&wentE,	wentSE=wentS&&wentE;
		bool wentSW=wentS&&wentW,	wentNW=wentN&&wentW;
		if(wentNE)			{	slidetally(x,y).ne++;	/*cout<<"north ";*/	}
		else if(wentNW)	{	slidetally(x,y).nw++;	/*cout<<"north ";*/	}
		else if(wentSW)	{	slidetally(x,y).sw++;	/*cout<<"north ";*/	}
		else if(wentSE)	{	slidetally(x,y).se++;	/*cout<<"north ";*/	}
		else if(wentN)		{	slidetally(x,y).n++;	/*cout<<"north ";*/	}
		else if(wentS)		{	slidetally(x,y).s++;	/*cout<<"south ";*/	}
		else if(wentE)		{	slidetally(x,y).e++;	/*cout<<"east ";*/	}
		else if(wentW)		{	slidetally(x,y).w++;	/*cout<<"west ";*/	}
		else 					{	slidetally(x,y).c++;	}
	//
	}

bool TerrainEroder::OddsMet(int pctchance)
	{
	return rand()%101<=pctchance;
	}

void TerrainEroder::SingleDrop(int x,int y)
	{
	//reset visited flags before this drop goes down slopes
		ResetVisitedFlags();
	//save the heights before eroding; this is for recalling
		SaveHeights();
	//set up variables
		int pathlen=0;
		const int accumthresh=0;	//accumulation threshold
		int soilaccum=0;		//accumulated soil
		bool done=pathlen>=maxpathlen;
		int nx,ny;
		intvector2d dir(0,0);	//direction
		intvector2d dirnorm(0,0);
		intvector2d dirinc(0,0);
		int htdiff;
	//
		while(!done)
			{
			++pathlen;
			visited(x,y)=true;
			//move to new position
				ter->SetSafetyHt(ter->ht(x,y));
				do	{
					PickFallNeighbor(nx,ny,x,y);
					}	while(StillUnvisitedNeighborsLeft(x,y)&&visited(nx,ny));

				////cout<<nx<<" "<<ny<<" "<<x<<" "<<y<<endl;
				htdiff=ter->ht(x,y)-ter->ht(nx,ny);	//neg for drops, pos for rises
			//add to total direction
				dirinc.SetX(nx-x);	dirinc.SetY(ny-y);
				////dirinc*=max(htdiff,1);
				////dirinc*=digdepth;
				dir+=dirinc;
			//normalize direction
				int hidim=max(abs(dir.GetX()),abs(dir.GetY()));
				hidim=max(hidim,1);
				dirnorm=dir;
				dirnorm*=3;
				dirnorm/=hidim;
				dirnorm/=2;
				////dirnorm/=digdepth;
				dirnorm*=radius;

			//figure out if done or not
				////done=x==nx&&y==ny;
				done=done||dirnorm.mag()==0;
				done=done||pathlen>=maxpathlen||nx<0||ny<0||nx>=w||ny>=l;
				done=done||visited(nx,ny);
			//update x and y if not done
				if(!done)
					{
					//10% change of deposit if over threshold
						if(soilaccum>accumthresh)
							{	DepositSoilPct(10,x,y,soilaccum);	}
					//else, accumulate 100% of soil surfaces encountered and 25% of rock
						soilaccum+=CollectSoilPct(10,100,x,y);
					//figure direction to fall
						int maxdirdim=max(abs(dir.GetX()),abs(dir.GetY()));
						maxdirdim=max(maxdirdim,1);	//avoid div by zero
						intvector2d o=dirnorm;
						////o*=3;	o*=radius/2;
						////o/=2;	o/=maxdirdim;
						int jumpstep=max(radius/2,1);
						////int xo=range(o.GetX(),-jumpstep,jumpstep);
						////int yo=range(o.GetY(),-jumpstep,jumpstep);
						int xo=nx-x;
						int yo=ny-y;	//keep it simple for now

						////done=done||(xo==0&&yo==0);
						x+=xo;	y+=yo;
					//reduce dir speed
						////dir*=1;						dir/=2;
					}
				else
					done=true;
			}
	////cout<<"pathlen="<<pathlen<<"\t";
	}

int TerrainEroder::CollectSoilPct(int rockpct,int soilpct,int x,int y)
	{
	int depth;
	if(ter->ht(x,y)==0&&ter->rockht(x,y)==0)	//bottom level, can't collect
		return 0;
	else if(ter->ht(x,y)>ter->rockht(x,y))	//soil surface
		{
		depth=min(digdepth*soilpct/100,(int)ter->ht(x,y));
		ter->RaiseOctagon(x,y,radius,-depth);
		return depth;
		}
	else	//rock surface
		{
		depth=min(digdepth*rockpct/100,(int)ter->rockht(x,y));
		ter->RaiseOctagon(x,y,radius,-depth);
		return depth;
		}
	}

int TerrainEroder::CollectSoil(int rockchance,int soilchance,int x,int y)
	{
	int depth;
	if(ter->ht(x,y)==0&&ter->rockht(x,y)==0)	//bottom level, can't collect
		return 0;
	else if(ter->ht(x,y)>ter->rockht(x,y))	//soil surface
		{
		depth=min(digdepth,(int)ter->ht(x,y));
		if(OddsMet(soilchance))
			{	ter->RaiseOctagon(x,y,radius,-depth);	return 1;	}
		else	return 0;
		}
	else	//rock surface
		{
		depth=min(digdepth,(int)ter->rockht(x,y));
		if(OddsMet(rockchance))
			{	ter->ht(x,y)--;	ter->RaiseOctagon(x,y,radius,-depth);	return 1;	}
		else	return 0;
		}
	}

void TerrainEroder::DepositSoilPct(int pct,int x,int y,int&soilaccum)
	{
	int depth=min(digdepth*pct/100,(int)ter->GetZCap()-ter->ht(x,y));
	////depth=min(depth,soilaccum);
	depth=max(depth,0);
	ter->RaiseOctagon(x,y,radius,depth);
	soilaccum-=depth;
	}

void TerrainEroder::DepositSoil(int pctchance,int x,int y)
	{
	int depth=min(digdepth,(int)ter->GetZCap()-ter->ht(x,y));
	if(OddsMet(pctchance))
		{
		ter->RaiseOctagon(x,y,radius,depth);
		}
	}
