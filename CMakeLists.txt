cmake_minimum_required(VERSION 3.16)

project(CusSuperV VERSION 0.1 LANGUAGES CXX)

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


    REQUIRED)
find_package(glog CONFIG REQUIRED)
find_package(unofficial-breakpad CONFIG REQUIRED)
find_package(folly CONFIG REQUIRED)

#-----project----
include_directories(
     guarantee/
    template/
    )
file(GLOB src
    guarantee/*cpp
    template/*cpp
    )
file(
    GLOB
    asset_src
    asset_src.qrc

    )
# translations files
file(GLOB ts
  translator/CusSuperV_zh_CN.ts
  translator/CusSuperV_en_AS.ts
    )


qt_create_translation(
    TS_FILES  ${CMAKE_SOURCE_DIR} ${ts}

  )

set(
source
${src}
${asset_src}
${ts}
${TS_FILES}

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

    unofficial::breakpad::libbreakpad
    unofficial::breakpad::libbreakpad_client


    glog::glog
    Folly::folly
    Folly::folly_deps
    )
