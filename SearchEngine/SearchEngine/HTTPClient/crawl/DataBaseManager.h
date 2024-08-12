#pragma once

#include "IniParser.h"

class DataBaseManager {
public:
	DataBaseManager(std::shared_ptr<IniParser> parser);
	bool ConnectToDB();
	std::string GetConnectionString();


private:
	std::shared_ptr<IniParser> _parser;
	bool _connectionStatus;
	std::string _connectionStr;
};