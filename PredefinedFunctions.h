#pragma once
#include <vector>
#include <string>
#include <any>
#include <unordered_map>
#include <functional>

#include "ParserTypes.h"


//In Runner.h
class Runner;
class FunctionDefinition;

struct BuiltinFunction {
	std::variant<std::function<void(Runner*)>, std::function<void(Runner*, RunnerContext*)>> f;
	bool takesContext;
};

class PredefinedFunctions {
private:
	//helper function to ensure the function is 2 things:
	//a) a number
	//b) an int
	static bool isInt(CharmFunction f);
public:
	std::unordered_map<std::string, BuiltinFunction> cppFunctionNames;
	PredefinedFunctions();
	void functionLookup(std::string functionName, Runner* r, RunnerContext* context);
	void addBuiltinFunction(std::string n, std::function<void(Runner*, RunnerContext*)> f);
	void addBuiltinFunction(std::string n, std::function<void(Runner*)> f);
};
