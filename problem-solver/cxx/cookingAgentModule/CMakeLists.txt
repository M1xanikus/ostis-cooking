file(GLOB_RECURSE SOURCES "*.cpp" "*.hpp")

add_library(cooking-agent-module SHARED ${SOURCES})

target_link_libraries(cooking-agent-module
    LINK_PRIVATE sc-memory
)
target_include_directories(cooking-agent-module
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)
