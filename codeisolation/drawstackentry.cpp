#define drawstackentry_implementation 1
#if drawstackentry_implementation

#include "drawstackentry.h"

DrawStackEntry::
	DrawStackEntry()	{	mpi=0;	}


DrawStackEntry::
	DrawStackEntry(MapPatternIndex::Struct*i)	{	mpi=i;	}


bool
DrawStackEntry::
	compare_ascend(DrawStackEntry&a,DrawStackEntry&b)
		// returns true if a goes behind b
		{
		if(!a.mpi||!b.mpi)	return 0;
		else return a.mpi->orderval()<b.mpi->orderval();
		}


bool
DrawStackEntry::
	compare_descend(DrawStackEntry&a,DrawStackEntry&b)
		// returns true if b goes in front of a
		{	return	(!compare_ascend(a,b)); }


#endif
