#pragma once
#ifndef UtilityFunctions_h
#define UtilityFunctions_h

using namespace std;
#include <vector>
#include <map>
#include <algorithm>
#include <iostream>
#include <cmath>


vector<string> AdditionList = { "MSCI", "ABMD", "EVRG", "TWTR", "HFC", "BR", "FLT", "CPRT", "ANET", "WCG", "ROL", "FTNT", "KEYS", "JKHY", "FANG", "MXIM", "LW", "CE", "FRC", "TFX", "ATO", "WAB", "DOW", "CTVA", "BMS", "MKTX" };
vector<string> DeletionList = { "CSRA", "WYN", "NAVI", "MON", "AYI", "RRC", "TWX", "DPS", "GGP", "XL", "ANDV", "EVHC", "CA", "EQT", "SRCL", "AET", "COL", "ESRX", "SCG", "PCG", "NFX", "GT", "BHF", "FLR", "MAT", "LLL" };


typedef vector<float> Vector;
typedef vector<Vector> Matrix;



//-------------------------Vector Operators-----------------------------------

Vector operator*(const Matrix& C, const Vector& V)
{
	unsigned long d = C.size();
	Vector W(d);
	for (int j = 0; j < d; j++)
	{
		W[j] = 0.0;
		for (int l = 0; l < d; l++) W[j] = W[j] + C[j][l] * V[l];
	}
	return W;
}

Vector operator+(const Vector& V, const Vector& W)
{
	unsigned long d = V.size();
	Vector U(d);
	for (int j = 0; j < d; j++) U[j] = V[j] + W[j];
	return U;
}

Vector operator+(const float& a, const Vector& V)
{
	unsigned long d = V.size();
	Vector U(d);
	for (int j = 0; j < d; j++) U[j] = a + V[j];
	return U;
}

Vector operator*(const float& a, const Vector& V)
{
	unsigned long d = V.size();
	Vector U(d);
	for (int j = 0; j < d; j++) U[j] = a * V[j];
	return U;
}

Vector operator/(const Vector& V, const float& a)
{
	unsigned long d = V.size();
	Vector U(d);
	for (int j = 0; j < d; j++) U[j] = V[j] / a;
	return U;
}

Vector operator*(const Vector& V, const Vector& W)
{
	unsigned long d = V.size();
	Vector U(d);
	for (int j = 0; j < d; j++) U[j] = V[j] * W[j];
	return U;
}

Vector operator* (const Vector& V, map<string, Vector> & M)
{
	map<string, Vector> temp;
	for (map<string, Vector>::iterator it = M.begin(); it != M.end(); it++)
	{
		for (int i = 0; i < V.size(); i++)
			temp[it->first] = V[i] * it->second;
	}

	unsigned long l = V.size();
	Vector sum(l);
	for (int i = 0; i < l; i++)
	{
		for (map<string, Vector>::iterator itr = temp.begin(); itr != temp.end(); itr++)
			sum[i] += itr->second[i];
	}
	return sum;
}

Vector exp(const Vector& V)
{
	unsigned long d = V.size();
	Vector U(d);
	for (int j = 0; j < d; j++) U[j] = exp(V[j]);
	return U;
}

float operator^(const Vector& V, const Vector& W)
{
	float sum = 0.0;
	unsigned long d = V.size();
	for (int j = 0; j < d; j++) sum = sum + V[j] * W[j];
	return sum;
}

ostream & operator<<(ostream & out, Vector & V)
{
	for (Vector::iterator itr = V.begin(); itr != V.end(); itr++)
		out << *itr << "   ";
	out << endl;
	return out;
}

ostream & operator<<(ostream & out, Matrix & W)
{
	for (Matrix::iterator itr = W.begin(); itr != W.end(); itr++)
		out << *itr;
	out << endl;
	return out;
}

//-------------------------Avg and Variance of Vector-----------------------------------
float AvgofVector(Vector V)
{
	float sum = 0;
	for (int i = 0; i < V.size(); i++)
	{
		sum += V[i];
	}
	float avg = sum / (float)V.size();
	return avg;
}

float VarofVector(Vector V)
{
	float avg = AvgofVector(V);
	double Sqrsum = 0;
	for (int i = 0; i < V.size(); i++)
	{
		Sqrsum += pow((double)(V[i] - avg), 2.0);
	}
	float result = (float)(Sqrsum / V.size());
	return result;
}

#endif