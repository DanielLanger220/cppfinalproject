#include "FunctionObject.h"
#include "pljit/SemanticAnalysis/AstNode.h"

namespace jit {


FunctionObject::FunctionObject(std::string code) : sourceCode(std::move(code)), manager{sourceCode} {

}


} // namespace jit