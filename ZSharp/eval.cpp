// ZS program to evaluate a given expression
#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <limits>
#include <stack>
#include "eval.h"
#include "strops.h"
#include <math.h>
// #include "builtin.h"
using namespace std;

float precedence(const char& op) {
	if (op == '+' || op == '-')
		return 1;
	if (op == '*' || op == '/')
		return 2;
	if (op == '^')
		return 3;
	return 0;
}

float applyOp(const float& a, const float& b, const char& op) {
	switch (op) {
	case '+': return a + b;
	case '-': return a - b;
	case '*': return a * b;
	case '/': return a / b;
	case '^': return pow(a, b);
	}
	string s(1, op);
	//LogWarning("operator \'" + s + "\' does not exist");
	return 0;
}

// Function that returns value of
// expression after evaluation.
float evaluate(const string& t)
{
	string tokens = replace(t, " ", "");

	float i;

	stack <float> values;
	stack <char> ops;
	bool negative = false;

	for (i = 0; i < tokens.length(); i++)
	{
		// Current token is an opening
		// brace, push it to 'ops'
		if (tokens[i] == '(')
		{
			ops.push(tokens[i]);
		}

		// Current token is a number, push
		// it to stack for numbers.
		else if (isdigit(tokens[i]))
		{
			float val = 0;
			bool encounteredDecimal = false;
			int decPlaces = 0;
			// There may be more than one
			// digits in number.
			while (i < tokens.length() && (isdigit(tokens[i]) || tokens[i] == '.'))
			{
				if (tokens[i] == '.')
				{
					encounteredDecimal = true;
					i++;
					decPlaces++;
					continue;
				}

				if (encounteredDecimal == true)
				{
					val = (val)+(tokens[i] - '0') / pow(10.0f, decPlaces);
					decPlaces++;
				}
				else
					val = (val * 10) + (tokens[i] - '0');

				i++;
			}
			
			values.push(val);

			i--;
		}
		// Closing brace encountered, solve
		// entire brace.
		else if (tokens[i] == ')')
		{
			while (!ops.empty() && ops.top() != '(')
			{
				float val2 = values.top();
				values.pop();

				float val1 = values.top();
				values.pop();

				char op = ops.top();
				ops.pop();

				values.push(applyOp(val1, val2, op));
			}

			if (!ops.empty())
				ops.pop();
		}
		// Current token is an operator.
		else
		{
			if (tokens[i] == '-' && (i == 0 || tokens[i-1] == '*' || tokens[i-1] == '/' || tokens[i-1] == '+' || tokens[i-1] == '-' || tokens[i-1] == '^' || tokens[i-1] == '(' || tokens[i-1] == ')'))
			{
				negative = true;
				continue;
			}
			else
			{
				while (!ops.empty() && precedence(ops.top()) >= precedence(tokens[i])) {
					float val2 = values.top();
					values.pop();

					float val1 = values.top();
					values.pop();

					char op = ops.top();
					ops.pop();

					values.push(applyOp(val1, val2, op));
				}

				ops.push(tokens[i]);
			}
		}

		if (negative)
		{
			values.top() *= -1;
			negative = false;
		}
	}

	// Entire expression has been parsed at this
	// point, apply remaining ops to remaining
	// values.
	while (!ops.empty()) {
		float val2 = values.top();
		values.pop();

		float val1 = values.top();
		values.pop();

		char op = ops.top();
		ops.pop();

		values.push(applyOp(val1, val2, op));
	}

	return values.top();
}

// Used to test evaluator
//int main() {
//	cout << evaluate("-10 + 2 * 6 * -3") << "\n";
//	cout << evaluate("100 * 2 + 12") << "\n";
//	cout << evaluate("100 * (0 + (12 - 3))") << "\n";
//	cout << evaluate("0.05*0.05");
//	return 0;
//}
