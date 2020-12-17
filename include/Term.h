#pragma once
#include <iostream>
#include "stack.h"
#include <vector>
using namespace std;

enum TypeTerm { NUMBER, OPERATOR, OPBRACKET, CLBRACKET };

class Term //лексемы. Абстрактный класс, нужен для создания массива из указателей на конкретные лексемы
{
public:
	virtual int GetKey() = 0;
	virtual void Print() = 0;
};

class Number : public Term
{
private:
	double value;
public:
	Number(double tmp) { value = tmp; }
	
	int GetKey() { return TypeTerm::NUMBER; }
	void Print() { cout << value; }
	
	double GetValue() { return value; }

};

class Operator : public Term
{
private:
	char symbol;
public:
	Operator(char tmp) { symbol = tmp; }
	
	int GetKey() { return TypeTerm::OPERATOR;  }
	void Print() { cout << symbol; }
		
	int Priority()	//Приоритет операций. Нужен для перевода в постфиксную запись
	{
		if (symbol == '+' || symbol == '-') return 0;
		if (symbol == '*' || symbol == '/') return 1;
		else throw "Unknown operator";
	}
	char GetSymbol() { return symbol; }
};

class Bracket : public Term
{
protected:
	char symbol;
public:
	Bracket(char tmp) { symbol = tmp; }
	
	
	int GetKey()
	{
		if (symbol == '(') return TypeTerm::OPBRACKET;
		else return TypeTerm::CLBRACKET;
	}
	void Print() { cout << symbol; }
};
/*
vector<Term*> v{};
v.size();*/