cmake_minimum_required (VERSION 2.8)
project (locale)

set (DOMAINS
     "fr"
)

foreach (DOMAIN ${DOMAINS})
     add_custom_command (
          OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${DOMAIN}.mo"
          COMMAND msgfmt -o ${CMAKE_CURRENT_BINARY_DIR}/${DOMAIN}.mo ${CMAKE_CURRENT_SOURCE_DIR}/${DOMAIN}.po
          DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${DOMAIN}.po"
     )

     set (SOURCE ${SOURCE} "${DOMAIN}.mo")

     install (FILES ${CMAKE_CURRENT_BINARY_DIR}/${DOMAIN}.mo DESTINATION ${CMAKE_INSTALL_LOCALEDIR}/${DOMAIN}/LC_MESSAGES/ RENAME cream-browser.mo)
endforeach ()

add_custom_target (locale ALL DEPENDS ${SOURCE})
