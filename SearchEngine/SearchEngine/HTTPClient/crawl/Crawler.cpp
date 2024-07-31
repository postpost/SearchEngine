#include "Crawler.h"
#include "indexer.h"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

#include <chrono>
#include <ctime>
#include <iomanip>

#include <Windows.h>
#include <mutex>


#pragma execution_character_set("utf-8")

namespace beast = boost::beast; // from <boost/beast.hpp>
namespace http = beast::http;   // from <boost/beast/http.hpp>
namespace net = boost::asio;    // from <boost/asio.hpp>
namespace ssl = net::ssl;       // from <boost/asio/ssl.hpp>
using tcp = net::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

Crawler::Crawler(std::string fileName)
{
	_IniParser = new IniParser(fileName);
	_IniParser->ParseIniFile();
	
	_urlString =_IniParser->GetStartWebPage();
	DownloadURLs(_urlString);

	//Indexer
	_indexer = new Indexer;

	//_self_regex = std::regex("<a href=\"(.*?)\"", std::regex_constants::ECMAScript | std::regex_constants::icase);
	_self_regex = std::regex("\\b((?:https?|ftp|file):"
							 "\\/\\/[a-zA-Z0-9+&@#\\/%?=~_|!:,.;]*"
							 "[a-zA-Z0-9+&@#\\/%=~_|])", 
							 std::regex_constants::ECMAScript | std::regex_constants::icase);
}

Crawler::~Crawler()
{
	delete _IniParser;
	delete _indexer;
}

void Crawler::DownloadURLs(const std::string& url)
{
	URLComponents startWeb;
	ParseURL(url, startWeb);
	DownloadWebPage(startWeb);
}

int Crawler::DownloadWebPage(URLComponents& urlFields)
{
	try {
		net::io_context ioc;

		ssl::context ctx{ boost::asio::ssl::context::sslv23_client };
		ctx.set_default_verify_paths();

		// These objects perform our I/O
		tcp::resolver resolver{ ioc };
		ssl::stream<tcp::socket> stream{ ioc, ctx };
		//beast::ssl_stream<beast::tcp_stream> stream{ ioc, ctx };

		//Look up domain name
		auto const results = resolver.resolve(urlFields.hostname, urlFields.protocol);
		
		//Connect on the IP from looking up
		//beast::get_lowest_layer(stream).connect(results);

		//SSL_set_tlsext_host_name(stream.native_handle(), _urlFields.hostname.c_str());

		// Set SNI Hostname (many hosts need this to handshake successfully)
		if (!SSL_set_tlsext_host_name(stream.native_handle(), urlFields.hostname.c_str()))
		{
			boost::system::error_code ec{ static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category() };
			throw boost::system::system_error{ ec };
		}

		boost::asio::connect(stream.next_layer(), results.begin(), results.end());

		//SSL handshake
		stream.handshake(ssl::stream_base::client);

		//Set up GET request
		http::request<http::string_body> req{ http::verb::get, urlFields.path, urlFields.version };
		req.set(http::field::host, urlFields.hostname);
		req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

		//Send HTTP req
		http::write(stream, req);

		//boost vars
		//Container to hold the response
		http::response <http::dynamic_body> _response;

		//Buffer for  reading
		beast::flat_buffer _buffer;

		//Read the response
		http::read(stream, _buffer, _response);

		//Write the message to .html
		
		std::ofstream fout(_file);

		fout << _response;
		fout.close();

		//Close the stream
		beast::error_code ec;

		// Simply closing the lowest layer may make the session vulnerable to a truncation attack.
		//stream.shutdown() leads to the truncate error
		beast::get_lowest_layer(stream).cancel();
		beast::get_lowest_layer(stream).close();

		ReadStatusLineHandler(ec);
		if (ec == net::error::eof) {
			// Rationale:
			// http://stackoverflow.com/questions/25587403/boost-asio-ssl-async-shutdown-always-finishes-with-an-error
			ec = {};
		}
		if (ec) {
			throw beast::system_error{ ec };
		}
	}
	catch (std::exception const& ex) {
		std::cerr << "Error: " << ex.what() << std::endl;
		return EXIT_FAILURE;
	}

	std::cerr << "\n--------------SUCCESS-------------------------\n";
	return EXIT_SUCCESS;

}

std::unordered_set<std::string> Crawler::GetHTMLLinks()
{
	std::ifstream fin(_HTMLContentFile);
	std::string inputString;
	std::getline(fin, inputString);
	
	//находит все совпадения
	std::sregex_iterator itr_regex(inputString.begin(), inputString.end(), _self_regex); //здесь запишутся вообще все совпадения
	std::sregex_iterator itr_regex_end;

	while (itr_regex != itr_regex_end) {
		_urls.insert(itr_regex->str());
		itr_regex++;
	}

	if (_urls.size() == 0) {
		std:: cout << "-1" << std::endl;
	}

	return _urls;
}

int Crawler::GetURLCount(std::unordered_set<std::string>& urls)
{
	int count = 0;
	for (const auto& url : urls) {
		++count;
	}
	return count;
}

void Crawler::ParseURL(const std::string& urlStr, URLComponents& urlFields)
{
	int pos = urlStr.find(':');
	urlFields.protocol = urlStr.substr(0, pos);
	
	pos = urlStr.find('/');
	std::string res = urlStr.substr(pos + 2);
	urlFields.hostname = res.substr(0, res.find('/'));
	urlFields.path = res.substr(res.find('/'), '/r/n');

	urlFields.version = 11; //int version = argc == 5 && !std::strcmp("1.0", argv[4]) ? 10 : 11; (в функции main?)
	_recursionDepth = _IniParser->GetRecursionDepth();
}

void Crawler::PrintCertificate(ssl::verify_context& ctx)
{
	char subject_name[256];
	X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
	X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
	std::cout << "Verifying " << subject_name << "\n";
}

void Crawler::ReadStatusLineHandler(const boost::system::error_code& err)
{
	std::cout << "READ STATUS LINE\n";
	std::ifstream response_stream(_file);
		std::string http_version;
		response_stream >> http_version;
		response_stream >> _statusCode;
		std::string statusMsg;
		std::getline(response_stream, statusMsg);

		if (!response_stream || http_version.substr(0, 5) != "HTTP/") {
			std::cerr << "Invalid response\n";
			return;
		}

		if (_statusCode != 200) {
			std::cerr << "Response returned with status_code = " << _statusCode << std::endl;
			return;
		}
		std::cerr << "Status_code: " << _statusCode << std::endl;
	
	if (err)
	{
		std::cout << "Error: " << err.message() << "\n";
	}

	//method to read headers
	ReadHeaderHandler(err, response_stream);
	SaveContent(err, response_stream);
}

void Crawler::ReadHeaderHandler(const boost::system::error_code& err, std::ifstream& response_stream)
{
	std::cerr << "Read Headers...\n";
	if (_statusCode == 200) {
		std::string header;
		std::cerr << "-------------HEADER-------------\n";
		while (std::getline(response_stream, header) && header != "\r") {
			std::cerr << header << std::endl;
		}
	}
	if (err)
	{
		std::cout << "Error: " << err.message() << "\n";
	}
}

void Crawler::SaveContent(const boost::system::error_code& err, std::ifstream& response_stream)
{
	std::cerr << "Save html...\n";

	std::string time = GetDate();
	
	DefineFileName(time);
	 
	std::ofstream fout(_HTMLContentFile);
	std::string content;
	std::string htmlCursor = "<";

	std::getline(response_stream, content); //убирает первую строку (появляется в www.google.com)
	
	if (content.find_first_not_of(htmlCursor)) {
		content.erase(0, '\n');
	}

	while (std::getline(response_stream, content)) {
		fout << content;
	}
	
	response_stream.close();
	fout.close();
}

std::string Crawler::DefineFileName(std::string& timeStr)
{
	++count;
	_HTMLContentFile = ".\\HTMLDB\\HTMLContent_" + std::to_string(count) + "_" + timeStr + ".html";
	return _HTMLContentFile;

}

std::string Crawler::GetDate()
{
	auto now = std::chrono::system_clock::now();
	auto time_t = std::chrono::system_clock::to_time_t(now);
	std::stringstream timeStr;
	timeStr << std::put_time(std::localtime(&time_t), "%Y-%m-%d-%H%m");
	return timeStr.str();
}

void Crawler::CleanHTML(std::string& fileToClean)
{
	std::string stream;
	std::string result;
	std::ifstream fin(fileToClean);
	if (fin.is_open() && _indexer != nullptr) {
		while (std::getline(fin, result)){
			result += stream;
		}
		fin.close();
	}
	_indexer->CleanText(result);
}

std::string Crawler::GetHTMLContentFileName()
{
	return _HTMLContentFile;
}
