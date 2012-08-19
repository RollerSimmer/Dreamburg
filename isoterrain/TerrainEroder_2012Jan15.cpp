#include "TerrainEroder.h"
#include "SlopeFaller.h"
#include "FlowFieldDrawer.h"
#include <algorithm>
#include <stack>


TerrainEroder::TerrainEroder(IsoTerrain*_ter,int _w,int _l,int _dropsPerRound,int _smoothradius)
	:	ter(_ter),	w(_w)	,	l(_l)
	,	flows(_w*_l,intvector3d(0,0,0))
	,	dropsPerRound(_dropsPerRound)
	,	smoothradius(_smoothradius)
	,	safetyflow(0,0,0)
	,	visited(_w*_l,false)
	{
	safetyvisited=true;
	ter=_ter;
	for(int i=0;i<w*l;i++)
		{
		flows.push_back(intvector3d(0,0,0));
		visited.push_back(false);
		}
	}

intvector3d&TerrainEroder::flow(int x,int y)
	{
	if(x<0||y<0||x>=w||y>=l)
		return safetyflow;
	else
		return flows[y*w+x];
	}

bool&TerrainEroder::visited(int x,int y)
	{
	if(x<0||y<0||x>=w||y>=l)
		return safetyvisited;
	else
		return visitedflags[y*w+x];
	}

void TerrainEroder::Erode(int ncycles)
	{
	for(int cycle=ncycles;cycle>0;cycle--)
		{
		RoundSetup();
		FindFlowFields();
		Rainfall();
		SmoothAllTerrain();
		}
	}

void TerrainEroder::RoundSetup()
	{
	ResetFlowField();
	ResetVisitedFlags();
	}

void TerrainEroder::ResetFlowField()
	{
	for(int y=0;y<l;y++)
	for(int x=0;x<w;x++)
		flow(x,y).SetXYZ(0,0,0);
	}

void TerrainEroder::ResetVisitedFlags()
	{
	for(int y=0;y<l;y++)
	for(int x=0;x<w;x++)
		visited(x,y)=false;
	}

void TerrainEroder::FindFlowFields()
	{
	FindFlowFields_Random();
	FindFlowFields_Sequential();
	////FlowFieldDrawer ffd(this);
	////ffd.Draw();
	}

void TerrainEroder::Rainfall()
	{
	const int maxdrops=w*l;
	int ndrops=rand()%dropsPerRound;
	for(int d=0;d<ndrops;d++)
		{
		int x=rand()%w;
		int y=rand()%l;
		SingleDrop(x,y);
		}
	}

void TerrainEroder::FindFlowFields_Random()
	{
	const int maxdrops=w*l/8;
	int ndrops=rand()%maxdrops;
	for(int d=0;d<ndrops;d++)
		{
		int x=rand()%w;
		int y=rand()%l;
		MakeFlowPathAt(x,y);
		}
	}

void TerrainEroder::FindFlowFields_Sequential()
	{
	for(int y=0;y<l;y++)
	for(int x=0;x<w;x++)
		MakeFlowPathAt(x,y);
	}

void TerrainEroder::PickFallNeighbor(int&nx,int&ny)
	{

	}

void TerrainEroder::MakeFlowPathAt(int x,int y)
	{
	if(flow(x,y)!=intvector3d(0))	//already a flow there; return
		return;
	SlopeFaller sf(ter,x,y);
	int nx,ny;
	sf.PickNeighbor(nx,ny);
	flow(x,y).SetXYZ(nx-x,ny-y,0);
	int pathlen=1;

	bool done=nx==x||ny==y||nx<0||ny<0||nx>=w||ny>=l||pathlen>=maxpathlen;

	while(!done)
		{
		sf.PickNeighbor(nx,ny);
		flow(x,y).SetXYZ(nx-x,ny-y,0);
		++pathlen;
		done=nx==x||ny==y||nx<0||ny<0||nx>=w||ny>=l||pathlen>=maxpathlen;
		if(!done)	done=flow(nx,ny)!=intvector3d(0);	//already a flow there; done
		if(!done)	{	x=nx;	y=ny;	sf.ResetHood(x,y);	}
		}
	}

bool TerrainEroder::OddsMet(int pctchance)
	{
	return rand()%101<=pctchance;
	}

void TerrainEroder::SingleDrop(int x,int y)
	{
	int pathlen=0;
	const int accumthresh=10;	//accumulation threshold
	int soilaccum=0;		//accumulated soil
	bool done=pathlen>=maxpathlen;
	while(!done)
		{
		//25% change of deposit if over threshold
			if(soilaccum>accumthresh)
				{	DepositSoil(25,x,y,radius,digdepth);	--soilaccum;	}
		//else, accumulate 50% of soil surfaces encountered and 25% of rock
			else
				soilaccum+=CollectSoil(50,25,x,y,radius,digdepth);
		++pathlen;
		visited(x,y)=true;
		//move to new position
			x+=flow(x,y).GetX();
			y+=flow(x,y).GetY();
		//figure out if done or not
			done=pathlen>=maxpathlen||x<0||y<0||x>=w||y>=l;
			done=done||flow(x,y)==intvector3d(0,0,0);
			done=done||visited(x,y);
		}
	//just follow a path defined by flow field until max path length is reached
	}

int TerrainEroder::CollectSoil(int rockchance,int soilchance,int x,int y,int radius,int depth)
	{
	if(ter->ht(x,y)==0&&ter->rockht(x,y)==0)	//bottom level, can't collect
		return 0;
	else if(ter->ht(x,y)>ter->rockht(x,y))	//soil surface
		{
		depth=min(depth,(int)ter->ht(x,y));
		if(OddsMet(soilchance))
			{	ter->RaiseOctagon(x,y,radius,-depth);	return 1;	}
		else	return 0;
		}
	else	//rock surface
		{
		depth=min(depth,(int)ter->rockht(x,y));
		if(OddsMet(rockchance))
			{	ter->ht(x,y)--;	ter->RaiseOctagon(x,y,radius,-depth);	return 1;	}
		else	return 0;
		}
	}

void TerrainEroder::DepositSoil(int pctchance,int x,int y,int radius,int depth)
	{
	depth=min(depth,(int)ter->GetZCap()-ter->ht(x,y));
	if(OddsMet(pctchance))
		{
		ter->RaiseOctagon(x,y,radius,depth);
		}
	}
