cmake_minimum_required (VERSION 2.8)
project (cream-browser)

# Build options

option (GTK3 "Build with GTK+-3.0" OFF)
option (DIRECTFB "Build with GTK+-2.0 DirectFB backend" OFF)
option (DEBUG "Build in debug mode" ON)

# Check libraries
find_package (PkgConfig REQUIRED)

pkg_check_modules (GLIB REQUIRED glib-2.0)
include_directories (${GLIB_INCLUDE_DIRS})
link_directories (${GLIB_LIBRARY_DIRS})

pkg_check_modules (LUA REQUIRED lua)
include_directories (${LUA_INCLUDE_DIRS})
link_directories (${LUA_LIBRARY_DIRS})

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

# Generate marshal.h/.c
add_custom_command (
     OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/marshal.h"
     COMMAND glib-genmarshal --prefix=cream_marshal --header ${CMAKE_CURRENT_SOURCE_DIR}/marshal.txt > ${CMAKE_CURRENT_BINARY_DIR}/marshal.h
     DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/marshal.txt"
)

add_custom_command (
     OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/marshal.c"
     COMMAND glib-genmarshal --prefix=cream_marshal --body ${CMAKE_CURRENT_SOURCE_DIR}/marshal.txt > ${CMAKE_CURRENT_BINARY_DIR}/marshal.c
     DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/marshal.txt"
)

# Check sources
set (SOURCE
     "${CMAKE_CURRENT_BINARY_DIR}/marshal.c"
     "${CMAKE_CURRENT_BINARY_DIR}/marshal.h"
     "WebView.c"          "WebView.h"   "lua/WebView.c"
     "Notebook.c"         "Notebook.h"
     "GtkVimSplit.c"      "GtkVimSplit.h"
     "modules.c"          "modules.h"        "lua/modules.c"
     "lua.c"              "lua.h"
     "lua/clipboard.c"
     "lua/util.c"
     "lua/theme.c"        "lua/theme.h"
     "interface.c"        "interface.h"
     "modules/scheme.c"   "modules/scheme.h"
     "socket.c"
     "main.c"             "structs.h" "local.h"
)

include_directories ("${PROJECT_BINARY_DIR}/..")
include_directories ("${PROJECT_BINARY_DIR}")
add_executable (cream-browser ${SOURCE})
target_link_libraries (cream-browser
     ${GLIB_LIBRARIES}
     ${LUA_LIBRARIES}
     ${GTK_LIBRARIES}
)

install (TARGETS cream-browser DESTINATION ${CMAKE_INSTALL_BINDIR})
file (GLOB files "${PROJECT_SOURCE_DIR}/lua/lib/cream/*.lua")
install (FILES ${files} DESTINATION ${CMAKE_INSTALL_DATADIR}/cream-browser/lib/cream)

