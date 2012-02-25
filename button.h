
#ifndef _MAHBUTTON_
#define _MAHBUTTON_
#include "element.h"

//#include "region.h"

class button:public element
{
	wchar_t * m_str;
	size_t len;

public:

	
	button(wchar_t const * str, point const & p, ioState * gs);
	~button();

	void Draw();
	void unDraw();



	int ofType()
	{ return IS_BUTTON;}

};

#endif