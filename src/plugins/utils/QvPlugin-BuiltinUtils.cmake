set(UTILS_PLUGIN_TARGET QvPlugin-BuiltinUtils)

set(QVPLUGIN_INTERFACE_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/src/plugin-interface)

include(${CMAKE_SOURCE_DIR}/src/plugin-interface/QvPluginInterface.cmake)
include(${CMAKE_SOURCE_DIR}/src/plugin-interface/QvGUIPluginInterface.cmake)

set(BUILTIN_SUBSCRIPTION_PLUGIN_SOURCES
    ${QVPLUGIN_INTERFACE_HEADERS}
    ${QVGUIPLUGIN_INTERFACE_HEADERS}
    ${CMAKE_CURRENT_LIST_DIR}/BuiltinUtils.cpp
    ${CMAKE_CURRENT_LIST_DIR}/BuiltinUtils.hpp
    ${CMAKE_CURRENT_LIST_DIR}/core/EventHandler.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/EventHandler.hpp
    ${CMAKE_CURRENT_LIST_DIR}/core/GUIInterface.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/GUIInterface.hpp
    ${CMAKE_CURRENT_LIST_DIR}/core/MainWindowWidget.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/MainWindowWidget.hpp
    ${CMAKE_CURRENT_LIST_DIR}/core/MainWindowWidget.ui
    )
list(APPEND PLUGIN_TRANSLATION_SOURCES ${BUILTIN_PROTOCOL_PLUGIN_SOURCES})

add_library(${UTILS_PLUGIN_TARGET} MODULE
    ${CMAKE_CURRENT_LIST_DIR}/resx.qrc
    ${BUILTIN_SUBSCRIPTION_PLUGIN_SOURCES}
    )

target_include_directories(${UTILS_PLUGIN_TARGET} PRIVATE ${QVPLUGIN_INTERFACE_INCLUDE_DIR})
target_include_directories(${UTILS_PLUGIN_TARGET} PRIVATE ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(${UTILS_PLUGIN_TARGET} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../common)

if(UNIX AND NOT APPLE AND NOT WIN32 AND NOT ANDROID)
    install(TARGETS ${UTILS_PLUGIN_TARGET} LIBRARY DESTINATION share/qv2ray/plugins)
elseif(WIN32)
    install(TARGETS ${UTILS_PLUGIN_TARGET} LIBRARY DESTINATION plugins)
else()
    message(FATAL_ERROR "?")
endif()

target_link_libraries(${UTILS_PLUGIN_TARGET}
    ${QV_QT_LIBNAME}::Core
    ${QV_QT_LIBNAME}::Gui
    ${QV_QT_LIBNAME}::Widgets)
