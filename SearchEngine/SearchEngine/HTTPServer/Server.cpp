#include "Server.h"


Server::Server() {
	
	_parser = std::make_shared<IniParser>();
	_dbManager = new DataBaseManager(_parser);

	if (_parser) {
		_port = _parser->GetServerPort();
	}
}

Server::~Server()
{
	delete _dbManager;
}

void Server::RequestToDB(std::string& word)
{
	if (_dbManager) {
		_dbManager->RequestToDB(word);
	}
}
