/**********************************************************************
**             Allegro game library project template                 **
**********************************************************************/

#include <iostream>
#include <allegro.h>

using namespace std;





int main()
	{
	//initialize Allegro
	cout<<"Initializing Allegro..."<<endl;
	allegro_init();
	cout<<"...Done"<<endl;
	install_keyboard();
	set_color_depth(32);
	set_gfx_mode(GFX_AUTODETECT_WINDOWED,640,480,0,0);

	//clear the screen w/ white
	clear_to_color(screen,0xffffffff);
	//& draw a green filled circle
	circlefill(screen,SCREEN_W/2,SCREEN_H/2,SCREEN_H/2
	           ,0xff007f00);
	//loop for input
	while(!keypressed())
		{
		rest(10);
		}
	}
