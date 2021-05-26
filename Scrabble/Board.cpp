#include "Board.h"
#include "Cell.h"
#include "Dictionary.h"
#include "GUI.h"
#include "Tile.h"
#include <algorithm>
#include <ctime>
#include <string>

int extraPointCells[][3] = {	// [0] = x, [1] = y, [2] = Extra Point Type
	{ 3, 0, DOUBLE_LETTER_SCORES },
	{ 11, 0, DOUBLE_LETTER_SCORES },
	{ 6, 2, DOUBLE_LETTER_SCORES },
	{ 8, 2, DOUBLE_LETTER_SCORES },
	{ 0, 3, DOUBLE_LETTER_SCORES },
	{ 7, 3, DOUBLE_LETTER_SCORES },
	{ 14, 3, DOUBLE_LETTER_SCORES },
	{ 2, 6, DOUBLE_LETTER_SCORES },
	{ 6, 6, DOUBLE_LETTER_SCORES },
	{ 8, 6, DOUBLE_LETTER_SCORES },
	{ 12, 6, DOUBLE_LETTER_SCORES },
	{ 3, 7, DOUBLE_LETTER_SCORES },
	{ 11, 7, DOUBLE_LETTER_SCORES },
	{ 2, 8, DOUBLE_LETTER_SCORES },
	{ 6, 8, DOUBLE_LETTER_SCORES },
	{ 8, 8, DOUBLE_LETTER_SCORES },
	{ 12, 8, DOUBLE_LETTER_SCORES },
	{ 0, 11, DOUBLE_LETTER_SCORES },
	{ 7, 11, DOUBLE_LETTER_SCORES },
	{ 14, 11, DOUBLE_LETTER_SCORES },
	{ 6, 12, DOUBLE_LETTER_SCORES },
	{ 8, 12, DOUBLE_LETTER_SCORES },
	{ 3, 14, DOUBLE_LETTER_SCORES },
	{ 11, 14, DOUBLE_LETTER_SCORES },

	{ 5, 1, TRIPLE_LETTER_SCORES },
	{ 9, 1, TRIPLE_LETTER_SCORES },
	{ 1, 5, TRIPLE_LETTER_SCORES },
	{ 5, 5, TRIPLE_LETTER_SCORES },
	{ 9, 5, TRIPLE_LETTER_SCORES },
	{ 13, 5, TRIPLE_LETTER_SCORES },
	{ 1, 9, TRIPLE_LETTER_SCORES },
	{ 5, 9, TRIPLE_LETTER_SCORES },
	{ 9, 9, TRIPLE_LETTER_SCORES },
	{ 13, 9, TRIPLE_LETTER_SCORES },
	{ 5, 13, TRIPLE_LETTER_SCORES },
	{ 9, 13, TRIPLE_LETTER_SCORES },

	{ 1, 1, DOUBLE_WORD_SCORES },
	{ 13, 1, DOUBLE_WORD_SCORES },
	{ 2, 2, DOUBLE_WORD_SCORES },
	{ 12, 2, DOUBLE_WORD_SCORES },
	{ 3, 3, DOUBLE_WORD_SCORES },
	{ 11, 3, DOUBLE_WORD_SCORES },
	{ 4, 4, DOUBLE_WORD_SCORES },
	{ 10, 4, DOUBLE_WORD_SCORES },
	{ 4, 10, DOUBLE_WORD_SCORES },
	{ 10, 10, DOUBLE_WORD_SCORES },
	{ 3, 11, DOUBLE_WORD_SCORES },
	{ 11, 11, DOUBLE_WORD_SCORES },
	{ 2, 12, DOUBLE_WORD_SCORES },
	{ 12, 12, DOUBLE_WORD_SCORES },
	{ 1, 13, DOUBLE_WORD_SCORES },
	{ 13, 13, DOUBLE_WORD_SCORES },

	{ 0, 0, TRIPLE_WORD_SCORES },
	{ 7, 0, TRIPLE_WORD_SCORES },
	{ 14, 0, TRIPLE_WORD_SCORES },
	{ 0, 7, TRIPLE_WORD_SCORES },
	{ 14, 7, TRIPLE_WORD_SCORES },
	{ 0, 14, TRIPLE_WORD_SCORES },
	{ 7, 14, TRIPLE_WORD_SCORES },
	{ 14, 14, TRIPLE_WORD_SCORES },

	{ 7, 7, DOUBLE_WORD_SCORES },
};

Board::Board(bool shuffleBonus)
{
	boardTexture = NULL;

	//	Initialize 15 x 15 cells
	cells = std::vector <std::vector<Cell*>>(15, std::vector<Cell*>(15));
	int offsetX = (1024 - 15 * 42) / 2 + 2;
	int offsetY = ((768 - 60) - 15 * 42) / 2 + 2;
	for (int i = 0; i < 15; i++) {
		for (int j = 0; j < 15; j++) {
			cells[j][i] = new Cell(j, i, j * 42 + offsetX, i * 42 + offsetY, 38, 38);
		}
	}
	
	//	Set Extra Points
	if (!shuffleBonus) {
		for (int i = 0; i < 61; i++) {
			int x = extraPointCells[i][0];
			int y = extraPointCells[i][1];
			ExtraPoint extraPoint = (ExtraPoint)extraPointCells[i][2];
			cells[x][y]->setExtraPoint(extraPoint);
		}
	}
	else {
		for (int i = 0; i < 15; i++) {
			for (int j = 0; j < 15; j++) {
				if (i == 7 && j == 7) continue;
				int bonusType = rand() % 5;
				cells[j][i]->setExtraPoint((ExtraPoint)bonusType);
			}
		}
		ExtraPoint extraPoint = (ExtraPoint)extraPointCells[60][2];
		cells[7][7]->setExtraPoint(extraPoint);
	}

	//	Initialize dictionary
	dictionary = new Dictionary();
}

Board::~Board()
{
	SDL_DestroyTexture(boardTexture);
	for (int i = 0; i < 15; i++) {
		for (int j = 0; j < 15; j++) {
			delete cells[j][i];
		}
	}
	cells.clear();
	delete dictionary;
}

bool compare(const Move m1, const Move m2)
{
	return m1.row * 15 + m1.col < m2.row * 15 + m2.col;
}

bool Board::validateMove(std::vector<Move>& moves, Move::Direction & direction, std::string * resultMessage)
{
	std::sort(moves.begin(), moves.end(), compare);
	
	//	Check if any move
	if (moves.size() == 0) {
		resultMessage->append("You didn't drag any tiles onto the board!");
		return false;
	}

	//	Check if tiles placed is either in horizontal or vertical direction
	bool horizontal, vertical;
	horizontal = vertical = false;

	int movesSize = moves.size();
	int startCol = moves[0].col;
	int startRow = moves[0].row;
	int endCol = moves[movesSize - 1].col;
	int endRow = moves[movesSize - 1].row;

	for (int i = startCol; i <= endCol && startCol != endCol; i++) {
		horizontal = true;
		Cell * c = getCell(i, startRow);
		if (!c->getOccupied() && !c->getLocked()) {
			horizontal = false;
			break;
		}
	}

	for (int i = startRow; i <= endRow && startRow != endRow; i++) {
		vertical = true;
		Cell * c = getCell(startCol, i);
		if (!c->getOccupied() && !c->getLocked()) {
			vertical = false;
			break;
		}
	}

	if (!horizontal && !vertical) {
		resultMessage->append("The tiles you placed are not forming a straight line");
		return false;
	}

	if (horizontal) direction = Move::DIRECTION_HORIZONTAL;
	if (vertical) direction = Move::DIRECTION_VERTICAL;

	//	The star at the center
	if (!cells[7][7]->getOccupied()) {
		resultMessage->append("At least one tile should be placed on the star at the center");
		return false;
	}

	//	Check if touched tiles previously placed on the board
	if (cells[7][7]->getLocked()) {
	bool touched = false;
	for (Move move : moves) {

		//	North
		int nCol = move.col;
		int nRow = move.row - 1;
		//	East
		int eCol = move.col + 1;
		int eRow = move.row;
		//	South
		int sCol = move.col;
		int sRow = move.row + 1;
		//	West
		int wCol = move.col - 1;
		int wRow = move.row;

		//	Touched
		bool northTouched = nRow < 0 || cells[nCol][nRow]->getLocked();
		bool eastTouched = eCol > 14 || cells[eCol][eRow]->getLocked();
		bool southTouched = sRow > 14 || cells[sCol][sRow]->getLocked();
		bool westTouched = wCol < 0 || cells[wCol][wRow]->getLocked();

		if (northTouched || eastTouched || southTouched || westTouched) {
			touched = true;
			break;
		}
	}
	if (!touched) {
		resultMessage->append("At least one of your tiles placed should touch a tile already existed on the board!");
		return false;
	}
	}
	// Valid move
	return true;
}

int Board::calculateScore(std::vector<Move>& moves, Move::Direction & direction, std::string * resultMessage)
{
	int totalScore = 0;

	//	BINGO, used all 7 tiles
	if (moves.size() == 7) {
		totalScore += 50;
		resultMessage->append("got a BINGO and\n");
	}

	Move::Direction reverseDirection = direction == Move::DIRECTION_HORIZONTAL ? Move::DIRECTION_VERTICAL : Move::DIRECTION_HORIZONTAL;
	std::string validWords = "";
	std::string invalidWords = "";

	for (int i = 0; i < moves.size(); i++) {
		if (i == 0) {
			//	Check both vertical and horizontal direction for the first tile
			std::string firstWordA, firstWordB;
			int wordScoreA = getWholeWordAndScore(moves[i].tile, direction, &firstWordA);
			int wordScoreB = getWholeWordAndScore(moves[i].tile, reverseDirection, &firstWordB);

			if (firstWordA.length() > 1) {
				if (wordScoreA > 0)
					validWords += "\n" + firstWordA;
				else
					invalidWords += "\n" + firstWordA;
			}
			if (firstWordB.length() > 1) {
				if (wordScoreB > 0)
					validWords += "\n" + firstWordB;
				else
					invalidWords += "\n" + firstWordB;
			}
			//	Add score to total score
			totalScore += wordScoreA + wordScoreB;
		}
		else {
			std::string word;
			int wordScore = getWholeWordAndScore(moves[i].tile, reverseDirection, &word);
			if (word.length() > 1) {
				if (wordScore > 0)
					validWords += "\n" + word;
				else
					invalidWords += "\n" + word;
			}
			totalScore += wordScore;
		}
	}

	if (invalidWords.length() == 0) {
		if (totalScore > 0) {
			std::string suffix = "";
			if (totalScore > 1)
				suffix += "s";
			//	Extra 50 scores for bingo
			if (moves.size() == 7) {
				suffix += " [BINGO!]";
				totalScore += 50;
			}
			resultMessage->append("awarded " + std::to_string(totalScore) + " score" + suffix + " for the following word(s)" + validWords);
			for (Move move : moves) {
				move.tile->setDraggable(false);
				cells[move.tile->col][move.tile->row]->setLocked(true);
			}
			moves.clear();
			return totalScore;
		}
		else {
			resultMessage->append("Your tiles are not forming any words!");
			return 0;
		}
	}
	else {
		resultMessage->append("The following word(s) are invalid!" + invalidWords);
		return 0;
	}
}

int Board::getWholeWordAndScore(Tile * tile, Move::Direction checkDirection, std::string * word)
{
	int score = 0;		//	Score to be returned
	int end = checkDirection == Move::DIRECTION_HORIZONTAL ? tile->col : tile->row;
	int doubleWordScore = 0;
	int tripleWordScore = 0;

	for (int i = 0; i < 15; i++) {
		Tile* iTile = checkDirection == Move::DIRECTION_HORIZONTAL ? cells[i][tile->row]->getTilePlaced() : cells[tile->col][i]->getTilePlaced();
		if (iTile == NULL) {
			if (i > end) {
				break;
			}
			*word = "";
			score = 0;
			continue;
		}
		char letter = iTile->getLetter();
		*word += (letter + 32);	//	To lower case

		//	Get the score for the letter
		int tileScore = iTile->getScore();
		//	Ensure the extra point is not yet used
		if (iTile->getDraggable()) {
			//	Get the extra point from the Cells
			Cell * cell = cells[iTile->col][iTile->row];
			switch (cell->getExtraPoint()) {
			case DOUBLE_LETTER_SCORES:
				score += tileScore * 2;
				break;
			case TRIPLE_LETTER_SCORES:
				score += tileScore * 3;
				break;
			case DOUBLE_WORD_SCORES:
				score += tileScore;
				doubleWordScore++;
				break;
			case TRIPLE_WORD_SCORES:
				score += tileScore;
				tripleWordScore++;
				break;
			case NO_EXTRA_POINT:
				score += tileScore;
				break;
			}
		}
		else
			score += tileScore;
	}
	int multiplier = (doubleWordScore * 2) + (tripleWordScore * 3);
	if (multiplier > 0)
		score *= multiplier;

	//	Check word with length > 1
	if (word->length() > 1) {
		// Check dictionary
		bool valid = dictionary->checkWordValidity(word->c_str());
		if (!valid) {
			score = 0;	//	Reset the score if it is a invalid word
		}
	}
	else
		score = 0;	//	Reset the score if it is just a single letter instead of a word
	return score;
}

void Board::draw(GUI * gui)
{
	//	Draw the board for once
	if (!boardTexture) {
		TTF_Font * fontSmall = gui->getFontSmall();
		TTF_SetFontStyle(fontSmall, TTF_STYLE_BOLD);
		boardTexture = SDL_CreateTexture(gui->getRenderer(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, 1024, 768);
		SDL_SetTextureBlendMode(boardTexture, SDL_BLENDMODE_BLEND);
		SDL_SetRenderTarget(gui->getRenderer(), boardTexture);
		gui->drawBackground();
		for (int i = 0; i < 15; i++) {
			for (int j = 0; j < 15; j++) {
				ExtraPoint extraPoint = cells[j][i]->getExtraPoint();
				if (extraPoint == NO_EXTRA_POINT) {
					gui->drawObject(cells[j][i], SDL_Color{ 0, 0, 0, 60 });
				}
				else {
					std::string s = "";
					SDL_Color bgColor = { 205, 181, 108, 255 };
					SDL_Color color = { 255, 255, 255, 255 };
					switch (extraPoint) {
					case DOUBLE_LETTER_SCORES:
						bgColor = { 92, 163, 224, 255 };
						color = { 156, 196, 236, 255 };
						s = "DL";
						break;
					case TRIPLE_LETTER_SCORES:
						bgColor = { 141, 171, 89, 255 };
						color = { 176, 197, 140, 255 };
						s = "TL";
						break;
					case DOUBLE_WORD_SCORES:
						bgColor = { 219, 64, 64, 255 };
						color = { 231, 126, 126, 255 };
						s = "DW";
						break;
					case TRIPLE_WORD_SCORES:
						bgColor = { 214, 110, 58, 255 };
						color = { 227, 155, 120, 255 };
						s = "TW";
						break;
					}
					gui->drawObject(cells[j][i], bgColor);
					//	Text
					SDL_Rect rect;
					SDL_Texture * texture = gui->createWrappedTextTexture(&rect, fontSmall, s.c_str(), color, cells[j][i]->w);
					rect.x = cells[j][i]->x + (cells[j][i]->w - rect.w) / 2;
					rect.y = cells[j][i]->y + (cells[j][i]->h - rect.h) / 2;
					gui->drawTexture(texture, &rect);
					SDL_DestroyTexture(texture);
				}
			}
		}
		// Star for center tile
		gui->drawObject(cells[7][7], SDL_Color{ 219, 64, 64, 255 });
		SDL_Rect rect;
		std::wstring s = L"\uf005";
		SDL_Texture * texture = gui->createUnicodeTextTexture(&rect, gui->getFontAwesome(), (Uint16*)s.c_str(), SDL_Color{ 231, 126, 126, 255 });
		rect.x = cells[7][7]->x + (cells[7][7]->w - rect.w) / 2;
		rect.y = cells[7][7]->y + (cells[7][7]->h - rect.h) / 2;
		gui->drawTexture(texture, &rect);
		SDL_DestroyTexture(texture);
		SDL_SetRenderTarget(gui->getRenderer(), NULL);
		TTF_SetFontStyle(fontSmall, TTF_STYLE_NORMAL);

		boardGUI = gui;
	}
	gui->drawTexture(boardTexture, NULL);
}

void Board::updateTiles()
{
	SDL_SetRenderTarget(boardGUI->getRenderer(), boardTexture);
	for (int i = 0; i < 15; i++) {
		for (int j = 0; j < 15; j++) {
			Cell * cell = cells[j][i];
			Tile * tile = cell->getTilePlaced();
			if (tile && !tile->getDraggable()) {
				tile->draw(boardGUI);
			}
		}
	}
	SDL_SetRenderTarget(boardGUI->getRenderer(), NULL);
}

Cell * Board::getCell(int col, int row)
{
	return cells[col][row];
}

Cell * Board::findCell(int mouseX, int mouseY)
{
	int min = abs(mouseX - cells[0][0]->x) + abs(mouseY - cells[0][0]->y);
	Cell * cell = nullptr;
	for (int i = 0; i < 15; i++) {
		for (int j = 0; j < 15; j++) {
			int tx = cells[j][i]->x + 16;
			int ty = cells[j][i]->y + 16;
			int distance = abs(mouseX - tx) + abs(mouseY - ty);
			if (distance < min) {
				min = distance;
				cell = cells[j][i];
			}
		}
	}

	if (min < 40)	// Max allowance of the distance from the mouse
		return cell;
	return nullptr;	// Too far away
}

std::vector<std::pair<Cell *, std::vector<std::string>>> Board::getLockedCellRegex() {
	std::vector<std::pair<Cell *, std::vector<std::string>>> list;
	for (int i = 0; i < 15; i++) {
		for (int j = 0; j < 15; j++) {
			Cell * cell = getCell(j, i);
			if (!cell->getLocked()) continue;
			//	To

			//	LEFT
			std::string lRegex = "";
			int lSpace = 0;
			for (int j = cell->col - 1; j >= 0; j--) {
				Cell * jCell = getCell(j, cell->row);
				if (!jCell->getLocked())
					lSpace++;
				else {
					char jLetter = jCell->getTilePlaced()->getLetter();
					std::string jLetter_str(1, jLetter);
					if (lSpace <= 0) {
						lRegex = jLetter_str + lRegex;
						continue;
					}
					lRegex = jLetter_str + ".{" + std::to_string(lSpace) + "," + std::to_string(lSpace) + "}" + lRegex;
					lSpace = 0;
				}
			}
			if (lSpace > 0)
				lRegex = ".{0," + std::to_string(lSpace) + "}" + lRegex;

			//	RIGHT
			std::string rRegex = "";
			int rSpace = 0;
			for (int j = cell->col + 1; j <= 14; j++) {
				Cell * jCell = getCell(j, cell->row);
				if (!jCell->getLocked())
					rSpace++;
				else {
					char jLetter = jCell->getTilePlaced()->getLetter();
					std::string jLetter_str(1, jLetter);
					if (rSpace <= 0) {
						rRegex = rRegex + jLetter_str;
						continue;
					}
					rRegex = rRegex + ".{" + std::to_string(rSpace) + "," + std::to_string(rSpace) + "}" + jLetter_str;
					rSpace = 0;
				}
			}
			if (rSpace > 0)
				rRegex = rRegex + ".{0," + std::to_string(rSpace) + "}";

			//	TOP
			std::string tRegex = "";
			int tSpace = 0;
			for (int j = cell->row - 1; j >= 0; j--) {
				Cell * jCell = getCell(cell->col, j);
				if (!jCell->getLocked())
					tSpace++;
				else {
					char jLetter = jCell->getTilePlaced()->getLetter();
					std::string jLetter_str(1, jLetter);
					if (tSpace <= 0) {
						tRegex = jLetter_str + tRegex;
						continue;
					}
					tRegex = jLetter_str + ".{" + std::to_string(tSpace) + "," + std::to_string(tSpace) + "}" + tRegex;
					tSpace = 0;
				}
			}
			if (tSpace > 0)
				tRegex = ".{0," + std::to_string(tSpace) + "}" + tRegex;

			//	BOTTOM
			std::string bRegex = "";
			int bSpace = 0;
			for (int j = cell->row + 1; j <= 14; j++) {
				Cell * jCell = getCell(cell->col, j);
				if (!jCell->getLocked())
					bSpace++;
				else {
					char jLetter = jCell->getTilePlaced()->getLetter();
					std::string jLetter_str(1, jLetter);
					if (bSpace <= 0) {
						bRegex = bRegex + jLetter_str;
						continue;
					}
					bRegex = bRegex + ".{" + std::to_string(bSpace) + "," + std::to_string(bSpace) + "}" + jLetter_str;
					bSpace = 0;
				}
			}
			if (bSpace > 0)
				bRegex = bRegex + ".{0," + std::to_string(bSpace) + "}";

			//	
			boost::regex regex1("\.{[0-9],[0-9]}[A-Z]*|[A-Z]+");
			boost::regex regex2("[A-Z]*\.{[0-9],[0-9]}|[A-Z]+");

			//	T ITERATE
			boost::sregex_token_iterator titer(tRegex.begin(), tRegex.end(), regex1, 0);
			boost::sregex_token_iterator tend;
			std::vector<std::string> tCollection(titer, tend);

			for (int j = 1; j < tCollection.size(); j++) {
				char l = tCollection[j][4];
				if (l <= '1') break;
				tRegex += '|';
				char n1 = tCollection[j][2];
				n1 = '0';
				char n2 = tCollection[j][4];
				n2--;
				tCollection[j][2] = n1;
				tCollection[j][4] = n2;

				for (int k = j; k < tCollection.size(); k++) {
					tRegex += tCollection[k];
				}
			}

			//	B ITERATE
			boost::sregex_token_iterator biter(bRegex.begin(), bRegex.end(), regex2, 0);
			boost::sregex_token_iterator bend;
			std::vector<std::string> bCollection(biter, bend);

			for (int j = bCollection.size() - 2; j >= 0; j--) {
				char n1 = bCollection[j][bCollection[j].length() - 4];
				n1 = '0';
				char n2 = bCollection[j][bCollection[j].length() - 2];
				n2--;
				if (n2 == '0') break;
				bRegex += '|';
				bCollection[j][bCollection[j].length() - 4] = n1;
				bCollection[j][bCollection[j].length() - 2] = n2;
				for (int k = 0; k <= j; k++) {
					bRegex += bCollection[k];
				}
			}

			//	L ITERATE
			boost::sregex_token_iterator liter(lRegex.begin(), lRegex.end(), regex1, 0);
			boost::sregex_token_iterator lend;
			std::vector<std::string> lCollection(liter, lend);

			for (int j = 1; j < lCollection.size(); j++) {
				char l = lCollection[j][4];
				if (l <= '1') break;
				lRegex += '|';
				char n1 = lCollection[j][2];
				n1 = '0';
				char n2 = lCollection[j][4];
				n2--;
				lCollection[j][2] = n1;
				lCollection[j][4] = n2;

				for (int k = j; k < lCollection.size(); k++) {
					lRegex += lCollection[k];
				}
			}

			//	R ITERATE
			boost::sregex_token_iterator riter(rRegex.begin(), rRegex.end(), regex2, 0);
			boost::sregex_token_iterator rend;
			std::vector<std::string> rCollection(riter, rend);

			for (int j = rCollection.size() - 2; j >= 0; j--) {
				char n1 = rCollection[j][rCollection[j].length() - 4];
				n1 = '0';
				char n2 = rCollection[j][rCollection[j].length() - 2];
				n2--;
				if (n2 == '0') break;
				rRegex += '|';
				rCollection[j][rCollection[j].length() - 4] = n1;
				rCollection[j][rCollection[j].length() - 2] = n2;
				for (int k = 0; k <= j; k++) {
					rRegex += rCollection[k];
				}
			}

			lRegex = "(" + lRegex + ")";
			rRegex = "(" + rRegex + ")";
			std::string regexH = lRegex + std::string(1, cell->getTilePlaced()->getLetter()) + rRegex;
			tRegex = "(" + tRegex + ")";
			bRegex = "(" + bRegex + ")";
			std::string regexV = tRegex + std::string(1, cell->getTilePlaced()->getLetter()) + bRegex;

			std::vector<std::string> regex;
			regex.push_back(regexH);
			regex.push_back(regexV);
			list.push_back(std::pair<Cell *, std::vector<std::string>>(cell, regex));
		}
	}
	srand(time(NULL));
	std::random_shuffle(list.begin(), list.end());

	return list;
}