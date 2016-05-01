include_directories(${GoogleMock_INCLUDE_DIRS})
include_directories("../core")

add_executable(cimm_test
  cimm/def_test.cpp
  cimm/error_test.cpp
  cimm/eval_test.cpp
  cimm/fn_test.cpp
  cimm/if_test.cpp
  cimm/let_test.cpp
  cimm/list_test.cpp
  cimm/keyword_test.cpp
  cimm/parse_test.cpp
  cimm/pr_str_test.cpp
  cimm/slist_test.cpp
  cimm/str_test.cpp
  cimm/symbol_test.cpp
  cimm/vector_test.cpp
  main.cpp
)
target_link_libraries(cimm_test cimm_core ${Boost_LIBRARIES} gmock pthread)
