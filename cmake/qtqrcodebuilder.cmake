set(QT_QRCODEBUILDER_BASEDIR ${CMAKE_SOURCE_DIR}/3rdparty/QtQrCodeBuilder)

include_directories(${QT_QRCODEBUILDER_BASEDIR})

set(QT_QRCODEBUILDER_SOURCES
    ${QT_QRCODEBUILDER_BASEDIR}/qrcodegencpp/qrcodegen.hpp
    ${QT_QRCODEBUILDER_BASEDIR}/qrcodegencpp/qrcodegen.cpp
    ${QT_QRCODEBUILDER_BASEDIR}/QtQrCodeBuilder.hpp
    ${QT_QRCODEBUILDER_BASEDIR}/QtQrCodeBuilder.cpp
    )

find_package(${QV_QT_LIBNAME} COMPONENTS Gui REQUIRED)

set(QV2RAY_QTQRCODEBUILDER_LIBRARY qv2ray_qtqrcodebuilder)
add_library(qv2ray_qtqrcodebuilder STATIC ${QT_QRCODEBUILDER_SOURCES})
target_link_libraries(${QV2RAY_QTQRCODEBUILDER_LIBRARY} ${QV_QT_LIBNAME}::Gui)