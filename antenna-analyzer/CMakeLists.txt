cmake_minimum_required(VERSION 3.18)

project(aapi VERSION 4.3.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED	ON)
set(QT_DEBUG_FIND_PACKAGE ON)
set(QT_NO_PRIVATE_MODULE_WARNING ON)

list(APPEND CMAKE_FIND_ROOT_PATH ${CMAKE_PREFIX_PATH})

find_package(Qt6 REQUIRED COMPONENTS Core Gui GuiPrivate Widgets Qml Quick QuickControls2 Graphs)
qt_standard_project_setup(REQUIRES 6.8)

add_subdirectory(src/app)