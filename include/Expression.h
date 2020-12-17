#pragma once
#include <iostream>
#include "stack.h"
#include "Term.h"
#include <vector>
#include <string>

enum Error { ALL_RIGHT, INCORRECT_SYMBOL, INCORRECT_PLACEMENT_OF_BRACKETS, SYNTAX_ERROR, DIVISION_BY_ZERO };

class Expression
{
private:
	std::string str;	//входная строка
	std::vector<Term*> Lexemes;	//преобразование в массив лексем
	std::vector<Term*> PostfixLexemes;
	double result=0;
	int KeyError = Error::ALL_RIGHT;	//по умолчанию всё в порядке
	int IdError;	//Номер элемента где возникла ошибка, НАЧИНАЯ С ЕДИНИЦЫ! Пока только для некорректного символа

	void LexicalParsing();	//лексический разбор
	void BracketsControl();	//проверка правильности расстановки скобок
	void SyntacticControl();	//синтаксический анализ
	void TranslatorToPostfix();	//транслятор, перевод в постфиксную запись
	void Calculation();	//вычисление
public:
	Expression(std::string s) { str = s; }; //конструктор по умолчанию
	~Expression();	//деструктор

	int GetKeyError(){ return KeyError; }
	int GetIdError() { return IdError; }

	double solve();	//итоговое решение, если все предыдущие этапы были корректно пройдены
};

//деструктор 
Expression::~Expression()
{
	for (int i = 0; i < Lexemes.size(); i++)
		delete Lexemes[i];
}

//лексический разбор, преобразование строки в массив указателей на лексемы
void Expression::LexicalParsing()
{
	int DlinaSrtroki = str.size();						//утечки памяти при использовании new не происходит, т.к. в классе Expression есть деструктор, удаляющий составные элементы массива лексем
	while (str.size()>0 && str[0] != '=')	//идём до первого знака "равно" либо до конца строки
	{
		if (str[0] == '+' || str[0] == '-' || str[0] == '*' || str[0] == '/') {		//если встретили знак операции, то создаём соответствующую 
			Lexemes.push_back(new Operator(str[0]));		//лексему, и указтель на неё помещяем в наш массив лексем
			str.erase(0, 1);			//после этого из строки мы удаляем один элемент (с нулевой позицией), т.к. операция занимает ровно один символ
		}														//Приоритет операции будет автоматически определён в классе Operator
		else if (str[0] == '(' || str[0] == ')') {
			Lexemes.push_back(new Bracket(str[0]));		//всё аналогично. Тип скобки будет определён в классе Bracket
			str.erase(0, 1);
		}
		else if (str[0] >= '0' && str[0] <= '9') {		//если встретили цифру - сложнее. Нужно "накапливать" число, т.е. идти по строке до тех пор, 
			size_t LenghtTmpNumber;						//пока не встретим первое нечисловое значение. Затем "накопленное" число нужно поместить в массив лексем,
			double TmpNumber;							//а из начала строки удалить первые LenghtTmpNumber элементов(цифр), где LenghtTmpNumber - длина "накопленного" числа
			TmpNumber = std::stod(str, &LenghtTmpNumber);	//второй параметр - для запоминания, в каком месте строки закончилось число
			Lexemes.push_back(new Number(TmpNumber));
			str.erase(0, LenghtTmpNumber);
		}
		else if (str[0] == ' ')		//пропускаем пробелы
		{
			str.erase(0, 1);
		}
		else 
		{
			KeyError = Error::INCORRECT_SYMBOL;
			IdError = DlinaSrtroki - str.size()+1;			//количество элементов, которые уже переместили в массив лексем
			break;
		}
	}
}

//проверка правильности расстановки скобок
void Expression::BracketsControl()	//Как в предыдущей задаче на скобки на стеке
{
	LexicalParsing();	//по цепочке вызываем все методы, начиная с первого
	if (KeyError == Error::ALL_RIGHT)
	{
		vector<int> brc;		//будут храниться номера ячеек, где лежат открывающие скобки
		bool allright = true;
		int i = 0;
		for (i; i < Lexemes.size() && allright == true; i++)
		{
			if (Lexemes[i]->GetKey() == TypeTerm::OPBRACKET)		//то есть если встретили открывающую скобку
				brc.push_back(i);	//для выявления индекса ошибки (при продолжении реализации в будущем)

			else if (Lexemes[i]->GetKey() == TypeTerm::CLBRACKET)	//то есть если встретили закрывающую скобку
			{
				if (!brc.empty())	//если стек не пуст (иначе обращение непонятно куда) 
					brc.pop_back();
				else {
					allright = false; 
					break;
				}
			}
		}

		if (allright == false)
		{
			KeyError = Error::INCORRECT_PLACEMENT_OF_BRACKETS;
			//IdError = i+1;
		}
		else if (!brc.empty())
		{
			KeyError = Error::INCORRECT_PLACEMENT_OF_BRACKETS;
			//IdError = brc[0]+1;
		}
	}
}

//проверка правильности введённого синтаксиса
void Expression::SyntacticControl()
{
	//int KolvoSdvigov = 0;
	BracketsControl();		//продолжаем вызов по цепочке
	if (KeyError == Error::ALL_RIGHT)
	{
		bool allright = true;
		//первый и последний элементы нужно проверять отдельно. Мы точно знаем, что там не закрывающая скобка (из проверка на скобки).  
		if (Lexemes.size() > 0 && Lexemes[0]->GetKey() == TypeTerm::OPERATOR)		//Проверка, что массив не пуст! Вдруг во входной строке пришёл первым знак равно. Тогда обращаться некуда
			if (((Operator*)(Lexemes[0]))->Priority() == 1)			//Осталось убедиться, что там не операция умножения или деления (а у них приоритет 1). Если - или + - норм, унарные операции.
			{
				allright = false;
				//IdError = 1;
			}
			else Lexemes.insert(Lexemes.begin(), new Number(0));		//если у нас операция + или -, то добавляем в начало 0. Было -5+2, станет 0-5+2.
															//ДЛИНА МАССИВА ПРИ ЭТОМ УВЕЛИЧИТСЯ!
		for (int i=0; Lexemes.size()>0 && i < Lexemes.size() - 1 && allright == true; i++)
			switch (Lexemes[i]->GetKey()) {
			case(TypeTerm::NUMBER):					//если там число, то после него может быть только закрывающая скобка или операция
				if (Lexemes[i + 1]->GetKey() == TypeTerm::NUMBER || Lexemes[i + 1]->GetKey() == TypeTerm::OPBRACKET)
				{
					allright = false;
					//IdError = i + 1 - KolvoSdvigov;
				}
				break;
			case(TypeTerm::OPERATOR):
				if (Lexemes[i + 1]->GetKey() == TypeTerm::OPERATOR || Lexemes[i + 1]->GetKey() == TypeTerm::CLBRACKET)
				{
					allright = false;
					//IdError = i + 1 - KolvoSdvigov;
				}
				break;
			case(TypeTerm::OPBRACKET):
				if (Lexemes[i + 1]->GetKey() == TypeTerm::CLBRACKET)		//если закрывающая скобка - сразу ошибка
				{
					allright = false;
					//IdError = i + 1 - KolvoSdvigov;
					break;
				}
				if (Lexemes[i + 1]->GetKey() == TypeTerm::OPERATOR)		//если операция, то ошибка в случаях если операция это умножение или деление
				{
					if (((Operator*)(Lexemes[i + 1]))->Priority() == 1)
					{
						allright = false;
						//IdError = i + 1 - KolvoSdvigov;
						break;
					}
					else {
						Lexemes.insert(Lexemes.begin() + i + 1, new Number(0));	//иначе вставляем 0, как и в случае начала строки. Длина строки снова увеличится. 
						i++;	//чтобы не исследовать на следующем шаге вставленный ноль: с ним заведомо всё в порядке, впереди плюс или минус.
						//KolvoSdvigov++;
					}
				}
				break;
			case(TypeTerm::CLBRACKET):
				if (Lexemes[i + 1]->GetKey() == TypeTerm::NUMBER || Lexemes[i + 1]->GetKey() == TypeTerm::OPBRACKET)
				{
					allright = false;
					//IdError = i + 1 - KolvoSdvigov;
				}
				break;
			}


		//отдельная проверка для последнего элемента. !!Опять же, не забываем проверку длины массива лексем, ведь Lexemes может быть пустым!
		if (allright == true && Lexemes.size() > 0 && Lexemes[Lexemes.size() - 1]->GetKey() == TypeTerm::OPERATOR)	//Мы точно знаем, что там не открывающая скобка(из проверка на скобки). Осталось убедиться, что там не операция
		{	
			allright = false;
			//IdError = Lexemes.size() - KolvoSdvigov;
		}

		if (allright == false)
			KeyError = Error::SYNTAX_ERROR;
	}
}

//перевод выражения в постфиксную запись
void Expression::TranslatorToPostfix()
{
	SyntacticControl();		//продолжаем вызов по цепочке
	if (KeyError == Error::ALL_RIGHT)
	{
		Stack<Term*> tmp;	//вспомогательный стек для накапливания операций и открывающих скобок
		Term* z;
		for (int i = 0; i < Lexemes.size(); i++)
			switch (Lexemes[i]->GetKey())
			{
			case(TypeTerm::NUMBER):
				PostfixLexemes.push_back(Lexemes[i]);	//указатель на ЧИСЛО из массива лексем копируем в массив постфиксной записи
				break;
			case(TypeTerm::OPBRACKET):
				tmp.push(Lexemes[i]);
				break;
			case(TypeTerm::CLBRACKET):
			{
				bool flag = false;	//пока не встретим открывающую скобку. Это точно случится, т.к. правильность расстановки скобок была проверна ранее
				while (flag == false)
				{
					z = tmp.top();
					tmp.pop();
					if (z->GetKey() == TypeTerm::OPBRACKET)
						flag = true;
					else
						PostfixLexemes.push_back(z);	//достаём все операции, пока не встретим открывающую скобку
				}
				break;
			}
			case(TypeTerm::OPERATOR):
				while (!tmp.empty())	//пока стек не пуст
				{
					z = tmp.top();
					tmp.pop();
					if (z->GetKey() == TypeTerm::OPBRACKET)	//если вверху стека лежит открывающая скобка, то мы зря её достали, кладём её обратно
					{
						tmp.push(z);
						break;
					}
					else if (((Operator*)(z))->Priority() >= ((Operator*)(Lexemes[i]))->Priority())	//если приоритет лежащей наверху стека операции >= приоритета операции текущей операции
						PostfixLexemes.push_back(z);				//добавляем операцию в постфиксный массив лексем
					else
					{
						tmp.push(z);	//в противном случае мы зря достали операцию, кладём обратно
						break;
					}
				}
				tmp.push(Lexemes[i]);	//в любом случае, текущую лексему кладём на вершину стека
				break;
			}
		while (!tmp.empty())
		{
			PostfixLexemes.push_back(tmp.top());
			tmp.pop();
		}
	}
}

//Вычисление значения. До это проверены все пункты и произведено преобразование в постфиксную запись
void Expression::Calculation()
{
	TranslatorToPostfix();		//продолжаем вызов по цепочке
	if (KeyError == Error::ALL_RIGHT)
	{
		Stack<double> tmp;
		for (int i = 0; i < PostfixLexemes.size() && KeyError==Error::ALL_RIGHT; i++)
			switch (PostfixLexemes[i]->GetKey()) {
			case(TypeTerm::NUMBER):
				tmp.push(((Number*)(PostfixLexemes[i]))->GetValue());
				break;
			case(TypeTerm::OPERATOR):
				double operand2 = tmp.top();	//порядок операндов важен при некоммутативных операциях (деление, вычитание)
				tmp.pop();		//из стека чисел значения удаляем
				double operand1 = tmp.top();
				tmp.pop();
				switch (((Operator*)(PostfixLexemes[i]))->GetSymbol()) {
				case('+'):
					tmp.push(operand1 + operand2);	//результат записываем на вершину стека
					break;
				case('-'):
					tmp.push(operand1 - operand2);
					break;
				case('*'):
					tmp.push(operand1 * operand2);
					break;
				case('/'):
					if (operand2 != 0.0)
						tmp.push(operand1 / operand2);
					else
						KeyError = DIVISION_BY_ZERO;	//если встретили деление на ноль, сразу записываем ошибку и прекращаем просмотр
					break;
				}
			}
		if (KeyError == Error::ALL_RIGHT && !tmp.empty()) result = tmp.top();	//в результате на вершине стека (единственный элемент) будет лежать посчитанное выражение
									//нужна проверка, не пустой ли стек!
	}
}

//собираем решение воедино

double Expression::solve()
{
	Calculation();		//в результате мы прошли через все этапы
	try {
		if (KeyError > Error::ALL_RIGHT)
			throw KeyError;
		else return result;
	}
	catch (int problem)
	{
		switch (problem)
		{
		case(Error::ALL_RIGHT):
			return result;
			break;
		case(Error::INCORRECT_SYMBOL):
			cerr << "Incorrect symbol, position number: " << IdError << "\n";
			throw "Incorrect symbol";
			return NAN;
			break;
		case(Error::INCORRECT_PLACEMENT_OF_BRACKETS):
			cerr << "Incorrect placement of brackets\n";
			throw "Incorrect placement of brackets";
			return NAN;
			break;
		case(Error::SYNTAX_ERROR):
			cerr << "Syntax error\n";
			throw "Syntax error";
			return NAN;
			break;
		case(Error::DIVISION_BY_ZERO):
			cerr << "Division by zero\n";
			throw "Division by zero";
			return NAN;
			break;
		}
	}
}