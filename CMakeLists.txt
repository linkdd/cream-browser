cmake_minimum_required (VERSION 2.8)
project (Cream-Browser)

set (VERSION "unstable")
set (LICENSE "MIT")

option (ENABLE_MOD_DUMMY "Build module 'dummy'" ON)

# Build type

if (DEBUG)
     set (CMAKE_BUILD_TYPE "Debug")
     set (HAVE_DEBUG 1)
else ()
     set (CMAKE_BUILD_TYPE "Release")
     set (HAVE_DEBUG 0)
endif ()

set (CMAKE_C_FLAGS_DEBUG "-g -ggdb3 -fno-inline -Wall -Wstrict-prototypes -Wpointer-arith -Wno-sign-compare -O2")
set (CMAKE_C_FLAGS_RELEASE "-Wall -O2")
add_subdirectory (src)
add_subdirectory (po)

# prefix directories

set (CMAKE_INSTALL_EXEC_PREFIX "${CMAKE_INSTALL_PREFIX}")
set (CMAKE_INSTALL_SYSCONFDIR  "${CMAKE_INSTALL_PREFIX}/etc")
set (CMAKE_INSTALL_BINDIR      "${CMAKE_INSTALL_PREFIX}/bin")
set (CMAKE_INSTALL_LIBDIR      "${CMAKE_INSTALL_PREFIX}/lib")
set (CMAKE_INSTALL_DATADIR     "${CMAKE_INSTALL_PREFIX}/share")
set (CMAKE_INSTALL_LOCALEDIR   "${CMAKE_INSTALL_PREFIX}/locale")
set (CMAKE_INSTALL_MANDIR      "${CMAKE_INSTALL_PREFIX}/man")
set (CMAKE_INSTALL_INFODIR     "${CMAKE_INSTALL_PREFIX}/info")

# configure header

configure_file (
     "${PROJECT_SOURCE_DIR}/cream-browser_build.h.in"
     "${PROJECT_BINARY_DIR}/cream-browser_build.h"
)

# Build modules
foreach (mod "dummy")
     string (TOUPPER ${mod} modname)

     if (ENABLE_MOD_${modname})
          message ("-- Enabling module ${mod}")
          add_subdirectory (src/modules/mod-${mod})
     endif ()
endforeach (mod)

