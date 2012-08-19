#pragma once

const int latticepoint_maxwgt=10;
const int latticepoint_minwgt=10;

#include "ShuffleQueue.h"

struct LatticePoint
	{
	int x,y,z,wgt,routelen,amtbranches,amtNestedBranches;
	LatticePoint
		*next
		,*r,*ur,*u,*ul,*l,*dl,*d,*dr;
	};

typedef enum	{	cornerur=0,cornerul,cornerdl,cornerdr
						,edgelft,edgergt,edgetop,edgebtm
                  ,edgenone
                  ,amtedges	}		edgetype;
typedef enum	{	dirl=0,dirul,diru,dirur,dirr,dirdr,dird,dirdl,dirNoneYet,amtdirs } dirtype;

class Dendrite
	{
	private:
		int w,l,spacing;
		LatticePoint*points,safepoint,exitpoint;

	public:

		Dendrite(int _w,int _l,int _spacing);
		~Dendrite();

		LatticePoint*pt(int x,int y,bool usesafecoords);
		inline LatticePoint*pt(int x,int y)	{	return pt(x,y,true);	}
		int fixdir(int dir);
		bool inrange(int x,int y);
		bool onedge(int x,int y,int edge);
		bool OnAnyEdge(int x,int y);
		int safex(int x);
		int safey(int y);
		int getw()	{	return w;	}
		int getl()	{	return l;	}
		LatticePoint*GetExitPoint()	{	return &exitpoint;	}
		int getspacing()	{	return spacing;	}

		void ClearVisits();
		void InitPoints();
		void UnlinkPoints();
		void WalkFrom(int x,int y);

			void WalkStart(int x,int y,int &dir,bool&done);
			int ClosestNeighborDirection(int x,int y,int prevdir);
			void WalkStep(int &x,int &y,int &dir,bool&done);

		void FillMapWithBackTracing(bool byht,bool doinitpts);
			void WalkBackwardsFrom(int x,int y,bool byht);
				static void CycleAmtLinks(int&amtlinks);
				void Walk_ProcessShuffleQueue( ShuffleQueue<LatticePoint*>*lpsq
                                          ,bool byht);
					void Walk_ShufflePop(LatticePoint*&lp,ShuffleQueue<LatticePoint*>*lpsq
												,int&amtlinks);
					void Walk_PushNeighbors( LatticePoint*lp,ShuffleQueue<LatticePoint*>*lpsq
													,int amtlinks,bool byht);

		LatticePoint*ngbr(int x,int y,int dir);
		static int dist(LatticePoint*p1,LatticePoint*p2);
		static int slope(LatticePoint*p1,LatticePoint*p2);

		void FillMap();
			void ScatterEdgeWalk(bool byht,int maxspacing);
				void ScatterWalk(bool byht);
					inline void ScatterWalk()	{	ScatterWalk(true);	}
			void SequentialWalk(bool byht);
				inline void SequentialWalk()	{	SequentialWalk(true);	}

		void TracePaths(int amtseeds);
		void CountBranches();
			int CountBranches(LatticePoint*lp);
			int CountImmediateBranches(LatticePoint*lp);
	};
