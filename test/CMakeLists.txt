add_definitions("-Wall")

include_directories(${GoogleMock_INCLUDE_DIRS})
include_directories("../source")

add_executable(cimm_test main.cpp)
target_link_libraries(cimm_test ${Boost_LIBRARIES} gmock pthread)
