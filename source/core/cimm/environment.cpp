#include "environment.hpp"

namespace cimm
{

auto define_native_function(environment& env, symbol name, native_function fn) -> void
{
    env.functions.emplace(std::move(name.value), std::move(fn));
}

}
