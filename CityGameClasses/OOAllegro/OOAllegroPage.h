#pragma once
#include <allegro.h>

class OOAllegroPage
	{
	private:
		BITMAP*page;
		BITMAP*parent;
	public:
		//initialization:
			OOAllegroPage(BITMAP*parent);
			~OOAllegroPage();
		//access
			void reparent(BITMAP*_parent)	{	parent=_parent;	}			
		//blitting:
			void pageflip();
		//preparation:
			void acquire(bool prepare);
			void release(bool prepare);
			inline void acquire()	{	acquire(true);	}
			inline void release()	{	release(true);	}
		//drawing:
			void rectfill(int x,int y,int w,int h,int color,bool prepare);
			void putpixel(int x,int y,int color,bool prepare);
			void clear(int color,bool prepare);


	};


