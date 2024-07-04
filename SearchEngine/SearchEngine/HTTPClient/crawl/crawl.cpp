#include "Crawler.h"
#include "Indexer.h"
#include "IniParser.h"
#include <string>

int main(int argc, char* argv[]) {

	std::string iniFileName = "config.ini";
	
	//Crawler
	Crawler crawler (iniFileName);
	crawler.DownloadWebPage();

	return 0;
}