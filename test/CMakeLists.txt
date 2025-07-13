include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(math_matrix_tests MathMatrixTest.cpp MathVectorTest.cpp)

target_link_libraries(math_matrix_tests PRIVATE matrix_library gtest_main)
include(GoogleTest)
gtest_discover_tests(math_matrix_tests)