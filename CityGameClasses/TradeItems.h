#pragma once

#include <types/mytypes.h>

#define useShortageAndSurplus 0
#if(useShortageAndSurplus)
	struct TradeItem_SpaceBalance
		{
		u32 shortage;
		u32 surplus;
		};
#else
	typedef long TradeItem_SpaceBalance;
#endif

struct TradeItem_UnitDef
	{
	u8 make;
	u8 take;
	u8 air;
	u8 airradius;	//in wards
	};

namespace tradeitem
	{

	typedef enum
		{
		//Natural:
			 minerals=0
			,soilnutrients
			,sunlight
			,rain
		//business:
			,buying
			,office
			,miningAndFarming
			,industry
		//progress
			,knowledge
			,culture
		//environment
			////,traffic
			,pollution
		//personal:
			,enjoyment
			,quarters
			,health
			,jobs
		//public:
			,government
			,fireAndRescue
			,law
			,defense
		//port
			,worldimport
			,worldexport
		//count
			,amount
		} id;

	}

typedef TradeItem_SpaceBalance TradeItem_SpaceBalance_Array[tradeitem::amount];
typedef TradeItem_UnitDef TradeItem_UnitDef_Array[tradeitem::amount];

//trade networks:
	const int idUnusedPathNetwork=0;
	const int idFirstUsablePathNetwork=1;





