cmake_minimum_required(VERSION 2.8)

project(lameenc C)

if (APPLE)
    set(BUILT_FILE "lameenc-1.0.0-cp34-cp34m-macosx_10_6_intel.whl")
    set(LAME_FLAGS "-arch i386 -arch x86_64 -mmacosx-version-min=10.6")
endif (APPLE)

if (WIN32)
    set(BUILT_FILE "lameenc-1.0.0-cp34-cp34m-win32.whl")
endif (WIN32)

if (UNIX)
    set(BUILT_FILE "lameenc-1.0.0-cp34-cp34m-manylinux.whl")
endif (UNIX)

include(ExternalProject)
ExternalProject_Add(lame
    PREFIX "${CMAKE_CURRENT_BINARY_DIR}/lame"
    URL https://sourceforge.net/projects/lame/files/lame/3.100/lame-3.100.tar.gz/download
    URL_HASH SHA256=ddfe36cab873794038ae2c1210557ad34857a4b6bdc515785d1da9e175b1da1e
    PATCH_COMMAND "${CMAKE_COMMAND}" -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/fixed-libmp3lame.sym"
        "<SOURCE_DIR>/include/libmp3lame.sym"
    CONFIGURE_COMMAND "<SOURCE_DIR>/configure" "--prefix=<BINARY_DIR>"
    BUILD_COMMAND
        $(MAKE) "CFLAGS=${LAME_FLAGS}" "LDFLAGS=${LAME_FLAGS}"
)
ExternalProject_Get_Property(lame BINARY_DIR)
ExternalProject_Get_Property(lame SOURCE_DIR)

find_package(PythonInterp 3.4 REQUIRED)
add_custom_command(OUTPUT ${BUILT_FILE}
    COMMAND ${PYTHON_EXECUTABLE}
    ARGS "${CMAKE_CURRENT_SOURCE_DIR}/setup.py" bdist_wheel
        -b "${CMAKE_CURRENT_BINARY_DIR}/build_tmp"
        -d "${CMAKE_CURRENT_BINARY_DIR}"
        "--libdir=${BINARY_DIR}/lib"
        "--incdir=${SOURCE_DIR}/include"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    DEPENDS
        lame
        "${CMAKE_CURRENT_SOURCE_DIR}/setup.py"
        "${CMAKE_CURRENT_SOURCE_DIR}/lameenc.c"
)
add_custom_target(library ALL DEPENDS ${BUILT_FILE}) 
