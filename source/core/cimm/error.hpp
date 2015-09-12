#pragma once

namespace cimm
{

struct evaluation_error : std::runtime_error
{
    evaluation_error(const string& msg) : std::runtime_error(msg) {}
};

struct undefined_function_error : evaluation_error
{
    undefined_function_error(const string& name)
        : evaluation_error("undefined function \'" + name + "\'") { }
};

}
