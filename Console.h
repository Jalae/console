#include <Windows.h>
#include "ioState.h"

namespace _console
{
template <typename charT>
class console
{
private:
	console()
	{
		ConsoleState = nullptr;
		X=0;
		Y=0;
	}

	static console InnerConsole;


	ioState* ConsoleState;
	SHORT X;
	SHORT Y;

public:
	typedef charT char_type;
	~console()
	{
		delete ConsoleState;
	}

//Singleton getters
	static console* GetConsole()
	{
		if(!InnerConsole.ConsoleState)
		{
			InnerConsole.ConsoleState = new ioState;
			InnerConsole.SetupConsoleSize(0,0);
		}
		return &InnerConsole;
	}
	static console* GetConsole(SHORT x, SHORT y)
	{
		if(!InnerConsole.ConsoleState)
		{
			InnerConsole.ConsoleState = new ioState;
		}
		SetupConsoleSize(x,y);
		return &InnerConsole;
	}

//Output Functions


//Input Functions


//Console Modifiers

	void SetupConsoleSize(SHORT x, SHORT y)
	{
		SMALL_RECT WindowRect;
		COORD ScreenCoord;

		//if the size is the same.... well we're done.
		if(x==X && y==Y)
			return;

		ScreenCoord = GetLargestConsoleWindowSize(ConsoleState->stout);
		
		//we want to make the screen no larger than the max size
		//TODO: check for minimum
		x = (SHORT) (min(x, ScreenCoord.X));
		y = (SHORT) (min(y, ScreenCoord.Y));
		COORD fontSize = GetFontSize();
		x = (SHORT) (max(x, GetSystemMetrics(SM_CXMIN) / fontSize.X));
		y = (SHORT) (max(y, GetSystemMetrics(SM_CYMIN) / fontSize.Y));
		//make the rectangle 1 smaller than what we want so we can force the
		//scroll bars away
		WindowRect.Right = x - 1; 
		WindowRect.Bottom = y - 1;
		WindowRect.Left = WindowRect.Top = (SHORT) 0;
		if(SetConsoleScreenBufferSize(ConsoleState->stout, ScreenCoord) == 0)
			throw (GetLastError());
		if(SetConsoleWindowInfo(ConsoleState->stout, TRUE, &WindowRect) == 0)
			throw (GetLastError());

		//and now that the info thinks it is smaller than we want increase the
		//buffer to be the size of the console
		ScreenCoord.X = x;
		ScreenCoord.Y = y;
		if(SetConsoleScreenBufferSize(ConsoleState->stout, ScreenCoord) == 0)
			throw(GetLastError());

		this->X = x;//make the size variables the size of the buffers
		this->Y = y;

	}

	void HideCursor()
	{
		//hide cursor
		CONSOLE_CURSOR_INFO info;
		GetConsoleCursorInfo(ConsoleState->stout,&info);
		info.bVisible=false;
		SetConsoleCursorInfo(ConsoleState->stout,&info);
	}
	void ShowCursor()
	{
		//hide cursor
		CONSOLE_CURSOR_INFO info;
		GetConsoleCursorInfo(ConsoleState->stout,&info);
		info.bVisible=true;
		SetConsoleCursorInfo(ConsoleState->stout,&info);
	}

	void DisableWraping()
	{
		DWORD mode;
		GetConsoleMode(ConsoleState->stout, &mode);
		mode &= ~ENABLE_WRAP_AT_EOL_OUTPUT;
		SetConsoleMode(ConsoleState->stout, mode);
	}
	void EnableWraping()
	{
		DWORD mode;
		GetConsoleMode(gameState.stout, &mode);
		mode |= ENABLE_WRAP_AT_EOL_OUTPUT;
		SetConsoleMode(gameState.stout, mode);
	}

	void SetFontSize(SHORT x, SHORT y)
	{
		CONSOLE_FONT_INFOEX fn;
		fn.cbSize = sizeof( CONSOLE_FONT_INFOEX );
		GetCurrentConsoleFontEx(ConsoleState->stout, false, &fn);
		fn.dwFontSize.X = x;
		fn.dwFontSize.Y = y;
		SetCurrentConsoleFontEx(ConsoleState->stout, false, &fn);
	}
	


//AUX Functions


	COORD GetFontSize()
	{
		CONSOLE_FONT_INFOEX fn;
		fn.cbSize = sizeof( CONSOLE_FONT_INFOEX );
		GetCurrentConsoleFontEx(ConsoleState->stout, false, &fn);
		return fn.dwFontSize;
	}


};

bool operator== (CHAR_INFO& a, CHAR_INFO& b)
{
	return true;
}

template<typename charT> console<charT> console<charT>::InnerConsole;
typedef console<char> console_;
typedef console<wchar_t> console_w;

}
