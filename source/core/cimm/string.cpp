#include "string.hpp"
#include <array>
#include <utility>

namespace cimm
{

namespace
{

std::array<std::pair<char, const char *>, 3> escaped = { { { '\n', "\\n" }, { '\"', "\\\"" }, { '\\', "\\\\" } } };

void append_char(char c, std::string& printed)
{
    auto found = std::find_if(begin(escaped), end(escaped), [c](auto& e) { return e.first == c; });
    if (found != end(escaped))
        printed += found->second;
    else
        printed += c;
}

}

auto pr_str(const string& s) -> string
{
    std::string printed = "\"";
    for (auto c : s)
        append_char(c, printed);
    return printed += "\"";
}

}
