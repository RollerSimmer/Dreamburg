#pragma once

class intvector3d
	{
	protected:
		int x,y,z;
	public:
		inline void SetXYZ(int _x,int _y,int _z)	{x=_x;y=_y;z=_z;}
		void SetX(int _x)	{x=_x;}
		void SetY(int _y)	{y=_y;}
		void SetZ(int _z)	{z=_z;}
		int GetX()	{return x;}
		int GetY()	{return y;}
		int GetZ()	{return z;}
		intvector3d(int _x,int _y,int _z)	:x(_x),y(_y),z(_z)	{}
		intvector3d(int d)	:x(d),y(d),z(d)	{}
		intvector3d();
		intvector3d operator+=(intvector3d v);
		intvector3d operator-=(intvector3d v);
		intvector3d operator/=(int a);
		intvector3d operator*=(int a);
		intvector3d operator+(intvector3d v);
		intvector3d operator-(intvector3d v);
		intvector3d operator/(int a);
		intvector3d operator*(int a);
		intvector3d operator-();	//unary negative
		int operator*(intvector3d&v);
		int dot(intvector3d&v);
		int cross(intvector3d&v);
		bool operator==(intvector3d v);
		bool operator!=(intvector3d v);
		int dist(intvector3d v);
		int dist(int x2,int y2,int z2);
		int mag();
		unsigned long isqrt(unsigned long n);
	};
