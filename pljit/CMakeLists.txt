set(PLJIT_SOURCES
        main.cpp
        SourceCodeManager.cpp SourceCodeManager.h Lexer.cpp Lexer.h Token.cpp Token.h Pljit.cpp Pljit.h TreeNode.cpp TreeNode.h Parser.cpp Parser.h ParseTreeVisitor.h ParsePrintVisitor.cpp ParsePrintVisitor.h)

add_library(pljit_core ${PLJIT_SOURCES})
target_include_directories(pljit_core PUBLIC ${CMAKE_SOURCE_DIR})

add_clang_tidy_target(lint_pljit_core ${PLJIT_SOURCES})
add_dependencies(lint lint_pljit_core)

add_executable(pljit main.cpp)
target_link_libraries(pljit PUBLIC pljit_core)
