#include <stdexcept>

#include "FunctionAnalyzer.h"
#include "ParserTypes.h"
#include "Error.h"
#include "Debug.h"

FunctionAnalyzer::FunctionAnalyzer() {
}

void FunctionAnalyzer::addTypeSignature(CharmTypeSignature t) {
    typeSignatures[t.functionName] = t;
}

void FunctionAnalyzer::addToInlineDefinitions(CharmFunction f) {
    if (f.functionType == FUNCTION_DEFINITION) {
        ONLYDEBUG printf("Adding %s to the inlineDefinitions\n", f.functionName.c_str());
        inlineDefinitions[f.functionName] = f;
    } else {
        throw std::runtime_error("Tried to insert non-function definition into inlineDefinitions");
    }
}

bool FunctionAnalyzer::doInline(CHARM_LIST_TYPE& out, CharmFunction currentFunction) {
    //search through the inline definitions that have been parsed to see if this function is inlineable
    auto fIter = inlineDefinitions.find(currentFunction.functionName);
    ONLYDEBUG printf("Looking for inlineDefinition of %s, did we find it? %s\n", currentFunction.functionName.c_str(), fIter != inlineDefinitions.end() ? "Yes" : "No");
    if (fIter != inlineDefinitions.end()) {
        ONLYDEBUG printf("PERFORMING INLINE REPLACEMENT FOR %s\n    %s -> ", currentFunction.functionName.c_str(), currentFunction.functionName.c_str());
        for (CharmFunction inlineReplacement : fIter->second.literalFunctions) {
            out.push_back(inlineReplacement);
            ONLYDEBUG printf("%s ", charmFunctionToString(inlineReplacement).c_str());
        }
        ONLYDEBUG printf("\n");
        if (DEBUGMODE) {
            printf("AFTER INLINE OPTIMIZATION, OUT NOW LOOKS LIKE THIS:\n     ");
            for (CharmFunction f : out) {
                printf("%s ", charmFunctionToString(f).c_str());
            }
            printf("\n");
        }
        return true;
    }
    return false;
}

bool FunctionAnalyzer::_isInlineable(std::string fName, CharmFunction f) {
	//if the function calls itself, it's recursive and not inlineable
	bool recursive = false;
	for (unsigned long long fIndex = 0; fIndex < f.literalFunctions.size(); fIndex++) {
		if (f.literalFunctions[fIndex].functionType == LIST_FUNCTION) {
			recursive = recursive || (!_isInlineable(fName, f.literalFunctions[fIndex]));
		} else {
			recursive = recursive || (fName == f.literalFunctions[fIndex].functionName);
		}
		if (recursive) {
			break;
		}
	}
	return !recursive;
}
bool FunctionAnalyzer::isInlinable(CharmFunction f) {
	return (_isInlineable(f.functionName, f));
}

bool FunctionAnalyzer::isTailCallRecursive(CharmFunction f) {
	//this is only static, basic tail call recursion analysis.
	//this only catches functions of form `f := <code> f`
	//and sends this flag off to Runner.cpp::handleDefinedFunctions()
	//the rest of the tail call recursion code happens within PredefinedFunctions.cpp::ifthen
	//over there, the usual case is caught, with code of the form `f := [ <cond> ] [ <code> f] [ <code> f ] ifthen`
    if (f.literalFunctions.size() > 0) {
       return (f.functionName == f.literalFunctions.back().functionName);
    } else {
        return false;
    }
}
