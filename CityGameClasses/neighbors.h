#pragma once

const short xo8[8]={1,1,0,-1,-1,-1,0,1};
const short yo8[8]={0,-1,-1,-1,0,1,1,1};

namespace edge
	{
	typedef enum	{ rgt=0,top,lft,btm,mid,amt	} dirtype;
	typedef enum	{ e=0,n,w,s,c	} maptype;
	}

const short xo4[4]={1,0,-1,0};
const short yo4[4]={0,-1,0,1};
const short edgeary[4]={edge::rgt,edge::top,edge::lft,edge::btm};

const short enteredgeary[4]=
	{
	 edge::lft	//going right: enter from the left side
	,edge::btm	//going up:    enter from the bottom
	,edge::rgt	//going left:  enter from the right side
	,edge::top	//going down:  enter from the top
	};

namespace route
	{
	typedef enum	{	EW,NS,NE,NW,SW,SE,EC,NC,WC,SC,amt	}	type;
	}

