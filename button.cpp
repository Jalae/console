#include "button.h"
#include <iostream>

button::button(wchar_t const * str, point const & p, ioState * gs):
													element(p, wcslen(str), 1, gs),
													len(wcslen(str)),
													m_str(0)
{ 
	m_str = new wchar_t [len+1];
	wcscpy(m_str, str);
}

button::~button()
{
	unDraw();
	delete[] m_str;
	m_str = nullptr;
}

void button::Draw()
{
	COORD temp;
	{
		CONSOLE_SCREEN_BUFFER_INFO ScreenBufferInfo = { 0 };
		GetConsoleScreenBufferInfo(gameState->stout, &ScreenBufferInfo );
		temp.X = ScreenBufferInfo.dwCursorPosition.X;
		temp.Y = ScreenBufferInfo.dwCursorPosition.Y;
	}
	SetConsoleCursorPosition(gameState->stout, pos);
	std::wcout << m_str;
	dirty = false;
	SetConsoleCursorPosition(gameState->stout, temp);
}

void button::unDraw()
{
	COORD temp;
	{
		CONSOLE_SCREEN_BUFFER_INFO ScreenBufferInfo = { 0 };
		GetConsoleScreenBufferInfo( gameState->stout, &ScreenBufferInfo );
		temp.X = ScreenBufferInfo.dwCursorPosition.X;
		temp.Y = ScreenBufferInfo.dwCursorPosition.Y;
	}
	SetConsoleCursorPosition(gameState->stout, pos);
	for(int i(len); i > 0; i--)
		std::wcout << " ";
	dirty = true;
	SetConsoleCursorPosition(gameState->stout, temp);
}