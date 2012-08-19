#include "ColorCodedTradeBal.h"
#include "../OOAllegro/OOAllegroPage.h"

//initialization:
	ColorCodedTradeBal::ColorCodedTradeBal(Shire*_shire)
		{
		shire=_shire;
		oopage=new OOAllegroPage(screen);
		}

	ColorCodedTradeBal::~ColorCodedTradeBal()
		{
		delete oopage;
		}

//display:
	void ColorCodedTradeBal::DrawTradeBalances()
		{
		int balcolor;

		////cout<<"Number of shortages before color-coded output: "<<shire->CountShortages()<<endl;

		oopage->acquire();
			oopage->clear(0,false);
			for(int wy=0;wy<shire_amtWardsAcross;wy++)
				{
				////cout<<"row #1:"<<endl;
				for(int wx=0;wx<shire_amtWardsAcross;wx++)
					{
					////cout<<"\tward@("<<wx<<","<<wy<<"):"<<endl;
					for(int item=0;item<tradeitem::amount;item++)
						{
						////cout<<"\t\ttrade item #"<<item<<": "<<*shire->ward(wx,wy)->gettradebal(item)<<endl;
						balcolor=getbalcolor(wx,wy,item);
						int dx,dy;
						dx=(wx*6+item/5)*3;
						dy=(wy*6+item%5)*3;
						oopage->rectfill(dx,dy,3,3,balcolor,false);
						}
					}
				}
		oopage->release();
		acquire_screen();
			oopage->reparent(screen);
			oopage->pageflip();
		release_screen();
		}

	int ColorCodedTradeBal::getbalcolor(int wx,int wy,int item)
		{
		TradeItem_SpaceBalance*bal;
		Ward*ward=shire->ward(wx,wy);
		if(ward)
			{
			bal=ward->gettradebal(item);
			if(bal)
				#if(useShortageAndSurplus)
					return getbalcolor((long)bal->surplus-(long)bal->shortage);
				#else
					return getbalcolor(*bal);
				#endif
			}
		else
			return 0;
		}

	int ColorCodedTradeBal::getbalcolor(long balance)
		{
		const int balmax=1000000;
		const int balmin=-balmax;
		int balcap;
		balcap=balance;
		if(balcap>balmax)
			balcap=balmax;
		if(balcap<balmin)
			balcap=balmin;
		if(balcap<0)
			balcap=*(&balcap);
		int balcolor,r,g,b;
		g=255*(balcap-balmin)/(balmax-balmin);
		r=255-g;
		b=0;
		balcolor=makecol(r,g,b);
		return balcolor;
		}




