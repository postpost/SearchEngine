#pragma once


#include "..\HTTPClient\crawl\IniParser.h"
#include "..\HTTPClient\crawl\DataBaseManager.h"


class Server {
public:
	Server();
	~Server();

private:
	DataBaseManager* _dbManager;
	std::shared_ptr<IniParser> _parser;
	int _port;

public:
	void RequestToDB(std::string& word);
};