cmake_minimum_required(VERSION 3.8)
project(IZ_1)

set (CMAKE_C_FLAGS "-std=gnu99 ${CMAKE_C_FLAGS}")

set(LIB_SOURCE_FILES
        statistic.c)

add_library(study_std_lib STATIC ${LIB_SOURCE_FILES})

add_executable(prog contracts.c)
target_link_libraries(prog study_std_lib)


find_package( GTest REQUIRED )
find_package( Threads REQUIRED )

include_directories(${GTEST_INCLUDE_DIRS})

add_executable( tests_gtest tests_gtest.cpp)
target_link_libraries(tests_gtest study_std_lib ${GTEST_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
