#pragma once
#include "evaluation_error.hpp"

namespace cimm
{

struct type_error : evaluation_error
{
    type_error(const expression& val, const string& type) : evaluation_error(pr_str(val) + " is not " + type) { }
};

}
