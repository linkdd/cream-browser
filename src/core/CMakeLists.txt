include_directories(
    ${CREAM_INCLUDES}
    ${TOP_SRCDIR}
    ${TOP_BUILDDIR}
    ${TOP_SRCDIR}/src/core
    ${TOP_SRCDIR}/src/libs
    ${TOP_SRCDIR}/src/libs/config
    ${TOP_SRCDIR}/src/libs/CreamView
    ${TOP_SRCDIR}/src/libs/ftp
    ${TOP_SRCDIR}/src/libs/gopher
    ${TOP_SRCDIR}/src/libs/modules
    )

set(cream_bin_SRCS
    main.c
    marshal.c
    CreamTabbed.c
    interface.c
    callbacks.c
    command.c
    bind.c
    handler.c
    )

set(creamctl_SRCS
    creamctl.c
    )

add_executable(cream_bin ${cream_bin_SRCS})
add_executable(creamctl ${creamctl_SRCS})

# It's _LIBRARIES_DIR for pkg-config checked packages
target_link_libraries(cream_bin
    ${GTK2_LDFLAGS}
    ${GTK2_LDFLAGS_OTHER}
    ${WEBKIT_LDFLAGS}
    ${WEBKIT_LDFLAGS_OTHER}
    ${CONFUSE_LDFLAGS}
    ${CONFUSE_LDFLAGS_OTHER}
    ${GTHREAD_LDFLAGS}
    ${GTHREAD_LDFLAGS_OTHER}
    ${VTE_LDFLAGS}
    ${VTE_LDFLAGS_OTHER}
    ${CREAM_LIBCREAM_LDFLAGS}
    )

target_link_libraries(creamctl
    ${GTK2_LDFLAGS}
    ${GTK2_LDFLAGS_OTHER}
    )

# We can't have two target with same name :)
set_target_properties(cream_bin PROPERTIES OUTPUT_NAME cream)

install(TARGETS cream_bin RUNTIME DESTINATION bin)
install(TARGETS creamctl RUNTIME DESTINATION bin)
