#ifndef _TEHPLAYERS_
#define _TEHPLAYERS_


#include "card.h"
#include "button.h"
#include "image.h"
//#include "element.h"

int const X_IF_VERT(17);
int const Y_IF_VERT(39);
int const X_IF_HORZ(77);
int const Y_IF_HORZ(17);



class player:public element
{
	
protected:
	card ** hand;
	size_t cardCount;
	bool horz, BR;
	void removeCard(size_t index);
	//returns number of cards able to play
	size_t decodeADF(card*(*ADF)[2], card::Rank * & playable, size_t defSize);
	size_t decodeADF(card*(*ADF)[2], int *& needToBeat);
	

public:
	player(bool h, bool br, point tl, ioState * gs);

	size_t GetCount()
	{
		return cardCount;
	}

	virtual ~player()
	{
	}	
	
	virtual int attackPhase(card*(*&)[2], card::Suit kozer, size_t defSize) = 0;
	virtual int defendPhase(card*(*&)[2], card::Suit kozer) = 0;
	
	virtual void Draw() = 0;
	virtual void unDraw() = 0;

	virtual void setCardPos() = 0;

	void addCard(card * c);
	
	int ofType()
	{
		return IS_PLAYER;
	}

};

class cPlayer:public player
{
	ascii_image * blank;

public:
	cPlayer(point p, bool horz, bool br, ioState * gs, ascii_image* blank);
	~cPlayer();

	int attackPhase(card * (*&ADF)[2], card::Suit kozer, size_t defSize);
	int defendPhase(card * (*&ADF)[2], card::Suit kozer);
	void Draw();
	void unDraw();

	void setCardPos()
	{

	}

};


class hPlayer:public player
{
protected:
	button Play;
	button Pass;
	button Swap;

public:
	int attackPhase(card * (*&ADF)[2], card::Suit kozer, size_t defSize);
	int defendPhase(card * (*&ADF)[2], card::Suit kozer);

	hPlayer(point p, ioState * gs);
	~hPlayer();

	void setCardPos();

	void Draw();
	void unDraw();
};

#endif
