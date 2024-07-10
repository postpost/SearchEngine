#include "Crawler.h"
#include "Indexer.h"
#include "IniParser.h"
#include <string>




int main(int argc, char* argv[]) {

	std::string iniFileName = "config.ini";
	
	//Crawler
	Crawler crawler (iniFileName);
	crawler.DownloadWebPage();
	crawler.GetHTMLLinks();

	std::string fileToClean = crawler.GetHTMLContentFileName();
	crawler.CleanHTML(fileToClean);
	

	return 0;
}