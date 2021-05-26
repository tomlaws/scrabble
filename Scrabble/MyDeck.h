#include "SDL.h"
#include "GUI.h"
#include "Deck.h"
#include "Tile.h"

class MyDeck {
public:
	MyDeck();
	~MyDeck();
	void draw();
private:
	const int MYDECK_HEIGHT = 60;
	void drawPanel();
};