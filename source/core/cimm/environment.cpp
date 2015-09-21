#include "environment.hpp"

namespace cimm
{

auto define_native_function(environment& env, symbol name, native_function fn) -> void
{
    env.definitions.emplace(std::move(str(name)), std::move(fn));
}

}
