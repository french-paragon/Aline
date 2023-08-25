list(APPEND LIB_TEST_SRC
            aline_test_global.h
            testeditableitemmanager.h
            testeditableitemmanager.cpp
            editableitemtestmodel.h
            editableitemtestmodel.cpp
            dummyeditableitem.h
            dummyeditableitem.cpp
            referingeditableitem.h
            referingeditableitem.cpp)

add_library(${PROJECT_NAME}_tests ${LIB_TEST_SRC})

target_compile_definitions(${PROJECT_NAME}_tests
  PRIVATE ALINE_TEST_LIBRARY)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../src)

target_link_libraries(${PROJECT_NAME}_tests Qt5::Core)
target_link_libraries(${PROJECT_NAME}_tests Qt5::Test)
target_link_libraries(${PROJECT_NAME}_tests ${PROJECT_NAME})
