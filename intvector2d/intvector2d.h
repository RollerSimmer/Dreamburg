#pragma once

class intvector2d
	{
	private:
		int x,y;
	public:
		inline void SetXY(int _x,int _y)	{x=_x;y=_y;}
		void SetX(int newx);
		void SetY(int newy);
		int GetX();
		int GetY();
		intvector2d(int firstx,int firsty);
		intvector2d();
		inline intvector2d operator+=(intvector2d v)	{	return *this=*this+v;	}
		inline intvector2d operator-=(intvector2d v)	{	return *this=*this-v;	}
		inline intvector2d operator/=(int a)	{	return *this=*this/a;	}
		inline intvector2d operator*=(int a)	{	return *this=*this*a;	}
		intvector2d operator+(intvector2d v);
		intvector2d operator-(intvector2d v);
		intvector2d operator/(int a);
		intvector2d operator*(int a);
		intvector2d operator-();	//unary negative
		int operator*(intvector2d&v);
		int dot(intvector2d&v);
		int cross(intvector2d&v);
		bool operator==(intvector2d&v);
		bool operator!=(intvector2d&v);
		int dist(intvector2d v);
		int dist(int x2,int y2);
		static int dist(int x1,int y1,int x2,int y2);
		int mag();
		static intvector2d rcosandsina(int r,int a);
		static unsigned long isqrt(unsigned long n);
	};


