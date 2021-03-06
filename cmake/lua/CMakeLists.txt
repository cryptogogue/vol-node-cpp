cmake_minimum_required ( VERSION 3.10.2 )
project ( lua )

set ( CMAKE_CURRENT_SOURCE_DIR "${VOLITION_ROOT}/3rd-party/lua-5.3.5/src/" )

set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLUA_USE_POSIX" )

add_library ( lua "" )

target_include_directories ( lua
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources ( lua
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}lapi.c
        ${CMAKE_CURRENT_SOURCE_DIR}lauxlib.c
        ${CMAKE_CURRENT_SOURCE_DIR}lbaselib.c
        ${CMAKE_CURRENT_SOURCE_DIR}lbitlib.c
        ${CMAKE_CURRENT_SOURCE_DIR}lcode.c
        ${CMAKE_CURRENT_SOURCE_DIR}lcorolib.c
        ${CMAKE_CURRENT_SOURCE_DIR}lctype.c
        ${CMAKE_CURRENT_SOURCE_DIR}ldblib.c
        ${CMAKE_CURRENT_SOURCE_DIR}ldebug.c
        ${CMAKE_CURRENT_SOURCE_DIR}ldo.c
        ${CMAKE_CURRENT_SOURCE_DIR}ldump.c
        ${CMAKE_CURRENT_SOURCE_DIR}lfunc.c
        ${CMAKE_CURRENT_SOURCE_DIR}lgc.c
        ${CMAKE_CURRENT_SOURCE_DIR}linit.c
        ${CMAKE_CURRENT_SOURCE_DIR}llex.c
        ${CMAKE_CURRENT_SOURCE_DIR}liolib.c
        ${CMAKE_CURRENT_SOURCE_DIR}lmathlib.c
        ${CMAKE_CURRENT_SOURCE_DIR}lmem.c
        ${CMAKE_CURRENT_SOURCE_DIR}loadlib.c
        ${CMAKE_CURRENT_SOURCE_DIR}lobject.c
        ${CMAKE_CURRENT_SOURCE_DIR}lopcodes.c
        ${CMAKE_CURRENT_SOURCE_DIR}loslib.c
        ${CMAKE_CURRENT_SOURCE_DIR}lparser.c
        ${CMAKE_CURRENT_SOURCE_DIR}lstate.c
        ${CMAKE_CURRENT_SOURCE_DIR}lstring.c
        ${CMAKE_CURRENT_SOURCE_DIR}lstrlib.c
        ${CMAKE_CURRENT_SOURCE_DIR}ltable.c
        ${CMAKE_CURRENT_SOURCE_DIR}ltablib.c
        ${CMAKE_CURRENT_SOURCE_DIR}ltm.c
        ${CMAKE_CURRENT_SOURCE_DIR}lundump.c
        ${CMAKE_CURRENT_SOURCE_DIR}lutf8lib.c
        ${CMAKE_CURRENT_SOURCE_DIR}lvm.c
        ${CMAKE_CURRENT_SOURCE_DIR}lzio.c
)

#install ( TARGETS lua EXPORT liblua ARCHIVE DESTINATION lib )

#install ( FILES 
#    ${CMAKE_CURRENT_SOURCE_DIR}lua.h
#    ${CMAKE_CURRENT_SOURCE_DIR}lualib.h
#    ${CMAKE_CURRENT_SOURCE_DIR}luaconf.h
#    ${CMAKE_CURRENT_SOURCE_DIR}lauxlib.h
#    DESTINATION include/lua
#)

