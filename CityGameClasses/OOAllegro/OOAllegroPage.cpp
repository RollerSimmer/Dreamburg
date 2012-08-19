#include "OOAllegroPage.h"
#include <algorithm>

using namespace std;

//initialization
	OOAllegroPage::OOAllegroPage(BITMAP*_parent)
		{
		parent=_parent;
		if(parent)
			page=create_bitmap(parent->w,parent->h);
		else	page=0;
		}

	OOAllegroPage::~OOAllegroPage()
		{
		if(page)
			destroy_bitmap(page);
		}

//preparation:
	void OOAllegroPage::acquire(bool prepare)
		{	if(page&&prepare)	acquire_bitmap(page);	}

	void OOAllegroPage::release(bool prepare)
		{	if(page&&prepare)	release_bitmap(page);	}

//blitting:
	void OOAllegroPage::pageflip()
		{
		if(page)
			blit(page,parent,0,0,0,0,page->w,page->h);
		}

//drawing:
	void OOAllegroPage::rectfill(int x,int y,int w,int h,int color,bool prepare)
		{
		w=abs(w);	h=abs(h);
		acquire(prepare);
			::rectfill(page,x,y,x+w-1,y+h-1,color);
		release(prepare);
		}

	void OOAllegroPage::putpixel(int x,int y,int color,bool prepare)
		{
		acquire(prepare);
			::putpixel(page,x,y,color);
		release(prepare);
		}

	void OOAllegroPage::clear(int color,bool prepare)
		{
		acquire(prepare);
			::clear_to_color(page,color);
		release(prepare);
		}

