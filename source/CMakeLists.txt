add_definitions("-Wall -Wno-unused-local-typedef")
include_directories(".")

add_subdirectory("core")
add_subdirectory("repl")
add_subdirectory("runner")
add_subdirectory("test")
