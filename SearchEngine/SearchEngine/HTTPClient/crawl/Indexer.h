#pragma once
#include <iostream>
#include <regex>

class Indexer {
public:
	Indexer();
	void CleanText(std::string& txtToClean);
	void AddToDabaBase();

private:
	std::regex _sign_regex;
	std::string _cleanedHTML = "CleanedHTML.html"; //будет сохранено в отдельной папке
};