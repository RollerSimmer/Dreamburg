#include "GameInterface.h"

GameInterface( MapInterface*_mapUI,ZonePopup*_zonepopups[],ToolPopup*_toolpopups[]
              ,ToolButton*_toolbtns,HotkeyHandler*_keys)
	{
	mapUI=_mapUI;
	zonepopups=_zonepopups;
	toolpopups=_toolpopups;
	toolbtns=_toolbtns;
	keys=_keys;
	}
       


