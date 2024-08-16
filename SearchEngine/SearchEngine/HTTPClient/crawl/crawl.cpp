#include "Crawler.h"
#include "Indexer.h"
#include "IniParser.h"
#include <string>
#include <unordered_set>
#include <vector>


#include <boost/locale.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/system/error_code.hpp>
#include <iostream>

#include <Windows.h>
#pragma execution_character_set("utf-8")

using namespace boost::locale;

int main(int argc, char* argv[]) {
	
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);

	std::string iniFileName = "config.ini";
	std::unordered_set<std::string> urls;

	std::vector <std::thread> threads;
	
	//Crawler
	Crawler crawler (iniFileName);
	urls = crawler.GetHTMLLinks();

	int depth = crawler.GetRecursionDepth();
	int count = crawler.GetURLCount(urls);

/*
---------------------------------*METHOD_01**START*----------------------------
	if (count >= depth) {
		for (auto itr = urls.begin(); depth>0; ++itr) {
			threads.push_back(std::thread(&Crawler::DownloadURLs, &crawler, std::cref(*itr)));
			--depth;
		}

	}
	for (auto& thread : threads) {
		thread.join();
	}
---------------------------------*METHOD_01**END*----------------------------

*/

//-------------------------------- - *METHOD_02 * *START * ----------------------------
//1. пусть кол-во потоков равно кол-ву глубины рекурсии?
//2. потоки будут осуществлять задачи внутри объекта класса Crawler или здесь в main?
//	 нужно ли тогда для каждого потока создавать свой io_context? 


//-------------------------------- - *METHOD_02 * *END * ----------------------------
 	
	
	std::string fileToClean = crawler.GetHTMLContentFileName();
	crawler.CleanHTML(fileToClean);
	crawler.SaveLowerCaseFile();
	crawler.CountWords();
	//crawler.PrintCountedWords();

	crawler.AddToDB();
	
	return 0;
}