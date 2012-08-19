#include "FlowFieldDrawer.h"
#include <allegro.h>
#include <algorithm>

using namespace std;

void FlowFieldDrawer::Draw()
	{
	for(int y=0;y<eroder->GetLength();y++)
		{
		for(int x=0;x<eroder->GetWidth();x++)
			{
			DrawTile(x,y);
			}
		}
	KeyPause();
	}

void FlowFieldDrawer::DrawTile(int x,int y)
	{
	int colorbg;
	const int colorarrow=0xffffff;	//white

	int fx=eroder->flow(x,y).GetX();
	int fy=eroder->flow(x,y).GetY();
	colorbg=makecol((fx+2)*80,(fy+2)*80,0);
	const int tilesz=8;
	int l=x*tilesz;			int t=y*tilesz;
	int r=(x+1)*tilesz-1;	int b=(y+1)*tilesz-1;
	int tailx,taily,tipx,tipy;
	//find x values for arrow tip and tail
		if(fx==-1)	{	tipx=l;	tailx=r;	}
		else if(fx==1)	{	tipx=r;	tailx=l;	}
		else if(fx==0)	{	tipx=tailx=(l+r)/2;	}
	//find y values for arrow tip and tail
		if(fy==-1)	{	tipy=t;	taily=b;	}
		else if(fy==1)	{	tipy=b;	taily=t;	}
		else if(fy==0)	{	tipy=taily=(t+b)/2;	}
	//set up draw
		acquire_screen();
	//draw background rectangle
		rectfill(screen,l,t,r+1,b+1,colorbg);
		////rect(screen,l,t,r,b,0xffff00);
	//draw arrow shaft
		line(screen,tailx,taily,tipx,tipy,colorarrow);
	//draw arrow head
		if(abs(fx)==1&&abs(fy)==1)	//diagonal
			{
			line(screen,tipx,tipy,(tipx+tailx)/2,tipy,colorarrow);
			line(screen,tipx,tipy,tipx,(tipy+taily)/2,colorarrow);
			}
		else if(fx==0&&fy==0)
			;
		else if(fx==0)	//vertical
			{
			int midy=(tipy+taily)/2;
			line(screen,tipx,tipy,l,midy,colorarrow);
			line(screen,tipx,tipy,r,midy,colorarrow);
			}
		else if(fx==0)	//horizontal
			{
			int midx=(tipx+tailx)/2;
			line(screen,tipx,tipy,midx,t,colorarrow);
			line(screen,tipx,tipy,midx,b,colorarrow);
			}
	//finish draw
		release_screen();
	}

void FlowFieldDrawer::KeyPause()
	{
	readkey();
	rest(200);
	}
