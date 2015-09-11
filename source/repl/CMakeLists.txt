include_directories("../core")

add_executable(cimm_repl main.cpp)
target_link_libraries(cimm_repl cimm_core)
