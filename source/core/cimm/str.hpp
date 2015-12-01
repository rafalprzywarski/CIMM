#pragma once
#include "string.hpp"

namespace cimm
{

class expression;
class vector;
class list;

auto str(const expression& e) -> string;
auto pr_str(const expression& e) -> string;
auto pr_str(vector const& v) -> string;
auto pr_str(list const& l) -> string;

}
