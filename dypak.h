

#ifndef _DYPAK_
#define _DYPAK_

#include "deck.h"
#include "player.h"
#include "ioState.h"
#include "image.h"

class dypak
{
protected:
	static wchar_t const * game_title[];
	static wchar_t const * cardblank[];

	//this is card*(*)[n], don't know it?
	card*(* attackDefendField)[2];
	point ADFpos;
	card::Suit kozer;
	ascii_image blank;

	bool print_title();
	bool print_menu();
	void play();
	void drawStatus(size_t, size_t, size_t);

	void clearscreen();

	int turn(player ** players, size_t defIndex);
	void deal(player & p, deck & d);

	void setGameSize(SHORT x, SHORT y);
	ioState gameState;

public:


	dypak();
	~dypak();

	void startGame()
	{
		while(print_title())
		{
			print_menu();
		}
	}
};










#endif