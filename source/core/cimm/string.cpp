#include "string.hpp"
#include <array>
#include <utility>

namespace cimm
{

namespace
{

std::array<std::pair<char, const char *>, 3> escaped = { { { '\n', "\\n" }, { '\"', "\\\"" }, { '\\', "\\\\" } } };

}

auto pr_str(const string& s) -> string
{
    string printed = "\"";

    for (auto c : s)
    {
        auto found = std::find_if(begin(escaped), end(escaped), [c](auto& e) { return e.first == c; });
        if (found != end(escaped))
            printed += found->second;
        else
            printed += c;
    }
    printed += "\"";
    return printed;
}

}
