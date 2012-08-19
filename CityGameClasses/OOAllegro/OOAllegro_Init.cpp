#include "OOAllegro.h"
#include <allegro.h>
#include <iostream>

using namespace std;

OOAllegro::OOAllegro()	
	{
	Init(800,600,32);
	}

OOAllegro::OOAllegro(int w,int h,int colordepth)	
	{
	Init(w,h,colordepth);
	}

void OOAllegro::Init(int _w,int _h,int _colordepth)
	{
	//init data members
		w=_w;	h=_h;	colordepth=_colordepth;
	//do allegro init routines:
		cout<<"Initializing Allegro..."<<'\t';
			allegro_init();
			install_keyboard();
			install_timer();
			set_color_depth(colordepth);
			set_gfx_mode(GFX_AUTODETECT_WINDOWED,800,600,0,0);
		cout<<"...Done"<<endl;
	}

