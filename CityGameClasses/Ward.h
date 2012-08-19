#pragma once

#include "map.h"
#include "Spaces.h"
#include "neighbors.h"

/*	Ward: a space with 32x32 tiles (1.02 km^2)
		+ Wards are fully walkable areas
		+ Transportation and trade is done between wards
 */

const int amtSistersPerItem=3;
#define amtTilesPerWard 32

struct Ward_TileVisitMap
	{
	#if amtTilesPerWard==32
		u32 row[amtTilesPerWard];
	#else
		u32 row[amtTilesPerWard][(amtTilesPerWard+31)/32];
	#endif
	};

#define map_paths_implemented_yet false

class Ward: public Space
	{
	private:
		Map::Data*map;
		Ward*sisterwards[tradeitem::amount][amtSistersPerItem];		//...wards that are "sister" to this one
		u16 pathnet;				//...trade network
		////bool isused;				//...is this ward used?
		bool isconnectedAry[route::amt];	//n-s,e-w,n-e,n-w,s-w,s-e
		bool wasvisited;
		Ward /* *pathnext,*/*pathprev;
		u8 pathEnterEdge/*,pathExitEdge*/;
		Ward_TileVisitMap tilevisits;
		int curTrafficVol;
		int peakTrafficVol;
	public:
		//construction and initalizers:
			Ward(int xward,int yward,Map::Data*_map);
		//accessors:
			inline Map::Data*getmap()	{	return map;	}
			inline Ward*getsister(int item,int isister);
			//path net:
				inline u16 getpathnet()	{	return pathnet;	}
				inline void setpathnet(u16 _pathnet)	{	pathnet=_pathnet;	}
			//ward visited:
				inline void SetVisited()	{	wasvisited=true;	}
				inline void ClearVisited()	{	wasvisited=false;	}
				inline bool GetVisitedState()	{	return wasvisited;	}
			//tile visited:
				void SetTileVisitedState(int tx,int ty,bool state);
				bool WasTileVisited(int tx,int ty);
			//path connections:
				inline Ward*GetPathPrev()	{	return pathprev;	}
				inline Ward*SetPathPrev(Ward*prev)	{	pathprev=prev;	}
				////inline u8 GetPathExitEdge()	{	return pathExitEdge;	}
				inline u8 GetPathEnterEdge()	{	return pathEnterEdge;	}
				////inline u8 SetPathExitEdge(u8 edge)	{	pathExitEdge=edge;	}
				inline u8 SetPathEnterEdge(u8 edge)	{	pathEnterEdge=edge;	}
				////inline Ward*GetPathNext()	{	return pathprev;	}
				////inline Ward*SetPathNext(Ward*next)	{	pathnext=next;	}
			//traffic volume:
				inline void SetPeakTrafficVol(int peakvol)	{	peakTrafficVol=peakvol;	}
				inline int GetPeakTrafficVol()	{	return peakTrafficVol;	}
				inline void SetCurTrafficVol(int peakvol)	{	curTrafficVol=peakvol;	}
				inline int GetCurTrafficVol()	{	return curTrafficVol;	}
		//methods:
			bool IsEdgeTile(int tx,int ty,int edgev);
			bool IsTileInWard(int tx,int ty);
			bool IsConnected(int edge1,int edge2);
			bool IsConnected(int routev);
			bool IsFullyConnected();
			bool IsPathConnectedTo(int edge1);
			bool DoesEdgeHavePath(int edgev);
			bool DoesTileHavePath(int tx,int ty);
			void FindConnections();
			bool FindConnection(int routev);
			bool FindConnection(int edge1,int edge2);
			void ClearTileVisits();
			void StepTrade_MakeAndTake();
			void Air_Receive(int item,int airamt);
			void IncShortage(int item,int amt);
			void IncSurplus(int item,int amt);
			void FixShortageAndSurplus(int item);
			bool HasShortage(int item);
			bool HasSurplus(int item);
			inline bool IsBalanced(int item)	{	return !HasShortage(item)&&!HasSurplus(item);	}
			//traffic volume:
				inline void IncCurTrafficVol(int volinc)	{	curTrafficVol+=volinc;	};
				void UpdatePeakTrafficVol();				
		//testing methods:
			void RandomizeTradeBalances();
			void RandomizeConnections(int pctconnected);
			
	};




