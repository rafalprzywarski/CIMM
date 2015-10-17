#include "environment.hpp"

namespace cimm
{

auto define_native_function(environment& env, native_function fn) -> void
{
    env.definitions.emplace(std::move(name(fn)), std::move(fn));
}

}
