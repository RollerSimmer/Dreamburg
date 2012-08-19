#include "RiverEroder.h"
#include <stack>
#include <algorithm>
#include <allegro.h>
#include <list>
#include <iostream>

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

u16&RiverEroder::waterlevel(int x,int y)
	{
	if(inrange(x,y))
		return waterleveldata[y*mapwidth+x];
	else
		{
		fixxy(x,y);
		waterlevelsafe=waterlevel(x,y);
		return waterlevelsafe;
		}
	}

bool RiverEroder::haswater(int x,int y)
	{
	if(!inrange(x,y))
		return false;
	else
		return waterlevel(x,y)>ht(x,y);
	}

void RiverEroder::Erode(int x,int y,int _slope,int _slopescale)
	{
	//init data members
		slope=_slope;
		slopescale=_slopescale;
		riverlevel=ter->ht(x,y)*slopescale;
	//first chart the course:
		MakeCourse(x,y);

	#if 0
		DisplayCourseOnScreen();
	#endif

	//drop course using a for loop over whole map:
		int cx=x,	cy=y;
		bool done=false;
		while(!done)
			{
			//drop if on course, otherwise leave loop:
				if(course(cx,cy))
					{
					Drop(cx,cy);
					course(cx,cy)=0;
					}
				else	break;
			//find neighbor along course:
				int nx,ny;
				done=!inrange(cx,cy);
				if(!done)
					{
					for(int a=0;a<8;a++)
						{
						nx=cx+iary[a];	ny=cy+jary[a];
						if(course(nx,ny))
							{
							cx=nx;	cy=ny;
							done=false;
							break;
							}
						}
					}
			}
	}

void RiverEroder::Drop(	int x,int y	)
	{
	//drop the height along a sloped grade using slope and slopescale
		int landht;
		landht=ht(x,y);
		//waterfall check
			riverlevel=min(riverlevel,landht*slopescale);
		//drop the river level by slope's value, keeping it above sea level (zero);
			riverlevel-=slope;
			riverlevel=max(riverlevel,0);
		int riverht=riverlevel/slopescale;
		int digdepth=max((int)landht-riverht,0);
		ter->RaiseVertex(x,y,-digdepth);
		//afterwards, smooth the area:
			////int smoothradius=max(digdepth/10,1);
			////ter->Smooth(x,y,smoothradius);
			int smoothrad=max(digdepth/20,1);	//smooth radius
			smoothrad=min(smoothrad,4);
			smoothrad=rand()%smoothrad+1;

			for(int i=-smoothrad;i<=smoothrad;i++)
			for(int j=-smoothrad;j<=smoothrad;j++)
				ter->Smooth(x+i,y+j,1);
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
		long httop=ht(x,y)*10;
		for(int a=0;a<8;a++)	//step through angles
			{
			xary[a]=x+iary[a];
			yary[a]=y+jary[a];
			courseary[a]=course(xary[a],yary[a]);
			httop=max(httop,(long)ht(xary[a],yary[a])*scaleary[a]);
			}
		long htbtm=ht(x,y)*10;
		long htsum=ht(x,y)*10;
		for(int a=0;a<8;a++)	//step through angles
			{
			if(!inrange(xary[a],yary[a]))
				nhts[a]=0;
			else if(courseary[a]==1)
				nhts[a]=httop;
			else
				nhts[a]=ht(xary[a],yary[a])*scaleary[a];

			htsum+=nhts[a];

			htbtm=min(nhts[a],(int)htbtm);
			}
		long htavg=htsum/9;

		/*	give weight only to those neighbors whose difference from min ht is
		 *	within 3% of top-btm, plus an additional 2	*/
			int totspan=httop-htbtm;
			int maxspan=totspan*3/100;	//3% of max span+2 - lowest-counted span
			int curspan;
			//get average angle if angle list has been initialized
				int a45;


			for(int a=0;a<8;a++)
				{
				curspan=nhts[a]-htbtm;
				bool inspan=  (curspan<=maxspan||curspan==0)
								&&!course(xary[a],yary[a]);
				nwgts[a]=(inspan)?	httop-nhts[a]:	0;
				//try to steer in direction of previous path if htavg is closer to top than btm
					if(courselen>1&&httop-htavg<htavg-htbtm)
						{
						if(a==angle45(0,0,xoavg,yoavg))
							nwgts[a]=2*(httop-htbtm)/10;
						}

				wgtsum+=nwgts[a];

				////if(nhts[a]==htbtm)
				////	nwgts[a]*=2;	//bonus pull for being the minimum.
				}
		//if

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
				if(nhts[a]==htbtm&&courseary[a]==0)	//minimum and unvisited vertex
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

void RiverEroder::DrainWater()
	{
	for_xy
		{
		waterlevel(x,y)=0;	//0 = unvisited,1=visited
		}
	}

void RiverEroder::LowerWaterLevels(int amt)
	{
	for_xy
		{
		if(waterlevel(x,y)>0)
			--waterlevel(x,y);
		}
	}

template <class T>
	T minmax(T a,T m,T M)
		{	return (min((int)(M),max((int)(m),(int)(a))));	}

int RiverEroder::angle45(int x1,int y1,int x2,int y2)
	{
	int xo,yo,a,xy;
	xo=minmax(x2-x1,-1,1);	yo=minmax(y2-y1,-1,1);
	xy=(xo+2)*10+(yo+2);
	switch(xy)
		{
		case 22:	return 0/45;	//(0,0) has an undefined angle
		case 32:	return 0/45;	//(1,0)
		case 33:	return 45/45;	//(1,1)
		case 23:	return 90/45;	//...
		case 13:	return 135/45;	//...
		case 12:	return 180/45;	//...
		case 11:	return 225/45;	//...
		case 21:	return 270/45;	//...
		case 31:	return 270/45;	//(1,-1)
		default:	return 0/45;
		}
	}


void RiverEroder::MakeCourse(int x,int y)
	{
	//clear it first:
		ClearCourse();
	//make a new course:
		courselen=0;
		int curangle;
		int nx,ny;
		nx=x;	ny=y;
		bool done=false;
		while(!done)
			{
			ChoosePathFromHere(nx,ny,x,y);
			int cxo=nx-x,cyo=ny-y;
			if(courselen==0)
				{
				for(int ai=0;ai<nlastangles;ai++)
					{
					xoary[ai]=cxo;
					yoary[ai]=cyo;
					}
				}
			else
				{
				int ai=courselen%nlastangles;
				xoary[ai]=cxo;
				yoary[ai]=cyo;
				}

			++courselen;
			done=nx==x&&ny==y;
			done=done||!inrange(nx,ny);
			if(!done)	{	course(x,y)=1;	x=nx;	y=ny;	}
			}
	LastVectorsAvg();
	int angleavg=angle45(0,0,xoavg,yoavg);
	cout<<"courselen="<<courselen<<"; angleavg="<<angleavg<<";"<<endl;
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

void RiverEroder::FloodExitPoint(int &nx,int&ny,int x,int y)
	{
	/*note: be sure to fix the other neighbor drop function to only move to lower points.
			  this will cover other cases	*/


	stack<short> fstack;	//flood stack
	int nhts[8];
	int wl=ht(x,y)+1;//flood water level
	int cx,cy,cnx,cny;
	const int waterlevellimit=ter->maxht()+1;

	//keep increasing flood level until limit is reached:
		bool limitreached=false;
		while(!limitreached)
			{
			DrainWater();
			while(!fstack.empty())
				fstack.pop();
			fstack.push(x);	fstack.push(y);
			if(wl>=waterlevellimit)
				limitreached=true;
			else
				limitreached=false;
			//flood at level
				while (!fstack.empty())
					{
					cy=fstack.top();	fstack.pop();
					cx=fstack.top();	fstack.pop();
					waterlevel(cx,cy)=wl;
					for(int a=0;a<8;a++)
						{
						cnx=cx+iary[a];	cny=cy+jary[a];
						if(inrange(cnx,cny))
							{
							if(ht(cnx,cny)<wl&&!haswater(cnx,cny))
								{
								//push this point
								fstack.push(cnx);	fstack.push(cny);
								}
							}
						else	//spills off the map:
							{
							//clean stack and lower flood level by one:
								////while(!fstack.empty())
								////	fstack.pop();
								////--wl;
								////LowerWaterLevels(1);
								limitreached=true;
								//jump out of loop:
								////	break;
							}
						}
					}
			if(!limitreached)
				++wl;
			} //end of while(!limitreached)
	#define RiverEroder_FloodExitPoint_debug true
	#if RiverEroder_FloodExitPoint_debug
		readkey();
		acquire_screen();
			for(int dy=0;dy<maplength;dy++)
			for(int dx=0;dx<mapwidth;dx++)
				{
				if(haswater(dx,dy))
					putpixel(screen,dx,dy,0x0000ff);
				else
					putpixel(screen,dx,dy,0x00ff00);
				}
		release_screen();
		readkey();
	#endif
	return;
	}


int RiverEroder::LastAnglesAvg()
	{
	int asum=0,aavg;
	if(nlastangles==0)		return 0;
	for(int ai=0;ai<nlastangles;ai++)
	////	asum+=lastangles[ai];
	aavg=asum/nlastangles;
	aavg=min(aavg,7);
	return aavg;
	}

void RiverEroder::LastVectorsAvg()
	{
	int xosum=0,yosum=0;
	for(int ai=0;ai<nlastangles;ai++)
		{	xosum+=xoary[ai];	yosum+=yoary[ai];	}
	int M=max(abs(xosum),abs(yosum));
	M=max(M,1);
	int xoavgabs=(abs(xosum)+M/2)/M;
	int yoavgabs=(abs(yosum)+M/2)/M;
	xoavg=(xosum>0)?xoavgabs:-xoavgabs;
	yoavg=(yosum>0)?yoavgabs:-yoavgabs;
	++xoavg;
	--xoavg;

	}
