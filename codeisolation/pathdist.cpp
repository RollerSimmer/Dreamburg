/********************************************************************
**          City Game Problem Isolation - Distance finding         **
********************************************************************/

#include <iostream>
//#include <allegro.h>
#include <cstdlib>
#include <cmath>

using namespace std;

template <class A,class B>	//	A=dim type, B=square type
struct intvector2d
	{
	A x,y;
	intvector2d(A x,A y)		{	Set(x,y);	}
	void Set(A sx,A sy)	{	x=sx;	y=sy;	}
	static int intsqrt(B n)
		{
		int biti=sizeof(B)/2*8-1;
		bool found=false;
		B guessbase=(B)0;	B guessSqrd;	B guessbit=(B)1<<biti;
		B guess;
		while(guessbit>n)	guessbit>>=1;

		////cout<<"n="<<n<<" guessbase="<<guessbase<<" guess="<<guess<<" guessbit="<<guessbit<<endl;
		while(!found)
			{
			B guess=guessbase|guessbit;
			guessSqrd=guess*guess;
			found=n==guessSqrd||!guessbit;
			if(found)	return guess;
			else if(n>guessSqrd)
				guessbase=guess;
			else	////if(n<=guessSqrd)
				;
			////cout<<"n="<<n<<" guessbase="<<guessbase<<" guess="<<guess<<" guessbit="<<guessbit<<" guessSqrd="<<guessSqrd<<endl;
			guessbit>>=1;
			}
		return guess;
		}
	A intmag(A x,A y)
		{	return intsqrt(x*x+y*y);	}
	};
typedef intvector2d<long,unsigned long> longvector2d;

int main()
	{
	cout<<"********************************"<<endl;
	cout<<"**       Path distance        **"<<endl;
	cout<<"********************************"<<endl;
	srand(time(0));

	//test shift right;
	long n=0x1;
	cout<<n<<endl;
	for (int i=0;i<64;i++)
		{	n=n<<=1;	cout<<n<<endl;	}

	//test int OR float
	const int dimsize=1000;
	const int nsquares=dimsize*dimsize;
	unsigned long tstart,tend,ttaken;
	tstart=clock();
	int i=0;
	for(int y=0;y<dimsize;y++)
	for(int x=0;x<dimsize;x++)
		{
		const unsigned long base=200000;
		////long n=rand()%base;
		////long x=rand()%base,	y=rand()%base;
		unsigned long crowlen=longvector2d::intsqrt(x*x+y*y);
		unsigned long arclen=157*crowlen/100;
		unsigned long avglen=(crowlen+arclen)/2;
		//float froot=sqrt(n);
		if	(	  (x%(dimsize/10)==0||x==dimsize-1)
				&&(y%(dimsize/10)==0||y==dimsize-1)		)
		   cout<<"i="<<i<<" x="<<x<<" y="<<y<<" lens: crow="<<crowlen<<" arc="<<arclen<<" avg="<<avglen<<endl
			;
		++i;
		}
	tend=clock();
	ttaken=(tend-tstart)/(CLOCKS_PER_SEC/1000);
	cout<<"square root took "<<ttaken<<"ms"<<endl;
	}
