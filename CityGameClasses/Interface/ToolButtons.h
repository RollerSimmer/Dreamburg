#pragma once

#include "LayerButtons.h"
#include "BottomButtons.h"
#include "OptionButtons.h"
#include "Hotmap.h"
#include "BuildButtons.h"
#include "ReportButtons.h"

class ToolButtons
	{
	private:
		LayerButtons*layers;
		OptionButtons*options;
		Hotmap*hotmap;
		BuildButtons*build;
		ReportButtons*reports;		
	public:
		ToolButtons( LayerButtons*layers,OptionButtons*options,Hotmap*hotmap
		            ,BuildButtons*build,ReportButtons*reports);
	};


