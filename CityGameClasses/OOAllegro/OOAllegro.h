#pragma once

class OOAllegro
	{
	private:		
		int w,h,colordepth;
	public:
		OOAllegro();
		OOAllegro(int w,int h,int colordepth);
		~OOAllegro()	{	}
		void Init(int w,int h,int colordepth);
	};

