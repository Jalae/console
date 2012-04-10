/******************************************************************************
*Author:		Kehnin Dyer
*File name:		Console_Object.h
*Date Created:	2012/04/09
*Modifed:		
*					:see git commits for more details
******************************************************************************/



#include "Console.h"

#ifndef _NC_CONSOLE_OBJECT__
#define _NC_CONSOLE_OBJECT__

namespace nitrocorp{ namespace console {
	enum object_state
{
	//Add more up to 1 << 31 here;
	EMPTY =		1 << 0,
	CLICKED =	1 << 1, //a 0 at any possition means the opposite
	VISIBLE =	1 << 2
	// etc
};

class absObject
{
	COORD m_Position;
	SMALL_RECT m_Area; //hmm...
	
	//assigned with object_states
	WORD m_state; 

public:
	// this stuff will need moved to it's own file
	absObject():m_Position(ORIGIN), m_Area(ZERO_RECT), m_state(object_state::EMPTY)
	{}
	virtual ~absObject()
	{}
	//no touchy these functions

	void setSize(size_t const X, size_t const Y)
	{
		m_Area.Bottom = m_Area.Top + Y;
		m_Area.Right = m_Area.Left + X;
	}

	void getSize(size_t & X, size_t & Y) const
	{
		Y = m_Area.Bottom - m_Area.Top;
		X = m_Area.Right - m_Area.Left;
	}
	COORD getSize() const
	{
		COORD temp = {(m_Area.Bottom - m_Area.Top), (m_Area.Right - m_Area.Left)};
		return temp;
	}
	void addState(object_state a)
	{
		m_state|=a;
	}
	void removeState(object_state a)
	{
		m_state&=~a;
	}



};




}}
#endif