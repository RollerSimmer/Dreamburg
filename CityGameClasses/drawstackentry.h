#pragma once

#include "mappatternindex.h"

struct DrawStackEntry
	{	MapPatternIndex::Struct*mpi;
		DrawStackEntry();
		DrawStackEntry(MapPatternIndex::Struct*i);
		static bool compare_ascend(DrawStackEntry&a,DrawStackEntry&b);
			// returns true if a goes behind b
		static bool compare_descend(DrawStackEntry&a,DrawStackEntry&b);
			// returns true if b goes in front of a
	};
