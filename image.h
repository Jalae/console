#ifndef _IMAGE_ASCII_
#define _IMAGE_ASCII_
#include "element.h"




class ascii_image:public element
{
	wchar_t const ** image; //nullptr terminated
public:
	ascii_image(wchar_t const ** str, point const p, int const y, ioState * gs):image(str), element(p, wcslen(str[0]), y, gs)
	{}
	//elements to be dirty.

	~ascii_image()
	{unDraw();}

	void Draw();
	void unDraw();

	int ofType()
	{return IS_IMAGE;}
};

#endif
