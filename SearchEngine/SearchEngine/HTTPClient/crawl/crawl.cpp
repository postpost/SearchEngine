#include "Crawler.h"
#include "Indexer.h"
#include "IniParser.h"
#include <string>
#include <unordered_set>
#include <vector>

int main(int argc, char* argv[]) {

	std::string iniFileName = "config.ini";
	std::unordered_set<std::string> urls;

	std::vector <std::thread> threads;
	
	//Crawler
	Crawler crawler (iniFileName);
	urls = crawler.GetHTMLLinks();

	for (const auto& url : urls) {
		std::cout << url << std::endl;
	}

	int depth = crawler.GetRecursionDepth();
	int count = crawler.GetURLCount(urls);
	
	//std::vector<std::string> urlList(depth);
	

	if (count >= depth) {
		for (auto itr = urls.begin(); depth>0; ++itr) {
			//urlList.push_back(*itr);
			threads.push_back(std::thread(&Crawler::DownloadURLs, &crawler, std::cref(*itr)));
			--depth;
		}

	}
	for (auto& thread : threads) {
		thread.join();
	}
	
	std::string fileToClean = crawler.GetHTMLContentFileName();
	crawler.CleanHTML(fileToClean);
	
	return 0;
}