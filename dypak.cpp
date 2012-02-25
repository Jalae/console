#include <iostream>
#include <windows.h>

//#include "button.h"
#include "dypak.h"
#include "image.h"
//#include "player.h"
//#include "deck.h"



dypak::dypak():ADFpos(27,30), blank(cardblank, point(0,0), 6, &gameState)
{

	//setup my gameState
	gameState.stout= GetStdHandle(STD_OUTPUT_HANDLE);
	gameState.stin = GetStdHandle(STD_INPUT_HANDLE);
	gameState.M_POS.X = 0;
	gameState.M_POS.Y = 0;
	gameState.numRead = 0;
	gameState.numEvents = 0;
	gameState.inputStat = nullptr;

	//hide cursor
	CONSOLE_CURSOR_INFO info;
	GetConsoleCursorInfo(gameState.stout,&info);
	info.bVisible=false;
	SetConsoleCursorInfo(gameState.stout,&info);

	//set nowrap
	DWORD mode;
	GetConsoleMode(gameState.stout, &mode);
	mode &= ~ENABLE_WRAP_AT_EOL_OUTPUT;
	SetConsoleMode(gameState.stout, mode);

	//set font size to be smaller.
	CONSOLE_FONT_INFOEX fn;
	fn.cbSize = sizeof( CONSOLE_FONT_INFOEX );
	GetCurrentConsoleFontEx(gameState.stout, false, &fn);
	fn.dwFontSize.X = 6;
	fn.dwFontSize.Y = 12;
	SetCurrentConsoleFontEx(gameState.stout, false, &fn);

	setGameSize(124, 53);
	/*
	the window is made of 5 play areas, north, east, south, and west, and a center play area
	north and south are each 7 segments tall and 77


	*/
	attackDefendField = nullptr;
	
}
dypak::~dypak()
{
	clearInputEvents(gameState);
}




bool dypak::print_title()
{		
	COORD out_cursor = {5, 3};//where to write for screen output
	COORD debug = {0, 0};//where debug info shows up

	button play(L"[play]", point(32, 30), &gameState);
	button quit(L"[quit]", point(44, 30), &gameState);
	ascii_image title (game_title, point(5, 3), 19, &gameState);

	title.Draw();
	play.Draw();
	quit.Draw();



	while(1)
	{
		getInputEvents(gameState);
		SetConsoleCursorPosition(gameState.stout, debug);
		std::cout << gameState.M_POS.X << " "<< gameState.M_POS.Y << ": :";
		if(play.onMouseClick())
		{
			clearInputEvents(gameState);
			return 1;
		}
		if(quit.onMouseClick())
		{
			clearInputEvents(gameState);
			return 0;
		}
		clearInputEvents(gameState);
	}
}


/**
Sets the consoles size

@param x Width
@param y Height
*/
void dypak::setGameSize(SHORT x, SHORT y)
{
	SMALL_RECT srWindowRect; // window size
	COORD coordScreen;       // Largest window 
	
	//Whats the biggest window
	coordScreen = GetLargestConsoleWindowSize(gameState.stout);

	//Window Size (1 less than buffer size)
    srWindowRect.Right = (SHORT) (min(x, coordScreen.X)) - 1;
    srWindowRect.Bottom = (SHORT) (min(y, coordScreen.Y)) - 1;

	//Upper Left
    srWindowRect.Left = srWindowRect.Top = (SHORT) 0;

    SetConsoleScreenBufferSize(gameState.stout, coordScreen);

    SetConsoleWindowInfo(gameState.stout, TRUE, &srWindowRect);
	
	//Set Buffer Size
    coordScreen.X = x;
    coordScreen.Y = y;

	SetConsoleScreenBufferSize(gameState.stout, coordScreen);
}




bool dypak::print_menu()
{

	//todo, buttons for options, playgame, title, 
	play();
	return false;
}



void drawADF(card*(*ADF)[2], ascii_image & blank, point & ADFpos)
	{//draw ADF
		if(ADF)
		{
			for(size_t i(0); i < 5; i++)
			{
				for(size_t j(0); j < 2;j++)
				{
					if(ADF[i][j])
					{
						ADF[i][j]->moveTo(point(ADFpos.X()+((77/5)*i), ADFpos.Y()+ ((13/2)*j)));
						ADF[i][j]->Draw();
					}
					else
					{
						blank.moveTo(point(ADFpos.X()+((77/5)*i), ADFpos.Y()+ ((13/2)*j)));
						blank.Draw();
					}
				}
			}
		}
		else
		{
			for(size_t i(0); i < 5; i++)
			{
				for(size_t j(0); j < 2;j++)
				{
					blank.moveTo(point(ADFpos.X()+((77/5)*i), ADFpos.Y()+ ((13/2)*j)));
					blank.Draw();
				}
			}
		}
	}



void dypak::play()
{

	deck dypakdeck(&gameState, true, point(42+16, 9+5));
	
	//i really want to see what would happen here
	player * players[] = {new hPlayer(point(22, 46), &gameState),
		//new cPlayer(point (22, 46), true, true, &gameState, &blank), //number players = 0!
		new cPlayer(point(1, 7), false, false, &gameState, &blank),
			new cPlayer(point(22, 1), true, false, &gameState, &blank),
			new cPlayer(point(118, 7), false, true, &gameState, &blank)
			};

	dypakdeck.Shuffle();
	kozer = dypakdeck.kozer();
	dypakdeck.Draw();
	for(int i(0); i < 4; i++)
	{
		deal(*players[i], dypakdeck);
	}	

	drawADF(attackDefendField, blank, ADFpos);

		for(size_t i(0);i<4;i++)
		{
			players[i]->unDraw();
			players[i]->setCardPos();
			//players[i]->makeDirty();
			players[i]->Draw();
		}

	bool continuing(true);
	int prevDefend(1);
	for(size_t defender(1); continuing ; defender=(defender+1)%4 )
	{
		for(size_t i(0); i < 4; i++)
		{
			//previous defender draws first.
			deal(*players[(i+3+defender)%4], dypakdeck);
		}
		if(players[defender]->GetCount() > 0)
		{
			if(prevDefend == 1)
			{
				prevDefend = turn(players, defender);
				if(prevDefend == 0)
				{
					players[defender]->unDraw();		
					for(size_t i(0); i<5;i++)
						for(size_t j(0); j < 2; j++)
						{
							if(attackDefendField[i][j])
							{

								players[defender]->addCard(attackDefendField[i][j]);
							}
						}
				}
			}
			else
			{
				prevDefend = 1;
			}
		}


		drawADF(attackDefendField, blank, ADFpos);
		
		for(size_t i(0);i<4;i++)
		{
			players[i]->unDraw();
			players[i]->setCardPos();
			//players[i]->makeDirty();
			players[i]->Draw();
		}


		delete[] attackDefendField;
		attackDefendField = nullptr;

		size_t winners(0);
		for(size_t i(0); i < 4; i++)
		{
			if(players[i]->GetCount() == 0)
			{
				winners++;
			}
		}
			continuing = winners < 3;
	}
	size_t i(0);
	for(i; i < 4 && players[i]->GetCount() == 0 ;i++);
	//system("cls");
	clearscreen();
	std::wcout << L"\n\n\n\n\n\t\tOMG WTF THE GAME IS OVER!\n\n\n" <<
		(i==0?L"south":(i==1?L"west":(i==2?L"north":(i==3?L"east":L"no one"))))<< L" is the DYPAK!\n\n"; 
	std::wcout << L"\\t\tclick the [X] in the top to exit.";
	button clicktoexit(L"[X]", point(121,0), &gameState);
	clicktoexit.Draw();
	while(true)
	{
		getInputEvents(gameState);
		if(clicktoexit.onMouseClick())
		{
			clearInputEvents(gameState);
			break;
		}
		clearInputEvents(gameState);
	}
	clearscreen();

	for(int i(0); i < 4 ; i++)
	{
		delete players [i];
	}

	//system("cls");
	//system("pause");
}

void dypak::drawStatus(size_t defender, size_t initer, size_t attker)
{
	COORD temp;
	{
		CONSOLE_SCREEN_BUFFER_INFO ScreenBufferInfo = { 0 };
		GetConsoleScreenBufferInfo(gameState.stout, &ScreenBufferInfo );
		temp.X = ScreenBufferInfo.dwCursorPosition.X;
		temp.Y = ScreenBufferInfo.dwCursorPosition.Y;
	}
	point pos(0,0);
	SetConsoleCursorPosition(gameState.stout, pos);
	std::wcout << L"Defender: " << (defender==0?L"south":(defender==1?L"west ":(defender==2?L"north":(defender==3?L"east ":L"error"))));
	SetConsoleCursorPosition(gameState.stout, point(pos.X(), pos.Y()+1));
	std::wcout << L"Initator: " << (initer==0?L"south":(initer==1?L"west ":(initer==2?L"north":(initer==3?L"east ":L"error"))));
	SetConsoleCursorPosition(gameState.stout, point(pos.X(), pos.Y()+2));
	std::wcout << L"Cur. Turn: " << (attker==0?L"south":(attker==1?L"west ":(attker==2?L"north":(attker==3?L"east ":L"error"))));
	
	SetConsoleCursorPosition(gameState.stout, temp);
}


int dypak::turn(player ** players, size_t defIndex)
{
	size_t atkIndex = (3+defIndex)%4;
	
	int defVal(0), atkVal(0), playersleft(4);

	while(players[atkIndex]->GetCount() == 0)
	{
		atkIndex = (atkIndex+3)%4;
		playersleft--;
	}
	while(players[defIndex]->GetCount() == 0)
	{
		defIndex = (defIndex+1)%4;
		playersleft--;
	}
	drawStatus(defIndex, atkIndex, atkIndex);
	drawADF(attackDefendField, blank, ADFpos);
	players[atkIndex]->attackPhase(attackDefendField, kozer, players[defIndex]->GetCount() );
	do
	{
		atkVal = 0;
		drawADF(attackDefendField, blank, ADFpos);

		drawStatus(defIndex, atkIndex, defIndex);

		defVal = players[defIndex]->defendPhase(attackDefendField, kozer);
		drawADF(attackDefendField, blank, ADFpos);
		for(size_t i(1); i < playersleft; i++)
		{
			drawStatus(defIndex, atkIndex, (defIndex+i)%4);
			if(players[(defIndex+i)%4]->GetCount() > 0)
				atkVal =  players[(defIndex+i)%4]->attackPhase(attackDefendField, kozer, players[defIndex]->GetCount() )|| atkVal;
			drawADF(attackDefendField, blank, ADFpos);
		}

		if(defVal < 0)
			break;
	}while((defVal == 1 && atkVal==1));

	//player[3+defIndex%4].attack();
	//do while ()
	//
	return defVal;
}


void dypak::deal(player & p, deck & d)
{
	card * t;
	while(p.GetCount() < 5)
	{
		t = d.Deal();
		if(t)
		{
			p.addCard(t);
		}
		else
		{
			break;
		}
	}
}


void dypak::clearscreen()
{
	COORD temp;
	int i(0);
	{
		CONSOLE_SCREEN_BUFFER_INFO ScreenBufferInfo = { 0 };
		GetConsoleScreenBufferInfo(gameState.stout, &ScreenBufferInfo );
		temp.X = ScreenBufferInfo.dwCursorPosition.X;
		temp.Y = ScreenBufferInfo.dwCursorPosition.Y;
	}
	SetConsoleCursorPosition(gameState.stout, point(0,0));
	for(int i (0); i < 124; i++)
		{
			for(int j(0); j< 96;j++)
				std::cout << " ";
			std::cout << "\n";
		}
		SetConsoleCursorPosition(gameState.stout, temp);
}


void clearInputEvents(ioState & gameState)
{
	delete[] gameState.inputStat;
	gameState.inputStat = nullptr;
}


void getInputEvents(ioState & gameState)
{
	//DWORD old = numEvents;
	GetNumberOfConsoleInputEvents(gameState.stin, &gameState.numEvents);
	gameState.inputStat = new INPUT_RECORD[gameState.numEvents];
    ReadConsoleInput(gameState.stin, gameState.inputStat, gameState.numEvents, &gameState.numRead);
	for(int i = gameState.numRead; i >=0; i--)
	{
		if(gameState.inputStat[i].EventType == MOUSE_EVENT)
			gameState.M_POS = gameState.inputStat[i].Event.MouseEvent.dwMousePosition;
	}
}


wchar_t const * dypak::game_title[] =
{
	L"      ##### ##                                         /        ",
	L"   /#####  /##                                       #/         ",
	L" //    /  / ###                                      ##         ",
	L"/     /  /   ###                                     ##         ",
	L"     /  /     ###                                    ##         ",
	L"    ## ##      ## ##   ####         /###     /###    ##  /##    ",
	L"    ## ##      ##  ##    ###  /    / ###  / / ###  / ## / ###   ",
	L"    ## ##      ##  ##     ###/    /   ###/ /   ###/  ##/   /    ",
	L"    ## ##      ##  ##      ##    ##    ## ##    ##   ##   /     ",
	L"    ## ##      ##  ##      ##    ##    ## ##    ##   ##  /      ",
	L"    #  ##      ##  ##      ##    ##    ## ##    ##   ## ##      ",
	L"       /       /   ##      ##    ##    ## ##    ##   ######     ",
	L"  /###/       /    ##      ##    ##    ## ##    /#   ##  ###    ",
	L" /   ########/      #########    #######   ####/ ##  ##   ### / ",
	L"/       ####          #### ###   ######     ###   ##  ##   ##/  ",
	L"#                           ###  ##                             ",
	L" ##                  #####   ### ##                             ",
	L"                   /#######  /#  ##                             ",
	L"                  /      ###/     ##							 ",
	nullptr
};

wchar_t const * dypak::cardblank[] =
{
	L"\xDA\xC4\xC4\xC4\xBF",
	L"\xB3   \xB3",
	L"\xB3   \xB3",
	L"\xB3   \xB3",
	L"\xC0\xC4\xC4\xC4\xD9",
	nullptr
};
