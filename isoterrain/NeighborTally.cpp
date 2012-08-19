#include "NeighborTally.h"

void NeighborTally::select(int&nx,int&ny,int x,int y,int neighbordist)
	{
	int totalv=total();
	if(totalv==0)
		{	nx=x;	ny=x;	return;	}
	int selectval=rand()%(totalv+1);
	////short c,n,e,w,s,nw,ne,se,sw;
	if(selectval>=c+n+e+w+s+nw+ne+se)	//sw
		{		nx=x-neighbordist;	ny=y+neighbordist;		}
	else if(selectval>=c+n+e+w+s+nw+ne)	//se
		{		nx=x+neighbordist;	ny=y+neighbordist;		}
	else if(selectval>=c+n+e+w+s+nw)	//ne
		{		nx=x+neighbordist;	ny=y-neighbordist;		}
	else if(selectval>=c+n+e+w+s)	//nw
		{		nx=x-neighbordist;	ny=y-neighbordist;		}
	else if(selectval>=c+n+e+w)	//s
		{		nx=x;	ny=y+neighbordist;		}
	else if(selectval>=c+n+e)	//w
		{		nx=x-neighbordist;	ny=y;		}
	else if(selectval>=c+n)	//e
		{		nx=x+neighbordist;	ny=y;		}
	else if(selectval>=c)	//n
		{		nx=x;	ny=y-neighbordist;		}
	else
		{		nx=x;	ny=y;		}
	}
