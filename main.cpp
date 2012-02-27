// console.cpp : Defines the entry point for the console application.
//
#include "console.h"
#include <iostream>
void main ()

{

try{
	using namespace _console;
	{
		const int X(200), Y(50); 
		console_w * c;
		c = console_w::GetConsole();
		c->SetConsoleSize(X,Y);

		//test speed of toggling Display buffers
		CHAR_INFO bufferA[X*Y];
		CHAR_INFO bufferB[X*Y];
		for(size_t i = 0; i < X; i++)
		{
			for(size_t j = 0; j < Y; j++)
			{
				bufferA[j*X + i].Char.UnicodeChar =  15000+j*X+i;
				bufferA[j*X + i].Attributes = 7;
				bufferB[j*X + i].Char.UnicodeChar =  8000+j*X+i;
				bufferB[j*X + i].Attributes = 7;
			}
		}
		COORD size = {{X},{Y}};
		COORD pos = {{0},{0}};
		SMALL_RECT rec = {{0},{0},{X},{Y}};
		if (0==WriteConsoleOutput(c->ConsoleState->stout, bufferA, size, pos, &rec ))
			throw(GetLastError());
		if (0==WriteConsoleOutput(c->ConsoleState->out_buffer, bufferB, size, pos, &rec ))
			throw(GetLastError());
		while(1)
		{
			c->ToggleDisplayBuffers();
			std::system("pause");
		}


	}

	}catch(DWORD e)
	{

		wchar_t errorMsg[256];
		FormatMessage(
			FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			e,
			0,
			errorMsg,
			256,
			NULL
			);
			throw;
	}

}