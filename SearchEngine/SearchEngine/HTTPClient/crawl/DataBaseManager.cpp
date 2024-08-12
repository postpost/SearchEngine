#include "DataBaseManager.h"
#include <iostream>
#include <pqxx/pqxx>


DataBaseManager::DataBaseManager(std::shared_ptr<IniParser> parser): _parser(parser){
	
}

bool DataBaseManager::ConnectToDB()
{
	try {
		GetConnectionString();
		pqxx::connection conn(_connectionStr);
		std::cout << "Connection to DB... SUCCESS" << std::endl;
		return true;
	}
	catch (pqxx::sql_error ex) {
		std::cout << "ERROR (CONNECTION TO DB): " << ex.what() << std::endl;
	}
	return false;
}

std::string DataBaseManager::GetConnectionString()
{
	if (_parser) {
		std::string host = "host=" + _parser->GetHost();
		std::string port = "port=" + _parser->GetPort();
		std::string dbname = "dbname=" + _parser->GetDBName();
		std::string user = "user=" + _parser->GetUser();
		std::string password = "password=" + _parser->GetPassword();
		_connectionStr = host + " " + port + " " + dbname + " " + user + " " + password;
		return _connectionStr;
	}

	//std::cerr << _connectionStr << std::endl;
}
