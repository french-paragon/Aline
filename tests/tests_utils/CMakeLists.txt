list(APPEND LIB_TEST_SRC
            aline_test_global.h
            testeditableitemmanager.cpp
            editableitemtestmodel.cpp)

add_library(${PROJECT_NAME}_tests ${LIB_TEST_SRC})

target_compile_definitions(${PROJECT_NAME}_tests
  PRIVATE ALINE_TEST_LIBRARY)

link_directories(../../src)
include_directories(../../src)

target_link_libraries(${PROJECT_NAME}_tests Qt5::Core)
target_link_libraries(${PROJECT_NAME}_tests Qt5::Test)
target_link_libraries(${PROJECT_NAME}_tests ${PROJECT_NAME})
