#include "intvector3d.h"
#include "../intvector2d/intvector2d.h"

#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <iostream>

using namespace std;


intvector3d intvector3d::operator+=(intvector3d v)
	{	x+=x;	y+=y; z+=z; return *this; }

intvector3d intvector3d::operator-=(intvector3d v)
	{	x-=x;	y-=y; z-=z; return *this; }

intvector3d intvector3d::operator*=(int a)
	{	x*=a;	y*=a; z*=a; return *this;	}

intvector3d intvector3d::operator/=(int a)
	{	x/=a;	y/=a; z/=a; return *this;	}

intvector3d intvector3d::operator+(intvector3d v)
	{	intvector3d ov=*this;	ov+=v;	return ov;	}

intvector3d intvector3d::operator-(intvector3d v)
	{	intvector3d ov=*this;	ov-=v;	return ov;	}

intvector3d intvector3d::operator*(int a)
	{	intvector3d ov=*this;	ov*=a;	return ov;	}

intvector3d intvector3d::operator/(int a)
	{	intvector3d ov=*this;	ov/=a;	return ov;	}

int intvector3d::operator*(intvector3d&v)
	{	return dot(v);	}

intvector3d intvector3d::operator-()
	{
	intvector3d neg=*this;
	neg.SetX(-neg.GetX());
	neg.SetY(-neg.GetY());
	neg.SetZ(-neg.GetZ());
	return neg;
	}

int intvector3d::dot(intvector3d&v)
	{
	return x*v.GetX()+y*v.GetY()+z*v.GetZ();
	}
int intvector3d::cross(intvector3d&v)
	{
	return x*v.GetY()+y*v.GetX();
	}

bool intvector3d::operator==(intvector3d v)
	{
	return x==v.GetX()&&y==v.GetY()&&z==v.GetZ();
	}

bool intvector3d::operator!=(intvector3d v)
	{
	return!(*this==v);
	}


int intvector3d::dist(intvector3d v)
	{
	intvector2d thisxy(x,y);
	intvector2d vxy(v.GetX(),v.GetY());
	int distxy=thisxy.dist(vxy);
	int m=min(abs(z-v.GetZ()),distxy);
	int M=max(abs(z-v.GetZ()),distxy);
	return M+(m+1)/3;
	}

int intvector3d::dist(int x2,int y2,int z2)
	{	return dist(intvector3d(x2,y2,z2));	}

int intvector3d::mag()
	{	return dist(intvector3d(0,0,0));	}

unsigned long intvector3d::isqrt(unsigned long n)
	{	return intvector2d::isqrt(n);	}




