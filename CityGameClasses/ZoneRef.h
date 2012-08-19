#pragma once

#include "zonepatternlist.h"

class ZoneRef
	{
	private:
		u16 tx,ty;	u8 tq;	u8 tw,tl;	u16 patternID,zval;
	public:
		ZoneRef(u16 stx,u16 sty,u8 stq,u16 spatternID=0);
		ZoneRef(u16 stx,u16 sty,u8 stq,u8 stw,u8 stl,u16 spatternID=0);
		inline u16 rightedge()	{	return tx+pattern().tw-1;	}
		inline u16 bottomedge()	{	return ty+pattern().tl-1;	}
		ZonePattern&pattern();
		static bool comparezvals_descending(ZoneRef a,ZoneRef b);
		static bool comparezvals_ascending(ZoneRef a,ZoneRef b);
		inline bool PointInZone(short ptx,short pty) {	return ptx>=tx&&ptx<=rightedge()&&pty>=ty&&pty<=bottomedge();	}
		void DrawZone(BITMAP*page,short txo,short tyo);
	};






