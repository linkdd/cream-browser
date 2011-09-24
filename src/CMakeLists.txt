cmake_minimum_required (VERSION 2.8)
project (cream-browser)

# Build options

option (DIRECTFB "Build with GTK+-2.0 DirectFB backend" OFF)

option (ENABLE_MOD_DUMMY "Build module 'dummy'" ON)
option (ENABLE_MOD_WEBKIT "Build module 'webkit'" ON)

# Check libraries
find_package (PkgConfig REQUIRED)

pkg_check_modules (GLIB REQUIRED glib-2.0)
include_directories (${GLIB_INCLUDE_DIRS})
link_directories (${GLIB_LIBRARY_DIRS})
set (LIBRARIES ${GLIB_LIBRARIES})

pkg_check_modules (GIO gio-unix-2.0)
include_directories (${GIO_INCLUDE_DIRS})
link_directories (${GIO_LIBRARY_DIRS})
set (LIBRARIES ${LIBRARIES} ${GIO_LIBRARIES})

pkg_check_modules (LUA REQUIRED lua)
include_directories (${LUA_INCLUDE_DIRS})
link_directories (${LUA_LIBRARY_DIRS})
set (LIBRARIES ${LIBRARIES} ${LUA_LIBRARIES})

pkg_check_modules (GTK REQUIRED gtk+-3.0)
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
     "NotebookTabLabel.c"
     "GtkVimSplit.c"
     "Inputbox.c"
     "Statusbar.c"
     "keybinds.c"
     "interface.c"
     "theme.c"
     "lua.c"
     "lua/WebView.c"
     "lua/Notebook.c"
     "lua/clipboard.c"
     "lua/util.c"
     "lua/bit.c"
     "lua/widgets.c"
     "lua/keybinds.c"
     "command.c"
     "scheme.c"
     "modules.c"
     "socket.c"
     "main.c"
     "WebView.h"
     "Notebook.h"
     "NotebookTabLabel.h"
     "GtkVimSplit.h"
     "Inputbox.h"
     "Statusbar.h"
     "keybinds.h"
     "interface.h"
     "theme.h"
     "socket.h"
     "lua.h"
     "scheme.h"
     "structs.h"
     "local.h"
)

# Check modules
if (ENABLE_MOD_DUMMY)
     set (SOURCE ${SOURCE} "modules/dummy.c" "modules/dummy.h")
endif ()

if (ENABLE_MOD_WEBKIT)
     set (SOURCE ${SOURCE} "modules/webkit.c" "modules/webkit.h")

     pkg_check_modules (WEBKIT REQUIRED webkitgtk-3.0)
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

