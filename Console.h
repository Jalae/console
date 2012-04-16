#include <Windows.h>

#include "ioState.h"


#ifndef _NC_CONSOLE__
#define _NC_CONSOLE__


namespace nitrocorp { namespace console {
	//put these somewhere
COORD const ORIGIN = {0,0};
COORD const ONE_COORD = {1,1};
SMALL_RECT const ZERO_RECT = {0,0,0,0};
SMALL_RECT const ONE_RECT = {0,0,1,1};
size_t const NUMBEROFESCAPE = 3;
// we'll hold off on these
/*
#define RED_FG_ = "$C"
#define GREEN_FG_ = "$A"
*/

template <typename charT>
class console;

template <typename charT>
class console_streambuf : public std::basic_streambuf<charT>
{
protected:
	console<charT> * _console;

public:
	console_streambuf( console<charT> * _console ) : _console( _console )
	{}

	int_type overflow(int_type ch)
	{
		if( traits_type::eq_int_type(ch, traits_type::eof()) != true )
		{
			_console->Write( ch );
		}
		
		return traits_type::not_eof(ch);
	}
};

template <typename charT>
class console : public std::basic_ostream<charT>
{
private:
	static console InnerConsole;
	
	console();
	~console();

	ioState* ConsoleState;
	COORD BufferSize;
	COORD vCursorPos;
	WORD CurrentAttribute;
	console_streambuf<charT> streambuf;
	charT escapeMode;
	static charT const FG_ESCAPE;//$
	static charT const BG_ESCAPE;//#
	static charT const NL_ESCAPE;
	static charT const CR_ESCAPE;

	//These functions look at the given charactor and determine if it is
	//the start of an escape sequence. if it is it changes the attribute
	//for that escape sequence. 
	charT ParseEscape(charT s);
	CHAR_INFO BuildCharInfo(charT c);
	
	void SwapDisplayBuffers();
	void ToggleDisplayBuffers();
	typedef charT char_type;

public:



//Singleton getters
	static console* GetConsole()
	{
		InnerConsole.SetConsoleSize(0,0);
		return &InnerConsole;
	}
	static console* GetConsole(SHORT x, SHORT y)
	{
		SetConsoleSize(x,y);	
		return &InnerConsole;
	}

//Output Functions
	void Draw();

	void Write(charT ch);

//Input Functions


//Console Modifiers
	void SetConsoleSize(COORD size)
	{
		SetConsoleSize(size.X, size.Y);
	}
	void SetConsoleSize(SHORT x, SHORT y)
	{
		SMALL_RECT WindowRect;
		COORD ScreenCoord;

		//if the size is the same.... well we're done.
		if(x==BufferSize.X && y==BufferSize.Y)
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

		BufferSize.X = x;//make the size variables the size of the buffers
		BufferSize.Y = y;
	
	}

	void HideCursor()
	{
		//hide cursor
		CONSOLE_CURSOR_INFO info;
		GetConsoleCursorInfo(ConsoleState->stout,&info);
		info.bVisible=false;
		SetConsoleCursorInfo(ConsoleState->stout,&info);
		SetConsoleCursorInfo(ConsoleState->out_buffer,&info);
	}
	void ShowCursor()
	{
		//hide cursor
		CONSOLE_CURSOR_INFO info;
		GetConsoleCursorInfo(ConsoleState->stout,&info);
		info.bVisible=true;
		SetConsoleCursorInfo(ConsoleState->stout,&info);
		SetConsoleCursorInfo(ConsoleState->out_buffer,&info);
	}
	void DisableWraping()
	{
		DWORD mode;
		GetConsoleMode(ConsoleState->stout, &mode);
		mode &= ~ENABLE_WRAP_AT_EOL_OUTPUT;
		SetConsoleMode(ConsoleState->stout, mode);
		SetConsoleMode(ConsoleState->out_buffer, mode);
	}
	void EnableWraping()
	{
		DWORD mode;
		GetConsoleMode(ConsoleState->stout, &mode);
		mode |= ENABLE_WRAP_AT_EOL_OUTPUT;
		SetConsoleMode(ConsoleState->stout, mode);
		SetConsoleMode(ConsoleState->out_buffer, mode);
	}

	void SetFontSize(SHORT x, SHORT y)
	{
		CONSOLE_FONT_INFOEX fn;
		fn.cbSize = sizeof( CONSOLE_FONT_INFOEX );
		GetCurrentConsoleFontEx(ConsoleState->stout, false, &fn);
		fn.dwFontSize.X = x;
		fn.dwFontSize.Y = y;
		SetCurrentConsoleFontEx(ConsoleState->stout, false, &fn);
		SetCurrentConsoleFontEx(ConsoleState->out_buffer, false, &fn)
	}
	


//AUX Functions




	COORD GetFontSize()
	{
		CONSOLE_FONT_INFOEX fn;
		fn.cbSize = sizeof( CONSOLE_FONT_INFOEX );
		GetCurrentConsoleFontEx(ConsoleState->stout, false, &fn);
		return fn.dwFontSize;
	}

//Input

	bool WaitForInput(DWORD timeout = 0)
	{
		return WAIT_OBJECT_0 == WaitForSingleObject( ConsoleState->stin, timeout );
	}
};

/*
bool operator== (CHAR_INFO& a, CHAR_INFO& b)
{
	return true;
}
*/
template<typename charT> console<charT> console<charT>::InnerConsole;
typedef console<char> console_;
typedef console<wchar_t> console_w;
typedef console<TCHAR> console_T;

}}

//za definitions
#include "Console_Aux.h"
#include "Console_InOut.h"

#endif



