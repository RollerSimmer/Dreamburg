#include "mappatternindex.h"
#include "zonedims.h"

#define MapPatternIndex_implementation 1
#if MapPatternIndex_implementation


u32
MapPatternIndex::Struct::
	orderval()
		{
		u32 ov;
		ov=patquad;
		ov*=zone::maxelevation;
		ov+=elevation;
		ov*=zone::maxwidth;
		ov+=(zone::maxwidth-patxofs-1);
		ov*=zone::maxlength;
		ov+=(zone::maxlength-patyofs-1);
		ov*=zone::maxstack;
		ov+=stackidx;
		return ov;
		}

#endif
