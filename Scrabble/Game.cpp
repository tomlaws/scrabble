#include "Game.h"
#include "Board.h"
#include "Button.h"
#include "GUI.h"
#include "Player.h"
#include "Tile.h"
#include "TileBag.h"
#include "TileButton.h"
#include "SDL_thread.h"

int Game::roundforPassrate;
int Game::round;
int Game::turn;
Game::Difficulty Game::difficulty;
GUI * Game::gui;
TileBag * Game::tileBag;
Board * Game::board;
std::vector<Player*> Game::players;
std::string Game::botMessage = "";

Game::~Game(void) {
	delete gui;
}
//
Game::Game(int w, int h)
{
	gui = new GUI(&quit, w, h);
	exchanging = false;
	selectingLetter = false;
}

int Game::startBot(void *data)
{
	if (gui->getMainEvent()->type == SDL_QUIT) return 0;
	std::string computerMessage = "";
	players[turn]->autoPlay(board, tileBag, computerMessage, difficulty, round, roundforPassrate);
	botMessage = players[turn]->getName() + " " + computerMessage;
	roundforPassrate++;
	round++;
	return 0;
}

void Game::loop()
{
	while (!quit && gui->getMainEvent()->type != SDL_QUIT)
	{
		//
		gui->begin();
		SDL_GetMouseState(&mouseX, &mouseY);

		//	Lobby
		if (status == Lobby) {

			//	Title
			SDL_Rect titlePos;
			SDL_Texture * title = gui->createTextTexture(&titlePos, gui->getFontLemonMilkLarge(), "Scrabble", SDL_Color{ 122, 202, 255, 255 });
			titlePos.y = 50;
			titlePos.x = (1024 - titlePos.w) / 2;

			//	Welcome message
			SDL_Rect wmPos;
			SDL_Texture * wm = gui->createTextTexture(&wmPos, gui->getFontMedium(), "Welcome to Scrabble, please choose a level to play!", SDL_Color{ 255, 255, 255, 255 });
			wmPos.y = 225;
			wmPos.x = (1024 - wmPos.w) / 2;
			
			//	Initialize the buttons
			Button * buttons = new Button();
			buttons->add("Button_Beginner", L"Beginner", "The entry level for beginner", SDL_Color{ 230, 172, 0, 255 }, gui->getFontLemonMilk(), 287, 260, 450, 60, std::bind(&Game::button_beginner, this));
			buttons->add("Button_Intermediate", L"Intermediate", "The moderate level for experienced player", SDL_Color{ 46, 138, 92, 255 }, gui->getFontLemonMilk(), 287, 320, 450, 60, std::bind(&Game::button_intermediate, this));
			buttons->add("Button_Skilled", L"Skilled", "The advanced level for skilled player", SDL_Color{ 184, 61, 122, 255 }, gui->getFontLemonMilk(), 287, 380, 450, 60, std::bind(&Game::button_skilled, this));
			buttons->add("Button_Expert", L"Expert", "The hardest level. Come and try!", SDL_Color{ 0, 77, 153, 255 }, gui->getFontLemonMilk(), 287, 440, 450, 60, std::bind(&Game::button_expert, this));
			buttons->add("Button_Unknown", L"Unknown", "Unknown!", SDL_Color{ 51, 0, 102, 255 }, gui->getFontLemonMilk(), 287, 500, 450, 60, std::bind(&Game::button_unknown, this));
			
			while (status == Lobby && gui->getMainEvent()->type != SDL_QUIT) {
				gui->begin();
				SDL_GetMouseState(&mouseX, &mouseY);

				//	Background
				gui->drawBackground();

				//	Title
				gui->drawTexture(title, &titlePos);

				//	Panel
				gui->drawObject(262, 209, 500, 380, SDL_Color{ 0, 0, 0, 85 });

				//	Welcome message
				gui->drawTexture(wm, &wmPos);

				//	Draw all buttons
				Button::drawAll(gui->getRenderer());
				if (gui->getHasEvent())
					Button::mouseEvent(gui->getMainEvent(), mouseX, mouseY);

				SDL_Delay(1);
				gui->end();
			}

			//	Clean up
			SDL_DestroyTexture(title);
			SDL_DestroyTexture(wm);
			delete buttons;
		}

		//	Play
		if (status == Play) {
			//	Initialize the buttons
			Button * buttons = new Button();
			buttonConfirm = buttons->add("Button_Confirm", L"\uf00c", "Confirm", SDL_Color{ 30, 188, 70, 255 }, gui->getFontAwesome(), 970, 718, 40, 40, std::bind(&Game::button_confirm, this));
			buttonPass = buttons->add("Button_Pass", L"\uf04e", "Pass", SDL_Color{ 42, 153, 255, 255 }, gui->getFontAwesome(), 915, 718, 40, 40, std::bind(&Game::button_pass, this));
			buttonRecall = buttons->add("Button_Recall", L"\uf01e", "Recall Tiles", SDL_Color{ 229, 50, 229, 255 }, gui->getFontAwesome(), 860, 718, 40, 40, std::bind(&Game::button_recall, this));
			buttonShuffle = buttons->add("Button_Shuffle", L"\uf074", "Shuffle Tiles", SDL_Color{ 255, 51, 102, 255 }, gui->getFontAwesome(), 124, 718, 40, 40, std::bind(&Game::button_shuffle, this));
			buttonExchange = buttons->add("Button_Exchange", L"\uf362", "Exchange Tiles", SDL_Color{ 239, 184, 50, 255 }, gui->getFontAwesome(), 69, 718, 40, 40, std::bind(&Game::button_exchange, this));
			buttonLeave = buttons->add("Button_Leave", L"\uf0d9", "Leave", SDL_Color{ 48, 96, 192, 255 }, gui->getFontAwesome(), 14, 718, 40, 40, std::bind(&Game::button_leave, this));

			//	Tile Selector
			SDL_Rect selectingTileTitlePos;
			SDL_Texture * selectingTileTitleTexture = gui->createTextTexture(&selectingTileTitlePos, gui->getFontLarge(), "Please choose a letter", SDL_Color{ 255, 255, 255, 255 });
			selectingTileTitlePos.y = 250;
			selectingTileTitlePos.x = (1024 - selectingTileTitlePos.w) / 2;

			auto tileButtons = new TileButton();
			for (int i = 0; i < 26; i++) {
				char letter = (char)(65 + i);
				std::wstring letter_str = std::wstring(1, letter);
				int c = i % 8;
				int r = i / 8;
				tileButtons->add("Button_Letters", letter_str, SDL_Color{ 255, 231, 158, 255 }, gui->getFontLarge(), 335 + c * 45, 330 + 45 * r, 40, 40, std::bind(&Game::button_selectCharForBlankTile, this, std::placeholders::_1));
			}

			//	Initialize tile bag
			tileBag = new TileBag();
			board = new Board(difficulty == Unknown);
			
			//	Initialize the players
			int numOfPlayer = 4;
			players.push_back(new Player(0, "Me"));
			players[0]->draw7Tiles(tileBag);
			for (int i = 1; i < numOfPlayer; i++) {
				players.push_back(new Bot(i, "PC" + std::to_string(i)));
				players[i]->draw7Tiles(tileBag);
			}

			//	Some variables
			round = 0;
			int notifiedTurn = -1;
			GUI::Message * waitMessage = nullptr;
			bool freeze = false;
			bool myTurn = false;
			bool gameEnd = false;

			while (status == Play && gui->getMainEvent()->type != SDL_QUIT) {
				gui->begin();

				SDL_GetMouseState(&mouseX, &mouseY);

				//	Draw Background
				gui->drawBackground();

				//	Draw Board
				board->draw(gui);

				//	Draw Panel
				gui->drawObject(0, 708, 1024, 60, SDL_Color{ 0, 0, 0, 85 });

				//	Draw Players On 4 Corners
				for (int i = 0; i < players.size(); i++) 
					players[i]->draw(gui);
				
				players[0]->drawTiles(gui);

				//	Draw all buttons
				Button::drawAll(gui->getRenderer());

				//	Check if any player won, if yes show the results and back to lobby
				bool noTile = tileBag->getRemainingTilesCount() == 0;
				if (noTile) {
					for (Player * player : players) {
						if (!gameEnd && player->getTiles().size() == 0) {
							//	This player won
							gameEnd = true;
							std::string winnerName = player->getName() == "Me" ? "You" : player->getName();
							int winnerScore = player->getScore();
							waitMessage = gui->addMessage(winnerName + " won the game with " + std::to_string(winnerScore) + " score!", 0, 400);
							break;
						}
					}
					if (gameEnd && waitMessage && waitMessage->time <= 0) {
						waitMessage = nullptr;
						status = Lobby;
						break;
					}
				}

				//	Check if any message is pending and show them
				if (botMessage.length() > 0) {
					gui->addMessage(botMessage, 1, 350);
					botMessage = "";
				}

				if (!gameEnd) {
					turn = round % numOfPlayer;
					if (turn == 0) {
						myTurn = true;
						if (notifiedTurn != turn) {
							board->updateTiles();
							buttonConfirm->setVisible(false);
							buttonPass->setVisible(false);
							buttonRecall->setVisible(false);
							buttonShuffle->setVisible(false);
							buttonExchange->setVisible(false);
							buttonLeave->setVisible(false);
							waitMessage = gui->addMessage("It's your turn now!", 1, 1024);
							notifiedTurn = turn;
						}
						if (waitMessage && waitMessage->time <= 0 && !exchanging) {
							waitMessage = nullptr;
							buttonConfirm->setVisible(true);
							buttonPass->setVisible(true);
							buttonRecall->setVisible(true);
							buttonShuffle->setVisible(true);
							buttonExchange->setVisible(true);
							buttonLeave->setVisible(true);
						}
					}
					else {
						myTurn = false;
						if (notifiedTurn != turn) {
							board->updateTiles();
							buttonConfirm->setVisible(false);
							buttonPass->setVisible(false);
							buttonRecall->setVisible(false);
							buttonShuffle->setVisible(false);
							buttonExchange->setVisible(false);
							buttonLeave->setVisible(false);

							waitMessage = gui->addMessage("It's " + players[turn]->getName() + " turn now!", 1, 1024);
							gui->addMessage("Waiting " + players[turn]->getName() + " ...", 1, 1024);
							notifiedTurn = turn;
						}
						if (waitMessage && waitMessage->time <= 0) {
							waitMessage = nullptr;
							SDL_Thread *botThread = SDL_CreateThread(startBot, "starBot", NULL);
							SDL_DetachThread(botThread);
						}
					}
				}

				//	Draw Messages if any
				freeze = gui->drawMessage();

				//	Open Letter Selector
				if (selectingLetter) {
					SDL_Rect bg;
					bg.w = 400;
					bg.h = 300;
					bg.x = (1024 - bg.w) / 2;
					bg.y = (768 - bg.h) / 2;
					gui->drawObject(&bg, SDL_Color{ 0, 0, 0, 200 });
					gui->drawTexture(selectingTileTitleTexture, &selectingTileTitlePos);
					tileButtons->drawAll(gui->getRenderer());
					TileButton::mouseEvent(gui->getMainEvent(), mouseX, mouseY);
					freeze = true;
				}

				//	Mouse related events
				if (!freeze && gui->getHasEvent()) {
					if (myTurn) {
						int xrel = gui->getMainEvent()->motion.xrel;
						int yrel = gui->getMainEvent()->motion.yrel;
						if (!exchanging) {
							Tile * draggedTile = players[0]->dragTile(gui->getMainEvent(), board, mouseX, mouseY, xrel, yrel);
							if (draggedTile != nullptr)
								if (draggedTile->getLetter() == ' ') {
									blankTile = draggedTile;
									selectingLetter = true;
								}
						}
						else
							players[0]->dropTiles(gui->getMainEvent(), mouseX, mouseY);
					}
					Button::mouseEvent(gui->getMainEvent(), mouseX, mouseY);
				}
				gui->end();
				SDL_Delay(1);
			}

			//	Clean up
			delete board;
			delete tileBag;
			for (Player * player : players)
				delete player;
			players.clear();
			round = 0;
			gui->clearMessage();
			delete buttons;
			delete tileButtons;
			SDL_DestroyTexture(selectingTileTitleTexture);
		}
		
		//
		gui->end();
		SDL_Delay(1);
	}
}

void Game::button_beginner()
{
	difficulty = Beginner;
	status = Play;
}

void Game::button_intermediate()
{
	difficulty = Intermediate;
	status = Play;
}

void Game::button_skilled()
{
	difficulty = Skilled;
	status = Play;
}

void Game::button_expert()
{
	difficulty = Expert;
	status = Play;
}

void Game::button_unknown()
{
	difficulty = Unknown;
	status = Play;
}

void Game::button_confirm()
{
	int score = 0;
	std::string msg = players[0]->confirmMove(board, score);
	if (score > 0) {
		board->updateTiles();
		players[0]->refillTiles(tileBag);
		round++;
		msg = "Congratulations!\nYou " + msg;
	}
	gui->addMessage(msg, 0, 350);
}

void Game::button_pass()
{
	players[0]->recallTiles();
	round++;
}

void Game::button_recall()
{
	players[0]->recallTiles();
}

void Game::button_shuffle()
{
	players[0]->recallTiles();
	players[0]->shuffleTiles();
}

void Game::button_exchange()
{
	players[0]->recallTiles();
	
	buttonExchange->setVisible(false);
	buttonRecall->setVisible(false);
	buttonShuffle->setVisible(false);
	buttonPass->setVisible(false);
	buttonConfirm->setClickEvent(std::bind(&Game::button_exchangeConfirm, this));
	buttonLeave->setClickEvent(std::bind(&Game::button_exchangeLeave, this));
	exchanging = true;

	std::string msg = "Select the tiles you want to exchange";
	gui->addMessage(msg, 2, 400);
}

void Game::button_leave()
{
	status = Lobby;
}

void Game::button_exchangeConfirm()
{
	buttonExchange->setVisible(true);
	buttonRecall->setVisible(true);
	buttonShuffle->setVisible(true);
	buttonPass->setVisible(true);
	exchanging = false;
	buttonConfirm->setClickEvent(std::bind(&Game::button_confirm, this));
	buttonLeave->setClickEvent(std::bind(&Game::button_leave, this));
	if (players[0]->confirmExchange(tileBag))
		round++;
}

void Game::button_exchangeLeave()
{
	players[0]->undoExchange();
	buttonExchange->setVisible(true);
	buttonRecall->setVisible(true);
	buttonShuffle->setVisible(true);
	buttonPass->setVisible(true);
	exchanging = false;
	buttonConfirm->setClickEvent(std::bind(&Game::button_confirm, this));
	buttonLeave->setClickEvent(std::bind(&Game::button_leave, this));
}

void Game::button_selectCharForBlankTile(char letter)
{
	blankTile->setLetter(letter);
	selectingLetter = false;
	blankTile = nullptr;
}