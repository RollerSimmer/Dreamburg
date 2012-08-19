#pragma once

#include "IsoTerrain.h"
#include <types/mytypes.h>

const int nlastangles=5;

class RiverEroder
	{
	private:
		IsoTerrain*ter;
		int courselen;
		u8 courseData[mapwidth*maplength];
		u8 courseDataSafe;
		int riverlevel;
		int slope;
		int slopescale;
		u16 waterleveldata[mapwidth*maplength];
		u16 waterlevelsafe;
		int xoary[nlastangles];
		int yoary[nlastangles];
		int xoavg;
		int yoavg;
	public:
		void fixxy(int&x,int&y)	{	ter->fixxy(x,y);	}
		bool inrange(int x,int y)	{	return ter->inrange(x,y);}
		RiverEroder(IsoTerrain*_ter)	{	Init(_ter);	}
		void Init(IsoTerrain*_ter)	{	ter=_ter;	}
		~RiverEroder()	{	Kill();	}
		void Kill()	{	}
		u8&course(int x,int y);
		u16&waterlevel(int x,int y);
		bool haswater(int x,int y);
		u16&ht(int x,int y)	{	return ter->ht(x,y);	}
		void Erode(int x,int y,int slope,int slopescale);
		void ChoosePathFromHere(int&nx,int&ny,int x,int y);
		void Drop(	int x,int y	);
		void ClearCourse();
		void DrainWater();
		void LowerWaterLevels(int amt);
		void MakeCourse(int x,int y);
		void DisplayCourseOnScreen();
		void FloodExitPoint(int &nx,int&ny,int x,int y);
		static int angle45(int x1,int y1,int x2,int y2);
		int LastAnglesAvg();
		void LastVectorsAvg();
	};

