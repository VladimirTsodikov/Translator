#include <iostream>
#include "stack.h"
#include "Expression.h"
#include <string>

int main()
{
	string tmp;
	getline(cin, tmp);

	Expression test(tmp);
	try {
		double k = test.solve();
		cout << k;
	}
	catch (const char*)
	{
		return 0;	//аварийное завершение программы
	}
}