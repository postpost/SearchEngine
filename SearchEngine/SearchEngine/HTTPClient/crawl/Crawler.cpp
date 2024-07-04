#include "Crawler.h"
#include <boost/asio.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <fstream>

#include <iterator>
#include <regex>
#include <string>

#include "indexer.h"
#include <unordered_map>
#include <map>
#include <functional>

#include <iostream>
#include "pqxx/pqxx"
#include <Windows.h>
#pragma execution_character_set("utf-8")

using tcp = boost::asio::ip::tcp; //использует все имена в объекте tcp
namespace http = boost::beast::http; // from <boost/beast/http.hpp>
//пространство имен - область действия, которая содержит нужные объекты


Crawler::Crawler(std::string fileName)
{
	_IniParser = new IniParser(fileName);
	_IniParser->ParseIniFile();
	//_IniParser->PrintConfigData();

	_host = _IniParser->GetStartWebPage();
	_port = "80"; // 80 - http
	_target = "/";
	_version = 11; //int version = argc == 5 && !std::strcmp("1.0", argv[4]) ? 10 : 11; (в функции main?)
	_recursionDepth = _IniParser->GetRecursionDepth();
}

Crawler::~Crawler()
{
	delete _IniParser;
}

int Crawler::DownloadWebPage()
{
	try {
		boost::asio::io_context ioc;
		tcp::socket socket{ ioc };
		tcp::resolver resolver{ ioc };

		auto const results = resolver.resolve(_host, _port); //ОШИБКА - хост не найден; www.google.com

		boost::asio::connect(socket, results.begin(), results.end());

		//prepare HTTP request
		http::request<http::string_body> request{ http::verb::get, _target, _version };
		request.set(http::field::host, _host);
		request.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

		//SEND REQUEST TO THE REMOTE HOST
		http::write(socket, request);

		//READ REQUEST
		boost::beast::flat_buffer buffer; //to read response
		http::response<http::dynamic_body>response; //container to store response
		http::read(socket, buffer, response);

		//READ IN OUT
		std::ofstream fout("HTTPResponse.html");
		fout << response;
		fout.close();

		//CLOSE SOCKET
		boost::system::error_code error;
		socket.shutdown(tcp::socket::shutdown_both, error);

		if (error && error != boost::system::errc::not_connected)
			throw boost::system::system_error{ error };
	}
	catch (std::exception& ex) {
		std::cerr << "Error: " << ex.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
