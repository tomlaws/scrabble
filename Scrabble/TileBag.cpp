#include "TileBag.h"
#include "Tile.h"
#include <algorithm>
#include <ctime>

const int TileBag::letterCount[27]{
	9, 2, 2, 4, 12, 2, 3, 2, 9, 1, 1, 4, 2, 6, 8, 2, 1, 6, 4, 6, 4, 2, 2, 1, 2, 1, 2
};

const int TileBag::scoreDistributions[27]{
	1, 3, 3, 2, 1, 4, 2, 4, 1, 8, 5, 1, 3, 1, 1, 3, 10, 1, 1, 1, 1, 4, 4, 8, 4, 10, 0
};

TileBag::TileBag()
{
	reset();
}

TileBag::~TileBag()
{
	for (Tile * tile : tiles)
		delete tile;
	tiles.clear();
}

void TileBag::reset()
{
	//	Initialize the 100 tiles in the bag
	for (int i = 0; i < 26; i++) {
		int count = letterCount[i];
		for (int j = 0; j < count; j++) {
			char letter = (char)(65 + i);
			int score = scoreDistributions[i];
			tiles.push_back(new Tile(letter, score));
		}
	}

	//	Two blank tiles
	int scoreForBlankTile = scoreDistributions[26];
	tiles.push_back(new Tile(' ', scoreForBlankTile));
	tiles.push_back(new Tile(' ', scoreForBlankTile));

	//	Shuffle
	std::srand((int)time(NULL));
	std::random_shuffle(tiles.begin(), tiles.end());
}

Tile * TileBag::drawTile()
{
	if (tiles.size() == 0)
		return nullptr;
	Tile * tile = tiles.front();
	tiles.erase(tiles.begin());
	return tile;
}
Tile * TileBag::exchangeTile(Tile * tile)
{
	//	Add back to the vector
	tiles.push_back(tile);

	//	Shuffle
	std::srand((int)time(NULL));
	std::random_shuffle(tiles.begin(), tiles.end());

	Tile * exchangedTile = tiles.front();
	tiles.erase(tiles.begin());
	return exchangedTile;
}

int TileBag::getRemainingTilesCount()
{
	return tiles.size();
}
