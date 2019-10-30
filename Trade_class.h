#pragma once
#ifndef Trade_class_h
#define Trade_class_h

#include <iostream>

using namespace std;

class Trade
{
private:
	string date;
	float open;
	float high;
	float low;
	float close;
	float adjusted_close;
	float volume;
public:
	Trade(string date_, float open_, float high_, float low_, float close_, float adjusted_close_, float volume_) :
		date(date_), open(open_), high(high_), low(low_), close(close_), adjusted_close(adjusted_close_), volume(volume_)
	{}
	~Trade() {}
	float GetAdjClose() { return adjusted_close; }
	friend ostream & operator << (ostream & out, const Trade & t)
	{
		out << "Date: " << t.date << " Open: " << t.open << " High: " << t.high << " Low: " << t.low << " Close: " << t.close << " Adjusted_Close: " << t.adjusted_close << " Volume: " << t.volume << endl;
		return out;
	}
};
#endif

