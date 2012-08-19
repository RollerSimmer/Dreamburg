#include "IsoTerrain.h"
#include "TerrainEroder.h"
#include "RiverEroder.h"
#include "../dendrite/DendriteDrawer.h"
#include <algorithm>
#include "../intvector2d/intvector2d.h"
#include <iostream>
#include <cmath>

using namespace std;

#define formap(_x,_y,_w,_l) for(int y=_y;y<_l;y++) for(int x=_x;x<_w;x++)

const int iary[8]={+1,+1, 0,-1,-1,-1, 0,+1};
const int jary[8]={ 0,-1,-1,-1, 0,+1,+1,+1};

IsoTerrain::IsoTerrain(IsoTerrain*base)
	{
	*this=*base;
	}

void IsoTerrain::fixxy(int&x,int&y)
	{	x=xfix(x);	y=yfix(y);	}

int IsoTerrain::xfix(int x)
	{	return max(0,(int)min((int)x,(int)mapwidth-1));	}
int IsoTerrain::yfix(int y)
	{	return max(0,(int)min((int)y,(int)maplength-1));	}


u16 &IsoTerrain::ht(int x,int y)
	{
	if(!inrange(x,y))
		{
		safetyht=ht(xfix(x),yfix(y));
		return safetyht;
		}
	return hts[y*mapwidth+x];
	}

u16 IsoTerrain::maxht()
	{
	u16 M=ht(0,0);
	for(int y=0;y<maplength;y++)
	for(int x=0;x<mapwidth;x++)
		{	M=max(M,ht(x,y));	}
	return M;
	}

u16 &IsoTerrain::rockht(int x,int y)
	{
	if(x<0||y<0)	return safetyrockht;
	if(x>=mapwidth||y>=maplength)	return safetyrockht;
	return rockhts[y*mapwidth+x];
	}

IsoTerrain::IsoTerrain()
		:	zcap(500)
	{
	for(int y=0;y<maplength;y++)
	for(int x=0;x<mapwidth;x++)
		{
		ht(x,y)=rockht(x,y)=0;
		}
	}

void IsoTerrain::GenRandHts(int zcap)
	{
	for(int y=0;y<maplength;y++)
	for(int x=0;x<mapwidth;x++)
		{
		ht(x,y)=rand()%(zcap+1);
		rockht(x,y)=rand()%(ht(x,y)+1);
		}
	}

void IsoTerrain::ScatterInc(int htinc)
	{
	int x,y,curhtinc,r;
	int npoints=mapwidth*maplength/20;
	for(int p=npoints;p>0;p--)
		{
		curhtinc=rand()%(htinc+1);
		x=rand()%mapwidth;
		y=rand()%maplength;
		r=curhtinc;
		RaiseOctagon(x,y,r,curhtinc);
		}
	}

void IsoTerrain::MakeRugged(int radius)
	{
	int x,y,curhtinc,r;
	int npoints=mapwidth*maplength/20;
	for(int p=npoints;p>0;p--)
		{
		x=rand()%mapwidth;
		y=rand()%maplength;
		r=rand()%(radius+1);
		RuggedOctagon(x,y,r);
		}
	}

void IsoTerrain::RaiseOctagon(int cx,int cy,int r,int htinc)
	{
	int i,j,x,yt,yb,t;
	//-45..45
	for(t=0,i=r,j=0	;	i>j ; t++,j++,i-=((t%3==0)?1:0))
		{
		int a=0;	//dummy break line
		for(x=cx+i,yt=cy-j,yb=cy+j;x>=cx-i;x--)
			{
			RaiseVertex(x,yt,htinc);
			if(yb!=yt)	RaiseVertex(x,yb,htinc);
			}
		}
	//45..90,-45..-90
	for(	/*no inits*/	;	i>=0	;	t++,j+=1,i-=3)
		{
		int a=0;	//dummy break line
		for(x=cx+i,yt=cy-j,yb=cy+j;x>=cx-i;x--)
			{
			RaiseVertex(x,yt,htinc);
			if(yb!=yt)	RaiseVertex(x,yb,htinc);
			}
		}
	}

void IsoTerrain::RuggedOctagon(int cx,int cy,int r)
	{
	int i,j,x,yt,yb,t;
	//-45..45
	for(t=0,i=r,j=0	;	i>j ; t++,j++,i-=((t%3==0)?1:0))
		{
		int a=0;	//dummy break line
		for(x=cx+i,yt=cy-j,yb=cy+j;x>=cx-i;x--)
			{
			RuggedVertex(x,yt);
			if(yb!=yt)	RuggedVertex(x,yb);
			}
		}
	//45..90,-45..-90
	for(	/*no inits*/	;	i>=0	;	t++,j+=1,i-=3)
		{
		int a=0;	//dummy break line
		for(x=cx+i,yt=cy-j,yb=cy+j;x>=cx-i;x--)
			{
			RuggedVertex(x,yt);
			if(yb!=yt)	RuggedVertex(x,yb);
			}
		}
	}

void IsoTerrain::RaiseVertex(int x,int y,int htinc)
	{
	if(htinc>0)
		htinc=min((int)zcap-ht(x,y),htinc);
	else
		htinc=max(-ht(x,y),htinc);
	ht(x,y)+=htinc;
	int newrockht=min((int)rockht(x,y)+htinc,(int)ht(x,y));
	newrockht=max(newrockht,0);
	rockht(x,y)=newrockht;
	}

void IsoTerrain::RuggedVertex(int x,int y)
	{
	int htary[8];
	int hthi,htlo;
	hthi=htlo=ht(x,y);
	for(int a=0;a<8;a++)
		{
		htary[a]=ht(x+iary[a],y+jary[a]);
		hthi=max(hthi,htary[a]);
		htlo=min(htlo,htary[a]);
		}
	int htspan=abs(hthi-htlo)*1/3;
	int htmid=(htlo+hthi)/2;
	int htnew=rand()%(htspan+1)+htmid-htspan/2;
	RaiseVertex(x,y,htnew-ht(x,y));
	}


void IsoTerrain::FlattenAll(int level)
	{
	for(int y=0;y<maplength;y++)
	for(int x=0;x<mapwidth;x++)
		{
		ht(x,y)=level;
		rockht(x,y)=level/2;
		}
	}

void IsoTerrain::Smooth(int x,int y,int radius)
	{
	const int maxradius=50;
	static int wtsum=0;
	const int arysz=maxradius*2+1;
	const int arymid=arysz/2;
	static u16 weights[arysz];
	intvector2d c(arymid,arymid);

	static bool wtinit=false;
	if(!wtinit)
		{
		for(int t=0;t<arysz;t++)
			{
			int dist=abs(t-arymid);
			int curwt=1000/(1+dist*dist);
			weights[t]=curwt;
			////cout<<"weights["<<t<<"]="<<weights[t]<<";"<<endl;
			}
		wtinit=true;
		}

	radius=min(radius,maxradius);
	int rowsz=radius*2+1;
	int rowmid=rowsz-radius-1;
	u32 row[rowsz];
	u32 rockrow[rowsz];

	//set the safety tile is set to current tile's height;
		safetyht=ht(x,y);
		safetyrockht=rockht(x,y);

	////cout<<"wtsum="<<wtsum<<";"<<endl;
	int curht=0;
	int currockht=0;
	for(int i=-radius;i<radius+1;i++)
		{
		row[rowmid+i]=0;
		rockrow[rowmid+i]=0;
		wtsum=0;
		for(int j=-radius;j<radius+1;j++)
			{
			row[rowmid+i]+=ht(x+i,y+j)*weights[arymid+j];
			rockrow[rowmid+i]+=rockht(x+i,y+j)*weights[arymid+j];
			wtsum+=weights[arymid+j];
			}
		wtsum=max(wtsum,1);

		row[rowmid+i]/=wtsum;
		rockrow[rowmid+i]/=wtsum;
		curht+=row[rowmid+i]*weights[arymid+i];
		currockht+=rockrow[rowmid+i]*weights[arymid+i];
		////cout<<"row["<<(arymid+i)<<"]="<<row[rowmid+i]<<";"<<endl;
		}
	////cout<<"curht(sum)="<<curht<<";"<<endl;
	curht/=wtsum;
	currockht/=wtsum;
	////cout<<"curht="<<curht<<";"<<endl;
	////cout<<"************************************************************"<<endl;
	ht(x,y)=curht;	//return the current height after the matrix is applied
	rockht(x,y)=currockht;
	}

void IsoTerrain::SmoothAll(int radius)
	{
	for(int y=0;y<maplength;y++)
	for(int x=0;x<mapwidth;x++)
		{
		Smooth(x,y,radius);
		}
	}

void IsoTerrain::Erode(int ncycles,int dropsPerRound,int radius,int digdepth,int smoothradius)
	{
	TerrainEroder*te;
	te=new TerrainEroder(TerrainEroder(this,mapwidth,maplength,dropsPerRound,radius,digdepth,smoothradius));
	te->Erode(ncycles);
	delete te;
	}

//IsoTerrain primitives

	void IsoTerrain::Pyramid(int h)
		{
		for(int y=0;y<maplength;y++)
		for(int x=0;x<mapwidth;x++)
			{
			int d=mapsz/2-max(abs(x-mapwidth/2),abs(y-maplength/2));
			ht(x,y)=2*d*h/mapsz;
			rockht(x,y)=ht(x,y)*2/3;
			}
		}

	void IsoTerrain::Invert()
		{
		//get max and min ht
			int htmax=ht(0,0),htmin=htmax;	//to ensure it is some height
			for(int y=0;y<maplength;y++)
			for(int x=0;x<mapwidth;x++)
				{
				htmax=max((int)ht(x,y),htmax);
				htmin=min((int)ht(x,y),htmin);
				}
		//invert the terrain:
			int span;
			for(int y=0;y<maplength;y++)
			for(int x=0;x<mapwidth;x++)
				{
				//make the current vertex the same distance from the max as it was from min:
					span=ht(x,y)-htmin;
					ht(x,y)=htmax-span;
				}
		}

	void IsoTerrain::Dome(int h)
		{
		for(int y=0;y<maplength;y++)
		for(int x=0;x<mapwidth;x++)
			{
			float r=(float)max(mapwidth,maplength)/2;
			float dx=((float)x-(float)r)/(float)r;
			float dy=((float)y-(float)r)/(float)r;
			float d=sqrt(dx*dx+dy*dy);
			d=min(d,(float)1.0);
			float hf=sqrt(1.0-d*d);
			hf*=h;	hf+=0.5;
			ht(x,y)=(int)hf;
			rockht(x,y)=ht(x,y)*2/3;
			}
		}

	void IsoTerrain::SineHill(int h)
		{
		int dir=rand()%2;
		int d1,d2;
		const float pi=3.1415927;
		float ang;
		int hsina;
		for(int d1=0;d1<mapwidth;d1++)
			{
			ang=(float)(d1*2)*pi/(float)mapwidth;
			ang-=pi/2.0;
			hsina=(int)((float)h/2*(sin(ang)+1.0));
			for(int d2=0;d2<maplength;d2++)
				{
				if(dir==0)			ht(d2,d1)=hsina;
				else if(dir==1)	ht(d1,d2)=hsina;
				}
			}
		}

	void IsoTerrain::SineHillMesh(int h,int xperiods,int yperiods)
		{
		float angx,angy;
		const float pi=3.1415927;
		int hsinax,hsinay,hhi,havg;
		for(int y=0;y<maplength;y++)
			{
			angy=(float)(y*yperiods)/(float)mapwidth*pi;
			hsinay=(int)((float)h/2*(sin(angy)+1.0));
			for(int x=0;x<mapwidth;x++)
				{
				angx=(float)(x*xperiods)/(float)mapwidth*pi;
				hsinax=(int)((float)h/2*(sin(angx)+1.0));
				hhi=max(hsinax,hsinay);
				havg=(hsinax+hsinay)/2;
				ht(x,y)=hhi;
				////ht(x,y)=havg;
				}
			}
		}

	void IsoTerrain::SharpHill(int h)
		{
		int dir=rand()%2;
		int d1,d2;
		int htline;
		int mapsz=max(mapwidth,maplength);
		for(int d1=0;d1<mapwidth;d1++)
			{
			htline=h-abs(mapsz/2-d1)*2*h/mapsz;
			for(int d2=0;d2<maplength;d2++)
				{
				if(dir==0)			ht(d2,d1)=htline;
				else if(dir==1)	ht(d1,d2)=htline;
				}
			}
		}

	void IsoTerrain::PeaksAndValleysAt(int x,int y)
		{
		int m,M;
		m=M=ht(x,y);
		//get max and min:
			for(int j=-1;j<=1;j++)
			for(int i=-1;i<=1;i++)
				{
				int nx=x+i,	ny=y+j;
					if(nx>=0&&nx<mapwidth&&ny>=0&&ny<maplength)
						{
						M=max(M,(int)ht(nx,ny));
						m=min(m,(int)ht(nx,ny));
						}
				}
		//set height to closest bound; if tied, pick one randomly:
			static int mincount=0,maxcount=0,choosecount=0;
			int h=ht(x,y);
			if(abs(h-m)<abs(M-h))
				{	ht(x,y)=m;	/*mincount++;	cout<<"PV->min calls: "<<mincount<<endl;*/	}
			else if (abs(h-m)>abs(M-h))
				{	ht(x,y)=M;	/*maxcount++;	cout<<"PV->max calls: "<<maxcount<<endl;*/		}
			else
				ht(x,y)=ht(x,y);
				/*{	ht(x,y)=(rand()%2==0)?	M	:	m;	choosecount++;	cout<<"PV->choose calls: "<<choosecount<<endl;		*/
		//update rock height:
			rockht(x,y)=ht(x,y)*2/3;

		}

	void IsoTerrain::PeaksAndValleys()
		{
		//random points:
			const int maxpoints=mapwidth*maplength*16;
			for(int n=0;n<maxpoints;n++)
				{
				int x=rand()%mapwidth;
				int y=rand()%maplength;
				PeaksAndValleysAt(x,y);
				}
		//sequential:
			#if 0
				formap(0,0,mapwidth,maplength)
					{
					PeaksAndValleysAt(x,y);
					}
			#endif
		}

void IsoTerrain::RiverErode(int ntimes,int slope,int slopescale)
	{
	RiverEroder*re=new RiverEroder(this);

	int i=ntimes;
	int curslope=slope;
	while(i>0)
		{
		int x=rand()%mapwidth;
		int y=rand()%maplength;

		re->Erode(x,y,curslope,slopescale);
		////curslope=i*slope/ntimes;
		curslope=slope;
		////re->MakeCourse(x,y);
		////re->DisplayCourseOnScreen();

		--i;
		}

	delete re;
	}

struct point
	{
	int x,y,dist;
	point()	{x=y=0;}
	point(int _x,int _y)	{	x=_x;	y=_y;	}
	void DistFromTarget(int tx,int ty) {	dist=intvector2d::dist(x,y,tx,ty);	}
	static bool sort_closest(point p1,point p2)	{	return p1.dist<p2.dist;	}
	};

void IsoTerrain::Voronoi(int npoints,int c1,int c2,int c3)
	{

	int x,y;

	vector<point> ptary(npoints);

	for(int i=0;i<npoints;i++)
		{
		x=rand()%mapwidth;
		y=rand()%maplength;
		ptary[i]=point(x,y);
		}

	for(y=0;y<maplength;y++)
	for(x=0;x<mapwidth;x++)
		{
		for(int i=0;i<npoints;i++)
			{
			ptary[i].DistFromTarget(x,y);
			}
		//sort ptary vector:
			sort(ptary.begin(),ptary.end(),point::sort_closest);
		//voronoi with the closest two points
			ht(x,y)=c1*ptary[0].dist+c2*ptary[1].dist+c3*ptary[2].dist;
		}

	while(!ptary.empty())
		ptary.pop_back();
	}

void IsoTerrain::RaiseAll(int zo)
	{
	for(int y=0;y<maplength;y++)
	for(int x=0;x<mapwidth;x++)
		{
		int z=ht(x,y);
		if(z<-zo)
			ht(x,y)=0;
		else if(z+zo>zcap)
			ht(x,y)=zcap;
		else
			ht(x,y)+=zo;
		}
	}

void IsoTerrain::Double()
	{
	for(int y=0;y<maplength;y++)
	for(int x=0;x<mapwidth;x++)
		ht(x,y)=ht(x,y)*2;
	}

void IsoTerrain::Halve()
	{
	for(int y=0;y<maplength;y++)
	for(int x=0;x<mapwidth;x++)
		ht(x,y)=ht(x,y)/2;
	}

void IsoTerrain::DendriteErode(int wedgewidth,int wedgeheight,int nseeds,int dropstep)
	{
	//create dendrite:
		Dendrite*d=new Dendrite(mapwidth,maplength,1);
		d->InitPoints();
		DendriteMapHeights(d);
	//fill dendrite map:
		const bool byheight=true;
		const bool doinitpts=false;
		d->FillMapWithBackTracing(byheight,false);
	#if 1
		//Draw dendrite map:
			DendriteDrawer*dd=new DendriteDrawer(d,1);
			dd->Draw(0,0);
			delete dd;
			rest(2000);
	#endif
	#if 1
		//trace paths and lower land by the amount of visits for each point:

			d->TracePaths(mapwidth*maplength*2);
			int mx,my,x1,y1,z1,x2,y2,z2,zx100;
			int curdepth;
			const int visitdepth=1;
			for(int i=0;i<nseeds;i++)
				{
				mx=rand()%d->getw();	my=rand()%d->getl();
				LatticePoint*lp;
				lp=d->pt(mx,my);
				if(lp->next!=0)
					{
					x1=lp->x;	y1=lp->y;
					////zx100=ht(x1,y1)*100;
					zx100=maxht()/2*100;
					z1=zx100/100;
					while(lp->next!=0&&lp->next!=d->GetExitPoint())
						{
						x1=lp->x;	y1=lp->y;	x2=lp->next->x;	y2=lp->next->y;
						if(zx100>dropstep)	zx100-=dropstep;	else	zx100=0;
						z2=max(zx100/100,0);

						//wedge cut:
							WedgeCut(x1,y1,z1,x2,y2,z2,wedgewidth,wedgeheight);

						z1=min(z2,(int)(ht(x2,y2)));
						////z1=z2;
						lp=lp->next;
						}
					}
				}
	#endif

	//delete dendrite:
		delete d;
	}

void IsoTerrain::DendriteMapHeights(Dendrite*d)
	{
	formap(0,0,mapwidth,maplength)
		{
		////if(d->pt(x,y)->z!=this->ht(x,y))
			d->pt(x,y)->z=this->ht(x,y);
		}
	}

void IsoTerrain::DendriteRidges(int wedgewidth,int wedgeheight,int nseeds,int dropstep)
	{
	//create dendrite:
		Dendrite*d=new Dendrite(mapwidth,maplength,1);
		d->InitPoints();
		DendriteMapHeights(d);
	//fill dendrite map:
		const bool byheight=false;
		const bool doinitpts=false;
		d->FillMapWithBackTracing(byheight,doinitpts);
	#if 1
		//Draw dendrite map:
			DendriteDrawer*dd=new DendriteDrawer(d,1);
			dd->Draw(0,0);
			delete dd;
			rest(3000);
	#endif
	#if 0
		//trace paths and lower land by the amount of visits for each point:
			d->TracePaths(mapwidth*maplength*2);
			int mx,my,x1,y1,z1,x2,y2,z2,zx100;
			int curdepth;
			const int visitdepth=1;
			for(int i=0;i<nseeds;i++)
				{
				mx=rand()%d->getw();	my=rand()%d->getl();
				LatticePoint*lp;
				lp=d->pt(mx,my);
				if(lp->next!=0)
					{
					x1=lp->x;	y1=lp->y;
					////zx100=ht(x1,y1)*100;
					zx100=maxht()/2*100;
					z1=zx100/100;
					while(lp->next!=0&&lp->next!=d->GetExitPoint())
						{
						x1=lp->x;	y1=lp->y;	x2=lp->next->x;	y2=lp->next->y;
						if(zx100+dropstep<zcap*100)	zx100+=dropstep;	else	zx100=0;
						z2=max(zx100/100,0);

						//wedge cut:
							Wedge(x1,y1,z1,x2,y2,z2,wedgewidth,wedgeheight);

						z1=min(z2,(int)(ht(x2,y2)));
						////z1=z2;
						lp=lp->next;
						}
					}
				}
	#else
		//make a pyramid at (x1,y1):
			//first trace the paths:
				d->TracePaths(mapwidth*maplength*2);
			//step through the map and draw pyramids at heights corresponding to route lengths:
				int xpyr,ypyr,zpyr,zbasex100,zpyrx100,wpyr,lpyr;
				LatticePoint*lp;
				IsoTerrain*prevter=new IsoTerrain(this);
				for(int iseed=0;iseed<100;iseed++)
					{
					x=rand()%mapwidth;
					y=rand()%maplength;

					lp=d->pt(x,y);
					zbasex100=prevter->ht(lp->x,lp->y)*100;

					while(lp!=d->GetExitPoint())
						{
						xpyr=lp->x;	ypyr=lp->y;
						if(dropstep>=0)
							zpyr=10;
						else
							zpyr=-10;

						if(dropstep>=0)
							MaxPyramid(xpyr,ypyr,zpyr,wpyr,lpyr);
						else
							MinPyramidPit(xpyr,ypyr,zpyr,wpyr,lpyr,zmax);

						zpyrx100+=dropstep;
						}



					}

				#if 0
					for(int y=0;y<maplength;y++)
					for(int x=0;x<mapwidth;x++)
						{
						lp=d->pt(x,y);
						xpyr=lp->x;	ypyr=lp->y;
						zpyr=lp->routelen;
							////zpyr+=rand()%11-5;
							zpyr=zpyr*dropstep/100;
						if(abs(zpyr)>0)
							wpyr=lpyr=abs(zpyr)/8;
						else
							wpyr=lpyr=5;
						int zmax;
						#if 0
							zmax=maxht();
						#else
							zmax=prevter->ht(xpyr,ypyr);
						#endif

						if(dropstep>=0)
							MaxPyramid(xpyr,ypyr,zpyr,wpyr,lpyr);
						else
							MinPyramidPit(xpyr,ypyr,zpyr,wpyr,lpyr,zmax);
						}
				#endif
				delete prevter;
	#endif

	//delete dendrite:
		delete d;
	}

void IsoTerrain::ScramblePyramids(int amt,int loht,int hiht,int loslope,int hislope)
	{
	if(loslope>hislope)
		{	ScramblePyramids(amt,loht,hiht,hislope,loslope);	return;	}
	else if(loht>hiht)
		{	ScramblePyramids(amt,hiht,loht,loslope,hislope);	return;	}
	int x,y,slopey,slopex,htinc;
	for(int i=0;i<amt;i++)
		{
		//raise a single pyramid:
			x=rand()%mapwidth;	y=rand()%maplength;
			slopey=rand()%(hislope-loslope+1)+loslope;
			slopex=rand()%(hislope-loslope+1)+loslope;
			htinc=rand()%(hiht-loht+1)+loht;
			RaisePyramid(x,y,htinc,slopey,slopex);
			cout<<"pyr htinc="<<htinc<<endl;
		}
	}


void IsoTerrain::RaisePyramid(int x,int y,int htinc,int slopey,int slopex)
	{
	int r=htinc*slopex/slopey;
	if(r<=0)
		{
		return;
		}
	else
		{
		for(int py=y-r;py<=y+r;py++)
		for(int px=x-r;px<=x+r;px++)
			{
			int dx,dy,R,curhtinc;
			dx=abs(x-px);	dy=abs(y-py);
			R=max(dx,dy);
			curhtinc=(r+1-R)*htinc/r;
			RaiseVertex(px,py,curhtinc);
			}
		}
	}

void IsoTerrain::Wedge( int x1,int y1,int z1
							  ,int x2,int y2,int z2
							  ,int width,int height  )
	{
	WedgeCut(x1,y1,z1,x2,y2,z2,width,-height);
	}

void IsoTerrain::WedgeCut( int x1,int y1,int z1
                          ,int x2,int y2,int z2
								  ,int width,int height  )
	{
	#if 0
		int dx=x2-x1,dy=y2-y1,dz=z2-z1
			,hi=max(abs(dx),abs(dy))
			,lo=min(abs(dx),abs(dy))
			,x,y,z,balance;
		x=x1;	y=y1;	z=z1;	balance=0;
		int scale=0;
		while(!(x==x2&&y==y2))
			{
			////continue here

			}
	#endif

	int l,w,hs,hw;
	l=dist2d(x1,y1,x2,y2);
	if(l==0)	return;	//no length- return
	w=width;
	hs=z2-z1;
	hw=height;
	int dxs,dys,dxw,dyw;
	dxs=abs(x2-x1);	dys=abs(y2-y1);
	dxw=dys*w/l;	dyw=dxs*w/l;
	int slopecount,pitchcount;
	slopecount=dxs+dys+1;
	pitchcount=dxw+dyw+1;

	intvector2d*slopesteps=new intvector2d[slopecount];
	intvector2d*pitchsteps=new intvector2d[pitchcount];
	int*zslopesteps=new int[slopecount];
	int*zpitchsteps=new int[pitchcount];

	//from here, set up the slope and pitch line steps.

		int balance,m,M;
		intvector2d incx,incy;
		intvector2d incminor,incmajor;

		//first the slope:
			if(x2>=x1)	incx.SetXY(+1,0);
			else			incx.SetXY(-1,0);
			if(y2>=y1)	incy.SetXY(0,+1);
			else			incy.SetXY(0,-1);

			if(dxs>dys)		{	incmajor=incx;	incminor=incy;	M=dxs;	m=dys;	}
			else				{	incmajor=incy;	incminor=incx;	M=dys;	m=dxs;	}

			balance=M;

			for(int i=0;i<slopecount;i++)
				{
				if(balance<=0)	{	balance+=M;	slopesteps[i]=incminor;	}
				else				{	balance-=m;	slopesteps[i]=incmajor;	}
				zslopesteps[i]=i*hs/slopecount;
				}

		//second the pitch:
			intvector2d incxleft90,incyleft90;
			incxleft90=-incx;		incyleft90=incy;

			if(dxw>dyw)		{	incmajor=incxleft90;	incminor=incyleft90;	M=dxw;	m=dyw;	}
			else				{	incmajor=incyleft90;	incminor=incxleft90;	M=dyw;	m=dxw;	}

			balance=M;

			for(int i=0;i<pitchcount;i++)
				{
				if(balance<=0)	{	balance+=M;	pitchsteps[i]=incminor;	}
				else				{	balance-=m;	pitchsteps[i]=incmajor;	}
				zpitchsteps[i]=i*hw/pitchcount;
				}

		//test draw lines on screen to see if it worked:
			#if 0
				acquire_screen();
			#endif
				int sxl,syl,sz;	//screen left
				int sxr,syr;	//screen right
				int exl,eyl,ez;	//edge left
				int exr,eyr;	//edge right

				#if 0
					rectfill(screen,0,0,mapwidth,maplength,0x000000);
				#endif

				//pitch lines:
					exl=x1;	eyl=y1;	ez=z1;
					exr=x1;	eyr=y1;
					for(int i=0;i<pitchcount;i++)
						{
						//slope line:
							sxr=exr;	syr=eyr;	sz=ez;
							sxl=exl;	syl=eyl;
							for(int i=0;i<slopecount;i++)
								{
								#if 0
									int color,level;
									level=min(sz,255);
									color=makecol(level,level,level);
									putpixel(screen,sxl,syl,color);
									putpixel(screen,sxr,syr,color);
								#endif

								if(height<0)
									{
									maxht(sxl,syl,sz);
									maxht(sxr,syr,sz);
									}
								else
									{
									minht(sxl,syl,sz);
									minht(sxr,syr,sz);
									}

								sxl+=slopesteps[i].GetX();
								syl+=slopesteps[i].GetY();
								sxr+=slopesteps[i].GetX();
								syr+=slopesteps[i].GetY();
								sz+=zslopesteps[i];
								}

						exl+=pitchsteps[i].GetX();
						eyl+=pitchsteps[i].GetY();
						exr-=pitchsteps[i].GetX();
						eyr-=pitchsteps[i].GetY();
						ez+=zpitchsteps[i];
						}

			#if 0
				release_screen();
				rest(2000);
			#endif

	delete [] zpitchsteps;
	delete [] zslopesteps;
	delete [] pitchsteps;
	delete [] slopesteps;
	}

int IsoTerrain::minht(int x,int y,int z)
	{
	z=max(z,0);
	if(inrange(x,y))
		ht(x,y)=min((int)(ht(x,y)),z);
	}

int IsoTerrain::maxht(int x,int y,int z)
	{
	z=min(z,(int)zcap);
	if(inrange(x,y))
		ht(x,y)=max((int)(ht(x,y)),z);
	}

int IsoTerrain::dist2d(int x1,int y1,int x2,int y2)
	{
	int dx,dy,m,M,retv;
	dx=abs(x2-x1);	dy=abs(y2-y1);
	m=min(dx,dy);	M=max(dx,dy);
	retv=M+m*3/10;
	return retv;
	}

void IsoTerrain::MaxPyramid(int x,int y,int z,int w,int l)
	{
	int r=max(abs(w),abs(l));
	if(w<=0||l<=0)
		{
		return;
		}
	else
		{
		for(int py=y-l/2;py<=y+l/2;py++)
		for(int px=x-w/2;px<=x+w/2;px++)
			{
			int dx,dy,R,curz;
			dx=abs(x-px);	dy=abs(y-py);
			R=max(dx,dy);
			curz=(r+1-R)*z/r;
			maxht(px,py,curz);
			}
		}
	}

void IsoTerrain::MinPyramidPit(int x,int y,int z,int w,int l,int top)
	{
	++w;++l;
	int r;
	r=max(abs(w/2),abs(l/2));	r=max(r,1);
	if(w<=0||l<=0)
		{
		return;
		}
	else
		{
		for(int py=y-l/2;py<=y+l/2;py++)
		for(int px=x-w/2;px<=x+w/2;px++)
			{
			int dx,dy,R,curz;
			dx=abs(x-px);	dy=abs(y-py);
			R=max(dx,dy);
			curz=(r+1-R)*z/r+top;
			minht(px,py,curz);
			}
		}
	}
