
#include "player.h"
//#include "card.h"



player::player(bool h, bool br, point tl, ioState * gs)
		: hand(nullptr), horz(h), BR(br), element(tl,
		(h?X_IF_HORZ:X_IF_VERT), (h?Y_IF_HORZ:Y_IF_VERT), gs),
		cardCount(0)
	{}

void player::addCard(card * c)
{
	card** temp = nullptr;
	cardCount++;
	temp = new card * [cardCount];
	for(size_t i(0); i < (cardCount - 1); i++)
	{
		temp[i] = hand[i];
	}
	delete[] hand;//i am dumb...
	hand = temp;
	hand[cardCount-1]=c;
}


void player::removeCard(size_t index)
{
	if(index >= cardCount)
	{
		return;
	}
	card ** temp = new card * [cardCount-1];
	for(size_t k(0), j(0); k<cardCount; k++, j++)
	{
		if(index==j)
			j++;
		temp[k] = hand[j];
	}
	delete[] hand;
	hand = temp;
	cardCount--;
}

size_t player::decodeADF(card*(*ADF)[2], card::Rank * & playable, size_t defSize)
{
	size_t count(0);
	size_t const handMax(5);

		if(ADF == nullptr)
		{
			playable = new card::Rank [(card::ace - card::six)+1];
			card::Rank toadd(card::six);
			for(int i(0); toadd <= card::ace; i++, toadd++)
			{
				playable[i] = toadd;
			}
			return (handMax<defSize?handMax:defSize);
		}
		else
		{
			for(size_t i(0);i < 2;i++)
			{//i want to rewrite this so bad...
				for(size_t j(0); j < handMax && ADF[j][i] != nullptr; j++)
				{
					count++;
					card::Rank * temp = new card::Rank [count+1];
					for(size_t i(0); i < count - 1; i++)
					{
						temp[i] = playable[i];
					}
					delete[] playable;
					playable = temp;
					playable[count-1] = ADF[j][i]->GetRank();
					playable[count] = card::two;
					if(j == handMax-1 && i == 0)
					{
						delete[] playable;
						playable=nullptr;
						break;
					}
				}
				if(playable == nullptr)
				{
					break;
				}
			}
			size_t remaining(handMax);
			for(size_t i(0); ADF[i][0] && i < handMax; i++, remaining--);
			return (remaining<defSize?remaining:defSize);
		}
}


size_t player::decodeADF(card*(*ADF)[2], int *& needToBeat)
{
	size_t count(0);

	for(size_t j(0); j < 5 && ADF[j][0] != nullptr; j++)
	{
		if(ADF[j][1] == nullptr)
		{
			count++;

			int * temp = new int [count+1];
			for(size_t i(0); i < count - 1; i++)
			{
				temp[i] = needToBeat[i];
			}
			delete[] needToBeat;
			needToBeat = temp;
			needToBeat[count-1] = j;
			needToBeat[count] = -1;
		}

	}
	return count;
}



cPlayer::cPlayer(point p, bool horz, bool br, ioState * gs, ascii_image* bl):blank(bl),player(horz, br, p, gs)
{
}


cPlayer::~cPlayer()
{
	delete[] hand;
	hand = nullptr;
}


int cPlayer::attackPhase(card * (*&ADF)[2], card::Suit kozer, size_t defSize)
{
		Sleep(900);
size_t const handMax(5);
	card::Rank * playable = nullptr;
	size_t canPlay(decodeADF(ADF, playable, defSize));

	if(canPlay <= 0)
	{//there are no more legal moves
			delete[] playable;
		return 0;
	}
	unDraw();
	//size_t count(0);
	bool valid(true);
	//card ** buffer = new card * [canPlay];
	
	size_t selected(0);

	if(ADF == nullptr)//this is the first time in a turn
	{
		//we are looking for our lowest ranked cards. 
		ADF = new card * [5][2];
		for(size_t i(0); i < 5; i++)
			for(size_t j(0); j < 2;j++)
				ADF[i][j] = nullptr;

		card::Rank lowest(hand[0]->GetRank());

		for(size_t i(0); i < cardCount && selected < canPlay ; i++)
		{
			if (lowest == hand[i]->GetRank()) //this will get one rank (any) 
				//and play all of that rank. including kozer. (sucks for you.)
			{
				ADF[selected++][0] = hand[i];
				removeCard(i--);
			}
		}

	}
	else
	{
		for(size_t i(0); (i<cardCount) && selected < canPlay; i++)
		{
			for(size_t j(0); playable[j] != card::two ; j++)
			{
				if(hand[i]->GetRank() == playable[j])
				{
					size_t k(0);
					while(ADF[k][0] != nullptr && k < 5)
					{k++;}
					ADF[k][0] = hand[i];
					removeCard(i--);
					selected++;
					break;
				}
			}
		}
	}


	delete[] playable;
	setCardPos();
	Draw();
	return selected>0?1:0;
}



//similar to attack, only different.
int cPlayer::defendPhase(card * (*&ADF)[2], card::Suit kozer)
{
	Sleep(900);
	int * toPlay = nullptr;
	size_t mustPlay(decodeADF(ADF, toPlay));	

	if(mustPlay == 0)
	{//this really shouldn't happen, it would indicate that either there are no cards left to beat
		//or noone played any cards in the fist place... neither should happen.
			delete[] toPlay;
		return -2;
	}
	if(mustPlay > cardCount)
	{//omg error. someone somewhere didn't play by the rules
			delete[] toPlay;
		return -1;
	}


	//size_t count(0);
	bool valid(true);
	//card ** buffer = new card * [mustPlay];

	unDraw();


	size_t selected(0);
	for(size_t j(0);j < mustPlay && valid; j++)
	{//we want to go through the hand 1 time for every card that must be played.
		for(size_t i(0); i < cardCount && selected < mustPlay; i++)
		{//we are looping through every card in the comps hand untill we have
			//either found enough cards that are valid, or untill we have gone through the whole
			// hand without avail.
			card * tobeat = ADF[toPlay[selected]][0];
			if(tobeat->GetSuit() == kozer)
			{
				//we have a live one here!
				if(hand[i]->GetSuit() == kozer && hand[i]->GetRank() > tobeat->GetRank())
				{//we have a win rar, but there may be a previous winrar that is better suited!
					if(ADF[toPlay[selected]][1])
					{
						if(ADF[toPlay[selected]][1]->GetRank() > hand[i]->GetRank())
						{//this is better!
							swap(ADF[toPlay[selected]][1], hand[i]); //we don't change the hand size
						}
					}
					else
					{
						ADF[toPlay[selected]][1] = hand[i];
						removeCard(i);
						i--;//we changed the handsize, so redo the last index.('sa different card now.)
					}
					
				}
					
			}
			else
			{
				if(tobeat->GetSuit() == hand[i]->GetSuit() || hand[i]->GetSuit() == kozer)
				{//if it aint, we need to be the same, or kozer.
					//this is messy because we need to always prefer samesuit over kozer
					//thus it will seem... redundant...
					if(tobeat->GetSuit() == hand[i]->GetSuit() && hand[i]->GetRank() > tobeat->GetRank())
					{//we won!
						if(ADF[toPlay[selected]][1])
						{//we had a previous winner
							if( (ADF[toPlay[selected]][1]->GetRank() > hand[i]->GetRank()) ||
								(ADF[toPlay[selected]][1]->GetSuit()==kozer) )
							{//we are better!
								swap(ADF[toPlay[selected]][1], hand[i]);
							}
						}
						else
						{
							ADF[toPlay[selected]][1] = hand[i];
							removeCard(i);
							i--;
						}
					}
					else
					{
						if(hand[i]->GetSuit() == kozer)
						{
							if(ADF[toPlay[selected]][1])
							{//there is already a card
								if(ADF[toPlay[selected]][1]->GetSuit() == kozer)
								{//twas kozer, are we better?
									if(ADF[toPlay[selected]][1]->GetRank() > hand[i]->GetRank())
									{//we are better!
										swap(ADF[toPlay[selected]][1], hand[i]);
									}
								}
							}
							else
							{
								ADF[toPlay[selected]][1] = hand[i];
								removeCard(i);
								i--;
							}
						}
					}
				}
			}
		}
		if(ADF[toPlay[selected]][1])
		{//we found the very best card in our hand to play!
			selected++;
		}
		else
		{//we are misrebal falures.
			for(size_t i(0); i <= selected; i ++)
			{
				if(ADF[toPlay[i]][1])
				{
					addCard(ADF[toPlay[i]][1]);
					ADF[toPlay[i]][1] = nullptr;
				}
			}
			valid = false;
		}
	}

	delete[] toPlay;

	//delay loop
	setCardPos();
	Draw();


	//return (Pass.clicked?0:1);
	return valid?1:0;
}



void cPlayer::Draw()
{///////////////////////////////////////////////////////////////////////////////////
	COORD temp;
	int i(0);
	{
		CONSOLE_SCREEN_BUFFER_INFO ScreenBufferInfo = { 0 };
		GetConsoleScreenBufferInfo(gameState->stout, &ScreenBufferInfo );
		temp.X = ScreenBufferInfo.dwCursorPosition.X;
		temp.Y = ScreenBufferInfo.dwCursorPosition.Y;
	}
	SetConsoleCursorPosition(gameState->stout, pos);
	//draw code
	for(int i(0); i < cardCount; i++)
	{
		if(!horz)
			blank->moveTo(point(pos.X(), pos.Y()+(i*2)));
		else
			blank->moveTo(point(pos.X()+(2*i),pos.Y()));
		blank->Draw();
	}
	//end draw code
	dirty = false;
	SetConsoleCursorPosition(gameState->stout, temp);
}

void cPlayer::unDraw()
{
			COORD temp;
	int i(0);
	{
		CONSOLE_SCREEN_BUFFER_INFO ScreenBufferInfo = { 0 };
		GetConsoleScreenBufferInfo(gameState->stout, &ScreenBufferInfo );
		temp.X = ScreenBufferInfo.dwCursorPosition.X;
		temp.Y = ScreenBufferInfo.dwCursorPosition.Y;
	}
	SetConsoleCursorPosition(gameState->stout, pos);
	//draw code
	for(int i(0); i < cardCount; i++)
	{
		if(!horz)
			blank->moveTo(point(pos.X(), pos.Y()+(i*2)));
		else
			blank->moveTo(point(pos.X()+(2*i),pos.Y()));
		blank->unDraw();
	}
	//end draw code
	dirty = false;
	SetConsoleCursorPosition(gameState->stout, temp);
}






hPlayer::hPlayer(point p, ioState * gs)
		:Play(L"[Play]", point( p.X()+75+5, p.Y()+2), gs),
		Pass(L"[Pass]", point(p.X()+75+5, p.Y()+4), gs),
		Swap(L"[Swap]", point(p.X()+84+5, p.Y()+2), gs),
		player(true, true, p, gs)
{

}

hPlayer::~hPlayer()
{
	delete[] hand;
	hand = nullptr;
}



//return of 1 means continue the attack forward
//return of 0 means that there are no more spaces to attack.
//return of -1 means bail out to the menu, undraw everything, abandon ship.
int hPlayer::attackPhase(card * (*&ADF)[2], card::Suit kozer, size_t defSize)
{
	size_t const handMax(5);
	Play.clicked = false;
	Pass.clicked = false;

	card::Rank * playable = nullptr;
	size_t canPlay(decodeADF(ADF, playable, defSize));

	if(canPlay == 0)
	{//there are no more legal moves
			delete[] playable;
		return 0;
	}
	//decifer what has been played already to define legal moves.


	while(Play.clicked == false && Pass.clicked ==false)
	{
		getInputEvents(*gameState);

		//Swap button code
		if(Swap.onMouseClick())
		{
			int t1(-1), t2(-1);
			for(size_t i(0); i < cardCount; i++)
			{
				if((hand[i]->clicked) && (t1 == -1))
					t1 = i;
				else
				if((hand[i]->clicked) && (t1 != -1) && (t2 == -1))
					t2=i;

				if(hand[i]->clicked)
				{
					hand[i]->unDraw();
					hand[i]->clicked = false;
				}
			}
			swap(hand[t1], hand[t2]);
			setCardPos();
			makeDirty();
		}



		//card click code
		for(size_t i(0); i < cardCount; i++)
		{
			if(hand[i]->onMouseClick())
			{
				hand[i]->clicked = !hand[i]->clicked;
				if(hand[i]->clicked)
				{
					hand[i]->unDraw();
					hand[i]->move(0,-1);
				//	hand[i]-Draw()
				}
				else
				{
					hand[i]->unDraw();
					hand[i]->move(0,1);
				}
				makeDirty();
			}
		}



		if(Play.onMouseClick())
		{
			//size_t count(0);
			bool valid(true);
			card ** buffer = new card * [canPlay];


			size_t selected(0);
			for(size_t i(0); i < cardCount;i++)
			{
				if(hand[i]->clicked)
				{
					hand[i]->clicked = false;
					buffer[selected++] = hand[i];
					unDraw();
					removeCard(i);
					i--;
					makeDirty();
				}
			}
			setCardPos();

			if(ADF == nullptr)//this is the first time in a turn
			{
				for(size_t i(1); (i < selected) && valid; i++)
				{
					valid = valid && (buffer[0]->GetRank() == buffer[i]->GetRank()) && (selected <= canPlay) ;
				}
			}
			else
			{
				for(size_t i(0); (i<selected) && valid; i++)
				{
					size_t j(0);
					for(j = 0; playable[j] != buffer[i]->GetRank() && playable[j] != card::two ; j++);
					valid = valid && (playable[j] == buffer[i]->GetRank());
				}
			}

				//first time, we will set up ADF				
			if(valid)
			{
				if (ADF == nullptr)
				{
					ADF = new card * [5][2];
					for(size_t i(0);i<5;i++)
						for(size_t j(0);j<2;j++)
							ADF[i][j] = nullptr;
				}
				size_t j(0);
				for(size_t i(0); j < selected; i++)
				{
					if(ADF[i][0] == nullptr)
					{
						ADF[i][0] = buffer[j++];
						
					}
				}

				Play.clicked = true;
			}
			else
			{
				for(size_t i(0); i < selected; i++)
				{
					addCard(buffer[i]);
				}
				setCardPos();
			}
			delete[] buffer;
					//check what cards are selected
		}

		if(Pass.onMouseClick())
		{
			unDraw();
			for(int i(0); i < cardCount;i++)
			{
			
				hand[i]->clicked = false;
			}
			setCardPos();
			makeDirty();
			if(ADF)
				Pass.clicked = true;
		}

		
		//mouse epilouge.
		if(isDirty())
		{
			//setCardPos();
			Draw();
		}
		clearInputEvents(*gameState);
	}

	//here is a loop that adds all selected cards to ADF


	
	delete[] playable;
	setCardPos();
	return Pass.clicked?0:1;
}



//1 sucsessful defense.
//0 i lost give me all the cards
//
int hPlayer::defendPhase(card * (*&ADF)[2], card::Suit kozer)
{
	Play.clicked = false;
	Pass.clicked = false;

	int * toPlay = nullptr;
	size_t mustPlay(decodeADF(ADF, toPlay));

	if(mustPlay == 0)
	{//this really shouldn't happen, it would indicate that either there are no cards left to beat
		//or noone played any cards in the fist place... neither should happen.
		delete[] toPlay;
		return -2;
	}
	//decifer what has been played already to define legal moves.


	while(Play.clicked == false && Pass.clicked == false)
	{
		getInputEvents(*gameState);

		//Swap button code
		if(Swap.onMouseClick())
		{
			int t1(-1), t2(-1);
			for(size_t i(0); i < cardCount; i++)
			{
				if((hand[i]->clicked) && (t1 == -1))
					t1 = i;
				else
				if((hand[i]->clicked) && (t1 != -1) && (t2 == -1))
					t2=i;

				if(hand[i]->clicked)
				{
					hand[i]->unDraw();
					hand[i]->clicked = false;
				}
			}
			swap(hand[t1], hand[t2]);
			setCardPos();
			makeDirty();
		}
		
		//card click code
		for(size_t i(0); i < cardCount; i++)
		{
			if(hand[i]->onMouseClick())
			{
				hand[i]->clicked = !hand[i]->clicked;
				if(hand[i]->clicked)
				{
					hand[i]->unDraw();
					hand[i]->move(0,-1);
				//	hand[i]-Draw()
				}
				else
				{
					hand[i]->unDraw();
					hand[i]->move(0,1);
				}
				makeDirty();
			}
		}



		if(Play.onMouseClick())
		{
			//size_t count(0);
			bool valid(true);
			card ** buffer = new card * [mustPlay];


			size_t selected(0);
			for(size_t i(0); i < cardCount;i++)
			{
				if(hand[i]->clicked)
				{
					hand[i]->clicked = false;
					buffer[selected++] = hand[i];
					unDraw();
					removeCard(i);
					i--;
					makeDirty();
				}
			}
			setCardPos();

			if(mustPlay==selected)
			{
				for(size_t i(0); (i < selected) && valid; i++)
				{
					if(ADF[toPlay[i]][0]->GetSuit() == kozer && mustPlay>0)
					{
						for(size_t j(0);(j < selected) &&
						(buffer[j] == nullptr ||
						 ( buffer[j]->GetSuit() == kozer && 
						  (
						   (buffer[j]->GetRank() > ADF[toPlay[i]][0]->GetRank()) 
						  )
						 ) 
						)  ;j++)
						{
							if(buffer[j])
							{
								if(ADF[toPlay[i]][1] && ADF[toPlay[i]][1]->GetRank() < buffer[j]->GetRank())
								{

								}
								else
								{
									//if ADF is null it moves in the null, if it isn't it swaps the higer lvl card
									//back to the buffer to be used latter.
									swap(ADF[toPlay[i]][1],buffer[j]);
									if(buffer[j]==nullptr) mustPlay--;
								}
							}
						}
					}
					else
					{
						for(size_t j(0);//god, it looks like lisp...
						j < selected && (buffer[j] == nullptr ||
						 ( ADF[toPlay[i]][0]->GetSuit() == buffer[j]->GetSuit()  &&
						 (ADF[toPlay[i]][0]->GetRank() < buffer[j]->GetRank()) ||
						   (buffer[j]->GetSuit() == kozer) 
						)) ;j++)
						{
							if(buffer[j]==nullptr)//less tabs
								continue;

							if(ADF[toPlay[i]][1])
							{
								if(ADF[toPlay[i]][1]->GetSuit() != kozer &&
									buffer[j]->GetSuit() == kozer);//do nothing
								else
								{
									if(ADF[toPlay[i]][1]->GetSuit() == kozer &&
									buffer[j]->GetSuit() == kozer)//we want the lowest one
									{
										if(ADF[toPlay[i]][1]->GetRank() < 
											buffer[j]->GetRank());//do nothing
										else
											swap(ADF[toPlay[i]][1],buffer[j]);
									}
									else
										if(ADF[toPlay[i]][1]->GetSuit() == kozer &&
											buffer[j]->GetSuit() != kozer)
										{
											swap(ADF[toPlay[i]][1],buffer[j]);
										}
								}
							}
							else
							{
								swap(ADF[toPlay[i]][1],buffer[j]);
								mustPlay--;
							}
						}
					}
				}
				if(mustPlay == 0)
				{
					valid = true;
				}
				else
				{
					valid = false;
				}
			}
			else
			{
				valid = false;
			}
			

		
			if(valid)
			{
				Play.clicked = true;
			}
			else
			{
				for(size_t i(0); i < selected; i++)
				{
					if(buffer[i])
						addCard(buffer[i]);
					if(ADF[toPlay[i]][1])
					{
						addCard(ADF[toPlay[i]][1]);
						ADF[toPlay[i]][1] = nullptr;
					}
				}
				setCardPos();
				mustPlay = decodeADF(ADF, toPlay);
			}
			delete[] buffer;
					//check what cards are selected
		}



		
		if(Pass.onMouseClick())
		{
			unDraw();
			for(int i(0); i < cardCount;i++)
			{
				hand[i]->clicked = false;
			}
			setCardPos();
			makeDirty();
			Pass.clicked = true;
		}

		
		//mouse epilouge.
		if(isDirty())
		{
			//setCardPos();
			Draw();
		}
		clearInputEvents(*gameState);
	}

	//here is a loop that adds all selected cards to ADF



	delete[] toPlay;
	setCardPos();
	return (Pass.clicked?0:1);
}


void hPlayer::setCardPos()
{
	int cardSpace((X_IF_HORZ+5)/(cardCount+1));
	for (size_t i(0); i < cardCount; i++)
	{
		hand[i]->moveTo(point(pos.X()-5 + (i+1)*(cardSpace), pos.Y()+1 ) );
	}
}

void hPlayer::Draw()
{//to draw we need to know vertical li, visible li, and position.
	//human is always south but we'll make it dynamic.
	
	for(size_t i(0); i < cardCount; i++)
	{
		hand[i]->Draw();
	}
	Play.Draw();
	Swap.Draw();
	Pass.Draw();
	dirty = false;
}

void hPlayer::unDraw()
{
	for(size_t i(0); i < cardCount; i++)
	{
		hand[i]->unDraw();
	}
}