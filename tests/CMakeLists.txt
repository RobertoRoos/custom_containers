enable_testing()

add_executable(tests
        main_test.cpp
        test_buffer.cpp
        test_fifo.cpp)

target_link_libraries(tests
        GTest::gtest_main
        custom_containers)

include(GoogleTest)
gtest_discover_tests(tests)
