#pragma once
#include <windows.h>



enum class MouseType { LDown, LUp, RDown, RUp, Move, Wheel };

struct MouseEvent {
	MouseType type;
	POINT     pos;
	POINT     dragStart{ 0,0 };
	POINT     dragEnd{ 0,0 };
	int       wheelDelta = 0;
};

 
