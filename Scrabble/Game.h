#include <vector>
#ifndef GAME_H
#define GAME_H

class Board;
class Button;
class GUI;
class Player;
class Tile;
class TileBag;

class Game {
private:
	bool quit;
	static GUI * gui;
	int mouseX, mouseY;
	static int roundforPassrate;
	enum Status { Lobby, Play };
	static enum Difficulty { Beginner, Intermediate, Skilled, Expert, Unknown };
	Status status = Lobby;
	static Difficulty difficulty;
	static int round;
	static int turn;
	static TileBag * tileBag;
	static Board * board;
	static std::vector<Player*> players;
	static std::string botMessage;
	bool exchanging;
	bool selectingLetter;
	Tile * blankTile;

	Button * buttonConfirm;
	Button * buttonPass;
	Button * buttonRecall;
	Button * buttonShuffle;
	Button * buttonExchange;
	Button * buttonLeave;

	void button_beginner();
	void button_intermediate();
	void button_skilled();
	void button_expert();
	void button_unknown();

	void button_confirm();
	void button_pass();
	void button_recall();
	void button_shuffle();
	void button_exchange();
	void button_leave();
	void button_exchangeConfirm();
	void button_exchangeLeave();

	void button_selectCharForBlankTile(char letter);
public:
	Game(int w, int h);
	static int startBot(void * data);
	~Game();
	void loop();
};

#endif 