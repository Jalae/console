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


	
	SHORT X;
	SHORT Y;

public:
	ioState* ConsoleState;
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
		//do the same for second buffer
		if(SetConsoleScreenBufferSize(ConsoleState->out_buffer, ScreenCoord) == 0)
			throw (GetLastError());

		if(SetConsoleWindowInfo(ConsoleState->stout, TRUE, &WindowRect) == 0)
			throw (GetLastError());
		if(SetConsoleWindowInfo(ConsoleState->out_buffer, TRUE, &WindowRect) == 0)
			throw (GetLastError());

		//and now that the info thinks it is smaller than we want increase the
		//buffer to be the size of the console
		ScreenCoord.X = x;
		ScreenCoord.Y = y;
		if(SetConsoleScreenBufferSize(ConsoleState->stout, ScreenCoord) == 0)
			throw(GetLastError());
		if(SetConsoleScreenBufferSize(ConsoleState->out_buffer, ScreenCoord) == 0)
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

	void SwapDisplayBuffers()
	{
		//swap the names
		ToggleDisplayBuffers();
		//now we need to make the secondarybuffer the same as what is being displayed.
		//this can be done in 1 of 2 ways.
		// 1: COPY EVERYTHING. this may or may not be fast in runtime. i have no idea because i currently don't know if writes to a not displaying buffer suffers display lag
		// 2: COPY CHANGES. this requires the code to walk through and compare every element, fairly computation intensive, but may be considerably faster if there is lag
		//method 1
		CHAR_INFO buffer[X*Y];
		COORD size = {{X}{Y}};
		COORD pos = {{0}{0}};
		SMALL_RECT* rec = {{0}{0}{X}{Y}};
		ReadConsoleOutput(
						stout,
						buffer,
						size,
						pos,
						&rec
					);
		WriteConsoleOutput(
						out_buffer,
						buffer,
						size,
						pos,
						&rec
					);

	}

	void ToggleDisplayBuffers()
	{
		HANDLE temp;
		temp = ConsoleState->stout;
		ConsoleState->stout = ConsoleState->out_buffer;
		ConsoleState->out_buffer = temp;
		//make the active buffer the previous secondarybuffer
		//NOTE: these lines may need changed, if switching buffers suffers from console lag for every charactor.
		SetConsoleActiveScreenBuffer(ConsoleState->stout);
	}



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
