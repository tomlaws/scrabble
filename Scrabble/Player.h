#include "Board.h"
#include "SDL.h"
#include <string>
#include <vector>
#include "Game.h"

class GUI;
struct Move;
class Tile;
class TileBag;

class Player {
private:
	Tile * tileDragging;
	Tile * tileDropping;
	Tile * findPressedTile(int mouseX, int mouseY);
protected:
	int index;
	std::string name;
	int score;
	std::vector<Tile*> tiles;
	std::vector<Tile*> tilesDropping;

	std::vector<Move> moves;

	void addMove(Tile * tile, int col, int row);
	void removeMove(Tile * tile);

	bool swapTile(int mouseX, int mouseY, Tile * swappingTile);

public:
	Player(int index, std::string name);
	~Player();
	std::string getName();
	int getScore();
	std::vector<Tile*> getTiles();

	void setName(std::string name);
	void setScore(int score);
	void addScore(int score);

	void draw7Tiles(TileBag * tileBag);
	void refillTiles(TileBag * tileBag);
	void recallTiles();
	void shuffleTiles();

	std::string confirmMove(Board * board, int & score);
	bool confirmExchange(TileBag * tileBag);
	void undoExchange();
	
	virtual void autoPlay(Board * board, TileBag * tileBag, std::string & computerMessage, int diff, int& round, int&roundforPassrate);

	void draw(GUI * gui);
	void drawTiles(GUI * gui);
	Tile * dragTile(SDL_Event * event, Board * board, int mouseX, int mouseY, int relX, int relY);
	void dropTiles(SDL_Event * event, int mouseX, int mouseY);
};

class Bot : public Player {
public:
	Bot(int index, std::string name) :Player(index, name) {};

	Tile * extractTileByLetter(std::vector<Tile*>& tiles, char letter);
	void autoPlay(Board * board, TileBag * tileBag, std::string & computerMessage, int diff, int& round, int&roundforPassrate);
	void autoExchange(TileBag * tileBag);
	bool attemptFirstWord(Board * board, std::string word, std::string & computerMessage);
	void generatePass(int diff, int& round, int& hit, int& passrate, const int allRound);
	bool attemptWordVectically(Board * board, Cell * cell, std::pair<std::string, std::string> word, std::string & computerMessage);
	bool attemptWordHorizontally(Board * board, Cell * cell, std::pair<std::string, std::string> word, std::string & computerMessage);
};