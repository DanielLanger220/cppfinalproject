set(PLJIT_SOURCES
        main.cpp
        CodeManagement/SourceCodeManager.cpp
        Lexer/Lexer.cpp
        Lexer/Token.cpp
        Pljit.cpp
        Parser/ParseTreeNode.cpp
        Parser/Parser.cpp
        Parser/ParsePrintVisitor.cpp
        SemanticAnalysis/AstNode.cpp
        SemanticAnalysis/SymbolTable.cpp
        SemanticAnalysis/SemanticAnalyser.cpp
        SemanticAnalysis/AstPrintVisitor.cpp
        Evaluation/EvalInstance.cpp
        SemanticAnalysis/DeadCodeOpt.cpp
        SemanticAnalysis/ConstantPropOpt.cpp)

add_library(pljit_core ${PLJIT_SOURCES})
target_include_directories(pljit_core PUBLIC ${CMAKE_SOURCE_DIR})

add_clang_tidy_target(lint_pljit_core ${PLJIT_SOURCES})
add_dependencies(lint lint_pljit_core)

add_executable(pljit main.cpp)
target_link_libraries(pljit PUBLIC pljit_core Threads::Threads)
