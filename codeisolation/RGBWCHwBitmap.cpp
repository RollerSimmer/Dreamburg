/**********************************************************
**             WCL - {Wave,Clarity,Light}                **
**********************************************************/

/*
To do:
	+  add a function to remap a color using only wave and clarity,
		keeping light
*/

/*
***** Update Log *****
	2011:Jul:25:PM:07:04
		+	Started this file,
		+	added image list loader
	2011:Jul:25:PM:08:40
		+	colors now remap for a whole image
		+
*/

#include <iostream>
#include <allegro.h>

#include <types/mytypes.h>
#include <cstdlib>
#include <ctime>
#include <list>
#include <sstream>
#include <fstream>

using namespace std;

//w=Wave,c=Clarity,h=High

struct Rgb2Wcl
	{
	u16 r,g,b;
	u16 w,c,l;
	enum waveranges{yellow,cyan,magenta};

	Rgb2Wcl(u16 r,u16 g,u16 b)		{	SetRGB(r,g,b);	}
	Rgb2Wcl()							{	SetRGB(0,0,0);	}
	void SetRGB(u16 sr,u16 sg,u16 sb)	{	r=sr;	g=sg;	b=sb;	towcl();	}
	void SetRGB(u32 color)	{		SetColorFromInt(color);	}
	void SetWCL(u16 sw,u16 sc,u16 sl)	{	w=sw;	c=sc;	l=sl;	torgb();	}

	void towcl(u16 r,u16 g,u16 b)	{	SetRGB(r,g,b);	towcl();	}
	void towcl()
		{
		u16 rr,gg,bb;
		u16 m=std::min(r,std::min(g,b));
		u16 M=std::max(r,std::max(g,b));
		c=M-m;
		l=(M+m)/2;
		const u32 s=425;
		rr=r-m;	gg=g-m;	bb=b-m;
		if(c==0)	w=0;
		else if(m==b&&M==r)	w=	s*0/10	+	gg*43/rr;
		else if(m==b&&M==g)	w=	s*1/10	+	43-rr*43/gg;
		else if(m==r&&M==g)	w=	s*2/10	+	bb*43/gg;
		else if(m==r&&M==b)	w=	s*3/10	+	43-gg*43/bb;
		else if(m==g&&M==b)	w=	s*4/10	+	rr*43/bb;
		else if(m==g&&M==r)	w=	s*5/10	+	43-bb*43/rr;
		else w=0;
		}

	short waverange()	{	return (u16)w*10/425;	}
	short waverangemod()	{	return ((u16)w*10%425)/10;	}

	static short colorfix(short color)
		{
		short cf;
		cf=color;
		if(cf<0)	cf=0;
		if(cf>255)	cf=255;
		return cf;
		}

	void FixClarityWithLight()
		{
		short m=(short)l-c/2;
		short M=(short)m+c;
		short cnew=c;
		if(M>255)
			cnew=std::min((short)cnew,(short)(((short)255-l)*2));
		if(m<0)	m=0;
			cnew=std::min((short)cnew,(short)((short)(l)*2));
		if(cnew>255)
			cnew=255;
		c=cnew;
		}

	void torgb(u16 w,u16 c,u16 l)	{	SetWCL(w,c,l);	torgb();	}
	void torgb()
		{
		////u16 rr,gg,bb;
		FixClarityWithLight();
		u16 m=l-c/2,	M=m+c;
		u16 wz=waverange();		//wave zone
		u16 wzm=waverangemod();	//wave remainder
		u16 u=colorfix(m+wzm*c/42);		//intermediate value for zones going up
		u16 d=colorfix(m+(42-wzm)*c/42);	//intermediate value for zones going down

		if(c==0)	{	r=g=b=l;	}
		else
			switch(wz)		{
			case 0:	r=M;	g=u;	b=m;	break;
			case 1:	r=d;	g=M;	b=m;	break;
			case 2:	r=m;	g=M;	b=u;	break;
			case 3:	r=m;	g=d;	b=M;	break;
			case 4:	r=u;	g=m;	b=M;	break;
			case 5: 	r=M;	g=m;	b=d;	break;
			default: break;	}
		}

	void Display(int v)
		{
		cout<<"Color #"<<v<<": r="<<r<<"; g="<<g<<"; b="<<b;
		cout<<"; wave="<<w<<"; clarity="<<c<<"; light="<<l<<";";
		cout<<endl;
		}

	void SetColorFromInt(u32 color)
		{
		union intcolor
			{	struct {	u8 b,g,r,a;	};
				u32 w;		};
		intcolor ic;
		ic.w=color;	r=ic.r;	g=ic.g;		b=ic.b;
		towcl();
		}
	static u32 MakeIntColor(u16 r,u16 g,u16 b)
		{

		union intcolor
			{	struct {	u8 b,g,r,a;	};
				u32 w;		};
		intcolor ic;
		ic.w=0;	ic.r=r;	ic.g=g;	ic.b=b;
		return ic.w;
		}
	u32 MakeIntColor()	{	return MakeIntColor(r,g,b);	}

	//laxequ(): laxed equal
	static bool laxequ(int a,int b,int errmargin)
		{	return(std::abs(a-b)<=errmargin);	}

	bool remapmatch(u16 tw,u16 tc,u16 werrm,u16 cerrm)
		{	return(laxequ(tw,w,werrm)&&laxequ(tc,c,cerrm));		}

	//MakeRemapColor(): remap any color that matches target wave and clarity with remap wave and clarity
	u32 MakeRemapColor(u16 tw,u16 tc,u16 rw,u16 rc,u16 wmargin,u16 cmargin)
		{
		if(remapmatch(tw,tc,wmargin,cmargin))	{
			Rgb2Wcl mrc;
			u16 rcfix=std::min(rc,(u16)(l*2));
			mrc.SetWCL(rw,rcfix,l);
			return mrc.MakeIntColor();		}
		else	return MakeIntColor();
		}

	static void RemapImage(BITMAP*bmp,BITMAP*&bmpr,u16 tw,u16 tc
	                       ,u16 rw,u16 rc,u16 wtolerance,u16 ctolerance)
		{
		short bw,bl;
		bw=bmp->w;	bl=bmp->h;
		bmpr=create_bitmap(bw,bl);
		blit(bmp,bmpr,0,0,0,0,bw,bl);

		Rgb2Wcl pixel;

		//remap the image's colors
		for(int y=0;y<bl;y++)
		for(int x=0;x<bw;x++)
			{
			//get the pixel
			pixel.SetRGB(getpixel(bmp,x,y));
			//remap the pixel, and put it on the remap image
			putpixel(bmpr,x,y,pixel.MakeRemapColor(tw,tc,rw,rc,wtolerance,ctolerance));
			}

		}
	};


struct ImageLoader
	{
	ifstream imglistfile;

	ImageLoader(const char*imgListFilename)
		{
		imglistfile.open(imgListFilename);
		}
	BITMAP*ReadNextFile()
		{
		char imgfname[256];
		stringstream line;
		BITMAP*bmp;

		//read file name
		imglistfile.getline(imgfname,256,'\n');
		//load the file
		bmp=load_bitmap(imgfname,0);
		if(bmp)
			cout<<"file "<<"\'"<<imgfname<<"\' loaded"<<endl;
		else
			cout<<"error loading "<<"\'"<<imgfname<<"\'"<<endl;
		return bmp;
		}
	};





int main()
	{
	//initialize Allegro
	cout<<"Initializing Allegro..."<<endl;
	allegro_init();
	cout<<"...Done"<<endl;
	install_keyboard();
	install_timer();
	set_color_depth(32);
	set_gfx_mode(GFX_AUTODETECT_WINDOWED,640,480,0,0);
	srand(time(0));

	acquire_screen();
	//clear the screen w/ white
	clear_to_color(screen,0xffffffff);
	release_screen();

	Rgb2Wcl color,rcolor;

	ImageLoader imgl("filelist.txt");
	BITMAP*bmp;
	bool done=false;

	while(!done)
		{

		bmp=imgl.ReadNextFile();
		done=bmp==(BITMAP*)0;

		if(!done)
			{
			BITMAP*bmpr;
			u16 tw,tc,rw,rc,wtolerance,ctolerance;
			tw=128;	tc=128+32;	rw=42;	rc=160;
			wtolerance=42;	ctolerance=128;
			Rgb2Wcl::RemapImage(bmp,bmpr,tw,tc,rw,rc,wtolerance,ctolerance);

			short x,y,rx,ry;
			x=y=0;	rx=x+bmp->w;		ry=y;

			acquire_screen();

			//clear screen and draw original and remapped images side by side
			clear_to_color(screen,0xff00ff);
			blit(bmp,screen,0,0,x,y,bmp->w,bmp->h);
			blit(bmpr,screen,0,0,rx,ry,bmpr->w,bmpr->h);

			release_screen();

			destroy_bitmap(bmpr);
			destroy_bitmap(bmp);
			}

		if(readkey()>>8==KEY_ESC)	done=true;
		}
	}
