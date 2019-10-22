project(DynLib)
set(SOURCES d_user_messages.c ../mes/message.c)
set(HEADERS d_user_messages.h ../mes/message.h)

add_library(dn_lib SHARED ${SOURCES} ${HEADERS})
target_include_directories(dn_lib PUBLIC .)