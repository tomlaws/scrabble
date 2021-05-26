#include "Dictionary.h"
#include <iostream>

boost::array<std::string, DICT_SIZE> Dictionary::dictionary;	//	A dictionary contains all English words available in Scrabble game
boost::array<std::string, DICT_SIZE> Dictionary::randomizedWordList;	//	An already randomized available word list

//	Whether the dictionary is loaded into the above arrays
bool Dictionary::loaded = false;

Dictionary::Dictionary()
{
	if (!loaded) {
		loaded = true;
		file.open(path);
		std::string line;
		int lineCount = 0;
		while (std::getline(file, line))
		{
			dictionary[lineCount] = line;
			randomizedWordList[lineCount] = line;
			lineCount++;
		}
		file.close();
	}
}

Dictionary::~Dictionary()
{
	
}

//	Binary searching method
boost::array<std::string, DICT_SIZE>::iterator binary_find(boost::array<std::string, 79339>::iterator begin, boost::array<std::string, 79339>::iterator end, std::string val)
{
	boost::array<std::string, DICT_SIZE>::iterator i = std::lower_bound(begin, end, val);
	if (i != end && !(val < *i))
		return i; // Found
	else
		return end; // Not found
}

//	Check whether a word is in the dictionary
bool Dictionary::checkWordValidity(const char word[])
{
	std::string wordToFind = word;
	if (binary_find(dictionary.begin(), dictionary.end(), wordToFind) != dictionary.end())
		return true;
	return false;
}

//	Returns a vector of possible words with its direction and a pair of string (prefix and suffix)
std::vector<std::pair<int, std::pair<std::string, std::string>>> Dictionary::getPossibleWords(std::string regexStringV, std::string regexStringH)
{
	std::random_shuffle(randomizedWordList.begin(), randomizedWordList.end());
	std::vector<std::pair<int, std::pair<std::string, std::string>>> shortWords, longWords;
	boost::regex regexV(regexStringV, boost::regex_constants::ECMAScript | boost::regex_constants::icase | boost::regex::optimize);
	boost::regex regexH(regexStringH, boost::regex_constants::ECMAScript | boost::regex_constants::icase | boost::regex::optimize);

	for (int linec = 0; linec < 10000; linec++)
	{
		std::string currentWord = randomizedWordList[linec];

		boost::smatch matchesV;
		if (boost::regex_match(currentWord, matchesV, regexV)) {
			std::pair<std::string, std::string> pair(matchesV[1], matchesV[2]);
			std::pair<int, std::pair<std::string, std::string>> collections (0, pair);
			if (currentWord.length() < 4)
				shortWords.push_back(collections);
			else
				longWords.push_back(collections);
		}
		boost::smatch matchesH;
		if (boost::regex_match(currentWord, matchesH, regexH)) {
			std::pair<std::string, std::string> pair(matchesH[1], matchesH[2]);
			std::pair<int, std::pair<std::string, std::string>> collections(1, pair);
			if (currentWord.length() < 4)
				shortWords.push_back(collections);
			else
				longWords.push_back(collections);
		}
	}
	longWords.insert(longWords.end(), shortWords.begin(), shortWords.end());
	return longWords;
}

//	Just randomly find one word that based on the characters available
std::string Dictionary::getPossibleWordsB(std::string characters) {
	std::random_shuffle(randomizedWordList.begin(), randomizedWordList.end());
	std::transform(characters.begin(), characters.end(), characters.begin(), ::tolower);

	for (int linec = 0; linec < randomizedWordList.size(); linec++)
	{
		std::string currentWord = randomizedWordList[linec];
		for (int j = 0; j < characters.length(); j++) {
			int pos = currentWord.find_first_of(characters[j]);
			if (pos != std::string::npos)
				currentWord.erase(pos, 1);
		}

		if (currentWord.length() == 0) {
			return randomizedWordList[linec];
		}
	}
	return "";
}
