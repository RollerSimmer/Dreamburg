#include "PathNetDisplay.h"
#include <iostream>
using namespace std;

//initialization:
	PathNetDisplay::PathNetDisplay(Shire*_shire)
		{
		shire=_shire;
		oopage=new OOAllegroPage(screen);
		}

	PathNetDisplay::~PathNetDisplay()
		{
		delete oopage;
		}

//display:
	void PathNetDisplay::DrawPathNets()
		{
		oopage->acquire();
			oopage->clear(0,false);
			////cout<<"Drawing Trade Nets:"<<endl;
			for(int wy=0;wy<shire_amtWardsAcross;wy++)
				{
				////cout<<"\tWY="<<wy<<":"<<endl;
				for(int wx=0;wx<shire_amtWardsAcross;wx++)
					{
					////cout<<"\t\tWX="<<wx<<": ";
					int dx,dy;
					int netcolor=GetPathNetColor(wx,wy);
					////if(netcolor!=0)
						{
						////cout<<"netcolor="<<netcolor;
						////cout<<"; pathnet="<<(int)(shire->ward(wx,wy)->getpathnet());
						}
					////cout <<";"<<endl;
					dx=wx*16;
					dy=wy*16;
					oopage->rectfill(dx,dy,15,15,netcolor,false);

					}
				}
		oopage->release();
		acquire_screen();
			oopage->reparent(screen);
			oopage->pageflip();
		release_screen();
		}

	int PathNetDisplay::GetPathNetColor(int wx,int wy)
		{
		Ward*cward=shire->ward(wx,wy);
		if(cward)
			return GetPathNetColor(cward->getpathnet());
		else
			return 0;
		}

	int PathNetDisplay::GetPathNetColor(int pathnet)
		{
		int r,g,b,netcolor;
		pathnet=pathnet%512;
		r=pathnet%8;		g=pathnet/8%8;		b=pathnet/8/8%8;
		r*=32;				g*=32;				b*=32;
		netcolor=makecol(r,g,b);
		return netcolor;
		}




