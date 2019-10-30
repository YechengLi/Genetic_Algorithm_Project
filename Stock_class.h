#pragma once
#ifndef Stock_class_h
#define Stock_class_h
using namespace std;
#include <vector>
#include <iostream>
#include "Trade_class.h"
#include "UtilityFunctions.h"

#define NUM_OF_STOCKS 505;

class Stock
{
private:
	string symbol;
	vector<Trade> trades;
	map<string, float> GAReturn;
	map<string, float> TradingPrice;
	Vector DailyReturn;
	float LastDatePrice;

public:
	Stock(string symbol_) :symbol(symbol_)
	{}
	~Stock() {}
	Stock() {}
	void addTrade(Trade aTrade)
	{
		trades.push_back(aTrade);
	}

	Vector GetDailyReturn() { return DailyReturn; }
	string GetSymbol() { return symbol; }
	map<string, float> GetGAReturn() { return GAReturn; }
	map<string, float> GetTradingPrice() { return TradingPrice; }
	void SetGAReturn(map<string, float> GAReturn_) { GAReturn = GAReturn_; }
	void SetPrice(float Price_) { LastDatePrice = Price_; }
	float GetLastDatePrice() { return LastDatePrice; }

	vector<string> GetDate()
	{
		vector<string> temp;
		for (map<string, float>::iterator it = GAReturn.begin(); it != GAReturn.end(); it++)
		{
			temp.push_back(it->first);
		}
		return temp;
	}

	float FindReturn(string Date)
	{
		return GAReturn[Date];
	}

	void EuqalizeLength(vector<string> benchDate)
	{
		map<string, float> Replacement;
		auto ite = GAReturn.begin();
		
		for (int i = 0; i < benchDate.size(); i++)
		{
			if (ite == GAReturn.end());
			{
				Replacement.insert(pair<string, float>(benchDate[i], 0.0f));
				continue;
			}
			if (ite->first == benchDate[i])
			{
				Replacement.insert(pair<string, float>(ite->first, ite->second));
				ite++;
			}
			else
				Replacement.insert(pair<string, float>(benchDate[i], 0.0f));

		}

		GAReturn = Replacement;
	}

	void EuqalizePrice(vector<string> benchDate)
	{
		map<string, float> Replacement;
		auto itr = TradingPrice.begin();
		float LastPrice = 0;
		for (int i = 0; i < benchDate.size(); i++)
		{
			if (itr == TradingPrice.end());
			{
				Replacement.insert(pair<string, float>(benchDate[i], LastPrice));
				continue;
			}
			if (itr->first == benchDate[i])
			{
				Replacement.insert(pair<string, float>(itr->first, itr->second));
				LastPrice = itr->second;
				itr++;
			}
			else
			{
				Replacement.insert(pair<string, float>(benchDate[i], LastPrice));
			}
		}

		TradingPrice = Replacement;
	}

	int AdjPrice(string Date, vector<string> benchDate)
	{
		map<string, float>temp;
		auto it = TradingPrice.find(Date);
		int count = 0;
		if (it == TradingPrice.end())
		{
			auto ite = TradingPrice.begin();
			for (int i = 0; i < benchDate.size(); i++)
			{
				if (ite->first != benchDate[i])
					temp[benchDate[i]] = 0;
				else
				{
					temp[benchDate[i]] = ite->second;
					ite++;
				}
				count++;
			}
		}
		else
		{
			for (auto itr = it; itr != TradingPrice.end(); itr++)
			{
				temp[itr->first] = itr->second;
				count++;
			}
		}
		TradingPrice = temp;
		return count;
	}

	void addReturn(string date, float Return)
	{
		map<string, float>::iterator it = GAReturn.end();
		GAReturn.insert(it, pair<string, float>(date, Return));
	}

	void addPrice(string date, float Price)
	{
		TradingPrice[date] = Price;
	}

	float FindPrice(string date) {return TradingPrice[date];}

	float CalAvgReturn()
	{
		float sum = 0.0f;
		int count = 0;
		for (map<string, float>::iterator it = GAReturn.begin(); it != GAReturn.end(); it++)
		{
			sum += it->second;
			count++;
		}
		return sum / count;
	}

	float CalReturn(int count)
	{
		if (count == 0)
		{
			DailyReturn.push_back(0);
			return 0.0f;
		}
		else
		{
			float CalculatedReturn = (trades[count].GetAdjClose() - trades[count - 1].GetAdjClose()) / trades[count - 1].GetAdjClose();
			DailyReturn.push_back(CalculatedReturn);
			return CalculatedReturn;
		}
	}

	void SelectData(string StartDate, string EndDate)
	{
		map<string, float> temp;
		auto itr = GAReturn.find(StartDate);
		auto ite = GAReturn.find(EndDate);
		ite++;
		for (auto it = itr; it != ite; it++)
			temp[it->first] = it->second;
		GAReturn = temp;
	}
	
	void SelectTestData(string StartDate, string EndDate)
	{
		map<string, float> temp;
		auto itr = TradingPrice.find(StartDate);
		auto ite = TradingPrice.find(EndDate);
		ite++;
		for (auto it = itr; it != ite; it++)
			temp[it -> first] = it->second;
		TradingPrice = temp;
	}
	friend ostream & operator << (ostream & out, const Stock & s)
	{
		out << "Symbol: " << s.symbol << endl;
		for (vector<Trade>::const_iterator itr = s.trades.begin(); itr != s.trades.end(); itr++)
			out << *itr;
		return out;
	}
};

void Select(vector<Stock> &StockObjList_Full, vector<Stock> &StockObjList, string StartDate, string EndDate)
{
	for (int i = 0; i < StockObjList_Full.size(); i++)
	{
		Stock temp;
		temp = StockObjList_Full[i];
		temp.SelectData(StartDate, EndDate);
		StockObjList.push_back(temp);
	}
}

void Select4Test(vector<Stock> &StockObjList_Full, vector<Stock> &StockObjList, string StartDate, string EndDate)
{
	for (int i = 0; i < StockObjList_Full.size(); i++)
	{
		Stock temp;
		temp = StockObjList_Full[i];
		temp.SelectTestData(StartDate, EndDate);
		StockObjList.push_back(temp);
	}
}

#endif

