cmake_minimum_required(VERSION 3.16)

project(hl-craft C)

# ---- Options ----
option(BUILD_STATIC "Build static libraries and static executable" OFF)

# ---- C standard ----
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

set(CMAKE_C_FLAGS_DEBUG "-O0 -g")
set(CMAKE_C_FLAGS_MINSIZEREL "-Os")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O3")
set(CMAKE_C_FLAGS_RELEASE "-O3 -flto")

# ---- Static / Shared control ----
if(BUILD_STATIC)
    set(INTERNAL_LIBS_TYPE STATIC)
else()
    set(INTERNAL_LIBS_TYPE SHARED)
endif()

# ---- GNU extensions macro ----
add_compile_definitions(_GNU_SOURCE)
link_libraries(m)

# ---- Helper macro to collect sources ----
function(collect_sources out_var dir)
    file(GLOB_RECURSE sources CONFIGURE_DEPENDS
        ${dir}/*.c
    )
    set(${out_var} ${sources} PARENT_SCOPE)
endfunction()

# ---- CORE LIB ----
collect_sources(CORE_SOURCES core/src)

add_library(core ${INTERNAL_LIBS_TYPE} ${CORE_SOURCES})

target_include_directories(core
    PUBLIC
        ${CMAKE_SOURCE_DIR}/inc
    PRIVATE
        ${CMAKE_SOURCE_DIR}/core/inc
)

# ---- CLIENT RENDER LIB ----
collect_sources(RENDER_SOURCES client-render/src)

add_library(client-render ${INTERNAL_LIBS_TYPE} ${RENDER_SOURCES})

target_include_directories(client-render
    PUBLIC
        ${CMAKE_SOURCE_DIR}/inc
    PRIVATE
        ${CMAKE_SOURCE_DIR}/client-render/inc
)

target_link_libraries(client-render
    PUBLIC core
)

# ---- SERVER LIB ----
collect_sources(SERVER_SOURCES server/src)

add_library(server ${INTERNAL_LIBS_TYPE} ${SERVER_SOURCES})

target_include_directories(server
    PUBLIC
        ${CMAKE_SOURCE_DIR}/inc
    PRIVATE
        ${CMAKE_SOURCE_DIR}/server/inc
)

target_link_libraries(server
    PUBLIC core
)

# ---- EXECUTABLE ----
find_package(PkgConfig REQUIRED)

pkg_check_modules(WAYLAND REQUIRED wayland-client)
pkg_check_modules(XCB REQUIRED xkbcommon)
pkg_check_modules(CAIRO REQUIRED cairo)

find_program(WAYLAND_SCANNER_EXECUTABLE wayland-scanner REQUIRED)

function(generate_wayland_protocol protocol_xml out_srcs)
    get_filename_component(proto_name ${protocol_xml} NAME_WE)

    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/wayland-protocols)

    set(header_file ${CMAKE_CURRENT_BINARY_DIR}/wayland-protocols/${proto_name}-client-protocol.h)
    set(source_file ${CMAKE_CURRENT_BINARY_DIR}/wayland-protocols/${proto_name}-protocol.c)

    add_custom_command(
        OUTPUT ${header_file}
        COMMAND ${WAYLAND_SCANNER_EXECUTABLE} client-header
                ${protocol_xml} ${header_file}
        DEPENDS ${protocol_xml}
    )

    add_custom_command(
        OUTPUT ${source_file}
        COMMAND ${WAYLAND_SCANNER_EXECUTABLE} private-code
                ${protocol_xml} ${source_file}
        DEPENDS ${protocol_xml}
    )

    set(${out_srcs}
        ${${out_srcs}}
        ${header_file}
        ${source_file}
        PARENT_SCOPE
    )
endfunction()

file(GLOB PROTOCOL_XMLS
    ${CMAKE_SOURCE_DIR}/hl-craft/protocols/*.xml
)

set(GENERATED_PROTOCOLS)

foreach(proto ${PROTOCOL_XMLS})
    generate_wayland_protocol(${proto} GENERATED_PROTOCOLS)
endforeach()

collect_sources(EXE_SOURCES hl-craft/src)

add_executable(hl-craft ${EXE_SOURCES})

target_sources(hl-craft PRIVATE ${GENERATED_PROTOCOLS})

target_include_directories(hl-craft
    PRIVATE
        ${CMAKE_SOURCE_DIR}/hl-craft/inc
        ${CMAKE_SOURCE_DIR}/inc
        ${WAYLAND_INCLUDE_DIRS}
        ${XCB_INCLUDE_DIRS}
        ${CAIRO_INCLUDE_DIRS}
        ${CMAKE_CURRENT_BINARY_DIR}/wayland-protocols
)

target_link_libraries(hl-craft
    PRIVATE
        core
        client-render
        server
        ${WAYLAND_LIBRARIES}
        ${XCB_LIBRARIES}
        ${CAIRO_LIBRARIES}
)

target_compile_options(hl-craft PRIVATE
    ${WAYLAND_CFLAGS_OTHER}
    ${XCB_CFLAGS_OTHER}
    ${CAIRO_CFLAGS_OTHER}
)
