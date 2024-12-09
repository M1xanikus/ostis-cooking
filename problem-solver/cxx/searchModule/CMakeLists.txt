file(GLOB_RECURSE SOURCES "*.cpp" "*.hpp")

add_library (search-module SHARED ${SOURCES})
target_link_libraries(search-module LINK_PRIVATE sc-memory)
include_directories(search-module PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})