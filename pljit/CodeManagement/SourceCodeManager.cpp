#include "pljit/CodeManagement/SourceCodeManager.h"

using namespace std;

namespace jit {

SourceCodeManager::SourceCodeManager(const string &sourceCode) : code{sourceCode} {

    if (code.empty()) {
        noflines = 0;
        return;
    }

    noflines = 1;

    size_t pos = 0;
    lines.push_back(0);

    while (pos < code.size()) {
        pos = code.find('\n', pos);
        if (pos == string::npos) {
            lines.push_back(code.size());
            return;
        }

        lines.push_back(pos + 1);
        ++noflines;
        ++pos;
    }
}

void SourceCodeManager::printErrorMessage(const string& message, const SourceCodeReference& location) const {

    cerr << location.line << ":" << location.position << ":  " << message << endl;
    cerr << code.substr(lines[location.line - 1], lines[location.line] - lines[location.line - 1]);

    for (size_t i = 1; i < location.position; ++i)
        cerr << ' ';

    cerr << '^';

    for (size_t i = 1; i < location.range; ++i)
        cerr << '~';

    cerr << endl;
}


string_view SourceCodeManager::getString(const SourceCodeReference &loc) const {

    return string_view{code.data() + getabsolutePosition(loc), loc.range};
}



} // namespace jit