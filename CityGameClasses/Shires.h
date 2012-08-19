#pragma once

#include "Ward.h"

/*	Shire: a space with 1024x1024 tiles and 32x32 wards (10 km^2)
		+ Shires are county-sized spaces, made up of 8 Burgs (256x256).
		+ Burgs may be added later for a smaller space of multiple
		  wards.
 */

const int shire_amtWardsAcross=32;

class Shire: public Space
	{
	private:
		Ward*wardary[shire_amtWardsAcross][shire_amtWardsAcross];
		int amtpathnets;
	public:
		//construction and init:
			Shire();
			void AllocAllWards(Map::Data*map);
		//accessors:
			Ward*ward(int xw,int yw);
			inline int getamtpathnets()	{	return amtpathnets;	}
			inline int setamtpathnets(int _amtpathnets)	{	amtpathnets=_amtpathnets;	}
		//methods:
			inline bool inshire(int x,int y)	{	return x>=0&&y>=0&&x<shire_amtWardsAcross&&y<shire_amtWardsAcross;	}
			void MakePath(Ward**&wardpath,int sx,int sy,int dx,int dy,int&wardpath_len,bool&pathfound);
			void ClearWardVisitedFlags();
			void ClearWardPathLinks();
			void FindShortage(int&shx,int&shy);
			void FindSurplus(int tradeitem,bool&found,int&sux,int&suy,int shx,int shy);
			void CheckForSurplusAt(int tradeitem,bool&found,int&sux,int&suy,int shx,int shy,int trgtpathnet);
			void StepWardTrade();
				void StepWardTrade_Air(int x,int y);
					void StepWardTrade_Air_FromTile(int wx,int wy,int tx,int ty,int tz);
					void StepWardTrade_Air_FromTile(int tx,int ty,int tz);
					void AirFromWard(int cwx,int cwy,int item,int airAmtPerWard,int airradius);					
				void StepWardTrade_MakeAndTake(int x,int y);
			void TradeBetweenWards(int amtseeds);
			TradeItem_SpaceBalance*GetBalanceAt(int wx,int wy,int item);			
			bool IsConnectedByPath(int sx,int sy,int dx,int dy);
		//trade networks:
			void UpdateWardPathNets();
			void ClearWardPathNets();
			void RenumberWardPathNets();
			void UpdateWardPathNetFrom(int wx,int wy);
		//traffic volume:
			void ResetTrafficVolumes();
			void ResetTrafficVolumes_Peak();
			void ResetTrafficVolumes_Current();
			void IncCurTrafficVol(int x,int y,int volinc);
			void UpdatePeakTrafficVol(int x,int y);
		//testing methods:
			#define shires_dotesting true
			#if(shires_dotesting)
				void RandomizeWardTradeBalances();
				void RandomizeWardConnections(int pctconnected);
				int CountShortages();
			#endif
	};

