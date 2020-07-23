#include "SourceCodeManager.h"

namespace jit {

SourceCodeManager::SourceCodeManager(std::string& sourceCode) : code{sourceCode} {
    char newline = '\n';
    size_t end = code.find_last_not_of(newline);
    code.erase(end + 1);

    if (code.empty()) {
        noflines = 0;
        return;
    }

    noflines = 1;

    size_t pos = 0;
    lines.push_back(0);

    while (pos < code.size()) {
        pos = code.find('\n', pos + 1);
        if (pos == std::string::npos) {
            code.push_back('\n');
            lines.push_back(code.size());
            return;
        }

        lines.push_back(pos + 1);
        ++noflines;
    }
}

void SourceCodeManager::printErrorMessage(const std::string& message, const SourceCodeReference& location) const {
    std::cerr << location.line << ":" << location.position << ":  " << message << std::endl;

    std::string_view codeview{code};
    std::cerr << codeview.substr(lines[location.line - 1], lines[location.line] - lines[location.line - 1]);

    for (size_t i = 1; i < location.position; ++i)
        std::cerr << ' ';

    std::cerr << '^';

    for (size_t i = 1; i < location.range; ++i)
        std::cerr << '~';

    std::cerr << std::endl;
}

std::string_view SourceCodeManager::getString(const SourceCodeReference &loc) const {

    return std::string_view{code.data() + getabsolutePosition(loc), loc.range};
}



} // namespace jit