
#ifndef _NC_CONSOLE_AUX__
#define _NC_CONSOLE_AUX__
//here to make intellisence happier
#include "Console.h"
#include "cstdint"


namespace nitrocorp { namespace console {
///////////////////////////////////////////////////////////////////////////////
//private methods which are required by usermode methods
///////////////////////////////////////////////////////////////////////////////
template<typename charT>
console<charT>::console()
	: streambuf(this), std::basic_ostream<charT>( &streambuf ), escapeMode(0)
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

//console destructor
template<typename charT>
console<charT>::~console()
	{
		delete ConsoleState;
	}

//parse functions

int8_t hex2num(char ch)
{
	int8_t num = -1;
	if(ch >= '0' && ch <= '9')
		num = ch-'0';
	else if(ch >= 'a' && ch <= 'f')
		num = ch-'a';
	else if( ch >= 'A' && ch <= 'F' )
		num = ch-'A';
	return num;
}

int8_t hex2num(wchar_t ch)
{
	int8_t num = -1;
	if(ch >= L'0' && ch <= L'9')
		num = ch-L'0';
	else if(ch >= L'a' && ch <= L'f')
		num = ch-L'a'+10;
	else if( ch >= L'A' && ch <= L'F' )
		num = ch-L'A'+10;
	return num;
}

template <> char const console<char>::FG_ESCAPE = '$';//$
template <> char const console<char>::BG_ESCAPE = '#';//#
template <> char const console<char>::NL_ESCAPE = '\n';
template <> char const console<char>::CR_ESCAPE = '\r';

template <> wchar_t const console<wchar_t>::FG_ESCAPE = L'$';//$
template <> wchar_t const console<wchar_t>::BG_ESCAPE = L'#';//#
template <> wchar_t const console<wchar_t>::NL_ESCAPE = L'\n';
template <> wchar_t const console<wchar_t>::CR_ESCAPE = L'\r';

template <typename charT>
charT console<charT>::ParseEscape(charT ch)
{
	if( escapeMode == BG_ESCAPE )
	{
		int8_t num = hex2num( ch );
		CurrentAttribute = (CurrentAttribute & ~(15<<4)) | num << 4;
		escapeMode = 0;
	}
	else if( escapeMode == FG_ESCAPE )
	{
		int8_t num = hex2num( ch );
		CurrentAttribute = (CurrentAttribute & ~(15<<0)) | num << 0;
		escapeMode = 0;
	}
	else
	{
		if( ch == BG_ESCAPE )
		{
			escapeMode = BG_ESCAPE;
		}
		else if( ch == FG_ESCAPE )
		{
			escapeMode = FG_ESCAPE;
		}
		else if( ch == NL_ESCAPE )
		{
			vCursorPos.Y = vCursorPos.Y+1;
			vCursorPos.X = 0;
		}
		else if( ch == CR_ESCAPE )
		{
		}	
		else
		{
			return ch;
		}
	}
	return 0;
}

//Build char struct
template <typename charT>
CHAR_INFO console<charT>::BuildCharInfo(charT c)
{}
template <> 
CHAR_INFO console<char>::BuildCharInfo(char c)
{
	CHAR_INFO ci;
	ci.Attributes = CurrentAttribute;
	ci.Char.UnicodeChar = c;
	// this works because of the union, and the top half needs init to 0
	return ci;
}
template <>
CHAR_INFO console<wchar_t>::BuildCharInfo(wchar_t c)
	{
		CHAR_INFO ci;
		ci.Attributes = CurrentAttribute;
		ci.Char.UnicodeChar = c;
		return ci;
	}

//swaps the active and buffer display buffers
template <typename charT>
void console<charT>::SwapDisplayBuffers()
{
	//swap the names
	ToggleDisplayBuffers();
	//now we need to make the secondarybuffer the same as what is being displayed.
	//this can be done in 1 of 2 ways.
	// 1: COPY EVERYTHING. this may or may not be fast in runtime. i have no idea because i currently don't know if writes to a not displaying buffer suffers display lag
	//Edit: method one works plenty fast.
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

//Toggle the display buffers
template <typename charT>
void console<charT>::ToggleDisplayBuffers()
{
	HANDLE temp;
	temp = ConsoleState->stout;
	ConsoleState->stout = ConsoleState->out_buffer;
	ConsoleState->out_buffer = temp;
	//make the active buffer the previous secondarybuffer
	//NOTE: these lines may need changed, if switching buffers suffers from console lag for every charactor.
	//^left for historical reasons. In practice it works plenty fast
	SetConsoleActiveScreenBuffer(ConsoleState->stout);
}

//done



}}
#endif