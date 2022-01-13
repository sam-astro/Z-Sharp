
#ifndef MAIN_H
#define MAIN_H

using namespace std;

boost::any ExecuteFunction(const string& functionName, const vector<boost::any>& inputVarVals);
boost::any EvalExpression(const string& ex, unordered_map<string, boost::any>& variableValues);

#endif
