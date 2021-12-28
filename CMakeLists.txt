cmake_minimum_required(VERSION 3.16)

project(CusSuperV VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
find_package(Qt6 6.2 COMPONENTS
    Quick
    GUi
    Qml
    Widgets
    Core


    REQUIRED)
find_package(glog CONFIG REQUIRED)

#-----project----
include_directories(
     Guarantee/
    Template/
    )
file(GLOB src
    Guarantee/*cpp
    Template/*cpp
 
    )
set(
source
${src}
	)
qt_add_executable(appCusSuperV
    main.cpp
    ${source}
)

qt_add_qml_module(appCusSuperV
    URI CusSuperV
    VERSION 1.0
    QML_FILES main.qml 
)

target_compile_definitions(appCusSuperV
    PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(appCusSuperV
    PRIVATE
    Qt6::Quick
    Qt6::Gui
    Qt6::Qml
    Qt6::Widgets


    glog::glog
    )
