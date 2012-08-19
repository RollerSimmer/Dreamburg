#pragma once
#include <types/mytypes.h>
#include <algorithm>

class NeighborTally
	{
	public:
		short c,n,e,w,s,nw,ne,se,sw;
		int total()	{	return	c+n+e+w+s+nw+ne+se+sw; }
		void select(int&nx,int&ny,int x,int y,int neighbordist);

	};
