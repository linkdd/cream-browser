include_directories(
    ${CREAM_INCLUDES}
    ${TOP_SRCDIR}
    ${TOP_SRCDIR}/src/core
    ${TOP_SRCDIR}/src/libs
    ${TOP_SRCDIR}/src/libs/config
    ${TOP_SRCDIR}/src/libs/CreamView
    ${TOP_SRCDIR}/src/libs/curl
    ${TOP_SRCDIR}/src/libs/gopher
    ${TOP_SRCDIR}/src/libs/modules
    )

set(libcream_SRCS
    CreamView/CreamView.c
    CreamView/CreamBackwardForwardList.c
    config/libconfig.c
    curl/CurlClient.c
    gopher/GopherClient.c
    modules/about/AboutModule.c
    modules/ftp/FTPModule.c
    modules/www/WebViewModule.c
    modules/www/Favicon.c
    CreamView/CreamView.h
    CreamView/CreamBackwardForwardList.h
    config/libconfig.h
    curl/CurlClient.h
    gopher/GopherClient.h
    modules/about/AboutModule.h
    modules/ftp/FTPModule.h
    modules/www/WebViewModule.h
    modules/www/Favicon.h
    )

add_library(cream ${libcream_SRCS})

