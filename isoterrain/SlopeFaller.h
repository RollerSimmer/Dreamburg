#pragma once

#include "IsoTerrain.h"
#include "TerrainEroder.h"

class SlopeFaller
	{
	protected:
		IsoTerrain*ter;
		TerrainEroder*eroder;
		int x,y,radius;
		int wgthoodhts[3][3];
		int &curht;
		int hooddists[3][3];
		bool visitedflags[3][3];
	public:
		bool&visited(int i,int j)	{	return visitedflags[j+1][i+1];	}
		int&wgthoodht(int i,int j)	{	return wgthoodhts[j+1][i+1];	}
		int&hooddist(int i,int j)	{	return hooddists[j+1][i+1];	}
		SlopeFaller(IsoTerrain*_ter,TerrainEroder*_eroder,int _x,int _y,int _radius);
		void PickNeighbor(int&rx,int&ry);
		void ResetHood(int _x,int _y);
	};
