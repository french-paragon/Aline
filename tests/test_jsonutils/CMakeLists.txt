set(TEST_NAME "testjsonutils")
add_executable(${TEST_NAME} testjsonutils.cpp)

target_link_libraries(${TEST_NAME} Qt5::Core)
target_link_libraries(${TEST_NAME} Qt5::Test)

target_link_libraries(${TEST_NAME} ${PROJECT_NAME})

add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
