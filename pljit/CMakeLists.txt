set(PLJIT_SOURCES
        main.cpp
        SourceCodeManager.cpp SourceCodeManager.h Lexer/Lexer.cpp Lexer/Lexer.h Lexer/Token.h Pljit.cpp Pljit.h Parser/ParseTreeNode.h Parser/Parser.cpp Parser/Parser.h Parser/ParseTreeVisitor.h Parser/ParsePrintVisitor.cpp Parser/ParsePrintVisitor.h SemanticAnalysis/AstNode.h SemanticAnalysis/SymbolTable.cpp SemanticAnalysis/SymbolTable.h SemanticAnalysis/SemanticAnalyzer.cpp SemanticAnalysis/SemanticAnalyzer.h SemanticAnalysis/AstVisitor.h Parser/ParseTreeNode.cpp SemanticAnalysis/AstPrintVisitor.cpp SemanticAnalysis/AstPrintVisitor.h)

add_library(pljit_core ${PLJIT_SOURCES})
target_include_directories(pljit_core PUBLIC ${CMAKE_SOURCE_DIR})

add_clang_tidy_target(lint_pljit_core ${PLJIT_SOURCES})
add_dependencies(lint lint_pljit_core)

add_executable(pljit main.cpp)
target_link_libraries(pljit PUBLIC pljit_core)
