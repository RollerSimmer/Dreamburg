#include "dendrite.h"
#include <algorithm>

using namespace std;

#define for_xy()	\
	for(int y=0;y<l;y++) \
	for(int x=0;x<w;x++)

Dendrite::Dendrite(int _w,int _l,int _spacing)
	{
	w=max(_w,1);
	l=max(_l,1);
	spacing=_spacing;
	points=new LatticePoint[w*l];
	InitPoints();
	}

Dendrite::~Dendrite()
	{
	delete [] points;
	}

LatticePoint*Dendrite::pt(int x,int y)
	{
	if(inrange(x,y))
		return &points[y*w+x];
	else
		{
		//return a safe point
			////safepoint=*pt(safex(x),safey(y));
			////return &safepoint;
			return pt(safex(x),safey(y));
		}
	}

bool Dendrite::inrange(int x,int y)
	{
	return x>=0&&y>=0&&x<w&&y<l;
	}

int Dendrite::safex(int x)
	{	return min(w-1,max(0,x));	}

int Dendrite::safey(int y)
	{	return min(l-1,max(0,y));	}

void Dendrite::InitPoints()
	{
	LatticePoint*cp;
	for_xy()
		{
		cp=pt(x,y);
		cp->x=x*spacing+rand()%spacing;
		cp->y=y*spacing+rand()%spacing;
		cp->right=pt(x+1,y);
		cp->up=   pt(x,y-1);
		cp->left= pt(x-1,y);
		cp->down= pt(x,y+1);
		}
	UnlinkPoints();
	}

void Dendrite::UnlinkPoints()
	{
	for_xy()
		{
		pt(x,y)->next=0;
		}
	}

void Dendrite::WalkFrom(int x,int y)
	{
	bool done=false;
	LatticePoint*lp;
	lp=pt(x,y);
	int xo,yo;
	int dir=rand()%4;
	bool dirchange=false;

	while(!done)
		{
		dirchange=rand()%100>=90;
		if(dirchange)
			{
			dir=dir+rand()%3-1;
			while(dir<0)	{	dir+=4;	}
			if(dir>=4)	dir=dir%4;
			}

		switch(dir)
			{
			case 1:	lp->next=lp->up;		break;
			case 2:	lp->next=lp->left;	break;
			case 3:	lp->next=lp->down;	break;
			case 0:
			default:	lp->next=lp->right;	break;
			}

		lp=lp->next;

		done=lp==0;
		if(!done)
			done=   lp->x<=0       || lp->y<=0
				  || lp->x>=(w-1)*4 || lp->y>=(l-1)*4
				  || lp->next!=0
				  ;
		}
	}
