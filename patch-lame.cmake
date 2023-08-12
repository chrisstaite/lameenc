
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
