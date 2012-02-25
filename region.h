#ifndef _MAHREGION_
#define _MAHREGION_

#include <Windows.h>


class point
{
	
public:
	point(int const x, int const y)
	{
		m_p.X = x;
		m_p.Y = y;
	}
	point(COORD const & p)
	{
		m_p.X = p.X;
		m_p.Y = p.Y;
	}
	operator COORD () const
	{
		return m_p;
	}

	COORD m_p;

	//these only get the value to set it you must use m_p.X, or Y
	int X() const
	{
		return m_p.X; 
	}
	int Y() const
	{
		return m_p.Y;
	}
};


class region
{
protected:
	point m_pos_tl;
	point m_pos_br;
public:
	region(point const a, point const b):m_pos_tl(a), m_pos_br(b)
	{}

	region(region const &t):m_pos_tl(t.m_pos_tl), m_pos_br(t.m_pos_br)
	{}

	region(point const a, int const x = 1, int const y = 1):m_pos_tl(a), m_pos_br(a.X() + x - 1, a.Y() + y -1)
	{}

	bool contains(point const that)
	{
		return( that.X() >= m_pos_tl.X() && that.X() <= m_pos_br.X()  && that.Y() >= m_pos_tl.Y() && that.Y() <= m_pos_br.Y());
	}

	void operator=(point const that)
	{
		int difx = m_pos_tl.X() - m_pos_br.X();
		int dify = m_pos_tl.Y() - m_pos_br.Y();
		m_pos_tl = that;
		m_pos_br.m_p.X = that.X() - difx;
		m_pos_br.m_p.Y = that.Y() - dify;
	}

	void setRegion(point const a, point const b)
	{
		m_pos_tl = a;
		m_pos_br = b;
	}
};




#endif

