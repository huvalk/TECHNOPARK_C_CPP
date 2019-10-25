cmake_minimum_required(VERSION 3.9.2)

project(StaticLib C)
set(SOURCES s_user_messages.c ../common/message.c ../common/mes_sort.c)
set(HEADERS s_user_messages.h ../common/message.h ../common/mes_sort.h)
add_library(st_lib STATIC ${SOURCES} ${HEADERS})
target_include_directories(st_lib PUBLIC .)