#pragma once
#include <string>
#include <ostream>

namespace cimm
{

class string
{
public:
    using const_iterator = std::string::const_iterator;

    string() {}
    string(std::string s) : value(std::move(s)) { }
    string(const char *s) : value(s) { }

    auto c_str() const { return value.c_str(); }
    auto std_hash() const { return std::hash<std::string>()(value); }

    friend const_iterator begin(const string& s) { return std::begin(s.value); }
    friend const_iterator end(const string& s) { return std::end(s.value); }

    friend auto operator==(const string& left, const string& right) { return left.value == right.value; }
    friend string operator+(char left, const string& right) { return left + right.value; }
    friend string operator+(const string& left, char right) { return left.value + right; }
    friend string operator+(const string& left, const string& right) { return left.value + right.value; }
    friend auto& operator<<(std::ostream& os, const string& s) { return os << s.value; }
private:
    std::string value;
};

auto pr_str(const string& s) -> string;

}

namespace std
{
template <>
struct hash<cimm::string>
{
    typedef cimm::string argument_type;
    typedef std::size_t result_type;
    result_type operator()(argument_type const& s) const { return s.std_hash(); }
};

}
