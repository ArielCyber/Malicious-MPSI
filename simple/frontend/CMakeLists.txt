
#project(frontend)


file(GLOB_RECURSE SRCS *.cpp)

include_directories(${CMAKE_SOURCE_DIR})


add_executable(tests tests.cpp)

target_link_libraries(tests gtest_main libOTe_Tests tests_cryptoTools ENCRYPTO_utils::encrypto_utils project_)

add_executable(main main.cpp)

target_link_libraries(main libOTe_Tests tests_cryptoTools ENCRYPTO_utils::encrypto_utils project_)
