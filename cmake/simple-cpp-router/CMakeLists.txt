cmake_minimum_required ( VERSION 3.10.2 )
project ( routing )

set ( CMAKE_CURRENT_SOURCE_DIR "${VOLITION_ROOT}/3rd-party/simple-cpp-router/src/" )

set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLUA_USE_POSIX" )

add_library ( routing "" )

target_include_directories ( routing
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources ( routing
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}Routing/PathMatch.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}Routing/Router.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}Routing/Details/PathTemplate.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}Routing/Details/PathTemplateParser.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}Routing/Details/RegexConverter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}Routing/Details/StringTokenizer.cpp
)

#install ( TARGETS routing EXPORT librouting ARCHIVE DESTINATION lib )
#install ( DIRECTORY src/routing/ DESTINATION include/routing FILES_MATCHING PATTERN "*.h" )
