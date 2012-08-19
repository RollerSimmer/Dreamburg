#include "SlopeFaller.h"

#include <stack>
#include <algorithm>

using namespace std;

SlopeFaller::SlopeFaller(IsoTerrain*_ter,TerrainEroder*_eroder,int _x,int _y,int _radius)
		:	ter(_ter)
		,	eroder(_eroder)
		,	x(_x)	,	y(_y)
		,	curht(wgthoodht(0,0))
		,	radius(_radius)
	{
	int o=141*radius,d=100*radius;
	hooddists={{d,o,d},{o,1,o},{d,o,d}};
	ter=_ter;
	ResetHood(x,y);
	}

void SlopeFaller::ResetHood(int _x,int _y)
	{
	x=_x;	y=_y;
	int xo,yo;
	ter->SetSafetyHt(ter->ht(x,y));
	for(int j=-1;j<=1;j++)
	for(int i=-1;i<=1;i++)
		{
		int htdiff=ter->ht(x+i*radius,y+j*radius)-ter->ht(x,y);
		wgthoodht(i,j)=htdiff*100/max(hooddist(i,j),1);
		visited(i,j)=eroder->visited(x+i*radius,y+j*radius);
		}
	}

struct pnStackStruct	{	int i,j;	};

void SlopeFaller::PickNeighbor(int&rx,int&ry)
	{
	int i,j;
	pnStackStruct ss;
	stack<pnStackStruct> tiestack;
	int minwgtht=0;
	//find min:
		for(j=-1;j<=1;j++) for(i=-1;i<=1;i++)	{	/*1*/
			if(!visited(i,j)&&wgthoodht(i,j)<=minwgtht)
				minwgtht=wgthoodht(i,j);	}	/*1*/
	//clear stack
		while(!tiestack.empty())	tiestack.pop();
	//push all i,j whose height diff is equal to min
		for(j=-1;j<=1;j++) for(i=-1;i<=1;i++)
			{
			if(!visited(i,j)&&wgthoodht(i,j)==minwgtht)
				{
				ss.i=i;	ss.j=j;
				tiestack.push(ss);
				}
			}
	//find size of stack and an number of entries to pop before taking one
		int stacksz=tiestack.size();
		int npops;
		npops=	(stacksz==0)	?	0	:	rand()%(stacksz);
	//pop until selected entry is on top
		if(stacksz>0)
			{
			while(npops>0)
				{	tiestack.pop();	--npops;	}

			ss=tiestack.top();
			//return values
			rx=x+ss.i*radius;
			ry=y+ss.j*radius;
			}
		else	{	rx=x;	ry=y;	}
	//clear stack
		while(!tiestack.empty())	tiestack.pop();
	}


