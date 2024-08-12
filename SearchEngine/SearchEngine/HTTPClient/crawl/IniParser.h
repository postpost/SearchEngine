#pragma once
#include <iostream>
#include <fstream>

struct DatabaseConnection {
	//data to connect to PostgreSQL
	std::string _host;
	std::string _port;
	std::string _dbname;
	std::string _user;
	std::string _password;
};

struct CrawlerData {
	std::string _startWeb;
	std::string _port;
	std::string _url;
	std::string _target;
	int	_recursionDepth;
};

class IniParser {
public:
	IniParser(std::string iniFileName);
	~IniParser();
	
	void ParseIniFile();
	void PrintConfigData();

	std::string GetHost();
	std::string GetPort();
	std::string GetDBName();
	std::string GetUser();
	std::string GetPassword();
	std::string GetStartWebPage();
	int GetRecursionDepth();

private:
	//Fields
	std::string _iniFileName;
	std::ifstream _fileIn;
	DatabaseConnection dataToConnect;
	CrawlerData dataToCrawl;

private:
	//Methods
	void SaveDataFromFile(std::string& dataString);
	void GetDataFromIni();
};