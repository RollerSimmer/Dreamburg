#include "RiverEroder.h"
#include <stack>
#include <algorithm>
#include <allegro.h>

using namespace std;

const int iary[8]={+1,+1, 0,-1,-1,-1, 0,+1};
const int jary[8]={ 0,-1,-1,-1, 0,+1,+1,+1};

#define for_xy for(int y=0;y<maplength;y++) for(int x=0;x<mapwidth;x++)

u8&RiverEroder::course(int x,int y)
	{
	if(inrange(x,y))
		return courseData[y*mapwidth+x];
	else
		{
		fixxy(x,y);
		courseDataSafe=course(x,y);
		return courseDataSafe;
		}
	}

void RiverEroder::Erode(int x,int y,int _digdepth)
	{
	digdepth=_digdepth;
	//first chart the course
		MakeCourse(x,y);

	#if 0
		DisplayCourseOnScreen();
	#endif

	#define RiverEroder_Erode_usestack 0
	#if (!RiverEroder_Erode_usestack)
		//drop course using a for loop over whole map
			for(int cy=0;cy<maplength;cy++)
			for(int cx=0;cx<mapwidth;cx++)
				{
				if(course(cx,cy))
					Drop(cx,cy);
				}
	#else
		//now drop all points in river using a stack and starting x,y as seed
			stack<int>estack;	//erode stack of x,y entries
			//if seed point has been visited, push its coordinates
				if(course(x,y))	{	estack.push(x);	estack.push(y);	}
			int cx,cy;
			while(!estack.empty())
				{
				cx=estack.top();	estack.pop();
				cy=estack.top();	estack.pop();
				//lower the current vertex:
					Drop(cx,cy);
				//clear the course flag for this point so it won't be pushed on stack again
					course(cx,cy)=0;

				for(int a=0;a<8;a++)	//step through neighbors in 45 degree increments
					{
					int nx=cx+iary[a];	int ny=cy+jary[a];
					//push neighbor x and y only if the course has passed the point
						if(inrange(nx,ny))
							{
							if(course(nx,ny))
								{	estack.push(nx);	estack.push(ny);	}
							}
					}
				}
	#endif
	}

void RiverEroder::Drop(	int x,int y	)
	{
	////ter->RaiseVertex(x,y,-digdepth);

	//try something new:
		//ignore dig depth - divide by 2:
			{
			int vmul=7;
			int vdiv=10;
			ht(x,y)=ht(x,y)*vmul/vdiv;
			ter->rockht(x,y)=ter->rockht(x,y)*vmul/vdiv;
			}
	}

void RiverEroder::ChoosePathFromHere(int&nx,int&ny,int x,int y)
	{
	//init nx and ny to x and y for safety:
		nx=x;	ny=y;

	const int scaleary[8]={10,7,10,7,10,7,10,7};
	int xary[8],yary[8];
	u8 courseary[8];

	stack<int> cstack;	//"choose stack"
	//empty stack just in case
		while(!cstack.empty())	cstack.pop();
	//find min neighbor
		int nhts[8];
		int nwgts[8];
		int wgtsum=0;
		int topht=0;
		for(int a=0;a<8;a++)	//step through angles
			{
			xary[a]=x+iary[a];
			yary[a]=y+jary[a];
			courseary[a]=course(xary[a],yary[a]);
			topht=max(topht,(int)ht(xary[a],yary[a])*scaleary[a]);
			}
		const long zhi=0x7fffffff;
		long htmin=zhi;
		for(int a=0;a<8;a++)	//step through angles
			{
			if(!inrange(xary[a],yary[a]))
				nhts[a]=0;
			else if(courseary[a]==1)
				nhts[a]=topht;
			else
				nhts[a]=ht(xary[a],yary[a])*scaleary[a];
			wgtsum+=nwgts[a];

			htmin=min(nhts[a],(int)htmin);
			}
		for(int a=0;a<8;a++)
			{
			nwgts[a]=topht-nhts[a];
			if(nhts[a]==htmin)
				nwgts[a]*=2;	//bonus pull for being the minimum.
			}

	#define RiverEroder_ChoosePathFromHere_usestack 0
	#if (!RiverEroder_ChoosePathFromHere_usestack)
		if(wgtsum>0)
			{
			int selector=rand()%wgtsum;
			int wgttally=0;
			for(int a=0;a<8;a++)
				{
				if(selector>=wgttally&&selector<wgttally+nwgts[a])
					//selector is in range:
						{	nx=xary[a];	ny=yary[a];	return;	}
				wgttally+=nwgts[a];
				}
			}
	#else
		//push any neighbors that are as low as the minimum vertex
			for(int a=0;a<8;a++)	//step through angles
				{
				if(nhts[a]==htmin&&courseary[a]==0)	//minimum and unvisited vertex
					{
					//push x and y
						cstack.push(xary[a]);
						cstack.push(yary[a]);
					}
				}
		int nentries=cstack.size()/2;
		if(nentries>0)
			{
			int stackselector=rand()%nentries;
			int si=0;
			while(!cstack.empty())	//get ny and nx in reverse order only if selector equals index
				{
				if(si==stackselector)
					ny=cstack.top();
				cstack.pop();
				if(si==stackselector)
					nx=cstack.top();
				cstack.pop();
				si++;
				}
			}
	#endif
	}

void RiverEroder::ClearCourse()
	{
	for_xy
		{
		course(x,y)=0;	//0 = unvisited,1=visited
		}
	}

void RiverEroder::MakeCourse(int x,int y)
	{
	//clear it first:
		ClearCourse();
	//make a new course:
		int nx,ny;
		nx=x;	ny=y;
		bool done=false;
		while(!done)
			{
			ChoosePathFromHere(nx,ny,x,y);
			done=nx==x&&ny==y;
			done=done||!inrange(nx,ny);
			if(!done)	{	course(x,y)=1;	x=nx;	y=ny;	}
			}
	}

void RiverEroder::DisplayCourseOnScreen()
	{
	// draw a river diagram on screen, usually after it is completed:
		acquire_screen();
			for_xy
				{
				int color=(course(x,y)==0)?0x007f00:0x0000bf;
				putpixel(screen,x,300+y,color);
				}
		release_screen();
		rest(1000);
		while(!keypressed()&&!key[KEY_ESC]);
	}
