// console.cpp : Defines the entry point for the console application.
//
#include "console.h"
#include <iostream>
void main ()

{

try{
	using namespace nitrocorp::console;
	{
		COORD size = {{4},{4}};
		console_w * c;
		c = console_w::GetConsole();
		c->SetConsoleSize(size);

		while(1)
		{
			c->Draw();
		}
		system("PAUSE");

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