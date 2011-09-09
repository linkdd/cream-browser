cmake_minimum_required (VERSION 2.8)
project (Cream-Browser)

set (VERSION "unstable")
set (LICENSE "MIT")

# prefix directories

set (CMAKE_INSTALL_EXEC_PREFIX "${CMAKE_INSTALL_PREFIX}"         CACHE STRING "Iinstallation base")
set (CMAKE_INSTALL_SYSCONFDIR  "${CMAKE_INSTALL_PREFIX}/etc"     CACHE STRING "System configuration files")
set (CMAKE_INSTALL_BINDIR      "${CMAKE_INSTALL_PREFIX}/bin"     CACHE STRING "Executables (for common users)")
set (CMAKE_INSTALL_LIBDIR      "${CMAKE_INSTALL_PREFIX}/lib"     CACHE STRING "System libraries")
set (CMAKE_INSTALL_DATADIR     "${CMAKE_INSTALL_PREFIX}/share"   CACHE STRING "Data files (for programs)")
set (CMAKE_INSTALL_LOCALEDIR   "${CMAKE_INSTALL_PREFIX}/locale"  CACHE STRING "Multi-language support locales")
set (CMAKE_INSTALL_MANDIR      "${CMAKE_INSTALL_PREFIX}/man"     CACHE STRING "Manual page documentation")
set (CMAKE_INSTALL_INFODIR     "${CMAKE_INSTALL_PREFIX}/info"    CACHE STRING "Texinfo documentation")

# Build type

if (DEBUG)
     set (CMAKE_BUILD_TYPE "Debug")
     set (HAVE_DEBUG 1)
else ()
     set (CMAKE_BUILD_TYPE "Release")
     set (HAVE_DEBUG 0)
endif ()

set (CMAKE_C_FLAGS_DEBUG "-D_GNU_SOURCE -g -ggdb3 -fno-inline -Wall -Wstrict-prototypes -Wpointer-arith -Wno-sign-compare -O2")
set (CMAKE_C_FLAGS_RELEASE "-Wall -O2 -D_GNU_SOURCE")
add_subdirectory (src)
add_subdirectory (po)

# configure header

if (ENABLE_MOD_DUMMY)
     set (HAVE_MOD_DUMMY 1)
endif ()

if (ENABLE_MOD_WEBKIT)
     set (HAVE_MOD_WEBKIT 1)
endif ()

configure_file (
     "${PROJECT_SOURCE_DIR}/cream-browser_build.h.in"
     "${PROJECT_BINARY_DIR}/cream-browser_build.h"
)

# Documentation
configure_file (
     "${CMAKE_SOURCE_DIR}/Doxyfile.in"
     "${CMAKE_BINARY_DIR}/Doxyfile"
)

add_custom_target (luadoc
     COMMENT "Generating lua documentation"
     COMMAND mkdir -p ${CMAKE_BINARY_DIR}/docs/lua
     COMMAND luadoc ${CMAKE_SOURCE_DIR}/src/lua/doc/*.lua -d ${CMAKE_BINARY_DIR}/docs/lua --nofiles
)

add_custom_target (doxygen
     COMMENT "Generating doxygen documentation"
     COMMAND mkdir -p ${CMAKE_BINARY_DIR}/docs/cream-browser
     COMMAND doxygen ${CMAKE_BINARY_DIR}/Doxyfile
)

add_custom_target (doc
     DEPENDS "luadoc" "doxygen"
)

# Archive
add_custom_target (dist
     COMMENT "Creating tar archive"
     COMMAND cd ${CMAKE_SOURCE_DIR} && git archive --format=tar --prefix=${CMAKE_PROJECT_NAME}-`git rev-parse --short HEAD`/ HEAD > ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}-`git rev-parse --short HEAD`.tar
)

# Uninstall target
configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY
)

add_custom_target (uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
)
