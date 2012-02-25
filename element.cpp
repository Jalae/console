#include "element.h"


bool element::onMouseClick()
{
	if(gameState->numRead == 0)
		return false;
	else
		for(int i = gameState->numRead; i >= 0; i--)
		{
			if(gameState->inputStat[i].EventType==MOUSE_EVENT 
				&& gameState->inputStat[i].Event.MouseEvent.dwEventFlags != MOUSE_MOVED
				&& gameState->inputStat[i].Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED
				&& r.contains(gameState->inputStat[i].Event.MouseEvent.dwMousePosition))
					return true;
		}
	return false;
}



void element::move(int const x, int const y)//these are offsets.
{
	pos.m_p.X += x;
	pos.m_p.Y += y;
	r = pos;
}


void element::moveTo(point const p)//this is absolute
{
	pos = p;
	r = p;
}