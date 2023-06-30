#include "stack.h"
#include "Expression.h"
#include <gtest.h>

TEST(Expression, can_create_expression)
{
	ASSERT_NO_THROW(Expression tmp("-5+2"));
}

TEST(Expression, can_get_solve_without_brackets)
{
	Expression exp("5+2*7");

	//Неточные сравнения для double
	EXPECT_DOUBLE_EQ(19, exp.solve());
	EXPECT_EQ(Error::ALL_RIGHT, exp.GetKeyError());

	Expression exp3("-5+2*()=");
	Expression exp4("17/0");
	Expression exp5("-9*(4/(3-1-1-1))");

	


	ASSERT_THROW(exp3.solve(), const char*);
	ASSERT_THROW(exp4.solve(), const char*);
	ASSERT_THROW(exp5.solve(), const char*);
}

TEST(Expression, can_get_solve_with_brackets)
{
	Expression exp("5*(8+4)/5-2");

	EXPECT_DOUBLE_EQ(10, exp.solve());
	EXPECT_EQ(Error::ALL_RIGHT, exp.GetKeyError());
}

TEST(Expression, can_handle_unary_operations)
{
	Expression exp("-5+2");

	EXPECT_DOUBLE_EQ(-3, exp.solve());
	EXPECT_EQ(Error::ALL_RIGHT, exp.GetKeyError());
}

TEST(Expression, can_handle_an_equal_sign_at_the_end)
{
	Expression exp("-5.88+2.19=");

	EXPECT_DOUBLE_EQ(-3.69, exp.solve());
	EXPECT_EQ(Error::ALL_RIGHT, exp.GetKeyError());
}

TEST(Expression, can_handle_an_equal_sign_in_the_middle)	//чтение строки будет идти до знака равно, ответ -15 
{
	Expression exp("-17+2=-5+4");

	EXPECT_DOUBLE_EQ(-15, exp.solve());
	EXPECT_EQ(Error::ALL_RIGHT, exp.GetKeyError());
}

TEST(Expression, can_handle_an_equal_sign_at_the_beginning)
{
	Expression exp("=5+4");

	EXPECT_DOUBLE_EQ(0, exp.solve());
	EXPECT_EQ(Error::ALL_RIGHT, exp.GetKeyError());
}

TEST(Expression, can_handle_spaces)
{
	Expression exp("-5 + 2 *    3=");

	EXPECT_DOUBLE_EQ(1, exp.solve());
	EXPECT_EQ(Error::ALL_RIGHT, exp.GetKeyError());
}

TEST(Expression, can_return_throw_and_id_when_lexical_error)
{
	Expression exp1("5+d");	//3
	ASSERT_THROW(exp1.solve(), const char*);
	EXPECT_EQ(Error::INCORRECT_SYMBOL, exp1.GetKeyError());
	EXPECT_EQ(3, exp1.GetIdError());

	Expression exp2("-52ew+2=");		//4
	ASSERT_THROW(exp2.solve(), const char*);
	EXPECT_EQ(Error::INCORRECT_SYMBOL, exp2.GetKeyError());
	EXPECT_EQ(4, exp2.GetIdError());

	Expression exp3("(5+15) - 9+s");	//12, т.к. считаем пробелы
	ASSERT_THROW(exp3.solve(), const char*);
	EXPECT_EQ(Error::INCORRECT_SYMBOL, exp3.GetKeyError());
	EXPECT_EQ(12, exp3.GetIdError());
}

TEST(Expression, can_return_throw_when_brackets_control_error)
{
	Expression exp1("-(5(7 + 2) =");	//ещё синтаксическая ошибка. Пока не обращаем внимания
	ASSERT_THROW(exp1.solve(), const char*);
	EXPECT_EQ(Error::INCORRECT_PLACEMENT_OF_BRACKETS, exp1.GetKeyError());
	
	Expression exp2("-5(7 + 2) )= ");
	ASSERT_THROW(exp2.solve(), const char*);
	EXPECT_EQ(Error::INCORRECT_PLACEMENT_OF_BRACKETS, exp2.GetKeyError());

	Expression exp3("-(5(7 + 2) =)");	//последняя закрывающая скобка за знаком равно, не будет читаться
	ASSERT_THROW(exp3.solve(), const char*);
	EXPECT_EQ(Error::INCORRECT_PLACEMENT_OF_BRACKETS, exp3.GetKeyError());
}

TEST(Exspression, can_return_throw_when_syntactic_control_error)
{
	Expression exp1("-(5+2)*=");	//знак операции в конце выражения
	ASSERT_THROW(exp1.solve(), const char*);
	EXPECT_EQ(Error::SYNTAX_ERROR, exp1.GetKeyError());

	Expression exp2("/(5+2)=");	//не унарный знак операции в начале выражения
	ASSERT_THROW(exp2.solve(), const char*);
	EXPECT_EQ(Error::SYNTAX_ERROR, exp2.GetKeyError());
	
	Expression exp3("-(5+-2)=");	//два знака операций подряд
	ASSERT_THROW(exp3.solve(), const char*);
	EXPECT_EQ(Error::SYNTAX_ERROR, exp3.GetKeyError());
	
	Expression exp4("-(5 2)=");	//два числа подряд
	ASSERT_THROW(exp4.solve(), const char*);
	EXPECT_EQ(Error::SYNTAX_ERROR, exp4.GetKeyError());
	
	Expression exp5("-(*2+5)=");	//не унарный знак операции после открывающей скобки
	ASSERT_THROW(exp5.solve(), const char*);
	EXPECT_EQ(Error::SYNTAX_ERROR, exp5.GetKeyError());
	
	Expression exp6("-5+2*()=");	//закрывающая скобка сразу после открывающей. Выявляется в синтаксической проверке, а не в проверке на скобки
	ASSERT_THROW(exp6.solve(), const char*);
	EXPECT_EQ(Error::SYNTAX_ERROR, exp6.GetKeyError());
}

TEST(Expression, can_return_throw_when_calculation_error)
{
	Expression exp("17/0");
	ASSERT_THROW(exp.solve(), const char*);
	EXPECT_EQ(Error::DIVISION_BY_ZERO, exp.GetKeyError());
}