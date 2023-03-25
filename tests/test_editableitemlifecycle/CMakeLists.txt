set(TEST_NAME "testeditableitemlifecycle")
add_executable(${TEST_NAME} testeditableitemlifecycle.cpp)

target_link_libraries(${TEST_NAME} Qt5::Core)
target_link_libraries(${TEST_NAME} Qt5::Test)

target_link_libraries(${TEST_NAME} ${PROJECT_NAME})

target_link_libraries(${TEST_NAME} Aline_tests)

add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
