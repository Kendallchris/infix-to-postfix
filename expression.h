/**
 * Title:       Expression
 * Purpose:		Declaration of the Expression Class
 * Author:		Carlos Arias
 * Date:		April 11, 2023
 *
 * D O   N O T   M O D I F Y   T H I S   F I L E
 */

#ifndef INFIX_EXPRESSION_H
#define INFIX_EXPRESSION_H

#include <string>

using std::string;

class Expression {
private:
    string _infix;
    string _postfix;
public:
    Expression();
    Expression(const string& infix);
    void ConvertToPostfix();
    string GetInfix()const;
    string GetPostfix()const;
    string ToJSON()const;
    double Evaluate(bool& error)const;
    size_t Precedence(char& op);
};
string trim(const string& str, const string& whitespace = " \t");

#endif
