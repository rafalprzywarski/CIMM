#pragma once
#include <stdexcept>
#include "string.hpp"

namespace cimm
{

struct evaluation_error : std::runtime_error
{
    evaluation_error(const string& msg) : std::runtime_error(msg) {}
};

}
