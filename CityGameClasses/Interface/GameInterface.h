#pragma once

#include "ZonePopup.h"
#include "ToolPopup.h"
#include "ToolButtons.h"
#include "MapInterface.h"
#include "HotkeyHandler.h"

class GameInterface
	{
	private:
		MapInterface*mapUI;
		ZonePopup*zonepopups[];
		ToolPopup*toolpopups[];
		ToolButton*toolbtns;
		HotkeyHandler*keys;
	public:
		GameInterface( MapInterface*mapUI,ZonePopup*zonepopups[],ToolPopup*toolpopups[]
		              ,ToolButton*toolbtns,HotkeyHandler*keys);

	};


