#include <cstdlib>

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
