#include "DataBaseManager.h"
#include <iostream>

DataBaseManager::DataBaseManager(std::shared_ptr<IniParser> parser): _parser(parser)
{
	_wordsTableQuery = "CREATE TABLE if not exists public.words "
		"(id SERIAL primary key, "
		"word text NOT NULL)";

	_docsTableQuery = "CREATE TABLE if not exists public.docs "
		"(id SERIAL primary key, "
		"doc_path text NOT NULL)";

	_wordFrequency = "CREATE TABLE if not exists word_frequency "
		"(word_id INTEGER references public.words(id), "
		"doc_id INTEGER references public.docs(id), "
		"word_frequency int NOT NULL, "
		"constraint pk primary key (word_id, doc_id))";
		
	_insertQuery = "";
}

void DataBaseManager::AddToDB(std::map<std::string, int>& words, const std::string& filePath)
{
	try 
	{
		GetConnectionString();
		pqxx::connection conn(_connectionStr);
		std::cout << "Connection to DB... SUCCESS" << std::endl;
		_connectionStatus = true;
		
		CreateTable(conn, _wordsTableQuery);
		CreateTable(conn, _docsTableQuery);
		CreateTable(conn, _wordFrequency);
		
		for (const auto& rows : words) {
			InsertRow(conn, TableType::words, rows.first);
			InsertRow(conn, TableType::docs, filePath);
			InsertRow(conn, TableType::frequency, std::to_string(rows.second));
		}
	}

	catch (pqxx::sql_error ex) {
		std::cout << "ERROR (CONNECTION TO DB): " << ex.what() << std::endl;
		_connectionStatus = false;
	}
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

bool DataBaseManager::InsertRow(pqxx::connection& connection, TableType table, const std::string& field)
{
	try {

		pqxx::transaction tx(connection);
		switch (table) {
			case TableType::words:
			{
				_wordId.clear();
				_insertQuery = "INSERT INTO public.words(word) VALUES ('" + tx.esc(field) + "') returning id";
				_wordId = tx.exec(_insertQuery);
			} break;
			case TableType::docs:
			{
				if (_docPath != field) {
					_docId.clear();
					_insertQuery = "INSERT INTO public.docs(doc_path) VALUES ('" + tx.esc(field) + "') returning id";
					_docId = tx.exec(_insertQuery);
					_docPath = field;
				}
			} break;
			case TableType::frequency:
			{
				std::tuple<std::string> word_id = _wordId[0].as<std::string>();
				std::tuple<std::string> doc_id = _docId[0].as<std::string>();

				//auto ids = std::make_tuple(_wordId[0].as<std::string>(), _docId[0].as<std::string>());
				_insertQuery = "INSERT INTO word_frequency(word_id, doc_id, word_frequency) VALUES ('" + tx.esc(std::get<0>(word_id)) + "','" + tx.esc(std::get<0>(doc_id)) + "','" + tx.esc(field) + "')";
				tx.exec(_insertQuery);
			} break;
			default:
				break;
		}
		tx.commit();
		return true;
	}
	catch (std::exception& ex) {
		std::cout << "ERROR (INSERT INTO DB): " << ex.what() << std::endl;
		return false;
	}
}

bool DataBaseManager::CreateTable(pqxx::connection& connection, std::string& query)
{
	if (_connectionStatus) {
		pqxx::transaction trx(connection);
		trx.exec(query);
		trx.commit();
		return true;
	}
	return false;
}
