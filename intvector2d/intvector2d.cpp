#include "../intvector2d/intvector2d.h"
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <iostream>

using namespace std;

void intvector2d::SetX(int newx)
	{	x=newx;	}

void intvector2d::SetY(int newy)
	{	y=newy;	}

int intvector2d::GetX()
	{	return x;	}

int intvector2d::GetY()
	{	return y;	}

intvector2d intvector2d::operator+(intvector2d v)
	{
	intvector2d newv;
	newv.SetX(x+v.GetX());
	newv.SetY(y+v.GetY());
	return newv;
	}

intvector2d intvector2d::operator-(intvector2d v)
	{
	intvector2d newv;
	newv.SetX(x-v.GetX());
	newv.SetY(y-v.GetY());
	return newv;
	}

int intvector2d::operator*(intvector2d&v)
	{
	return dot(v);
	}

intvector2d intvector2d::operator-()
	{
	intvector2d neg=*this;
	neg.SetX(-neg.GetX());
	neg.SetY(-neg.GetY());
	return neg;
	}

int intvector2d::dot(intvector2d&v)
	{
	return x*v.GetX()+y*v.GetY();
	}

int intvector2d::cross(intvector2d&v)
	{
	return x*v.GetY()-y*v.GetX();
	}

intvector2d intvector2d::operator/(int a)
	{
	intvector2d newv;
	newv.SetX(x/a);
	newv.SetY(y/a);
	return newv;
	}

intvector2d intvector2d::operator*(int a)
	{
	intvector2d newv;
	newv.SetX(x*a);
	newv.SetY(y*a);
	return newv;
	}

intvector2d::intvector2d(int firstx,int firsty)
	{
	x=firstx;
	y=firsty;
	}

intvector2d::intvector2d()
	{	x=y=0;	}

bool intvector2d::operator==(intvector2d&v)
	{
	return x==v.GetX()&&y==v.GetY();
	}

bool intvector2d::operator!=(intvector2d&v)
	{
	return!(*this==v);
	}


int intvector2d::dist(intvector2d v)
	{
	int m=min(abs(x-v.GetX()),abs(y-v.GetY()));
	int M=max(abs(x-v.GetX()),abs(y-v.GetY()));
	return M+(m+1)/3;
	}

int intvector2d::dist(int x2,int y2)
	{	return dist(intvector2d(x2,y2));	}

int intvector2d::dist(int x1,int y1,int x2,int y2)
	{	intvector2d v1(x1,y1);	intvector2d v2(x2,y2);	return v1.dist(v2);	}

int intvector2d::mag()
	{	return dist(intvector2d(0,0));	}

intvector2d intvector2d::rcosandsina(int r,int a)
	{
	const int precisionpad=10;
	int rscale=r*precisionpad;
	int x,y,acap;
	int lo,hi;	//low and high dimensions
	enum	{xdim,ydim} hidim;
	a=a%360;		if(a<0)	a+=360;		//fix angle range
	//store signs, make a from 0 to 90, and choose min and max;
		int xsign, ysign;
		if(a>=0&&a<=180)	ysign=1;
		else					ysign=-1;
		if((a>=0&&a<=90)||(a>=270&&a<=360))  xsign=1;
		else	                               xsign=-1;
		if(a>=180)	a-=180;
		if(a>=90)	a=180-a;
		if(a>=45)
			{	hidim=ydim;	a=90-a;	}
		else
			{	hidim=xdim;	}	//don't change a
	// go counterclockwise from 0..45
		if(a>=0&&a<=45)
			{
			int aspan;
			hi=rscale;	lo=0;
			aspan=min(a,15)-0;
			hi+=rscale*(966-1000)*aspan/1000/15;
			lo+=rscale*(259-0)*aspan/1000/15;
			if(a>=15)
				{
				aspan=min(a,30)-15;
				hi+=rscale*(866-966)*aspan/1000/15;
				lo+=rscale*(500-259)*aspan/1000/15;
				if(a>=30)
					{
					aspan=min(a,45)-30;
					hi+=rscale*(707-866)*aspan/1000/15;
					lo+=rscale*(707-500)*aspan/1000/15;
					}
				}
			//find x and y from lo,hi,hidim,xsign and ysign;
				x=((hidim==xdim)?hi:lo)*xsign/precisionpad;
				y=((hidim==ydim)?hi:lo)*ysign/precisionpad;
			}
		else	//just return (r,0)
			{	x=r;		y=0;	}
	//convert to vector and return
		intvector2d retv(x,y);
		return retv;
	}

unsigned long intvector2d::isqrt(unsigned long n)
	{
	if(n==1||n==0)	return n;	//easy root, just return the argument
	unsigned long sr=max(n>>16,(unsigned long)1);	//first guess
	////cout<<"**********ISQRT("<<n<<")************"<<endl;
	////cout<<"Initial guess: "<<sr<<endl;
	const int maxguesses=30;
	int nguesses=1;
	unsigned long lastguess=n;
	bool done=false;
	while(!done)
		{
		sr=max((unsigned long)(sr+n/sr)>>1,(unsigned long)1);
		++nguesses;
		////cout<<"Guess "<<nguesses<<": "<<sr<<endl;
		done=sr*sr==n||sr==lastguess||nguesses>=maxguesses;
		if(done)	continue;
		lastguess=sr;
		}
	////cout<<"******************************"<<endl;
	return sr;
	}




