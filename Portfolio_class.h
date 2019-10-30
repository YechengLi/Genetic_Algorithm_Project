#pragma once
#ifndef Portfolio_class_h
#define Portfolio_class_h

#include <algorithm>
#include <iomanip>
#include "Fundamental_class.h"
#include "UtilityFunctions.h"
#include "DatabaseLogic.h"

using namespace std;

vector<string> Sectors = { "Industrials", "Healthcare", "Technology", "Energy", "Consumer Cyclical", "Utilities", "Financial Services", "Basic Materials", "Real Estate", "Consumer Defensive", "Communication Services", "Other"};


#define RANDOM_NUM      ((float)rand()/RAND_MAX)
#define CROSSOVER_RATE            0.7
#define MUTATION_RATE             0.03
#define POP_SIZE                  100           //must be an even number
#define CHROMO_LENGTH             300
#define GENE_LENGTH               4    //4 bites is 1 gene
#define MAX_ALLOWABLE_GENERATIONS   1000



class Portfolio
{
private:
	map<string, int> PureList;
	map<string, float> weights;
	map<string, float> Portfolio_Return;
	float Portfolio_Avg_Return;
	float Portfolio_Deviation;
	float SharpRatio;
	float JAlpha;
	float TreynorRatio;
	float DiversIndex;
	float PE;
	float DIVYIELD;
	float BETA;
	float MarketCap;
	float PEGRatio;
	float fitness;
	float FundAnalysis;

public:
	Portfolio(map<string, int> PureList_, vector<Fundamental> &FundObjList, vector<Stock> &StockObjList, float AvgRiskFree, float AvgSPY, map<string, float> RfReturn)
	{
		map<string, float> weights_;
		map<string, float> Portfolio_Return_;
		weights = weights_;
		Portfolio_Return = Portfolio_Return_;
		PureList = PureList_;

		Portfolio_Avg_Return = 0;
		Portfolio_Deviation = 0;
		SharpRatio = 0;
		JAlpha = 0;
		TreynorRatio = 0;
		DiversIndex = 0;
		PE = 0;
		DIVYIELD = 0;
		BETA = 0;
		MarketCap = 0;
		PEGRatio = 0;
		fitness = 0;
		FundAnalysis = 0;

		SetEverything(FundObjList, StockObjList, AvgRiskFree, AvgSPY, RfReturn);
	}
	~Portfolio() {}
	Portfolio()
	{
		map<string, int> PureList_;
		map<string, float> weights_;
		map<string, float> Portfolio_Return_;

		PureList = PureList_;
		weights = weights_;
		Portfolio_Return = Portfolio_Return_;

		Portfolio_Avg_Return = 0;
		Portfolio_Deviation = 0;
		SharpRatio = 0;
		JAlpha = 0;
		TreynorRatio = 0;
		DiversIndex = 0;
		PE = 0;
		DIVYIELD = 0;
		BETA = 0;
		MarketCap = 0;
		PEGRatio = 0;
		fitness = 0;
		FundAnalysis = 0;
	}

	void SetEverything(vector<Fundamental> &FundObjList, vector<Stock> &StockObjList, float AvgRiskFree, float AvgSPY, map<string, float> RfReturn)
	{
		GetWeights(FundObjList);
		cout << "**********" << endl;
		PopulateFundData(FundObjList);
		cout << "**********" << endl;
		CalPortfolioR(StockObjList);
		cout << "**********" << endl;
		CalPortAvgR();
		cout << "**********" << endl;
		CalPortDeviation();
		cout << "**********" << endl;
		CalSharpRatio(RfReturn);
		cout << "**********" << endl;
		CalAlpha(StockObjList, AvgSPY, AvgRiskFree);
		cout << "**********" << endl;
		CalTreynor(RfReturn);
		cout << "**********" << endl;
		GetDiversIndex();
		cout << "**********" << endl;
		FundamentalAnalysis(FundObjList, StockObjList);
		cout << "**********" << endl;
	}

	float GetPortAvgR() { return Portfolio_Avg_Return; }
	float GetPortDeviation() { return Portfolio_Deviation; }
	map<string, int> GetPureList() { return PureList; }
	float GetFitness() { return fitness; }
	void SetZero() { fitness = 0; }
	float GetSharp() {return SharpRatio;}
	float GetJAlpha() { return JAlpha; }
	float GetTreynor() { return TreynorRatio; }
	float GetDivers() { return DiversIndex; }
	float GetPE() { return PE; }
	float GetDIV() { return DIVYIELD; }
	float GetBETA() { return BETA; }
	float GetCap() { return MarketCap; }
	float GetFund() { return FundAnalysis; }

	void DisplayStockTicker()
	{
		for (auto it = PureList.begin(); it != PureList.end(); it++)
			cout << it->first << endl;
	}

	vector<string> GetStockTicker()
	{
		vector<string>temp;
		for (auto it = PureList.begin(); it != PureList.end(); it++)
			temp.push_back(it->first);
		return temp;
	}

	void PopulatePureList(vector<Fundamental> &FundObjList, map<string, vector<string>> &SectorMap)
	{
		vector<string> temp;
		vector<int> sectorTemp;
		
		int Num = 0;
		while (Num <= 9)
		{
			int randomNum = (int)(RANDOM_NUM * 12);
			if (randomNum == 12)
				randomNum -= 1;
			if (find(sectorTemp.begin(), sectorTemp.end(), randomNum) == sectorTemp.end())
			{
				sectorTemp.push_back(randomNum);
				Num++;
			}
			else
				continue;
		}

		for (int i = 0; i < sectorTemp.size(); i++)
		{
			string sector = Sectors[sectorTemp[i]];
			int StockPick = (int)(RANDOM_NUM * SectorMap[sector].size());
			if (StockPick == SectorMap[sector].size())
				StockPick -= 1;
			string Stock = SectorMap[sector][StockPick];
			for (int n = 0; n < FundObjList.size(); n++)
			{
				if (FundObjList[n].GetSymbol() == Stock)
				{
					PureList[Stock] = n;
					break;
				}
			}

		}
	}
	
	void GetWeights(vector<Fundamental> &FundObjList)
	{
		Vector temp;
		float sum = 0.0f;
		for (auto it = PureList.begin(); it != PureList.end(); it++)
			sum += FundObjList[it->second].GetMCap();

		for (auto it = PureList.begin(); it != PureList.end(); it++)
		{
			float weight = FundObjList[it->second].GetMCap() / sum;
			weights[it->first] = weight;
		}
	}

	void CalPortfolioR(vector<Stock> &StockObjList) 
	{
		vector<string> Date = StockObjList[0].GetDate();
		
		for (int i = 0; i < Date.size(); i++)
		{
			Vector DailyReturn;
			for (auto it = PureList.begin(); it != PureList.end(); it++)
				DailyReturn.push_back(StockObjList[it->second].FindReturn(Date[i]));
			
			Vector weightstemp;
			for (auto it = weights.begin(); it != weights.end(); ++it)
				weightstemp.push_back(it->second);
			float result = weightstemp ^ DailyReturn;
			Portfolio_Return[Date[i]] = result;
		}	
	}

	void CalPortAvgR()
	{
		float sum = 0.0f;
		for (auto it = Portfolio_Return.begin(); it != Portfolio_Return.end(); it++)
			sum += it->second;

		Portfolio_Avg_Return = sum;
	}

	void CalPortDeviation()
	{
		float temp = Portfolio_Avg_Return / 2884;
		float sum = 0.0f;
		for (auto it = Portfolio_Return.begin(); it != Portfolio_Return.end(); it++)
		{
			sum += (it->second - temp) * (it->second - temp);
		}
		Portfolio_Deviation = sqrt(sum);
	}

	void CalSharpRatio(map<string, float> RfReturn) 
	{
		map<string, float> temp;
		for (auto it = Portfolio_Return.begin(); it != Portfolio_Return.end(); it++)
		{
			float ExcessReturn = it->second - RfReturn[it->first];
			temp[it->first] = ExcessReturn/Portfolio_Deviation;
		}
		float sum = 0.0f;
		for (auto it = temp.begin(); it != temp.end(); it++)
		{
			sum += it->second;
		}
		SharpRatio = sum;
	}

	void CalAlpha(vector<Stock> &StockObjList, float AvgSPY,float AvgRiskFree)
	{
		Vector BetaTemp;
		for (auto it = PureList.begin(); it != PureList.end(); it++)
		{
			float StockBeta = (StockObjList[it->second].CalAvgReturn() - AvgRiskFree) / (AvgSPY - AvgRiskFree);
			BetaTemp.push_back(StockBeta);
		}
		
		Vector weightstemp;
		for (map<string, float>::iterator it = weights.begin(); it != weights.end(); it++)
			weightstemp.push_back(it->second);

		float PortfolioBeta = weightstemp ^ BetaTemp;
		float CAPM = AvgRiskFree + PortfolioBeta * (AvgSPY - AvgRiskFree);
		JAlpha = Portfolio_Avg_Return - CAPM;
	}

	void CalTreynor(map<string, float> RfReturn)
	{
		map<string, float>temp;
		for (auto it = Portfolio_Return.begin(); it != Portfolio_Return.end(); it++)
		{
			float ExcessReturn = it->second - RfReturn[it->first];
			temp[it->first] = ExcessReturn / BETA;
		}
		float sum = 0.0f;
		for (auto it = temp.begin(); it != temp.end(); it++)
		{
			sum += it->second;
		}
		TreynorRatio = sum;
	}

	void GetDiversIndex()
	{
		float Wsqrt = 0.0f;
		for (auto it = weights.begin(); it != weights.end(); it++)
			Wsqrt += it->second * it->second;
		DiversIndex = 1 - Wsqrt;
	}

	void PopulateFundData(vector<Fundamental> &FundObjList)
	{
		Vector PEtemp, DIVtemp, BETAtemp, CAPtemp, PEGtemp;
		for (auto it = PureList.begin(); it != PureList.end(); it++)
		{
			PEtemp.push_back(FundObjList[it->second].GetPE());
			DIVtemp.push_back(FundObjList[it->second].GetDivY());
			BETAtemp.push_back(FundObjList[it->second].GetBeta());
			CAPtemp.push_back(FundObjList[it->second].GetMCap());
			PEGtemp.push_back(FundObjList[it->second].GetPEG());
		}
		
		Vector weightstemp;
		for (map<string, float>::iterator it = weights.begin(); it != weights.end(); it++)
			weightstemp.push_back(it->second);

		PE = PEtemp ^ weightstemp;
		DIVYIELD = DIVtemp ^ weightstemp;
		BETA = BETAtemp ^ weightstemp;
		MarketCap = CAPtemp ^ weightstemp;
		PEGRatio = PEGtemp ^ weightstemp;
	}

	void FundamentalAnalysis(vector<Fundamental> &FundObjList, vector<Stock> &StockObjList)
	{
		vector<float> H52temp, L52temp, LastDateTrade, WeekP, Targettemp, TargetP, MA50temp, MA200temp, MAPerform;
		for (auto it = PureList.begin(); it != PureList.end(); it++)
		{
			H52temp.push_back(FundObjList[it->second].GetHigh52());
			L52temp.push_back(FundObjList[it->second].GetLow52());
			LastDateTrade.push_back(StockObjList[it->second].GetLastDatePrice());
			Targettemp.push_back(FundObjList[it->second].GetTarget());
			MA50temp.push_back(FundObjList[it->second].GetMA50());
			MA200temp.push_back(FundObjList[it->second].GetMA200());
		}
		for (int i = 0; i < LastDateTrade.size(); i++)
		{
			if ((H52temp[i] - LastDateTrade[i]) < (L52temp[i] - LastDateTrade[i]))
				WeekP.push_back(0);
			else
				WeekP.push_back(5);
		}

		for (int i = 0; i < LastDateTrade.size(); i++)
		{
			if (LastDateTrade[i] < Targettemp[i])
				TargetP.push_back(5);
			else
				TargetP.push_back(0);
		}

		for (int i = 0; i < MA50temp.size(); i++)
		{
			if (MA50temp[i] > MA200temp[i])
				MAPerform.push_back(5);
			else
				MAPerform.push_back(0);
		}

		Vector weightstemp;
		for (map<string, float>::iterator it = weights.begin(); it != weights.end(); it++)
			weightstemp.push_back(it->second);

		FundAnalysis = (weightstemp ^ WeekP) + (weightstemp ^ TargetP) + (weightstemp ^ MAPerform);
	}

	void CalFitness()
	{
		if (SharpRatio >= 2)
			fitness += 20;
		else if (SharpRatio >= 1.8 && SharpRatio < 2)
			fitness += 18;
		else if (SharpRatio >= 1.6 && SharpRatio < 1.8)
			fitness += 16;
		else if (SharpRatio >= 1.4 && SharpRatio < 1.6)
			fitness += 14;
		else if (SharpRatio >= 1.2 && SharpRatio < 1.4)
			fitness += 12; 
		else if (SharpRatio >= 1 && SharpRatio < 1.2)
			fitness += 10;
		else if (SharpRatio >= 0.8 && SharpRatio < 1)
			fitness += 8;
		else if (SharpRatio >= 0.6 && SharpRatio < 0.8)
			fitness += 6;
		else if (SharpRatio >= 0.4 && SharpRatio < 0.6)
			fitness += 4;
		else if (SharpRatio >= 0.2 && SharpRatio < 0.4)
			fitness += 2;
		else if ( SharpRatio < 0.2)
			fitness += 0;

		if (JAlpha >= 2)
			fitness += 15;
		else if (JAlpha >= 1.5 && JAlpha < 2)
			fitness += 12;
		else if (JAlpha >= 1 && JAlpha < 1.5)
			fitness += 9;
		else if (JAlpha >= 0.5 && JAlpha < 1)
			fitness += 6;
		else if (JAlpha < 0.5)
			fitness += 3;

		if (TreynorRatio >= 1)
			fitness += 20;
		else if (TreynorRatio >= 0.9 && TreynorRatio < 1)
			fitness += 18;
		else if (TreynorRatio >= 0.8 && TreynorRatio < 0.9)
			fitness += 16;
		else if (TreynorRatio >= 0.7 && TreynorRatio < 0.8)
			fitness += 14;
		else if (TreynorRatio >= 0.6 && TreynorRatio < 0.7)
			fitness += 12;
		else if (TreynorRatio >= 0.5 && TreynorRatio < 0.6)
			fitness += 10;
		else if (TreynorRatio >= 0.4 && TreynorRatio < 0.5)
			fitness += 8;
		else if (TreynorRatio >= 0.3 && TreynorRatio < 0.4)
			fitness += 6;
		else if (TreynorRatio >= 0.2 && TreynorRatio < 0.3)
			fitness += 4;
		else if (TreynorRatio >= 0.1 && TreynorRatio < 0.2)
			fitness += 2;
		else if (TreynorRatio < 0.1)
			fitness += 0;

		if (DiversIndex >= 0.9 && DiversIndex < 1)
			fitness += 20;
		else if (DiversIndex >= 0.8 && DiversIndex < 0.9)
			fitness += 18;
		else if (DiversIndex >= 0.7 && DiversIndex < 0.8)
			fitness += 16;
		else if (DiversIndex >= 0.6 && DiversIndex < 0.7)
			fitness += 14;
		else if (DiversIndex >= 0.5 && DiversIndex < 0.6)
			fitness += 12;
		else if (DiversIndex >= 0.4 && DiversIndex < 0.5)
			fitness += 10;
		else if (DiversIndex >= 0.3 && DiversIndex < 0.4)
			fitness += 8;
		else if (DiversIndex >= 0.2 && DiversIndex < 0.3)
			fitness += 6;
		else if (DiversIndex >= 0.1 && DiversIndex < 0.2)
			fitness += 4;
		else if ( DiversIndex < 0.1)
			fitness += 2;

		if (PE >= 40)
			fitness += 2;
		else if (PE >= 25 && PE < 40)
			fitness += 1.6;
		else if (PE >= 17 && PE < 25)
			fitness += 1.2;
		else if (PE >= 11 && PE < 17)
			fitness += 0.8;
		else if (PE < 11)
			fitness += 0.4;

		if (DIVYIELD >= 0.03)
			fitness += 2;
		else if (DIVYIELD >= 0.023 && DIVYIELD < 0.03)
			fitness += 1.6;
		else if (DIVYIELD >= 0.015 && DIVYIELD < 0.023)
			fitness += 1.2;
		else if (DIVYIELD >= 0.005 && DIVYIELD < 0.015)
			fitness += 0.8;
		else if (DIVYIELD < 0.005)
			fitness += 0.4;
		
		if (BETA >= 1.48)
			fitness += 2;
		else if (BETA >= 1.19 && BETA < 1.48)
			fitness += 1.6;
		else if (BETA >= 0.95 && BETA < 1.19)
			fitness += 1.2;
		else if (BETA >= 0.584 && BETA < 0.95)
			fitness += 0.8;
		else if (BETA < 0.584)
			fitness += 0.4;

		if (MarketCap >= 55900)
			fitness += 2;
		else if (MarketCap >= 29200 && MarketCap < 55900)
			fitness += 1.6;
		else if (MarketCap >= 18100 && MarketCap < 29200)
			fitness += 1.2;
		else if (MarketCap >= 11425 && MarketCap < 18100)
			fitness += 0.8;
		else if (MarketCap < 11425)
			fitness += 0.4;

		if (PEGRatio >= 3)
			fitness += 2;
		else if (PEGRatio >= 2.1 && PEGRatio < 3)
			fitness += 1.6;
		else if (PEGRatio >= 1.62 && PEGRatio < 2.1)
			fitness += 1.2;
		else if (PEGRatio >= 1.1 && PEGRatio < 1.62)
			fitness += 0.8;
		else if (PEGRatio < 1.1)
			fitness += 0.4;

		fitness += FundAnalysis;
	}

	void BackTest(vector<Stock> &StockObjList_Back, map<string, float> &SPYPrice_Back, vector<string> &BeatTemp, vector<string> &MissTemp, vector<string> &Monthlybeat)
	{
		Vector weightstemp;
		for (map<string, float>::iterator it = weights.begin(); it != weights.end(); it++)
			weightstemp.push_back(it->second);

		vector<string> StartDate = { "2019-01-02", "2019-01-07", "2019-01-14", "2019-01-22", "2019-01-28", "2019-02-04", "2019-02-11", "2019-02-19", "2019-02-25", "2019-03-04", "2019-03-11", "2019-03-18", "2019-03-25", "2019-04-01", "2019-04-08", "2019-04-15", "2019-04-22", "2019-04-29", "2019-05-06", "2019-05-13", "2019-05-20", "2019-05-28", "2019-06-03", "2019-06-10", "2019-06-17", "2019-06-24" };
		vector<string> EndDate = {"2019-01-04", "2019-01-11", "2019-01-18", "2019-01-25", "2019-02-01","2019-02-08", "2019-02-15",  "2019-02-22", "2019-03-01", "2019-03-08", "2019-03-15", "2019-03-22", "2019-03-29", "2019-04-05", "2019-04-12", "2019-04-18", "2019-04-26", "2019-05-03", "2019-05-10", "2019-05-17", "2019-05-24", "2019-05-31", "2019-06-07", "2019-06-14", "2019-06-21", "2019-06-28" };
		vector<float> PortfolioPerform, SPYPerform;
		map<string, float> Monthlytemp;
		
		for (int i = 0; i < StartDate.size(); i++)
		{
			vector<float> StartTemp, EndTemp;
			for (auto it = PureList.begin(); it != PureList.end(); it++)
			{
				map<string, float> temp = StockObjList_Back[it->second].GetTradingPrice();
				StartTemp.push_back(temp[StartDate[i]]);
				EndTemp.push_back(temp[EndDate[i]]);
			}
			PortfolioPerform.push_back(((EndTemp ^ weightstemp) - (StartTemp ^ weightstemp)) / (StartTemp ^ weightstemp));
			SPYPerform.push_back((SPYPrice_Back[EndDate[i]] - SPYPrice_Back[StartDate[i]]) / SPYPrice_Back[StartDate[i]]);
			Monthlytemp[StartDate[i]] = StartTemp ^ weightstemp;
			Monthlytemp[EndDate[i]] = EndTemp ^ weightstemp;
		}

		cout << "****Weekly performance***" << endl;
		cout << right << setw(20) << "Week";
		cout << right << setw(20) << "Portfolio Return";
		cout << right << setw(20) << "SPY Return";
		cout << right << setw(20) << "Result";
		cout << "\n" << endl;
		for (int i = 0; i < PortfolioPerform.size(); i++)
		{
			string weekResult = "NULL";
			if (PortfolioPerform[i] > SPYPerform[i])
				weekResult = "Beat";
			else
				weekResult = "Miss";

			cout << right << setw(20) << StartDate[i];
			cout << right << setw(20) << PortfolioPerform[i];
			cout << right << setw(20) << SPYPerform[i];
			cout << right << setw(20) << weekResult;
			cout << "\n" << endl;
		}

		vector<string> JanStart = { "2019-01-02", "2019-01-07", "2019-01-14", "2019-01-22", "2019-01-28" };
		vector<string> JanEnd = { "2019-01-04", "2019-01-11", "2019-01-18", "2019-01-25", "2019-02-01" };
		int Jan = 0;
		for (int i = 0; i < JanStart.size(); i++)
		{
			if (PortfolioPerform[i] >= SPYPerform[i])
			{
				BeatTemp.push_back(JanStart[i]);
				Jan++;
			}
			else
			{
				MissTemp.push_back(JanStart[i]);
			}
		}
		if (Jan != 5)
		{
			float JanReturn = (Monthlytemp["2019-02-01"] - Monthlytemp["2019-01-02"])/Monthlytemp["2019-01-02"];
			float SPYtemp = (SPYPrice_Back["2019-02-01"] - SPYPrice_Back["2019-01-02"]) / SPYPrice_Back["2019-01-02"];
			cout << "****Jan Monthly Return " << "Portfolio Return: " << JanReturn << " " << "SPY Return: " << SPYtemp << endl;
			if (JanReturn >= SPYtemp)
			{
				cout << "Didn't beat every week but beat on average in Jan" << endl << endl;
				Monthlybeat.push_back("Jan");
			}
		}
		else if (Jan == 5)
		{
			cout << "**************Continuely beat in Jan*****************" << endl;
		}

		vector<string> FebStart = { "2019-02-04", "2019-02-11", "2019-02-19", "2019-02-25" };
		vector<string> FebEnd = { "2019-02-08", "2019-02-15",  "2019-02-22", "2019-03-01" };
		int Feb = 0;
		for (int i = 0; i < FebStart.size(); i++)
		{
			if (PortfolioPerform[i+5] >= SPYPerform[i+5])
			{
				BeatTemp.push_back(FebStart[i]);
				Feb++;
			}
			else
			{
				MissTemp.push_back(FebStart[i]);
			}
		}
		if (Feb != 4)
		{
			float FebReturn = (Monthlytemp["2019-03-01"] - Monthlytemp["2019-02-04"]) / Monthlytemp["2019-02-04"];
			float SPYtemp = (SPYPrice_Back["2019-03-01"] - SPYPrice_Back["2019-02-04"]) / SPYPrice_Back["2019-02-04"];
			cout << "****Feb Monthly Return " << "Portfolio Return: " << FebReturn << " " << "SPY Return: " << SPYtemp << endl;
			if (FebReturn >= SPYtemp)
			{
				cout << "Didn't beat every week but beat on average in Feb" << endl << endl;
				Monthlybeat.push_back("Feb");
			}
		}

		else if (Feb == 4)
		{
			cout << "**************Continuely beat in Feb*****************" << endl;
		}

		vector<string> MarStart = { "2019-03-04", "2019-03-11", "2019-03-18", "2019-03-25" };
		vector<string> MarEnd = { "2019-03-08", "2019-03-15", "2019-03-22", "2019-03-29" };
		int Mar = 0;
		for (int i = 0; i < MarStart.size(); i++)
		{
			if (PortfolioPerform[i + 9] >= SPYPerform[i + 9])
			{
				BeatTemp.push_back(MarStart[i]);
				Mar++;
			}
			else
			{
				MissTemp.push_back(MarStart[i]);
			}
		}
		if (Mar != 4)
		{
			float MarReturn = (Monthlytemp["2019-03-29"] - Monthlytemp["2019-03-04"]) / Monthlytemp["2019-03-04"];
			float SPYtemp = (SPYPrice_Back["2019-03-29"] - SPYPrice_Back["2019-03-04"]) / SPYPrice_Back["2019-03-04"];
			cout << "****Mar Monthly Return " << "Portfolio Return: " << MarReturn << " " << "SPY Return: " << SPYtemp << endl;
			if (MarReturn >= SPYtemp)
			{
				cout << "Didn't beat every week but beat on average in Mar" << endl << endl;
				Monthlybeat.push_back("Mar");
			}
		}
		else if (Mar == 4)
		{
			cout << "**************Continuely beat in Mar*****************" << endl;
		}
		
		vector<string> AprStart = { "2019-04-01", "2019-04-08", "2019-04-15", "2019-04-22", "2019-04-29" };
		vector<string> AprEnd = { "2019-04-05", "2019-04-12", "2019-04-18", "2019-04-26", "2019-05-03" };
		int Apr = 0;
		for (int i = 0; i < AprStart.size(); i++)
		{
			if (PortfolioPerform[i + 13] >= SPYPerform[i + 13])
			{
				BeatTemp.push_back(AprStart[i]);
				Apr++;
			}
			else
			{
				MissTemp.push_back(AprStart[i]);
			}
		}
		if (Apr != 5)
		{
			float AprReturn = (Monthlytemp["2019-05-03"] - Monthlytemp["2019-04-01"]) / Monthlytemp["2019-04-01"];
			float SPYtemp = (SPYPrice_Back["2019-05-03"] - SPYPrice_Back["2019-04-01"]) / SPYPrice_Back["2019-04-01"];
			cout << "****Apr Monthly Return " << "Portfolio Return: " << AprReturn << " " << "SPY Return: " << SPYtemp << endl;
			if (AprReturn >= SPYtemp)
			{
				cout << "Didn't beat every week but beat on average in Apr" << endl << endl;
				Monthlybeat.push_back("Apr");
			}
		}

		else if (Apr == 4)
		{
			cout << "**************Continuely beat in Apr*****************" << endl;
		}

		vector<string> MayStart = { "2019-05-06", "2019-05-13", "2019-05-20", "2019-05-28" };
		vector<string> MayEnd = { "2019-05-10", "2019-05-17", "2019-05-24", "2019-05-31" };
		int May = 0;
		for (int i = 0; i < MayStart.size(); i++)
		{
			if (PortfolioPerform[i + 18] >= SPYPerform[i + 18])
			{
				BeatTemp.push_back(MayStart[i]);
				May++;
			}
			else
			{
				MissTemp.push_back(MayStart[i]);
			}
		}
		if (May != 4)
		{
			float MayReturn = (Monthlytemp["2019-05-31"] - Monthlytemp["2019-05-06"]) / Monthlytemp["2019-05-06"];
			float SPYtemp = (SPYPrice_Back["2019-05-31"] - SPYPrice_Back["2019-05-06"]) / SPYPrice_Back["2019-05-06"];
			cout << "****May Monthly Return " << "Portfolio Return: " << MayReturn << " " << "SPY Return: " << SPYtemp << endl;
			if (MayReturn >= SPYtemp)
			{
				cout << "Didn't beat every week but beat on average in May" << endl << endl;
				Monthlybeat.push_back("May");
			}
		}
		else if (May == 4)
		{
			cout << "**************Continuely beat in May*****************" << endl;
		}

		vector<string> JunStart = { "2019-06-03", "2019-06-10", "2019-06-17", "2019-06-24" };
		vector<string> JunEnd = { "2019-06-07", "2019-06-14", "2019-06-21", "2019-06-28" };
		int Jun = 0;
		for (int i = 0; i < JunStart.size(); i++)
		{
			if (PortfolioPerform[i + 22] >= SPYPerform[i + 22])
			{
				BeatTemp.push_back(JunStart[i]);
				Jun++;
			}
			else
			{
				MissTemp.push_back(JunStart[i]);
			}
		}
		if (Jun != 4)
		{
			float JunReturn = (Monthlytemp["2019-06-28"] - Monthlytemp["2019-06-03"]) / Monthlytemp["2019-06-03"];
			float SPYtemp = (SPYPrice_Back["2019-06-28"] - SPYPrice_Back["2019-06-03"]) / SPYPrice_Back["2019-06-03"];
			cout << "****Jun Monthly Return " << "Portfolio Return: " << JunReturn << " " << "SPY Return: " << SPYtemp << endl;
			if (JunReturn >= SPYtemp)
			{
				cout << "Didn't beat every week but beat on average in Jun" << endl << endl;
				Monthlybeat.push_back("Jun");
			}
		}

		else if (Jun== 4)
		{
			cout << "**************Continuely beat in Jun*****************" << endl;
		}
	}

	string ProbationTest(vector<Stock> &StockObjList_Probation, map<string, float> &SPYPrice_Prob, vector<string> &Probation_Beat, vector<string> &Probation_Miss)
	{
		Vector weightstemp;
		for (map<string, float>::iterator it = weights.begin(); it != weights.end(); it++)
			weightstemp.push_back(it->second);
		
		vector<string> StartDate = {"2019-07-01", "2019-07-08", "2019-07-15", "2019-07-22", "2019-07-29" };
		vector<string> EndDate = {"2019-07-05", "2019-07-12", "2019-07-19", "2019-07-26", "2019-07-31" };
		vector<float> PortfolioPerform, SPYPerform;
		map<string, float> Monthlytemp;

		for (int i = 0; i < StartDate.size(); i++)
		{
			vector<float> StartTemp, EndTemp;
			for (auto it = PureList.begin(); it != PureList.end(); it++)
			{
				map<string, float> temp = StockObjList_Probation[it->second].GetTradingPrice();
				StartTemp.push_back(temp[StartDate[i]]);
				EndTemp.push_back(temp[EndDate[i]]);
			}
			PortfolioPerform.push_back(((EndTemp ^ weightstemp) - (StartTemp ^ weightstemp)) / (StartTemp ^ weightstemp));
			SPYPerform.push_back((SPYPrice_Prob[EndDate[i]] - SPYPrice_Prob[StartDate[i]]) / SPYPrice_Prob[StartDate[i]]);
			Monthlytemp[StartDate[i]] = StartTemp ^ weightstemp;
			Monthlytemp[EndDate[i]] = EndTemp ^ weightstemp;
		}

		cout << "****Weekly performance***" << endl;
		cout << right << setw(20) << "Week";
		cout << right << setw(20) << "Portfolio Return";
		cout << right << setw(20) << "SPY Return";
		cout << right << setw(20) << "Result";
		cout << "\n" << endl;
		for (int i = 0; i < PortfolioPerform.size(); i++)
		{
			string weekResult = "NULL";
			if (PortfolioPerform[i] > SPYPerform[i])
				weekResult = "Beat";
			else
				weekResult = "Miss";

			cout << right << setw(20) << StartDate[i];
			cout << right << setw(20) << PortfolioPerform[i];
			cout << right << setw(20) << SPYPerform[i];
			cout << right << setw(20) << weekResult;
			cout << "\n" << endl;
		}

		int July = 0;
		for (int i = 0; i < SPYPerform.size(); i++)
		{
			if (PortfolioPerform[i] >= SPYPerform[i])
			{
				Probation_Beat.push_back(StartDate[i]);
				July++;
			}
			else
			{
				Probation_Miss.push_back(StartDate[i]);
			}
		}

		string Target = "No";
		if (July != 5)
		{
			float JulyReturn = (Monthlytemp["2019-07-31"] - Monthlytemp["2019-07-01"]) / Monthlytemp["2019-07-01"];
			float SPYtemp = (SPYPrice_Prob["2019-07-31"] - SPYPrice_Prob["2019-07-01"]) / SPYPrice_Prob["2019-07-01"];
			cout << "****July Monthly Return " << "Portfolio Return: " << JulyReturn << " " << "SPY Return: " << SPYtemp << endl;
			if (JulyReturn >= SPYtemp)
			{
				cout << "Didn't beat every week but beat on average in July" << endl << endl;
				Target = "Yes";
			}
		}

		else if (July == 5)
		{
			cout << "**************Continuely beat in July*****************" << endl;
		}

		return Target;
	}
};

void Mutate(map<string, int> &offspring1 ,vector<string> &StockList, vector<Fundamental> &FundObjList, map<string, vector<string>> &SectorMap)
{
	vector<int> StockTemp;
	for (auto it = offspring1.begin(); it != offspring1.end(); it++)
	{
		StockTemp.push_back(it->second);
	}

	map<string, int> NewMap;

	for (int i = 0; i < StockTemp.size(); i++)
	{
		if (RANDOM_NUM < MUTATION_RATE)
		{
			bool bMutate = false;
			while (!bMutate)
			{
				string SectorNeedMutate = FundObjList[StockTemp[i]].GetSector();
				int randomNum = (int)(RANDOM_NUM * SectorMap[SectorNeedMutate].size());
				if (randomNum == SectorMap[SectorNeedMutate].size())
					randomNum -= 1;
				string MutateStock = SectorMap[SectorNeedMutate][randomNum];
				if (MutateStock != StockList[StockTemp[i]])
				{
					auto itr = find(StockList.begin(), StockList.end(), MutateStock);
					StockTemp[i] = distance(StockList.begin(), itr);

					bMutate = true;
					break;
				}
			}
		}
	}
	for (int i = 0; i < StockTemp.size(); i++)
	{
		NewMap[StockList[StockTemp[i]]] = StockTemp[i];
	}
	offspring1 = NewMap;
}

void Crossover(map<string, int> &offspring1, map<string, int> &offspring2, vector<string> &StockList)
{
	if (RANDOM_NUM < CROSSOVER_RATE)
	{
		int crossover = (int)(RANDOM_NUM * 10);
		vector<int> temp1;
		vector<int> temp2;
		map<string, int> NewMap1, NewMap2;

		for (auto it = offspring1.begin(); it != offspring1.end(); it++)
			temp1.push_back(it->second);
		for (auto it = offspring2.begin(); it != offspring2.end(); it++)
			temp2.push_back(it->second);

		vector<int> t1, t2;
		for (int i = 0; i < crossover; i++)
		{
			t1.push_back(temp1[i]);
			t2.push_back(temp2[i]);
		}

		for (int i = crossover; i < temp1.size(); i++)
		{
			t1.push_back(temp2[i]);
			t2.push_back(temp1[i]);
		}
		
		for (int i = 0; i < t1.size(); i++)
			NewMap1[StockList[t1[i]]] = t1[i];
		offspring1 = NewMap1;

		for (int i = 0; i < t2.size(); i++)
			NewMap2[StockList[t2[i]]] = t2[i];
		offspring2 = NewMap2;
	}
}

bool operator <( Portfolio& chromo1, Portfolio& chromo2) {
	if (chromo1.GetFitness() < chromo2.GetFitness()) {
		return true;
	}
	return false;
}

bool operator >(Portfolio& chromo1, Portfolio& chromo2) {
	if (chromo1.GetFitness() > chromo2.GetFitness()) {
		return true;
	}
	return false;
}

void GetCategStocks(map<string, vector<string>> &sectorMap, vector<Fundamental> &FundObjList)
{
	vector<string> Industrials, Healthcare, Technology, Energy, Consumer_Cyclical, Utilities, Financial_Services, Basic_Materials, Real_Estate, Consumer_Defensive, Communication_Services, Other;

	for (int i = 0; i < FundObjList.size(); i++)
	{
		string sctor = FundObjList[i].GetSector();
		string name = FundObjList[i].GetSymbol();

		if (sctor == "Industrials")
			Industrials.push_back(name);
		else if (sctor == "Healthcare")
			Healthcare.push_back(name);
		else if (sctor == "Technology")
			Technology.push_back(name);
		else if (sctor == "Energy")
			Energy.push_back(name);
		else if (sctor == "Consumer Cyclical")
			Consumer_Cyclical.push_back(name);
		else if (sctor == "Consumer Defensive")
			Consumer_Defensive.push_back(name);
		else if (sctor == "Financial Services")
			Financial_Services.push_back(name);
		else if (sctor == "Utilities")
			Utilities.push_back(name);
		else if (sctor == "Communication Services")
			Communication_Services.push_back(name);
		else if (sctor == "Real Estate")
			Real_Estate.push_back(name);
		else if (sctor == "Basic Materials")
			Basic_Materials.push_back(name);
		else if (sctor == "Other")
			Other.push_back(name);
	}
	sectorMap["Industrials"] = Industrials;
	sectorMap["Healthcare"] = Healthcare;
	sectorMap["Technology"] = Technology;
	sectorMap["Energy"] = Energy;
	sectorMap["Consumer Cyclical"] = Consumer_Cyclical;
	sectorMap["Consumer Defensive"] = Consumer_Defensive;
	sectorMap["Financial Services"] = Financial_Services;
	sectorMap["Utilities"] = Utilities;
	sectorMap["Communication Services"] = Communication_Services;
	sectorMap["Real Estate"] = Real_Estate;
	sectorMap["Basic Materials"] = Basic_Materials;
	sectorMap["Other"] = Other;
}


#endif
