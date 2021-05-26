#include "SDL.h"
#include "Main.h"
#include "Dictionary.h"
#include <vector>
#include <iostream>

const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 768;

int main(int argc, char *argv[])
{
	Game* game = new Game(SCREEN_WIDTH, SCREEN_HEIGHT);
	game->loop();
	delete game;
	return 0;
}