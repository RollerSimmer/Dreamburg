#include "TerrainEroder.h"
#include "SlopeFaller.h"
////#include "FlowFieldDrawer.h"
#include "intvector2d.h"
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
		}
	}

bool&TerrainEroder::visited(int x,int y)
	{
	if(x<0||y<0||x>=w||y>=l)
		return safetyvisited;
	else
		return *(bool*)(&visitedflags[y*w+x]);
	}

void TerrainEroder::Erode(int ncycles)
	{
	for(int cycle=ncycles;cycle>0;cycle--)
		{
		RoundSetup();
		Rainfall();
		SmoothAllTerrain();
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
	////int neighbordist=1;
	SlopeFaller sf(ter,this,x,y,neighbordist);
	sf.PickNeighbor(nx,ny);
	}

bool TerrainEroder::OddsMet(int pctchance)
	{
	return rand()%101<=pctchance;
	}

void TerrainEroder::SingleDrop(int x,int y)
	{
	//reset visited flags before this drop goes down slopes
		ResetVisitedFlags();
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
				PickFallNeighbor(nx,ny,x,y);
				////cout<<nx<<" "<<ny<<" "<<x<<" "<<y<<endl;
				htdiff=ter->ht(x,y)-ter->ht(nx,ny);	//neg for drops, pos for rises
			//add to total direction
				dirinc.SetX((nx-x)*htdiff);	dirinc.SetY((ny-y)*htdiff);
				dir+=dirinc;
			//normalize direction
				dirnorm=dir;	dirnorm/=pathlen;
			//figure out if done or not
				done=x==nx&&y==ny;
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
						soilaccum+=CollectSoilPct(100,10,x,y);
					//figure direction to fall
						int maxdirdim=max(abs(dir.GetX()),abs(dir.GetY()));
						maxdirdim=max(maxdirdim,1);	//avoid div by zero
						intvector2d o=dir;
						o*=3;	o*=radius/2;
						o/=2;	o/=maxdirdim;
						int xo=range(o.GetX(),-radius/2,radius/2);
						int yo=range(o.GetY(),-radius/2,radius/2);
						done=done||(xo==0&&yo==0);
						x+=xo;	y+=yo;
					//reduce dir speed
						dir*=1;						dir/=2;
					}
				else
					done=true;
			}
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
