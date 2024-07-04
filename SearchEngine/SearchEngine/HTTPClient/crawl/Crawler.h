#pragma once

#include <vector>
#include <iostream>
#include "IniParser.h"

class Crawler  {
public:
	Crawler(std::string fileName);
	~Crawler();

	int DownloadWebPage();
	std::vector<std::string> GetHTMLLinks();
	
private:
	std::string _iniFilePath;
	std::string _host;
	std::string _port;
	std::string _target;
	int _version;
	int _recursionDepth;

	IniParser* _IniParser = nullptr;

};