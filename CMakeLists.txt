cmake_minimum_required(VERSION 3.1)

project(websockets)

#
# common directories
#

set(websockets_ROOT        ${PROJECT_SOURCE_DIR})
set(websockets_EXT_ROOT    ${websockets_ROOT}/ext)

#
# common compiler settings
#

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")

    #add_compile_options(-std=c++11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

    # enable all warnings and treat warnings as errors
    add_compile_options(-Wall -Werror -pedantic -Wno-unused-function)

    # always enable debugging symbols
    add_compile_options(-g)

elseif (MSVC)

    # generic preprocessor definitions
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)

    # config-specific preprocessor definitions
    set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<CONFIG:Debug>:_ITERATOR_DEBUG_LEVEL=2>
    )
    set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS
        $<$<CONFIG:Release>:NDEBUG>
    )
    set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS
        $<$<CONFIG:RelWithDebInfo>:NDEBUG>
        $<$<CONFIG:RelWithDebInfo>:DBGREL>
    )

    # enable all warnings and treat warnings as errors
    add_compile_options(/W4 /WX)

    # set static runtime libs in all configurations
    # enable multiprocessor build in release config
    add_compile_options(
        $<$<CONFIG:Release>:/MT>
        $<$<CONFIG:Release>:/MP>
    )
    add_compile_options(
        $<$<CONFIG:Debug>:/MTd>
    )
    add_compile_options(
        $<$<CONFIG:RelWithDebInfo>:/MT>
        $<$<CONFIG:RelWithDebInfo>:/MP>
        $<$<CONFIG:RelWithDebInfo>:/INCREMENTAL:NO>
        $<$<CONFIG:RelWithDebInfo>:/Zi>
        $<$<CONFIG:RelWithDebInfo>:/Zo>
    )
endif()

#
# sources
#

file (GLOB_RECURSE websockets_EXTERNAL_SOURCES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ext/websocketpp/*.hpp
    ext/websocketpp/base64/*.hpp
    ext/websocketpp/common/*.hpp
    ext/websocketpp/concurrency/*.hpp
    ext/websocketpp/config/*.hpp
    ext/websocketpp/extensions/*.hpp
    ext/websocketpp/http/*.hpp
    ext/websocketpp/impl/*.hpp
    ext/websocketpp/logger/*.hpp
    ext/websocketpp/message_buffer/*.hpp
    ext/websocketpp/processors/*.hpp
    ext/websocketpp/random/*.hpp
    ext/websocketpp/roles/*.hpp
    ext/websocketpp/sha1/*.hpp
    ext/websocketpp/transport/*.hpp
    ext/asio/*.hpp
    ext/asio/asio/detail/*.hpp
    ext/asio/asio/experimental/*.hpp
    ext/asio/asio/generic/*.hpp
    ext/asio/asio/impl/*.hpp
    ext/asio/asio/ip/*.hpp
    ext/asio/asio/local/*.hpp
    ext/asio/asio/posix/*.hpp
    ext/asio/asio/ssl/*.hpp
    ext/asio/asio/ts/*.hpp
    ext/asio/asio/window/*.hpp
)

set(Shared_SOURCES
	include/InputLine.h
	src/InputLine.cpp
)


if (WIN32)
set(Platform_SOURCES
)
else()
set(Platform_SOURCES
	ext/conio-linux/conio.h
	ext/conio-linux/conio.c
)
endif()

set(TestServer_SOURCES
	app/TestServer/TestServer.cpp
)


set(TestClient_SOURCES
	app/TestClient/TestClient.cpp
)

#message("External sources:\n${websockets_EXTERNAL_SOURCES}")

# deal with subdirectories in external sources
foreach(source IN LISTS websockets_EXTERNAL_SOURCES)
    get_filename_component(source_path "${source}" PATH)
    string(REPLACE "/" "\\" source_path_msvc "${source_path}")
    source_group("${source_path_msvc}" FILES "${source}")
endforeach()

# deal with subdirectories in sources
foreach(source IN LISTS TestServer_SOURCES)
    get_filename_component(source_path "${source}" PATH)
    string(REPLACE "/" "\\" source_path_msvc "${source_path}")
    source_group("${source_path_msvc}" FILES "${source}")
endforeach()

# deal with subdirectories in sources
foreach(source IN LISTS TestClient_SOURCES)
    get_filename_component(source_path "${source}" PATH)
    string(REPLACE "/" "\\" source_path_msvc "${source_path}")
    source_group("${source_path_msvc}" FILES "${source}")
endforeach()

#
# executable target
#

add_executable(TestServer
    ${websockets_EXTERNAL_SOURCES}
    ${TestServer_SOURCES}
    ${Platform_SOURCES}
    ${Shared_SOURCES}
)

target_include_directories(TestServer PUBLIC
    ${websockets_EXT_ROOT}
    ${websockets_EXT_ROOT}/asio
    ${websockets_ROOT}/include
    ${websockets_EXT_ROOT}/conio-linux
)

if (WIN32)
    target_link_libraries(TestServer
    )
else()
    target_link_libraries(TestServer
        -ldl
        -lGL
        -lX11
        -lpthread
        -lncurses
    )
endif()




add_executable(TestClient
    ${websockets_EXTERNAL_SOURCES}
    ${TestClient_SOURCES}
    ${Platform_SOURCES}
    ${Shared_SOURCES}
)

target_include_directories(TestClient PUBLIC
    ${websockets_EXT_ROOT}
    ${websockets_EXT_ROOT}/asio
    ${websockets_ROOT}/include
    ${websockets_EXT_ROOT}/conio-linux
)

if (WIN32)
    target_link_libraries(TestClient
    )
else()
    target_link_libraries(TestClient
        -ldl
        -lGL
        -lX11
        -lpthread
        -lncurses
    )
endif()


set(websockets_BIN_DIR ${websockets_ROOT}/bin)
if (websockets_BUILD_PLATFORM)
    set(websockets_BIN_DIR ${websockets_BIN_DIR}/${websockets_BUILD_PLATFORM})
endif()
if (CMAKE_BUILD_TYPE)
    set(websockets_BIN_DIR ${websockets_BIN_DIR}/${CMAKE_BUILD_TYPE})
endif()

set_target_properties(TestServer
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${websockets_BIN_DIR}
)

set_target_properties(TestClient
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${websockets_BIN_DIR}
)
