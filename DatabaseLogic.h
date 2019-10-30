
#ifndef DatabaseLogic_h
#define DatabaseLogic_h

#include <iostream>
#include <map>
#include <fstream>
#include <stdio.h>
#include <sqlite3.h>
#include <algorithm>
#include <vector>

#include "json/json.h"
#include "curl/curl.h"
#include "Fundamental_class.h"
#include "UtilityFunctions.h"

using namespace std;

enum DailyReturn {id, symbol, date, open, high, low, close, adjusted_close, volume, Return};
enum FundData {Id, Symbol, pe_ratio, dividend_Yield, Beta, high_52, low_52, MA_50, MA_200, MarketCAP};



int OpenDatabase(const char * name, sqlite3 * & db)
{
	int rc = 0;
	cout << "Opening database: " << name << endl;
	rc = sqlite3_open("Stocks.db", &db);
	if (rc)
	{
		cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(db) << endl;
		sqlite3_close(db);
		system("pause");
		return -1;
	}
	cout << "Opened database: " << name << endl;
	return 0;

}

void CloseDatabase(sqlite3 *db)
{
	cout << "Closing a database ..." << endl;
	sqlite3_close(db);
	cout << "Closed a database" << endl << endl;
}

int DropTable(const char * sql_drop_table, sqlite3 *db)
{
	// Drop the table if exists
	if (sqlite3_exec(db, sql_drop_table, 0, 0, 0) != SQLITE_OK) { // or == -- same effect
		std::cout << "SQLite can't drop sessions table" << std::endl;
		sqlite3_close(db);
		system("pause");
		return -1;
	}
	return 0;
}

int CreateTable(const char *sql_create_table, sqlite3 *db)
{
	int rc = 0;
	char *error = NULL;
	// Create the table
	cout << "Creating a table..." << endl;
	rc = sqlite3_exec(db, sql_create_table, NULL, NULL, &error);
	if (rc)
	{
		cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_free(error);
		system("pause");
		return -1;
	}
	cout << "Created a table." << endl;
	return 0;
}

int InsertTable(const char *sql_insert, sqlite3 *db)
{
	int rc = 0;
	char *error = NULL;
	// Execute SQL
	cout << "Inserting a value into a table ..." << endl;
	rc = sqlite3_exec(db, sql_insert, NULL, NULL, &error);
	if (rc)
	{
		cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_free(error);
		system("pause");
		return -1;
	}
	cout << "Inserted a value into the table." << endl;
	return 0;
}

int DisplayTable(const char *sql_select, sqlite3 *db)
{
	int rc = 0;
	char *error = NULL;

	// Display MyTable
	cout << "Retrieving values in a table ..." << endl;
	char **results = NULL;
	int rows, columns;
	// A result table is memory data structure created by the sqlite3_get_table() interface.
	// A result table records the complete query results from one or more queries.
	sqlite3_get_table(db, sql_select, &results, &rows, &columns, &error);
	if (rc)
	{
		cerr << "Error executing SQLite3 query: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_free(error);
		system("pause");
		return -1;
	}

	// Display Table
	for (int rowCtr = 0; rowCtr <= rows; ++rowCtr)
	{
		for (int colCtr = 0; colCtr < columns; ++colCtr)
		{
			// Determine Cell Position
			int cellPosition = (rowCtr * columns) + colCtr;

			// Display Cell Value
			cout.width(12);
			cout.setf(ios::left);
			cout << results[cellPosition] << " ";
		}

		// End Line
		cout << endl;

		// Display Separator For Header
		if (0 == rowCtr)
		{
			for (int colCtr = 0; colCtr < columns; ++colCtr)
			{
				cout.width(12);
				cout.setf(ios::left);
				cout << "~~~~~~~~~~~~ ";
			}
			cout << endl;
		}
	}
	// This function properly releases the value array returned by sqlite3_get_table()
	sqlite3_free_table(results);
	return 0;
}

//writing call back function for storing fetched values in memory
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

int RetrieveMarketData(string url_request, Json::Value & root)
{
	std::string readBuffer;

	//global initiliation of curl before calling a function
	curl_global_init(CURL_GLOBAL_ALL);

	//creating session handle
	CURL * myHandle;

	// We¡¯ll store the result of CURL¡¯s webpage retrieval, for simple error checking.
	CURLcode result;

	// notice the lack of major error-checking, for brevity
	myHandle = curl_easy_init();

	curl_easy_setopt(myHandle, CURLOPT_URL, url_request.c_str());
	//curl_easy_setopt(myHandle, CURLOPT_URL, "https://eodhistoricaldata.com/api/eod/AAPL.US?from=2018-01-05&to=2019-02-10&api_token=5ba84ea974ab42.45160048&period=d&fmt=json");

	//adding a user agent
	curl_easy_setopt(myHandle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows; U; Windows NT 6.1; rv:2.2) Gecko/20110201");
	curl_easy_setopt(myHandle, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(myHandle, CURLOPT_SSL_VERIFYHOST, 0);
	curl_easy_setopt(myHandle, CURLOPT_VERBOSE, 1);

	// send all data to this function  
	curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, WriteCallback);

	// we pass our 'chunk' struct to the callback function */
	curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, &readBuffer);

	//perform a blocking file transfer
	result = curl_easy_perform(myHandle);

	// check for errors 
	if (result != CURLE_OK) {
		fprintf(stderr, "curl_easy_perform() failed: %s\n",
			curl_easy_strerror(result));
	}
	else {
		cout << readBuffer << endl;
		//
		// Now, our chunk.memory points to a memory block that is chunk.size
		// bytes big and contains the remote file.
		//
		// Do something nice with it!
		//

		// https://github.com/open-source-parsers/jsoncpp
		// Using JsonCpp in your project
		// Amalgamated source
		// https ://github.com/open-source-parsers/jsoncpp/wiki/Amalgamated

		//json parsing
		Json::CharReaderBuilder builder;
		Json::CharReader * reader = builder.newCharReader();
		string errors;

		bool parsingSuccessful = reader->parse(readBuffer.c_str(), readBuffer.c_str() + readBuffer.size(), &root, &errors);
		if (not parsingSuccessful)
		{
			// Report failures and their locations
			// in the document.
			cout << "Failed to parse JSON" << std::endl
				<< readBuffer
				<< errors << endl;
			system("pause");
			return -1;
		}
		std::cout << "\nSucess parsing json\n" << root << endl;

	}

	//End a libcurl easy handle.This function must be the last function to call for an easy session
	curl_easy_cleanup(myHandle);
	return 0;
}

int PopulateStockTable(const Json::Value & root, string symbol, sqlite3 *db)
{
	string date;
	float open, high, low, close, adjusted_close, Return;
	float volume = 0;
	Stock myStock(symbol);
	int count = 0;
	for (Json::Value::const_iterator itr = root.begin(); itr != root.end(); itr++)
	{
		for (Json::Value::const_iterator inner = (*itr).begin(); inner != (*itr).end(); inner++)
		{
			if (inner.key().asString() == "adjusted_close")
			{
				if ((inner->asString()).empty())
					adjusted_close = 0;
				else
					adjusted_close = (float)(inner->asDouble());
			}
			else if (inner.key().asString() == "close")
			{
				if ((inner->asString()).empty())
					close = 0;
				else
					close = (float)(inner->asDouble());
			}
			else if (inner.key() == "date")
				date = inner->asString();
			else if (inner.key().asString() == "high")
			{
				if ((inner->asString()).empty())
					high = 0;
				else
					high = (float)(inner->asDouble());
			}
			else if (inner.key().asString() == "low")
			{
				if ((inner->asString()).empty())
					low = 0;
				else
					low = (float)(inner->asDouble());
			}
			else if (inner.key() == "open")
			{
				if ((inner->asString()).empty())
					open = 0;
				else
					open = (float)(inner->asDouble());
			}
			else if (inner.key().asString() == "volume")
			{
				if ((inner->asString()).empty())
					volume = 0;
				else
					volume = (float)(inner->asDouble());
			}
			else
			{
				cout << "Invalid json field" << endl;
				system("pause");
				return -1;
			}
		}
		if (symbol == "RiskFree")
		{
			if (date == "2010-10-11")
			{
				open = 2.381;
				high = 2.381;
				low = 2.381;
				close = 2.381;
				adjusted_close = 2.381;
			}
		}
		if (symbol == "RiskFree")
		{
			if (date == "2016-11-11")
			{
				open = 2.117;
				high = 2.117;
				low = 2.117;
				close = 2.117;
				adjusted_close = 2.117;
			}
		}

		Trade aTrade(date, open, high, low, close, adjusted_close, volume);
		myStock.addTrade(aTrade);
		Return = myStock.CalReturn(count);
		count++;

		// Execute SQL
		char stockDB_insert_table[512];
		sprintf_s(stockDB_insert_table, "INSERT INTO %s (id, symbol, date, open, high, low, close, adjusted_close, volume, return) VALUES(%d, \"%s\", \"%s\", %f, %f, %f, %f, %f, %f, %f)", symbol.c_str(), count, symbol.c_str(), date.c_str(), open, high, low, close, adjusted_close, volume, Return);
		if (InsertTable(stockDB_insert_table, db) == -1)
			return -1;

		// Solve the MSI missing data problem
		if (symbol == "MSI")
		{
			if (date == "2017-11-20")
			{
				advance(itr, 27); continue;
			}
		}
	}
	cout << myStock;
	return 0;
}

int PopulateSP500List(const Json::Value & root, vector<string>& StockList)
{
	string symbol;
	for (Json::Value::const_iterator itr = root.begin(); itr != root.end(); itr++)
	{
		for (Json::Value::const_iterator inner = (*itr).begin(); inner != (*itr).end(); inner++)
		{
			if (inner.key() == "Symbol")
			{
				symbol = inner->asString();
				if (find(DeletionList.begin(), DeletionList.end(), symbol) != DeletionList.end())
				{
					vector<string>::iterator it = find(DeletionList.begin(), DeletionList.end(), symbol);
					int index = distance(DeletionList.begin(), it);
					symbol = AdditionList[index];
				}
				StockList.push_back(symbol);
			}
		}
	}
	return 0;
}

int PopulateSP500TableAndFundamental(const Json::Value & root, sqlite3 *db, int count)
{
	string symbol, sector, name;
	float PERatio=0;
	float DivYield=0;
	float Beta=0;
	float High52=0;
	float Low52=0;
	float MA50=0;
	float MA200=0;
	float MarketCap=0;
	float TargetPrice = 0;
	float PEGRatio = 0;

	for (Json::Value::const_iterator itr = root.begin(); itr != root.end(); itr++)
	{
		for (Json::Value::const_iterator inner = (*itr).begin(); inner != (*itr).end(); inner++)
		{
			if (inner.key().asString() == "Code")
				symbol = inner->asString();
			else if (inner.key().asString() == "Name")
				name = inner->asString();
			else if (inner.key().asString() == "Sector")
				sector = inner->asString();
			else if (inner.key().asString() == "PERatio")
			{
				if ((inner->asString()).empty())
					PERatio = 0;
				else
					PERatio = (float)atof(inner->asCString());
			}
			else if (inner.key().asString() == "DividendYield")
			{
				if ((inner->asString()).empty())
					DivYield = 0;
				else
					DivYield = (float)atof(inner->asCString());
			}
			else if (inner.key().asString() == "Beta")
			{
				if ((inner->asString()).empty())
					Beta = 0;
				else
					Beta = (float)atof(inner->asCString());
			}
			else if (inner.key().asString() == "52WeekHigh")
			{
				if ((inner->asString()).empty())
					High52 = 0;
				else
					High52 = (float)atof(inner->asCString());
			}
			else if (inner.key().asString() == "52WeekLow")
			{
				if ((inner->asString()).empty())
					Low52 = 0;
				else
					Low52 = (float)atof(inner->asCString());
			}
			else if (inner.key().asString() == "50DayMA")
			{
				if ((inner->asString()).empty())
					MA50 = 0;
				else
					MA50 = (float)atof(inner->asCString());
			}
			else if (inner.key().asString() == "200DayMA")
			{
				if ((inner->asString()).empty())
					MA200 = 0;
				else
					MA200 = (float)atof(inner->asCString());
			}
			else if (inner.key().asString() == "MarketCapitalizationMln")
			{
				if ((inner->asString()).empty())
					MarketCap = 0;
				else
					MarketCap = (float)atof(inner->asCString());
			}
			else if (inner.key().asString() == "WallStreetTargetPrice")
			{
				if ((inner->asString()).empty())
					TargetPrice = 0;
				else
					TargetPrice = (float)atof(inner->asCString());
			}
			else if (inner.key().asString() == "PEGRatio")
			{
				if ((inner->asString()).empty())
					PEGRatio = 0;
				else
					PEGRatio = (float)atof(inner->asCString());
			}
		}
	}

	//Excute SQL for S&P500
	char sp500_insert_table[512];
	sprintf_s(sp500_insert_table, "INSERT INTO SP500 (id, symbol, name, sector) VALUES(%d, \"%s\", \"%s\", \"%s\")", count, symbol.c_str(), name.c_str(), sector.c_str());
	if (InsertTable(sp500_insert_table, db) == -1)
		return -1;

	//Excute SQL for Fundamental
	char fundamental_insert_table[512];
	sprintf_s(fundamental_insert_table, "INSERT INTO FUNDAMENTAL (id, symbol, pe_ratio, dividend_Yield, beta, high_52, low_52, MA_50, MA_200, MarketCap, Target_Price, peg_ratio) VALUES(%d, \"%s\", %f, %f, %f, %f, %f, %f, %f, %f, %f, %f)", count, symbol.c_str(), PERatio, DivYield, Beta, High52, Low52, MA50, MA200, MarketCap, TargetPrice, PEGRatio);
	if (InsertTable(fundamental_insert_table, db) == -1)
		return -1;

	return 0;
}

int GetBenchDate(sqlite3 *db, vector<string> &benchDate)
{
	//-------------------For the use of benchmark---------------------------
	int rc = 0;
	char *error = NULL;
	string sql_select_benchmark = "SELECT * FROM AAPL;";
	char **results = NULL;
	int rows, columns;
	sqlite3_get_table(db, sql_select_benchmark.c_str(), &results, &rows, &columns, &error);
	if (rc)
	{
		cerr << "Error executing SQLite3 query: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_free(error);
		system("pause");
		return -1;
	}

	for (int rowCtr = 1; rowCtr <= rows; ++rowCtr)
	{
		int cellPosition = (rowCtr * columns) + 2;
		string date(results[cellPosition]);
		benchDate.push_back(date);
	}

	sqlite3_free_table(results);
	return 0;
}

//-------------------------Retrieve Fundamental using column name-----------------------------------
void GetFromFundamental(string ticker, sqlite3 *db, vector<Fundamental> &FundObjList)
{
	if (ticker == "BRK.B")
		ticker = "BRK-B";
	if (ticker == "BF.B")
		ticker = "BF-B";

	cout << "Retrieving Fundamental from DataBase for :" << ticker << endl;
	int rc = 0;
	char *error = NULL;
	string sql_select = "SELECT * FROM FUNDAMENTAL;";

	cout << "Retrieving values in a table ..." << endl;
	char **results = NULL;
	int rows, columns;
	sqlite3_get_table(db, sql_select.c_str(), &results, &rows, &columns, &error);
	if (rc)
	{
		cerr << "Error executing SQLite3 query: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_free(error);
		system("pause");
	}

	for (int rowCtr = 1; rowCtr <= rows; ++rowCtr)
	{
		// Determine Cell Position
		int symbolPosition = (rowCtr * columns) + 1;
		int PEPosition = (rowCtr * columns) + 2;
		int DivPosition = (rowCtr * columns) + 3;
		int BetaPosition = (rowCtr * columns) + 4;
		int H52Position = (rowCtr * columns) + 5;
		int L52Position = (rowCtr * columns) + 6;
		int MA50Position = (rowCtr * columns) + 7;
		int MA200Position = (rowCtr * columns) + 8;
		int CapPosition = (rowCtr * columns) + 9;
		int TargetPosition = (rowCtr * columns) + 10;
		int PEGPosition = (rowCtr * columns) + 11;
		string symbol(results[symbolPosition]);
		
		if (symbol == ticker)
		{
			Fundamental aFund(symbol);
			aFund.PopulateFund((float)atof(results[PEPosition]), (float)atof(results[DivPosition]), (float)atof(results[BetaPosition]), (float)atof(results[H52Position]), (float)atof(results[L52Position]), (float)atof(results[MA50Position]), (float)atof(results[MA200Position]), (float)atof(results[CapPosition]), (float)atof(results[TargetPosition]), (float)atof(results[PEGPosition]));
			FundObjList.push_back(aFund);
		}
	}
	sqlite3_free_table(results);
	cout << "Retrieving Fundamental Completed" << endl;
}

void PopulateSector(sqlite3 *db, vector<Fundamental> &FundObjList)
{
	int rc = 0;
	char *error = NULL;
	string sql_select = "SELECT * FROM SP500;";

	cout << "Retrieving values in a table ..." << endl;
	char **results = NULL;
	int rows, columns;
	sqlite3_get_table(db, sql_select.c_str(), &results, &rows, &columns, &error);
	if (rc)
	{
		cerr << "Error executing SQLite3 query: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_free(error);
		system("pause");
	}

	for (int rowCtr = 1; rowCtr <= rows; ++rowCtr)
	{
		// Determine Cell Position
		int symbolPosition = (rowCtr * columns) + 1;
		int sectorPosition = (rowCtr * columns) + 3;
		string symbol(results[symbolPosition]);
		string sector(results[sectorPosition]);

		for (int i = 0; i < FundObjList.size(); i++)
		{
			if (FundObjList[i].GetSymbol() == symbol)
			{
				FundObjList[i].SetSector(sector);
				break;
			}
		}

	}
	sqlite3_free_table(results);
}

//-------------------------Retrieve data using column name-----------------------------------

int GetFromDB(string ticker, sqlite3 *db, vector<Stock> &StockObjList, vector<string> benchDate, string LastDate)
{
	cout << "Retrieving Return from DataBase for :" << ticker << endl;
	if (ticker == "BRK-B")
		ticker = "BRKB";
	if (ticker == "BF-B")
		ticker = "BFB";
	if (ticker == "ALL")
		ticker = "Stock_ALL";

	int rc = 0;
	char *error = NULL;
	string sql_select = "SELECT * FROM " + ticker + ";";

	cout << "Retrieving values in a table ..." << endl;
	char **results = NULL;
	int rows, columns;
	sqlite3_get_table(db, sql_select.c_str(), &results, &rows, &columns, &error);

	if (rc)
	{
		cerr << "Error executing SQLite3 query: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_free(error);
		system("pause");
		return -1;
	}

	if (ticker == "BRKB")
		ticker = "BRK-B";
	if (ticker == "BFB")
		ticker = "BF-B";
	if (ticker == "Stock_ALL")
		ticker = "ALL";

	Stock myStock(ticker);

	for (int rowCtr = 1; rowCtr <= rows; ++rowCtr)
	{
		// Determine Cell Position
		int cellPosition = (rowCtr * columns) + 9;
		int datePosition = (rowCtr * columns) + 2;
		string stockDate(results[datePosition]);
		myStock.addReturn(stockDate, (float)atof(results[cellPosition]));

		if (stockDate == LastDate)
		{
			int PricePosition = (rowCtr * columns) + 6;
			float LastPrice = (float)atof(results[PricePosition]);
			myStock.SetPrice(LastPrice);
		}
	}

	if (rows != 2915)
		myStock.EuqalizeLength(benchDate);
	StockObjList.push_back(myStock);

	sqlite3_free_table(results);
	cout << "Retrieving Return Completed" << endl;
	return 0;
}


void UpdateList(vector<string> &StockList, sqlite3 *db)
{
	replace(StockList.begin(), StockList.end(), "BRK.B", "BRK-B");
	replace(StockList.begin(), StockList.end(), "BF.B", "BF-B");
	for (int i = 0; i < DeletionList.size(); i++)
	{
		replace(StockList.begin(), StockList.end(), DeletionList[i], AdditionList[i]);
	}
}

void ReplaceList(vector<string> &StockList, sqlite3 *db)
{
	int count = 1;
	vector<string> temp;
	for (int i = 0; i < StockList.size(); i++)
	{
		string ticker = StockList[i];
		//cout << "Retrieving Return from DataBase for :" << ticker << endl;
		if (ticker == "BRK-B")
			ticker = "BRKB";
		if (ticker == "BF-B")
			ticker = "BFB";
		if (ticker == "ALL")
			ticker = "Stock_ALL";

		int rc = 0;
		char *error = NULL;
		string sql_select = "SELECT * FROM " + ticker + ";";

		//cout << "Retrieving values in a table ..." << endl;
		char **results = NULL;
		int rows, columns;
		sqlite3_get_table(db, sql_select.c_str(), &results, &rows, &columns, &error);

		if (rc)
		{
			cerr << "Error executing SQLite3 query: " << sqlite3_errmsg(db) << endl << endl;
			sqlite3_free(error);
			system("pause");
		}

		if (rows > 2800)
		{
			if (ticker == "DWDP" || ticker == "HRS" || ticker == "BMS" || ticker == "RHT")
				continue;
			else
				temp.push_back(StockList[i]);
		}
		sqlite3_free_table(results);
	}
	StockList = temp;
}
//-------------------------Retrieve SPY Return from database-----------------------------------
int GetSPYReturn(sqlite3* db, map<string, float> &ReturnMap, DailyReturn column, string Probation)
{
	int rc = 0;
	char *error = NULL;
	string sql_select = "SELECT * FROM SPY;";

	cout << "Retrieving values in a table ..." << endl;
	char **results = NULL;
	int rows, columns;
	sqlite3_get_table(db, sql_select.c_str(), &results, &rows, &columns, &error);

	if (rc)
	{
		cerr << "Error executing SQLite3 query: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_free(error);
		system("pause");
		return -1;
	}

	if (column == 7)
	{
		if (Probation == "Probation")
		{
			for (int rowCtr = 2894; rowCtr <= rows; ++rowCtr)
			{
				// Determine Cell Position
				int cellPosition = (rowCtr * columns) + column;
				int datePosition = (rowCtr * columns) + 2;
				string stockDate(results[datePosition]);
				ReturnMap[stockDate] = (float)atof(results[cellPosition]);
			}
		}
		else
		{
			for (int rowCtr = 2770; rowCtr <= 2893; ++rowCtr)
			{
				// Determine Cell Position
				int cellPosition = (rowCtr * columns) + column;
				int datePosition = (rowCtr * columns) + 2;
				string stockDate(results[datePosition]);
				ReturnMap[stockDate] = (float)atof(results[cellPosition]);
			}
		}
	}
	if (column == 9)
	{
		for (int rowCtr = 1; rowCtr <= rows; ++rowCtr)
		{
			// Determine Cell Position
			int cellPosition = (rowCtr * columns) + column;
			int datePosition = (rowCtr * columns) + 2;
			string stockDate(results[datePosition]);
			map<string, float>::iterator it = ReturnMap.end();
			ReturnMap.insert(it, pair<string, float>(stockDate, (float)atof(results[cellPosition])));
		}
	}
	
	sqlite3_free_table(results);
	return 0;
}

int GetRiskFree(sqlite3* db, map<string, float> &ReturnMap, vector<string> benchDate)
{
	int rc = 0;
	char *error = NULL;
	string sql_select = "SELECT * FROM RiskFree;";

	cout << "Retrieving values in a table ..." << endl;
	char **results = NULL;
	int rows, columns;
	sqlite3_get_table(db, sql_select.c_str(), &results, &rows, &columns, &error);

	if (rc)
	{
		cerr << "Error executing SQLite3 query: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_free(error);
		system("pause");
		return -1;
	}

	for (int rowCtr = 1; rowCtr <= rows; ++rowCtr)
	{
		// Determine Cell Position
		int cellPosition = (rowCtr * columns) + 9;
		int datePosition = (rowCtr * columns) + 2;
		string stockDate(results[datePosition]);

		map<string, float>::iterator it = ReturnMap.end();
		ReturnMap.insert(it, pair<string, float>(stockDate, (float)atof(results[cellPosition])));
	}
	sqlite3_free_table(results);
	return 0;
}

int GetFromDBTest(string ticker, sqlite3 *db, vector<string> benchDate, vector<Stock> &StockObjList)
{
	cout << "Retrieving Return from DataBase for :" << ticker << endl;
	if (ticker == "BRK-B")
		ticker = "BRKB";
	if (ticker == "BF-B")
		ticker = "BFB";
	if (ticker == "ALL")
		ticker = "Stock_ALL";

	int rc = 0;
	char *error = NULL;
	string sql_select = "SELECT * FROM " + ticker + ";";

	cout << "Retrieving values in a table ..." << endl;
	char **results = NULL;
	int rows, columns;
	sqlite3_get_table(db, sql_select.c_str(), &results, &rows, &columns, &error);

	if (rc)
	{
		cerr << "Error executing SQLite3 query: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_free(error);
		system("pause");
		return -1;
	}

	if (ticker == "BRKB")
		ticker = "BRK-B";
	if (ticker == "BFB")
		ticker = "BF-B";
	if (ticker == "Stock_ALL")
		ticker = "ALL";

	Stock myStock(ticker);
	int index = 0;
	for (int rowCtr = 0; rowCtr <= rows; ++rowCtr)
	{
		int PricePosition = (rowCtr * columns) + 7;
		int datePosition = (rowCtr * columns) + 2;
		string stockDate(results[datePosition]);
		if (stockDate == benchDate[index])
		{
			myStock.addPrice(stockDate, (float)atof(results[PricePosition]));
			index++;
		}
		else
			continue;
	}

	int Num = myStock.AdjPrice("2019-01-02", benchDate);
	if (Num != 146)
		myStock.EuqalizePrice(benchDate);

	StockObjList.push_back(myStock);

	sqlite3_free_table(results);
	cout << "Retrieving Return Completed" << endl;
	return 0;
}

#endif

