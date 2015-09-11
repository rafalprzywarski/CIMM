add_definitions("-Wall")

include_directories(${GoogleMock_INCLUDE_DIRS})
include_directories("../core")

add_executable(cimm_test
  cimm/parse_test.cpp
  main.cpp
)
target_link_libraries(cimm_test ${Boost_LIBRARIES} gmock pthread)
