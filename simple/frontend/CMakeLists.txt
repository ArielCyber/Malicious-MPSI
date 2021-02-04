
#project(frontend)


file(GLOB_RECURSE SRCS *.cpp)

include_directories(${CMAKE_SOURCE_DIR})


add_executable(main main.cpp)

target_link_libraries(main libOTe_Tests tests_cryptoTools ENCRYPTO_utils::encrypto_utils project_)
