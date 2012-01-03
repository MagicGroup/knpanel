project(knpanel)

find_package(KDE4 REQUIRED 4.5)
include(KDE4Defaults)
include(MacroLibrary)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(${QT_INCLUDES} ${KDE4_INCLUDES})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

########################## main lib ##########################

set(knpanel_SRCS
    applet.cpp
    event.cpp
    tooltip.cpp
)

kde4_add_library(knpanel SHARED ${knpanel_SRCS})
target_link_libraries(knpanel ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS})
set_target_properties(knpanel PROPERTIES VERSION 0.0.1 SOVERSION 0)
install(TARGETS knpanel ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES applet.h event.h knpanel_export.h DESTINATION ${INCLUDE_INSTALL_DIR}/knpanel COMPONENT Devel)

########################## main app ##########################

set(knpanel_bin_SRCS
    container.cpp
    knpanel.cpp
    main.cpp
    nlayout.cpp
    plate.cpp
)

kde4_add_executable(knpanel_bin ${knpanel_bin_SRCS})
target_link_libraries(knpanel_bin ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} knpanel krootpixmap)
set_target_properties(knpanel_bin PROPERTIES OUTPUT_NAME knpanel)
install(TARGETS knpanel_bin ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES knpanelapplet.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR})
# install(FILES knpanel.desktop DESTINATION ${SERVICES_INSTALL_DIR})

########################## applets ##########################
add_subdirectory(applets)

