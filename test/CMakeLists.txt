set(TEST_SOURCES
    # add your *.cpp files here
        Tester.cpp
        Tester_Lexer.cpp Tester_Parser.cpp Tester_Semantic.cpp Tester_Evaluation.cpp Tester_Optimisation.cpp Tester_Pljit.cpp)

add_executable(tester ${TEST_SOURCES})
target_link_libraries(tester PUBLIC
    pljit_core
    GTest::GTest)
