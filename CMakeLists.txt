cmake_minimum_required(VERSION 3.1)

project(lameenc C)

set(ARCH_FLAGS "")
set(HOST_PLATFORM "")
if (APPLE)
    set(BUILT_FILE "lameenc-1.0.0-cp34-cp34m-macosx_10_6_intel.whl")
    set(HOST "")
    if ("${CMAKE_OSX_ARCHITECTURES}" STREQUAL "x86_64" OR "${CMAKE_OSX_ARCHITECTURES}" STREQUAL "")
      set(ARCH_FLAGS "-arch x86_64")
    endif ()
    if ("${CMAKE_OSX_ARCHITECTURES}" STREQUAL "arm64")
      set(ARCH_FLAGS "${ARCH_FLAGS} -arch arm64")
      set(HOST_PLATFORM "_PYTHON_HOST_PLATFORM=macosx-11.0-arm64")
      set(HOST "--host=aarch64")
    endif ()
    set(LAME_FLAGS "${ARCH_FLAGS} -mmacosx-version-min=10.6")
    set(LAME_CONFIGURE "<SOURCE_DIR>/configure" "CFLAGS=${LAME_FLAGS}" "LDFLAGS=${LAME_FLAGS}"
        "--prefix=<BINARY_DIR>" "${HOST}"
        "--enable-expopt" "--enable-nasm" "--disable-frontend" "--disable-analyzer-hooks"
        "--disable-debug" "--disable-dependency-tracking")
    set(LAME_INSTALL $(MAKE) install)
elseif (WIN32)
    set(BUILT_FILE "lameenc-1.0.0-cp34-cp34m-win32.whl")
    set(LAME_CONFIGURE "${CMAKE_COMMAND}" -E copy
        "<SOURCE_DIR>/configMS.h" "<SOURCE_DIR>/config.h")
    if ( CMAKE_SIZEOF_VOID_P EQUAL 8 )
        string(REPLACE "win32" "win_amd64" BUILT_FILE ${BUILT_FILE})
        set(MACHINE MSVCVER=Win64 MACHINE=/machine:x64)
    endif ()
    set(LAME_MAKE
        nmake -f "<SOURCE_DIR>/Makefile.MSVC"
            comp=msvc asm=no ${MACHINE} libmp3lame-static.lib)
    set(LAME_INSTALL ${CMAKE_COMMAND} -E copy
        "<SOURCE_DIR>/output/libmp3lame-static.lib" "<BINARY_DIR>/lib/libmp3lame.lib")
else ()
    # If you update this, don't forget .github/workflows/build.sh 
    set(BUILT_FILE "lameenc-1.0.0-cp34-cp34m-linux_x86_64.whl")
    set(LAME_FLAGS "-fPIC")
    if (DEFINED ENV{ARCH})
        set(LAME_CONFIGURE "<SOURCE_DIR>/configure" "CFLAGS=${LAME_FLAGS}" "LDFLAGS=${LAME_FLAGS}"
            "--prefix=<BINARY_DIR>"
            "--host=$ENV{ARCH}" "--build=x86_64"
            "--enable-expopt" "--enable-nasm" "--disable-frontend" "--disable-decoder"
            "--disable-analyzer-hooks" "--disable-debug" "--disable-dependency-tracking")
    else ()
        set(LAME_CONFIGURE "<SOURCE_DIR>/configure" "CFLAGS=${LAME_FLAGS}" "LDFLAGS=${LAME_FLAGS}"
            "--prefix=<BINARY_DIR>"
            "--enable-expopt" "--enable-nasm" "--disable-frontend" "--disable-decoder"
            "--disable-analyzer-hooks" "--disable-debug" "--disable-dependency-tracking")
    endif ()
    set(LAME_INSTALL $(MAKE) install)
endif ()

include(ExternalProject)
ExternalProject_Add(lame
    PREFIX "${CMAKE_CURRENT_BINARY_DIR}/lame"
    URL https://sourceforge.net/projects/lame/files/lame/3.100/lame-3.100.tar.gz/download
    URL_HASH SHA256=ddfe36cab873794038ae2c1210557ad34857a4b6bdc515785d1da9e175b1da1e
    DOWNLOAD_NO_PROGRESS 1
    PATCH_COMMAND "${CMAKE_COMMAND}" 
        -D "SOURCE_DIR=<SOURCE_DIR>"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/patch-lame.cmake"
    CONFIGURE_COMMAND ${LAME_CONFIGURE}
    BUILD_COMMAND ${LAME_MAKE}
    INSTALL_COMMAND ${LAME_INSTALL}
    BUILD_IN_SOURCE 1
)

ExternalProject_Get_Property(lame BINARY_DIR)
ExternalProject_Get_Property(lame SOURCE_DIR)

option(PYTHON_VERSIONS "Python versions to build" "3.7;3.8;3.9;3.10;3.11")
# New CMake doesn't take the default above for some reason...
if (${PYTHON_VERSIONS} STREQUAL "OFF")
    set(PYTHON_VERSIONS "3.7;3.8;3.9;3.10;3.11")
endif ()

foreach (Version IN LISTS PYTHON_VERSIONS)
    unset(PYTHON_EXECUTABLE)
    unset(PYTHONINTERP_FOUND)
    if (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 3.18)
        find_package(PythonInterp "${Version}.0" QUIET)
    else ()
        find_package(PythonInterp "${Version}." EXACT QUIET)
    endif ()
    if (PYTHONINTERP_FOUND)
        set(PYTHON${Version}_EXECUTABLE ${PYTHON_EXECUTABLE})
    else ()
        unset(PYTHON_EXECUTABLE)
        find_program(PYTHON${Version}_EXECUTABLE NAMES "python${Version}")
        if (PYTHON_EXECUTABLE)
            set(PYTHONINTERP_FOUND "Found")
        endif ()
    endif ()
    if (NOT ${PYTHON${Version}_EXECUTABLE} STREQUAL "PYTHON${Version}_EXECUTABLE-NOTFOUND")
        # Double check the version with the executable itself
        execute_process(COMMAND "${PYTHON${Version}_EXECUTABLE}" "--version"
            OUTPUT_VARIABLE VERSION_STRING)
        if (NOT "${VERSION_STRING}" MATCHES "${Version}")
            continue()
        endif()

        message("Building Python ${Version} with executable ${PYTHON${Version}_EXECUTABLE}")
        string(REPLACE "." "" VersionShort ${Version})
        string(REPLACE "34" ${VersionShort} THIS_BUILT_FILE ${BUILT_FILE})
        add_custom_command(OUTPUT ${THIS_BUILT_FILE}
            COMMAND ${CMAKE_COMMAND} 
            ARGS
                -E env "ARCHFLAGS=${ARCH_FLAGS}" "${HOST_PLATFORM}"
                ${PYTHON${Version}_EXECUTABLE}
                "${CMAKE_CURRENT_SOURCE_DIR}/setup.py" bdist_wheel
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
        add_custom_target(library${Version} ALL DEPENDS ${THIS_BUILT_FILE})
    endif ()
endforeach ()
