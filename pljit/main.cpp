#include <iostream>

#include "SourceCodeManager.h"
#include "Lexer.h"

//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------
int main() {

    std::string code = "Daniel liebt die niedliche :=Sophie!, 220 + 284 = 504\n(220 + 220 = 440);";


    SourceCodeManager manager(code);



    Lexer lex(code, manager);

    for(int i = 0; i < 22; ++i) {

        std::unique_ptr<Token> res = lex.nextToken();

        if (res == nullptr)
            break;

        cout << (int)res->tokentype;

        if (res->tokentype == Token::TokenType::ArithmeticOperator)
            cout << "\t" << (int)static_cast<ArithmeticOperator*>(res.get())->type << endl;
        else if (res->tokentype == Token::TokenType::Identifier)
            cout << "\t" << static_cast<Identifier*>(res.get())->id << endl;
        else if (res->tokentype == Token::TokenType::Literal)
            cout << "\t" << static_cast<Literal*>(res.get())->value << endl;
        else
            cout << "\n";
    }




}
//---------------------------------------------------------------------------
