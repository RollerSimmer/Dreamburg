#include	"zonepatternlist.h"


///*** ZonePatternList - implementation ***
#define ZonePatternList_implementation 1
#if ZonePatternList_implementation

ZonePatternList::
	ZonePatternList()
		:	plist(maxzonepatterns,ZonePattern(0,0,0,"","empty",false))
		{
		plist.clear();
		nlistentries=0;
		ZonePattern nullpattern(0,0,0,"","nullpat",false);
		plist.push_back(nullpattern);
		}

bool
ZonePatternList::
	AddPattern(	ZonePattern&pat	)
		{
		if(amount()<=maxzonepatterns)			{
			++nlistentries;
			plist.push_back(pat);
			cout<<"plist.size()="<<plist.size()<<endl;
			cout<<"plist.capacity()="<<plist.capacity()<<endl;
			return true;		}
		else
			return false;
		}

bool
ZonePatternList::
	AddPattern(u8 tw,u8 tl,u8 nfloors,const char*pcxfile
				  ,const char*patname,bool loadpcxfile
	           ,bool usesTriQuads)
		{
		ZonePattern zp(tw,tl,nfloors,pcxfile,patname,loadpcxfile,usesTriQuads);
		AddPattern(zp);
		}

short
ZonePatternList::
	amount()
		{
		////return list.size();
		return nlistentries;
		}


void
ZonePatternList::
	GenerateRandom(short npats)
		{
		char names[][13]=
			{
			"houses","apts","offices","stadium","arena","hospital"
			,"policedept","firedept","cityhall","stcapitol","mill"
			,"manuplant","mall"
			};
		const short nnames=sizeof(names)/sizeof(names[0]);
		for(int i=0;i<npats;i++)
			{
			u16 rtw=rand()%8+12,	rtl=rand()%8+12,	rnfloors=rand()%128;
			short ni=rand()%nnames;
			std::cout<<"Generating random zone #"<<i<<"..."<<std::endl;
			ZonePattern*zp=new ZonePattern(rtw,rtl,rnfloors,"",names[ni],false);
			AddPattern(*zp);
			delete zp;
			}
		}

ZonePattern&
ZonePatternList::
	operator[](int i)
		{
		if(i<amount())	return plist[i];
		else return plist[nullpatternID];
		}


#endif
//!*** ZonePatternList - implementation ***

