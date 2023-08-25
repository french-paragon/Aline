cmake_minimum_required(VERSION 2.8)
project(Aline)

include(CMakePackageConfigHelpers)

#some global variables

include(GNUInstallDirs)

set(CMAKE_INSTALL_BINDIR ${CMAKE_INSTALL_BINDIR})
set(CMAKE_INSTALL_CMAKEDIR ${CMAKE_INSTALL_LIBDIR})
set(CMAKE_INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME})
set(CMAKE_INSTALL_INCLUDEDIR ${CMAKE_INSTALL_INCLUDEDIR})

set(ALINE_NAME "Aline")
set(ALINE_EXPORTNAME "AlineTargets")

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
#installing

install(
  EXPORT ${ALINE_EXPORTNAME}
  DESTINATION ${CMAKE_INSTALL_CMAKEDIR}/cmake/${ALINE_NAME}
  NAMESPACE ${ALINE_NAME}::
  FILE AlineTargets.cmake # ensure the name is different than the AlineConfig.cmake file
  )

configure_package_config_file(
  "cmake/Config.cmake.in"
  "${ALINE_NAME}Config.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_CMAKEDIR}/cmake/${ALINE_NAME}
  PATH_VARS
    CMAKE_INSTALL_LIBDIR
  )

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${ALINE_NAME}ConfigVersion.cmake"
  VERSION "0.0"
  COMPATIBILITY AnyNewerVersion
)

install(FILES
	${CMAKE_CURRENT_BINARY_DIR}/${ALINE_NAME}Config.cmake
	${CMAKE_CURRENT_BINARY_DIR}/${ALINE_NAME}ConfigVersion.cmake
	DESTINATION ${CMAKE_INSTALL_CMAKEDIR}/cmake/${ALINE_NAME} )
