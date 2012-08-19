#pragma once

#include <types/mytypes.h>

namespace MapPatternIndex
	{
	enum{index=0,offset=1};
	enum{empty=0,first=1};

	struct Struct
		{
		u16 patxofs : 6;
		u16 patyofs : 6;
		u16 patquad : 3;
		u16 flagfulltile : 1;
		u16 patidx : 12;
		u16 elevation	:	4;
		u16 stackidx : 4;
		u16 colorremap1 : 4;
		u16 colorremap2 : 4;
		u16 colorremap3 : 4;

		u32 orderval();
		};
	}
