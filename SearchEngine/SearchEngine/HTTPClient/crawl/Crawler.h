#pragma once

#include "IniParser.h"
#include "Indexer.h"
#include <boost/system/api_config.hpp>
#include <boost/asio/ssl.hpp>
#include <vector>
#include <iostream>
#include <ctime> //to store HTML pages if they are more than 1

class Crawler  {
public:
	Crawler(std::string fileName);
	~Crawler();
	
public:
	//Fields
	std::vector<std::string> GetHTMLLinks();

	//Methods
	int DownloadWebPage();
	void CleanHTML(std::string& fileToClean);
	std::string GetHTMLContentFileName();
	
private:
	//Fields
	std::string _iniFilePath;
	std::string _host;
	std::string _port;
	std::string _target;
	int _version;
	int _recursionDepth;

	IniParser* _IniParser = nullptr;
	Indexer* _indexer = nullptr;

	std::string _file = "HTMLResponse.html";
	std::string _HTMLContentFile = "";
	std::vector<std::string> _urls;
	unsigned int _statusCode;
	std::regex _self_regex;

	//to handle HTMLContent
	time_t _timestamp;

private:
	//Methods
	void ShutdownHandler(const boost::system::error_code& err);
	void PrintCertificate(boost::asio::ssl::verify_context& ctx);
	void ReadStatusLineHandler(const boost::system::error_code& err);
	void ReadHeaderHandler(const boost::system::error_code& err, std::ifstream& response_stream);
	void SaveContent(const boost::system::error_code& err, std::ifstream& response_stream);
	std::string DefineFileName(std::string& timeStr);

};
