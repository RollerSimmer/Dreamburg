#pragma once

#include "../dendrite/dendrite.h"
#include <types/mytypes.h>

const int mapsz=64;
const int mapwidth=mapsz;
const int maplength=mapsz;

class IsoTerrain
	{
	protected:
		u16 hts[mapwidth*maplength];
		u16 rockhts[mapwidth*maplength];	//rock should always be below or equal to soil (ht)
		u16 safetyht,safetyrockht;
		u16 zcap;
		////TerrainEroder*te;
	public:
		IsoTerrain(IsoTerrain*base);
		void fixxy(int&x,int&y)	;
		int xfix(int x);
		int yfix(int y);
		bool inrange(int x,int y)	{	return x>=0&&y>=0&&x<mapwidth&&y<maplength;	}
		u16 &ht(int x,int y)	;
		u16 maxht();
		u16 &rockht(int x,int y)	;
		IsoTerrain()	;
		int GetZCap()	{	return zcap;	}
		void SetSafetyHt(u16 _safetyht)	{	safetyht=_safetyht;	}
		void GenRandHts(int zcap);
		void ScatterInc(int htinc);
		void MakeRugged(int radius);
		void FlattenAll(int level);
		void RaiseOctagon(int cx,int cy,int r,int htinc);
		void RaiseVertex(int x,int y,int htinc);
		void RuggedOctagon(int cx,int cy,int r);
		void RuggedVertex(int x,int y);
		void Smooth(int x,int y,int radius);
		void SmoothAll(int radius);
		void Halve();
		void Double();
		void RaiseAll(int zo);
		void Erode(int ncycles,int dropsPerRound,int radius,int digdepth,int smoothradius);
		void PeaksAndValleys();
			void PeaksAndValleysAt(int x,int y);
		//primitives
			void Pyramid()	{	Pyramid(zcap);	}
				void Pyramid(int h);
			void Invert();	//invert the terrain
			void Dome(int h);
			void SineHill(int h);
			void SineHillMesh(int h,int xperiods,int yperiods);
			void SharpHill(int h);
			void Voronoi(int npoints,int c1,int c2,int c3);
		void RiverErode(int ntimes,int slope,int slopescale);
		void DendriteErode(int wedgewidth,int wedgeheight,int nseeds,int dropsteps);
			void DendriteMapHeights(Dendrite*d);
		void DendriteRidges(int wedgewidth,int wedgeheight,int nseeds,int dropsteps);
			void MaxPyramid(int x,int y,int z,int w,int l);
			void MinPyramidPit(int x,int y,int z,int w,int l,int top);
		void ScramblePyramids(int amt,int loht,int hiht,int loslope,int hislope);
			void RaisePyramid(int x,int y,int htinc,int slopey,int slopex);
		void WedgeCut( int x1,int y1,int z1
						  ,int x2,int y2,int z2
						  ,int width,int height   );
			static int dist2d(int x1,int y1,int x2,int y2);
			int minht(int x,int y,int z);
			int maxht(int x,int y,int z);
		void Wedge( int x1,int y1,int z1,int x2,int y2,int z2,int wedgewidth,int wedgeheight);
	};
