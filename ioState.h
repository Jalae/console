#include <Windows.h>
#include <iostream>

#ifndef IOSTATE_

#define IOSTATE_


class ioState
{
public:
	ioState()
	{
		stout= GetStdHandle(STD_OUTPUT_HANDLE);
		stin = GetStdHandle(STD_INPUT_HANDLE);
		M_POS.X = 0;
		M_POS.Y = 0;
		numRead = 0;
		numEvents = 0;
		inputStat = nullptr;
	}

	HANDLE stout;
	HANDLE stin;
	DWORD numRead, numEvents;
	COORD M_POS;
	INPUT_RECORD * inputStat;
};


void clearInputEvents(ioState&);
void getInputEvents(ioState&);


#endif