cmake_minimum_required(VERSION 3.16)

project(CusSuperV VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
find_package(Qt6 6.2 COMPONENTS
    Quick
    GUi
    Qml
    Widgets
    Core
    LinguistTools
    QuickControls2
    SerialPort
    SerialBus
    Charts
    OpenGL

    REQUIRED)
find_package(glog CONFIG REQUIRED)
find_package(unofficial-breakpad CONFIG REQUIRED)
find_package(folly CONFIG REQUIRED)

#-----project----

file(GLOB
     head
     guarantee/*h
     template/*h
     rms_api/*h
    )

file(GLOB
    src
    guarantee/*cpp
    template/*cpp
     rms_api/*cpp
    )
file(
    GLOB
    asset_src
    asset_src.qrc
    )
# translations files
set(
    ts
    assets/translations/de.ts
    assets/translations/en.ts
    assets/translations/es.ts
    assets/translations/ru.ts
    assets/translations/zh.ts
    )
set(
    source
    ${head}
    ${src}
    ${asset_src}
    ${TS_FILES}

    )

qt_create_translation(
    TS_FILES  ${CMAKE_SOURCE_DIR} ${ts}

    )




qt_add_executable(appCusSuperV
    main.cpp
    ${source}


    )





#qt_add_qml_module(appCusSuperV  //qt6 新加的 构建qml的qmldir autorcc qt_add_source 构建资源等qrc
#    URI CusSuperV
#    VERSION 1.0
#    QML_FILES main.qml
#)




target_compile_definitions(appCusSuperV
    PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(appCusSuperV
    PRIVATE
    Qt6::Quick
    Qt6::Gui
    Qt6::Qml
    Qt6::Widgets
    Qt6::QuickControls2
    Qt6::SerialPort
    Qt6::SerialBus
    Qt6::Charts
    Qt6::OpenGL

    unofficial::breakpad::libbreakpad
    unofficial::breakpad::libbreakpad_client


    glog::glog
    Folly::folly
    Folly::folly_deps
    )
