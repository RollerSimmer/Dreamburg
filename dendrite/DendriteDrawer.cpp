#include "DendriteDrawer.h"

DendriteDrawer::DendriteDrawer(Dendrite*_d,int _scalediv)
	{
	d=_d;
	scalediv=_scalediv;
	page=create_bitmap(d->getw()*d->getspacing()/scalediv,d->getl()*d->getspacing()/scalediv);
	}

DendriteDrawer::~DendriteDrawer()
	{
	destroy_bitmap(page);
	}

void DendriteDrawer::Draw(int x,int y)
	{
	int x1,y1,x2,y2,ix,iy;
	LatticePoint*lp,*lplast;
	int color_bg=0x000000;
	////int color_line=0xffffff;
	int color_line;
	acquire_bitmap(page);
		clear_to_color(page,color_bg);
	release_bitmap(page);
	//draw lattice lightly:
		////DrawLattice(x,y,0x030703);

	int nrandpts=(d->getw()*d->getl()/32);

	//First, trace the paths
		d->TracePaths(nrandpts);

	acquire_bitmap(page);

		for(iy=0;iy<d->getl();iy++)
		for(ix=0;ix<d->getw();ix++)
		////for(int i=0;i<nrandpts;i++)
			{
			////ix=rand()%d->getw();
			////iy=rand()%d->getl();
			int linelen=0;
			const int linelen_max=(d->getw()+d->getl())*7/10;
			lp=d->pt(ix,iy);
			bool done=false;
			/*
			while(!done)
				{
				if(lp)
					{
					*/
					if(lp->next!=0&&lp->next!=d->GetExitPoint())
						{
						////++lp->routelen;

						x1=lp->x/scalediv;
						y1=lp->y/scalediv;
						x2=lp->next->x/scalediv;
						y2=lp->next->y/scalediv;

						int color_level;
						const int color_step=20;
						const int color_scalediv=10;
						////#define shadedlines false
						#define shadedlines true
						#if shadedlines
							////color_level=lp->routelen*color_step/color_scalediv;
							color_level=(10+lp->amtNestedBranches)*color_step/color_scalediv;
						#else
							color_level=255;
						#endif
						color_line=makecol(color_level,color_level,color_level);

						//draw a line from this coord to next coord
							line(page,x1,y1,x2,y2,color_line);

						}
					else
						{
						x1=lp->x/scalediv;
						y1=lp->y/scalediv;
						circlefill(page,x1,y1,3,0x7fff7f);
						}
						/*
					//update lp
						lplast=lp;
						lp=lp->next;
					linelen++;
					done=done||(linelen>linelen_max);
					done=done||(lp==lplast);
					}
				else
					done=true;

				}
				*/

			}
	release_bitmap(page);
	acquire_screen();
		blit(page,screen,0,0,x,y,page->w,page->h);
	release_screen();
	}

void DendriteDrawer::DrawLattice(int x,int y,int color)
	{
	LatticePoint*cp,*ep,*sep,*sp;
	int ix,iy;

	int color_bg=0x000000;
	int color_line=color;
	acquire_bitmap(page);
		clear_to_color(page,color_bg);
		for(iy=0;iy<d->getl();iy++)
		for(ix=0;ix<d->getw();ix++)
			{
			cp=d->pt(ix,iy);
			ep=d->pt(ix+1,iy);
			sep=d->pt(ix+1,iy+1);
			sp=d->pt(ix,iy+1);
			if(cp&&ep&&sep&&sp)
				{
				//draw a line from this coord to next coord
					line(page,cp->x/scalediv,cp->y/scalediv,ep->x/scalediv,ep->y/scalediv,color_line);
					line(page,ep->x/scalediv,ep->y/scalediv,sep->x/scalediv,sep->y/scalediv,color_line);
					line(page,sep->x/scalediv,sep->y/scalediv,sp->x/scalediv,sp->y/scalediv,color_line);
					line(page,sp->x/scalediv,sp->y/scalediv,cp->x/scalediv,cp->y/scalediv,color_line);
				}
			}
	release_bitmap(page);
	acquire_screen();
		blit(page,screen,0,0,x,y,page->w,page->h);
	release_screen();
	}

