cmake_minimum_required(VERSION 3.20)

include(GoogleTest)

add_executable(tarragon-test)
target_sources(tarragon-test PRIVATE
    moduletests.cpp
    generatortests.cpp
)

set_target_properties(tarragon-test PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
target_link_libraries(tarragon-test PUBLIC
    libtg
    gmock_main
)

gtest_discover_tests(tarragon-test)
