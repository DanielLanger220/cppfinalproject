//
// Created by daniel on 02.07.20.
//

#ifndef PLJIT_SOURCECODEMANAGER_H
#define PLJIT_SOURCECODEMANAGER_H


#include <string>
#include <vector>
#include <iostream>

struct SourceCodeReference {

    SourceCodeReference(size_t line, size_t position, size_t range = 1) : line{line}, position{position}, range{range}{}

    const size_t line;
    const size_t position;
    const size_t range;
};



class SourceCodeManager {

    public:

    // Constructor
    explicit SourceCodeManager(std::string& sourceCode);

    void printErrorMessage(const std::string& message, const SourceCodeReference& location) const;

    size_t getabsolutePosition(const SourceCodeReference& ref) const { return lines[ref.line - 1] + ref.position - 1;}


    private:

    std::string& code; //TODO: make code constant
    size_t noflines{0};
    std::vector<size_t> lines{};


};

#endif //PLJIT_SOURCECODEMANAGER_H
