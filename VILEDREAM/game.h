#pragma once

#include "color_constants.h"
#include "display.h"
#include "inputHandler.h"
#include "gamedata.h"
#include "mapgen.h"
#include "dataLoader.h"

#include "ai.h"
#include "messages.h"
#include "movement.h"


class game
{
public:
	game();
	~game() {}

	void start();
	void mainGameLoop();
	void drawScreen();
	
	void processInput();
	void mainGameInput();
	void backOut();

	void moveToNewMap(mapStairsPtr st);
	void tryDescend();

	void triggerMonsterSpawns();

	void doCreatureTick(creaturePtr cr);
	void tick();

	void awaitDebugCommand();

private:

	bool _isGameOver;

	display	_disp;
	gamedataPtr _gdata;
	loadedFileDataPtr _ldata;
	inputHandlerPtr _ih;

	int _globalTickCounter;

};

