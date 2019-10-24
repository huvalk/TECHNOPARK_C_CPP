project(DynLib)
set(SOURCES d_user_messages.c ../common/message.c ../common/mes_sort.c)
set(HEADERS d_user_messages.h ../common/message.h ../common/mes_sort.h)

add_library(dn_lib SHARED ${SOURCES} ${HEADERS})
target_include_directories(dn_lib PUBLIC .)