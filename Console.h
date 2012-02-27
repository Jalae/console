#include <Windows.h>
#include "ioState.h"


#ifndef _NC_CONSOLE__
#define _NC_CONSOLE__


namespace nitrocorp { namespace console {

COORD const ORIGIN = {0,0};
COORD const ONE_COORD = {1,1};
SMALL_RECT const ZERO_RECT = {0,0,0,0};
SMALL_RECT const ONE_RECT = {0,0,1,1};
size_t const NUMBEROFESCAPE = 2;

template <typename charT>
class console
{
private:
	static console InnerConsole;
	

	console()
	{
		ConsoleState = nullptr;
		ConsoleState = new ioState;
		HideCursor();
		BufferSize.X = 0;
		BufferSize.Y = 0;
		vCursorPos.X = 0;
		vCursorPos.Y = 0;
	}

	ioState* ConsoleState;

	COORD BufferSize;
	COORD vCursorPos;

	//These functions look at the given charactor and determine if it is
	//the start of an escape sequence. if it is it changes the attribute
	//for that escape sequence. 
	template <typename charT> charT* ParseEscape(charT * s, WORD& attrib){}//you lose sir
	template <> char* ParseEscape <char> (char * s, WORD& attrib)
	{
		char E[NUMBEROFESCAPE] = {'$', '#'};
		s++;
		for(size_t i = 0; i < NUMBEROFESCAPE; i++)
		{
			if(*s == E[i]) //we found our char
			{
				//increment the pointer. we are now looking for the same char again,
				//or a Hex char, where the bits represent from MSB to LSB -> intence, red, green, blue
				s++;
				if(*s == E[i])
					break; //we are escaping, to print the escape charactor
				switch(*s)
				{
				case '0':attrib = 0 << i*4; break;
				case '1':attrib = 1 << i*4; break;
				case '2':attrib = 2 << i*4; break;
				case '3':attrib = 3 << i*4; break;
				case '4':attrib = 4 << i*4; break;
				case '5':attrib = 5 << i*4; break;
				case '6':attrib = 6 << i*4; break;
				case '7':attrib = 7 << i*4; break;
				case '8':attrib = 8 << i*4; break;
				case '9':attrib = 9 << i*4; break;
				case 'a': case 'A':attrib = 10 << i*4; break;
				case 'b': case 'B':attrib = 11 << i*4; break;
				case 'c': case 'C':attrib = 12 << i*4; break;
				case 'd': case 'D':attrib = 13 << i*4; break;
				case 'e': case 'E':attrib = 14 << i*4; break;
				case 'f': case 'F':attrib = 15 << i*4; break;
				//cool huh?
				default:  s--; break;
				}
				s++;
				return s; //somehow have to check for the other escape 
			}

		}
		return s;
	}
	template <> wchar_t* ParseEscape <wchar_t> (wchar_t *s, WORD& attrib)
	{
		wchar_t E[NUMBEROFESCAPE] = {L'$', L'#'};
		s++;
		for(size_t i = 0; i < NUMBEROFESCAPE; i++)
		{
			if(*s == E[i]) //we found our char
			{
				//increment the pointer. we are now looking for the same char again,
				//or a Hex char, where the bits represent from MSB to LSB -> intence, red, green, blue
				s++;
				if(*s == E[i])
					break; //we are escaping, to print the escape charactor
				switch(*s)
				{
				case L'0':attrib = 0 << i*4; break;
				case L'1':attrib = 1 << i*4; break;
				case L'2':attrib = 2 << i*4; break;
				case L'3':attrib = 3 << i*4; break;
				case L'4':attrib = 4 << i*4; break;
				case L'5':attrib = 5 << i*4; break;
				case L'6':attrib = 6 << i*4; break;
				case L'7':attrib = 7 << i*4; break;
				case L'8':attrib = 8 << i*4; break;
				case L'9':attrib = 9 << i*4; break;
				case L'a': case L'A':attrib = 10 << i*4; break;
				case L'b': case L'B':attrib = 11 << i*4; break;
				case L'c': case L'C':attrib = 12 << i*4; break;
				case L'd': case L'D':attrib = 13 << i*4; break;
				case L'e': case L'E':attrib = 14 << i*4; break;
				case L'f': case L'F':attrib = 15 << i*4; break;
				//cool huh?
				default:  s--; break;
				}
				s++;
				return s; //somehow have to check for the other escape 
			}

		}
		return s;
	}

	template <typename charT> CHAR_INFO BuildCharInfo(charT c, WORD attrib){}//you get nothing
	template <> CHAR_INFO BuildCharInfo<char>(char c, WORD attrib)
	{
		CHAR_INFO ci;
		ci.Attributes = attrib;
		ci.Char.AsciiChar = c;
		return ci;
	}
	template <> CHAR_INFO BuildCharInfo<wchar_t>(wchar_t c, WORD attrib)
	{
		CHAR_INFO ci;
		ci.Attributes = attrib;
		ci.Char.UnicodeChar = c;
		return ci;
	}

	void SwapDisplayBuffers()
	{
		//swap the names
		ToggleDisplayBuffers();
		//now we need to make the secondarybuffer the same as what is being displayed.
		//this can be done in 1 of 2 ways.
		// 1: COPY EVERYTHING. this may or may not be fast in runtime. i have no idea because i currently don't know if writes to a not displaying buffer suffers display lag
		// 2: COPY CHANGES. this requires the code to walk through and compare every element, fairly computation intensive, but may be considerably faster if there is lag
		//method 1
		CHAR_INFO * buffer  = new CHAR_INFO [BufferSize.X * BufferSize.Y];
		COORD size = BufferSize;
		SMALL_RECT rec = {0,0,BufferSize.X-1,BufferSize.Y-1};
		bool success = ReadConsoleOutput(
						ConsoleState->stout,
						buffer,
						size,
						ORIGIN,
						&rec
					);
		if(!success)
		{
			//break here;
		}
		WriteConsoleOutput(
						ConsoleState->out_buffer,
						buffer,
						size,
						ORIGIN,
						&rec
					);
		delete[] buffer;
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



public:

	typedef charT char_type;
	~console()
	{
		delete ConsoleState;
	}

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
	void Draw()
	{
		SwapDisplayBuffers();
	}

	void Write(charT* str)
	{
			CHAR_INFO temp;
			WORD attrib=8;
			while(*str)
			{
				//figureout if str* is an escape sequence-|
				//if it is change attrib to match---------|-1 function 
				ParseEscape(str, attrib);
				//build temp
				temp = BuildCharInfo(*str, attrib);
				SMALL_RECT rec = {vCursorPos.X,vCursorPos.Y,vCursorPos.X,vCursorPos.Y};
				WriteConsoleOutput(
						ConsoleState->out_buffer,
						&temp,
						ONE_COORD,
						ORIGIN,
						&rec
					);
				vCursorPos.X++;
				str++;
			}

	}

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

#endif

