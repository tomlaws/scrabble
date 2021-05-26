#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <boost/array.hpp>
#include <boost/regex.hpp>
#include <fstream>
#include <regex>
#include <string>
#include <vector>
#define DICT_SIZE 79340	//	There are 79340 lines in the dictionary file

class Dictionary {
private:
	static bool loaded;
	std::ifstream file;
	char path[9] = "ospd.txt";
public:
	Dictionary();
	~Dictionary();
	bool checkWordValidity(const char word[]);
	std::vector<std::pair<int, std::pair<std::string, std::string>>> getPossibleWords(std::string regexStringV, std::string regexStringH);
	std::string getPossibleWordsB(std::string characters);

	static boost::array<std::string, DICT_SIZE> dictionary;
	static boost::array<std::string, DICT_SIZE> randomizedWordList;
};

#endif // !DICTIONARY_H