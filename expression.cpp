/**
 * Title:       Expression
 * Purpose:		Definition of the Expression Class
 * Author:		Chris Kendall
 * Date:		May 5, 2023
 */

#include "expression.h"

#include <stack>
#include <sstream>
#include <cctype>
#include <cstdlib>

using std::stringstream;
using std::stack;

/**
 * Got this from: https://stackoverflow.com/questions/1798112/removing-leading-and-trailing-spaces-from-a-string
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * W A R N I N G        W A R N I N G        W A R N I N G        W A R N I N G    * *
 * Even when I am giving you code that I found on the Internet, you are still      * *
 * not allowed to copy ANY code from the Internet or from any AI tool.             * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * @param str the string that we want to trim from trailing and leading spaces
 * @param whitespace the whitespaces we want to remove, by default it will remove space and tab
 * @return a string without leading or trailing spaces.
 */
string trim(const string& str, const string& whitespace){
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == string::npos)
        return ""; // no content
    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;
    return str.substr(strBegin, strRange);
}

/**
 * Default constructor of the Expression class
 */
Expression::Expression(): _infix(""), _postfix("") {

}

/**
 * Constructor of Expression class, give a string that will be stored in _infix
 * @param infix the string to be stored in _infix
 */
Expression::Expression(const string& infix): _infix(infix), _postfix("") {

}

/**
 * This method takes only implied parameters of the Expression object class and converts their _infix string
 * to the proper _postfix, storing it in _postfix as such. It returns nothing.
 */
void Expression::ConvertToPostfix() {
    stack<char> mystack;
    stringstream ss;
    string operand;
    for (char c: _infix) {
        if (c == ' ') {
            continue; // Ignore spaces in the input
        }
        if (isalnum(c)) {
            operand += c; // Append the character to operand
        } else {
            if (!operand.empty()) {
                ss << operand << ' ';
                operand.clear(); // Reset operand
            }
            if (c == '(') {
                mystack.push(c);
            } else if (c == ')') {
                while (!mystack.empty() && mystack.top() != '(') {
                    ss << mystack.top() << ' ';
                    mystack.pop();
                }
                if (!mystack.empty()) {
                    mystack.pop(); // Remove the '(' from the stack
                }
            } else {
                while (!mystack.empty() && Precedence(c) <= Precedence(mystack.top())) {
                    ss << mystack.top() << ' ';
                    mystack.pop();
                }
                mystack.push(c);
            }
        }
    }

    // Add the last operand, if any
    if (!operand.empty()) {
        ss << operand << ' ';
    }

    while (!mystack.empty()) {
        ss << mystack.top() << ' ';
        mystack.pop();
    }

    _postfix = trim(ss.str());
}

/**
 * This method gets _infix and returns it
 * @return the _infix string of the implied object
 */
string Expression::GetInfix() const {
    return _infix;
}

/**
 * This method gets _postfix and returns it
 * @return the _postfix string of the implied object
 */
string Expression::GetPostfix() const {
    return _postfix;
}

/**
 * Returns a .JSON formatted string representation of the implied Expression object
 * @return a string representation of _infix and _postfix formatted in .JSON
 */
string Expression::ToJSON() const {
    return "{\"infix\":\"" + _infix + "\", \"postfix\":\"" + _postfix + "\"}";
}

/**
 * This method takes an implied Expression object and calculated the value of the stored
 * _postfix expression. If there is an error, 'error' is returned as true and 0 is returned.
 * @param error the bool parameter passed in order to return whether or not an error occurred while
 * still returning a double regularly. 'error' = true if an error occurs.
 * @return a double representation of the evaluated _postfix expression
 */
double Expression::Evaluate(bool& error) const {
    stack<double> myStack;
    string op;

    // traverse through the postfix expression
    for (char c : _postfix) {
        // handle spaces
        if (isspace(c)) {
            // if op has value, push to stack and clear op value
            if (!op.empty()) {
                myStack.push(stod(op));
                op.clear();
            }
        // handle letter inputs
        } else if (isalpha(c)) {
            error = true;
            return 0;
        // if reaches this point it is not a space or letter - checking now if it is a number
        } else if (isalnum(c)) {
            op += c;
        // assuming no bad inputs the char must now be an operator
        } else {
            // store the top 2 double values from the stack into temp variables and remove them from the stack
            double b = myStack.top(); myStack.pop();
            double a = myStack.top(); myStack.pop();

            // depending on the symbol present, handle arithmetic
            // since properly formatted postfix, don't need to worry about precedence of symbols
            switch (c) {
                case '+': myStack.push(a + b); break;
                case '-': myStack.push(a - b); break;
                case '*': myStack.push(a * b); break;
                case '/':
                    // make sure it is not an undefined operation
                    if (b == 0) {
                        error = true;
                        return 0;
                    }
                    myStack.push(a / b);
                    break;
                // if it's not one of these symbols (it should be b/c no bad inputs) error by default
                default:
                    error = true;
                    return 0;
            }
        }
    }
    // if there are still contents in stack after operations,etc. have been used up throw error
    if (myStack.size() != 1) {
        error = true;
        return 0;
    }

    error = false;
    return myStack.top();
}

/**
 * This is a helper method to determine the precedence of the operator. Assuming valid input only 1 or 2
 * should be returned. The higher the number, the higher the precedence.
 * @param op the char of the operator to be evaluated for precedence
 * @return size_t level of precedence the char operator holds. (1 or 2 ... or 0 if invalid input)
 */
size_t Expression::Precedence(char& op) {
    switch (op) {
        case '+':
        case '-':
            return 1;
        case '*':
        case '/':
            return 2;
        default:
            return 0;
    }
}
