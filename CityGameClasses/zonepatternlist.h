/*

To do:
	+	Speed up CutupImage()!
In Progress:
	+

********** Update Log *******************
2011:Aug:13:PM:03:25
	+	added to ZonePattern a method to generate a simple zone image for testing
	+	...method to cut up image into smaller tile images
2011:Aug:14:PM:11:36
	+	added a function to display spaced tile images
2011:Aug:14:PM:12:49
	+	added a class to ZonePattern::CutupImage() called Cutup
		which adds some methods commonly used within CutupImage.
		My first experiment with subfunctions in C++. Used them
		some in Pascal
2011:Aug:14:PM:04:56
	+	CutupImage() works, but it's pretty slow for real time cutting
		I will have to speed it up some.  put/get pixel refs
		need to be modified, and bounds need to be optimized.
2011:Aug:15:AM:10:45
	+	fixed CutUpImage's progress display method
	+	fixed some loops that compared tx against tl instead of tw
		like it should have
2011:Aug:18:AM:10:17
	+	added occupancy() methods, and fixed their initialization
2011:Aug:18:PM:12:00
	+	now cutup looks at occupancy
2011:Aug:18:PM:05:11
	+	now cutup ignores unoccupied tiles on full cases, need to
		on quad cases
	+	a occupancy bitmap generator was created
2011:Aug:19:PM:07:17
	+	most of the cutup process works as far as i can tell
		need to do more testing
	+	Added DrawWholeImage to zonepattern
2011:Aug:20:PM:06:43
	+	now the triangle cut up seems to work and fill the whole image
		(except for bottom tiles since height goes UP)
	+	messy code, so I may need to plan carefully a
		already getting lost.
*/

#pragma once

#include "zonepattern.h"
#include "drawstackentry.h"

using namespace std;

const short maxzonepatterns=16384;
const short nullpatternID=0;

struct ZonePatternList
	{
	std::vector<ZonePattern> plist;
	ZonePatternList();
	u32 nlistentries;
	bool AddPattern(	ZonePattern&pat	);
	bool AddPattern(u8 tw,u8 tl,u8 nfloors=1,const char*pcxfile=""
				      ,const char*patname="pattern",bool loadpcxfile=false
	               ,bool usesTriQuads=false);
	short amount();
	void GenerateRandom(short npats);
	ZonePattern&operator[](int i);
	ZonePattern*Zone(int zpati);
	};
	

extern ZonePatternList*patterns;
