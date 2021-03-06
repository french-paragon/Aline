cmake_minimum_required(VERSION 2.8.9)
project(Aline)

#cross compilation
unset(MXE_CROSS_COMPILE)
if (CMAKE_CROSS_COMPILING AND CMAKE_SYSTEM_NAME STREQUAL Windows)
    set(MXE_CROSS_COMPILE 1)
    add_definitions(-DCROSS_COMPILE_MXE)
endif (CMAKE_CROSS_COMPILING AND CMAKE_SYSTEM_NAME STREQUAL Windows)

#set (CMAKE_CXX_STANDARD 11) # we use C++ version 11
set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")

#Qt

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Test REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5QuickWidgets REQUIRED)
find_package(Qt5Qml REQUIRED)

ADD_DEFINITIONS(-DQT_NO_KEYWORDS)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTOUIC ON)
SET(CMAKE_AUTORCC ON)

add_subdirectory(src)
add_subdirectory(tests)
