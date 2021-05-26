#include "Player.h"
#include "Game.h"
#include "Cell.h"
#include "Dictionary.h"
#include "GUI.h"
#include "Move.h"
#include "Tile.h"
#include "TileBag.h"
#include <algorithm>
#include <ctime>

#define TIMEOUT 10	//	If the computer cannot make any words within 10 seconds, pass to the next player

Player::Player(int index, std::string name)
{
	this->index = index;
	this->name = name;
	score = 0;
	tileDragging = NULL;
}

Player::~Player()
{
	moves.clear();
	for (Tile * tile : tiles)
		delete tile;
	tiles.clear();
}

void Player::draw(GUI * gui)
{
	SDL_Rect rect;
	rect.w = 180;
	rect.h = 80;
	switch (index) {
	case 0:
		rect.x = 1024 - rect.w;
		rect.y = 708 - rect.h;
		break;
	case 1:
		rect.x = 0;
		rect.y = 0;
		break;
	case 2:
		rect.x = 1024 - rect.w;
		rect.y = 0;
		break;
	case 3:
		rect.x = 0;
		rect.y = 708 - rect.h;
		break;
	}
	//	Player Information
	SDL_RenderSetViewport(gui->getRenderer(), &rect);

	//	Panel
	gui->drawObject(NULL, SDL_Color{ 0, 0, 0, 85 });

	//	Player name
	SDL_Rect nameRect;
	SDL_Texture * textureName = gui->createTextTexture(&nameRect, gui->getFontMedium(), name.c_str(), SDL_Color{ 255, 255, 255, 255 });
	nameRect.x = (180 - nameRect.w) / 2;
	nameRect.y = 10;
	gui->drawTexture(textureName, &nameRect);
	SDL_DestroyTexture(textureName);

	//	Score
	SDL_Rect scoreArea;
	scoreArea.x = 0;
	scoreArea.y = 40;
	scoreArea.w = 180;
	scoreArea.h = 40;
	gui->drawObject(&scoreArea, SDL_Color{ 0, 0, 0, 85 });
	SDL_Rect scoreRect;
	SDL_Texture * textureScore = gui->createTextTexture(&scoreRect, gui->getFontMedium(), std::to_string(score).c_str(), SDL_Color{ 255, 255, 255, 255 });
	scoreRect.x = (180 - scoreRect.w) / 2;
	scoreRect.y = 50;
	gui->drawTexture(textureScore, &scoreRect);
	SDL_DestroyTexture(textureScore);
	SDL_RenderSetViewport(gui->getRenderer(), NULL);
}

void Player::draw7Tiles(TileBag * tileBag)
{
	for (int i = 0; i < 7; i++) {
		Tile * tile = tileBag->drawTile();
		tiles.push_back(tile);
	}
}

void Player::refillTiles(TileBag * tileBag)
{
	std::vector <Tile*>::iterator it;
	for (it = tiles.begin(); it != tiles.end();) {
		bool draggable = (*it)->getDraggable();
		if (!draggable) {
			it = tiles.erase(it);
		}
		else
			it++;
	}

	for (int i = tiles.size(); i < 7; i++) {
		Tile * tile = tileBag->drawTile();
		if (tile) tiles.push_back(tile);
	}
}

void Player::recallTiles()
{
	for (int i = 0; i < tiles.size(); i++) {
		tiles[i]->boundToCell(NULL);
	}
	moves.clear();
}

void Player::shuffleTiles() {
	//	Shuffle
	std::srand((int)time(NULL));
	std::random_shuffle(tiles.begin(), tiles.end());
}

std::string Player::getName()
{
	return name;
}

int Player::getScore()
{
	return score;
}

std::vector<Tile*> Player::getTiles()
{
	return tiles;
}

void Player::setName(std::string name)
{
	this->name = name;
}

void Player::setScore(int score)
{
	this->score = score;
}

void Player::addScore(int score)
{
	this->score += score;
}

void Player::addMove(Tile * tile, int col, int row)
{
	Move move;
	move.tile = tile;
	move.col = col;
	move.row = row;
	moves.push_back(move);
}

void Player::removeMove(Tile * tile)
{
	std::vector<Move>::iterator it;
	for (it = moves.begin(); it != moves.end(); it++) {
		if (it->tile == tile) {
			moves.erase(it);
			break;
		}
	}
}

std::string Player::confirmMove(Board * board, int &score)
{
	Move::Direction direction;
	std::string msg = "";
	bool isValid = board->validateMove(moves, direction, &msg);
	if (!isValid)
		return msg;
	score = board->calculateScore(moves, direction, &msg);
	this->score += score;
	return msg;
}

bool Player::confirmExchange(TileBag * tileBag)
{
	if (tilesDropping.size() == 0) return false;
	for (Tile * tile : tilesDropping) {
		tile->setVisible(true);
		Tile * newTile = tileBag->exchangeTile(tile);
		if (!newTile) return false;
		//	Swap
		std::vector <Tile*>::iterator it;
		for (it = tiles.begin(); it != tiles.end(); it++) {
			Tile * t = *it;
			if (t == tile) {
				tiles.erase(it);
				break;
			}
		}
		tiles.push_back(newTile);
	}
	tilesDropping.clear();
	return true;
}

void Player::undoExchange()
{
	for (Tile * tile : tilesDropping) {
		tile->setVisible(true);
	}
	tilesDropping.clear();
}

void Player::autoPlay(Board * board, TileBag * tileBag, std::string & computerMessage, int diff, int & round, int & roundforPassrate)
{
}

Tile * Bot::extractTileByLetter(std::vector<Tile*> &tiles, char letter) {
	if (letter != ' ') letter -= 32;
	for (int i = 0; i < tiles.size(); i++) {
		if (tiles[i]->getLetter() == letter) {
			Tile * target = tiles[i];
			tiles.erase(tiles.begin() + i);
			return target;
		}
	}
	//	If no then find blank tile
	for (int i = 0; i < tiles.size(); i++) {
		if (tiles[i]->getLetter() == ' ') {
			Tile * target = tiles[i];
			target->setLetter(letter);
			tiles.erase(tiles.begin() + i);
			return target;
		}
	}
	return nullptr;
}

void Bot::generatePass(int diff, int& round, int& hit, int& passrate, const int Allround)
{
	srand(time(NULL));
	switch (diff)
	{
		case 0:
			passrate = rand() % 3;
			hit = rand() % 3;
			if (round >= 2)
			{
				passrate = rand() % 1;
				hit = rand() % 1;
			}
			break;
		case 1:
			passrate = rand() % 4;
			hit = rand() % 4;
			if (round >= 5)
			{
				passrate = rand() % 2;
				hit = rand() % 2;
			}
			break;
		case 2:
			passrate = rand() % 6;
			hit = rand() % 6;
			if (round >= 8)
			{
				passrate = rand() % 2;
				hit = rand() % 2;
			}
			break;
		case 3:
			passrate = rand() % 10;
			hit = rand() % 10;
			if (round >= 12)
			{
				passrate = rand() % 2;
				hit = rand() % 2;
			}
			break;
		default:
			break;
	}
}

void Bot::autoPlay(Board * board, TileBag * tileBag, std::string & computerMessage, int diff, int& round, int& roundforPassrate)
{
	srand(time(NULL));

	int passrate = 0, hit = 10;
	if (round >= 2)
		generatePass(diff, roundforPassrate, hit, passrate, round);

	std::time_t t1 = std::time(0);
	Dictionary * dict = new Dictionary();
	std::vector<std::pair<Cell *, std::vector<std::string>>> list = board->getLockedCellRegex();

	//	If no cells on the board
	if (list.size() == 0) {
		//	Retrive characters currently owning
		std::string lettersOwning = "";
		int size = tiles.size();
		for (int i = 0; i < size; i++) {
			lettersOwning.push_back(tiles[i]->getLetter());
		}

		//	Get a random possible word from the dictionary
		std::string word = dict->getPossibleWordsB(lettersOwning);

		//	Place the word to the empty board
		bool result = attemptFirstWord(board, word, computerMessage);
		if (result) {
			refillTiles(tileBag);
			delete dict;
			return;
		}
	}

	for (int i = 0; i < list.size() && passrate != hit; i++) 
	{
		Cell * cell = list[i].first;
		std::string regexV = list[i].second[1];
		std::string regexH = list[i].second[0];

		auto words = dict->getPossibleWords(regexV, regexH);

		for (int j = 0; j < words.size(); j++) {

			std::time_t t2 = std::time(0);
			if ((t2 - t1) >= TIMEOUT)
			{
				autoExchange(tileBag);
				roundforPassrate = 0;
				delete dict;
				autoExchange(tileBag);
				computerMessage = " passed its turn!";
				return;
			}

			bool result = false;

			std::pair<int, std::pair<std::string, std::string>> collections = words[j];
			int direction = collections.first;
			std::pair<std::string, std::string> pair = collections.second;

			if (direction == 0) {
				result = attemptWordVectically(board, cell, pair, computerMessage);
			}
			else {
				result = attemptWordHorizontally(board, cell, pair, computerMessage);
			}

			//	If successful, then refill tiles and exit from the current function
			if (result) {
				refillTiles(tileBag);
				delete dict;
				return;
			}
		}
	}

	//	If fail to form a word, exchange tiles
	autoExchange(tileBag);

	//	Message when passing the turn
	computerMessage = " passed its turn!";
	delete dict;
}

void Bot::autoExchange(TileBag * tileBag) {
	for (Tile * tile : tiles) {
		char letter = tile->getLetter();
		if (letter != 'A' && letter != 'E' && letter != 'I' && letter != 'O' && letter != 'T')
			tilesDropping.push_back(tile);
	}
	confirmExchange(tileBag);
}

bool Bot::attemptFirstWord(Board * board, std::string word, std::string & computerMessage) {
	std::vector<Tile *> tempTiles(tiles);
	bool noTile = false;

	if (word.length() == 0) return false;
	int first = rand() % word.length();

	Cell * centerCell = board->getCell(7, 7);

	int seed = rand() % 2;

	if (seed == 0) {
		for (int l = 0; l < first; l++) {
			char letter = word[l];
			Tile * tile = extractTileByLetter(tempTiles, letter);
			if (!tile) {
				noTile = true;
				break;
			}
			addMove(tile, centerCell->col, centerCell->row - first + l);
			tile->boundToCell(board->getCell(centerCell->col, centerCell->row - first + l));
		}

		for (int l = 0; l < word.length() - first && !noTile; l++) {
			char letter = word[first + l];
			Tile * tile = extractTileByLetter(tempTiles, letter);
			if (!tile) {
				noTile = true;
				break;
			}
			addMove(tile, centerCell->col, centerCell->row + l);
			tile->boundToCell(board->getCell(centerCell->col, centerCell->row + l));
		}
	}
	else {
		for (int l = 0; l < first; l++) {
			char letter = word[l];
			Tile * tile = extractTileByLetter(tempTiles, letter);
			if (!tile) {
				noTile = true;
				break;
			}
			addMove(tile, centerCell->col - first + l, centerCell->row);
			tile->boundToCell(board->getCell(centerCell->col - first + l, centerCell->row));
		}

		for (int l = 0; l < word.length() - first && !noTile; l++) {
			char letter = word[first + l];
			Tile * tile = extractTileByLetter(tempTiles, letter);
			if (!tile) {
				noTile = true;
				break;
			}
			addMove(tile, centerCell->col + l, centerCell->row);
			tile->boundToCell(board->getCell(centerCell->col + l, centerCell->row));
		}
	}

	if (!noTile) {
		int score = 0;
		std::string imessage = confirmMove(board, score);
		if (score > 0) {
			computerMessage = imessage;
			return true;
		}
	}

	moves.clear();
	for (int l = 0; l < tiles.size(); l++)
		tiles[l]->boundToCell(NULL);

	return false;
}

bool Bot::attemptWordVectically(Board * board, Cell * cell, std::pair<std::string, std::string> word, std::string & computerMessage) {
	std::string aletter = cell->getTilePlaced() == nullptr ? "" : std::string(1, cell->getTilePlaced()->getLetter());
	std::vector<Tile *> tempTiles(tiles);
	bool noTile = false;
	
	for (int l = 0; l < word.first.length(); l++) {
		if (board->getCell(cell->col, cell->row - word.first.length() + l)->getLocked())
			continue;
		char letter = word.first[l];
		Tile * tile = extractTileByLetter(tempTiles, letter);
		if (!tile) {
			noTile = true;
			break;
		}
		addMove(tile, cell->col, cell->row - word.first.length() + l);
		tile->boundToCell(board->getCell(cell->col, cell->row - word.first.length() + l));
	}

	for (int l = 0; l < word.second.length() && !noTile; l++) {
		if (board->getCell(cell->col, cell->row + 1 + l)->getLocked())
			continue;
		char letter = word.second[l];
		Tile * tile = extractTileByLetter(tempTiles, letter);
		if (!tile) {
			noTile = true;
			break;
		}
		addMove(tile, cell->col, cell->row + 1 + l);
		tile->boundToCell(board->getCell(cell->col, cell->row + 1 + l));
	}

	if (!noTile) {
		int score = 0;
		std::string imessage = confirmMove(board, score);
		if (score > 0) {
			computerMessage = imessage;
			return true;
		}
	}

	moves.clear();
	for (int l = 0; l < tiles.size(); l++)
		tiles[l]->boundToCell(NULL);

	return false;
}

bool Bot::attemptWordHorizontally(Board * board, Cell * cell, std::pair<std::string, std::string> word, std::string & computerMessage) {
	std::string aletter = cell->getTilePlaced() == nullptr ? "" : std::string(1, cell->getTilePlaced()->getLetter());
	std::vector<Tile *> tempTiles(tiles);
	bool noTile = false;

	for (int l = 0; l < word.first.length(); l++) {
		if (board->getCell(cell->col - word.first.length() + l, cell->row)->getLocked())
			continue;
		char letter = word.first[l];
		Tile * tile = extractTileByLetter(tempTiles, letter);
		if (!tile) {
			noTile = true;
			break;
		}
		addMove(tile, cell->col - word.first.length() + l, cell->row);
		tile->boundToCell(board->getCell(cell->col - word.first.length() + l, cell->row));
	}

	for (int l = 0; l < word.second.length() && !noTile; l++) {
		if (board->getCell(cell->col + 1 + l, cell->row)->getLocked())
			continue;
		char letter = word.second[l];
		Tile * tile = extractTileByLetter(tempTiles, letter);
		if (!tile) {
			noTile = true;
			break;
		}
		addMove(tile, cell->col + 1 + l, cell->row);
		tile->boundToCell(board->getCell(cell->col + 1 + l, cell->row));
	}

	if (!noTile) {
		int score = 0;
		std::string imessage = confirmMove(board, score);
		if (score > 0) {
			computerMessage = imessage;
			return true;
		}
	}

	moves.clear();
	for (int l = 0; l < tiles.size(); l++)
		tiles[l]->boundToCell(NULL);

	return false;
}

/*	GUI Related Functions */

void Player::drawTiles(GUI * gui)
{
	SDL_Color textColor = { 0, 61, 82, 255 };

	int offsetX = (1024 - 7 * 58) / 2 + 6;
	int offsetY = (768 - 60) + (60 - 48) / 2;

	bool dragging = false;
	for (int i = 0; i < tiles.size(); i++) {
		Tile * tile = tiles[i];

		//	The dragging one should be the latest one to render in order to avoid overlapping
		if (tile == tileDragging) {
			dragging = true;
			continue;
		}

		//	Set tiles position
		if (!tile->getOnBoard()) {
			tile->x = tile->originalX = i * 58 + offsetX;
			tile->y = tile->originalY = offsetY;
			tile->w = tile->h = 48;
		}

		tile->draw(gui);
	}

	if (dragging) {
		tileDragging->draw(gui);
	}
}

Tile * Player::dragTile(SDL_Event * event, Board * board, int mouseX, int mouseY, int relX, int relY)
{
	if (event->button.button != SDL_BUTTON_LEFT) return nullptr;
	if (event->type == SDL_MOUSEBUTTONDOWN) {
		tileDragging = findPressedTile(mouseX, mouseY);
		if (tileDragging != NULL) {
			if (!tileDragging->getDraggable()) {
				tileDragging = NULL;
				return nullptr;
			}
			tileDragging->boundToCell(NULL);
		}
	}
	else if (event->type == SDL_MOUSEBUTTONUP) {
		if (tileDragging != NULL) {
			//	Retrive the point at the center of the tile
			int tileCenterX = tileDragging->x + 24;
			int tileCenterY = tileDragging->y + 24;
			Cell * cell = board->findCell(tileCenterX, tileCenterY);

			//	Check if dragged to a empty cell
			if (cell != NULL && !cell->getOccupied()) {
				removeMove(tileDragging);
				tileDragging->boundToCell(cell);
				addMove(tileDragging, tileDragging->col, tileDragging->row);
				return tileDragging;
			}
			else {
				//	Check if dragged to swap with another tile
				swapTile(mouseX, mouseY, tileDragging);

				//	If not swapped then remove the move
				removeMove(tileDragging);
			}

			tileDragging = NULL;
		}
	}
	else if (event->type == SDL_MOUSEMOTION) {
		if (tileDragging != NULL) {
			tileDragging->x += relX;
			tileDragging->y += relY;
		}
	}
	return nullptr;
}

void Player::dropTiles(SDL_Event * event, int mouseX, int mouseY)
{
	if (event->button.button != SDL_BUTTON_LEFT) return;
	if (event->type == SDL_MOUSEBUTTONDOWN) {
		tileDropping = findPressedTile(mouseX, mouseY);
		if (tileDropping != NULL) {
			if (tileDropping->getOnBoard()) {
				tileDropping = NULL;
				return;
			}
		}
	}
	else if (event->type == SDL_MOUSEBUTTONUP) {
		if (tileDropping != NULL) {
			tileDropping->setVisible(false);
			tilesDropping.push_back(tileDropping);
		}
	}
}

Tile * Player::findPressedTile(int mouseX, int mouseY)
{
	Tile * pressedTile = nullptr;
	for (Tile * tile : tiles) {
		if (!tile->getVisible()) continue;
		int tx = tile->x;
		int ty = tile->y;
		int tw = tile->w;
		int th = tile->h;
		if (mouseX >= tx && mouseX <= tx + tw && mouseY >= ty && mouseY <= ty + th) {
			pressedTile = tile;
			break;
		}
	}
	return pressedTile;
}

bool Player::swapTile(int mouseX, int mouseY, Tile * swappingTile)
{
	int min = 40;
	int targetTile = -1;

	for (int i = tiles.size() - 1; i >= 0; i--) {
		Tile * tile = tiles[i];
		if (tile == swappingTile) continue;
		int tx = tile->x + tile->w / 2;
		int ty = tile->y + tile->h / 2;
		int distance = abs(mouseX - tx) + abs(mouseY - ty);
		if (distance < min && !tile->getOnBoard()) {
			min = distance;
			targetTile = i;
		}
	}

	if (min < 40) {
		int swapTile = -1;
		for (int i = tiles.size() - 1; i >= 0; i--) {
			Tile * tile = tiles[i];
			if (tile == swappingTile) {
				swapTile = i;
				break;
			}
		}
		std::swap(tiles[targetTile], tiles[swapTile]);
		return true;
	}
	return false;	// Too far away
}