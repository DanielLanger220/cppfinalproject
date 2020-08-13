#include "../pljit/SemanticAnalysis/AstNode.h"
#include "gtest/gtest.h"
#include "pljit/Pljit/Pljit.h"

#include <thread>

using namespace std;
using namespace jit;

namespace jit::Tester_Pljit {


string code1 = "PARAM a, b;\n"
              "VAR c;\n"
              "CONST d = 220;\n"
              "BEGIN\n"
              "c := (a + b) * d;\n"
              "RETURN (a - 2 * b) + 3 * c\n"
              "END.\n";

string code2 = "PARAM a, b;\n"
               "VAR c, d;\n"
               "BEGIN\n"
               "c := a * a;\n"
               "d := b * (-b)\n;"
               "RETURN c + d;\n"
               "RETURN a;\n"
               "RETURN b;\n"
               "RETURN c\n"
               "END.";

void run1(Pljit::PljitHandle h, int64_t v) {

    optional<int64_t> result{};

    for(int i = 0; i < 10; ++i) {
        result = h({v, 2 * v});
    }

    EXPECT_EQ(result.value(), v - 4 * v + 3 *  3 * v * 220);
}

void run2(Pljit::PljitHandle h, int64_t v) {

    optional<int64_t> result{};

    for(int i = 0; i < 10; ++i) {
        result = h({v, 2 * v});
    }

    EXPECT_EQ(result.value(), v * v - 4 * v * v);

}

void run(Pljit::PljitHandle h1, Pljit::PljitHandle h2, int64_t v) {

    optional<int64_t> result1{};
    optional<int64_t> result2{};

    for(int i = 0; i < 10; ++i) {
        result1 = h1({v, 2 * v});
        result2 = h2({v, -v});
    }

    EXPECT_EQ(result1.value(), v - 4 * v + 3 *  3 * v * 220);
    EXPECT_EQ(result2.value(), 0);


}

TEST(Pljit, test1) {

    Pljit jit{};

    auto h1 = jit.registerFunction(code1);
    auto h2 = jit.registerFunction(code2);

    vector<thread> threads{};
    for(int i = 0; i < 100; ++i)
        threads.emplace_back(thread{run1, h1, i});

    for(int i = 0; i < 100; ++i)
        threads[i].join();

}

TEST(Pljit, test2) {

    Pljit jit{};

    auto h1 = jit.registerFunction(code1);
    auto h2 = jit.registerFunction(code2);

    vector<thread> threads{};
    for(int i = 0; i < 100; ++i)
        threads.emplace_back(thread{run, h1, h2, i});

    for(int i = 0; i < 100; ++i)
        threads[i].join();

}

} // namespace jit::Tester_Pljit