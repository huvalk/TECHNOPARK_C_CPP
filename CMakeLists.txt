cmake_minimum_required(VERSION 3.9.2)
project(IZ2 C CXX)

set(CMAKE_CXX_FLAGS "-g -O0 -Wall --coverage")
set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE ON)

add_subdirectory(dyn_lib)
add_subdirectory(static_lib)

add_executable(IZ2_dyn main_with_dyn.c)
target_link_libraries(IZ2_dyn PRIVATE dn_lib ${CMAKE_DL_LIBS})

add_executable(IZ2_st main_with_static.c)
target_link_libraries(IZ2_st PUBLIC st_lib)


find_package( GTest REQUIRED )
find_package( Threads REQUIRED )

include_directories(${GTEST_INCLUDE_DIRS})
add_executable( tests_gtest tests_gtest.cpp include_cpp/cpp_d_user_messages.h include_cpp/cpp_s_user_messages.h )



target_link_libraries(tests_gtest PRIVATE dn_lib ${CMAKE_DL_LIBS})
target_link_libraries(tests_gtest PUBLIC  PUBLIC st_lib ${GTEST_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})