#include "image.h"


void ascii_image::Draw()
{
	COORD temp;
	int i(0);
	{
		CONSOLE_SCREEN_BUFFER_INFO ScreenBufferInfo = { 0 };
		GetConsoleScreenBufferInfo(gameState->stout, &ScreenBufferInfo );
		temp.X = ScreenBufferInfo.dwCursorPosition.X;
		temp.Y = ScreenBufferInfo.dwCursorPosition.Y;
	}
	SetConsoleCursorPosition(gameState->stout, pos);
	while (image[i] != nullptr)
	{
		std::wcout << image[i++];
		SetConsoleCursorPosition(gameState->stout, point(pos.X(), pos.Y()+i));
	}
	dirty = false;
	SetConsoleCursorPosition(gameState->stout, temp);
}


void ascii_image::unDraw()
{
	COORD temp;
	int i(0);
	{
		CONSOLE_SCREEN_BUFFER_INFO ScreenBufferInfo = { 0 };
		GetConsoleScreenBufferInfo(gameState->stout, &ScreenBufferInfo );
		temp.X = ScreenBufferInfo.dwCursorPosition.X;
		temp.Y = ScreenBufferInfo.dwCursorPosition.Y;
	}
	SetConsoleCursorPosition(gameState->stout, pos);
	while (image[i] != nullptr)
	{
		for(int j(wcslen(image[0])); j>0 ; j--)
		{
			std::wcout << " ";
		}
		i++;
		SetConsoleCursorPosition(gameState->stout, point(pos.X(), pos.Y() +i));
	}
	SetConsoleCursorPosition(gameState->stout, temp);
}