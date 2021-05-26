#include "Tile.h"
#include "Cell.h"
#include <string>

Tile::Tile(char letter, int score)
{
	this->letter = letter;
	this->score = score;
	col = row = -1;
	onBoard = false;
	draggable = true;
	visible = true;
	boundedCell = nullptr;

	letterTexture = nullptr;
	scoreTexture = nullptr;
}

Tile::~Tile()
{
	SDL_DestroyTexture(letterTexture);
	SDL_DestroyTexture(scoreTexture);
	SDL_DestroyTexture(s_letterTexture);
	SDL_DestroyTexture(s_scoreTexture);
}

char Tile::getLetter()
{
	return letter;
}

int Tile::getScore()
{
	return score;
}

bool Tile::getOnBoard()
{
	return onBoard;
}

bool Tile::getDraggable()
{
	return draggable;
}

bool Tile::getVisible()
{
	return visible;
}

void Tile::setLetter(char letter)
{
	this->letter = letter;
	SDL_DestroyTexture(letterTexture);
	SDL_DestroyTexture(s_letterTexture);
	letterTexture = s_letterTexture = nullptr;
}

void Tile::setOnBoard(bool onBoard)
{
	this->onBoard = onBoard;
}

void Tile::setDraggable(bool draggable)
{
	this->draggable = draggable;
}

void Tile::setVisible(bool visible)
{
	this->visible = visible;
}

void Tile::boundToCell(Cell * cell)
{
	if (boundedCell) {
		boundedCell->setOccupied(false);
		boundedCell->setTilePlaced(nullptr);
		boundedCell = nullptr;
	}
	if (!cell) {
		w = 48;
		h = 48;
		x -= 5;
		y -= 5;
		col = row = -1;
		onBoard = false;
	}
	else {
		x = cell->x;
		y = cell->y;
		w = cell->w;
		h = cell->h;
		col = cell->col;
		row = cell->row;
		onBoard = true;
		cell->setOccupied(true);
		cell->setTilePlaced(this);
		boundedCell = cell;
	}
}

SDL_Color textColor = { 0, 61, 82, 255 };
void Tile::draw(GUI * gui)
{
	if (!visible) return;
	//	Draw Rect
	gui->drawObject(this, SDL_Color{ 255, 231, 158, 255 });

	//	Get Letter
	char c = getLetter();

	//	Get Score
	std::string score = std::to_string(getScore());

	//	Render
	if (getOnBoard()) {
		//	Letter
		if (!s_letterTexture)
			s_letterTexture = gui->createCharTexture(&s_letterTextureRect, gui->getFontLarge(), &c, textColor);

		s_letterTextureRect.x = x + (w - s_letterTextureRect.w) / 2;
		s_letterTextureRect.y = y + (h - s_letterTextureRect.h) / 2;
		gui->drawTexture(s_letterTexture, &s_letterTextureRect);

		//	Score
		if (!s_scoreTexture)
			s_scoreTexture = gui->createTextTexture(&s_scoreTextureRect, gui->getFontExtraSmall(), score.c_str(), textColor);

		s_scoreTextureRect.x = x + w - s_scoreTextureRect.w - 2;
		s_scoreTextureRect.y = y + h - s_scoreTextureRect.h;
		gui->drawTexture(s_scoreTexture, &s_scoreTextureRect);
	}
	else {
		//	Letter
		if (!letterTexture)
			letterTexture = gui->createCharTexture(&letterTextureRect, gui->getFontExtraLarge(), &c, textColor);

		letterTextureRect.x = x + (w - letterTextureRect.w) / 2;
		letterTextureRect.y = y + (h - letterTextureRect.h) / 2;
		gui->drawTexture(letterTexture, &letterTextureRect);

		//	Score
		if (!scoreTexture)
			scoreTexture = gui->createTextTexture(&scoreTextureRect, gui->getFontSmall(), score.c_str(), textColor);

		scoreTextureRect.x = x + w - scoreTextureRect.w - 2;
		scoreTextureRect.y = y + h - scoreTextureRect.h;
		gui->drawTexture(scoreTexture, &scoreTextureRect);
	}
}