
# Copy a fixed version of libmp3lame.sym over the old one becuase there's
# incorrect symbols that cause the library to not compile.
execute_process(COMMAND "${CMAKE_COMMAND}" -E copy
        "${CMAKE_CURRENT_LIST_DIR}/fixed-libmp3lame.sym"
        "${SOURCE_DIR}/include/libmp3lame.sym")

# Fix the GCC detection so optimisation is applied
# (see https://sourceforge.net/p/lame/bugs/491/)
file(READ "${SOURCE_DIR}/configure" CONFIGURE)
string(REPLACE "0-9*0-9*)" "[0-9]*[0-9]*)" CONFIGURE "${CONFIGURE}")
file(WRITE "${SOURCE_DIR}/configure" "${CONFIGURE}")

# LAME 4.0 no longer ships the mpglib decoder sources, but its MSVC makefile
# still lists them and enables the decoder.
file(READ "${SOURCE_DIR}/Makefile.MSVC" MSVC_MAKEFILE)
string(REPLACE "\r\n" "\n" MSVC_MAKEFILE "${MSVC_MAKEFILE}")
string(FIND "${MSVC_MAKEFILE}" "mpglib_sources =" DECODER_SOURCES_START)
if (DECODER_SOURCES_START EQUAL -1)
    message(FATAL_ERROR "Could not find the decoder source list in Makefile.MSVC")
endif ()
string(SUBSTRING "${MSVC_MAKEFILE}" ${DECODER_SOURCES_START} -1 DECODER_SOURCES_TAIL)
string(FIND "${DECODER_SOURCES_TAIL}" "\n\n" DECODER_SOURCES_END)
if (DECODER_SOURCES_END EQUAL -1)
    message(FATAL_ERROR "Could not find the end of the decoder source list in Makefile.MSVC")
endif ()
string(SUBSTRING "${MSVC_MAKEFILE}" 0 ${DECODER_SOURCES_START} MSVC_MAKEFILE_PREFIX)
math(EXPR DECODER_SOURCES_SUFFIX_START "${DECODER_SOURCES_START} + ${DECODER_SOURCES_END}")
string(SUBSTRING "${MSVC_MAKEFILE}" ${DECODER_SOURCES_SUFFIX_START} -1 MSVC_MAKEFILE_SUFFIX)
set(MSVC_MAKEFILE
    "${MSVC_MAKEFILE_PREFIX}mpglib_sources =${MSVC_MAKEFILE_SUFFIX}")
string(FIND "${MSVC_MAKEFILE}" "/DHAVE_MPGLIB" DECODER_DEFINE)
if (NOT DECODER_DEFINE EQUAL -1)
    string(REPLACE "CPP_OPTS = $(CPP_OPTS) /DHAVE_MPGLIB"
        "CPP_OPTS = $(CPP_OPTS)" MSVC_MAKEFILE "${MSVC_MAKEFILE}")
endif ()
file(WRITE "${SOURCE_DIR}/Makefile.MSVC" "${MSVC_MAKEFILE}")
