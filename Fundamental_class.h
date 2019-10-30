
#ifndef Fundamental_class_h
#define Fundamental_class_h

#include <iostream>
#include <vector>
using namespace std;

#include "Trade_class.h"
#include "Stock_class.h"


class Fundamental
{
private:
	string symbol;
	string sector;
	float PERatio;
	float DivYield;
	float Beta;
	float High52;
	float Low52;
	float MA50;
	float MA200;
	float MarketCap;
	float TargetPrice;
	float PEGRatio;

public:
	Fundamental(string symbol_):symbol(symbol_){}
	~Fundamental() {}
	Fundamental() {}

	void PopulateFund(float PERatio_, float DivYield_, float Beta_, float High52_, float Low52_, float MA50_, float MA200_, float MarketCap_, float TargetPrice_, float PEGRatio_)
	{
		PERatio = PERatio_;
		DivYield = DivYield_;
		Beta = Beta_;
		High52 = High52_;
		Low52 = Low52_;
		MA50 = MA50_;
		MA200 = MA200_;
		MarketCap = MarketCap_;
		TargetPrice = TargetPrice_;
		PEGRatio = PEGRatio_;
	}

	void SetSector(string sector_) { sector = sector_; }
	
	string GetSector() { return sector; }
	string GetSymbol() { return symbol; }
	float GetMCap() {return MarketCap;}
	float GetPE() { return PERatio; }
	float GetDivY() { return DivYield; }
	float GetBeta() { return Beta; }
	float GetHigh52() { return High52; }
	float GetLow52() { return Low52; }
	float GetMA50() { return MA50; }
	float GetMA200() { return MA200; }
	float GetTarget() { return TargetPrice; }
	float GetPEG() { return PEGRatio; }

	friend ostream & operator << (ostream & out, const Fundamental & t)
	{
		out  << " PERatio: " << t.PERatio << " DividendYield " << t.DivYield << " Beta " << t.Beta << " 52WeekHigh " << t.High52 << " 52WeekLow " << t.Low52 << " 50DayMA " << t.MA50 << " 200DayMA " << t.MA200  << " MarketCap " << t.MarketCap << endl;
		return out;
	}
};



#endif