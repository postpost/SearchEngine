#pragma once
#include <iostream>
#include <regex>
#include <map>
#include "DataBaseManager.h"

class Indexer {
public:
	Indexer(std::shared_ptr<IniParser> parser);
	~Indexer();
	void CleanText(std::string& txtToClean);
	void ConvertToLowerCase();

	
	std::map<std::string, int> CountWords();
	void PrintCountedWords();

	//connect to DB
	void AddToDataBase();

private:
	std::regex _sign_regex;
	std::string _cleanedHTML = "CleanedHTML.html"; //будет сохранено в отдельной папке
	std::string _lowerCaseFile = " ";
	uint32_t _fileCount;

	std::map<std::string, int> _countedWords;

	std::string DefineFileName();
	void SaveToFile(std::string& text);

	void CleanMap(std::map <std::string, int>& container);

	std::shared_ptr<IniParser> _parser;

	//Database manager
	DataBaseManager *_DBManager;
};