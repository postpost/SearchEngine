#pragma once

#include "IniParser.h"
#include "Indexer.h"
#include <boost/system/api_config.hpp>
#include <boost/asio/ssl.hpp>
#include <unordered_set>
#include <iostream>
#include <ctime> //to store HTML pages if they are more than 1

struct URLComponents {
	std::string hostname;
	std::string protocol;
	std::string path;
	int version;
};

class Crawler  {
public:
	Crawler(std::string fileName);
	~Crawler();
	
	//Methods
	int GetRecursionDepth() { return _recursionDepth; }
	void DownloadURLs(const std::string& url);
	
	void CleanHTML(std::string& fileToClean);
	std::string GetHTMLContentFileName();

	void SaveLowerCaseFile();
	void CountWords();

	void PrintCountedWords();

	//connection to DB via Indexer
	void AddToDB();

public:
	//Fields
	std::unordered_set<std::string> GetHTMLLinks();
	int GetURLCount(std::unordered_set<std::string>& urls);

	
private:
	//Fields
	std::string _iniFilePath;
	std::string _urlString;
	int _recursionDepth;

	std::shared_ptr<IniParser> _IniParser = nullptr;
	Indexer* _indexer = nullptr;

	std::string _file = "HTMLResponse.html";
	std::string _HTMLContentFile = "";
	std::unordered_set<std::string> _urls;
	unsigned int _statusCode;
	std::regex _self_regex;

	//to handle HTMLContent
	time_t _timestamp;

	//parse url
	URLComponents _urlFields;
	int count = 0;

private:
	//Methods
	void ParseURL(const std::string& urlStr, URLComponents& urlFields);
	int DownloadWebPage(URLComponents& urlFields);
	void ShutdownHandler(const boost::system::error_code& err);
	void PrintCertificate(boost::asio::ssl::verify_context& ctx);
	void ReadStatusLineHandler(const boost::system::error_code& err);
	void ReadHeaderHandler(const boost::system::error_code& err, std::ifstream& response_stream);
	void SaveContent(const boost::system::error_code& err, std::ifstream& response_stream);
	std::string DefineFileName(std::string& timeStr);
	std::string GetDate();

};
