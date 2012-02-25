
#ifndef _1ELEMENT2RULE_
#define _1ELEMENT2RULE_

#define _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAPALLOC
#include <stdlib.h>
#include <crtdbg.h>


#ifdef _DEBUG
  #define DEBUG_CLIENTBLOCK   new( _CLIENT_BLOCK, __FILE__, __LINE__)
#else
  #define DEBUG_CLIENTBLOCK
#endif // _DEBUG
#ifdef _DEBUG
#define new DEBUG_CLIENTBLOCK
#endif
 

#include "region.h"
#include "ioState.h"


#define IS_IMAGE 5
#define IS_BUTTON 10
#define IS_TEXT 15
#define IS_PLAYER 20
#define IS_CARD 25
#define IS_DECK 30



class element
{
protected:
	point pos;
	bool dirty;

public:
	region r;
	ioState * gameState;
	bool clicked;

	element(point const & p1, point const & p2, ioState * gs):pos(p1),r(p1, p2), dirty(true), gameState(gs),clicked(false)
	{}
	element(point const & p, int const x, int const y, ioState * gs):pos(p), r(p, x, y), dirty(true), gameState(gs),clicked(false)
	{}
	virtual ~element(){}
	//anything that calls the draw functions needs to set adjacent
	//elements to be dirty.

	virtual void Draw() = 0; //this needs to set dirty false!
	virtual void unDraw() = 0; //needs to clear this elements region of it's image.
	virtual int ofType() = 0; //returns the type of element

	bool isDirty()
	{return dirty;}

	void makeDirty()
	{dirty = true;}

	//these auto call undraw and draw, calling things
	//should first mark dirty the things it was touching for redraw.
	void moveTo(point const p);

	void move(int const x, int const y);

	bool onMouseClick();
};

#endif
