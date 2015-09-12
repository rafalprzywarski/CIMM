include_directories("../core")

add_library(cimm_core
  cimm/environment.cpp
  cimm/eval.cpp
  cimm/parse.cpp
  cimm/str.cpp
)
