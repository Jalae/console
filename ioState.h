#pragma once

#include <Windows.h>
#include <iostream>

namespace nitrocorp { namespace console {

class ioState
{
public:
	ioState()
	{
		stin = GetStdHandle(STD_INPUT_HANDLE);
		stout = CreateConsoleScreenBuffer( 
			GENERIC_READ | GENERIC_WRITE,
			0,//not shared with other apps
			NULL,
			CONSOLE_TEXTMODE_BUFFER,
			NULL
			);
		//here we make a new stout, because we need
		//to have read access to the Console output buffer
		//via functions
		// ReadConsoleOutputAttribute()
		// ReadConsoleOutputCharacter()
		//or 
		// ReadConsoleOutput()
		SetConsoleActiveScreenBuffer(stout);

		//make a second (or rather 3rd...) buffer so we can
		//latter swap them out, reducing screen flashing.
		out_buffer = CreateConsoleScreenBuffer(
			GENERIC_READ | GENERIC_WRITE,
			0,//not shared with other apps
			NULL,
			CONSOLE_TEXTMODE_BUFFER,
			NULL
			);
		


		M_POS.X = 0;
		M_POS.Y = 0;
		numRead = 0;
		numEvents = 0;
		inputStat = nullptr;
	}

	HANDLE stout;
	HANDLE stin;
	HANDLE out_buffer;
	DWORD numRead, numEvents;
	COORD M_POS;
	INPUT_RECORD * inputStat;
};


void clearInputEvents(ioState&);
void getInputEvents(ioState&);

} }
