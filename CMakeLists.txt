cmake_minimum_required(VERSION 3.9.2)
project(IZ2 C CXX)

add_subdirectory(dyn_lib)
add_executable(IZ2_dyn main_with_dyn.c)
target_link_libraries(IZ2_dyn PRIVATE dn_lib ${CMAKE_DL_LIBS})

add_subdirectory(static_lib)
add_executable(IZ2_st main_with_static.c)
target_link_libraries(IZ2_st PUBLIC st_lib ${CMAKE_DL_LIBS})


find_package( GTest REQUIRED )
find_package( Threads REQUIRED )

include_directories(${GTEST_INCLUDE_DIRS})

add_executable( tests_gtest tests_gtest.cpp )
target_link_libraries(tests_gtest message_lib user_messages_lib ${GTEST_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})