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
find_package(folly  CONFIG REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)
find_package(Boost         REQUIRED)

#-----project----

file(GLOB
     head
      ${CMAKE_CURRENT_SOURCE_DIR}/guarantee/*h
      ${CMAKE_CURRENT_SOURCE_DIR}/template/*h
      ${CMAKE_CURRENT_SOURCE_DIR}/rms_api/*h
      ${CMAKE_CURRENT_SOURCE_DIR}/rms_api/filter/savitzky_golay/*h
      ${CMAKE_CURRENT_SOURCE_DIR}/IO/*h
    )
#include_directories(
#     guarantee/
#     template/
#     rms_api/
#     rms_api/filter/savitzky_golay/
#     IO/
#    )
file(GLOB
    src
    ${CMAKE_CURRENT_SOURCE_DIR}/guarantee/*cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/template/*cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/rms_api/*cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/rms_api/filter/savitzky_golay/*cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IO/*cpp
    )
set(
    head_src
    ${src}
    ${head}

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
    ${head_src}
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

set(project_libs
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
    Eigen3::Eigen
    Boost::boost
    )

target_link_libraries(appCusSuperV
    PUBLIC
    ${project_libs}

    )

add_subdirectory(test/Catch2)

# These tests can use the Catch2-provided main
add_executable(0101-TestCase test/Catch2/examples/010-TestCase.cpp)
add_executable(0109-AsioTest test/AsioTest.cpp   ${source} )
target_link_libraries(0101-TestCase PRIVATE Catch2::Catch2WithMain)
target_link_libraries(0109-AsioTest PRIVATE Catch2::Catch2WithMain ${project_libs})
## These tests need their own main
#add_executable(custom-main-tests test.cpp test-main.cpp)
#target_link_libraries(custom-main-tests PRIVATE Catch2::Catch2)
