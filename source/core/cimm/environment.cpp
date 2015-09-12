#include "environment.hpp"

namespace cimm
{

auto define_native_function(environment& env, string name, native_function fn) -> void
{
    env.functions[name] = fn;
}

}
