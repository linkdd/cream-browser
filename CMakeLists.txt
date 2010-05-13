project(cream-browser)
set(VERSION 201005)
set(PROJECT_NAME "cream-browser")

cmake_minimum_required(VERSION 2.6)
include(FindPkgConfig)

CONFIGURE_FILE(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE
    @ONLY
    )

ADD_CUSTOM_TARGET(uninstall
    "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    )

# Dependencies

####
# CURL
# CURL_FOUND	CURL_INCLUDE_DIRS	CURL_LIBRARIES
find_package(CURL)
if(CURL_FOUND)
else(CURL_FOUND)
    message(FATAL_ERROR "Could not find Curl")
endif(CURL_FOUND)

# Need test SSL : curl-config --feature | grep SSL`" == "SSL"
# to define HAVE_CURL_SSL
set(CURL_HAVE_SSL "#define HAVE_CURL_SSL 1")

# For pkg_check
# _LIBRARIES	_LIBRARY_DIRS	_LDFLAGS	_LDFLAGS_OTHER
# _INCLUDE_DIRS	_CFLAGS	_CFLAGS_OTHER

####
# GTK2
pkg_check_modules(GTK2 REQUIRED gtk+-2.0)
if(GTK2_FOUND)
else(GTK2_FOUND)
    message(FATAL_ERROR "Could not find GTK2")
endif(GTK2_FOUND)

####
# Webkit-GTK
pkg_check_modules(WEBKIT REQUIRED webkit-1.0>=1.1.15)
if(WEBKIT_FOUND)
else(WEBKIT_FOUND)
    message(FATAL_ERROR "Could not find Webkit-1.0 >= 1.1.15")
endif(WEBKIT_FOUND)

####
# GNet
pkg_check_modules(GNET REQUIRED gnet-2.0)
if(GNET_FOUND)
else(GNET_FOUND)
    message(FATAL_ERROR "Could not find gnet-2.0")
endif(GNET_FOUND)

####
# Confuse
pkg_check_modules(CONFUSE REQUIRED libconfuse)
if(CONFUSE_FOUND)
else(CONFUSE_FOUND)
    message(FATAL_ERROR, "Could not find libconfuse")
endif(CONFUSE_FOUND)

####
# VTE
pkg_check_modules(VTE vte)
if(VTE_FOUND)
     set (CREAM_IN_HAVE_VTE "#define HAVE_VTE 1")
endif(VTE_FOUND)

# Includes
set(CREAM_INCLUDES
    ${CURL_INCLUDE_DIRS}
    ${GTK2_INCLUDE_DIRS}
    ${WEBKIT_INCLUDE_DIRS}
    ${GNET_INCLUDE_DIRS}
    ${CONFUSE_INCLUDE_DIRS}
    ${VTE_INCLUDE_DIRS}
    )

# Some things
# ... used in others CMakeFiles
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(TOP_SRCDIR ${SOURCE_DIR})
set(TOP_BUILDDIR ${BUILD_DIR})
set(CREAM_LIBCREAM_LDFLAGS "-L${TOP_BUILDDIR}/src/libs -lcream")

# ... used in .in files
set(CREAM_IN_HAVE_CURL_SSL ${CURL_HAVE_SSL})
set(CREAM_IN_VERSION ${VERSION})
set(CREAM_IN_PACKAGE ${PROJECT_NAME})
set(CREAM_IN_PACKAGE_NAME ${CREAM_PACKAGE})

# PREFIX
if(NOT DEFINED PREFIX)
    if(CMAKE_SYSTEM_NAME MATCHES FreeBSD)
        set(PREFIX "/usr/local")
    elseif(CMAKE_SYSTEM_NAME MATCHES NetBSD)
        set(PREFIX "/usr/pkg")
    elseif(CMAKE_SYSTEM_NAME MATCHES Linux)
        set(PREFIX "/usr")
    else()
        message(FATAL_ERROR "Please add your OS in the CMakeLists.txt for PREFIX then send it to authors")
    endif(CMAKE_SYSTEM_NAME MATCHES FreeBSD)
endif(NOT DEFINED PREFIX)

if(NOT DEFINED SYSCONFDIR)
    set(SYSCONFDIR ${PREFIX}/etc)
endif(NOT DEFINED SYSCONFDIR)

set(CMAKE_INSTALL_PREFIX ${PREFIX})
set(CMAKE_INSTALL_RPATH "${PREFIX}/lib")

set(CFLAGS "-Wall -g -ggdb3 -fno-inline")
set(CMAKE_C_FLAGS ${CFLAGS})
set(DEFAULT_LDFLAGS "${PREFIX}/lib")

# Configure .in files
set(dot_in_files
    config.h.in
    )

macro(a_configure_file file)
    string(REGEX REPLACE ".in\$" "" outfile ${file})
    message(STATUS "Configuring ${outfile}")
    configure_file(${SOURCE_DIR}/${file}
		   ${BUILD_DIR}/${outfile}
		   ESCAPE_QUOTE
		   @ONLY)
endmacro()

foreach(file ${dot_in_files})
    a_configure_file(${file})
endforeach()

add_definitions("-DPREFIX=\"${PREFIX}\"")

# libcream
add_subdirectory( src/libs )
# Core (creamctl and cream-browser) (cream is linked to libcream!)
add_subdirectory( src/core )
# Docs
add_subdirectory( doc )

message("-- Build infos")
message("--  System: ${CMAKE_SYSTEM_NAME}")
message("--  Prefix: ${PREFIX}")
message("--  Sysconfdir: ${SYSCONFDIR}")
message("--  Project version: ${VERSION}")

set(CREAM_XDG_CONFIG "${TOP_SRCDIR}/config.sample")
# Install things
# Install targets are in src/core/CMakeLists.txt
install(FILES ${CREAM_XDG_CONFIG} DESTINATION "${SYSCONFDIR}/xdg/cream-browser" RENAME config)
