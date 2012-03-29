
#ifndef _NC_CONSOLE_AUX__
#define _NC_CONSOLE_AUX__
//here to make intellisence happier
#include "Console.h"


namespace nitrocorp { namespace console {
///////////////////////////////////////////////////////////////////////////////
//private methods which are required by usermode methods
///////////////////////////////////////////////////////////////////////////////
template<typename charT>
console<charT>::console()
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
template<typename charT>
charT* console<charT>::ParseEscape(charT* s)
{}
template <>
char* console<char>::ParseEscape <char> (char * s)
{
	char E[NUMBEROFESCAPE] = {'$', '#', '\n'};
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
					if(*s>='0' && *s <= '9')
						CurrentAttribute = (CurrentAttribute & ~(15<<i*4)) | *s - '0' << i*4; 
					else if(*s>='a' && *s <= 'f')
						CurrentAttribute = (CurrentAttribute & ~(15<<i*4)) | (*s - 'a' + 10) << i*4;
					else if(*s>='A' && *s <= 'F')
						CurrentAttribute = (CurrentAttribute & ~(15<<i*4)) | (*s - 'A' + 10) << i*4;
					else
						s--;
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
template <>
wchar_t* console<wchar_t>::ParseEscape <wchar_t> (wchar_t * s)
{
	wchar_t E[NUMBEROFESCAPE] = {L'$', L'#', L'\n'};
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
					if(*s>=L'0' && *s <= L'9')
						CurrentAttribute = (CurrentAttribute & ~(15<<i*4)) | *s - L'0' << i*4; 
					else if(*s>=L'a' && *s <= L'f')
						CurrentAttribute = (CurrentAttribute & ~(15<<i*4)) | (*s - L'a' + 10) << i*4;
					else if(*s>=L'A' && *s <= L'F')
						CurrentAttribute = (CurrentAttribute & ~(15<<i*4)) | (*s - L'A' + 10) << i*4;
					else
						s--;
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

//Build char struct
template <typename charT>
CHAR_INFO console<charT>::BuildCharInfo(charT c)
{}
template <> 
CHAR_INFO console<char>::BuildCharInfo <char> (char c)
{
	CHAR_INFO ci;
	ci.Attributes = CurrentAttribute;
	ci.Char.UnicodeChar = c;
	// this works because of the union, and the top half needs init to 0
	return ci;
}
template <>
CHAR_INFO console<wchar_t>::BuildCharInfo <wchar_t> (wchar_t c)
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