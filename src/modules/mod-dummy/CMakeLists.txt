cmake_minimum_required (VERSION 2.8)
project (module-dummy)

set (BUILD_SHARED_LIBS ON)
set (LIBNAME "creammod-dummy")
set (VERSION "0.1")

# Build options

option (GTK3 "Build with GTK+-3.0" OFF)
option (DIRECTFB "Build with GTK+-2.0 DirectFB backend" OFF)
option (DEBUG "Build in debug mode" ON)



# Check libraries
find_package (PkgConfig REQUIRED)

pkg_check_modules (GLIB REQUIRED glib-2.0)
include_directories (${GLIB_INCLUDE_DIRS})
link_directories (${GLIB_LIBRARY_DIRS})

# check we are not asking GTK+-3.0 with DirectFB.
if (DIRECTFB AND GTK3)
     message (WARNING "GTK+-3.0 doesn't have DirectFB support, using GTK+-2.0 instead.")
     set (GTK3 OFF)
endif ()

if (GTK3)
     pkg_check_modules (GTK REQUIRED gtk+-3.0)
elseif (DIRECTFB)
     pkg_check_modules (GTK REQUIRED gtk+-directfb-2.0)
else ()
     pkg_check_modules (GTK REQUIRED gtk+-2.0)
endif ()

include_directories (${GTK_INCLUDE_DIRS})
link_directories (${GTK_LIBRARY_DIRS})

# Check sources
set (SOURCE
     "../api.c"
     "../scheme.c"
     "mod-dummy.c"
)

include_directories ("${PROJECT_SOURCE_DIR}/../../")
add_library (${LIBNAME} ${SOURCE})
target_link_libraries (${LIBNAME}
     ${GLIB_LIBRARIES}
     ${LUA_LIBRARIES}
     ${GTK_LIBRARIES}
)

install (TARGETS ${LIBNAME} DESTINATION ${CMAKE_INSTALL_LIBDIR})
