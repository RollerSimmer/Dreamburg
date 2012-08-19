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

int main()
	{
	//initialize random generator
	srand(time(0));

	//initialize Allegro
		cout<<"Initializing Allegro..."<<endl;
		allegro_init();
		cout<<"...Done"<<endl;
		install_keyboard();
		set_color_depth(32);
		set_gfx_mode(GFX_AUTODETECT_WINDOWED,640,480,0,0);

		//clear the screen w/ white
		clear_to_color(screen,0xffffffff);

	//initialize dendrite and its drawer
		Dendrite*d=new Dendrite(100,100,4);
		DendriteDrawer dd(d);

	//walk 2 border tiles
		d->WalkFrom(0,rand()%d->getl());
		d->WalkFrom(rand()%d->getw(),0);

	//walk a few hundred more
		for(int i=0;i<100;i++)
			{
			int wx=rand()%d->getw(),wy=rand()%d->getl();
			d->WalkFrom(wx,wy);
			}

	//draw the dendrite at (0,0)
		dd.Draw(0,0);
		////dd.DrawLattice(0,0);

	//loop for input
	while(!keypressed())
		{
		rest(10);
		}

	delete d;
	}
