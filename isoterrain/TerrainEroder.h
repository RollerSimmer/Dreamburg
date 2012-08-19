#pragma once

#include "IsoTerrain.h"
#include "intvector3d.h"
#include <types/mytypes.h>
#include <vector>
#include "NeighborTally.h"

using namespace std;

const int maxpathlen=255;

class TerrainEroder
	{
	protected:
		int w,l;	//width and length
		IsoTerrain*ter;
		vector<intvector3d> flows;
		intvector3d safetyflow;
		vector<int> htrecalls;
		int safetyhtrecall;
		int dropsPerRound;
		int radius;
		int digdepth;
		int smoothradius;
		vector<long>visitedflags;
		vector<NeighborTally> slidetallies;
		NeighborTally safetySlideTally;
		bool safetyvisited;
	public:
		TerrainEroder(IsoTerrain*_ter,int _w,int _l,int _dropsPerRound
		              ,int _radius,int _digdepth,int _smoothradius);
		~TerrainEroder()	{	}
		int GetWidth()		{return w;}
		int GetLength()	{return l;}
		intvector3d&flow(int x,int y);
		int&htrecall(int x,int y);
		bool StillUnvisitedNeighborsLeft(int x,int y);
			bool&visited(int x,int y);
		NeighborTally&slidetally(int x,int y);
		bool OnMap(int x,int y);
		int edgex(int x);
		int edgey(int y);
		void Erode(int ncycles);
			void RoundSetup();
				void ResetFlowField();
				void ResetVisitedFlags();
				void ResetSlideTallies();
				void SaveHeights();
			void Rainfall();
				void PickFallNeighbor(int&nx,int&ny,int x,int y);
				void SingleDrop(int x,int y);
					void DepositSoilPct(int pct,int x,int y,int&soilaccum);
					int CollectSoilPct(int rockpct,int soilpct,int x,int y);
					void DepositSoil(int pctchance,int x,int y);
					int CollectSoil(int rockchance,int soilchance,int x,int y);
						static bool OddsMet(int pctchance);
			void SmoothAllTerrain();
			bool ShrinkDrops();
		static int range(int n,int l,int h)	{	return min(max(n,l),h);		}

	};
