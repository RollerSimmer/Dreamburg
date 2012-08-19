#pragma once

#include <types/mytypes.h>

namespace MapPatternIndex
	{
	enum{index=0,offset=1};
	enum{empty=0,first=1};

	struct Struct
		{
		u16 patxofs : 6;16 patyofs : 6;
		u16 patquad : 3;16 flagfulltile : 1;
		u16 patidx : 12;16 elevation	:	4;
		u16 stackindex : 4;16 colorremap1 : 4;
		u16 colorremap2 : 4;16 colorremap3 : 4;

		u32 orderval();
		};
	}

