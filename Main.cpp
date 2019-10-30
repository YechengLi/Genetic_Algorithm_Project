#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <vector>
#include <stdio.h>
#include <sqlite3.h>

#include "json/json.h"
#include "curl/curl.h"
#include "Fundamental_class.h"
#include "DatabaseLogic.h"
#include "Portfolio_class.h"
using namespace std;

int main()
{
	const char * stockDB_name = "Stocks.db";
	sqlite3 * stockDB = NULL;
	if (OpenDatabase(stockDB_name, stockDB) == -1)
		return -1;
	srand((int)time(NULL));
	ofstream fout;
	fout.open("Results.txt");
	//--------------------Initialize Global Variables-------------------
	vector<string> StockList;
	vector<string> StockList_GA;
	vector<Portfolio> Population(POP_SIZE);
	vector<string> benchDate;
	vector<Fundamental> FundObjList;
	vector<Stock> StockObjList;
	map<string, vector<string>> SectorMap;
	vector<string> SolutionFound;
	vector<string> MonthDate = { "2018-12-03", "2018-12-31", "2019-01-02", "2019-01-31", "2019-02-01", "2019-02-28", "2019-03-01", "2019-03-29", "2019-04-01", "2019-04-30" };
	int MonthCount = 0;

	int Option = 0;

	do
	{
		cout << "Please enter the options' number to proceed:" << endl;
		cout << "1. Populate Stock List" << endl
			<< "2. Retrieve RiskFree Rate and SPY Data" << endl
			<< "3. Retrieve Market and Fundamental Data value and store in the database" << endl
			<< "4. Perform Genetic Algorithm to optimize portfolio and Testing" << endl
			<< "5. Display Result of the best Portfolio" << endl
			<< "6. Exit Program" << endl;
		cout << endl;
		cout << "Enter your option: "; cin >> Option;
		
		switch (Option)
		{
			case 1:
			{
				string sp500_data_request = "https://pkgstore.datahub.io/core/s-and-p-500-companies/constituents_json/data/64dd3e9582b936b0352fdd826ecd3c95/constituents_json.json";
				Json::Value sp500_root;
				if (RetrieveMarketData(sp500_data_request, sp500_root) == -1)
					return -1;
				if (PopulateSP500List(sp500_root, StockList) == -1)
					return -1;
				break;
			}

			case 2:
			{
				//-----------------------Populate Risk Free Rate Data-----------------------------------------	
				std::string INDX_drop_table = "DROP TABLE IF EXISTS RiskFree;";
				if (DropTable(INDX_drop_table.c_str(), stockDB) == -1)
					return -1;
				string INDX_create_table = "CREATE TABLE RiskFree (id INT PRIMARY KEY NOT NULL, symbol CHAR(20) NOT NULL, date CHAR(20) NOT NULL, open REAL NOT NULL, high REAL NOT NULL, low REAL NOT NULL, close REAL NOT NULL, adjusted_close REAL NOT NULL, volume INT NOT NULL, return REAL NOT NULL);";
				if (CreateTable(INDX_create_table.c_str(), stockDB) == -1)
					return -1;

				string risk_free_data_request = "https://eodhistoricaldata.com/api/eod/TNX.INDX?from=2008-01-01&to=2019-07-31&api_token=5ba84ea974ab42.45160048&period=d&fmt=json";
				Json::Value INDX_root;
				if (RetrieveMarketData(risk_free_data_request, INDX_root) == -1)
					return -1;
				if (PopulateStockTable(INDX_root, "RiskFree", stockDB) == -1)
					return -1;

				//-----------------------Populate SPY Data-----------------------------------------	
				std::string SPY_drop_table = "DROP TABLE IF EXISTS SPY;";
				if (DropTable(SPY_drop_table.c_str(), stockDB) == -1)
					return -1;
				string SPY_create_table = "CREATE TABLE SPY (id INT PRIMARY KEY NOT NULL, symbol CHAR(20) NOT NULL, date CHAR(20) NOT NULL, open REAL NOT NULL, high REAL NOT NULL, low REAL NOT NULL, close REAL NOT NULL, adjusted_close REAL NOT NULL, volume INT NOT NULL, return REAL NOT NULL);";
				if (CreateTable(SPY_create_table.c_str(), stockDB) == -1)
					return -1;

				string SPY_data_request = "https://eodhistoricaldata.com/api/eod/SPY?from=2008-01-01&to=2019-07-31&api_token=5ba84ea974ab42.45160048&period=d&fmt=json";
				Json::Value SPY_root;
				if (RetrieveMarketData(SPY_data_request, SPY_root) == -1)
					return -1;
				if (PopulateStockTable(SPY_root, "SPY", stockDB) == -1)
					return -1;

				break;
			}

			case 3:
			{
				//-----------------------Create Table for S&P500 List-----------------------------------------
				std::string sp500_drop_table = "DROP TABLE IF EXISTS SP500;";
				if (DropTable(sp500_drop_table.c_str(), stockDB) == -1)
					return -1;
				string sp500_create_table = "CREATE TABLE SP500 (id INT PRIMARY KEY NOT NULL, symbol CHAR(20) NOT NULL, name CHAR(20) NOT NULL, sector CHAR(20) NOT NULL);";
				if (CreateTable(sp500_create_table.c_str(), stockDB) == -1)
					return -1;

				//-----------------------Create Table for Fundamental Data-----------------------------------------
				std::string Fundamental_drop_table = "DROP TABLE IF EXISTS FUNDAMENTAL;";
				if (DropTable(Fundamental_drop_table.c_str(), stockDB) == -1)
					return -1;
				string Fundamental_create_table = "CREATE TABLE FUNDAMENTAL(id INT PRIMARY KEY NOT NULL, symbol CHAR(20) NOT NULL, pe_ratio REAL NOT NULL, dividend_Yield REAL NOT NULL, beta REAL NOT NULL, high_52 REAL NOT NULL, low_52 REAL NOT NULL, MA_50 REAL NOT NULL, MA_200 REAL NOT NULL, MarketCap REAL NOT NULL, Target_Price REAL NOT NULL, peg_ratio REAL NOT NULL);";
				if (CreateTable(Fundamental_create_table.c_str(), stockDB) == -1)
					return -1;
				
				//-----------------------Loop Through StockList to get fundamental and trading data ---------------------------
				for (int i = 0; i < StockList.size(); i++)
				{

					string stockDB_symbol = StockList[i];
					string stock_url_common = "https://eodhistoricaldata.com/api/eod/";
					string stock_start_date = "2008-01-01";
					string stock_end_date = "2019-07-31";
					string api_token = "5ba84ea974ab42.45160048";
					if (stockDB_symbol == "LUK")
						stock_end_date = "2018-05-23";
					if (stockDB_symbol == "MON")
						stock_end_date = "2018-06-06";
					if (stockDB_symbol == "PX")
						stock_end_date = "2018-06-04";
					string stockDB_data_request = stock_url_common + stockDB_symbol + ".US?" +
						"from=" + stock_start_date + "&to=" + stock_end_date + "&api_token=" + api_token + "&period=d&fmt=json";
					string fundamental_data_request = "https://eodhistoricaldata.com/api/fundamentals/" + stockDB_symbol + ".US?api_token=5ba84ea974ab42.45160048&period=d&fmt=json";

					//https://eodhistoricaldata.com/api/eod/LUK.US?from=2008-01-01&to=2019-07-31&api_token=5ba84ea974ab42.45160048&period=d&fmt=json

					Json::Value stockDB_root;  // will contains the root value after parsing.
					if (RetrieveMarketData(stockDB_data_request, stockDB_root) == -1)
						return -1;

					if (stockDB_symbol == "BRK-B")
						stockDB_symbol = "BRKB";
					if (stockDB_symbol == "BF-B")
						stockDB_symbol = "BFB";
					if (stockDB_symbol == "ALL")
						stockDB_symbol = "Stock_ALL";

					//-------------------------Drop or create table for each stock-----------------------------------------
					std::string stockDB_drop_table = "DROP TABLE IF EXISTS " + stockDB_symbol + ";";
					if (DropTable(stockDB_drop_table.c_str(), stockDB) == -1)
						return -1;
					string stockDB_create_table = "CREATE TABLE " + stockDB_symbol
						+ "(id INT PRIMARY KEY NOT NULL,"
						+ "symbol CHAR(20) NOT NULL,"
						+ "date CHAR(20) NOT NULL,"
						+ "open REAL NOT NULL,"
						+ "high REAL NOT NULL,"
						+ "low REAL NOT NULL,"
						+ "close REAL NOT NULL,"
						+ "adjusted_close REAL NOT NULL,"
						+ "volume REAL NOT NULL,"
						+ "return REAL NOT NULL);";
					if (CreateTable(stockDB_create_table.c_str(), stockDB) == -1)
						return -1;

					cout << "------------------------" << stockDB_symbol << "-------------------------------------";
					if (PopulateStockTable(stockDB_root, stockDB_symbol, stockDB) == -1) //Problems
						return -1;

					//--------------------------------Parse and populate fundamental data----------------------------
					Json::Value stockDB_fundamental_root;
					if (RetrieveMarketData(fundamental_data_request, stockDB_fundamental_root) == -1)
						return -1;
					if (PopulateSP500TableAndFundamental(stockDB_fundamental_root, stockDB, i + 1) == -1)
						return -1;
				}
				break;
			}

			case 4:
			{
				bool BestSolution = false;
				
				while (!BestSolution)
				{
					if (MonthCount + 1 == 9)
					{
						cout << "We didn't found any best solution. Need to run it again" << endl;
						break;
					}
					StockList_GA = StockList;
					ReplaceList(StockList_GA, stockDB);

					//Benchmark Date
					GetBenchDate(stockDB, benchDate);
					
					//List of All MarketData
					vector<Stock> StockObjList_Full;

					for (int i = 0; i < StockList_GA.size(); i++)
					{
						string symbol = StockList_GA[i];
						GetFromDB(symbol, stockDB, StockObjList_Full, benchDate, MonthDate[MonthCount + 1]);
						GetFromFundamental(symbol, stockDB, FundObjList);
					}

					Select(StockObjList_Full, StockObjList, "2008-01-02", MonthDate[MonthCount + 1]);
					PopulateSector(stockDB, FundObjList);


					//Sector Map
					GetCategStocks(SectorMap, FundObjList);

					//GetSPY
					cout << "Retrieving SPY from DataBase " << endl;
					map<string, float> SPYReturn_Full;
					GetSPYReturn(stockDB, SPYReturn_Full, Return, "Normal");
					map<string, float> SPYReturn;
					auto itr = SPYReturn_Full.find("2008-01-02");
					auto ite = SPYReturn_Full.find(MonthDate[MonthCount + 1]);
					ite++;
					for (auto it = itr; it != ite; it++)
						SPYReturn[it->first] = it->second;
					float sum = 0.0f;
					for (map<string, float>::iterator it = SPYReturn.begin(); it != SPYReturn.end(); it++)
						sum += it->second;
					float AvgSPY = sum;
					cout << "-------------------------------" << AvgSPY << endl;
					cout << "Retrieving SPY Completed" << endl;


					//GetRiskFree
					cout << "Retrieving Risk Free from DataBase " << endl;
					map<string, float> RfReturn_Full;
					GetRiskFree(stockDB, RfReturn_Full, benchDate);
					map<string, float> RfReturn;
					auto itr2 = RfReturn_Full.find("2008-01-02");
					auto ite2 = RfReturn_Full.find(MonthDate[MonthCount + 1]);
					ite2++;
					for (auto it = itr2; it != ite2; it++)
						RfReturn[it->first] = it->second;
					float sum2 = 0.0f;
					for (map<string, float>::iterator it = RfReturn.begin(); it != RfReturn.end(); it++)
						sum2 += it->second;
					float AvgRiskFree = sum2;
					cout << "-------------------------------" << AvgRiskFree << endl;
					cout << "Retrieving Risk Free Completed" << endl;

					//first create a random population, all with zero fitness.
					for (int i = 0; i < POP_SIZE; i++)
					{
						Population[i].PopulatePureList(FundObjList, SectorMap);
						cout << "This is " << i + 1 << "Portfolio" << endl;
						Population[i].DisplayStockTicker();
						Population[i].SetEverything(FundObjList, StockObjList, AvgRiskFree, AvgSPY, RfReturn);
					}

					int GenerationsRequiredToFindASolution = 0;

					//we will set this flag if a solution has been found
					bool bFound = false;

					while (!bFound)
					{
						fout << "------------------This is" << GenerationsRequiredToFindASolution << "th Generation---------------" << endl;
						//Assign fitness to each portfolio
						for (int i = 0; i < POP_SIZE; i++)
						{
							Population[i].SetZero();
							Population[i].CalFitness();
							cout << "-----------" << Population[i].GetFitness() << endl;
							fout << "**" << i << "Fitness:" << Population[i].GetFitness() << endl;
							map<string, int> StockMapTemp = Population[i].GetPureList();
							fout << "This Portfolio Contains";
							for (auto it = StockMapTemp.begin(); it != StockMapTemp.end(); it++)
								fout << it->first << "  ";
							fout << endl << endl;
						}

						//Sort the first generation according to their fitness
						std::sort(Population.begin(), Population.end());
						std::reverse(Population.begin(), Population.end());

						for (int i = 0; i < POP_SIZE; i++)
						{
							if (Population[i].GetFitness() >= 96.3)
							{
								cout << "\nSolution found in " << GenerationsRequiredToFindASolution << " generations!" << endl << endl;;
								cout << "Result: ";
								Population[i].DisplayStockTicker();
								Population[i].GetPortAvgR();

								bFound = true;

								break;
							}
						}

						vector<Portfolio> temp(POP_SIZE);

						int cPop = 0;

						//loop until we have created POP_SIZE * CROSSOVER_RATE new chromosomes

						int NumberOfChange = int(POP_SIZE * CROSSOVER_RATE);
						cout << "Generating New Portfolio" << endl;
						while (cPop < NumberOfChange)
						{
							// we are going to create the new population by grabbing members of the old population
							// two at a time via roulette wheel selection.

							map<string, int> offspring1 = Population[cPop].GetPureList();
							map<string, int> offspring2 = Population[cPop].GetPureList();


							//add crossover dependent on the crossover rate
							Crossover(offspring1, offspring2, StockList_GA);

							//now mutate dependent on the mutation rate
							Mutate(offspring1, StockList_GA, FundObjList, SectorMap);
							Mutate(offspring2, StockList_GA, FundObjList, SectorMap);


							//add these offspring to the new population. (assigning zero as their
							//fitness scores)
							Population[99 - cPop] = Portfolio(offspring1, FundObjList, StockObjList, AvgRiskFree, AvgSPY, RfReturn);
							Population[98 - cPop] = Portfolio(offspring2, FundObjList, StockObjList, AvgRiskFree, AvgSPY, RfReturn);

							cPop += 2;

						}//end loop

						cout << "Generation Complete" << endl;

						++GenerationsRequiredToFindASolution;

						// exit app if no solution found within the maximum allowable number of generations
						if (GenerationsRequiredToFindASolution > MAX_ALLOWABLE_GENERATIONS)
						{
							cout << "No solutions found this run!";
							bFound = true;
						}
					}


					//--------------------------------Initializing Back Test----------------------------
					auto it = find(benchDate.begin(), benchDate.end(), "2019-01-02");
					int index = distance(benchDate.begin(), it);
					vector<string> benchDate_Back;
					for (int i = index; i < benchDate.size(); i++)
						benchDate_Back.push_back(benchDate[i]);

					vector<Stock> StockObjList_Back_Full;
					for (int i = 0; i < StockList_GA.size(); i++)
					{
						string symbol = StockList_GA[i];
						GetFromDBTest(symbol, stockDB, benchDate_Back, StockObjList_Back_Full);
					}
					vector<Stock> StockObjList_Back;
					Select4Test(StockObjList_Back_Full, StockObjList_Back, "2019-01-02", "2019-06-28");

					map<string, float> SPYPrice_Back;
					GetSPYReturn(stockDB, SPYPrice_Back, adjusted_close, "Back");

					//--------------------------------Initializing Probation Test----------------------------
					//GetSPY
					cout << "Retrieving SPY from DataBase " << endl;
					map<string, float> SPYPrice_Probation;
					GetSPYReturn(stockDB, SPYPrice_Probation, adjusted_close, "Probation");

					vector<Stock> StockObjList_Probation;
					Select4Test(StockObjList_Back_Full, StockObjList_Probation, "2019-07-01", "2019-07-31");
					
					vector<int> Resultvector;
					string indicator = "Null";
					for (int i = 0; i < Population.size(); i++)
					{
						fout << "*******************This is " << i + 1 << "th Portfolio**********************" << endl;
						vector<string>temp = Population[i].GetStockTicker();
						fout << "The Portfolio contains: " << endl;
						for (int i = 0; i < temp.size(); i++)
							fout << temp[i] << endl;
						Population[i].CalFitness();
						vector<string> BeatTemp, MissTemp, Probation_Beat, Probation_Miss, MonthlyBeat;
						cout << "Back Testing results" << endl;
						Population[i].BackTest(StockObjList_Back, SPYPrice_Back, BeatTemp, MissTemp, MonthlyBeat);
						fout << "-------------------Back Test results--------------------" << endl;
						int count = 0;
						for (int i = 0; i < BeatTemp.size(); i++)
						{
							fout << "Beat on" << BeatTemp[i] << endl;
							count++;
						}
						for (int i = 0; i < MissTemp.size(); i++)
							fout << "Miss On" << MissTemp[i] << endl;
						fout << "The result is " << count << "/26" << endl;
						fout << "Portfolio Beat in:";
						for (int i = 0; i < MonthlyBeat.size(); i++)
							fout << MonthlyBeat[i] << "  ";
						fout << endl;
						Resultvector.push_back(MonthlyBeat.size());


						cout << "Probation Testing results" << endl;
						indicator = Population[i].ProbationTest(StockObjList_Probation, SPYPrice_Probation, Probation_Beat, Probation_Miss);
						fout << "-------------------Probation Test results--------------------" << endl;
						for (int i = 0; i < Probation_Beat.size(); i++)
							fout << "Beat On" << Probation_Beat[i] << endl;
						for (int i = 0; i < Probation_Miss.size(); i++)
							fout << "Miss On" << Probation_Miss[i] << endl;
						fout << "**********" << indicator << endl;

						fout << "--------------------------" << endl;
						fout << "Sharp:" << Population[i].GetSharp() << endl;
						fout << "PortfolioReturn:" << Population[i].GetPortAvgR() << endl;
						fout << "PortfolioVar:" << Population[i].GetPortDeviation() << endl;
						fout << "JAlpha:" << Population[i].GetJAlpha() << endl;
						fout << "Treynor:" << Population[i].GetTreynor() << endl;
						fout << "DiversIndex:" << Population[i].GetDivers() << endl;
						fout << "PE:" << Population[i].GetPE() << endl;
						fout << "Beta:" << Population[i].GetBETA() << endl;
						fout << "DivYield:" << Population[i].GetDIV() << endl;
						fout << "Cap:" << Population[i].GetCap() << endl;
						fout << "FundAnalysis:" << Population[i].GetFund() << endl;
						fout << "Fitness:" << Population[i].GetFitness() << endl;	
					}
					if (find(Resultvector.begin(), Resultvector.end(), 6) != Resultvector.end() && indicator == "Yes")
					{
						auto itb = find(Resultvector.begin(), Resultvector.end(), 6);
						int BestIndex = distance(Resultvector.begin(), itb);
						SolutionFound = Population[BestIndex].GetStockTicker();
						BestSolution = true;
					}
					else
					{
						MonthCount += 2;
					}
				}

				if (MonthCount + 1 != 9)
				{
					cout << "We have found the portfolio that passes both back test and probation test" << endl;
					cout << "The Portfolio contains: " << endl;
					for (int i = 0; i < SolutionFound.size(); i++)
						cout << SolutionFound[i] << endl;
					system("Pause");
				}

				break;
			}

			case 5:
			{
				int subOption = 0;
				cout << "Select 1 if you have not run option 4; Otherwise, select 2" << endl;
				cout << "Enter Your Selection:"; cin >> subOption;

				StockList_GA = StockList;
				ReplaceList(StockList_GA, stockDB);

				//Benchmark Date
				GetBenchDate(stockDB, benchDate);

				//List of All MarketData
				vector<Stock> StockObjList_Full;

				for (int i = 0; i < StockList_GA.size(); i++)
				{
					string symbol = StockList_GA[i];
					GetFromDB(symbol, stockDB, StockObjList_Full, benchDate, MonthDate[MonthCount + 1]);
					GetFromFundamental(symbol, stockDB, FundObjList);
				}

				Select(StockObjList_Full, StockObjList, "2008-01-02", MonthDate[MonthCount + 1]);
				PopulateSector(stockDB, FundObjList);

				//Sector Map
				GetCategStocks(SectorMap, FundObjList);

				//--------------------------------Initializing Back Test----------------------------
				auto it = find(benchDate.begin(), benchDate.end(), "2019-01-02");
				int index = distance(benchDate.begin(), it);
				vector<string> benchDate_Back;
				for (int i = index; i < benchDate.size(); i++)
					benchDate_Back.push_back(benchDate[i]);
				
				vector<Stock> StockObjList_Back_Full;
				for (int i = 0; i < StockList_GA.size(); i++)
				{
					string symbol = StockList_GA[i];
					GetFromDBTest(symbol, stockDB, benchDate_Back, StockObjList_Back_Full);
				}
				vector<Stock> StockObjList_Back;
				Select4Test(StockObjList_Back_Full, StockObjList_Back, "2019-01-02", "2019-06-28");

				map<string, float> SPYPrice_Back;
				GetSPYReturn(stockDB, SPYPrice_Back, adjusted_close, "Back");

				//--------------------------------Initializing Probation Test----------------------------
				//GetSPY
				cout << "Retrieving SPY from DataBase " << endl;
				map<string, float> SPYPrice_Probation;
				GetSPYReturn(stockDB, SPYPrice_Probation, adjusted_close, "Probation");

				vector<Stock> StockObjList_Probation;
				Select4Test(StockObjList_Back_Full, StockObjList_Probation, "2019-07-01", "2019-07-31");
				
				cout << "----------------" << "Probation List is size of" << StockObjList_Probation.size() << endl;

				//--------------------------------Initializing GA List----------------------------
				//GetSPY
				cout << "Retrieving SPY from DataBase " << endl;
				map<string, float> SPYReturn_Full;
				GetSPYReturn(stockDB, SPYReturn_Full, Return, "Normal");
				map<string, float> SPYReturn;
				auto itr = SPYReturn_Full.find("2008-01-02");
				auto ite = SPYReturn_Full.find(MonthDate[MonthCount + 1]);
				ite++;
				for (auto it = itr; it != ite; it++)
					SPYReturn[it->first] = it->second;
				float sum = 0.0f;
				for (map<string, float>::iterator it = SPYReturn.begin(); it != SPYReturn.end(); it++)
					sum += it->second;
				float AvgSPY = sum;
				cout << "-------------------------------" << AvgSPY << endl;
				cout << "Retrieving SPY Completed" << endl;


				//GetRiskFree
				cout << "Retrieving Risk Free from DataBase " << endl;
				map<string, float> RfReturn_Full;
				GetRiskFree(stockDB, RfReturn_Full, benchDate);
				map<string, float> RfReturn;
				auto itr2 = RfReturn_Full.find("2008-01-02");
				auto ite2 = RfReturn_Full.find(MonthDate[MonthCount + 1]);
				ite2++;
				for (auto it = itr2; it != ite2; it++)
					RfReturn[it->first] = it->second;
				float sum2 = 0.0f;
				for (map<string, float>::iterator it = RfReturn.begin(); it != RfReturn.end(); it++)
					sum2 += it->second;
				float AvgRiskFree = sum2;
				cout << "-------------------------------" << AvgRiskFree << endl;
				cout << "Retrieving Risk Free Completed" << endl;

				//--------------------------------Test Best Portfolio----------------------------
				vector<string> test1;
				if(subOption == 2)
					test1 = SolutionFound;
				else if (subOption == 1)
					test1 = { "AMD", "AWK", "COF", "CTL", "EL", "ESS", "IDXX", "IFF", "NKE", "OKE" };

				map<string, int> List1;
				for (int i = 0; i < test1.size(); i++)
				{
					auto it = find(StockList_GA.begin(), StockList_GA.end(), test1[i]);
					int index = distance(StockList_GA.begin(), it);
					List1[test1[i]] = index;
				}

				Portfolio Port1 = Portfolio(List1, FundObjList, StockObjList, AvgRiskFree, AvgSPY, RfReturn);

				cout << "*******************This is the best Portfolio**********************" << endl;
				vector<string>temp = Port1.GetStockTicker();
				cout << "The Portfolio contains: " << endl;
				for (int i = 0; i < temp.size(); i++)
					cout << temp[i] << endl;
				Port1.CalFitness();
				vector<string> BeatTemp, MissTemp, Probation_Beat, Probation_Miss, MonthlyBeat;
				cout << "Back Testing results" << endl;
				Port1.BackTest(StockObjList_Back, SPYPrice_Back, BeatTemp, MissTemp, MonthlyBeat);
				cout << "-------------------Back Testing results--------------------" << endl;
				int count = 0;
				for (int i = 0; i < BeatTemp.size(); i++)
				{
					cout << "Beat on" << BeatTemp[i] << endl;
					count++;
				}
				for (int i = 0; i < MissTemp.size(); i++)
					cout << "Miss On" << MissTemp[i] << endl;
				cout << "The result is " << count << "/26" << endl;
				cout << "Portfolio Beat in:";
				for (int i = 0; i < MonthlyBeat.size(); i++)
					cout << MonthlyBeat[i] << "  ";
				cout << endl;


				cout << "Probation Testing results" << endl;
				string indicator = Port1.ProbationTest(StockObjList_Probation, SPYPrice_Probation, Probation_Beat, Probation_Miss);
				cout << "-------------------Back Testing results--------------------" << endl;
				for (int i = 0; i < Probation_Beat.size(); i++)
					cout << "Beat On" << Probation_Beat[i] << endl;
				for (int i = 0; i < Probation_Miss.size(); i++)
					cout << "Miss On" << Probation_Miss[i] << endl;
				cout << "**********" << indicator << endl;

				cout << "--------------------------" << endl;
				cout << "Sharp:" << Port1.GetSharp() << endl;
				cout << "PortfolioReturn:" << Port1.GetPortAvgR() << endl;
				cout << "PortfolioVar:" << Port1.GetPortDeviation() << endl;
				cout << "JAlpha:" << Port1.GetJAlpha() << endl;
				cout << "Treynor:" << Port1.GetTreynor() << endl;
				cout << "DiversIndex:" << Port1.GetDivers() << endl;
				cout << "PE:" << Port1.GetPE() << endl;
				cout << "Beta:" << Port1.GetBETA() << endl;
				cout << "DivYield:" << Port1.GetDIV() << endl;
				cout << "Cap:" << Port1.GetCap() << endl;
				cout << "FundAnalysis:" << Port1.GetFund() << endl;
				cout << "Fitness:" << Port1.GetFitness() << endl;
				
				break;
			}

			case 6:
			{
				cout << "Programming Terminating" << endl;
				break;
			}

			default:
			{
				cout << endl << "Invalid input. Input should be intenger within range [1,6]" << endl << endl;
				Option = 0; //Reset Option
				break;
			}
		}
	}
	while (Option >= 0 && Option < 6);
	//Close Database
	CloseDatabase(stockDB);
	system("Pause");

	return 0;
}

