/******************************************************************************
*Author:		Kehnin Dyer
*File name:		Console_Object.h
*Date Created:	2012/04/09
*Modifed:		
*					:see git commits for more details
******************************************************************************/



#include "Console.h"

#ifndef _NC_CONSOLE_OBJECT__
#define _NC_CONSOLE_OBJECT__

namespace nitrocorp{ namespace console {
class absObject
{
	COORD m_Position;
	SMALL_RECT m_Area;
	
	//assigned with object_states
	WORD m_state; 

public:

};




}}
#endif