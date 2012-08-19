/**********************************************************************
**             Allegro game library project template                 **
**********************************************************************/

#include "dendrite.h"
#include "DendriteDrawer.h"

#include <iostream>
#include <allegro.h>
#include <algorithm>
#include <ctime>

using namespace std;

void main_dendritemap();

int main()
	{
	//initialize random generator
	srand(time(0));

	//initialize Allegro
		cout<<"Initializing Allegro..."<<endl;
		allegro_init();
		cout<<"...Done"<<endl;
		install_keyboard();
		install_timer();
		set_color_depth(32);
		set_gfx_mode(GFX_AUTODETECT_WINDOWED,800,600,0,0);

		//clear the screen w/ white
		clear_to_color(screen,0xffffffff);

	//initialize dendrite and its drawer
		Dendrite*d=new Dendrite(32,32,16);
		DendriteDrawer dd(d,1);

	//repeat filling map until done
		bool done=false;
		while(keypressed())
			{	rest(0);	}

		dd.Draw(0,0);

		while(!done)
			{
			if(keypressed())
				{
				if(key[KEY_ESC])
					done=true;
				if(key[KEY_D])
					{
					//fill dendrite map
						////d->FillMap();
						d->FillMapWithBackTracing(false,true);
					//draw the dendrite at (0,0)
						dd.Draw(0,0);
						////dd.DrawLattice(0,0,0xffffff);
					}
				rest(10);
				}
			else
				rest(0);
			}

	delete d;
	}

void main_dendritemap()
	{

	}

