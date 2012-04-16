#ifndef _NC_CONSOLE_OUT__
#define _NC_CONSOLE_OUT__
//here to make intellisence happier
#include "Console.h"

namespace nitrocorp { namespace console {

///////////////////////////////////////////////////////////////////////////////
//Usermode functions for controlling output to the screen
///////////////////////////////////////////////////////////////////////////////
	template<typename charT>
	void console<charT>::Draw()
	{
		SwapDisplayBuffers();
	}

	template<typename charT>
	void console<charT>::Write(charT ch)
	{
		CHAR_INFO temp;
			
		//if it is change attrib to match---------|-1 function 
		ch = ParseEscape(ch);
		if(ch == 0)
			return; //we don't want to write a 0

		//build temp
		temp = BuildCharInfo(ch);
		SMALL_RECT rec = {vCursorPos.X,vCursorPos.Y,vCursorPos.X,vCursorPos.Y};
		WriteConsoleOutput(
				ConsoleState->out_buffer,
				&temp,
				ONE_COORD,
				ORIGIN,
				&rec
			);
		vCursorPos.X++;
	}


	/*
	template<typename charT>
	console<charT>& console<charT>::operator<< (console<charT>& out, charT* str)
	{
		Write(str);
		return out;
	}
	*/
///////////////////////////////////////////////////////////////////////////////
//Usermode functions for controlling Input to the screen
///////////////////////////////////////////////////////////////////////////////


}}

#endif