cmake_minimum_required (VERSION 2.8)
project (cream-browser)

# Build options

option (GTK3 "Build with GTK+-3.0" OFF)
option (DIRECTFB "Build with GTK+-2.0 DirectFB backend" OFF)
option (DEBUG "Build in debug mode" ON)

option (ENABLE_MOD_DUMMY "Build module 'dummy'" ON)
option (ENABLE_MOD_WEBKIT "Build module 'webkit'" ON)

# Check libraries
find_package (PkgConfig REQUIRED)

pkg_check_modules (GLIB REQUIRED glib-2.0)
include_directories (${GLIB_INCLUDE_DIRS})
link_directories (${GLIB_LIBRARY_DIRS})
set (LIBRARIES ${GLIB_LIBRARIES})

pkg_check_modules (LUA REQUIRED lua)
include_directories (${LUA_INCLUDE_DIRS})
link_directories (${LUA_LIBRARY_DIRS})
set (LIBRARIES ${LIBRARIES} ${LUA_LIBRARIES})

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
set (LIBRARIES ${LIBRARIES} ${GTK_LIBRARIES})

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
     "WebView.c"
     "Notebook.c"
     "GtkVimSplit.c"
     "Inputbox.c"
     "interface.c"
     "lua.c"
     "lua/WebView.c"
     "lua/clipboard.c"
     "lua/util.c"
     "lua/theme.c"
     "command.c"
     "scheme.c"
     "modules.c"
     "socket.c"
     "main.c"
     "WebView.h"
     "Notebook.h"
     "GtkVimSplit.h"
     "Inputbox.h"
     "interface.h"
     "lua.h"
     "lua/theme.h"
     "scheme.h"
     "structs.h"
     "local.h"
)

if (ENABLE_MOD_DUMMY)
     set (SOURCE ${SOURCE} "modules/dummy.c" "modules/dummy.h")
endif ()

if (ENABLE_MOD_WEBKIT)
     set (SOURCE ${SOURCE} "modules/webkit.c" "modules/webkit.h")

     if (GTK3)
          pkg_check_modules (WEBKIT REQUIRED webkitgtk-3.0)
     else ()
          pkg_check_modules (WEBKIT REQUIRED webkit-1.0)
     endif ()

     include_directories (${WEBKIT_INCLUDE_DIRS})
     link_directories (${WEBKIT_LIBRARY_DIRS})
     set (LIBRARIES ${LIBRARIES} ${WEBKIT_LIBRARIES})
endif ()

include_directories ("${PROJECT_BINARY_DIR}/..")
include_directories ("${PROJECT_BINARY_DIR}")
add_executable (cream-browser ${SOURCE})
target_link_libraries (cream-browser ${LIBRARIES})

install (TARGETS cream-browser DESTINATION ${CMAKE_INSTALL_BINDIR})
file (GLOB files "${PROJECT_SOURCE_DIR}/lua/lib/cream/*.lua")
install (FILES ${files} DESTINATION ${CMAKE_INSTALL_DATADIR}/cream-browser/lib/cream)

