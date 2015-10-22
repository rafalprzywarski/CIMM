include_directories("../core")

add_executable(cimm main.cpp)
target_link_libraries(cimm cimm_core)
