#pragma once

#include "IsoTerrain.h"
#include "intvector3d.h"
#include <types/mytypes.h>
#include <vector>

using namespace std;

const int maxpathlen=255;

class TerrainEroder
	{
	protected:
		int w,l;	//width and length
		IsoTerrain*ter;
		vector<intvector3d> flows;
		intvector3d safetyflow;
		int dropsPerRound;
		int smoothradius;
		vector<bool>visitedflags;
		bool safetyvisited;
	public:
		TerrainEroder(IsoTerrain*_ter,int _w,int _l,int _dropsPerRound,int _smoothradius);
		~TerrainEroder()	{	}
		int GetWidth()		{return w;}
		int GetLength()	{return l;}
		intvector3d&flow(int x,int y);
		bool&visited(int x,int y);
		void Erode(int ncycles);
			void RoundSetup();
				void ResetFlowField();
				void ResetVisitedFlags();
			void FindFlowFields();
				void FindFlowFields_Random();
					void MakeFlowPathAt(int x,int y);	//used in both random and sequential routines
						void PickFallNeighbor(int&nx,int&ny);
				void FindFlowFields_Sequential();
			void Rainfall();
				void SingleDrop(int x,int y);
					void DepositSoil(int pctchance,int x,int y,int radius,int depth);
					int CollectSoil(int rockchance,int soilchance,int x,int y,int radius,int depth);
						static bool OddsMet(int pctchance);
			void SmoothAllTerrain()	{	ter->SmoothAll(smoothradius);	}

	};
