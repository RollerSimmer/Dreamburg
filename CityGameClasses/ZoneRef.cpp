#include "ZoneRef.h"

ZoneRef::ZoneRef(u16 stx,u16 sty,u8 stq,u16 spatternID)
	{
	tx=stx;	ty=sty;
	tq=stq;
	patternID=spatternID;
	zval=tx+ty;
	tw=pattern().tw;	tl=pattern().tl;
	}

ZoneRef::ZoneRef(u16 stx,u16 sty,u8 stq,u8 stw,u8 stl,u16 spatternID)
	{
	//construct the zone reference with directly assigned w & l
	tx=stx;	ty=sty;
	tq=stq;
	patternID=spatternID;
	zval=tx+ty;
	tw=stw;	tl=stl;
	}

ZonePattern&ZoneRef::pattern()
	{
	if(patterns->amount()<maxzonepatterns)
		return patterns->plist[patternID];
	else
		return patterns->plist[nullpatternID];
	}

bool ZoneRef::comparezvals_descending(ZoneRef a,ZoneRef b)
	{	return (a.zval>b.zval);		}

bool ZoneRef::comparezvals_ascending(ZoneRef a,ZoneRef b)
	{	return (a.zval<b.zval);		}

void ZoneRef::DrawZone(BITMAP*page,short txo,short tyo)
	{
	short dtx=tx+txo;	short dty=ty+tyo;
	short dtw;	short dtl;
	dtx*=xpixelsPerTile;
	dty*=ypixelsPerTile;

	BITMAP*patsprite=(*patterns)[patternID].sprite(tx,ty,tq);
	dtw=patsprite->w;	dtl=patsprite->h;
	blit(patsprite,page,0,0,dtx,dty,dtw,dtl);
	}



