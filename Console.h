#include <Windows.h>
#include "ioState.h"


#ifndef _NC_CONSOLE__
#define _NC_CONSOLE__


namespace nitrocorp { namespace console {

COORD const ORIGIN = {0,0};
COORD const ONE_COORD = {1,1};
SMALL_RECT const ZERO_RECT = {0,0,0,0};
SMALL_RECT const ONE_RECT = {0,0,1,1};
size_t const NUMBEROFESCAPE = 3;

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
		CurrentAttribute = 15;//black background white text
	}

	ioState* ConsoleState;

	COORD BufferSize;
	COORD vCursorPos;
	WORD CurrentAttribute;

	//These functions look at the given charactor and determine if it is
	//the start of an escape sequence. if it is it changes the attribute
	//for that escape sequence. 
	template <typename charT> charT* ParseEscape(charT * s){}//you lose sir
	template <> char* ParseEscape <char> (char * s)
	{
		char E[NUMBEROFESCAPE] = {'$', '#', '\n'};
//		s++;
		for(size_t i = 0; i < NUMBEROFESCAPE; i++)
		{
			if(*s == E[i]) //we found our char
			{
				//increment the pointer. we are now looking for the same char again,
				//or a Hex char, where the bits represent from MSB to LSB -> intence, red, green, blue
				s++;

				if(*s == E[i])
					break; //we are escaping, to print the escape charactor. this isn't finding one

				switch(i)
				{
					case 0: case 1:
					switch(*s)
					{
						case '0':CurrentAttribute = (CurrentAttribute & ~(15<<i*4)) | 0 << i*4; break;
						case '1':CurrentAttribute = (CurrentAttribute & ~(15<<i*4)) | 1 << i*4; break;
						case '2':CurrentAttribute = (CurrentAttribute & ~(15<<i*4)) | 2 << i*4; break;
						case '3':CurrentAttribute = (CurrentAttribute & ~(15<<i*4)) | 3 << i*4; break;
						case '4':CurrentAttribute = (CurrentAttribute & ~(15<<i*4)) | 4 << i*4; break;
						case '5':CurrentAttribute = (CurrentAttribute & ~(15<<i*4)) | 5 << i*4; break;
						case '6':CurrentAttribute = (CurrentAttribute & ~(15<<i*4)) | 6 << i*4; break;
						case '7':CurrentAttribute = (CurrentAttribute & ~(15<<i*4)) | 7 << i*4; break;
						case '8':CurrentAttribute = (CurrentAttribute & ~(15<<i*4)) | 8 << i*4; break;
						case '9':CurrentAttribute = (CurrentAttribute & ~(15<<i*4)) | 9 << i*4; break;
						case 'a': case 'A':CurrentAttribute = (CurrentAttribute & ~(15<<i*4)) | 10 << i*4; break;
						case 'b': case 'B':CurrentAttribute = (CurrentAttribute & ~(15<<i*4)) | 11 << i*4; break;
						case 'c': case 'C':CurrentAttribute = (CurrentAttribute & ~(15<<i*4)) | 12 << i*4; break;
						case 'd': case 'D':CurrentAttribute = (CurrentAttribute & ~(15<<i*4)) | 13 << i*4; break;
						case 'e': case 'E':CurrentAttribute = (CurrentAttribute & ~(15<<i*4)) | 14 << i*4; break;
						case 'f': case 'F':CurrentAttribute = (CurrentAttribute & ~(15<<i*4)) | 15 << i*4; break;
						//cool huh?
						default:  s--; break;
					}
					s++;
					break;

					case 2: // \n
						vCursorPos.Y = vCursorPos.Y+1;
						vCursorPos.X = 0;
						break;
					default: ;
				}
				i = -1; //we found one. start looking again

			}

		}
		return s;
	}
	template <> wchar_t* ParseEscape <wchar_t> (wchar_t * s)
	{
		wchar_t E[NUMBEROFESCAPE] = {L'$', L'#', L'\n'};
//		s++;
		for(size_t i = 0; i < NUMBEROFESCAPE; i++)
		{
			if(*s == E[i]) //we found our char
			{
				//increment the pointer. we are now looking for the same char again,
				//or a Hex char, where the bits represent from MSB to LSB -> intence, red, green, blue
				s++;

				if(*s == E[i])
					break; //we are escaping, to print the escape charactor. this isn't finding one

				switch(i)
				{
					case 0: case 1:
					switch(*s)
					{
						case L'0':CurrentAttribute = (CurrentAttribute & ~(15<<i*4)) | 0 << i*4; break;
						case L'1':CurrentAttribute = (CurrentAttribute & ~(15<<i*4)) | 1 << i*4; break;
						case L'2':CurrentAttribute = (CurrentAttribute & ~(15<<i*4)) | 2 << i*4; break;
						case L'3':CurrentAttribute = (CurrentAttribute & ~(15<<i*4)) | 3 << i*4; break;
						case L'4':CurrentAttribute = (CurrentAttribute & ~(15<<i*4)) | 4 << i*4; break;
						case L'5':CurrentAttribute = (CurrentAttribute & ~(15<<i*4)) | 5 << i*4; break;
						case L'6':CurrentAttribute = (CurrentAttribute & ~(15<<i*4)) | 6 << i*4; break;
						case L'7':CurrentAttribute = (CurrentAttribute & ~(15<<i*4)) | 7 << i*4; break;
						case L'8':CurrentAttribute = (CurrentAttribute & ~(15<<i*4)) | 8 << i*4; break;
						case L'9':CurrentAttribute = (CurrentAttribute & ~(15<<i*4)) | 9 << i*4; break;
						case L'a': case L'A':CurrentAttribute = (CurrentAttribute & ~(15<<i*4)) | 10 << i*4; break;
						case L'b': case L'B':CurrentAttribute = (CurrentAttribute & ~(15<<i*4)) | 11 << i*4; break;
						case L'c': case L'C':CurrentAttribute = (CurrentAttribute & ~(15<<i*4)) | 12 << i*4; break;
						case L'd': case L'D':CurrentAttribute = (CurrentAttribute & ~(15<<i*4)) | 13 << i*4; break;
						case L'e': case L'E':CurrentAttribute = (CurrentAttribute & ~(15<<i*4)) | 14 << i*4; break;
						case L'f': case L'F':CurrentAttribute = (CurrentAttribute & ~(15<<i*4)) | 15 << i*4; break;
						//cool huh?
						default:  s--; break;
					}
					s++;
					break;

					case 2: // \n
						vCursorPos.Y = vCursorPos.Y+1;
						vCursorPos.X = 0;
						break;
					default: ;
				}
				i = -1; //we found one. start looking again

			}

		}
		return s;
	}

	template <typename charT> CHAR_INFO BuildCharInfo(charT c){}//you get nothing
	template <> CHAR_INFO BuildCharInfo<char>(char c)
	{
		CHAR_INFO ci;
		ci.Attributes = CurrentAttribute;
		ci.Char.AsciiChar = c;
		return ci;
	}
	template <> CHAR_INFO BuildCharInfo<wchar_t>(wchar_t c)
	{
		CHAR_INFO ci;
		ci.Attributes = CurrentAttribute;
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
		ReadConsoleOutput(
						ConsoleState->stout,
						buffer,
						size,
						ORIGIN,
						&rec
					);

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
			while(*str)
			{
				//figureout if str* is an escape sequence-|
				//if it is change attrib to match---------|-1 function 
				str = ParseEscape(str);
				if(!*str)
					return; //we don't want to write a 0
				//build temp
				temp = BuildCharInfo(*str);
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

