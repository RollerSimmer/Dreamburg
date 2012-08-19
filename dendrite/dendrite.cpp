#if 0
	#include <allegro.h>
#endif
#include "dendrite.h"
#include <algorithm>
#include <iostream>
#include <cstring>
#include <list>
#include <stack>

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

LatticePoint*Dendrite::pt(int x,int y,bool usesafecoords)
	{
	if(inrange(x,y))
		return &points[y*w+x];
	else if(usesafecoords)
		{
		//return a safe point
			////safepoint=*pt(safex(x),safey(y));
			////return &safepoint;
			return pt(safex(x),safey(y));
		}
	else
		{
		return (LatticePoint*)0;
		}
	}

bool Dendrite::inrange(int x,int y)
	{
	return x>=0&&y>=0&&x<w&&y<l;
	}

bool Dendrite::onedge(int x,int y,int edge)
	{
	switch(edge)
		{
		case cornerur: return x==w-1&&y==0;
		case cornerul: return x==0&&y==0;
		case cornerdl: return x==0&&y==l-1;
		case cornerdr: return x==w-1&&y==l-1;
		case edgelft:	return x==0&&!(y==w-1||y==0);
		case edgergt:	return x==w-1&&!(y==w-1||y==0);
		case edgetop:	return y==0&&!(x==l-1||x==0);
		case edgebtm:	return y==l-1&&!(x==l-1||x==0);
		case edgenone:
		default:
			return false;
      }
	}

bool Dendrite::OnAnyEdge(int x,int y)
	{
	int edge;
	bool oae=false;
	for(edge=0;edge<amtedges;edge++)
		oae=oae||onedge(x,y,edge);
	return oae;
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
		cp->x=x*spacing+rand()%max((int)spacing*8/10,(int)1)+spacing/10;
		cp->y=y*spacing+rand()%max((int)spacing*8/10,(int)1)+spacing/10;
		cp->z=0;
		////cp->x=x*spacing+rand()%(spacing*10/10)+spacing*0/10;
		////cp->y=y*spacing+rand()%(spacing*10/10)+spacing*0/10;
		cp->wgt=rand()%(latticepoint_maxwgt-latticepoint_minwgt+1);
		cp->wgt+=latticepoint_minwgt;
		cp->r= pt(x+1,y,false);
		cp->d= pt(x,y+1,false);
		cp->l= pt(x-1,y,false);
		cp->u= pt(x,y-1,false);
		cp->ur=pt(x+1,y-1,false);
		cp->ul=pt(x-1,y-1,false);
		cp->dr=pt(x+1,y+1,false);
		cp->dl=pt(x-1,y+1,false);
		}
	UnlinkPoints();
	ClearVisits();
	}

void Dendrite::ClearVisits()
	{
	for_xy()
		{
		pt(x,y)->routelen=0;
		}
	}

void Dendrite::UnlinkPoints()
	{
	safepoint.next=0;
	exitpoint.next=0;
	for_xy()
		{
		pt(x,y)->next=0;
		}
	}

void Dendrite::WalkStart(int x,int y,int &dir,bool&done)
	{
	static
		#include "startdirs.inl"
		;

	int edge=edgenone;
	for(int ei=0;ei<=edgenone;ei++)
		{
		if(onedge(x,y,ei))
				{
				edge=ei;
				break;
				}
		}
	////cout<<"edge="<<edge;
	//initialize walk by setting the direction based on edge:
		if(pt(x,y)->next!=0)
			{
			done=true;
			dir=dirNoneYet;
			}
		else if(edge!=edgenone)
			dir=startdirs[edge];
		else
			dir=dirNoneYet;
	////cout<<" dir="<<dir;
	////cout<<endl;

	if(!done)
		WalkStep(x,y,dir,done);
	}

void Dendrite::WalkStep(int &x,int &y,int &dir,bool&done)
	{
	int dirxs[8]={	1,1,0,-1,-1,-1,0,1	};
	int dirys[8]={	0,-1,-1,-1,0,1,1,1	};
	dir=ClosestNeighborDirection(x,y,dir);
	if(dir<dirNoneYet)
		{	pt(x,y)->next=ngbr(x,y,dir);

			done=pt(x,y)->next->next!=0;
			x=x+dirxs[dir];
			y=y+dirys[dir];
			////done=done||OnAnyEdge(x,y);
			bool choicesleft=false;
			for(int diro=-1;diro<=1;diro++)
				{
				int tdir=fixdir(dir+diro);
				choicesleft=choicesleft||inrange(x+dirxs[tdir],y+dirys[tdir]);
				}
			done=done||!choicesleft;
		}
	else
		{	////pt(x,y)->next=0;
			done=true;	}
	}

void Dendrite::WalkFrom(int x,int y)
	{
	bool done=false;
	LatticePoint*lp;
	lp=pt(x,y);
	int xo,yo;
	int dir=dirNoneYet;

	WalkStart(x,y,dir,done);

	while(!done)
		{
		WalkStep(x,y,dir,done);
		}
	}

LatticePoint*Dendrite::ngbr(int x,int y,int dir)
	{
	//fix direction:
		dir=fixdir(dir);
	//set up neighbor array to current point:
		LatticePoint*lp=pt(x,y);
		if(lp==0)	return 0;
		LatticePoint*ngbrs[8]=
			{	lp->r,lp->ur,lp->u,lp->ul,lp->l,lp->dl,lp->d,lp->dr	};
	//return neighbor array entry with dir as index
		return ngbrs[dir];
	}

int Dendrite::ClosestNeighborDirection(int x,int y,int prevdir)
	{
	int dir=prevdir;
	LatticePoint*clp=pt(x,y);
	int lodist;
	const int lw=1;	//low weight
	const int hw=1;	//high weight

	if(prevdir==dirNoneYet)
		{
		//any possible direction
			int distances[8];
			LatticePoint*ngbrs[8];
			int wgts[8]={lw,hw,lw,hw,lw,hw,lw,hw};
			for(int diri=0;diri<8;diri++)
				{
				ngbrs[diri]=ngbr(x,y,diri);
				distances[diri]=Dendrite::dist(clp,ngbrs[diri])
				                *wgts[diri]*ngbrs[diri]->wgt;
				if(diri==0)	lodist=distances[diri];
				else	lodist=min(lodist,distances[diri]);
				}
			//find lodist's index and set next link to that set of values:
				bool lodistfound=false;
				int diri;
				while(!lodistfound)
					{
					diri=rand()%8;	//this way each direction has equal chance
					lodistfound=lodist==distances[diri];
					}
				//diri should now have the lowest distance, proceed:
					return diri;
		}
	else
		{
		//using a pitchfork chose, only three options are available:
			int distances[3],dirs[3];
			LatticePoint*ngbrs[3];
			int wgts[3]={lw,hw,lw};
			for(int diri=0;diri<3;diri++)
				{
				dirs[diri]=fixdir(prevdir-1+diri);
				ngbrs[diri]=ngbr(x,y,dirs[diri]);
				distances[diri]=Dendrite::dist(clp,ngbrs[diri])
				                *wgts[diri]*ngbrs[diri]->wgt;
				if(diri==0)	lodist=distances[diri];
				else	lodist=min(lodist,distances[diri]);
				}
			//find lodist's index and set next link to that set of values:
				bool lodistfound=false;
				int diri;
				while(!lodistfound)
					{
					diri=rand()%3;	//this way each direction has equal chance
					lodistfound=lodist==distances[diri];
					}
				//diri should now have the lowest distance, proceed:
					return dirs[diri];
		}

	}

int Dendrite::dist(LatticePoint*p1,LatticePoint*p2)
	{
	if(p1==0||p2==0)	return 0x7fff;
	int m,M,dx,dy,retdist;
	dx=abs(p2->x-p1->x);
	dy=abs(p2->y-p1->y);
	m=min(dx,dy);
	M=max(dx,dy);
	retdist=M+m*340/100;
	return retdist;
	}

int Dendrite::slope(LatticePoint*p1,LatticePoint*p2)
	{
	if(p1==0||p2==0)	return 0x7fff;
	int retslope,xydist,dz;
	dz=p2->z-p1->z;
	xydist=max((int)dist(p1,p2),(int)1);
	retslope=dz*100/xydist;
	static int coutcount=0;
	#if 0
		if(retslope>0&&coutcount<100)
			{	cout<<"ret slope is over 0";	++coutcount;	}
		else if(retslope<0&&coutcount<100)
			{	cout<<"ret slope is under 0";	++coutcount;	}
	#endif

	return retslope;
	}

int Dendrite::fixdir(int dir)
	{
	int fdir=dir%8;
	while(fdir<0)	{	fdir+=8;	}
	return fdir;
	}

void Dendrite::FillMap()
	{
	//initialize points
		InitPoints();
	//perimeter loop
		int x,y,t,edge,amtedgeseeds;
		amtedgeseeds=(getl()+getw())/10;
		for(t=0;t<amtedgeseeds;t++)
			{
			edge=rand()%4+edgelft;
			switch(edge)
				{
				case edgelft:
					x=0;	y=rand()%getl();
					break;
				case edgergt:
					x=getw()-1;	y=rand()%getl();
					break;
				case edgetop:
					y=0;	x=rand()%getw();
					break;
				case edgebtm:	default:
					y=getl()-1;	x=rand()%getw();
					break;
				}
			WalkFrom(x,y);
			}
	#define fill_doscatter 1
	#if fill_doscatter
		//scattter
			ScatterWalk();
		//sequential
			SequentialWalk();
	#endif
	}

struct LatticePointSortData
	{
	LatticePoint*p;
	int val;
	};

bool LatticePointLessThan(LatticePointSortData a,LatticePointSortData b)
	{
	return (a.val<b.val);
	}

bool LatticePointMoreThan(LatticePointSortData a,LatticePointSortData b)
	{
	return (a.val>b.val);
	}

void Dendrite::WalkBackwardsFrom(int x,int y,bool byht)
	{
	//variables:
		ShuffleQueue<LatticePoint*> lpsq;
		LatticePoint  *lp
		             ,*ngbrlist[8]
		             ,*lpdefault;
		lpdefault=(LatticePoint*)0;

	//Push the current point
		while(!lpsq.empty())	{	lpsq.pop();	}
		pt(x,y)->next=&exitpoint;
		lpsq.push(pt(x,y));

	//test map heights by displaying them
		#if 0
			acquire_screen();
				rectfill(screen,0,0,w,l,0x000000);
				for(int y=0;y<l;y++)
				for(int x=0;x<w;x++)
					{
					int level=min((int)pt(x,y)->z,255);
					int color=makecol(level,level,level);
					putpixel(screen,x,y,color);
					}
			release_screen();
			rest(2000);
		#endif

	//Keep popping and processing until the shuffle queue is empty
		Walk_ProcessShuffleQueue(&lpsq,byht);
	}

void Dendrite::CycleAmtLinks(int&amtlinks)
	{
	#if 0
		amtlinks=amtlinks;
	#elif 0
		static int round=1;
		const int branchround=2;
		if(round==branchround)			{			amtlinks=2;			round=1;			}
		else			{			amtlinks=1;			++round;			}
	#elif 0 //8-7-6-5-4-3-2-1
		if(amtlinks>1) --amtlinks;
		else           amtlinks=8;
	#elif 0 //4-3-2-1
		if(amtlinks>1) --amtlinks;
		else           amtlinks=4;
	#elif 0	//3-2-1
		if(amtlinks>1)	--amtlinks;
		else           amtlinks=3;
	#elif 0	//1-90%, 2-10%
		int rv=rand()%100;
		if(rv<=90)	amtlinks=1;
		else           amtlinks=2;
	#elif 0	//2-1
		if(amtlinks>1)	--amtlinks;
		else           amtlinks=2;
	#elif 0 //all 1s
		amtlinks=1;
	#elif 0 //all 2s
		amtlinks=2;
	#elif 1 //all 3s
		amtlinks=3;
	#endif
	}

void Dendrite::FillMapWithBackTracing(bool byht,bool doinitpts)
	{
	int edge,x,y;
	//initialize points
		if(doinitpts)
			InitPoints();

		#if 0
			////case edgelft:
				x=0;	y=rand()%getl();
				WalkBackwardsFrom(x,y,byht);
				////break;
			////case edgergt:
				x=getw()-1;	y=rand()%getl();
				WalkBackwardsFrom(x,y,byht);
				////break;
			////case edgetop:
				y=0;	x=rand()%getw();
				WalkBackwardsFrom(x,y,byht);
				////break;
			////case edgebtm:
				y=getl()-1;	x=rand()%getw();
				WalkBackwardsFrom(x,y,byht);
				////break;
			////case edgenone:	default:
				x=rand()%getw();	y=rand()%getl();
				WalkBackwardsFrom(x,y,byht);
				////break;
			////cout<<"Walking backwards from ("<<x<<","<<y<<")"<<endl;
		#endif

	#define fill_doscatter 1
	#if fill_doscatter
		//scattter
			ScatterWalk(byht);
		//sequential
			SequentialWalk(byht);
	#endif
	}

void Dendrite::ScatterWalk(bool byht)
	{
	ScatterEdgeWalk(byht,w);
	}

void Dendrite::ScatterEdgeWalk(bool byht,int maxspacing)
	{
	#if 1 //test a value of maxspacing
		maxspacing=w*8;
	#endif

	int amtseeds,x,y,t;
	////amtseeds=getl()*getw();
	amtseeds=w;
	//variables:
		ShuffleQueue<LatticePoint*> lpsq;
		LatticePoint*lp;

	//push edge points
		while(!lpsq.empty())	{	lpsq.pop();	}
		//right edge
			x=w-1;
			for(y=rand()%(maxspacing+1);y<l;y+=rand()%(maxspacing+1)+1)
				{		pt(x,y)->next=&exitpoint;	lpsq.push(pt(x,y));	}
		//top edge
			y=0;
			for(x=rand()%(maxspacing+1);x<w;x+=rand()%(maxspacing+1)+1)
				{		pt(x,y)->next=&exitpoint;	lpsq.push(pt(x,y));	}
		//left edge
			x=0;
			for(y=rand()%(maxspacing+1);y<l;y+=rand()%(maxspacing+1)+1)
				{		pt(x,y)->next=&exitpoint;	lpsq.push(pt(x,y));	}
		//btm edge
			y=l-1;
			for(x=rand()%(maxspacing+1);x<w;x+=rand()%(maxspacing+1)+1)
				{		pt(x,y)->next=&exitpoint;	lpsq.push(pt(x,y));	}

	//Keep popping and processing until the shuffle queue is empty
		Walk_ProcessShuffleQueue(&lpsq,byht);
	}

void Dendrite::Walk_ProcessShuffleQueue( ShuffleQueue<LatticePoint*>*lpsq
                                        ,bool byht)
	{
	LatticePoint*lp;
	int amtlinks=1; //cycle down 3,2,1,3,2,1,...
	while(!(lpsq->empty()))
		{
		Walk_ShufflePop(lp,lpsq,amtlinks);
		Walk_PushNeighbors(lp,lpsq,amtlinks,byht);
		}
	}

void Dendrite::Walk_ShufflePop(LatticePoint*&lp,ShuffleQueue<LatticePoint*>*lpsq
                               ,int&amtlinks)
	{
	int shuffletimes,lpsq_sz;
	LatticePoint*lpdefault=(LatticePoint*)0;
	//cycle the number of links
		CycleAmtLinks(amtlinks);
	//first shuffle the queue of points:
		lpsq_sz=lpsq->size();
		if(lpsq_sz>0)
			shuffletimes=rand()%min(lpsq_sz,20);
		else
			shuffletimes=0;
		for(int shufflei=0;shufflei<shuffletimes;shufflei++)
			lpsq->shuffle();
	//take the top after the shuffle:
		lp=lpsq->take_front(lpdefault);
	}

void Dendrite::Walk_PushNeighbors( LatticePoint*lp,ShuffleQueue<LatticePoint*>*lpsq
                                  ,int amtlinks,bool byht)
	{
	LatticePoint
		*ngbrlist[8]
		,*lpdefault;

	//process if not empty:
		if(lp)
			{
			memcpy(ngbrlist,&(lp->r),sizeof(LatticePoint*)*8);
			list<LatticePointSortData> ngbrsort;
			LatticePointSortData cngbr;
			for(int i=0;i<8;i++)
				{
				cngbr.p=ngbrlist[i];
				if(cngbr.p)
					{
					if(byht)
						cngbr.val=slope(lp,cngbr.p);
					else
						cngbr.val=dist(lp,cngbr.p);
					}
				else
					{
					if(byht)
						cngbr.val=-1;
					else
						cngbr.val=spacing*10000;
					}

				if(byht)
					{
					if(cngbr.val<=0)
						{	cngbr.val++;	cngbr.val--;	}	//do nothing
					else
						ngbrsort.push_back(cngbr);
					}
				else
					ngbrsort.push_back(cngbr);
				}
			if(byht)
				ngbrsort.sort(LatticePointMoreThan);
			else
				ngbrsort.sort(LatticePointMoreThan);
			//take the three closest neighbors, link them to the current point, and push each:
				int linki=0;
				int nssz=ngbrsort.size();
				while(!ngbrsort.empty())
					{
					cngbr=ngbrsort.front();
					ngbrsort.pop_front();
					//if next is empty and neigbor pointer is not, push:
						if(cngbr.p==&safepoint||cngbr.p==0)
							{
							safepoint.next=0;
							}
						else if(linki<amtlinks&&cngbr.p->next==0)
							{
							cngbr.p->next=lp;
							lpsq->push(cngbr.p);
							++linki;
							}
					}

			}
	}

void Dendrite::SequentialWalk(bool byht)
	{
	int x,y;
	for(x=0;x<getw();x++)
		{
		for(y=0;y<getl();y++)
			{
			if(pt(x,y)->next==0)
				WalkBackwardsFrom(x,y,byht);
			}
		}
	}

void Dendrite::TracePaths(int amtseeds)
	{
	//variables:
		int seedi,x,y,routelen;
		LatticePoint*lp;
		bool done;
		int routelen_max=(w+l);
	//clear previous visits:
		ClearVisits();
	//scatter some seeds and trace their paths:
		for(seedi=0;seedi<amtseeds;seedi++)
			{
			x=rand()%w;	y=rand()%l;
			lp=pt(x,y);
			done=false;
			routelen=0;
			while(!done)
				{
				if(lp==0)
					done=true;
				else
					{
					lp->routelen=max(lp->routelen,routelen);
					if (lp->next==0)
						done=true;
					else if (lp->next==&exitpoint)
						done=true;
					else if(lp->next==lp)
						done=true;
					else if(routelen>=routelen_max)
						done=true;
					else
						{
						lp=lp->next;
						int inccase=rand()%10+1;
						if(inccase<=3)	//1..3
							routelen+=2;
						if(inccase<=8)	//4..8
							routelen+=0;
						else if(inccase<=10)	//9..10
							routelen-=1;
						}

					}
				}
			}
	//Count Branches as well:
		CountBranches();

	}

void Dendrite::CountBranches()
	{
	LatticePoint*lp;

	//step through lattice and set all nested branch counts to 0
		for(int y=0;y<l;y++)
		for(int x=0;x<w;x++)
			{
			lp=pt(x,y);
			if(lp)
				{
				lp->amtNestedBranches=0;
				lp->amtbranches=0;
				}
			}

	//step through lattice and find sources (points with no links to) and follow path
		for(int y=0;y<l;y++)
		for(int x=0;x<w;x++)
			{
			if(lp)
				{
				if(lp->next==GetExitPoint())
					{
					lp->amtNestedBranches=CountBranches(lp);
					}
				}
			}
	}

int Dendrite::CountBranches(LatticePoint*lp)
	{
	if(!lp) return 0;
	lp->amtbranches=CountImmediateBranches(lp);
	if(lp->amtbranches==0)	//base case
		return 1;
	else
		{
		int branchsum=0;
		LatticePoint*nbrary[8];
		memcpy(nbrary,&(lp->r),sizeof(LatticePoint*)*8);
		for(int n=0;n<8;n++)
			if(nbrary[n])
				{
				if(nbrary[n]->next==lp)
					branchsum+=CountBranches(nbrary[n]);
				}
		lp->amtNestedBranches=branchsum;
		return branchsum;
		}
	}

int Dendrite::CountImmediateBranches(LatticePoint*lp)
	{
	int amtbranches=0;
	LatticePoint*nbrary[8];
	memcpy(nbrary,&(lp->r),sizeof(LatticePoint*)*8);
	for(int n=0;n<8;n++)
		if(nbrary[n])	{	if(nbrary[n]->next==lp)	++amtbranches;	}
	return amtbranches;
	}
