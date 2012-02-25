// console.cpp : Defines the entry point for the console application.
//
#include "console.h"

void main ()

{

try{

		console_ * weee;
		weee = console_::GetConsole();


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