#include "Indexer.h"
#include <fstream>

Indexer::Indexer()
{
	_sign_regex = std::regex("<.*?>|\W"); //  ; <[^>]*> |\W/gx; <.*?>|\W
}

void Indexer::CleanText(std::string& txtToClean)
{
	std::ofstream fout(_cleanedHTML);
	std::string newStr = std::regex_replace(txtToClean, _sign_regex, " ");
	fout << newStr;
	fout.close();

	std::cout << "Result string: \"" << newStr << "\"\n";
}



